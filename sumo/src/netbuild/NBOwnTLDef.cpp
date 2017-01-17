/****************************************************************************/
/// @file    NBOwnTLDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A traffic light logics which must be computed (only nodes/edges are given)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#define MIN_GREEN_TIME 5

// ===========================================================================
// member method definitions
// ===========================================================================
NBOwnTLDef::NBOwnTLDef(const std::string& id,
                       const std::vector<NBNode*>& junctions, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, junctions, DefaultProgramID, offset, type),
    myHaveSinglePhase(false) {
}


NBOwnTLDef::NBOwnTLDef(const std::string& id, NBNode* junction, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, junction, DefaultProgramID, offset, type),
    myHaveSinglePhase(false) {
}


NBOwnTLDef::NBOwnTLDef(const std::string& id, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, DefaultProgramID, offset, type),
    myHaveSinglePhase(false) {
}


NBOwnTLDef::~NBOwnTLDef() {}


int
NBOwnTLDef::getToPrio(const NBEdge* const e) {
    return e->getJunctionPriority(e->getToNode());
}


SUMOReal
NBOwnTLDef::getDirectionalWeight(LinkDirection dir) {
    switch (dir) {
        case LINKDIR_STRAIGHT:
        case LINKDIR_PARTLEFT:
        case LINKDIR_PARTRIGHT:
            return 2.;
        case LINKDIR_LEFT:
        case LINKDIR_RIGHT:
            return .5;
        default:
            break;
    }
    return 0;
}

SUMOReal
NBOwnTLDef::computeUnblockedWeightedStreamNumber(const NBEdge* const e1, const NBEdge* const e2) {
    SUMOReal val = 0;
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
                    if (!forbids(e1, (*e1c).toEdge, e2, (*e2c).toEdge, true)) {
                        val += getDirectionalWeight(e1->getToNode()->getDirection(e1, (*e1c).toEdge));
                        val += getDirectionalWeight(e2->getToNode()->getDirection(e2, (*e2c).toEdge));
                    }
                }
            }
        }
    }
    return val;
}


std::pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestCombination(const EdgeVector& edges) {
    std::pair<NBEdge*, NBEdge*> bestPair(static_cast<NBEdge*>(0), static_cast<NBEdge*>(0));
    SUMOReal bestValue = -1;
    for (EdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        for (EdgeVector::const_iterator j = i + 1; j != edges.end(); ++j) {
            const SUMOReal value = computeUnblockedWeightedStreamNumber(*i, *j);
            if (value > bestValue) {
                bestValue = value;
                bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
            } else if (value == bestValue) {
                const SUMOReal ca = GeomHelper::getMinAngleDiff((*i)->getAngleAtNode((*i)->getToNode()), (*j)->getAngleAtNode((*j)->getToNode()));
                const SUMOReal oa = GeomHelper::getMinAngleDiff(bestPair.first->getAngleAtNode(bestPair.first->getToNode()), bestPair.second->getAngleAtNode(bestPair.second->getToNode()));
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
    return bestPair;
}


std::pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestPair(EdgeVector& incoming) {
    if (incoming.size() == 1) {
        // only one there - return the one
        std::pair<NBEdge*, NBEdge*> ret(*incoming.begin(), static_cast<NBEdge*>(0));
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
    incoming.erase(find(incoming.begin(), incoming.end(), ret.first));
    incoming.erase(find(incoming.begin(), incoming.end(), ret.second));
    return ret;
}

NBTrafficLightLogic*
NBOwnTLDef::myCompute(int brakingTimeSeconds) {
    return computeLogicAndConts(brakingTimeSeconds);
}

NBTrafficLightLogic*
NBOwnTLDef::computeLogicAndConts(int brakingTimeSeconds, bool onlyConts) {
    myNeedsContRelation.clear();
    myRightOnRedConflicts.clear();
    const SUMOTime brakingTime = TIME2STEPS(brakingTimeSeconds);
    const SUMOTime leftTurnTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.left-green.time"));
    // build complete lists first
    const EdgeVector& incoming = getIncomingEdges();
    EdgeVector fromEdges, toEdges;
    std::vector<bool> isTurnaround;
    std::vector<int> fromLanes;
    int noLanesAll = 0;
    int noLinksAll = 0;
    for (int i1 = 0; i1 < (int)incoming.size(); i1++) {
        int noLanes = incoming[i1]->getNumLanes();
        noLanesAll += noLanes;
        for (int i2 = 0; i2 < noLanes; i2++) {
            NBEdge* fromEdge = incoming[i1];
            std::vector<NBEdge::Connection> approached = fromEdge->getConnectionsFromLane(i2);
            noLinksAll += (int) approached.size();
            for (int i3 = 0; i3 < (int)approached.size(); i3++) {
                if (!fromEdge->mayBeTLSControlled(i2, approached[i3].toEdge, approached[i3].toLane)) {
                    --noLinksAll;
                    continue;
                }
                assert(i3 < (int)approached.size());
                NBEdge* toEdge = approached[i3].toEdge;
                fromEdges.push_back(fromEdge);
                fromLanes.push_back((int)i2);
                toEdges.push_back(toEdge);
                if (toEdge != 0) {
                    isTurnaround.push_back(fromEdge->isTurningDirectionAt(toEdge));
                } else {
                    isTurnaround.push_back(true);
                }
            }
        }
    }
    // collect crossings
    std::vector<NBNode::Crossing> crossings;
    for (std::vector<NBNode*>::iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
        const std::vector<NBNode::Crossing>& c = (*i)->getCrossings();
        if (!onlyConts) {
            // set tl indices for crossings
            (*i)->setCrossingTLIndices(getID(), noLinksAll);
        }
        copy(c.begin(), c.end(), std::back_inserter(crossings));
        noLinksAll += (int)c.size();
    }

    NBTrafficLightLogic* logic = new NBTrafficLightLogic(getID(), getProgramID(), noLinksAll, myOffset, myType);
    EdgeVector toProc = getConnectedOuterEdges(incoming);
    const int greenSeconds = OptionsCont::getOptions().getInt("tls.green.time");
    const SUMOTime greenTime = TIME2STEPS(greenSeconds);
    // build all phases
    std::vector<int> greenPhases; // indices of green phases
    std::vector<bool> hadGreenMajor(noLinksAll, false);
    while (toProc.size() > 0) {
        std::pair<NBEdge*, NBEdge*> chosen;
        if (incoming.size() == 2) {
            // if there are only 2 incoming edges we need to decide whether they are a crossing or a "continuation"
            // @node: this heuristic could be extended to also check the number of outgoing edges
            SUMOReal angle = fabs(NBHelpers::relAngle(incoming[0]->getAngleAtNode(incoming[0]->getToNode()), incoming[1]->getAngleAtNode(incoming[1]->getToNode())));
            // angle would be 180 for straight opposing incoming edges
            if (angle < 135) {
                chosen = std::pair<NBEdge*, NBEdge*>(toProc[0], static_cast<NBEdge*>(0));
                toProc.erase(toProc.begin());
            } else {
                chosen = getBestPair(toProc);
            }
        } else {
            chosen = getBestPair(toProc);
        }
        int pos = 0;
        std::string state((int) noLinksAll, 'r');
        //std::cout << " computing " << getID() << " prog=" << getProgramID() << " cho1=" << Named::getIDSecure(chosen.first) << " cho2=" << Named::getIDSecure(chosen.second) << " toProc=" << toString(toProc) << "\n";
        // plain straight movers
        for (int i1 = 0; i1 < (int) incoming.size(); ++i1) {
            NBEdge* fromEdge = incoming[i1];
            const bool inChosen = fromEdge == chosen.first || fromEdge == chosen.second; //chosen.find(fromEdge)!=chosen.end();
            const int numLanes = fromEdge->getNumLanes();
            for (int i2 = 0; i2 < numLanes; i2++) {
                std::vector<NBEdge::Connection> approached = fromEdge->getConnectionsFromLane(i2);
                for (int i3 = 0; i3 < (int)approached.size(); ++i3) {
                    if (!fromEdge->mayBeTLSControlled(i2, approached[i3].toEdge, approached[i3].toLane)) {
                        continue;
                    }
                    if (inChosen) {
                        state[pos] = 'G';
                    } else {
                        state[pos] = 'r';
                    }
                    ++pos;
                }
            }
        }
        //std::cout << " state after plain straight movers=" << state << "\n";
        // correct behaviour for those that are not in chosen, but may drive, though
        state = allowFollowersOfChosen(state, fromEdges, toEdges);
        for (int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] == 'G') {
                continue;
            }
            bool isForbidden = false;
            for (int i2 = 0; i2 < pos && !isForbidden; ++i2) {
                if (state[i2] == 'G' && !isTurnaround[i2] &&
                        (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true) || forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                    isForbidden = true;
                }
            }
            if (!isForbidden && !hasCrossing(fromEdges[i1], toEdges[i1], crossings)) {
                state[i1] = 'G';
            }
        }
        //std::cout << " state after finding additional 'G's=" << state << "\n";
        // correct behaviour for those that have to wait (mainly left-mover)
        bool haveForbiddenLeftMover = false;
        std::vector<bool> rightTurnConflicts(pos, false);
        state = correctConflicting(state, fromEdges, toEdges, isTurnaround, fromLanes, hadGreenMajor, haveForbiddenLeftMover, rightTurnConflicts);
        for (int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] == 'G') {
                hadGreenMajor[i1] = true;
            }
        }
        //std::cout << " state after correcting left movers=" << state << "\n";
        const std::string vehicleState = state; // backup state before pedestrian modifications
        greenPhases.push_back((int)logic->getPhases().size());
        state = addPedestrianPhases(logic, greenTime, state, crossings, fromEdges, toEdges);
        // pedestrians have 'r' from here on
        for (int i1 = pos; i1 < pos + (int)crossings.size(); ++i1) {
            state[i1] = 'r';
        }
        const bool buildLeftGreenPhase = haveForbiddenLeftMover && !myHaveSinglePhase && leftTurnTime > 0;
        if (brakingTime > 0) {
            // build yellow (straight)
            for (int i1 = 0; i1 < pos; ++i1) {
                if (state[i1] != 'G' && state[i1] != 'g') {
                    continue;
                }
                if ((vehicleState[i1] >= 'a' && vehicleState[i1] <= 'z') && buildLeftGreenPhase && !rightTurnConflicts[i1]) {
                    continue;
                }
                state[i1] = 'y';
            }
            // add step
            logic->addStep(brakingTime, state);
        }

        if (buildLeftGreenPhase) {
            // build left green
            for (int i1 = 0; i1 < pos; ++i1) {
                if (state[i1] == 'Y' || state[i1] == 'y') {
                    state[i1] = 'r';
                    continue;
                }
                if (state[i1] == 'g') {
                    state[i1] = 'G';
                }
            }
            state = allowFollowersOfChosen(state, fromEdges, toEdges);
            state = correctConflicting(state, fromEdges, toEdges, isTurnaround, fromLanes, hadGreenMajor, haveForbiddenLeftMover, rightTurnConflicts);

            // add step
            logic->addStep(leftTurnTime, state);

            // build left yellow
            if (brakingTime > 0) {
                for (int i1 = 0; i1 < pos; ++i1) {
                    if (state[i1] != 'G' && state[i1] != 'g') {
                        continue;
                    }
                    state[i1] = 'y';
                }
                // add step
                logic->addStep(brakingTime, state);
            }
        }
    }
    // fix pedestrian crossings that did not get the green light yet
    if (crossings.size() > 0) {
        addPedestrianScramble(logic, noLinksAll, TIME2STEPS(10), brakingTime, crossings, fromEdges, toEdges);
    }

    SUMOTime totalDuration = logic->getDuration();
    if (OptionsCont::getOptions().isDefault("tls.green.time") || !OptionsCont::getOptions().isDefault("tls.cycle.time")) {
        const SUMOTime cycleTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.cycle.time"));
        // adapt to cycle time by changing the duration of the green phases
        SUMOTime greenPhaseTime = 0;
        SUMOTime minGreenDuration = SUMOTime_MAX;
        for (std::vector<int>::const_iterator it = greenPhases.begin(); it != greenPhases.end(); ++it) {
            const SUMOTime dur = logic->getPhases()[*it].duration;
            greenPhaseTime += dur;
            minGreenDuration = MIN2(minGreenDuration, dur);
        }
        const int patchSeconds = (int)(STEPS2TIME(cycleTime - totalDuration) / greenPhases.size());
        const int patchSecondsRest = (int)(STEPS2TIME(cycleTime - totalDuration)) - patchSeconds * (int)greenPhases.size();
        //std::cout << "cT=" << cycleTime << " td=" << totalDuration << " pS=" << patchSeconds << " pSR=" << patchSecondsRest << "\n";
        if (STEPS2TIME(minGreenDuration) + patchSeconds < MIN_GREEN_TIME
                || STEPS2TIME(minGreenDuration) + patchSeconds + patchSecondsRest < MIN_GREEN_TIME
                || greenPhases.size() == 0) {
            if (getID() != DummyID) {
                WRITE_WARNING("The traffic light '" + getID() + "' cannot be adapted to a cycle time of " + time2string(cycleTime) + ".");
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

    myRightOnRedConflictsReady = true;
    // this computation only makes sense for single nodes
    myNeedsContRelationReady = (myControlledNodes.size() == 1);
    if (totalDuration > 0) {
        if (totalDuration > 3 * (greenTime + 2 * brakingTime + leftTurnTime)) {
            WRITE_WARNING("The traffic light '" + getID() + "' has a high cycle time of " + time2string(totalDuration) + ".");
        }
        logic->closeBuilding();
        return logic;
    } else {
        delete logic;
        return 0;
    }
}


bool
NBOwnTLDef::hasCrossing(const NBEdge* from, const NBEdge* to, const std::vector<NBNode::Crossing>& crossings) {
    assert(to != 0);
    for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
        const NBNode::Crossing& cross = *it;
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
NBOwnTLDef::addPedestrianPhases(NBTrafficLightLogic* logic, SUMOTime greenTime,
                                std::string state, const std::vector<NBNode::Crossing>& crossings, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    const SUMOTime pedClearingTime = TIME2STEPS(5); // compute based on length of the crossing
    const SUMOTime minPedTime = TIME2STEPS(4); // compute: must be able to reach the middle of the second "Richtungsfahrbahn"
    const std::string orig = state;
    state = patchStateForCrossings(state, crossings, fromEdges, toEdges);
    if (orig == state) {
        // add step
        logic->addStep(greenTime, state);
    } else {
        const SUMOTime pedTime = greenTime - pedClearingTime;
        if (pedTime >= minPedTime) {
            // ensure clearing time for pedestrians
            const int pedStates = (int)crossings.size();
            logic->addStep(pedTime, state);
            state = state.substr(0, state.size() - pedStates) + std::string(pedStates, 'r');
            logic->addStep(pedClearingTime, state);
        } else {
            state = orig;
            // not safe for pedestrians.
            logic->addStep(greenTime, state);
        }
    }
    return state;
}


std::string
NBOwnTLDef::patchStateForCrossings(const std::string& state, const std::vector<NBNode::Crossing>& crossings, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    std::string result = state;
    const int pos = (int)(state.size() - crossings.size()); // number of controlled vehicle links
    for (int ic = 0; ic < (int)crossings.size(); ++ic) {
        const int i1 = pos + ic;
        const NBNode::Crossing& cross = crossings[ic];
        bool isForbidden = false;
        for (int i2 = 0; i2 < pos && !isForbidden; ++i2) {
            // only check connections at this crossings node
            if (fromEdges[i2] != 0 && toEdges[i2] != 0 && fromEdges[i2]->getToNode() == cross.node) {
                for (EdgeVector::const_iterator it = cross.edges.begin(); it != cross.edges.end(); ++it) {
                    const NBEdge* edge = *it;
                    const LinkDirection i2dir = cross.node->getDirection(fromEdges[i2], toEdges[i2]);
                    if (state[i2] != 'r' && (edge == fromEdges[i2] ||
                                             (edge == toEdges[i2] && (i2dir == LINKDIR_STRAIGHT || i2dir == LINKDIR_PARTLEFT || i2dir == LINKDIR_PARTRIGHT)))) {
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
                const NBNode::Crossing& crossing = crossings[ic];
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


void
NBOwnTLDef::collectNodes() {}


void
NBOwnTLDef::collectLinks() {
    collectAllLinks();
}


void
NBOwnTLDef::setParticipantsInformation() {
    // assign participating nodes to the request
    collectNodes();
    // collect the information about participating edges and links
    collectEdges();
    collectLinks();
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
                           NBEdge* /*by*/, int /*byLane*/) {}


void
NBOwnTLDef::initNeedsContRelation() const {
    if (!myNeedsContRelationReady) {
        if (myControlledNodes.size() > 0) {
            // we use a dummy node just to maintain const-correctness
            myNeedsContRelation.clear();
            NBOwnTLDef dummy(DummyID, myControlledNodes, 0, TLTYPE_STATIC);
            dummy.setParticipantsInformation();
            NBTrafficLightLogic* tllDummy = dummy.computeLogicAndConts(0, true);
            delete tllDummy;
            myNeedsContRelation = dummy.myNeedsContRelation;
            for (std::vector<NBNode*>::const_iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
                (*i)->removeTrafficLight(&dummy);
            }
        }
        myNeedsContRelationReady = true;
    }
}


EdgeVector
NBOwnTLDef::getConnectedOuterEdges(const EdgeVector& incoming) {
    EdgeVector result = incoming;
    for (EdgeVector::iterator it = result.begin(); it != result.end();) {
        if ((*it)->getConnections().size() == 0 || (*it)->isInnerEdge()) {
            it = result.erase(it);
        } else {
            ++it;
        }
    }
    return result;
}


std::string
NBOwnTLDef::allowFollowersOfChosen(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    bool check = true;
    while (check) {
        check = false;
        for (int i1 = 0; i1 < (int)fromEdges.size(); ++i1) {
            if (state[i1] == 'G') {
                continue;
            }
            bool followsChosen = false;
            for (int i2 = 0; i2 < (int)fromEdges.size() && !followsChosen; ++i2) {
                if (state[i2] == 'G' && fromEdges[i1] == toEdges[i2]) {
                    followsChosen = true;
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
NBOwnTLDef::correctConflicting(std::string state, const EdgeVector& fromEdges, const EdgeVector& toEdges,
                               const std::vector<bool>& isTurnaround,
                               const std::vector<int>& fromLanes,
                               const std::vector<bool>& hadGreenMajor,
                               bool& haveForbiddenLeftMover,
                               std::vector<bool>& rightTurnConflicts) {
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
                        myNeedsContRelation.insert(StreamPair(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2]));
                        if (!isTurnaround[i1] && !hadGreenMajor[i1]) {
                            haveForbiddenLeftMover = true;
                        }
                    }
                }
            }
        }
        if (state[i1] == 'r') {
            if (fromEdges[i1]->getToNode()->getType() == NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED &&
                    fromEdges[i1]->getToNode()->getDirection(fromEdges[i1], toEdges[i1]) == LINKDIR_RIGHT) {
                // handle right-on-red conflicts
                state[i1] = 's';
                for (int i2 = 0; i2 < (int)fromEdges.size(); ++i2) {
                    if (state[i2] == 'G' && !isTurnaround[i2] &&
                            (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true) || forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                        myRightOnRedConflicts.insert(std::make_pair(i1, i2));
                    }
                }
            }
        }
    }
    return state;
}


void
NBOwnTLDef::addPedestrianScramble(NBTrafficLightLogic* logic, int noLinksAll, SUMOTime /* greenTime */, SUMOTime brakingTime,
                                  const std::vector<NBNode::Crossing>& crossings, const EdgeVector& fromEdges, const EdgeVector& toEdges) {
    const int vehLinks = noLinksAll - (int)crossings.size();
    std::vector<bool> foundGreen(crossings.size(), false);
    const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = logic->getPhases();
    for (int i = 0; i < (int)phases.size(); ++i) {
        const std::string state = phases[i].state;
        for (int j = 0; j < (int)crossings.size(); ++j) {
            LinkState ls = (LinkState)state[vehLinks + j];
            if (ls == LINKSTATE_TL_GREEN_MAJOR || ls == LINKSTATE_TL_GREEN_MINOR) {
                foundGreen[j] = true;
            }
        }
    }
    for (int j = 0; j < (int)foundGreen.size(); ++j) {
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
            addPedestrianPhases(logic, TIME2STEPS(10), std::string(noLinksAll, 'r'), crossings, fromEdges, toEdges);
            break;
        }
    }
}

/****************************************************************************/
