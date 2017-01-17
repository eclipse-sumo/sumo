/****************************************************************************/
/// @file    NBAlgorithms_Ramps.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    29. March 2012
/// @version $Id$
///
// Algorithms for highway on-/off-ramps computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/ToString.h>
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
        // collect join exclusions
        std::set<std::string> noramps;
        if (oc.isSet("ramps.unset")) {
            std::vector<std::string> edges = oc.getStringVector("ramps.unset");
            noramps.insert(edges.begin(), edges.end());
        }
        // exclude roundabouts
        const std::set<EdgeSet>& roundabouts = ec.getRoundabouts();
        for (std::set<EdgeSet>::const_iterator it_round = roundabouts.begin();
                it_round != roundabouts.end(); ++it_round) {
            for (EdgeSet::const_iterator it_edge = it_round->begin(); it_edge != it_round->end(); ++it_edge) {
                noramps.insert((*it_edge)->getID());
            }
        }
        // if an edge is part of two ramps, ordering is important
        std::set<NBNode*, Named::ComparatorIdLess> potOnRamps;
        std::set<NBNode*, Named::ComparatorIdLess> potOffRamps;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            NBNode* cur = (*i).second;
            if (mayNeedOnRamp(cur, minHighwaySpeed, maxRampSpeed, noramps)) {
                potOnRamps.insert(cur);
            }
            if (mayNeedOffRamp(cur, minHighwaySpeed, maxRampSpeed, noramps)) {
                potOffRamps.insert(cur);
            }
        }
        for (std::set<NBNode*, Named::ComparatorIdLess>::const_iterator i = potOnRamps.begin(); i != potOnRamps.end(); ++i) {
            buildOnRamp(*i, nc, ec, dc, rampLength, dontSplit);
        }
        for (std::set<NBNode*, Named::ComparatorIdLess>::const_iterator i = potOffRamps.begin(); i != potOffRamps.end(); ++i) {
            buildOffRamp(*i, nc, ec, dc, rampLength, dontSplit);
        }
    }
    // check whether on-off ramps are specified
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
                buildOnRamp(from, nc, ec, dc, rampLength, dontSplit);
            }
            // load edge again to check offramps
            e = ec.retrieve(*i);
            if (e == 0) {
                WRITE_WARNING("Can not build off ramp on edge '" + *i + "' - the edge is not known.");
                continue;
            }
            NBNode* to = e->getToNode();
            if (to->getIncomingEdges().size() == 1 && to->getOutgoingEdges().size() == 2) {
                buildOffRamp(to, nc, ec, dc, rampLength, dontSplit);
            }
        }
    }
}


bool
NBRampsComputer::mayNeedOnRamp(NBNode* cur, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed, const std::set<std::string>& noramps) {
    if (cur->getOutgoingEdges().size() != 1 || cur->getIncomingEdges().size() != 2) {
        return false;
    }
    NBEdge* potHighway, *potRamp, *cont;
    getOnRampEdges(cur, &potHighway, &potRamp, &cont);
    // may be an on-ramp
    return fulfillsRampConstraints(potHighway, potRamp, cont, minHighwaySpeed, maxRampSpeed, noramps);
}


bool
NBRampsComputer::mayNeedOffRamp(NBNode* cur, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed, const std::set<std::string>& noramps) {
    if (cur->getIncomingEdges().size() != 1 || cur->getOutgoingEdges().size() != 2) {
        return false;
    }
    // may be an off-ramp
    NBEdge* potHighway, *potRamp, *prev;
    getOffRampEdges(cur, &potHighway, &potRamp, &prev);
    return fulfillsRampConstraints(potHighway, potRamp, prev, minHighwaySpeed, maxRampSpeed, noramps);
}


void
NBRampsComputer::buildOnRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit) {
    NBEdge* potHighway, *potRamp, *cont;
    getOnRampEdges(cur, &potHighway, &potRamp, &cont);
    // compute the number of lanes to append
    const int firstLaneNumber = cont->getNumLanes();
    int toAdd = (potRamp->getNumLanes() + potHighway->getNumLanes()) - firstLaneNumber;
    NBEdge* first = cont;
    NBEdge* last = cont;
    NBEdge* curr = cont;
    std::set<NBEdge*> incremented;
    if (toAdd > 0 && find(incremented.begin(), incremented.end(), cont) == incremented.end()) {
        SUMOReal currLength = 0;
        while (curr != 0 && currLength + curr->getGeometry().length() - POSITION_EPS < rampLength) {
            if (find(incremented.begin(), incremented.end(), curr) == incremented.end()) {
                curr->incLaneNo(toAdd);
                if (curr->getStep() < NBEdge::LANES2LANES_USER) {
                    curr->invalidateConnections(true);
                }
                incremented.insert(curr);
                moveRampRight(curr, toAdd);
                currLength += curr->getGeometry().length(); // !!! loaded length?
                last = curr;
            }
            NBNode* nextN = curr->getToNode();
            if (nextN->getOutgoingEdges().size() == 1) {
                curr = nextN->getOutgoingEdges()[0];
                if (curr->getNumLanes() != firstLaneNumber) {
                    // the number of lanes changes along the computation; we'll stop...
                    curr = 0;
                } else if (curr->isTurningDirectionAt(last)) {
                    // turnarounds certainly should not be included in a ramp
                    curr = 0;
                } else if (curr == potHighway || curr == potRamp) {
                    // circular connectivity. do not split!
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
    if (potHighway->getStep() < NBEdge::LANES2LANES_USER) {
        if (!potHighway->addLane2LaneConnections(0, first, potRamp->getNumLanes(), MIN2(first->getNumLanes() - potRamp->getNumLanes(), potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true, true)) {
            throw ProcessError("Could not set connection!");
        }
    }
    if (potRamp->getStep() < NBEdge::LANES2LANES_USER) {
        if (!potRamp->addLane2LaneConnections(0, first, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, true, true)) {
            throw ProcessError("Could not set connection!");
        }
    }
    // patch ramp geometry
    PositionVector p = potRamp->getGeometry();
    p.pop_back();
    p.push_back(first->getLaneShape(0)[0]);
    potRamp->setGeometry(p);
}


void
NBRampsComputer::buildOffRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit) {
    NBEdge* potHighway, *potRamp, *prev;
    getOffRampEdges(cur, &potHighway, &potRamp, &prev);
    // compute the number of lanes to append
    const int firstLaneNumber = prev->getNumLanes();
    int toAdd = (potRamp->getNumLanes() + potHighway->getNumLanes()) - firstLaneNumber;
    NBEdge* first = prev;
    NBEdge* last = prev;
    NBEdge* curr = prev;
    std::set<NBEdge*> incremented;
    if (toAdd > 0 && find(incremented.begin(), incremented.end(), prev) == incremented.end()) {
        SUMOReal currLength = 0;
        while (curr != 0 && currLength + curr->getGeometry().length() - POSITION_EPS < rampLength) {
            if (find(incremented.begin(), incremented.end(), curr) == incremented.end()) {
                curr->incLaneNo(toAdd);
                if (curr->getStep() < NBEdge::LANES2LANES_USER) {
                    curr->invalidateConnections(true);
                }
                incremented.insert(curr);
                moveRampRight(curr, toAdd);
                currLength += curr->getGeometry().length(); // !!! loaded length?
                last = curr;
            }
            NBNode* prevN = curr->getFromNode();
            if (prevN->getIncomingEdges().size() == 1) {
                curr = prevN->getIncomingEdges()[0];
                if (curr->getStep() < NBEdge::LANES2LANES_USER && toAdd != 0) {
                    // curr might be an onRamp. In this case connections need to be rebuilt
                    curr->invalidateConnections();
                }
                if (curr->getNumLanes() != firstLaneNumber) {
                    // the number of lanes changes along the computation; we'll stop...
                    curr = 0;
                } else if (last->isTurningDirectionAt(curr)) {
                    // turnarounds certainly should not be included in a ramp
                    curr = 0;
                } else if (curr == potHighway || curr == potRamp) {
                    // circular connectivity. do not split!
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
                throw ProcessError("Ups - could not build off-ramp for edge '" + curr->getID() + "' (node could not be build)!");
            }
            std::string name = curr->getID();
            bool ok = ec.splitAt(dc, curr, rn, curr->getID(), curr->getID() + "-AddedOffRampEdge", curr->getNumLanes(), curr->getNumLanes() + toAdd);
            if (!ok) {
                WRITE_ERROR("Ups - could not build off-ramp for edge '" + curr->getID() + "'!");
                return;
            }
            curr = ec.retrieve(name + "-AddedOffRampEdge");
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
    if (first->getStep() < NBEdge::LANES2LANES_USER) {
        if (!first->addLane2LaneConnections(potRamp->getNumLanes(), potHighway, 0, MIN2(first->getNumLanes() - 1, potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true)) {
            throw ProcessError("Could not set connection!");
        }
        if (!first->addLane2LaneConnections(0, potRamp, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, false)) {
            throw ProcessError("Could not set connection!");
        }
    }
    // patch ramp geometry
    PositionVector p = potRamp->getGeometry();
    p[0] = first->getLaneShape(0)[-1];
    potRamp->setGeometry(p);
}


void
NBRampsComputer::moveRampRight(NBEdge* ramp, int addedLanes) {
    if (ramp->getLaneSpreadFunction() != LANESPREAD_CENTER) {
        return;
    }
    try {
        PositionVector g = ramp->getGeometry();
        const SUMOReal offset = (0.5 * addedLanes *
                                 (ramp->getLaneWidth() == NBEdge::UNSPECIFIED_WIDTH ? SUMO_const_laneWidth : ramp->getLaneWidth()));
        g.move2side(offset);
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
    NBEdge* potHighway, NBEdge* potRamp, NBEdge* other, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed,
    const std::set<std::string>& noramps) {
    // check modes that are not appropriate for rampsdo not build ramps on rail edges
    if (hasWrongMode(potHighway) || hasWrongMode(potRamp) || hasWrongMode(other)) {
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
    // is it really a highway?
    SUMOReal maxSpeed = MAX3(potHighway->getSpeed(), other->getSpeed(), potRamp->getSpeed());
    if (maxSpeed < minHighwaySpeed) {
        return false;
    }
    // is any of the connections a turnaround?
    if (other->getToNode() == potHighway->getFromNode()) {
        // off ramp
        if (other->isTurningDirectionAt(potHighway) ||
                other->isTurningDirectionAt(potRamp)) {
            return false;
        }
    } else {
        // on ramp
        if (other->isTurningDirectionAt(potHighway) ||
                other->isTurningDirectionAt(potRamp)) {
            return false;
        }
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
    if (noramps.find(other->getID()) != noramps.end()) {
        return false;
    }
    return true;
}


bool
NBRampsComputer::hasWrongMode(NBEdge* edge) {
    // must allow passenger vehicles
    if ((edge->getPermissions() & SVC_PASSENGER) == 0) {
        return true;
    }
    // must not have a green verge or a lane that is only for soft modes
    for (int i = 0; i < (int)edge->getNumLanes(); ++i) {
        if ((edge->getPermissions(i) & ~(SVC_PEDESTRIAN | SVC_BICYCLE)) == 0) {
            return true;
        }
    }
    return false;
}

/****************************************************************************/

