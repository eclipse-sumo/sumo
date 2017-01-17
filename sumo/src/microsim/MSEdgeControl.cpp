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

#include "MSEdgeControl.h"
#include "MSGlobals.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSVehicle.h"
#include <iostream>
#include <vector>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
    for (std::set<MSLane*, Named::ComparatorIdLess>::iterator i = myChangedStateLanes.begin(); i != myChangedStateLanes.end(); ++i) {
        LaneUsage& lu = myLanes[(*i)->getNumericalID()];
        // if the lane was inactive but is now...
        if (!lu.amActive && (*i)->getVehicleNumber() > 0) {
            // ... add to active lanes and mark as such
            if (lu.haveNeighbors) {
                myActiveLanes.push_front(*i);
            } else {
                myActiveLanes.push_back(*i);
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
    for (std::vector<MSLane*>::iterator i = myWithVehicles2Integrate.begin(); i != myWithVehicles2Integrate.end(); ++i) {
        if ((*i)->integrateNewVehicle(t)) {
            LaneUsage& lu = myLanes[(*i)->getNumericalID()];
            if (!lu.amActive) {
                if (lu.haveNeighbors) {
                    myActiveLanes.push_front(*i);
                } else {
                    myActiveLanes.push_back(*i);
                }
                lu.amActive = true;
            }
        }
    }
    if (MSGlobals::gLateralResolution > 0) {
        // multiple vehicle shadows may have entered an inactive lane and would
        // not be sorted otherwise
        for (LaneUsageVector::iterator it = myLanes.begin(); it != myLanes.end(); ++it) {
            (*it).lane->sortPartialVehicles();
        }
    }
}


void
MSEdgeControl::changeLanes(SUMOTime t) {
    std::vector<MSLane*> toAdd;
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end();) {
        LaneUsage& lu = myLanes[(*i)->getNumericalID()];
        if (lu.haveNeighbors) {
            MSEdge& edge = (*i)->getEdge();
            if (myLastLaneChange[edge.getNumericalID()] != t) {
                myLastLaneChange[edge.getNumericalID()] = t;
                edge.changeLanes(t);
                const std::vector<MSLane*>& lanes = edge.getLanes();
                for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
                    LaneUsage& lu = myLanes[(*i)->getNumericalID()];
                    //if ((*i)->getID() == "disabled") {
                    //    std::cout << SIMTIME << " vehicles=" << toString((*i)->getVehiclesSecure()) << "\n";
                    //    (*i)->releaseVehicles();
                    //}
                    if ((*i)->getVehicleNumber() > 0 && !lu.amActive) {
                        toAdd.push_back(*i);
                        lu.amActive = true;
                    }
                }
            }
            ++i;
        } else {
            i = myActiveLanes.end();
        }
    }
    for (std::vector<MSLane*>::iterator i = toAdd.begin(); i != toAdd.end(); ++i) {
        myActiveLanes.push_front(*i);
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep, const std::string& stage) {
    // Detections is made by the edge's lanes, therefore hand over.
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end(); ++i) {
        (*i)->detectCollisions(timestep, stage);
    }
}


std::vector<std::string>
MSEdgeControl::getEdgeNames() const {
    std::vector<std::string> ret;
    for (MSEdgeVector::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


void
MSEdgeControl::gotActive(MSLane* l) {
    myChangedStateLanes.insert(l);
}


/****************************************************************************/

