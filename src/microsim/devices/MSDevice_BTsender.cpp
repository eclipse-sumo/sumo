/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_BTsender.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_BTsender.h"
#include "MSDevice_BTreceiver.h"


// ===========================================================================
// static members
// ===========================================================================
std::map<std::string, MSDevice_BTsender::VehicleInformation*> MSDevice_BTsender::sVehicles;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_BTsender::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("btsender", "Communication", oc);
}


void
MSDevice_BTsender::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "btsender", v, false)) {
        MSDevice_BTsender* device = new MSDevice_BTsender(v, "btsender_" + v.getID());
        into.push_back(device);
    }
}

void
MSDevice_BTsender::cleanup() {
    std::map<std::string, MSDevice_BTsender::VehicleInformation*>::iterator i;
    for (i = sVehicles.begin(); i != sVehicles.end(); i++) {
        delete i->second;
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTsender-methods
// ---------------------------------------------------------------------------
MSDevice_BTsender::MSDevice_BTsender(SUMOVehicle& holder, const std::string& id)
    : MSVehicleDevice(holder, id) {
}


MSDevice_BTsender::~MSDevice_BTsender() {
}


bool
MSDevice_BTsender::notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED && sVehicles.find(veh.getID()) == sVehicles.end()) {
        sVehicles[veh.getID()] = new VehicleInformation(veh.getID());
        sVehicles[veh.getID()]->route.push_back(veh.getEdge());
    }
    if (reason == MSMoveReminder::NOTIFICATION_TELEPORT && sVehicles.find(veh.getID()) != sVehicles.end()) {
        sVehicles[veh.getID()]->amOnNet = true;
    }
    if (reason == MSMoveReminder::NOTIFICATION_TELEPORT || reason == MSMoveReminder::NOTIFICATION_JUNCTION) {
        sVehicles[veh.getID()]->route.push_back(veh.getEdge());
    }
    const MSVehicle& v = static_cast<MSVehicle&>(veh);
    sVehicles[veh.getID()]->updates.push_back(VehicleState(veh.getSpeed(), veh.getPosition(), v.getLane()->getID(), veh.getPositionOnLane(), v.getRoutePosition()));
    return true;
}


bool
MSDevice_BTsender::notifyMove(SUMOTrafficObject& veh, double /* oldPos */, double newPos, double newSpeed) {
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btsender: Can not update position of vehicle '" + veh.getID() + "' which is not on the road.");
        return true;
    }
    const MSVehicle& v = static_cast<MSVehicle&>(veh);
    sVehicles[veh.getID()]->updates.push_back(VehicleState(newSpeed, veh.getPosition(), v.getLane()->getID(), newPos, v.getRoutePosition()));
    return true;
}


bool
MSDevice_BTsender::notifyLeave(SUMOTrafficObject& veh, double /* lastPos */, Notification reason, const MSLane* /* enteredLane */) {
    if (reason < MSMoveReminder::NOTIFICATION_TELEPORT) {
        return true;
    }
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btsender: Can not update position of vehicle '" + veh.getID() + "' which is not on the road.");
        return true;
    }
    const MSVehicle& v = static_cast<MSVehicle&>(veh);
    sVehicles[veh.getID()]->updates.push_back(VehicleState(veh.getSpeed(), veh.getPosition(), v.getLane()->getID(), veh.getPositionOnLane(), v.getRoutePosition()));
    if (reason >= MSMoveReminder::NOTIFICATION_TELEPORT) {
        sVehicles[veh.getID()]->amOnNet = false;
    }
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        sVehicles[veh.getID()]->amOnNet = false;
        sVehicles[veh.getID()]->haveArrived = true;
    }
    return true;
}


/****************************************************************************/

