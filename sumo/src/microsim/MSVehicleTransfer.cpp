/****************************************************************************/
/// @file    MSVehicleTransfer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep 2003
/// @version $Id$
///
// A mover of vehicles that got stucked due to grid locks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
    const MSEdge *e = veh->getEdge();
    if (veh->isParking()) {
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_PARKING);
    } else {
        if ((veh->succEdge(1) == 0) || veh->enterLaneAtMove(veh->succEdge(1)->getLanes()[0], true)) {
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            return;
        }
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
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

        if (desc.myParking) {
            // handle parking vehicles
            if (desc.myVeh->processNextStop(1) == 0) {
                ++i;
                continue;
            }
            // parking finished, head back into traffic
        }
        const SUMOVehicleClass vclass = desc.myVeh->getVehicleType().getVehicleClass();
        const MSEdge *e = desc.myVeh->getEdge();
        const MSEdge *nextEdge = desc.myVeh->succEdge(1);

        // get the lane on which this vehicle should continue
        // first select all the lanes which allow continuation onto nextEdge
        //   then pick the one which is least occupied
        // @todo maybe parking vehicles should always continue on the rightmost lane?
        MSLane *l = e->getFreeLane(e->allowedLanes(*nextEdge, vclass), vclass);

        if (desc.myParking) {
            // handle parking vehicles
            if (l->isInsertionSuccess(desc.myVeh, 0, desc.myVeh->getPositionOnLane(), false, MSMoveReminder::NOTIFICATION_PARKING)) {
                i = myVehicles.erase(i);
            } else {
                i++;
            }
        } else {
            // handle teleporting vehicles
            if (l->freeInsertion(*(desc.myVeh), MIN2(l->getMaxSpeed(), desc.myVeh->getMaxSpeed()), MSMoveReminder::NOTIFICATION_TELEPORT)) {
                WRITE_WARNING("Vehicle '" + desc.myVeh->getID() + "' ends teleporting on edge '" + e->getID() + "', simulation time " + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VEHICLE_STATE_ENDING_TELEPORT);
                i = myVehicles.erase(i);
            } else {
                // could not insert. maybe we should proceed in virtual space
                if (desc.myProceedTime<time) {
                    // get the lanes of the next edge (the one the vehicle wiil be
                    //  virtually on after all these computations)
                    desc.myVeh->leaveLane(MSMoveReminder::NOTIFICATION_TELEPORT);
                    // get the one beyond the one the vehicle moved to
                    // !!! only move reminders are called but the edge is not advanced
                    const MSEdge *nextEdge = desc.myVeh->succEdge(1);
                    // let the vehicle move to the next edge
                    if (nextEdge==0) {
                        WRITE_WARNING("Vehicle '" + desc.myVeh->getID()+ "' ends teleporting on end edge '" + e->getID()+ "'.");
                        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(desc.myVeh);
                        i = myVehicles.erase(i);
                        continue;
                    }
                    // use current travel time to determine when to move the vehicle forward
                    desc.myProceedTime = time + TIME2STEPS(e->getCurrentTravelTime());
                }
                ++i;
            }
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

