/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSVehicleTransfer.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sep 2003
///
// A mover of vehicles that got stucked due to grid locks
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "MSNet.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSVehicle.h"
#include "MSParkingArea.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSVehicleControl.h"
#include "MSInsertionControl.h"
#include "MSVehicleTransfer.h"


// ===========================================================================
// static member definitions
// ===========================================================================
MSVehicleTransfer* MSVehicleTransfer::myInstance = nullptr;
const double MSVehicleTransfer::TeleportMinSpeed = 1;


// ===========================================================================
// member method definitions
// ===========================================================================
bool
MSVehicleTransfer::VehicleInformation::operator<(const VehicleInformation& v2) const {
    return myVeh->getNumericalID() < v2.myVeh->getNumericalID();
}


void
MSVehicleTransfer::add(const SUMOTime t, MSVehicle* veh) {
    const bool jumping = veh->isJumping();
    const SUMOTime proceed = jumping ? t + veh->getPastStops().back().jump : -1;
    if (veh->isParking()) {
        veh->getLaneChangeModel().endLaneChangeManeuver(MSMoveReminder::NOTIFICATION_PARKING);
        MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VehicleState::STARTING_PARKING);
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_PARKING);
    } else {
        veh->getLaneChangeModel().endLaneChangeManeuver(MSMoveReminder::NOTIFICATION_TELEPORT);
        MSNet::getInstance()->informVehicleStateListener(veh, MSNet::VehicleState::STARTING_TELEPORT);
        if (veh->succEdge(1) == nullptr) {
            WRITE_WARNINGF(TL("Vehicle '%' teleports beyond arrival edge '%', time=%."), veh->getID(), veh->getEdge()->getID(), time2string(t));
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            return;
        }
        veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
        veh->enterLaneAtMove(veh->succEdge(1)->getLanes()[0], true);
    }
    myVehicles.push_back(VehicleInformation(t, veh, proceed, veh->isParking(), jumping));
}


void
MSVehicleTransfer::remove(MSVehicle* veh) {
    auto& vehInfos = myVehicles.getContainer();
    for (auto i = vehInfos.begin(); i != vehInfos.end(); ++i) {
        if (i->myVeh == veh) {
            if (i->myParking) {
                veh->getMutableLane()->removeParking(veh);
            }
            vehInfos.erase(i);
            break;
        }
    }
    myVehicles.unlock();
}


void
MSVehicleTransfer::checkInsertions(SUMOTime time) {
    // go through vehicles
    auto& vehInfos = myVehicles.getContainer();
    std::sort(vehInfos.begin(), vehInfos.end());
    for (auto i = vehInfos.begin(); i != vehInfos.end();) {
        // vehicle information cannot be const because we need to assign the proceed time
        VehicleInformation& desc = *i;

        if (desc.myParking) {
            // handle parking vehicles
            if (time != desc.myTransferTime) {
                // avoid calling processNextStop twice in the transfer step
                const MSLane* lane = desc.myVeh->getLane();
                // lane must be locked because pedestrians may be added in during stop processing while existing passengers are being drawn simultaneously
                if (lane != nullptr) {
                    lane->getVehiclesSecure();
                }
                desc.myVeh->processNextStop(1);
                desc.myVeh->updateParkingState();
                if (lane != nullptr) {
                    lane->releaseVehicles();
                }
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


        if (desc.myParking) {
            MSParkingArea* pa = desc.myVeh->getCurrentParkingArea();
            const double departPos = pa != nullptr ? pa->getInsertionPosition(*desc.myVeh) : desc.myVeh->getPositionOnLane();
            // handle parking vehicles
            desc.myVeh->setIdling(true);
            if (desc.myVeh->getMutableLane()->isInsertionSuccess(desc.myVeh, 0, departPos, desc.myVeh->getLateralPositionOnLane(),
                    false, MSMoveReminder::NOTIFICATION_PARKING)) {
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VehicleState::ENDING_PARKING);
                desc.myVeh->getMutableLane()->removeParking(desc.myVeh);
                // at this point we are in the lane, blocking traffic & if required we configure the exit manoeuvre
                if (MSGlobals::gModelParkingManoeuver && desc.myVeh->setExitManoeuvre()) {
                    MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VehicleState::MANEUVERING);
                }
                desc.myVeh->setIdling(false);
                i = vehInfos.erase(i);
            } else {
                // blocked from entering the road - engine assumed to be idling.
                desc.myVeh->workOnIdleReminders();
                if (!desc.myVeh->signalSet(MSVehicle::VEH_SIGNAL_BLINKER_LEFT | MSVehicle::VEH_SIGNAL_BLINKER_RIGHT)) {
                    // signal wish to re-enter the road
                    desc.myVeh->switchOnSignal(MSGlobals::gLefthand ? MSVehicle::VEH_SIGNAL_BLINKER_RIGHT : MSVehicle::VEH_SIGNAL_BLINKER_LEFT);
                    if (pa) {
                        // update freePosition so other vehicles can help with insertion
                        desc.myVeh->getCurrentParkingArea()->notifyEgressBlocked();
                    }
                }
                i++;
            }
        } else if (desc.myJumping && desc.myProceedTime > time) {
            ++i;
        } else {
            const double departPos = 0;
            // get the lane on which this vehicle should continue
            // first select all the lanes which allow continuation onto nextEdge
            //   then pick the one which is least occupied
            MSLane* l = (nextEdge != nullptr ? e->getFreeLane(e->allowedLanes(*nextEdge, vclass), vclass, departPos) :
                         e->getFreeLane(nullptr, vclass, departPos));
            // handle teleporting vehicles, lane may be 0 because permissions were modified by a closing rerouter or TraCI
            if (l != nullptr && l->freeInsertion(*(desc.myVeh), MIN2(l->getSpeedLimit(), desc.myVeh->getMaxSpeed()), 0, MSMoveReminder::NOTIFICATION_TELEPORT)) {
                if (!desc.myJumping) {
                    WRITE_WARNINGF(TL("Vehicle '%' ends teleporting on edge '%', time=%."), desc.myVeh->getID(), e->getID(), time2string(time));
                }
                MSNet::getInstance()->informVehicleStateListener(desc.myVeh, MSNet::VehicleState::ENDING_TELEPORT);
                i = vehInfos.erase(i);
            } else {
                // vehicle is visible while show-route is active. Make it's state more obvious
                desc.myVeh->computeAngle();
                desc.myVeh->setLateralPositionOnLane(-desc.myVeh->getLane()->getWidth() / 2);
                desc.myVeh->invalidateCachedPosition();
                // could not insert. maybe we should proceed in virtual space
                if (desc.myProceedTime < 0) {
                    // initialize proceed time (delayed to avoid lane-order dependency in executeMove)
                    desc.myProceedTime = time + TIME2STEPS(e->getCurrentTravelTime(TeleportMinSpeed));
                } else if (desc.myProceedTime < time) {
                    if (desc.myVeh->succEdge(1) == nullptr) {
                        WRITE_WARNINGF(TL("Vehicle '%' teleports beyond arrival edge '%', time=%."), desc.myVeh->getID(), e->getID(), time2string(time));
                        desc.myVeh->leaveLane(MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED);
                        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(desc.myVeh);
                        i = vehInfos.erase(i);
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
    myVehicles.unlock();
}


MSVehicleTransfer*
MSVehicleTransfer::getInstance() {
    if (myInstance == nullptr) {
        myInstance = new MSVehicleTransfer();
    }
    return myInstance;
}


MSVehicleTransfer::MSVehicleTransfer() : myVehicles(MSGlobals::gNumSimThreads > 1) {}


MSVehicleTransfer::~MSVehicleTransfer() {
    myInstance = nullptr;
}


void
MSVehicleTransfer::saveState(OutputDevice& out) {
    for (const VehicleInformation& vehInfo : myVehicles.getContainer()) {
        out.openTag(SUMO_TAG_VEHICLETRANSFER);
        out.writeAttr(SUMO_ATTR_ID, vehInfo.myVeh->getID());
        out.writeAttr(SUMO_ATTR_DEPART, vehInfo.myProceedTime);
        if (vehInfo.myParking) {
            out.writeAttr(SUMO_ATTR_PARKING, vehInfo.myVeh->getLane()->getID());
        }
        if (vehInfo.myJumping) {
            out.writeAttr(SUMO_ATTR_JUMP, true);
        }
        out.closeTag();
    }
    myVehicles.unlock();
}


void
MSVehicleTransfer::clearState() {
    myVehicles.clear();
}


void
MSVehicleTransfer::loadState(const SUMOSAXAttributes& attrs, const SUMOTime offset, MSVehicleControl& vc) {
    MSVehicle* veh = dynamic_cast<MSVehicle*>(vc.getVehicle(attrs.getString(SUMO_ATTR_ID)));
    if (veh == nullptr) {
        // deleted
        return;
    }
    SUMOTime proceedTime = (SUMOTime)attrs.getLong(SUMO_ATTR_DEPART);
    MSLane* parkingLane = attrs.hasAttribute(SUMO_ATTR_PARKING) ? MSLane::dictionary(attrs.getString(SUMO_ATTR_PARKING)) : nullptr;
    bool ok = true;
    const bool jumping = attrs.getOpt<bool>(SUMO_ATTR_JUMP, veh->getID().c_str(), ok, false);
    myVehicles.push_back(VehicleInformation(-1, veh, proceedTime - offset, parkingLane != nullptr, jumping));
    if (parkingLane != nullptr) {
        parkingLane->addParking(veh);
        veh->setTentativeLaneAndPosition(parkingLane, veh->getPositionOnLane());
        veh->processNextStop(veh->getSpeed());
    }
    MSNet::getInstance()->getInsertionControl().alreadyDeparted(veh);
}


/****************************************************************************/
