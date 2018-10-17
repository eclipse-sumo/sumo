/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSEdgeControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// Stores edges and lanes, performs moving of vehicle
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSEdgeControl.h"
#include "MSGlobals.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSVehicle.h"
#include <iostream>
#include <vector>


// ===========================================================================
// member method definitions
// ===========================================================================
MSEdgeControl::MSEdgeControl(const std::vector< MSEdge* >& edges)
    : myEdges(edges),
      myLanes(MSLane::dictSize()),
      myLastLaneChange(MSEdge::dictSize()) {
    // build the usage definitions for lanes
    for (std::vector< MSEdge* >::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        const std::vector<MSLane*>& lanes = (*i)->getLanes();
        if (!(*i)->hasLaneChanger()) {
            int pos = (*lanes.begin())->getNumericalID();
            myLanes[pos].lane = *(lanes.begin());
            myLanes[pos].firstNeigh = lanes.end();
            myLanes[pos].lastNeigh = lanes.end();
            myLanes[pos].amActive = false;
            myLanes[pos].haveNeighbors = false;
        } else {
            for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
                int pos = (*j)->getNumericalID();
                myLanes[pos].lane = *j;
                myLanes[pos].firstNeigh = (j + 1);
                myLanes[pos].lastNeigh = lanes.end();
                myLanes[pos].amActive = false;
                myLanes[pos].haveNeighbors = true;
            }
            myLastLaneChange[(*i)->getNumericalID()] = -1;
        }
    }
}


MSEdgeControl::~MSEdgeControl() {
}


void
MSEdgeControl::patchActiveLanes() {
    for (auto myChangedStateLane : myChangedStateLanes) {
        LaneUsage& lu = myLanes[myChangedStateLane->getNumericalID()];
        // if the lane was inactive but is now...
        if (!lu.amActive && myChangedStateLane->getVehicleNumber() > 0) {
            // ... add to active lanes and mark as such
            if (lu.haveNeighbors) {
                myActiveLanes.push_front(myChangedStateLane);
            } else {
                myActiveLanes.push_back(myChangedStateLane);
            }
            lu.amActive = true;
        }
    }
    myChangedStateLanes.clear();
}

void
MSEdgeControl::planMovements(SUMOTime t) {
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end();) {
        if ((*i)->getVehicleNumber() == 0) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            (*i)->planMovements(t);
            ++i;
        }
    }
}


void
MSEdgeControl::executeMovements(SUMOTime t) {
    myWithVehicles2Integrate.clear();
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end();) {
        if ((*i)->getVehicleNumber() == 0 || (*i)->executeMovements(t, myWithVehicles2Integrate)) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
    for (auto & i : myWithVehicles2Integrate) {
        if (i->integrateNewVehicle(t)) {
            LaneUsage& lu = myLanes[i->getNumericalID()];
            if (!lu.amActive) {
                if (lu.haveNeighbors) {
                    myActiveLanes.push_front(i);
                } else {
                    myActiveLanes.push_back(i);
                }
                lu.amActive = true;
            }
        }
    }
    if (MSGlobals::gLateralResolution > 0) {
        // multiple vehicle shadows may have entered an inactive lane and would
        // not be sorted otherwise
        for (auto & myLane : myLanes) {
            myLane.lane->sortPartialVehicles();
        }
    }
}


void
MSEdgeControl::changeLanes(SUMOTime t) {
    std::vector<MSLane*> toAdd;
    MSGlobals::gComputeLC = true;
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end();) {
        LaneUsage& lu = myLanes[(*i)->getNumericalID()];
        if (lu.haveNeighbors) {
            MSEdge& edge = (*i)->getEdge();
            if (myLastLaneChange[edge.getNumericalID()] != t) {
                myLastLaneChange[edge.getNumericalID()] = t;
                edge.changeLanes(t);
                const std::vector<MSLane*>& lanes = edge.getLanes();
                for (auto lane : lanes) {
                    LaneUsage& lu = myLanes[lane->getNumericalID()];
                    //if ((*i)->getID() == "disabled") {
                    //    std::cout << SIMTIME << " vehicles=" << toString((*i)->getVehiclesSecure()) << "\n";
                    //    (*i)->releaseVehicles();
                    //}
                    if (lane->getVehicleNumber() > 0 && !lu.amActive) {
                        toAdd.push_back(lane);
                        lu.amActive = true;
                    }
                }
            }
            ++i;
        } else {
            i = myActiveLanes.end();
        }
    }
    MSGlobals::gComputeLC = false;
    for (auto & i : toAdd) {
        myActiveLanes.push_front(i);
    }
    if (MSGlobals::gLateralResolution > 0) {
        // sort maneuver reservations
        for (auto & myLane : myLanes) {
            myLane.lane->sortManeuverReservations();
        }
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep, const std::string& stage) {
    // Detections is made by the edge's lanes, therefore hand over.
    for (auto & myActiveLane : myActiveLanes) {
        myActiveLane->detectCollisions(timestep, stage);
    }
}


std::vector<std::string>
MSEdgeControl::getEdgeNames() const {
    std::vector<std::string> ret;
    for (auto myEdge : myEdges) {
        ret.push_back(myEdge->getID());
    }
    return ret;
}


void
MSEdgeControl::gotActive(MSLane* l) {
    myChangedStateLanes.insert(l);
}

void
MSEdgeControl::setAdditionalRestrictions() {
    for (MSEdgeVector::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        const std::vector<MSLane*>& lanes = (*i)->getLanes();
        for (auto lane : lanes) {
            lane->initRestrictions();
        }
    }
}


/****************************************************************************/

