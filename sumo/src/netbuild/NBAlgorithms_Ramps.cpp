/****************************************************************************/
/// @file    NBAlgorithms_Ramps.cpp
/// @author  Daniel Krajzewicz
/// @date    29. March 2012
/// @version $Id$
///
// Algorithms for highway on-/off-ramps computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include "NBNetBuilder.h"
#include "NBNodeCont.h"
#include "NBNode.h"
#include "NBEdge.h"
#include "NBAlgorithms_Ramps.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
const std::string NBRampsComputer::ADDED_ON_RAMP_EDGE("-AddedOnRampEdge");

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBRampsComputer
// ---------------------------------------------------------------------------
void
NBRampsComputer::computeRamps(NBNetBuilder& nb, OptionsCont& oc) {
    SUMOReal minHighwaySpeed = oc.getFloat("ramps.min-highway-speed");
    SUMOReal maxRampSpeed = oc.getFloat("ramps.max-ramp-speed");
    SUMOReal rampLength = oc.getFloat("ramps.ramp-length");
    bool dontSplit = oc.getBool("ramps.no-split");
    std::set<NBEdge*> incremented;
    // check whether on-off ramps shall be guessed
    if (oc.getBool("ramps.guess")) {
        NBNodeCont& nc = nb.getNodeCont();
        NBEdgeCont& ec = nb.getEdgeCont();
        NBDistrictCont& dc = nb.getDistrictCont();
        std::set<NBNode*> potOnRamps;
        std::set<NBNode*> potOffRamps;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            NBNode* cur = (*i).second;
            if (mayNeedOnRamp(cur, minHighwaySpeed, maxRampSpeed)) {
                potOnRamps.insert(cur);
            }
            if (mayNeedOffRamp(cur, minHighwaySpeed, maxRampSpeed)) {
                potOffRamps.insert(cur);
            }
        }
        for (std::set<NBNode*>::const_iterator i = potOnRamps.begin(); i != potOnRamps.end(); ++i) {
            buildOnRamp(*i, nc, ec, dc, rampLength, dontSplit, incremented);
        }
        for (std::set<NBNode*>::const_iterator i = potOffRamps.begin(); i != potOffRamps.end(); ++i) {
            buildOffRamp(*i, nc, ec, dc, rampLength, dontSplit, incremented);
        }
    }
    // check whether on-off ramps shall be guessed
    if (oc.isSet("ramps.set")) {
        std::vector<std::string> edges = oc.getStringVector("ramps.set");
        NBNodeCont& nc = nb.getNodeCont();
        NBEdgeCont& ec = nb.getEdgeCont();
        NBDistrictCont& dc = nb.getDistrictCont();
        for (std::vector<std::string>::iterator i = edges.begin(); i != edges.end(); ++i) {
            NBEdge* e = ec.retrieve(*i);
            if (e == 0) {
                WRITE_WARNING("Can not build on ramp on edge '" + *i + "' - the edge is not known.");
                continue;
            }
            NBNode* from = e->getFromNode();
            if (from->getIncomingEdges().size() == 2 && from->getOutgoingEdges().size() == 1) {
                buildOnRamp(from, nc, ec, dc, rampLength, dontSplit, incremented);
            }
            // load edge again to check offramps
            e = ec.retrieve(*i);
            if (e == 0) {
                WRITE_WARNING("Can not build off ramp on edge '" + *i + "' - the edge is not known.");
                continue;
            }
            NBNode* to = e->getToNode();
            if (to->getIncomingEdges().size() == 1 && to->getOutgoingEdges().size() == 2) {
                buildOffRamp(to, nc, ec, dc, rampLength, dontSplit, incremented);
            }
        }
    }
}


bool
NBRampsComputer::mayNeedOnRamp(NBNode* cur, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed) {
    if (cur->getOutgoingEdges().size() != 1 || cur->getIncomingEdges().size() != 2) {
        return false;
    }
    NBEdge* potHighway, *potRamp, *cont;
    getOnRampEdges(cur, &potHighway, &potRamp, &cont);
    // may be an on-ramp
    return fulfillsRampConstraints(potHighway, potRamp, cont, minHighwaySpeed, maxRampSpeed);
}


bool
NBRampsComputer::mayNeedOffRamp(NBNode* cur, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed) {
    if (cur->getIncomingEdges().size() != 1 || cur->getOutgoingEdges().size() != 2) {
        return false;
    }
    // may be an off-ramp
    NBEdge* potHighway, *potRamp, *prev;
    getOffRampEdges(cur, &potHighway, &potRamp, &prev);
    return fulfillsRampConstraints(potHighway, potRamp, prev, minHighwaySpeed, maxRampSpeed);
}


void
NBRampsComputer::buildOnRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit, std::set<NBEdge*>& incremented) {
    NBEdge* potHighway, *potRamp, *cont;
    getOnRampEdges(cur, &potHighway, &potRamp, &cont);
    // compute the number of lanes to append
    const unsigned int firstLaneNumber = potHighway->getNumLanes();
    int toAdd = (potRamp->getNumLanes() + firstLaneNumber) - cont->getNumLanes();
    NBEdge* first = cont;
    NBEdge* last = cont;
    NBEdge* curr = cont;
    if (toAdd > 0 && find(incremented.begin(), incremented.end(), cont) == incremented.end()) {
        SUMOReal currLength = 0;
        while (curr != 0 && currLength + curr->getGeometry().length() - POSITION_EPS < rampLength) {
            if (find(incremented.begin(), incremented.end(), curr) == incremented.end()) {
                curr->incLaneNo(toAdd);
                curr->invalidateConnections(true);
                incremented.insert(curr);
                moveRampRight(curr, toAdd);
                currLength += curr->getLength(); // !!! loaded length?
                last = curr;
            }
            NBNode* nextN = curr->getToNode();
            if (nextN->getOutgoingEdges().size() == 1) {
                curr = nextN->getOutgoingEdges()[0];
                if (curr->getNumLanes() != firstLaneNumber) {
                    // the number of lanes changes along the computation; we'll stop...
                    curr = 0;
                }
            } else {
                // ambigous; and, in fact, what should it be? ...stop
                curr = 0;
            }
        }
        // check whether a further split is necessary
        if (curr != 0 && !dontSplit && currLength - POSITION_EPS < rampLength && curr->getNumLanes() == firstLaneNumber && find(incremented.begin(), incremented.end(), curr) == incremented.end()) {
            // there is enough place to build a ramp; do it
            bool wasFirst = first == curr;
            NBNode* rn = new NBNode(curr->getID() + "-AddedOnRampNode", curr->getGeometry().positionAtOffset(rampLength - currLength));
            if (!nc.insert(rn)) {
                throw ProcessError("Ups - could not build on-ramp for edge '" + curr->getID() + "' (node could not be build)!");
            }
            std::string name = curr->getID();
            bool ok = ec.splitAt(dc, curr, rn, curr->getID() + ADDED_ON_RAMP_EDGE, curr->getID(), curr->getNumLanes() + toAdd, curr->getNumLanes());
            if (!ok) {
                WRITE_ERROR("Ups - could not build on-ramp for edge '" + curr->getID() + "'!");
                return;
            }
            //ec.retrieve(name)->invalidateConnections();
            curr = ec.retrieve(name + ADDED_ON_RAMP_EDGE);
            curr->invalidateConnections(true);
            incremented.insert(curr);
            last = curr;
            moveRampRight(curr, toAdd);
            if (wasFirst) {
                first = curr;
            }
        }
        if (curr == cont && dontSplit) {
            WRITE_WARNING("Could not build on-ramp for edge '"  + curr->getID() + "' due to option '--ramps.no-split'");
            return;
        }
    }
    // set connections from ramp/highway to added ramp
    if (!potHighway->addLane2LaneConnections(0, first, potRamp->getNumLanes(), MIN2(first->getNumLanes() - potRamp->getNumLanes(), potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true, true)) {
        throw ProcessError("Could not set connection!");
    }
    if (!potRamp->addLane2LaneConnections(0, first, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, true, true)) {
        throw ProcessError("Could not set connection!");
    }
    // patch ramp geometry
    PositionVector p = potRamp->getGeometry();
    p.pop_back();
    p.push_back(first->getLaneShape(0)[0]);
    potRamp->setGeometry(p);
    // set connections from added ramp to following highway
    NBNode* nextN = last->getToNode();
    if (nextN->getOutgoingEdges().size() == 1) {
        NBEdge* next = nextN->getOutgoingEdges()[0];//const EdgeVector& o1 = cont->getToNode()->getOutgoingEdges();
        if (next->getNumLanes() < last->getNumLanes()) {
            last->addLane2LaneConnections(last->getNumLanes() - next->getNumLanes(), next, 0, next->getNumLanes(), NBEdge::L2L_VALIDATED);
        }
    }
}


void
NBRampsComputer::buildOffRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit, std::set<NBEdge*>& incremented) {
    NBEdge* potHighway, *potRamp, *prev;
    getOffRampEdges(cur, &potHighway, &potRamp, &prev);
    // compute the number of lanes to append
    const unsigned int firstLaneNumber = potHighway->getNumLanes();
    int toAdd = (potRamp->getNumLanes() + firstLaneNumber) - prev->getNumLanes();
    NBEdge* first = prev;
    NBEdge* last = prev;
    NBEdge* curr = prev;
    if (toAdd > 0 && find(incremented.begin(), incremented.end(), prev) == incremented.end()) {
        SUMOReal currLength = 0;
        while (curr != 0 && currLength + curr->getGeometry().length() - POSITION_EPS < rampLength) {
            if (find(incremented.begin(), incremented.end(), curr) == incremented.end()) {
                curr->incLaneNo(toAdd);
                curr->invalidateConnections(true);
                incremented.insert(curr);
                moveRampRight(curr, toAdd);
                currLength += curr->getLength(); // !!! loaded length?
                last = curr;
            }
            NBNode* prevN = curr->getFromNode();
            if (prevN->getIncomingEdges().size() == 1) {
                curr = prevN->getIncomingEdges()[0];
                if (curr->getNumLanes() != firstLaneNumber) {
                    // the number of lanes changes along the computation; we'll stop...
                    curr = 0;
                }
            } else {
                // ambigous; and, in fact, what should it be? ...stop
                curr = 0;
            }
        }
        // check whether a further split is necessary
        if (curr != 0 && !dontSplit && currLength - POSITION_EPS < rampLength && curr->getNumLanes() == firstLaneNumber && find(incremented.begin(), incremented.end(), curr) == incremented.end()) {
            // there is enough place to build a ramp; do it
            bool wasFirst = first == curr;
            Position pos = curr->getGeometry().positionAtOffset(curr->getGeometry().length() - (rampLength  - currLength));
            NBNode* rn = new NBNode(curr->getID() + "-AddedOffRampNode", pos);
            if (!nc.insert(rn)) {
                throw ProcessError("Ups - could not build on-ramp for edge '" + curr->getID() + "' (node could not be build)!");
            }
            std::string name = curr->getID();
            bool ok = ec.splitAt(dc, curr, rn, curr->getID(), curr->getID() + "-AddedOffRampEdge", curr->getNumLanes(), curr->getNumLanes() + toAdd);
            if (!ok) {
                WRITE_ERROR("Ups - could not build on-ramp for edge '" + curr->getID() + "'!");
                return;
            }
            curr = ec.retrieve(name + "-AddedOffRampEdge");
            curr->invalidateConnections(true);
            incremented.insert(curr);
            last = curr;
            moveRampRight(curr, toAdd);
            if (wasFirst) {
                first = curr;
            }
        }
        if (curr == prev && dontSplit) {
            WRITE_WARNING("Could not build off-ramp for edge '"  + curr->getID() + "' due to option '--ramps.no-split'");
            return;
        }
    }
    // set connections from added ramp to ramp/highway
    if (!first->addLane2LaneConnections(potRamp->getNumLanes(), potHighway, 0, MIN2(first->getNumLanes() - 1, potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true)) {
        throw ProcessError("Could not set connection!");
    }
    if (!first->addLane2LaneConnections(0, potRamp, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, false)) {
        throw ProcessError("Could not set connection!");
    }
    // patch ramp geometry
    PositionVector p = potRamp->getGeometry();
    p.pop_front();
    p.push_front(first->getLaneShape(0)[-1]);
    potRamp->setGeometry(p);
    // set connections from previous highway to added ramp
    NBNode* prevN = last->getFromNode();
    if (prevN->getIncomingEdges().size() == 1) {
        NBEdge* prev = prevN->getIncomingEdges()[0];//const EdgeVector& o1 = cont->getToNode()->getOutgoingEdges();
        if (prev->getNumLanes() < last->getNumLanes()) {
            last->addLane2LaneConnections(last->getNumLanes() - prev->getNumLanes(), last, 0, prev->getNumLanes(), NBEdge::L2L_VALIDATED);
        }
    }
}


void
NBRampsComputer::moveRampRight(NBEdge* ramp, int addedLanes) {
    if (ramp->getLaneSpreadFunction() != LANESPREAD_CENTER) {
        return;
    }
    try {
        PositionVector g = ramp->getGeometry();
        SUMOReal factor = SUMO_const_laneWidthAndOffset * (SUMOReal)(addedLanes - 1) + SUMO_const_halfLaneAndOffset * (SUMOReal)(addedLanes % 2);
        g.move2side(factor);
        ramp->setGeometry(g);
    } catch (InvalidArgument&) {
        WRITE_WARNING("For edge '" + ramp->getID() + "': could not compute shape.");
    }
}


bool
NBRampsComputer::determinedBySpeed(NBEdge** potHighway, NBEdge** potRamp) {
    if (fabs((*potHighway)->getSpeed() - (*potRamp)->getSpeed()) < .1) {
        return false;
    }
    if ((*potHighway)->getSpeed() < (*potRamp)->getSpeed()) {
        std::swap(*potHighway, *potRamp);
    }
    return true;
}


bool
NBRampsComputer::determinedByLaneNumber(NBEdge** potHighway, NBEdge** potRamp) {
    if ((*potHighway)->getNumLanes() == (*potRamp)->getNumLanes()) {
        return false;
    }
    if ((*potHighway)->getNumLanes() < (*potRamp)->getNumLanes()) {
        std::swap(*potHighway, *potRamp);
    }
    return true;
}


void
NBRampsComputer::getOnRampEdges(NBNode* n, NBEdge** potHighway, NBEdge** potRamp, NBEdge** other) {
    *other = n->getOutgoingEdges()[0];
    const std::vector<NBEdge*>& edges = n->getIncomingEdges();
    assert(edges.size() == 2);
    *potHighway = edges[0];
    *potRamp = edges[1];
    /*
    // heuristic: highway is faster than ramp
    if(determinedBySpeed(potHighway, potRamp)) {
        return;
    }
    // heuristic: highway has more lanes than ramp
    if(determinedByLaneNumber(potHighway, potRamp)) {
        return;
    }
    */
    // heuristic: ramp comes from right
    const std::vector<NBEdge*>& edges2 = n->getEdges();
    std::vector<NBEdge*>::const_iterator i = std::find(edges2.begin(), edges2.end(), *other);
    NBContHelper::nextCW(edges2, i);
    if ((*i) == *potHighway) {
        std::swap(*potHighway, *potRamp);
    }
}


void
NBRampsComputer::getOffRampEdges(NBNode* n, NBEdge** potHighway, NBEdge** potRamp, NBEdge** other) {
    *other = n->getIncomingEdges()[0];
    const std::vector<NBEdge*>& edges = n->getOutgoingEdges();
    *potHighway = edges[0];
    *potRamp = edges[1];
    assert(edges.size() == 2);
    /*
    // heuristic: highway is faster than ramp
    if(determinedBySpeed(potHighway, potRamp)) {
        return;
    }
    // heuristic: highway has more lanes than ramp
    if(determinedByLaneNumber(potHighway, potRamp)) {
        return;
    }
    */
    // heuristic: ramp goes to right
    const std::vector<NBEdge*>& edges2 = n->getEdges();
    std::vector<NBEdge*>::const_iterator i = std::find(edges2.begin(), edges2.end(), *other);
    NBContHelper::nextCW(edges2, i);
    if ((*i) == *potRamp) {
        std::swap(*potHighway, *potRamp);
    }
}


bool
NBRampsComputer::fulfillsRampConstraints(
    NBEdge* potHighway, NBEdge* potRamp, NBEdge* other, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed) {
    // do not build ramps on rail edges
    if (isRailway(potHighway->getPermissions()) || isRailway(potRamp->getPermissions())) {
        return false;
    }
    // do not build ramps on connectors
    if (potHighway->isMacroscopicConnector() || potRamp->isMacroscopicConnector() || other->isMacroscopicConnector()) {
        return false;
    }
    // check whether a lane is missing
    if (potHighway->getNumLanes() + potRamp->getNumLanes() <= other->getNumLanes()) {
        return false;
    }
    // check conditions
    // is it really a highway?
    SUMOReal maxSpeed = MAX3(potHighway->getSpeed(), other->getSpeed(), potRamp->getSpeed());
    if (maxSpeed < minHighwaySpeed) {
        return false;
    }
    /*
    if (potHighway->getSpeed() < minHighwaySpeed || other->getSpeed() < minHighwaySpeed) {
        return false;
    }
    */
    // is it really a ramp?
    if (maxRampSpeed > 0 && maxRampSpeed < potRamp->getSpeed()) {
        return false;
    }
    return true;
}


/****************************************************************************/

