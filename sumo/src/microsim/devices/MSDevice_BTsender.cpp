/****************************************************************************/
/// @file    MSDevice_BTsender.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/TplConvert.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
MSDevice_BTsender::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "btsender", v)) {
        MSDevice_BTsender* device = new MSDevice_BTsender(v, "btsender_" + v.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTsender-methods
// ---------------------------------------------------------------------------
MSDevice_BTsender::MSDevice_BTsender(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id) {
}


MSDevice_BTsender::~MSDevice_BTsender() {
}


bool
MSDevice_BTsender::notifyEnter(SUMOVehicle& veh, Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED && sVehicles.find(veh.getID()) == sVehicles.end()) {
        sVehicles[veh.getID()] = new VehicleInformation(veh.getID());
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(veh.getSpeed(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), veh.getPositionOnLane()));
    return true;
}


bool
MSDevice_BTsender::notifyMove(SUMOVehicle& veh, SUMOReal /* oldPos */, SUMOReal newPos, SUMOReal newSpeed) {
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btsender: Can not update position of a vehicle that is not within the road network (" + veh.getID() + ").");
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(newSpeed, static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), newPos));
    return true;
}


bool
MSDevice_BTsender::notifyLeave(SUMOVehicle& veh, SUMOReal /* lastPos */, Notification reason) {
    if (reason < MSMoveReminder::NOTIFICATION_TELEPORT) {
        return true;
    }
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btsender: Can not update position of a vehicle that is not within the road network (" + veh.getID() + ").");
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(veh.getSpeed(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), veh.getPositionOnLane()));
    if (reason >= MSMoveReminder::NOTIFICATION_TELEPORT) {
        sVehicles[veh.getID()]->amOnNet = false;
    }
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        sVehicles[veh.getID()]->haveArrived = true;
    }
    return true;
}


/****************************************************************************/

