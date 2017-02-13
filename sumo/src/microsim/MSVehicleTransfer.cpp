/****************************************************************************/
/// @file    MSVehicleTransfer.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sep 2003
/// @version $Id$
///
// A mover of vehicles that got stucked due to grid locks
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

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "MSNet.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSVehicle.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSVehicleControl.h"
#include "MSInsertionControl.h"
#include "MSVehicleTransfer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
MSVehicleTransfer* MSVehicleTransfer::myInstance = 0;
const SUMOReal MSVehicleTransfer::TeleportMinSpeed = 1;
const std::set<const MSVehicle*> MSVehicleTransfer::myEmptyVehicleSet;

// ===========================================================================
// member method definitions
// ===========================================================================
void
MSVehicleTransfer::add(const SUMOTime t, MSVehicle* veh) {
    if (veh->isParking()) {
        veh->getLaneChangeModel().endLaneChangeManeuver(MSMoveReminder::NOTIFICATION_PARKING);
        MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VEHICLE_STATE_STARTING_PARKING);
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_PARKING);
        myParkingVehicles[veh->getLane()].insert(veh); // initialized to empty set on first use
    } else {
        veh->getLaneChangeModel().endLaneChangeManeuver(MSMoveReminder::NOTIFICATION_TELEPORT);
        MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VEHICLE_STATE_STARTING_TELEPORT);
        if (veh->succEdge(1) == 0) {
            WRITE_WARNING("Vehicle '" + veh->getID() + "' teleports beyond arrival edge '" + veh->getEdge()->getID() + "', time " + time2string(t) + ".");
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            return;
        }
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
        veh->enterLaneAtMove(veh->succEdge(1)->getLanes()[0], true);
    }
    myVehicles.push_back(VehicleInformation(veh,
                                            t + TIME2STEPS(veh->getEdge()->getCurrentTravelTime(TeleportMinSpeed)),
                                            veh->isParking()));
}


void
MSVehicleTransfer::remove(MSVehicle* veh) {
    for (VehicleInfVector::iterator i = myVehicles.begin(); i != myVehicles.end(); ++i) {
        if (i->myVeh == veh) {
            myVehicles.erase(i);
            break;
        }
    }
    if (veh->getLane() != 0) {
        myParkingVehicles[veh->getLane()].erase(veh);
    }
}


void
MSVehicleTransfer::checkInsertions(SUMOTime time) {
    // go through vehicles
    for (VehicleInfVector::iterator i = myVehicles.begin(); i != myVehicles.end();) {
        // get the vehicle information
        VehicleInformation& desc = *i;

        if (desc.myParking) {
            // handle parking vehicles
            if (desc.myVeh->processNextStop(1) <= 0) {
                ++i;
                continue;
            }
            // parking finished, head back into traffic
        }
        const SUMOVehicleClass vclass = desc.myVeh->getVehicleType().getVehicleClass();
        const MSEdge* e = desc.myVeh->getEdge();
        const MSEdge* nextEdge = desc.myVeh->succEdge(1);

        // get the lane on which this vehicle should continue
        // first select all the lanes which allow continuation onto nextEdge
        //   then pick the one which is least occupied
        // @todo maybe parking vehicles should always continue on the rightmost lane?
        const MSLane* oldLane = desc.myVeh->getLane();
        MSLane* l = (nextEdge != 0 ? e->getFreeLane(e->allowedLanes(*nextEdge, vclass), vclass) :
                     e->getFreeLane(0, vclass));

        if (desc.myParking) {
            // handle parking vehicles
            if (l->isInsertionSuccess(desc.myVeh, 0, desc.myVeh->getPositionOnLane(), desc.myVeh->getLateralPositionOnLane(), false, MSMoveReminder::NOTIFICATION_PARKING)) {
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VEHICLE_STATE_ENDING_PARKING);
                myParkingVehicles[oldLane].erase(desc.myVeh);
                i = myVehicles.erase(i);
            } else {
                i++;
            }
        } else {
            // handle teleporting vehicles, lane may be 0 because permissions were modified by a closing rerouter or TraCI
            if (l != 0 && l->freeInsertion(*(desc.myVeh), MIN2(l->getSpeedLimit(), desc.myVeh->getMaxSpeed()), MSMoveReminder::NOTIFICATION_TELEPORT)) {
                WRITE_WARNING("Vehicle '" + desc.myVeh->getID() + "' ends teleporting on edge '" + e->getID() + "', time " + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VEHICLE_STATE_ENDING_TELEPORT);
                i = myVehicles.erase(i);
            } else {
                // could not insert. maybe we should proceed in virtual space
                if (desc.myProceedTime < time) {
                    if (desc.myVeh->succEdge(1) == 0) {
                        WRITE_WARNING("Vehicle '" + desc.myVeh->getID() + "' teleports beyond arrival edge '" + e->getID() + "', time " + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                        desc.myVeh->leaveLane(MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED);
                        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(desc.myVeh);
                        i = myVehicles.erase(i);
                        continue;
                    }
                    // let the vehicle move to the next edge
                    desc.myVeh->leaveLane(MSMoveReminder::NOTIFICATION_TELEPORT);
                    // active move reminders (i.e. rerouters)
                    desc.myVeh->enterLaneAtMove(desc.myVeh->succEdge(1)->getLanes()[0], true);
                    // use current travel time to determine when to move the vehicle forward
                    desc.myProceedTime = time + TIME2STEPS(e->getCurrentTravelTime(TeleportMinSpeed));
                }
                ++i;
            }
        }
    }
}


bool
MSVehicleTransfer::hasPending() const {
    return !myVehicles.empty();
}


MSVehicleTransfer*
MSVehicleTransfer::getInstance() {
    if (myInstance == 0) {
        myInstance = new MSVehicleTransfer();
    }
    return myInstance;
}


MSVehicleTransfer::MSVehicleTransfer() {}


MSVehicleTransfer::~MSVehicleTransfer() {
    myInstance = 0;
}


const std::set<const MSVehicle*>&
MSVehicleTransfer::getParkingVehicles(const MSLane* lane) const {
    ParkingVehicles::const_iterator it = myParkingVehicles.find(lane);
    if (it != myParkingVehicles.end()) {
        return it->second;
    } else {
        return myEmptyVehicleSet;
    }
}


void
MSVehicleTransfer::saveState(OutputDevice& out) const {
    std::map<const MSVehicle*,  const MSLane*> parkingLanes;
    for (ParkingVehicles::const_iterator it = myParkingVehicles.begin(); it != myParkingVehicles.end(); ++it) {
        const std::set<const MSVehicle*>& vehs = it->second;
        for (std::set<const MSVehicle*>::const_iterator it2 = vehs.begin(); it2 != vehs.end(); ++it2) {
            parkingLanes[*it2] = it->first;
        }
    }
    for (VehicleInfVector::const_iterator it = myVehicles.begin(); it != myVehicles.end(); ++it) {
        out.openTag(SUMO_TAG_VEHICLETRANSFER);
        out.writeAttr(SUMO_ATTR_ID, it->myVeh->getID());
        out.writeAttr(SUMO_ATTR_DEPART, it->myProceedTime);
        if (it->myParking) {
            out.writeAttr(SUMO_ATTR_PARKING, parkingLanes[it->myVeh]->getID());
        }
        out.closeTag();
    }
}


void
MSVehicleTransfer::loadState(const SUMOSAXAttributes& attrs, const SUMOTime offset, MSVehicleControl& vc) {
    MSVehicle* veh = dynamic_cast<MSVehicle*>(vc.getVehicle(attrs.getString(SUMO_ATTR_ID)));
    if (veh == 0) {
        // deleted
        return;
    }
    SUMOTime proceedTime = (SUMOTime)attrs.getLong(SUMO_ATTR_DEPART);
    MSLane* parkingLane = attrs.hasAttribute(SUMO_ATTR_PARKING) ? MSLane::dictionary(attrs.getString(SUMO_ATTR_PARKING)) : 0;
    myVehicles.push_back(VehicleInformation(veh, proceedTime + offset, parkingLane != 0));
    if (parkingLane != 0) {
        myParkingVehicles[parkingLane].insert(veh);
        veh->setTentativeLaneAndPosition(parkingLane, veh->getPositionOnLane());
        veh->processNextStop(veh->getSpeed());
    }
    MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);
}



/****************************************************************************/

