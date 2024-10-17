/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NBOwnTLDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
///
// A traffic light logics which must be computed (only nodes/edges are given)
/****************************************************************************/
#include <config.h>

#include <vector>
#include <cassert>
#include <iterator>
#include "NBTrafficLightDefinition.h"
#include "NBNode.h"
#include "NBOwnTLDef.h"
#include "NBTrafficLightLogic.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>

#define HEIGH_WEIGHT 2
#define LOW_WEIGHT .5;

#define MIN_GREEN_TIME 5

//#define DEBUG_STREAM_ORDERING
//#define DEBUG_PHASES
//#define DEBUG_CONTRELATION
#define DEBUGID  "C"
#define DEBUGCOND (getID() == DEBUGID)
#define DEBUGCOND2(obj) (obj->getID() == DEBUGID)
//#define DEBUGEDGE(edge) (edge->getID() == "23209153#1" || edge->getID() == "319583927#0")
//#define DEBUGCOND (true)
#define DEBUGEDGE(edge) (true)

// ===========================================================================
// static members
// ===========================================================================
const double NBOwnTLDef::MIN_SPEED_CROSSING_TIME(25 / 3.6);


// ===========================================================================
// member method definitions
// ===========================================================================
NBOwnTLDef::NBOwnTLDef(const std::string& id,
                       const std::vector<NBNode*>& junctions, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, junctions, DefaultProgramID, offset, type),
    myHaveSinglePhase(false),
    myLayout(TrafficLightLayout::DEFAULT) {
}


NBOwnTLDef::NBOwnTLDef(const std::string& id, NBNode* junction, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, junction, DefaultProgramID, offset, type),
    myHaveSinglePhase(false),
    myLayout(TrafficLightLayout::DEFAULT) {
}


NBOwnTLDef::NBOwnTLDef(const std::string& id, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, DefaultProgramID, offset, type),
    myHaveSinglePhase(false),
    myLayout(TrafficLightLayout::DEFAULT) {
}


NBOwnTLDef::~NBOwnTLDef() {}


int
NBOwnTLDef::getToPrio(const NBEdge* const e) {
    return e->getJunctionPriority(e->getToNode());
}


double
NBOwnTLDef::getDirectionalWeight(LinkDirection dir) {
    switch (dir) {
        case LinkDirection::STRAIGHT:
        case LinkDirection::PARTLEFT:
        case LinkDirection::PARTRIGHT:
            return HEIGH_WEIGHT;
        case LinkDirection::LEFT:
        case LinkDirection::RIGHT:
            return LOW_WEIGHT;
        default:
            break;
    }
    return 0;
}

double
NBOwnTLDef::computeUnblockedWeightedStreamNumber(const NBEdge* const e1, const NBEdge* const e2) {
    double val = 0;
    for (int e1l = 0; e1l < e1->getNumLanes(); e1l++) {
        std::vector<NBEdge::Connection> approached1 = e1->getConnectionsFromLane(e1l);
        for (int e2l = 0; e2l < e2->getNumLanes(); e2l++) {
            std::vector<NBEdge::Connection> approached2 = e2->getConnectionsFromLane(e2l);
            for (std::vector<NBEdge::Connection>::iterator e1c = approached1.begin(); e1c != approached1.end(); ++e1c) {
                if (e1->getTurnDestination() == (*e1c).toEdge) {
                    continue;
                }
                for (std::vector<NBEdge::Connection>::iterator e2c = approached2.begin(); e2c != approached2.end(); ++e2c) {
                    if (e2->getTurnDestination() == (*e2c).toEdge) {
                        continue;
                    }
                    const double sign = (forbids(e1, (*e1c).toEdge, e2, (*e2c).toEdge, true)
                                         || forbids(e2, (*e2c).toEdge, e1, (*e1c).toEdge, true)) ? -1 : 1;
                    double w1;
                    double w2;
                    const int prio1 = e1->getJunctionPriority(e1->getToNode());
                    const int prio2 = e2->getJunctionPriority(e2->getToNode());
                    if (prio1 == prio2) {
                        w1 = getDirectionalWeight(e1->getToNode()->getDirection(e1, (*e1c).toEdge));
                        w2 = getDirectionalWeight(e2->getToNode()->getDirection(e2, (*e2c).toEdge));
                    } else {
                        if (prio1 > prio2) {
                            w1 = HEIGH_WEIGHT;
                            w2 = LOW_WEIGHT;
                        } else {
                            w1 = LOW_WEIGHT;
                            w2 = HEIGH_WEIGHT;
                        }
                        if (sign == -1) {
                            // extra penalty if edges with different junction priority are in conflict
                            w1 *= 2;
                            w2 *= 2;
                        }
                    }
                    if (isRailway(e1->getPermissions()) != isRailway(e2->getPermissions())) {
                        w1 *= 0.1;
                        w2 *= 0.1;
                    }
                    if ((e1->getPermissions() & SVC_PASSENGER) == 0) {
                        w1 *= 0.1;
                    }
                    if ((e2->getPermissions() & SVC_PASSENGER) == 0) {
                        w2 *= 0.1;
                    }
                    val += sign * w1;
                    val += sign * w2;
#ifdef DEBUG_STREAM_ORDERING
                    if (DEBUGCOND && DEBUGEDGE(e2) && DEBUGEDGE(e1)) {
                        std::cout << "      sign=" << sign << " w1=" << w1 << " w2=" << w2 << " val=" << val
                                  << " c1=" << (*e1c).getDescription(e1)
                                  << " c2=" << (*e2c).getDescription(e2)
                                  << "\n";
                    }
#endif
                }
            }
        }
    }
#ifdef DEBUG_STREAM_ORDERING
    if (DEBUGCOND && DEBUGEDGE(e2) && DEBUGEDGE(e1)) {
        std::cout << "     computeUnblockedWeightedStreamNumber e1=" << e1->getID() << " e2=" << e2->getID() << " val=" << val << "\n";
    }
#endif
    return val;
}


std::pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestCombination(const EdgeVector& edges) {
    std::pair<NBEdge*, NBEdge*> bestPair(static_cast<NBEdge*>(nullptr), static_cast<NBEdge*>(nullptr));
    double bestValue = -std::numeric_limits<double>::max();
    for (EdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        for (EdgeVector::const_iterator j = i + 1; j != edges.end(); ++j) {
            const double value = computeUnblockedWeightedStreamNumber(*i, *j);
            if (value > bestValue) {
                bestValue = value;
                bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
            } else if (value == bestValue) {
                const double ca = GeomHelper::getMinAngleDiff((*i)->getAngleAtNode((*i)->getToNode()), (*j)->getAngleAtNode((*j)->getToNode()));
                const double oa = GeomHelper::getMinAngleDiff(bestPair.first->getAngleAtNode(bestPair.first->getToNode()), bestPair.second->getAngleAtNode(bestPair.second->getToNode()));
                if (fabs(oa - ca) < NUMERICAL_EPS) { // break ties by id
                    if (bestPair.first->getID() < (*i)->getID()) {
                        bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
                    }
                } else if (oa < ca) {
                    bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
                }
            }
        }
    }
    if (bestValue <= 0) {
        // do not group edges
        if (bestPair.first->getPriority() < bestPair.second->getPriority()) {
            std::swap(bestPair.first, bestPair.second);
        }
        bestPair.second = nullptr;
    }
#ifdef DEBUG_STREAM_ORDERING
    if (DEBUGCOND) {
        std::cout << "   getBestCombination bestValue=" << bestValue << "  best=" << Named::getIDSecure(bestPair.first) << ", " << Named::getIDSecure(bestPair.second) << "\n";
    }
#endif
    return bestPair;
}


std::pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestPair(EdgeVector& incoming) {
    if (incoming.size() == 1) {
        // only one there - return the one
        std::pair<NBEdge*, NBEdge*> ret(*incoming.begin(), static_cast<NBEdge*>(nullptr));
        incoming.clear();
        return ret;
    }
    // determine the best combination
    //  by priority, first
    EdgeVector used;
    std::sort(incoming.begin(), incoming.end(), edge_by_incoming_priority_sorter());
    used.push_back(*incoming.begin()); // the first will definitely be used
    // get the ones with the same priority
    int prio = getToPrio(*used.begin());
    for (EdgeVector::iterator i = incoming.begin() + 1; i != incoming.end() && prio == getToPrio(*i); ++i) {
        used.push_back(*i);
    }
    //  if there only lower priorised, use these, too
    if (used.size() < 2) {
        used = incoming;
    }
    std::pair<NBEdge*, NBEdge*> ret = getBestCombination(used);
#ifdef DEBUG_STREAM_ORDERING
    if (DEBUGCOND) {
        std::cout << "getBestPair tls=" << getID() << " incoming=" << toString(incoming) << " prio=" << prio << " used=" << toString(used) << " best=" << Named::getIDSecure(ret.first) << ", " << Named::getIDSecure(ret.second) << "\n";
    }
#endif

    incoming.erase(find(incoming.begin(), incoming.end(), ret.first));
    if (ret.second != nullptr) {
        incoming.erase(find(incoming.begin(), incoming.end(), ret.second));
    }
    return ret;
}

bool
NBOwnTLDef::hasStraightConnection(const NBEdge* fromEdge) {
    for (const NBEdge::Connection& c : fromEdge->getConnections()) {
        LinkDirection dir = fromEdge->getToNode()->getDirection(fromEdge, c.toEdge);
        if (dir == LinkDirection::STRAIGHT) {
            return true;
        }
    }
    return false;
}

NBTrafficLightLogic*
NBOwnTLDef::myCompute(int brakingTimeSeconds) {
    return computeLogicAndConts(brakingTimeSeconds);
}


NBTrafficLightLogic*
NBOwnTLDef::computeLogicAndConts(int brakingTimeSeconds, bool onlyConts) {
    if (myControlledNodes.size() == 1) {
        // otherwise, use values from previous call to initNeedsContRelation
        myNeedsContRelation.clear();
    }
    myRightOnRedConflicts.clear();
    const bool isNEMA = myType == TrafficLightType::NEMA;
    const SUMOTime brakingTime = TIME2STEPS(brakingTimeSeconds);
    const SUMOTime leftTurnTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.left-green.time"));
    const SUMOTime minMinDur = (myType == TrafficLightType::STATIC) ? UNSPECIFIED_DURATION : TIME2STEPS(OptionsCont::getOptions().getInt("tls.min-dur"));
    const SUMOTime maxDur = (myType == TrafficLightType::STATIC) ? UNSPECIFIED_DURATION : TIME2STEPS(OptionsCont::getOptions().getInt("tls.max-dur"));
    const SUMOTime earliestEnd = UNSPECIFIED_DURATION;
    const SUMOTime latestEnd = UNSPECIFIED_DURATION;

    // things collect for NEMA phase building
    std::vector<std::pair<NBEdge*, NBEdge*> > chosenList;
    std::vector<std::string> straightStates;
    std::vector<std::string> leftStates;

    // build complete lists first
    const EdgeVector& incoming = getIncomingEdges();
    EdgeVector fromEdges, toEdges;
    std::vector<bool> isTurnaround;
    std::vector<bool> hasTurnLane;
    std::vector<int> fromLanes;
    std::vector<int> toLanes;
    std::vector<SUMOTime> crossingTime;
    int totalNumLinks = 0;
    for (NBEdge* const fromEdge : incoming) {
        const int numLanes = fromEdge->getNumLanes();
        const bool edgeHasStraight = hasStraightConnection(fromEdge);
        for (int i2 = 0; i2 < numLanes; i2++) {
            bool hasLeft = false;
            bool hasPartLeft = false;
            bool hasStraight = false;
            bool hasRight = false;
            bool hasTurnaround = false;
            for (const NBEdge::Connection& approached : fromEdge->getConnectionsFromLane(i2)) {
                if (!fromEdge->mayBeTLSControlled(i2, approached.toEdge, approached.toLane)) {
                    continue;
                }
                fromEdges.push_back(fromEdge);
                fromLanes.push_back(i2);
                toLanes.push_back(approached.toLane);
                toEdges.push_back(approached.toEdge);
                if (approached.vmax < NUMERICAL_EPS || (fromEdge->getPermissions() & SVC_PASSENGER) == 0
                        || (approached.toEdge->getPermissions() & SVC_PASSENGER) == 0) {
                    crossingTime.push_back(0);
                } else {
                    crossingTime.push_back(TIME2STEPS((approached.length + approached.viaLength) / MAX2(approached.vmax, MIN_SPEED_CROSSING_TIME)));
                }
                // std::cout << fromEdge->getID() << " " << approached.toEdge->getID() << " " << (fromEdge->getPermissions() & SVC_PASSENGER) << " " << approached.length << " " << approached.viaLength << " " << approached.vmax << " " << crossingTime.back() << std::endl;
                if (approached.toEdge != nullptr) {
                    isTurnaround.push_back(fromEdge->isTurningDirectionAt(approached.toEdge));
                } else {
                    isTurnaround.push_back(true);
                }
                LinkDirection dir = fromEdge->getToNode()->getDirection(fromEdge, approached.toEdge);
                if (dir == LinkDirection::STRAIGHT) {
                    hasStraight = true;
                } else if (dir == LinkDirection::RIGHT || dir == LinkDirection::PARTRIGHT) {
                    hasRight = true;
                } else if (dir == LinkDirection::LEFT) {
                    hasLeft = true;
                } else if (dir == LinkDirection::PARTLEFT) {
                    hasPartLeft = true;
                } else if (dir == LinkDirection::TURN) {
                    hasTurnaround = true;
                }
                totalNumLinks++;
            }
            for (const NBEdge::Connection& approached : fromEdge->getConnectionsFromLane(i2)) {
                if (!fromEdge->mayBeTLSControlled(i2, approached.toEdge, approached.toLane)) {
                    continue;
                }
                hasTurnLane.push_back(
                    (hasLeft && !hasPartLeft && !hasStraight && !hasRight)
                    || (hasPartLeft && !hasLeft && !hasStraight && !hasRight)
                    || (hasPartLeft && hasLeft && edgeHasStraight && !hasRight)
                    || (!hasLeft && !hasPartLeft && !hasTurnaround && hasRight));
            }
            //std::cout << " from=" << fromEdge->getID() << "_" << i2 << " hasTurnLane=" << hasTurnLane.back() << " s=" << hasStraight << " l=" << hasLeft << " r=" << hasRight << " t=" << hasTurnaround << "\n";
        }
    }
    // collect crossings
    std::vector<NBNode::Crossing*> crossings;
    for (NBNode* const node : myControlledNodes) {
        const std::vector<NBNode::Crossing*>& c = node->getCrossings();
        if (!onlyConts) {
            // set tl indices for crossings
            node->setCrossingTLIndices(getID(), totalNumLinks);
        }
        copy(c.begin(), c.end(), std::back_inserter(crossings));
        totalNumLinks += (int)c.size();
    }

    NBTrafficLightLogic* logic = new NBTrafficLightLogic(getID(), getProgramID(), totalNumLinks, myOffset, myType);
    EdgeVector toProc = getConnectedOuterEdges(incoming);
    const SUMOTime greenTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.green.time"));
    SUMOTime allRedTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.allred.time"));
    const double minorLeftSpeedThreshold = OptionsCont::getOptions().getFloat("tls.minor-left.max-speed");
    const bool noMixed = OptionsCont::getOptions().getBool("tls.no-mixed");
    // left-turn phases do not work well for joined tls, so we build incoming instead
    if (myLayout == TrafficLightLayout::DEFAULT) {
        // @note this prevents updating after loading plain-xml into netedit computing tls and then changing the default layout
        myLayout = SUMOXMLDefinitions::TrafficLightLayouts.get(OptionsCont::getOptions().getString("tls.layout"));
    }
    const bool groupOpposites = (myLayout == TrafficLightLayout::OPPOSITES && (myControlledNodes.size() <= 2 || corridorLike()));

    // build all phases
    std::vector<int> greenPhases; // indices of green phases
    std::vector<bool> hadGreenMajor(totalNumLinks, false);
    while (toProc.size() > 0) {
        bool groupTram = false;
        bool groupOther = false;
        std::pair<NBEdge*, NBEdge*> chosen;
        std::set<const NBEdge*> chosenSet;
        if (groupOpposites) {
            if (incoming.size() == 2) {
                // if there are only 2 incoming edges we need to decide whether they are a crossing or a "continuation"
                // @node: this heuristic could be extended to also check the number of outgoing edges
                double angle = fabs(NBHelpers::relAngle(incoming[0]->getAngleAtNode(incoming[0]->getToNode()), incoming[1]->getAngleAtNode(incoming[1]->getToNode())));
                // angle would be 180 for straight opposing incoming edges
                if (angle < 135) {
                    chosen = std::pair<NBEdge*, NBEdge*>(toProc[0], static_cast<NBEdge*>(nullptr));
                    toProc.erase(toProc.begin());
                } else {
                    chosen = getBestPair(toProc);
                }
            } else {
                chosen = getBestPair(toProc);
                if (chosen.second == nullptr && chosen.first->getPermissions() == SVC_TRAM) {
                    groupTram = true;
                    for (auto it = toProc.begin(); it != toProc.end();) {
                        if ((*it)->getPermissions() == SVC_TRAM) {
                            it = toProc.erase(it);
                        } else {
                            it++;
                        }
                    }
                }
            }
        } else {
            NBEdge* chosenEdge = toProc[0];
            chosen = std::pair<NBEdge*, NBEdge*>(chosenEdge, static_cast<NBEdge*>(nullptr));
            toProc.erase(toProc.begin());
            SVCPermissions perms = chosenEdge->getPermissions();
            if (perms == SVC_TRAM) {
                groupTram = true;
            } else if ((perms & ~(SVC_PEDESTRIAN | SVC_BICYCLE | SVC_DELIVERY)) == 0) {
                groupOther = true;
            }
            // group all edges with the same permissions into a single phase (later)
            if (groupTram || groupOther) {
                for (auto it = toProc.begin(); it != toProc.end();) {
                    if ((*it)->getPermissions() == perms) {
                        it = toProc.erase(it);
                    } else {
                        it++;
                    }
                }
            }
        }
        int pos = 0;
        std::string state(totalNumLinks, 'r');
#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            std::cout << " computing " << getID() << " prog=" << getProgramID() << " cho1=" << Named::getIDSecure(chosen.first) << " cho2=" << Named::getIDSecure(chosen.second) << " toProc=" << toString(toProc) << " bentPrio=" << chosen.first->getToNode()->isBentPriority() << "\n";
        }
#endif
        chosenList.push_back(chosen);
        chosenSet.insert(chosen.first);
        if (chosen.second != nullptr) {
            chosenSet.insert(chosen.second);
        }
        // find parallel bike edge for the chosen (passenger) edges
        for (const NBEdge* e : chosenSet) {
            if ((e->getPermissions() & SVC_PASSENGER) != 0) {
                std::vector<NBEdge*> parallelBikeEdges;
                for (NBEdge* cand : toProc) {
                    if ((cand->getPermissions() & ~SVC_PEDESTRIAN) == SVC_BICYCLE) {
                        double angle = fabs(NBHelpers::relAngle(e->getAngleAtNode(e->getToNode()), cand->getAngleAtNode(cand->getToNode())));
                        if (angle < 30) {
                            // roughly parallel
                            parallelBikeEdges.push_back(cand);
                        }
                    }
                }
                for (NBEdge* be : parallelBikeEdges) {
#ifdef DEBUG_PHASES
                    if (DEBUGCOND) {
                        std::cout << " chosen=" << e->getID() << " be=" << be->getID() << "\n";
                    }
#endif
                    chosenSet.insert(be);
                    toProc.erase(std::find(toProc.begin(), toProc.end(), be));
                }
            }
        }
        // plain straight movers
        double maxSpeed = 0;
        bool haveGreen = false;
        for (const NBEdge* const fromEdge : incoming) {
            const bool inChosen = chosenSet.count(fromEdge) != 0;
            const int numLanes = fromEdge->getNumLanes();
            for (int i2 = 0; i2 < numLanes; i2++) {
                for (const NBEdge::Connection& approached : fromEdge->getConnectionsFromLane(i2)) {
                    if (!fromEdge->mayBeTLSControlled(i2, approached.toEdge, approached.toLane)) {
                        continue;
                    }
                    if (inChosen) {
                        state[pos] = 'G';
                        haveGreen = true;
                        maxSpeed = MAX2(maxSpeed, fromEdge->getSpeed());
                    } else {
                        state[pos] = 'r';
                    }
                    ++pos;
                }
            }
        }
        if (!haveGreen) {
            continue;
        }

#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            std::cout << " state after plain straight movers " << state << "\n";
        }
#endif
        if (!isNEMA) {
            // correct behaviour for those that are not in chosen, but may drive, though
            state = allowCompatible(state, fromEdges, toEdges, fromLanes, toLanes);
#ifdef DEBUG_PHASES
            if (DEBUGCOND) {
                std::cout << " state after allowing compatible " << state << "\n";
            }
#endif
            if (groupTram) {
                state = allowByVClass(state, fromEdges, toEdges, SVC_TRAM);
            } else if (groupOther) {
                state = allowByVClass(state, fromEdges, toEdges, SVC_PEDESTRIAN | SVC_BICYCLE | SVC_DELIVERY);
            }
#ifdef DEBUG_PHASES
            if (DEBUGCOND) {
                std::cout << " state after grouping by vClass " << state << " (groupTram=" << groupTram << " groupOther=" << groupOther << ")\n";
            }
#endif
            if (groupOpposites || chosen.first->getToNode()->getType() == SumoXMLNodeType::TRAFFIC_LIGHT_RIGHT_ON_RED) {
                state = allowUnrelated(state, fromEdges, toEdges, isTurnaround, crossings);
            }
#ifdef DEBUG_PHASES
            if (DEBUGCOND) {
                std::cout << " state after finding allowUnrelated " << state << "\n";
            }
#endif
        }
        // correct behaviour for those that have to wait (mainly left-mover)
        bool haveForbiddenLeftMover = false;
        std::vector<bool> rightTurnConflicts(pos, false);
        std::vector<bool> mergeConflicts(pos, false);
        state = correctConflicting(state, fromEdges, toEdges, isTurnaround, fromLanes, toLanes, hadGreenMajor, haveForbiddenLeftMover, rightTurnConflicts, mergeConflicts);
        for (int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] == 'G') {
                hadGreenMajor[i1] = true;
            }
        }
#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            std::cout << " state after correcting left movers=" << state << "\n";
        }
#endif

        std::vector<bool> leftGreen(pos, false);
        // check whether at least one left-turn lane exist
        bool foundLeftTurnLane = false;
        for (int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] == 'g' && !rightTurnConflicts[i1] && !mergeConflicts[i1] && hasTurnLane[i1]) {
                foundLeftTurnLane = true;
            }
        }
        const bool buildLeftGreenPhase = (haveForbiddenLeftMover && !myHaveSinglePhase && leftTurnTime > 0 && foundLeftTurnLane
                                          && groupOpposites && !groupTram && !groupOther);

        // find indices for exclusive left green phase and apply option minor-left.max-speed
        for (int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] == 'g' && !rightTurnConflicts[i1] && !mergeConflicts[i1]
                    // only activate turn-around together with a real left-turn
                    && (!isTurnaround[i1] || (i1 > 0 && leftGreen[i1 - 1]))) {
                leftGreen[i1] = true;
                if (fromEdges[i1]->getSpeed() > minorLeftSpeedThreshold) {
                    if (buildLeftGreenPhase) {
                        state[i1] = 'r';
                        //std::cout << " disabling minorLeft " << i1 << " (speed=" << fromEdges[i1]->getSpeed() << " thresh=" << minorLeftSpeedThreshold << ")\n";
                    } else if (!isTurnaround[i1]) {
                        WRITE_WARNINGF(TL("Minor green from edge '%' to edge '%' exceeds %m/s. Maybe a left-turn lane is missing."),
                                       fromEdges[i1]->getID(), toEdges[i1]->getID(), minorLeftSpeedThreshold);
                    }
                }
            }
        }

#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            std::cout << getID() << " state=" << state << " buildLeft=" << buildLeftGreenPhase << " hFLM=" << haveForbiddenLeftMover << " turnLane=" << foundLeftTurnLane
                      << "   \nrtC=" << toString(rightTurnConflicts)
                      << "   \nmC=" << toString(mergeConflicts)
                      << "   \nhTL=" << toString(hasTurnLane)
                      << "   \nlGr=" << toString(leftGreen)
                      << "\n";
        }
#endif
        straightStates.push_back(state);

        const std::string vehicleState = state; // backup state before pedestrian modifications
        greenPhases.push_back((int)logic->getPhases().size());

        // 5s at 50km/h, 10s at 80km/h, rounded to full seconds
        const double minDurBySpeed = maxSpeed * 3.6 / 6 - 3.3;
        SUMOTime minDur = MAX2(minMinDur, TIME2STEPS(floor(minDurBySpeed + 0.5)));
        if (chosen.first->getPermissions() == SVC_TRAM && (chosen.second == nullptr || chosen.second->getPermissions() == SVC_TRAM)) {
            // shorter minDuration for tram phase (only if the phase is
            // exclusively for tram)
            bool tramExclusive = true;
            for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
                if (state[i1] == 'G') {
                    SVCPermissions linkPerm = (fromEdges[i1]->getPermissions() & toEdges[i1]->getPermissions());
                    if (linkPerm != SVC_TRAM) {
                        tramExclusive = false;
                        break;
                    }
                }
            }
            if (tramExclusive) {
                // one tram per actuated phase
                minDur = TIME2STEPS(1);
            }
        }

        state = addPedestrianPhases(logic, greenTime, minDur, maxDur, earliestEnd, latestEnd, state, crossings, fromEdges, toEdges);
        // pedestrians have 'r' from here on
        for (int i1 = pos; i1 < pos + (int)crossings.size(); ++i1) {
            state[i1] = 'r';
        }
        if (brakingTime > 0) {
            SUMOTime maxCross = 0;
            // build yellow (straight)
            for (int i1 = 0; i1 < pos; ++i1) {
                if (state[i1] != 'G' && state[i1] != 'g') {
                    continue;
                }
                if ((vehicleState[i1] >= 'a' && vehicleState[i1] <= 'z')
                        && buildLeftGreenPhase
                        && !rightTurnConflicts[i1]
                        && !mergeConflicts[i1]
                        && leftGreen[i1]) {
                    continue;
                }
                state[i1] = 'y';
                maxCross = MAX2(maxCross, crossingTime[i1]);
            }
            // add step
            logic->addStep(brakingTime, state);
            // add optional all-red state
            if (!buildLeftGreenPhase) {
                if (myLayout == TrafficLightLayout::ALTERNATE_ONEWAY) {
                    allRedTime = computeEscapeTime(state, fromEdges, toEdges);
                }
                buildAllRedState(allRedTime + MAX2(0ll, maxCross - brakingTime - allRedTime), logic, state);
            }
        }


        if (buildLeftGreenPhase) {
            // build left green
            for (int i1 = 0; i1 < pos; ++i1) {
                if (state[i1] == 'Y' || state[i1] == 'y') {
                    state[i1] = 'r';
                    continue;
                }
                if (leftGreen[i1]) {
                    state[i1] = 'G';
                }
            }
            leftStates.push_back(state);
            state = allowCompatible(state, fromEdges, toEdges, fromLanes, toLanes);
            state = correctConflicting(state, fromEdges, toEdges, isTurnaround, fromLanes, toLanes, hadGreenMajor, haveForbiddenLeftMover, rightTurnConflicts, mergeConflicts);
            bool buildMixedGreenPhase = false;
            std::vector<bool> mixedGreen(pos, false);
            const std::string oldState = state;
            if (noMixed) {
                state = correctMixed(state, fromEdges, fromLanes, buildMixedGreenPhase, mixedGreen);
            }
            if (state != oldState) {
                for (int i1 = 0; i1 < pos; ++i1) {
                    if (mixedGreen[i1]) {
                        // patch previous yellow and allred phase
                        int yellowIndex = (int)logic->getPhases().size() - 1;
                        if (allRedTime > 0) {
                            logic->setPhaseState(yellowIndex--, i1, LINKSTATE_TL_RED);
                        }
                        if (brakingTime > 0) {
                            logic->setPhaseState(yellowIndex, i1, LINKSTATE_TL_YELLOW_MINOR);
                        }
                    }
                }
                state = allowCompatible(state, fromEdges, toEdges, fromLanes, toLanes);
            }

            // add step
            logic->addStep(leftTurnTime, state, minDur, maxDur, earliestEnd, latestEnd);

            // build left yellow
            if (brakingTime > 0) {
                SUMOTime maxCross = 0;
                for (int i1 = 0; i1 < pos; ++i1) {
                    if (state[i1] != 'G' && state[i1] != 'g') {
                        continue;
                    }
                    state[i1] = 'y';
                    maxCross = MAX2(maxCross, crossingTime[i1]);
                }
                // add step
                logic->addStep(brakingTime, state);
                // add optional all-red state
                buildAllRedState(allRedTime + MAX2(0ll, maxCross - brakingTime - allRedTime), logic, state);
            }

            if (buildMixedGreenPhase) {
                // build mixed green
                // @todo if there is no left green phase we might want to build two
                // mixed-green phases but then we should consider avoid a common
                // opposite phase for this direction

                for (int i1 = 0; i1 < pos; ++i1) {
                    if (state[i1] == 'Y' || state[i1] == 'y') {
                        state[i1] = 'r';
                        continue;
                    }
                    if (mixedGreen[i1]) {
                        state[i1] = 'G';
                    }
                }
                state = allowCompatible(state, fromEdges, toEdges, fromLanes, toLanes);
                state = correctConflicting(state, fromEdges, toEdges, isTurnaround, fromLanes, toLanes, hadGreenMajor, haveForbiddenLeftMover, rightTurnConflicts, mergeConflicts);

                // add step
                logic->addStep(leftTurnTime, state, minDur, maxDur, earliestEnd, latestEnd);

                // build mixed yellow
                if (brakingTime > 0) {
                    SUMOTime maxCross = 0;
                    for (int i1 = 0; i1 < pos; ++i1) {
                        if (state[i1] != 'G' && state[i1] != 'g') {
                            continue;
                        }
                        state[i1] = 'y';
                        maxCross = MAX2(maxCross, crossingTime[i1]);
                    }
                    // add step
                    logic->addStep(brakingTime, state);
                    // add optional all-red state
                    buildAllRedState(allRedTime + MAX2(0ll, maxCross - brakingTime - allRedTime), logic, state);
                }
            }

        } else if (isNEMA) {
            std::string& s = straightStates.back();
            std::string leftState = s;
            for (int ii = 0; ii < pos; ++ii) {
                if (s[ii] != 'r') {
                    NBEdge* fromEdge = fromEdges[ii];
                    NBEdge* toEdge = toEdges[ii];
                    LinkDirection dir = fromEdge->getToNode()->getDirection(fromEdge, toEdge);
                    if (hasTurnLane[ii] && (dir == LinkDirection::LEFT || dir == LinkDirection::TURN)) {
                        s[ii] = 'r';
                        leftState[ii] = 'G';
                    } else {
                        leftState[ii] = 'r';
                    }
                }
            }
            leftStates.push_back(leftState);
        }
        // fix edges within joined traffic lights that did not get the green light yet
        if (myEdgesWithin.size() > 0 && !isNEMA && toProc.size() == 0) {
            addGreenWithin(logic, fromEdges, toProc);
        }
    }
    // fix pedestrian crossings that did not get the green light yet
    if (crossings.size() > 0) {
        addPedestrianScramble(logic, totalNumLinks, TIME2STEPS(10), brakingTime, crossings, fromEdges, toEdges);
    }
    // add optional red phase if there were no foes
    if (logic->getPhases().size() == 2 && brakingTime > 0
            && OptionsCont::getOptions().getInt("tls.red.time") > 0) {
        const SUMOTime redTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.red.time"));
        logic->addStep(redTime, std::string(totalNumLinks, 'r'));
    }
    // fix states to account for custom crossing link indices
    if (crossings.size() > 0 && !onlyConts) {
        checkCustomCrossingIndices(logic);
    }

    if (myLayout == TrafficLightLayout::ALTERNATE_ONEWAY) {
        // exiting the oneway section should always be possible
        deactivateInsideEdges(logic, fromEdges);
    }
    if (isNEMA) {
        NBTrafficLightLogic* nemaLogic = buildNemaPhases(fromEdges, toEdges, crossings, chosenList, straightStates, leftStates);
        if (nemaLogic == nullptr) {
            WRITE_WARNINGF(TL("Generating NEMA phases is not support for traffic light '%' with % incoming edges. Using tlType 'actuated' as fallback"), getID(), incoming.size());
            logic->setType(TrafficLightType::ACTUATED);
            setType(TrafficLightType::ACTUATED);
        } else {
            delete logic;
            logic = nemaLogic;
        }
    }

    SUMOTime totalDuration = logic->getDuration();

    if ((OptionsCont::getOptions().isDefault("tls.green.time") || !OptionsCont::getOptions().isDefault("tls.cycle.time")) && !isNEMA) {
        const SUMOTime cycleTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.cycle.time"));
        // adapt to cycle time by changing the duration of the green phases
        SUMOTime minGreenDuration = SUMOTime_MAX;
        for (std::vector<int>::const_iterator it = greenPhases.begin(); it != greenPhases.end(); ++it) {
            const SUMOTime dur = logic->getPhases()[*it].duration;
            minGreenDuration = MIN2(minGreenDuration, dur);
        }
        const int patchSeconds = (int)(STEPS2TIME(cycleTime - totalDuration) / (double)greenPhases.size());
        const int patchSecondsRest = (int)(STEPS2TIME(cycleTime - totalDuration)) - patchSeconds * (int)greenPhases.size();
        //std::cout << "cT=" << cycleTime << " td=" << totalDuration << " pS=" << patchSeconds << " pSR=" << patchSecondsRest << "\n";
        if (STEPS2TIME(minGreenDuration) + patchSeconds < MIN_GREEN_TIME
                || STEPS2TIME(minGreenDuration) + patchSeconds + patchSecondsRest < MIN_GREEN_TIME
                || greenPhases.size() == 0) {
            if (getID() != DummyID) {
                WRITE_WARNINGF(TL("The traffic light '%' cannot be adapted to a cycle time of %."), getID(), time2string(cycleTime));
            }
            // @todo use a multiple of cycleTime ?
        } else {
            for (std::vector<int>::const_iterator it = greenPhases.begin(); it != greenPhases.end(); ++it) {
                logic->setPhaseDuration(*it, logic->getPhases()[*it].duration + TIME2STEPS(patchSeconds));
            }
            if (greenPhases.size() > 0) {
                logic->setPhaseDuration(greenPhases.front(), logic->getPhases()[greenPhases.front()].duration + TIME2STEPS(patchSecondsRest));
            }
            totalDuration = logic->getDuration();
        }
    }

    // check for coherent signal sequence and remove yellow if preceded and followed by green
    const std::vector<NBTrafficLightLogic::PhaseDefinition>& allPhases = logic->getPhases();
    const int phaseCount = (int)allPhases.size();
    const int stateSize = (int)logic->getNumLinks();
    for (int i = 0; i < phaseCount; ++i) {
        std::string currState = allPhases[i].state;
        const int prevIndex = (i == 0) ? phaseCount - 1 : i - 1;
        const std::string prevState = allPhases[prevIndex].state;
        const std::string nextState = allPhases[(i + 1) % phaseCount].state;
        bool updatedState = false;
        for (int i1 = 0; i1 < stateSize; ++i1) {
            if (currState[i1] == 'y' && (nextState[i1] == 'g' || nextState[i1] == 'G') && (prevState[i1] == 'g' || prevState[i1] == 'G')) {
                LinkState ls = (nextState[i1] == prevState[i1]) ? (LinkState)prevState[i1] : (LinkState)'g';
                logic->setPhaseState(i, i1, ls);
                updatedState = true;
            }
        }
        UNUSED_PARAMETER(updatedState);  // disable warning
#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            if (updatedState) {
                std::cout << getID() << " state of phase index " << i <<  " was patched due to yellow in between green\n";
            }

        }
#endif
    }


    myRightOnRedConflictsReady = true;
    // this computation only makes sense for single nodes
    myNeedsContRelationReady = (myControlledNodes.size() == 1);
    if (totalDuration > 0) {
        if (totalDuration > 3 * (greenTime + 2 * brakingTime + leftTurnTime) && !isNEMA) {
            WRITE_WARNINGF(TL("The traffic light '%' has a high cycle time of %."), getID(), time2string(totalDuration));
        }
        logic->closeBuilding();
        return logic;
    } else {
        delete logic;
        return nullptr;
    }
}


bool
NBOwnTLDef::hasCrossing(const NBEdge* from, const NBEdge* to, const std::vector<NBNode::Crossing*>& crossings) {
    assert(to != 0);
    for (auto c : crossings) {
        const NBNode::Crossing& cross = *c;
        // only check connections at this crossings node
        if (to->getFromNode() == cross.node) {
            for (EdgeVector::const_iterator it_e = cross.edges.begin(); it_e != cross.edges.end(); ++it_e) {
                const NBEdge* edge = *it_e;
                if (edge == from || edge == to) {
                    return true;
                }
            }
        }
    }
    return false;
}


std::string
NBOwnTLDef::addPedestrianPhases(NBTrafficLightLogic* logic, const SUMOTime greenTime, const SUMOTime minDur, const SUMOTime maxDur,
                                const SUMOTime earliestEnd, const SUMOTime latestEnd,
                                std::string state, const std::vector<NBNode::Crossing*>& crossings, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    // compute based on length of the crossing if not set by the user
    const SUMOTime pedClearingTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.crossing-clearance.time"));
    // compute if not set by user: must be able to reach the middle of the second "Richtungsfahrbahn"
    const SUMOTime minPedTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.crossing-min.time"));
    const std::string orig = state;
    state = patchStateForCrossings(state, crossings, fromEdges, toEdges);
    if (orig == state) {
        // add step
        logic->addStep(greenTime, state, minDur, maxDur, earliestEnd, latestEnd);
    } else {
        const SUMOTime pedTime = greenTime - pedClearingTime;
        if (pedTime >= minPedTime) {
            // ensure clearing time for pedestrians
            const int pedStates = (int)crossings.size();
            logic->addStep(pedTime, state, minDur, maxDur, earliestEnd, latestEnd);
#ifdef DEBUG_PHASES
            if (DEBUGCOND2(logic)) {
                std::cout << " intermidate state for addPedestrianPhases " << state << "\n";
            }
#endif
            state = state.substr(0, state.size() - pedStates) + std::string(pedStates, 'r');
            logic->addStep(pedClearingTime, state);
        } else {
            state = orig;
            // not safe for pedestrians.
            logic->addStep(greenTime, state, minDur, maxDur, earliestEnd, latestEnd);
        }
    }
#ifdef DEBUG_PHASES
    if (DEBUGCOND2(logic)) {
        std::cout << " state after addPedestrianPhases " << state << "\n";
    }
#endif
    return state;
}


std::string
NBOwnTLDef::patchStateForCrossings(const std::string& state, const std::vector<NBNode::Crossing*>& crossings, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    std::string result = state;
    const int pos = (int)(state.size() - crossings.size()); // number of controlled vehicle links
    for (int ic = 0; ic < (int)crossings.size(); ++ic) {
        const int i1 = pos + ic;
        const NBNode::Crossing& cross = *crossings[ic];
        bool isForbidden = false;
        for (int i2 = 0; i2 < pos && !isForbidden; ++i2) {
            // only check connections at this crossings node
            if (fromEdges[i2] != 0 && toEdges[i2] != 0 && fromEdges[i2]->getToNode() == cross.node) {
                for (EdgeVector::const_iterator it = cross.edges.begin(); it != cross.edges.end(); ++it) {
                    const NBEdge* edge = *it;
                    const LinkDirection i2dir = cross.node->getDirection(fromEdges[i2], toEdges[i2]);
                    if (state[i2] != 'r' && state[i2] != 's' && (edge == fromEdges[i2] ||
                            (edge == toEdges[i2] && (i2dir == LinkDirection::STRAIGHT || i2dir == LinkDirection::PARTLEFT || i2dir == LinkDirection::PARTRIGHT)))) {
                        isForbidden = true;
                        break;
                    }
                }
            }
        }
        if (!isForbidden) {
            result[i1] = 'G';
        } else {
            result[i1] = 'r';
        }
    }

    // correct behaviour for roads that are in conflict with a pedestrian crossing
    for (int i1 = 0; i1 < pos; ++i1) {
        if (result[i1] == 'G') {
            for (int ic = 0; ic < (int)crossings.size(); ++ic) {
                const NBNode::Crossing& crossing = *crossings[ic];
                if (fromEdges[i1] != 0 && toEdges[i1] != 0 && fromEdges[i1]->getToNode() == crossing.node) {
                    const int i2 = pos + ic;
                    if (result[i2] == 'G' && crossing.node->mustBrakeForCrossing(fromEdges[i1], toEdges[i1], crossing)) {
                        result[i1] = 'g';
                        break;
                    }
                }
            }
        }
    }
    return result;
}


std::string
NBOwnTLDef::patchNEMAStateForCrossings(const std::string& state,
                                       const std::vector<NBNode::Crossing*>& crossings,
                                       const EdgeVector& fromEdges,
                                       const EdgeVector& toEdges,
                                       const NBEdge* greenEdge, NBEdge* otherChosen) {
    std::string result = state;
    const int pos = (int)(state.size() - crossings.size()); // number of controlled vehicle links
    const EdgeVector& all = greenEdge->getToNode()->getEdges();
    EdgeVector::const_iterator start = std::find(all.begin(), all.end(), greenEdge);

    // permit crossings over edges between the current green edge and it's straight continuation
    const NBEdge* endEdge = nullptr;
    for (int i = 0; i < (int)state.size(); i++) {
        if (state[i] == 'G' && fromEdges[i] == greenEdge
                && greenEdge->getToNode()->getDirection(greenEdge, toEdges[i]) == LinkDirection::STRAIGHT) {
            // straight edge found
            endEdge = toEdges[i];
            break;
        }
    }
    if (endEdge == nullptr) {
        endEdge = otherChosen;
    }
    if (endEdge == nullptr) {
        // try to find the reverse edge of the green edge
        auto itCW = start;
        NBContHelper::nextCW(all, itCW);
        if ((*itCW)->getFromNode() == greenEdge->getToNode()) {
            endEdge = *itCW;
        }
    }
    if (endEdge == nullptr) {
        // at least prevent an infinite loop
        endEdge = greenEdge;
    }
    //std::cout << " patchNEMAStateForCrossings green=" << greenEdge->getID() << " other=" << Named::getIDSecure(otherChosen) << " end=" << Named::getIDSecure(end) << " all=" << toString(all) << "\n";

    EdgeVector::const_iterator end = std::find(all.begin(), all.end(), endEdge);
    if (end == all.end()) {
        // at least prevent an infinite loop
        end = start;
    }
    auto it = start;
    NBContHelper::nextCCW(all, it);
    for (; it != end; NBContHelper::nextCCW(all, it)) {
        for (int ic = 0; ic < (int)crossings.size(); ++ic) {
            const int i1 = pos + ic;
            const NBNode::Crossing& cross = *crossings[ic];
            for (const NBEdge* crossed : cross.edges) {
                //std::cout << "   cand=" << (*it)->getID() << " crossed=" << crossed->getID() << "\n";
                if (crossed == *it) {
                    result[i1] = 'G';
                    break;
                }
            }
        }
    }
    // correct behaviour for roads that are in conflict with a pedestrian crossing
    for (int i1 = 0; i1 < pos; ++i1) {
        if (result[i1] == 'G') {
            for (int ic = 0; ic < (int)crossings.size(); ++ic) {
                const NBNode::Crossing& crossing = *crossings[ic];
                const int i2 = pos + ic;
                if (result[i2] == 'G' && crossing.node->mustBrakeForCrossing(fromEdges[i1], toEdges[i1], crossing)) {
                    result[i1] = 'g';
                    break;
                }
            }
        }
    }
    return result;
}


void
NBOwnTLDef::collectLinks() {
    myControlledLinks.clear();
    collectAllLinks(myControlledLinks);
}


void
NBOwnTLDef::setTLControllingInformation() const {
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    for (NBConnectionVector::const_iterator j = myControlledLinks.begin(); j != myControlledLinks.end(); ++j) {
        const NBConnection& conn = *j;
        NBEdge* edge = conn.getFrom();
        edge->setControllingTLInformation(conn, getID());
    }
}


void
NBOwnTLDef::remapRemoved(NBEdge* /*removed*/, const EdgeVector& /*incoming*/,
                         const EdgeVector& /*outgoing*/) {}


void
NBOwnTLDef::replaceRemoved(NBEdge* /*removed*/, int /*removedLane*/,
                           NBEdge* /*by*/, int /*byLane*/, bool /*incoming*/) {}


void
NBOwnTLDef::initNeedsContRelation() const {
    if (!myNeedsContRelationReady) {
        if (myControlledNodes.size() > 0) {
            // setParticipantsInformation resets myAmInTLS so we need to make a copy
            std::vector<bool> edgeInsideTLS;
            for (const NBEdge* e : myIncomingEdges) {
                edgeInsideTLS.push_back(e->isInsideTLS());
            }
            // we use a dummy node just to maintain const-correctness
            myNeedsContRelation.clear();
            for (NBNode* n : myControlledNodes) {
                NBOwnTLDef dummy(DummyID, n, 0, TrafficLightType::STATIC);
                dummy.setParticipantsInformation();
                NBTrafficLightLogic* tllDummy = dummy.computeLogicAndConts(0, true);
                delete tllDummy;
                myNeedsContRelation.insert(dummy.myNeedsContRelation.begin(), dummy.myNeedsContRelation.end());
                n->removeTrafficLight(&dummy);
            }
            if (myControlledNodes.size() > 1) {
                int i = 0;
                for (NBEdge* e : myIncomingEdges) {
                    e->setInsideTLS(edgeInsideTLS[i]);
                    i++;
                }
            }
#ifdef DEBUG_CONTRELATION
            if (DEBUGCOND) {
                std::cout << " contRelations at " << getID() << " prog=" << getProgramID() << ":\n";
                for (const StreamPair& s : myNeedsContRelation) {
                    std::cout << "   " << s.from1->getID() << "->" << s.to1->getID() << " foe " << s.from2->getID() << "->" << s.to2->getID() << "\n";
                }
            }
#endif

        }
        myNeedsContRelationReady = true;
    }
}


EdgeVector
NBOwnTLDef::getConnectedOuterEdges(const EdgeVector& incoming) {
    EdgeVector result = incoming;
    for (EdgeVector::iterator it = result.begin(); it != result.end();) {
        if ((*it)->getConnections().size() == 0 || (*it)->isInsideTLS()) {
            it = result.erase(it);
        } else {
            ++it;
        }
    }
    return result;
}


std::string
NBOwnTLDef::allowCompatible(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges,
                            const std::vector<int>& fromLanes, const std::vector<int>& toLanes) {
    state = allowSingleEdge(state, fromEdges);
#ifdef DEBUG_PHASES
    if (DEBUGCOND) {
        std::cout << " state after allowSingle " << state << "\n";
    }
#endif
    if (myControlledNodes.size() > 1) {
        state = allowFollowers(state, fromEdges, toEdges);
#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            std::cout << " state after allowFollowers " << state << "\n";
        }
#endif
        state = allowPredecessors(state, fromEdges, toEdges, fromLanes, toLanes);
#ifdef DEBUG_PHASES
        if (DEBUGCOND) {
            std::cout << " state after allowPredecessors " << state << "\n";
        }
#endif
    }
    return state;
}


std::string
NBOwnTLDef::allowSingleEdge(std::string state, const EdgeVector& fromEdges) {
    // if only one edge has green, ensure sure that all connections from that edge are green
    const int size = (int)fromEdges.size();
    NBEdge* greenEdge = nullptr;
    for (int i1 = 0; i1 < size; ++i1) {
        if (state[i1] == 'G') {
            if (greenEdge == nullptr) {
                greenEdge = fromEdges[i1];
            } else if (greenEdge != fromEdges[i1]) {
                return state;
            }
        }
    }
    if (greenEdge != nullptr) {
        for (int i1 = 0; i1 < size; ++i1) {
            if (fromEdges[i1] == greenEdge) {
                state[i1] = 'G';
            }
        }
    }
    return state;
}


std::string
NBOwnTLDef::allowFollowers(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    // check continuation within joined traffic lights
    bool check = true;
    while (check) {
        check = false;
        for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
            if (state[i1] == 'G') {
                continue;
            }
            if (forbidden(state, i1, fromEdges, toEdges, true)) {
                continue;
            }
            bool followsChosen = false;
            for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                if (state[i2] == 'G' && fromEdges[i1] == toEdges[i2]) {
                    followsChosen = true;
                    break;
                }
            }
            if (followsChosen) {
                state[i1] = 'G';
                check = true;
            }
        }
    }
    return state;
}


std::string
NBOwnTLDef::allowPredecessors(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges,
                              const std::vector<int>& fromLanes, const std::vector<int>& toLanes) {
    // also allow predecessors of chosen edges if the lanes match and there is no conflict
    // (must be done after the followers are done because followers are less specific)
    bool check = true;
    while (check) {
        check = false;
        for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
            if (state[i1] == 'G') {
                continue;
            }
            if (forbidden(state, i1, fromEdges, toEdges, false)) {
                continue;
            }
            bool preceedsChosen = false;
            for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                if (state[i2] == 'G' && fromEdges[i2] == toEdges[i1]
                        && fromLanes[i2] == toLanes[i1]) {
                    preceedsChosen = true;
                    break;
                }
            }
            if (preceedsChosen) {
                state[i1] = 'G';
                check = true;
            }
        }
    }
    return state;
}


std::string
NBOwnTLDef::allowUnrelated(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges,
                           const std::vector<bool>& isTurnaround,
                           const std::vector<NBNode::Crossing*>& crossings) {
    for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
        if (state[i1] == 'G') {
            continue;
        }
        bool isForbidden = false;
        for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
            if (state[i2] == 'G' && !isTurnaround[i2] &&
                    (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true) || forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                isForbidden = true;
                break;
            }
        }
        if (!isForbidden && !hasCrossing(fromEdges[i1], toEdges[i1], crossings)) {
            state[i1] = 'G';
        }
    }
    return state;
}


std::string
NBOwnTLDef::allowByVClass(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges, SVCPermissions perm) {
    for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
        SVCPermissions linkPerm = (fromEdges[i1]->getPermissions() & toEdges[i1]->getPermissions());
        if ((linkPerm & ~perm) == 0) {
            state[i1] = 'G';
        }
    }
    return state;
}


bool
NBOwnTLDef::forbidden(const std::string& state, int index, const EdgeVector& fromEdges, const EdgeVector& toEdges, bool allowCont) {
    for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
        if (state[i2] == 'G' && foes(fromEdges[i2], toEdges[i2], fromEdges[index], toEdges[index])) {
            if (!allowCont || (
                        !needsCont(fromEdges[i2], toEdges[i2], fromEdges[index], toEdges[index]) &&
                        !needsCont(fromEdges[index], toEdges[index], fromEdges[i2], toEdges[i2]))) {
                return true;
            }
        }
    }
    return false;
}


std::string
NBOwnTLDef::correctConflicting(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges,
                               const std::vector<bool>& isTurnaround,
                               const std::vector<int>& fromLanes,
                               const std::vector<int>& toLanes,
                               const std::vector<bool>& hadGreenMajor,
                               bool& haveForbiddenLeftMover,
                               std::vector<bool>& rightTurnConflicts,
                               std::vector<bool>& mergeConflicts) {
    const bool controlledWithin = !OptionsCont::getOptions().getBool("tls.uncontrolled-within");
    for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
        if (state[i1] == 'G') {
            for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                if ((state[i2] == 'G' || state[i2] == 'g')) {
                    if (NBNode::rightTurnConflict(
                                fromEdges[i1], toEdges[i1], fromLanes[i1], fromEdges[i2], toEdges[i2], fromLanes[i2])) {
                        rightTurnConflicts[i1] = true;
                    }
                    if (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true, controlledWithin) || rightTurnConflicts[i1]) {
                        state[i1] = 'g';
                        if (myControlledNodes.size() == 1) {
                            myNeedsContRelation.insert(StreamPair(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2]));
#ifdef DEBUG_CONTRELATION
                            if (DEBUGCOND) {
                                std::cout << getID() << " p=" << getProgramID() << " contRel: " << fromEdges[i1]->getID() << "->" << toEdges[i1]->getID()
                                          << " foe " << fromEdges[i2]->getID() << "->" << toEdges[i2]->getID() << "\n";
                            }
#endif
                        }
                        if (!isTurnaround[i1] && !hadGreenMajor[i1] && !rightTurnConflicts[i1]) {
                            haveForbiddenLeftMover = true;
                        }
                    } else if (fromEdges[i1] == fromEdges[i2]
                               && fromLanes[i1] != fromLanes[i2]
                               && toEdges[i1] == toEdges[i2]
                               && toLanes[i1] == toLanes[i2]
                               && fromEdges[i1]->getToNode()->mergeConflictYields(fromEdges[i1], fromLanes[i1], fromLanes[i2], toEdges[i1], toLanes[i1])) {
                        mergeConflicts[i1] = true;
                        state[i1] = 'g';
                    }
                }
            }
        }
        if (state[i1] == 'r') {
            if (fromEdges[i1]->getToNode()->getType() == SumoXMLNodeType::TRAFFIC_LIGHT_RIGHT_ON_RED &&
                    fromEdges[i1]->getToNode()->getDirection(fromEdges[i1], toEdges[i1]) == LinkDirection::RIGHT) {
                state[i1] = 's';
                // do not allow right-on-red when in conflict with exclusive left-turn phase
                for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                    if (state[i2] == 'G' && !isTurnaround[i2] &&
                            (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true) ||
                             forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                        const LinkDirection foeDir = fromEdges[i2]->getToNode()->getDirection(fromEdges[i2], toEdges[i2]);
                        if (foeDir == LinkDirection::LEFT || foeDir == LinkDirection::PARTLEFT) {
                            state[i1] = 'r';
                            break;
                        }
                    }
                }
                if (state[i1] == 's') {
                    // handle right-on-red conflicts
                    for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                        if (state[i2] == 'G' && !isTurnaround[i2] &&
                                (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true) ||
                                 forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                            myRightOnRedConflicts.insert(std::make_pair(i1, i2));
                        }
                    }
                }
            }
        }
    }
    return state;
}


std::string
NBOwnTLDef::correctMixed(std::string state, const EdgeVector& fromEdges,
                         const std::vector<int>& fromLanes,
                         bool& buildMixedGreenPhase, std::vector<bool>& mixedGreen) {
    for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
        if ((state[i1] == 'G' || state[i1] == 'g')) {
            for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                if (i1 != i2 && fromEdges[i1] == fromEdges[i2] && fromLanes[i1] == fromLanes[i2]
                        && state[i2] != 'G' && state[i2] != 'g') {
                    state[i1] = state[i2];
                    //std::cout << " mixedGreen i1=" << i1 << " i2=" << i2 << "\n";
                    mixedGreen[i1] = true;
                    if (fromEdges[i1]->getNumLanesThatAllow(SVC_PASSENGER) > 1) {
                        buildMixedGreenPhase = true;
                    }
                }
            }
        }
    }
    return state;
}


void
NBOwnTLDef::addGreenWithin(NBTrafficLightLogic* logic, const EdgeVector& fromEdges, EdgeVector& toProc) {
    std::vector<bool> foundGreen(fromEdges.size(), false);
    for (const auto& phase : logic->getPhases()) {
        const std::string state = phase.state;
        for (int j = 0; j < (int)fromEdges.size(); j++) {
            LinkState ls = (LinkState)state[j];
            if (ls == LINKSTATE_TL_GREEN_MAJOR || ls == LINKSTATE_TL_GREEN_MINOR) {
                foundGreen[j] = true;
            }
        }
    }
    for (int j = 0; j < (int)foundGreen.size(); j++) {
        if (!foundGreen[j]) {
            NBEdge* e = fromEdges[j];
            if (std::find(toProc.begin(), toProc.end(), e) == toProc.end()) {
                toProc.push_back(e);
            }
        }
    }
}


void
NBOwnTLDef::addPedestrianScramble(NBTrafficLightLogic* logic, int totalNumLinks, SUMOTime /* greenTime */, SUMOTime brakingTime,
                                  const std::vector<NBNode::Crossing*>& crossings, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    const int vehLinks = totalNumLinks - (int)crossings.size();
    std::vector<bool> foundGreen(crossings.size(), false);
    const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = logic->getPhases();
    for (int i = 0; i < (int)phases.size(); i++) {
        const std::string state = phases[i].state;
        for (int j = 0; j < (int)crossings.size(); j++) {
            LinkState ls = (LinkState)state[vehLinks + j];
            if (ls == LINKSTATE_TL_GREEN_MAJOR || ls == LINKSTATE_TL_GREEN_MINOR) {
                foundGreen[j] = true;
            }
        }
    }
#ifdef DEBUG_PHASES
    if (DEBUGCOND2(logic)) {
        std::cout << " foundCrossingGreen=" << toString(foundGreen) << "\n";
    }
#endif
    for (int j = 0; j < (int)foundGreen.size(); j++) {
        if (!foundGreen[j]) {
            // add a phase where all pedestrians may walk, (preceded by a yellow phase and followed by a clearing phase)
            if (phases.size() > 0) {
                bool needYellowPhase = false;
                std::string state = phases.back().state;
                for (int i1 = 0; i1 < vehLinks; ++i1) {
                    if (state[i1] == 'G' || state[i1] == 'g') {
                        state[i1] = 'y';
                        needYellowPhase = true;
                    }
                }
                // add yellow step
                if (needYellowPhase && brakingTime > 0) {
                    logic->addStep(brakingTime, state);
                }
            }
            const SUMOTime pedClearingTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.crossing-clearance.time"));
            const SUMOTime scrambleTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.scramble.time"));
            addPedestrianPhases(logic, scrambleTime + pedClearingTime, UNSPECIFIED_DURATION,
                                UNSPECIFIED_DURATION, UNSPECIFIED_DURATION, UNSPECIFIED_DURATION, std::string(totalNumLinks, 'r'), crossings, fromEdges, toEdges);
            break;
        }
    }
}


void
NBOwnTLDef::buildAllRedState(SUMOTime allRedTime, NBTrafficLightLogic* logic, const std::string& state) {
    if (allRedTime > 0) {
        // build all-red phase
        std::string allRedState = state;
        for (int i = 0; i < (int)state.size(); i++) {
            if (allRedState[i] == 'Y' || allRedState[i] == 'y') {
                allRedState[i] = 'r';
            }
        }
        logic->addStep(TIME2STEPS(ceil(STEPS2TIME(allRedTime))), allRedState);
    }
}


void
NBOwnTLDef::checkCustomCrossingIndices(NBTrafficLightLogic* logic) const {
    int minCustomIndex = -1;
    int maxCustomIndex = -1;
    // collect crossings
    for (std::vector<NBNode*>::const_iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
        const std::vector<NBNode::Crossing*>& c = (*i)->getCrossings();
        for (auto crossing : c) {
            minCustomIndex = MIN2(minCustomIndex, crossing->customTLIndex);
            minCustomIndex = MIN2(minCustomIndex, crossing->customTLIndex2);
            maxCustomIndex = MAX2(maxCustomIndex, crossing->customTLIndex);
            maxCustomIndex = MAX2(maxCustomIndex, crossing->customTLIndex2);
        }
    }
    // custom crossing linkIndex could lead to longer states. ensure that every index has a state
    if (maxCustomIndex >= logic->getNumLinks()) {
        logic->setStateLength(maxCustomIndex + 1);
    }
    // XXX shorter state vectors are possible as well
    // XXX if the indices are shuffled the guessed crossing states should be shuffled correspondingly
    // XXX initialize the backward index to the same state as the forward index
}

void
NBOwnTLDef::fixSuperfluousYellow(NBTrafficLightLogic* logic) const {
    // assume that yellow states last at most one phase
    const int n = logic->getNumLinks();
    const int p = (int)logic->getPhases().size();
    for (int i1 = 0; i1 < n; ++i1) {
        LinkState prev = (LinkState)logic->getPhases().back().state[i1];
        for (int i2 = 0; i2 < p; ++i2) {
            LinkState cur = (LinkState)logic->getPhases()[i2].state[i1];
            LinkState next = (LinkState)logic->getPhases()[(i2 + 1) % p].state[i1];
            if (cur == LINKSTATE_TL_YELLOW_MINOR
                    && (prev == LINKSTATE_TL_GREEN_MAJOR || prev == LINKSTATE_TL_YELLOW_MINOR)
                    && next == LINKSTATE_TL_GREEN_MAJOR) {
                logic->setPhaseState(i2, i1, prev);
            }
            prev = cur;
        }
    }
}


void
NBOwnTLDef::deactivateAlwaysGreen(NBTrafficLightLogic* logic) const {
    const int n = logic->getNumLinks();
    std::vector<bool> alwaysGreen(n, true);
    for (int i1 = 0; i1 < n; ++i1) {
        for (const auto& phase : logic->getPhases()) {
            if (phase.state[i1] != 'G') {
                alwaysGreen[i1] = false;
                break;
            }
        }
    }
    const int p = (int)logic->getPhases().size();
    for (int i1 = 0; i1 < n; ++i1) {
        if (alwaysGreen[i1]) {
            for (int i2 = 0; i2 < p; ++i2) {
                logic->setPhaseState(i2, i1, LINKSTATE_TL_OFF_NOSIGNAL);
            }
        }
    }
}


void
NBOwnTLDef::deactivateInsideEdges(NBTrafficLightLogic* logic, const EdgeVector& fromEdges) const {
    const int n = (int)fromEdges.size();
    const int p = (int)logic->getPhases().size();
    for (int i1 = 0; i1 < n; ++i1) {
        if (fromEdges[i1]->isInsideTLS()) {
            for (int i2 = 0; i2 < p; ++i2) {
                logic->setPhaseState(i2, i1, LINKSTATE_TL_OFF_NOSIGNAL);
            }
        }
    }
}


SUMOTime
NBOwnTLDef::computeEscapeTime(const std::string& state, const EdgeVector& fromEdges, const EdgeVector& toEdges) const {
    const int n = (int)fromEdges.size();
    double maxTime = 0;
    for (int i1 = 0; i1 < n; ++i1) {
        if (state[i1] == 'y' && !fromEdges[i1]->isInsideTLS()) {
            for (int i2 = 0; i2 < n; ++i2) {
                if (fromEdges[i2]->isInsideTLS()) {
                    double gapSpeed = (toEdges[i1]->getSpeed() + fromEdges[i2]->getSpeed()) / 2;
                    double time = fromEdges[i1]->getGeometry().back().distanceTo2D(fromEdges[i2]->getGeometry().back()) / gapSpeed;
                    maxTime = MAX2(maxTime, time);
                }
            }
        }
    }
    // give some slack
    return TIME2STEPS(floor(maxTime * 1.2 + 5));
}


int
NBOwnTLDef::getMaxIndex() {
    setParticipantsInformation();
    NBTrafficLightLogic* logic = compute(OptionsCont::getOptions());
    if (logic != nullptr) {
        return logic->getNumLinks() - 1;
    } else {
        return -1;
    }
}


bool
NBOwnTLDef::corridorLike() const {
    if (getID() == DummyID) {
        // avoid infinite recursion
        return true;
    }
    assert(myControlledNodes.size() >= 2);
    NBOwnTLDef dummy(DummyID, myControlledNodes, 0, TrafficLightType::STATIC);
    dummy.setParticipantsInformation();
    NBTrafficLightLogic* tllDummy = dummy.computeLogicAndConts(0, true);
    int greenPhases = 0;
    for (const auto& phase : tllDummy->getPhases()) {
        if (phase.state.find_first_of("gG") != std::string::npos) {
            greenPhases++;
        }
    }
    delete tllDummy;
    for (const auto& controlledNode : myControlledNodes) {
        controlledNode->removeTrafficLight(&dummy);
    }
    return greenPhases <= 2;
}


NBTrafficLightLogic*
NBOwnTLDef::buildNemaPhases(
    const EdgeVector& fromEdges,
    const EdgeVector& toEdges,
    const std::vector<NBNode::Crossing*>& crossings,
    const std::vector<std::pair<NBEdge*, NBEdge*> >& chosenList,
    const std::vector<std::string>& straightStates,
    const std::vector<std::string>& leftStates) {
    if (chosenList.size() != 2) {
        return nullptr;
    }
    const SUMOTime dur = TIME2STEPS(OptionsCont::getOptions().getInt("tls.cycle.time"));
    const SUMOTime vehExt = TIME2STEPS(OptionsCont::getOptions().getInt("tls.nema.vehExt"));
    const SUMOTime yellow = TIME2STEPS(OptionsCont::getOptions().getInt("tls.nema.yellow"));
    const SUMOTime red = TIME2STEPS(OptionsCont::getOptions().getInt("tls.nema.red"));
    const SUMOTime minMinDur = TIME2STEPS(OptionsCont::getOptions().getInt("tls.min-dur"));
    const SUMOTime maxDur = TIME2STEPS(OptionsCont::getOptions().getInt("tls.max-dur"));
    const SUMOTime earliestEnd = UNSPECIFIED_DURATION;
    const SUMOTime latestEnd = UNSPECIFIED_DURATION;

    const int totalNumLinks = (int)straightStates[0].size();
    NBTrafficLightLogic* logic = new NBTrafficLightLogic(getID(), getProgramID(), totalNumLinks, myOffset, myType);
    std::vector<int> ring1({1, 2, 3, 4});
    std::vector<int> ring2({5, 6, 7, 8});
    std::vector<int> barrier1({4, 8});
    std::vector<int> barrier2({2, 6});
    int phaseNameLeft = 1;
    for (int i = 0; i < (int)chosenList.size(); i++) {
        NBEdge* e1 = chosenList[i].first;
        assert(e1 != nullptr);
        NBEdge* e2 = chosenList[i].second;
        if (i < (int)leftStates.size()) {
            std::string left1 = filterState(leftStates[i], fromEdges, e1);
            if (left1 != "") {
                logic->addStep(dur, left1, minMinDur, maxDur, earliestEnd, latestEnd, vehExt, yellow, red, toString(phaseNameLeft));
            }
        }
        if (e2 != nullptr) {
            std::string straight2 = filterState(straightStates[i], fromEdges, e2);
            straight2 = patchNEMAStateForCrossings(straight2, crossings, fromEdges, toEdges, e2, e1);

            logic->addStep(dur, straight2, minMinDur, maxDur, earliestEnd, latestEnd, vehExt, yellow, red, toString(phaseNameLeft + 1));
            if (i < (int)leftStates.size()) {
                std::string left2 = filterState(leftStates[i], fromEdges, e2);
                if (left2 != "") {
                    logic->addStep(dur, left2, minMinDur, maxDur, earliestEnd, latestEnd, vehExt, yellow, red, toString(phaseNameLeft + 4));
                }
            }

        }
        std::string straight1 = filterState(straightStates[i], fromEdges, e1);
        if (straight1 == "") {
            delete logic;
            return nullptr;
        }
        straight1 = patchNEMAStateForCrossings(straight1, crossings, fromEdges, toEdges, e1, e2);
        logic->addStep(dur, straight1, minMinDur, maxDur, earliestEnd, latestEnd, vehExt, yellow, red, toString(phaseNameLeft + 5));
        phaseNameLeft += 2;
    }
    std::map<int, int> names; // nema phase name -> sumo phase index
    for (int i = 0; i < (int)logic->getPhases().size(); i++) {
        names[StringUtils::toInt(logic->getPhases()[i].name)] = i;
    }

    filterMissingNames(ring1, names, false);
    filterMissingNames(ring2, names, false);
    filterMissingNames(barrier1, names, true, 8);
    filterMissingNames(barrier2, names, true, 6);
    if (ring1[0] == 0 && ring1[1] == 0) {
        ring1[1] = 6;
    }
    if (ring1[2] == 0 && ring1[3] == 0) {
        ring1[3] = 8;
    }
    fixDurationSum(logic, names, ring1[0], ring1[1], ring2[0], ring2[1]);
    fixDurationSum(logic, names, ring1[2], ring1[3], ring2[2], ring2[3]);

    logic->setParameter("ring1", joinToString(ring1, ","));
    logic->setParameter("ring2", joinToString(ring2, ","));
    logic->setParameter("barrierPhases", joinToString(barrier1, ","));
    logic->setParameter("barrier2Phases", joinToString(barrier2, ","));
    return logic;
}


std::string
NBOwnTLDef::filterState(std::string state, const EdgeVector& fromEdges, const NBEdge* e) {
    bool haveGreen = false;
    for (int j = 0; j < (int)fromEdges.size(); j++) {
        if (fromEdges[j] != e) {
            state[j] = 'r';
        } else if (state[j] != 'r') {
            haveGreen = true;
        }
    }
    if (haveGreen) {
        return state;
    } else {
        return "";
    }
}

void
NBOwnTLDef::filterMissingNames(std::vector<int>& vec, const std::map<int, int>& names, bool isBarrier, int barrierDefault) {
    for (int i = 0; i < (int)vec.size(); i++) {
        if (names.count(vec[i]) == 0) {
            if (isBarrier) {
                if (names.count(vec[i] - 1) > 0) {
                    vec[i] = vec[i] - 1;
                } else {
                    vec[i] = barrierDefault;
                }
            } else {
                vec[i] = 0;
            }
        }
    }
}

void
NBOwnTLDef::fixDurationSum(NBTrafficLightLogic* logic, const std::map<int, int>& names, int ring1a, int ring1b, int ring2a, int ring2b) {
    std::set<int> ring1existing;
    std::set<int> ring2existing;
    if (names.count(ring1a) != 0) {
        ring1existing.insert(ring1a);
    }
    if (names.count(ring1b) != 0) {
        ring1existing.insert(ring1b);
    }
    if (names.count(ring2a) != 0) {
        ring2existing.insert(ring2a);
    }
    if (names.count(ring2b) != 0) {
        ring2existing.insert(ring2b);
    }
    if (ring1existing.size() > 0 && ring2existing.size() > 0 &&
            ring1existing.size() != ring2existing.size()) {
        int pI; // sumo phase index
        if (ring1existing.size() < ring2existing.size()) {
            pI = names.find(*ring1existing.begin())->second;
        } else {
            pI = names.find(*ring2existing.begin())->second;
        }
        const auto& p = logic->getPhases()[pI];
        SUMOTime newMaxDur = 2 * p.maxDur + p.yellow + p.red;
        logic->setPhaseMaxDuration(pI, newMaxDur);
    }
}

/****************************************************************************/
