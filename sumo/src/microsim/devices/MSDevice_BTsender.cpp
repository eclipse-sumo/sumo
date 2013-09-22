/****************************************************************************/
/// @file    MSDevice_BTsender.cpp
/// @author  Daniel Krajzewicz
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/SUMOVehicle.h>
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
std::set<MSVehicle*> MSDevice_BTsender::sRunningVehicles;
std::set<MSDevice_BTsender::ArrivedVehicleInformation*> MSDevice_BTsender::sArrivedVehicles;


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
    if(reason==MSMoveReminder::NOTIFICATION_TELEPORT || reason==MSMoveReminder::NOTIFICATION_PARKING || reason==MSMoveReminder::NOTIFICATION_DEPARTED) {
        sRunningVehicles.insert(static_cast<MSVehicle*>(&veh));
    }
    return true;
}


bool 
MSDevice_BTsender::notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, Notification reason) {
    if (reason >= MSMoveReminder::NOTIFICATION_TELEPORT) {
        MSVehicle *vehicle = static_cast<MSVehicle*>(&veh);
        sArrivedVehicles.insert(new ArrivedVehicleInformation(vehicle->getID(), vehicle->getSpeed(), vehicle->getPosition()));
        sRunningVehicles.erase(vehicle);
    }
    return true;
}


/****************************************************************************/

