/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
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
/// @file    NBAlgorithms.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    02. March 2012
///
// Algorithms for network computation
/****************************************************************************/
#include <config.h>

#include <sstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "NBEdge.h"
#include "NBOwnTLDef.h"
#include "NBTrafficLightLogicCont.h"
#include "NBNodeCont.h"
#include "NBTypeCont.h"
#include "NBNode.h"
#include "NBAlgorithms.h"


//#define DEBUG_SETPRIORITIES
//#define DEBUG_TURNAROUNDS
#define DEBUGCOND (n.getID() == "C")
//#define DEBUGCOND2(obj) (obj->getID() == "")
#define DEBUGCOND2(obj) (true)

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBTurningDirectionsComputer
// ---------------------------------------------------------------------------
void
NBTurningDirectionsComputer::computeTurnDirections(NBNodeCont& nc, bool warn) {
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        computeTurnDirectionsForNode(i->second, warn);
    }
}

void
NBTurningDirectionsComputer::computeTurnDirectionsForNode(NBNode* node, bool warn) {
    const std::vector<NBEdge*>& incoming = node->getIncomingEdges();
    const std::vector<NBEdge*>& outgoing = node->getOutgoingEdges();
    // reset turning directions since this may be called multiple times
    for (std::vector<NBEdge*>::const_iterator k = incoming.begin(); k != incoming.end(); ++k) {
        (*k)->setTurningDestination(nullptr);
    }
    std::vector<Combination> combinations;
    const bool geometryLike = node->geometryLike();
    for (NBEdge* outedge : outgoing) {
        for (NBEdge* e : incoming) {
            // @todo: check whether NBHelpers::relAngle is properly defined and whether it should really be used, here
            const double signedAngle = NBHelpers::normRelAngle(e->getAngleAtNode(node), outedge->getAngleAtNode(node));
            if (signedAngle > 0 && signedAngle < 177 && e->getGeometry().back().distanceTo2D(outedge->getGeometry().front()) < POSITION_EPS) {
                // backwards curving edges can only be turnaround when there are
                // non-default endpoints
#ifdef DEBUG_TURNAROUNDS
                if (DEBUGCOND2(node)) {
                    std::cout << "incoming=" << e->getID() << " outgoing=" << outedge->getID() << " signedAngle=" << signedAngle << " skipped\n";
                }
#endif
                continue;
            }
            double angle = fabs(signedAngle);
#ifdef DEBUG_TURNAROUNDS
            if (DEBUGCOND2(node)) {
                std::cout << "incoming=" << e->getID() << " outgoing=" << outedge->getID() << " relAngle=" << NBHelpers::relAngle(e->getAngleAtNode(node), outedge->getAngleAtNode(node)) << "\n";
            }
#endif
            const bool badPermissions = ((outedge->getPermissions() & e->getPermissions() & ~SVC_PEDESTRIAN) == 0
                                         && !geometryLike
                                         && outedge->getPermissions() != e->getPermissions());
            if (e->getFromNode() == outedge->getToNode()
                    && (angle > 120 || e->getFromNode()->getPosition() == e->getToNode()->getPosition())
                    && !badPermissions) {
                // they connect the same nodes; should be the turnaround direction
                // we'll assign a maximum number
                //
                // @todo: indeed, we have observed some pathological intersections
                //  see "294831560" in OSM/adlershof. Here, several edges are connecting
                //  same nodes. We have to do the angle check before...
                //
                // @todo: and well, there are some other as well, see plain import
                //  of delphi_muenchen (elmar), intersection "59534191". Not that it would
                //  be realistic in any means; we will warn, here.
                angle += 360;
            }
            if (angle < 160) {
                if (angle > 135) {
                    // could be a turnaround with a green median island, look at
                    // angle further away from the junction
                    const double inFA = getFarAngleAtNode(e, node);
                    const double outFA = getFarAngleAtNode(outedge, node);
                    const double signedFarAngle = NBHelpers::normRelAngle(inFA, outFA);
#ifdef DEBUG_TURNAROUNDS
                    if (DEBUGCOND2(node)) {
                        std::cout << "    inFA=" << inFA << " outFA=" << outFA << " sFA=" << signedFarAngle << "\n";
                    }
#endif
                    if (signedFarAngle > -160) {
                        continue;
                    }
                } else {
                    continue;
                }
            }
            if (badPermissions) {
                // penalty
                angle -= 90;
            }
            Combination c;
            c.from = e;
            c.to = outedge;
            c.angle = angle;
            combinations.push_back(c);
        }
    }
    // sort combinations so that the ones with the highest angle are at the begin
    std::sort(combinations.begin(), combinations.end(), combination_by_angle_sorter());
    std::set<NBEdge*> seen;
#ifdef DEBUG_TURNAROUNDS
    if (DEBUGCOND2(node)) {
        std::cout << "check combinations at " << node->getID() << "\n";
    }
#endif
    for (std::vector<Combination>::const_iterator j = combinations.begin(); j != combinations.end(); ++j) {
#ifdef DEBUG_TURNAROUNDS
        if (DEBUGCOND2(node)) {
            std::cout << " from=" << (*j).from->getID() << " to=" << (*j).to->getID() << " a=" << (*j).angle << "\n";
        }
#endif
        if (seen.find((*j).from) != seen.end() || seen.find((*j).to) != seen.end()) {
            // do not regard already set edges
            if ((*j).angle > 360 && warn) {
                WRITE_WARNINGF(TL("Ambiguity in turnarounds computation at junction '%'."), node->getID());
                //std::cout << "  already seen: " << toString(seen) << "\n";
                warn = false;
            }
            continue;
        }
        // mark as seen
        seen.insert((*j).from);
        seen.insert((*j).to);
        // set turnaround information
        bool onlyPossible = (*j).from->getConnections().size() != 0 && !(*j).from->isConnectedTo((*j).to);
#ifdef DEBUG_TURNAROUNDS
        if (DEBUGCOND2(node)) {
            std::cout << "    setTurningDestination from=" << (*j).from->getID() << " to=" << (*j).to->getID() << " onlyPossible=" << onlyPossible << "\n";
        }
#endif
        (*j).from->setTurningDestination((*j).to, onlyPossible);
    }
}


double
NBTurningDirectionsComputer::getFarAngleAtNode(const NBEdge* e, const NBNode* n, double dist) {
    Position atNode;
    Position far;
    double angle;
    const NBEdge* next = e;
    if (e->getToNode() == n) {
        // search upstream
        atNode = e->getGeometry().back();
        while (dist > 0) {
            const double length = next->getGeometry().length();
            if (dist <= length) {
                far = next->getGeometry().positionAtOffset(length - dist);
                break;
            } else {
                far = next->getGeometry().front();
                dist -= length;
                if (next->getToNode()->getIncomingEdges().size() == 1) {
                    next = next->getToNode()->getIncomingEdges().front();
                } else {
                    break;
                }
            }
        }
        angle = far.angleTo2D(atNode);
        //std::cout << " e=" << e->getID() << " n=" << n->getID() << " far=" << far << " atNode=" << atNode << " a=" << RAD2DEG(angle) << "\n";
    } else {
        // search downstream
        atNode = e->getGeometry().front();
        while (dist > 0) {
            const double length = next->getGeometry().length();
            if (dist <= length) {
                far = next->getGeometry().positionAtOffset(dist);
                break;
            } else {
                far = next->getGeometry().back();
                dist -= length;
                if (next->getToNode()->getOutgoingEdges().size() == 1) {
                    next = next->getToNode()->getOutgoingEdges().front();
                } else {
                    break;
                }
            }
        }
        angle = atNode.angleTo2D(far);
        //std::cout << " e=" << e->getID() << " n=" << n->getID() << " atNode=" << atNode << " far=" << far << " a=" << RAD2DEG(angle) << "\n";
    }
    return GeomHelper::legacyDegree(angle);
}


// ---------------------------------------------------------------------------
// NBNodesEdgesSorter
// ---------------------------------------------------------------------------
void
NBNodesEdgesSorter::sortNodesEdges(NBNodeCont& nc, bool useNodeShape) {
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        i->second->sortEdges(useNodeShape);
    }
}


void
NBNodesEdgesSorter::swapWhenReversed(const NBNode* const n,
                                     const std::vector<NBEdge*>::iterator& i1,
                                     const std::vector<NBEdge*>::iterator& i2) {
    NBEdge* e1 = *i1;
    NBEdge* e2 = *i2;
    // @todo: The difference between "isTurningDirectionAt" and "isTurnaround"
    //  is not nice. Maybe we could get rid of it if we would always mark edges
    //  as turnarounds, even if they do not have to be added, as mentioned in
    //  notes on NBTurningDirectionsComputer::computeTurnDirectionsForNode
    if (e2->getToNode() == n && e2->isTurningDirectionAt(e1)) {
        std::swap(*i1, *i2);
    }
}


// ---------------------------------------------------------------------------
// NBNodeTypeComputer
// ---------------------------------------------------------------------------
void
NBNodeTypeComputer::computeNodeTypes(NBNodeCont& nc, NBTrafficLightLogicCont& tlc) {
    validateRailCrossings(nc, tlc);
    const OptionsCont& oc = OptionsCont::getOptions();
    const double rightBeforeLeftSpeed = oc.getFloat("junctions.right-before-left.speed-threshold");
    for (const auto& nodeIt : nc) {
        NBNode* const n = nodeIt.second;
        // the type may already be set from the data
        if (n->myType != SumoXMLNodeType::UNKNOWN && n->myType != SumoXMLNodeType::DEAD_END) {
            n->myTypeWasGuessed = false;
            continue;
        }
        // check whether the node was set to be unregulated by the user
        if (oc.getBool("keep-nodes-unregulated") || oc.isInStringVector("keep-nodes-unregulated.explicit", n->getID())
                || (oc.getBool("keep-nodes-unregulated.district-nodes") && (n->isNearDistrict() || n->isDistrict()))) {
            n->myType = SumoXMLNodeType::NOJUNCTION;
            continue;
        }
        // check whether the node is a waterway node. Set to unregulated by default
        bool waterway = true;
        for (NBEdge* e : n->getEdges()) {
            if (!isWaterway(e->getPermissions())) {
                waterway = false;
                break;
            }
        }
        if (waterway && (n->myType == SumoXMLNodeType::UNKNOWN || n->myType == SumoXMLNodeType::DEAD_END)) {
            n->myType = SumoXMLNodeType::NOJUNCTION;
            continue;
        }

        // check whether the junction is not a real junction
        if (n->myIncomingEdges.size() == 1) {
            n->myType = SumoXMLNodeType::PRIORITY;
            continue;
        }
        // @todo "isSimpleContinuation" should be revalidated
        if (n->isSimpleContinuation()) {
            n->myType = SumoXMLNodeType::PRIORITY;
            continue;
        }
        if (isRailwayNode(n)) {
            // priority instead of unregulated to ensure that collisions can be detected
            n->myType = SumoXMLNodeType::PRIORITY;
            continue;
        }
        // determine the type
        SumoXMLNodeType type = oc.getBool("junctions.left-before-right") ? SumoXMLNodeType::LEFT_BEFORE_RIGHT : SumoXMLNodeType::RIGHT_BEFORE_LEFT;
        for (EdgeVector::const_iterator i = n->myIncomingEdges.begin(); i != n->myIncomingEdges.end(); i++) {
            for (EdgeVector::const_iterator j = i + 1; j != n->myIncomingEdges.end(); j++) {
                // @todo "getOppositeIncoming" should probably be refactored into something the edge knows
                if (n->getOppositeIncoming(*j) == *i && n->myIncomingEdges.size() > 2) {
                    continue;
                }
                // @todo check against a legal document
                // @todo figure out when SumoXMLNodeType::PRIORITY_STOP is appropriate
                const double s1 = (*i)->getSpeed();
                const double s2 = (*j)->getSpeed();
                const int p1 = (*i)->getPriority();
                const int p2 = (*j)->getPriority();
                if (fabs(s1 - s2) > (9.5 / 3.6) || MAX2(s1, s2) >= rightBeforeLeftSpeed || p1 != p2) {
                    type = SumoXMLNodeType::PRIORITY;
                    break;
                }
            }
        }
        // save type
        n->myType = type;
        n->myTypeWasGuessed = true;
    }
}


void
NBNodeTypeComputer::validateRailCrossings(NBNodeCont& nc, NBTrafficLightLogicCont& tlc) {
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        if (n->myType == SumoXMLNodeType::RAIL_CROSSING) {
            // check if it really is a rail crossing
            int numRailway = 0;
            int numNonRailIn = 0;
            int numNonRailOut = 0;
            std::set<const NBNode*> nonRailNodes;
            int numNonRailwayNonPed = 0;
            for (NBEdge* e : n->getIncomingEdges()) {
                if ((e->getPermissions() & ~SVC_RAIL_CLASSES) != 0) {
                    numNonRailIn += 1;
                    if (e->getPermissions() != SVC_PEDESTRIAN) {
                        numNonRailwayNonPed++;
                    }
                    nonRailNodes.insert(e->getFromNode());
                } else if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
                    numRailway++;
                }
            }
            for (NBEdge* e : n->getOutgoingEdges()) {
                if ((e->getPermissions() & ~SVC_RAIL_CLASSES) != 0) {
                    numNonRailOut += 1;
                    nonRailNodes.insert(e->getToNode());
                }
            }
            if (numNonRailIn == 0 || numNonRailOut == 0 || numRailway == 0) {
                // not a crossing (maybe unregulated or rail_signal)
                WRITE_WARNINGF(TL("Converting invalid rail_crossing to priority junction '%'."), n->getID());
                n->myType = SumoXMLNodeType::PRIORITY;
            } else if (numNonRailwayNonPed > 2 || nonRailNodes.size() > 2) {
                // does not look like a rail crossing (roads in conflict). maybe a traffic light?
                WRITE_WARNINGF(TL("Converting invalid rail_crossing to traffic_light at junction '%'."), n->getID());
                TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(OptionsCont::getOptions().getString("tls.default-type"));
                NBTrafficLightDefinition* tlDef = new NBOwnTLDef(n->getID(), n, 0, type);
                n->myType = SumoXMLNodeType::TRAFFIC_LIGHT;
                if (!tlc.insert(tlDef)) {
                    // actually, nothing should fail here
                    n->removeTrafficLight(tlDef);
                    n->myType = SumoXMLNodeType::PRIORITY;
                    delete tlDef;
                    WRITE_WARNINGF(TL("Could not allocate tls '%'."), n->getID());
                }
            }
        }
    }
}


bool
NBNodeTypeComputer::isRailwayNode(const NBNode* n) {
    bool hasRailway = false;
    for (NBEdge* e : n->getIncomingEdges()) {
        if ((e->getPermissions() & ~(SVC_RAIL_CLASSES | SVC_TAXI)) != 0) {
            return false;
        } else if ((e->getPermissions() & SVC_RAIL_CLASSES) != 0) {
            hasRailway = true;
        }
    }
    return hasRailway;
}

// ---------------------------------------------------------------------------
// NBEdgePriorityComputer
// ---------------------------------------------------------------------------
void
NBEdgePriorityComputer::computeEdgePriorities(NBNodeCont& nc) {
    for (const auto& node : nc) {
        // preset all junction's edge priorities to zero
        for (NBEdge* const edge : node.second->myAllEdges) {
            edge->setJunctionPriority(node.second, NBEdge::JunctionPriority::MINOR_ROAD);
        }
        node.second->markBentPriority(false);
        // check if the junction is not a real junction
        if (node.second->myIncomingEdges.size() == 1 && node.second->myOutgoingEdges.size() == 1) {
            continue;
        }
        // compute the priorities on junction when needed
        if (node.second->getType() != SumoXMLNodeType::RIGHT_BEFORE_LEFT
                && node.second->getType() != SumoXMLNodeType::LEFT_BEFORE_RIGHT
                && node.second->getType() != SumoXMLNodeType::ALLWAY_STOP
                && node.second->getType() != SumoXMLNodeType::NOJUNCTION) {
            if (node.second->getRightOfWay() == RightOfWay::EDGEPRIORITY) {
                for (NBEdge* e : node.second->getIncomingEdges()) {
                    e->setJunctionPriority(node.second, e->getPriority());
                }
            } else {
                setPriorityJunctionPriorities(*node.second);
            }
        }
    }
}


void
NBEdgePriorityComputer::setPriorityJunctionPriorities(NBNode& n, bool forceStraight) {
    if (n.myIncomingEdges.size() == 0 || n.myOutgoingEdges.size() == 0) {
        return;
    }
    int minPrio = std::numeric_limits<int>::max();
    int maxPrio = -std::numeric_limits<int>::max();
    int maxNumLanes = -std::numeric_limits<int>::max();
    double maxSpeed = -std::numeric_limits<double>::max();
    if (forceStraight) {
        // called a second time, preset all junction's edge priorities to zero
        for (NBEdge* const edge : n.myAllEdges) {
            edge->setJunctionPriority(&n, NBEdge::JunctionPriority::MINOR_ROAD);
            minPrio = MIN2(minPrio, edge->getPriority());
            maxPrio = MAX2(maxPrio, edge->getPriority());
            maxNumLanes = MAX2(maxNumLanes, edge->getNumLanes());
            maxSpeed = MAX2(maxSpeed, edge->getSpeed());
        }
    }
    EdgeVector incoming = n.myIncomingEdges;
    EdgeVector outgoing = n.myOutgoingEdges;
    // what we do want to have is to extract the pair of roads that are
    //  the major roads for this junction
    // let's get the list of incoming edges with the highest priority
    std::sort(incoming.begin(), incoming.end(), NBContHelper::edge_by_priority_sorter());
    EdgeVector bestIncoming;
    NBEdge* bestIn = incoming[0];
    while (incoming.size() > 0 && (forceStraight || samePriority(bestIn, incoming[0]))) {
        bestIncoming.push_back(*incoming.begin());
        incoming.erase(incoming.begin());
    }
    // now, let's get the list of best outgoing
    assert(outgoing.size() != 0);
    sort(outgoing.begin(), outgoing.end(), NBContHelper::edge_by_priority_sorter());
    EdgeVector bestOutgoing;
    const NBEdge* const firstOut = outgoing[0];
    while (outgoing.size() > 0 && (forceStraight || samePriority(firstOut, outgoing[0]))) { //->getPriority()==best->getPriority())
        bestOutgoing.push_back(*outgoing.begin());
        outgoing.erase(outgoing.begin());
    }
    // special case: user input makes mainDirection unambiguous
    const bool mainDirectionExplicit = (
                                           bestIncoming.size() == 1 && n.myIncomingEdges.size() <= 2
                                           && (incoming.size() == 0 || bestIncoming[0]->getPriority() > incoming[0]->getPriority())
                                           && bestOutgoing.size() == 1 && n.myOutgoingEdges.size() <= 2
                                           && (outgoing.size() == 0 || bestOutgoing[0]->getPriority() > outgoing[0]->getPriority())
                                           && !bestIncoming[0]->isTurningDirectionAt(bestOutgoing[0]));
    // now, let's compute for each of the best incoming edges
    //  the incoming which is most opposite
    //  the outgoing which is most opposite
    EdgeVector::iterator i;
    std::map<NBEdge*, NBEdge*> counterIncomingEdges;
    std::map<NBEdge*, NBEdge*> counterOutgoingEdges;
    incoming = n.myIncomingEdges;
    outgoing = n.myOutgoingEdges;
    for (i = bestIncoming.begin(); i != bestIncoming.end(); ++i) {
        std::sort(incoming.begin(), incoming.end(), NBContHelper::edge_opposite_direction_sorter(*i, &n, !forceStraight));
        counterIncomingEdges[*i] = *incoming.begin();
        std::sort(outgoing.begin(), outgoing.end(), NBContHelper::edge_opposite_direction_sorter(*i, &n, !forceStraight));
        counterOutgoingEdges[*i] = *outgoing.begin();
    }
#ifdef DEBUG_SETPRIORITIES
    if (DEBUGCOND) {
        std::map<std::string, std::string> tmp1;
        for (auto item : counterIncomingEdges) {
            tmp1[item.first->getID()] = item.second->getID();
        }
        std::map<std::string, std::string> tmp2;
        for (auto item : counterOutgoingEdges) {
            tmp2[item.first->getID()] = item.second->getID();
        }
        std::cout << "n=" << n.getID() << " bestIn=" << bestIn->getID() << " bestOut=" << toString(bestOutgoing)
                  << " counterBest=" << counterIncomingEdges.find(bestIncoming[0])->second->getID()
                  << " mainExplicit=" << mainDirectionExplicit
                  << " forceStraight=" << forceStraight
                  << "\n  bestIncoming=" << toString(bestIncoming) << " allIncoming=" << toString(incoming)
                  << "\n  bestOutgoing=" << toString(bestOutgoing) << " allOutgoing=" << toString(outgoing)
                  << "\n  counterIncomingEdges=" << toString(tmp1)
                  << "\n  counterOutgoingEdges=" << toString(tmp2)
                  << "\n";
    }
#endif
    // at a tls junction we must prevent an underlying bent-priority layout
    // because that would lead to invalid right-of-way rules for an oncoming
    // tls layout (but not vice versa). See #7764
    const bool hasTLS = n.isTLControlled();
    // ok, let's try
    // 1) there is one best incoming road
    if (bestIncoming.size() == 1) {
        // let's mark this road as the best
        NBEdge* best1 = extractAndMarkFirst(n, bestIncoming);
        if (!mainDirectionExplicit && counterIncomingEdges.find(best1) != counterIncomingEdges.end()) {
            // ok, look, what we want is the opposite of the straight continuation edge
            // but, what if such an edge does not exist? By now, we'll determine it
            // geometrically
            NBEdge* s = counterIncomingEdges.find(best1)->second;
            const double minAngleDiff = GeomHelper::getMinAngleDiff(best1->getAngleAtNode(&n), s->getAngleAtNode(&n));
            if (minAngleDiff > 180 - 45
                    || (minAngleDiff > 75 && s->getPriority() == best1->getPriority() && hasDifferentPriorities(incoming, best1))) {
                s->setJunctionPriority(&n, NBEdge::PRIORITY_ROAD);
            }
        }
        markBestParallel(n, best1, nullptr);
        assert(bestOutgoing.size() != 0);
        // mark the best outgoing as the continuation
        sort(bestOutgoing.begin(), bestOutgoing.end(), NBContHelper::edge_similar_direction_sorter(best1));
        // assign extra priority if the priorities are unambiguous (regardless of geometry)
        NBEdge* bestOut = extractAndMarkFirst(n, bestOutgoing);
        if (!mainDirectionExplicit && counterOutgoingEdges.find(bestOut) != counterOutgoingEdges.end()) {
            NBEdge* s = counterOutgoingEdges.find(bestOut)->second;
            if (GeomHelper::getMinAngleDiff(bestOut->getAngleAtNode(&n), s->getAngleAtNode(&n)) > 180 - 45) {
                s->setJunctionPriority(&n, 1);
            }
        }
        const bool isBent = n.getDirection(best1, bestOut) != LinkDirection::STRAIGHT;
#ifdef DEBUG_SETPRIORITIES
        if (DEBUGCOND) {
            std::cout << "  best1=" << best1->getID() << " bestOut=" << bestOut->getID() << " bestOutgoing=" << toString(bestOutgoing) << " mainDirectionExplicit=" << mainDirectionExplicit << " isBent=" << isBent << "\n";
        }
#endif
        if (isBent && hasTLS && !forceStraight) {
            // redo but force straight computation
            setPriorityJunctionPriorities(n, true);
        } else {
            n.markBentPriority(isBent);
        }
        return;
    }

    // ok, what we want to do in this case is to determine which incoming
    //  has the best continuation...
    // This means, when several incoming roads have the same priority,
    //  we want a (any) straight connection to be more priorised than a turning
    double bestAngle = -1;
    NBEdge* bestFirst = nullptr;
    NBEdge* bestSecond = nullptr;
    for (i = bestIncoming.begin(); i != bestIncoming.end(); ++i) {
        EdgeVector::iterator j;
        NBEdge* t1 = *i;
        double angle1 = t1->getAngleAtNode(&n) + 180;
        if (angle1 >= 360) {
            angle1 -= 360;
        }
        for (j = i + 1; j != bestIncoming.end(); ++j) {
            NBEdge* t2 = *j;
            double angle2 = t2->getAngleAtNode(&n) + 180;
            if (angle2 >= 360) {
                angle2 -= 360;
            }
            double score = forceStraight ? getScore(t1, t2, minPrio, maxPrio, maxNumLanes, maxSpeed) : 0;
            double angle = GeomHelper::getMinAngleDiff(angle1, angle2) + 45 * score;
            if (angle > bestAngle) {
                //if (forceStraight) std::cout << " node=" << n.getID() << " t1=" << t1->getID() << " t2=" << t2->getID() << " angle=" << angle << " bestAngle=" << bestAngle << " score=" << score << " minPrio=" << minPrio << " maxPrio=" << maxPrio << "\n";
                bestAngle = MAX2(angle, bestAngle);
                bestFirst = *i;
                bestSecond = *j;
            }
        }
    }
    bestFirst->setJunctionPriority(&n, 1);
    sort(bestOutgoing.begin(), bestOutgoing.end(), NBContHelper::edge_similar_direction_sorter(bestFirst));
#ifdef DEBUG_SETPRIORITIES
    if (DEBUGCOND) {
        std::cout << "  bestFirst=" << bestFirst->getID() << "  bestOutgoingFirst=" << toString(bestOutgoing) << "\n";
    }
#endif
    if (bestOutgoing.size() != 0) {
        extractAndMarkFirst(n, bestOutgoing);
    }
    bestSecond->setJunctionPriority(&n, 1);
    sort(bestOutgoing.begin(), bestOutgoing.end(), NBContHelper::edge_similar_direction_sorter(bestSecond));
#ifdef DEBUG_SETPRIORITIES
    if (DEBUGCOND) {
        std::cout << "  bestSecond=" << bestSecond->getID() << "  bestOutgoingSecond=" << toString(bestOutgoing) << "\n";
    }
#endif
    if (bestOutgoing.size() != 0) {
        extractAndMarkFirst(n, bestOutgoing);
    }
    const bool isBent = GeomHelper::getMinAngleDiff(bestFirst->getAngleAtNode(&n), bestSecond->getAngleAtNode(&n)) < 135;
    if (isBent && hasTLS && !forceStraight) {
        // redo but force straight computation
        setPriorityJunctionPriorities(n, true);
    } else {
        n.markBentPriority(isBent);
        markBestParallel(n, bestFirst, bestSecond);
    }
}

double
NBEdgePriorityComputer::getScore(const NBEdge* e1, const NBEdge* e2, int minPrio, int maxPrio, int maxNumLanes, double maxSpeed) {
    // normalize priorities to [0.1,1]
    double normPrio1 = 1;
    double normPrio2 = 1;
    if (minPrio != maxPrio) {
        normPrio1 = ((e1->getPriority() - minPrio) / (maxPrio - minPrio)) * 0.9 + 0.1;
        normPrio2 = ((e2->getPriority() - minPrio) / (maxPrio - minPrio)) * 0.9 + 0.1;
    }
    return (normPrio1
            * e1->getNumLanes() / maxNumLanes
            * e1->getSpeed() / maxSpeed
            * normPrio2
            * e2->getNumLanes() / maxNumLanes
            * e2->getSpeed() / maxSpeed);
}

void
NBEdgePriorityComputer::markBestParallel(const NBNode& n, NBEdge* bestFirst, NBEdge* bestSecond) {
    // edges running parallel to the main direction should also be prioritised
    const double a1 = bestFirst->getAngleAtNode(&n);
    const double a2 = bestSecond == nullptr ? a1 : bestSecond->getAngleAtNode(&n);
    SVCPermissions p1 = bestFirst->getPermissions();
    SVCPermissions p2 = bestSecond == nullptr ? p1 : bestSecond->getPermissions();
    for (NBEdge* e : n.getIncomingEdges()) {
        // @note: this rule might also apply if there are common permissions but
        // then we would not further rules to resolve the priority between the best edge and its parallel edge
        SVCPermissions perm = e->getPermissions();
        if (((GeomHelper::getMinAngleDiff(e->getAngleAtNode(&n), a1) < 10
                || GeomHelper::getMinAngleDiff(e->getAngleAtNode(&n), a2) < 10))
                && (p1 & perm) == 0 && (p2 & perm) == 0) {
            e->setJunctionPriority(&n, 1);
        }
    }
}


NBEdge*
NBEdgePriorityComputer::extractAndMarkFirst(NBNode& n, std::vector<NBEdge*>& s, int prio) {
    if (s.size() == 0) {
        return nullptr;
    }
    NBEdge* ret = s.front();
    s.erase(s.begin());
    ret->setJunctionPriority(&n, prio);
    return ret;
}


bool
NBEdgePriorityComputer::samePriority(const NBEdge* const e1, const NBEdge* const e2) {
    if (e1 == e2) {
        return true;
    }
    if (e1->getPriority() != e2->getPriority()) {
        return false;
    }
    if ((int) e1->getSpeed() != (int) e2->getSpeed()) {
        return false;
    }
    return (int) e1->getNumLanes() == (int) e2->getNumLanes();
}


bool
NBEdgePriorityComputer::hasDifferentPriorities(const EdgeVector& edges, const NBEdge* excluded) {
    if (edges.size() < 2) {
        return false;
    }
    int prio = edges[0] == excluded ? edges[1]->getPriority() : edges[0]->getPriority();
    for (auto e : edges) {
        if (e != excluded && e->getPriority() != prio) {
            return true;
        }
    }
    return false;
}


NBNodesEdgesSorter::crossing_by_junction_angle_sorter::crossing_by_junction_angle_sorter(const NBNode* node, const EdgeVector& ordering) {
    // reorder based on getAngleAtNodeToCenter
    myOrdering = ordering;
    sort(myOrdering.begin(), myOrdering.end(), NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter(node));
    // let the first edge remain the first
    rotate(myOrdering.begin(), std::find(myOrdering.begin(), myOrdering.end(), ordering.front()), myOrdering.end());
}


/****************************************************************************/
