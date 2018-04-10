/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// static member definitions
// ===========================================================================
MSVehicleTransfer* MSVehicleTransfer::myInstance = 0;
const double MSVehicleTransfer::TeleportMinSpeed = 1;
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
    myVehicles.push_back(VehicleInformation(t, veh,
                                            t + TIME2STEPS(veh->getEdge()->getCurrentTravelTime(TeleportMinSpeed)),
                                            veh->isParking()));
}


void
MSVehicleTransfer::remove(MSVehicle* veh) {
    for (VehicleInfVector::iterator i = myVehicles.begin(); i != myVehicles.end(); ++i) {
        if (i->myVeh == veh) {
            if (i->myParking) {
                veh->getLane()->removeParking(veh);
            }
            myVehicles.erase(i);
            break;
        }
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
            if (time != desc.myTransferTime) {
                // avoid calling processNextStop twice in the transfer step
                desc.myVeh->processNextStop(1);
            }
            if (desc.myVeh->keepStopping(true)) {
                i++;
                continue;
            }
            // parking finished, head back into traffic
        }
        const SUMOVehicleClass vclass = desc.myVeh->getVehicleType().getVehicleClass();
        const MSEdge* e = desc.myVeh->getEdge();
        const MSEdge* nextEdge = desc.myVeh->succEdge(1);

        const double departPos = desc.myParking ? desc.myVeh->getPositionOnLane() : 0;

        if (desc.myParking) {
            // handle parking vehicles
            if (desc.myVeh->getLane()->isInsertionSuccess(desc.myVeh, 0, departPos, desc.myVeh->getLateralPositionOnLane(),
                    false, MSMoveReminder::NOTIFICATION_PARKING)) {
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VEHICLE_STATE_ENDING_PARKING);
                desc.myVeh->getLane()->removeParking(desc.myVeh);
                i = myVehicles.erase(i);
            } else {
                i++;
            }
        } else {
            // get the lane on which this vehicle should continue
            // first select all the lanes which allow continuation onto nextEdge
            //   then pick the one which is least occupied
            MSLane* l = (nextEdge != 0 ? e->getFreeLane(e->allowedLanes(*nextEdge, vclass), vclass, departPos) :
                         e->getFreeLane(0, vclass, departPos));
            // handle teleporting vehicles, lane may be 0 because permissions were modified by a closing rerouter or TraCI
            if (l != 0 && l->freeInsertion(*(desc.myVeh), MIN2(l->getSpeedLimit(), desc.myVeh->getMaxSpeed()), 0, MSMoveReminder::NOTIFICATION_TELEPORT)) {
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


void
MSVehicleTransfer::saveState(OutputDevice& out) const {
    for (VehicleInfVector::const_iterator it = myVehicles.begin(); it != myVehicles.end(); ++it) {
        out.openTag(SUMO_TAG_VEHICLETRANSFER);
        out.writeAttr(SUMO_ATTR_ID, it->myVeh->getID());
        out.writeAttr(SUMO_ATTR_DEPART, it->myProceedTime);
        if (it->myParking) {
            out.writeAttr(SUMO_ATTR_PARKING, it->myVeh->getLane()->getID());
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
    myVehicles.push_back(VehicleInformation(-1, veh, proceedTime - offset, parkingLane != 0));
    if (parkingLane != 0) {
        parkingLane->addParking(veh);
        veh->setTentativeLaneAndPosition(parkingLane, veh->getPositionOnLane());
        veh->processNextStop(veh->getSpeed());
    }
    MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);
}



/****************************************************************************/

