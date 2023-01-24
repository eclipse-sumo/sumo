/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_BTsender.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    14.08.2013
///
// A BT sender
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/transportables/MSTransportable.h>
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
MSVehicleDevice_BTsender::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("btsender", "Communication", oc);
}


void
MSVehicleDevice_BTsender::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "btsender", v, false)) {
        MSVehicleDevice_BTsender* device = new MSVehicleDevice_BTsender(v, "btsender_" + v.getID());
        into.push_back(device);
    }
}

void
MSTransportableDevice_BTsender::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("btsender", "Communication", oc, true);
}


void
MSTransportableDevice_BTsender::buildDevices(MSTransportable& t, std::vector<MSTransportableDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "btsender", t, false, true)) {
        MSTransportableDevice_BTsender* device = new MSTransportableDevice_BTsender(t, "btsender_" + t.getID());
        into.push_back(device);
        MSDevice_BTreceiver::hasSendingPersons();
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


MSDevice_BTsender::~MSDevice_BTsender() {
}

std::string
MSDevice_BTsender::getLocation(const SUMOTrafficObject& o) {
    return o.getLane() == nullptr ? o.getEdge()->getID() : o.getLane()->getID();
}

bool
MSDevice_BTsender::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
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
    sVehicles[veh.getID()]->updates.push_back(VehicleState(veh.getSpeed(), veh.getPosition(), getLocation(veh), veh.getPositionOnLane(), veh.getRoutePosition()));
    return true;
}


bool
MSDevice_BTsender::notifyMove(SUMOTrafficObject& veh, double /* oldPos */, double newPos, double newSpeed) {
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNINGF(TL("btsender: Can not update position of vehicle '%' which is not on the road."), veh.getID());
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(newSpeed, veh.getPosition(), getLocation(veh), newPos, veh.getRoutePosition()));
    return true;
}


bool
MSDevice_BTsender::notifyLeave(SUMOTrafficObject& veh, double /* lastPos */, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason < MSMoveReminder::NOTIFICATION_TELEPORT) {
        return true;
    }
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNINGF(TL("btsender: Can not update position of vehicle '%' which is not on the road."), veh.getID());
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(veh.getSpeed(), veh.getPosition(), getLocation(veh), veh.getPositionOnLane(), veh.getRoutePosition()));
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
