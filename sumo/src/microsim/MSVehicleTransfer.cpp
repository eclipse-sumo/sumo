/****************************************************************************/
/// @file    MSVehicleTransfer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep 2003
/// @version $Id$
///
// A mover of vehicles that got stucked due to grid locks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <iostream>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include "MSNet.h"
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSVehicleControl.h"
#include "MSVehicleTransfer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
MSVehicleTransfer *MSVehicleTransfer::myInstance = 0;


// ===========================================================================
// member method definitions
// ===========================================================================
void
MSVehicleTransfer::addVeh(const SUMOTime t, MSVehicle *veh) throw() {
    // get the current edge of the vehicle
    MSEdge *e = MSEdge::dictionary(veh->getEdge()->getID());
    // let the vehicle be on the one
    veh->onRemovalFromNet(true);
    if (!veh->isParking()) {
        if ((veh->succEdge(1) == 0) || veh->enterLaneAtMove(veh->succEdge(1)->getLanes()[0], true)) {
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            return;
        }
        MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VEHICLE_STATE_STARTING_TELEPORT);
    }
    myVehicles.push_back(VehicleInformation(veh, t + TIME2STEPS(e->getCurrentTravelTime()), veh->isParking()));
}


void
MSVehicleTransfer::checkEmissions(SUMOTime time) throw() {
    // go through vehicles
    for (VehicleInfVector::iterator i=myVehicles.begin(); i!=myVehicles.end();) {
        // get the vehicle information
        VehicleInformation &desc = *i;
        const MSEdge *nextEdge = desc.myVeh->succEdge(1);
        if (desc.myParking) {
            if (desc.myVeh->processNextStop(1) == 0) {
                ++i;
                continue;
            }
            nextEdge = 0;
        }
        const MSEdge *e = desc.myVeh->getEdge();
        // get the lanes the vehicle may use
        const SUMOVehicleClass vclass = desc.myVeh->getVehicleType().getVehicleClass();
        MSLane *l = e->getFreeLane(e->allowedLanes(*nextEdge, vclass), vclass);
        // check whether the vehicle may be emitted onto a following edge
        if (l->freeEmit(*(desc.myVeh), MIN2(l->getMaxSpeed(), desc.myVeh->getMaxSpeed()))) {
            // remove from this if so
            if (!desc.myParking) {
                WRITE_WARNING("Vehicle '" + desc.myVeh->getID()+ "' ends teleporting on edge '" + e->getID()+ "', simulation time " + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VEHICLE_STATE_ENDING_TELEPORT);
            }
            i = myVehicles.erase(i);
        } else {
            // otherwise, check whether a consecutive edge may be used
            if (desc.myProceedTime<time) {
                // get the lanes of the next edge (the one the vehicle wiil be
                //  virtually on after all these computations)
                MSLane *tmp = *(e->getLanes().begin());
                // get the one beyond the one the vehicle moved to
                const MSEdge *nextEdge = desc.myVeh->succEdge(1);
                // let the vehicle move to the next edge
                if (nextEdge==0) {
                    WRITE_WARNING("Vehicle '" + desc.myVeh->getID()+ "' ends teleporting on end edge '" + e->getID()+ "'.");
                    MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(desc.myVeh);
                    i = myVehicles.erase(i);
                    continue;
                }
                // use current travel time to determine when to move the vehicle forward
                desc.myProceedTime = time + TIME2STEPS(tmp->getEdge().getCurrentTravelTime());
            }
            ++i;
        }

    }
}


bool
MSVehicleTransfer::hasPending() const throw() {
    return !myVehicles.empty();
}


MSVehicleTransfer *
MSVehicleTransfer::getInstance() throw() {
    if (myInstance==0) {
        myInstance = new MSVehicleTransfer();
    }
    return myInstance;
}


MSVehicleTransfer::MSVehicleTransfer() throw() {}


MSVehicleTransfer::~MSVehicleTransfer() throw() {
    myInstance = 0;
}



/****************************************************************************/

