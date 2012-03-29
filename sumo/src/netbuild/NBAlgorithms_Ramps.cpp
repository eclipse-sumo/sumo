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
    EdgeVector incremented;
    bool bEdgeDeleted = false;
    // check whether on-off ramps shall be guessed
    if (oc.getBool("ramps.guess")) {
        NBNodeCont &nc = nb.getNodeCont();
        NBEdgeCont &ec = nb.getEdgeCont();
        NBDistrictCont &dc = nb.getDistrictCont();
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            NBNode* cur = (*i).second;
            if (mayNeedOnRamp(oc, cur)) {
                buildOnRamp(oc, cur, nc, ec, dc, incremented);
            }
            if (mayNeedOffRamp(oc, cur)) {
                buildOffRamp(oc, cur, nc, ec, dc, incremented);
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
                bEdgeDeleted = buildOnRamp(oc, from, nc, ec, dc, incremented);
            }
            // load edge again to check offramps
            e = ec.retrieve(*i);
            if (e == 0) {
                WRITE_WARNING("Can not build off ramp on edge '" + *i + "' - the edge is not known.");
                continue;
            }
            NBNode* to = e->getToNode();
            if (to->getIncomingEdges().size() == 1 && to->getOutgoingEdges().size() == 2) {
                buildOffRamp(oc, to, nc, ec, dc, incremented);
            }
        }
    }
}


bool
NBRampsComputer::mayNeedOnRamp(OptionsCont& oc, NBNode* cur) {
    if (cur->getIncomingEdges().size() == 2 && cur->getOutgoingEdges().size() == 1) {
        // may be an on-ramp
        NBEdge* pot_highway = cur->getIncomingEdges()[0];
        NBEdge* pot_ramp = cur->getIncomingEdges()[1];
        NBEdge* cont = cur->getOutgoingEdges()[0];
        // do not build ramps on connectors
        if (pot_highway->isMacroscopicConnector() || pot_ramp->isMacroscopicConnector() || cont->isMacroscopicConnector()) {
            return false;
        }
        // check whether a lane is missing
        if (pot_highway->getNumLanes() + pot_ramp->getNumLanes() <= cont->getNumLanes()) {
            return false;
        }
        // assign highway/ramp properly
        if (pot_highway->getSpeed() < pot_ramp->getSpeed()) {
            std::swap(pot_highway, pot_ramp);
        } else if (pot_highway->getSpeed() == pot_ramp->getSpeed() && pot_highway->getNumLanes() < pot_ramp->getNumLanes()) {

            std::swap(pot_highway, pot_ramp);
        }
        // check conditions
        // is it really a highway?
        SUMOReal minHighwaySpeed = oc.getFloat("ramps.min-highway-speed");
        if (pot_highway->getSpeed() < minHighwaySpeed || cont->getSpeed() < minHighwaySpeed) {
            return false;
        }
        // is it really a ramp?
        SUMOReal maxRampSpeed = oc.getFloat("ramps.max-ramp-speed");
        if (maxRampSpeed > 0 && maxRampSpeed < pot_ramp->getSpeed()) {
            return false;
        }
        // ok, may be
        return true;
    }
    return false;
}


bool
NBRampsComputer::mayNeedOffRamp(OptionsCont& oc, NBNode* cur) {
    if (cur->getIncomingEdges().size() == 1 && cur->getOutgoingEdges().size() == 2) {
        // may be an off-ramp
        NBEdge* pot_highway = cur->getOutgoingEdges()[0];
        NBEdge* pot_ramp = cur->getOutgoingEdges()[1];
        NBEdge* prev = cur->getIncomingEdges()[0];
        // do not build ramps on connectors
        if (pot_highway->isMacroscopicConnector() || pot_ramp->isMacroscopicConnector() || prev->isMacroscopicConnector()) {
            return false;
        }
        // check whether a lane is missing
        if (pot_highway->getNumLanes() + pot_ramp->getNumLanes() <= prev->getNumLanes()) {
            return false;
        }
        // assign highway/ramp properly
        if (pot_highway->getSpeed() < pot_ramp->getSpeed()) {
            std::swap(pot_highway, pot_ramp);
        } else if (pot_highway->getSpeed() == pot_ramp->getSpeed() && pot_highway->getNumLanes() < pot_ramp->getNumLanes()) {
            std::swap(pot_highway, pot_ramp);
        }
        // check conditions
        // is it really a highway?
        SUMOReal minHighwaySpeed = oc.getFloat("ramps.min-highway-speed");
        if (pot_highway->getSpeed() < minHighwaySpeed || prev->getSpeed() < minHighwaySpeed) {
            return false;
        }
        // is it really a ramp?
        SUMOReal maxRampSpeed = oc.getFloat("ramps.max-ramp-speed");
        if (maxRampSpeed > 0 && maxRampSpeed < pot_ramp->getSpeed()) {
            return false;
        }
        return true;
    }
    return false;
}


bool
NBRampsComputer::buildOnRamp(OptionsCont& oc, NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, EdgeVector& incremented) {
    NBEdge* pot_highway = cur->getIncomingEdges()[0];
    NBEdge* pot_ramp = cur->getIncomingEdges()[1];
    NBEdge* cont = cur->getOutgoingEdges()[0];
    bool bEdgeDeleted = false;
    // assign highway/ramp properly
    if (pot_highway->getSpeed() < pot_ramp->getSpeed()) {
        std::swap(pot_highway, pot_ramp);
    } else if (pot_highway->getSpeed() == pot_ramp->getSpeed() && pot_highway->getNumLanes() < pot_ramp->getNumLanes()) {
        std::swap(pot_highway, pot_ramp);
    }
    // compute the number of lanes to append
    int toAdd = (pot_ramp->getNumLanes() + pot_highway->getNumLanes()) - cont->getNumLanes();
    if (toAdd <= 0) {
        return false;
    }
    //
    if (cont->getGeometry().length() - POSITION_EPS <= oc.getFloat("ramps.ramp-length")) {
        // the edge is shorter than the wished ramp
        //  append a lane only
        if (find(incremented.begin(), incremented.end(), cont) == incremented.end()) {
            cont->incLaneNo(toAdd);
            incremented.push_back(cont);
            if (!pot_highway->addLane2LaneConnections(0, cont, pot_ramp->getNumLanes(),
                    MIN2(cont->getNumLanes() - pot_ramp->getNumLanes(), pot_highway->getNumLanes()), NBEdge::L2L_VALIDATED, true, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!pot_ramp->addLane2LaneConnections(0, cont, 0, pot_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, true, true)) {
                throw ProcessError("Could not set connection!");
            }
            //
            cont->invalidateConnections(true);
            const EdgeVector& o1 = cont->getToNode()->getOutgoingEdges();
            if (o1.size() == 1 && o1[0]->getNumLanes() < cont->getNumLanes()) {
                cont->addLane2LaneConnections(cont->getNumLanes() - o1[0]->getNumLanes(), o1[0], 0, o1[0]->getNumLanes(), NBEdge::L2L_VALIDATED);
            }
            //
            if (cont->getLaneSpreadFunction() == LANESPREAD_CENTER) {
                try {
                    PositionVector g = cont->getGeometry();
                    g.move2side(SUMO_const_laneWidthAndOffset);
                    cont->setGeometry(g);
                } catch (InvalidArgument&) {
                    WRITE_WARNING("For edge '" + cont->getID() + "': could not compute shape.");
                }
            }
        }
        PositionVector p = pot_ramp->getGeometry();
        p.pop_back();
        p.push_back(cont->getFromNode()->getPosition());
        pot_ramp->setGeometry(p);
    } else {
        bEdgeDeleted = true;
        // there is enough place to build a ramp; do it
        NBNode* rn = new NBNode(cont->getID() + "-AddedOnRampNode", cont->getGeometry().positionAtLengthPosition(oc.getFloat("ramps.ramp-length")));
        if (!nc.insert(rn)) {
            throw ProcessError("Ups - could not build on-ramp for edge '" + pot_highway->getID() + "' (node could not be build)!");
        }
        std::string name = cont->getID();
        bool ok = ec.splitAt(dc, cont, rn, cont->getID() + "-AddedOnRampEdge", cont->getID(), cont->getNumLanes() + toAdd, cont->getNumLanes());
        if (!ok) {
            WRITE_ERROR("Ups - could not build on-ramp for edge '" + pot_highway->getID() + "'!");
            return true;
        } else {
            NBEdge* added_ramp = ec.retrieve(name + "-AddedOnRampEdge");
            NBEdge* added = ec.retrieve(name);
            incremented.push_back(added_ramp);
            if (added_ramp->getNumLanes() != added->getNumLanes()) {
                int off = added_ramp->getNumLanes() - added->getNumLanes();
                if (!added_ramp->addLane2LaneConnections(off, added, 0, added->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
                if (added_ramp->getLaneSpreadFunction() == LANESPREAD_CENTER) {
                    try {
                        PositionVector g = added_ramp->getGeometry();
                        SUMOReal factor = SUMO_const_laneWidthAndOffset * (SUMOReal)(toAdd - 1) + SUMO_const_halfLaneAndOffset * (SUMOReal)(toAdd % 2);
                        g.move2side(factor);
                        added_ramp->setGeometry(g);
                    } catch (InvalidArgument&) {
                        WRITE_WARNING("For edge '" + added_ramp->getID() + "': could not compute shape.");
                    }
                }
            } else {
                if (!added_ramp->addLane2LaneConnections(0, added, 0, added_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
            }
            if (!pot_highway->addLane2LaneConnections(0, added_ramp, pot_ramp->getNumLanes(),
                    MIN2(added_ramp->getNumLanes() - pot_ramp->getNumLanes(), pot_highway->getNumLanes()), NBEdge::L2L_VALIDATED, false, true)) {
                throw ProcessError("Could not set connection!");

            }
            if (!pot_ramp->addLane2LaneConnections(0, added_ramp, 0, pot_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, true, true)) {
                throw ProcessError("Could not set connection!");
            }
            PositionVector p = pot_ramp->getGeometry();
            p.pop_back();
            p.push_back(added_ramp->getFromNode()->getPosition());//added_ramp->getLaneShape(0).at(0));
            pot_ramp->setGeometry(p);
        }
    }
    return bEdgeDeleted;
}


void
NBRampsComputer::buildOffRamp(OptionsCont& oc, NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, EdgeVector& incremented) {
    NBEdge* pot_highway = cur->getOutgoingEdges()[0];
    NBEdge* pot_ramp = cur->getOutgoingEdges()[1];
    NBEdge* prev = cur->getIncomingEdges()[0];
    // assign highway/ramp properly
    if (pot_highway->getSpeed() < pot_ramp->getSpeed()) {
        std::swap(pot_highway, pot_ramp);
    } else if (pot_highway->getSpeed() == pot_ramp->getSpeed() && pot_highway->getNumLanes() < pot_ramp->getNumLanes()) {
        std::swap(pot_highway, pot_ramp);
    }
    // compute the number of lanes to append
    int toAdd = (pot_ramp->getNumLanes() + pot_highway->getNumLanes()) - prev->getNumLanes();
    if (toAdd <= 0) {
        return;
    }
    // append on-ramp
    if (prev->getGeometry().length() - POSITION_EPS <= oc.getFloat("ramps.ramp-length")) {
        // the edge is shorter than the wished ramp
        //  append a lane only
        if (find(incremented.begin(), incremented.end(), prev) == incremented.end()) {
            incremented.push_back(prev);
            prev->incLaneNo(toAdd);
            prev->invalidateConnections(true);
            if (!prev->addLane2LaneConnections(pot_ramp->getNumLanes(), pot_highway, 0, MIN2(prev->getNumLanes() - 1, pot_highway->getNumLanes()), NBEdge::L2L_VALIDATED, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!prev->addLane2LaneConnections(0, pot_ramp, 0, pot_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, false)) {
                throw ProcessError("Could not set connection!");
            }
            if (prev->getLaneSpreadFunction() == LANESPREAD_CENTER) {
                try {
                    PositionVector g = prev->getGeometry();
                    g.move2side(SUMO_const_laneWidthAndOffset);
                    prev->setGeometry(g);
                } catch (InvalidArgument&) {
                    WRITE_WARNING("For edge '" + prev->getID() + "': could not compute shape.");
                }
            }
        }
        PositionVector p = pot_ramp->getGeometry();
        p.pop_front();
        p.push_front(prev->getToNode()->getPosition());//added_ramp->getLaneShape(0).at(-1));
        pot_ramp->setGeometry(p);
    } else {
        Position pos = prev->getGeometry().positionAtLengthPosition(prev->getGeometry().length() - oc.getFloat("ramps.ramp-length"));
        NBNode* rn = new NBNode(prev->getID() + "-AddedOffRampNode", pos);
        if (!nc.insert(rn)) {
            throw ProcessError("Ups - could not build off-ramp for edge '" + pot_highway->getID() + "' (node could not be build)!");
        }
        std::string name = prev->getID();
        bool ok = ec.splitAt(dc, prev, rn, prev->getID(), prev->getID() + "-AddedOffRampEdge", prev->getNumLanes(), prev->getNumLanes() + toAdd);
        if (!ok) {
            WRITE_ERROR("Ups - could not build on-ramp for edge '" + pot_highway->getID() + "'!");
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
                if (added_ramp->getLaneSpreadFunction() == LANESPREAD_CENTER) {
                    try {
                        PositionVector g = added_ramp->getGeometry();
                        SUMOReal factor = SUMO_const_laneWidthAndOffset * (SUMOReal)(toAdd - 1) + SUMO_const_halfLaneAndOffset * (SUMOReal)(toAdd % 2);
                        g.move2side(factor);
                        added_ramp->setGeometry(g);
                    } catch (InvalidArgument&) {
                        WRITE_WARNING("For edge '" + added_ramp->getID() + "': could not compute shape.");
                    }
                }
            } else {
                if (!added->addLane2LaneConnections(0, added_ramp, 0, added_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, true)) {
                    throw ProcessError("Could not set connection!");
                }
            }
            if (!added_ramp->addLane2LaneConnections(pot_ramp->getNumLanes(), pot_highway, 0,
                    MIN2(added_ramp->getNumLanes() - pot_ramp->getNumLanes(), pot_highway->getNumLanes()), NBEdge::L2L_VALIDATED, true)) {
                throw ProcessError("Could not set connection!");
            }
            if (!added_ramp->addLane2LaneConnections(0, pot_ramp, 0, pot_ramp->getNumLanes(), NBEdge::L2L_VALIDATED, false)) {
                throw ProcessError("Could not set connection!");
            }
            PositionVector p = pot_ramp->getGeometry();
            p.pop_front();
            p.push_front(added_ramp->getToNode()->getPosition());
            pot_ramp->setGeometry(p);
        }
    }
}


void
NBRampsComputer::checkHighwayRampOrder(NBEdge *&pot_highway, NBEdge *&pot_ramp) {
    if (pot_highway->getSpeed() < pot_ramp->getSpeed()) {
        std::swap(pot_highway, pot_ramp);
    } else if (pot_highway->getSpeed() == pot_ramp->getSpeed() && pot_highway->getNumLanes() < pot_ramp->getNumLanes()) {
        std::swap(pot_highway, pot_ramp);
    }
}

/****************************************************************************/

