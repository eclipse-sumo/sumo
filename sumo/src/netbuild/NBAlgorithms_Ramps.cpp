/****************************************************************************/
/// @file    NBAlgorithms_Ramps.cpp
/// @author  Daniel Krajzewicz
/// @date    29. March 2012
/// @version $Id$
///
// Algorithms for highway on-/off-ramps computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBRampsComputer
// ---------------------------------------------------------------------------
void 
NBRampsComputer::computeRamps(NBNetBuilder &nb, OptionsCont& oc) {
    SUMOReal minHighwaySpeed = oc.getFloat("ramps.min-highway-speed");
    SUMOReal maxRampSpeed = oc.getFloat("ramps.max-ramp-speed");
    SUMOReal rampLength = oc.getFloat("ramps.ramp-length");
    bool dontSplit = oc.getBool("ramps.no-split");
    EdgeVector incremented;
    // check whether on-off ramps shall be guessed
    if (oc.getBool("ramps.guess")) {
        NBNodeCont &nc = nb.getNodeCont();
        NBEdgeCont &ec = nb.getEdgeCont();
        NBDistrictCont &dc = nb.getDistrictCont();
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            NBNode* cur = (*i).second;
            if (mayNeedOnRamp(cur, minHighwaySpeed, maxRampSpeed)) {
                buildOnRamp(cur, nc, ec, dc, rampLength, dontSplit, incremented);
            }
            if (mayNeedOffRamp(cur, minHighwaySpeed, maxRampSpeed)) {
                buildOffRamp(cur, nc, ec, dc, rampLength, dontSplit, incremented);
            }
        }
    }
    // check whether on-off ramps shall be guessed
    if (oc.isSet("ramps.set")) {
        std::vector<std::string> edges = oc.getStringVector("ramps.set");
        NBNodeCont &nc = nb.getNodeCont();
        NBEdgeCont &ec = nb.getEdgeCont();
        NBDistrictCont &dc = nb.getDistrictCont();
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
    NBEdge *potHighway, *potRamp, *cont;
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
    NBEdge *potHighway, *potRamp, *prev;
    getOffRampEdges(cur, &potHighway, &potRamp, &prev);
    return fulfillsRampConstraints(potHighway, potRamp, prev, minHighwaySpeed, maxRampSpeed);
}


void
NBRampsComputer::buildOnRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit, std::vector<NBEdge*>& incremented) {
    NBEdge *potHighway, *potRamp, *cont;
    getOnRampEdges(cur, &potHighway, &potRamp, &cont);
    // compute the number of lanes to append
    int toAdd = (potRamp->getNumLanes() + potHighway->getNumLanes()) - cont->getNumLanes();
    if (toAdd <= 0) {
        return;
    }
    //
    if (cont->getGeometry().length() - POSITION_EPS <= rampLength || dontSplit) {
        // the edge is shorter than the wished ramp
        //  append a lane only
        if (find(incremented.begin(), incremented.end(), cont) == incremented.end()) {
            cont->incLaneNo(toAdd);
            incremented.push_back(cont);
            if (!potHighway->addLane2LaneConnections(0, cont, potRamp->getNumLanes(),
                    MIN2(cont->getNumLanes() - potRamp->getNumLanes(), potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!potRamp->addLane2LaneConnections(0, cont, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, true, true)) {
                throw ProcessError("Could not set connection!");
            }
            //
            cont->invalidateConnections(true);
            const EdgeVector& o1 = cont->getToNode()->getOutgoingEdges();
            if (o1.size() == 1 && o1[0]->getNumLanes() < cont->getNumLanes()) {
                cont->addLane2LaneConnections(cont->getNumLanes() - o1[0]->getNumLanes(), o1[0], 0, o1[0]->getNumLanes(), NBEdge::L2L_VALIDATED);
            }
            moveRampRight(cont, toAdd);
        }
        PositionVector p = potRamp->getGeometry();
        p.pop_back();
        p.push_back(cont->getLaneShape(0)[0]);
        potRamp->setGeometry(p);
    } else {
        // there is enough place to build a ramp; do it
        NBNode* rn = new NBNode(cont->getID() + "-AddedOnRampNode", cont->getGeometry().positionAtLengthPosition(rampLength));
        if (!nc.insert(rn)) {
            throw ProcessError("Ups - could not build on-ramp for edge '" + potHighway->getID() + "' (node could not be build)!");
        }
        std::string name = cont->getID();
        bool ok = ec.splitAt(dc, cont, rn, cont->getID() + "-AddedOnRampEdge", cont->getID(), cont->getNumLanes() + toAdd, cont->getNumLanes());
        if (!ok) {
            WRITE_ERROR("Ups - could not build on-ramp for edge '" + potHighway->getID() + "'!");
            return;
        } else {
            NBEdge* added_ramp = ec.retrieve(name + "-AddedOnRampEdge");
            NBEdge* added = ec.retrieve(name);
            incremented.push_back(added_ramp);
            if (added_ramp->getNumLanes() != added->getNumLanes()) {
                int off = added_ramp->getNumLanes() - added->getNumLanes();
                if (!added_ramp->addLane2LaneConnections(off, added, 0, added->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
                moveRampRight(added_ramp, toAdd);
            } else {
                if (!added_ramp->addLane2LaneConnections(0, added, 0, added_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
            }
            if (!potHighway->addLane2LaneConnections(0, added_ramp, potRamp->getNumLanes(),
                    MIN2(added_ramp->getNumLanes() - potRamp->getNumLanes(), potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, false, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!potRamp->addLane2LaneConnections(0, added_ramp, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, true, true)) {
                throw ProcessError("Could not set connection!");
            }
            PositionVector p = potRamp->getGeometry();
            p.pop_back();
            p.push_back(added_ramp->getLaneShape(0)[0]);
            potRamp->setGeometry(p);
        }
    }
    return;
}


void
NBRampsComputer::buildOffRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit, std::vector<NBEdge*>& incremented) {
    NBEdge *potHighway, *potRamp, *prev;
    getOffRampEdges(cur, &potHighway, &potRamp, &prev);
    // compute the number of lanes to append
    int toAdd = (potRamp->getNumLanes() + potHighway->getNumLanes()) - prev->getNumLanes();
    if (toAdd <= 0) {
        return;
    }
    // append on-ramp
    if (prev->getGeometry().length() - POSITION_EPS <= rampLength || dontSplit) {
        // the edge is shorter than the wished ramp
        //  append a lane only
        if (find(incremented.begin(), incremented.end(), prev) == incremented.end()) {
            incremented.push_back(prev);
            prev->incLaneNo(toAdd);
            prev->invalidateConnections(true);
            if (!prev->addLane2LaneConnections(potRamp->getNumLanes(), potHighway, 0, MIN2(prev->getNumLanes() - 1, potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!prev->addLane2LaneConnections(0, potRamp, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, false)) {
                throw ProcessError("Could not set connection!");
            }
            moveRampRight(prev, toAdd);
        }
        PositionVector p = potRamp->getGeometry();
        p.pop_front();
        p.push_front(prev->getLaneShape(0)[-1]);
        potRamp->setGeometry(p);
    } else {
        Position pos = prev->getGeometry().positionAtLengthPosition(prev->getGeometry().length() - rampLength);
        NBNode* rn = new NBNode(prev->getID() + "-AddedOffRampNode", pos);
        if (!nc.insert(rn)) {
            throw ProcessError("Ups - could not build off-ramp for edge '" + potHighway->getID() + "' (node could not be build)!");
        }
        std::string name = prev->getID();
        bool ok = ec.splitAt(dc, prev, rn, prev->getID(), prev->getID() + "-AddedOffRampEdge", prev->getNumLanes(), prev->getNumLanes() + toAdd);
        if (!ok) {
            WRITE_ERROR("Ups - could not build on-ramp for edge '" + potHighway->getID() + "'!");
            return;
        } else {
            NBEdge* added_ramp = ec.retrieve(name + "-AddedOffRampEdge");
            NBEdge* added = ec.retrieve(name);
            if (added_ramp->getNumLanes() != added->getNumLanes()) {
                incremented.push_back(added_ramp);
                int off = added_ramp->getNumLanes() - added->getNumLanes();
                if (!added->addLane2LaneConnections(0, added_ramp, off, added->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
                moveRampRight(added_ramp, toAdd);
            } else {
                if (!added->addLane2LaneConnections(0, added_ramp, 0, added_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
            }
            if (!added_ramp->addLane2LaneConnections(potRamp->getNumLanes(), potHighway, 0,
                    MIN2(added_ramp->getNumLanes() - potRamp->getNumLanes(), potHighway->getNumLanes()), NBEdge::L2L_VALIDATED, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!added_ramp->addLane2LaneConnections(0, potRamp, 0, potRamp->getNumLanes(), NBEdge::L2L_VALIDATED, false)) {
                throw ProcessError("Could not set connection!");
            }
            PositionVector p = potRamp->getGeometry();
            p.pop_front();
            p.push_front(added_ramp->getLaneShape(0)[-1]);
            potRamp->setGeometry(p);
        }
    }
}


void 
NBRampsComputer::moveRampRight(NBEdge *ramp, int addedLanes) {
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
    if (fabs((*potHighway)->getSpeed() - (*potRamp)->getSpeed())<.1) {
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
NBRampsComputer::getOnRampEdges(NBNode *n, NBEdge **potHighway, NBEdge **potRamp, NBEdge **other) {
    *other = n->getOutgoingEdges()[0];
    const std::vector<NBEdge*> &edges = n->getIncomingEdges();
    assert(edges.size()==2);
    *potHighway = edges[0];
    *potRamp = edges[1];
    // heuristic: highway is faster than ramp
    if(determinedBySpeed(potHighway, potRamp)) {
        return;
    }
    // heuristic: highway has more lanes than ramp
    if(determinedByLaneNumber(potHighway, potRamp)) {
        return;
    }
    // heuristic: ramp comes from right
    const std::vector<NBEdge*> &edges2 = n->getEdges();
    std::vector<NBEdge*>::const_iterator i=std::find(edges2.begin(), edges2.end(), *other);
    NBContHelper::nextCW(edges2, i);
    if((*i)==*potHighway) {
        std::swap(*potHighway, *potRamp);
    }
}


void 
NBRampsComputer::getOffRampEdges(NBNode *n, NBEdge **potHighway, NBEdge **potRamp, NBEdge **other) {
    *other = n->getIncomingEdges()[0];
    const std::vector<NBEdge*> &edges = n->getOutgoingEdges();
    *potHighway = edges[0];
    *potRamp = edges[1];
    assert(edges.size()==2);
    // heuristic: highway is faster than ramp
    if(determinedBySpeed(potHighway, potRamp)) {
        return;
    }
    // heuristic: highway has more lanes than ramp
    if(determinedByLaneNumber(potHighway, potRamp)) {
        return;
    }
    // heuristic: ramp goes to right
    const std::vector<NBEdge*> &edges2 = n->getEdges();
    std::vector<NBEdge*>::const_iterator i=std::find(edges2.begin(), edges2.end(), *other);
    NBContHelper::nextCW(edges2, i);
    if((*i)==*potRamp) {
        std::swap(*potHighway, *potRamp);
    }
}


bool
NBRampsComputer::fulfillsRampConstraints(NBEdge *potHighway, NBEdge *potRamp, NBEdge *other, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed) {
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
    if (potHighway->getSpeed() < minHighwaySpeed || other->getSpeed() < minHighwaySpeed) {
        return false;
    }
    // is it really a ramp?
    if (maxRampSpeed > 0 && maxRampSpeed < potRamp->getSpeed()) {
        return false;
    }
    return true;
}


/****************************************************************************/

