/****************************************************************************/
/// @file    MSDevice.cpp
/// @author  Daniel Krajzewicz
/// @date    14.08.2013
/// @version $Id$
///
// Abstract in-vehicle device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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

#include <utils/options/OptionsCont.h>
#include <microsim/MSVehicle.h>
#include "MSDevice.h"
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Tripinfo.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_Person.h>
#include <microsim/devices/MSDevice_HBEFA.h>
#include <microsim/devices/MSDevice_Example.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
std::map<std::string, std::set<std::string> > MSDevice::myExplicitIDs;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice::insertOptions(OptionsCont& oc) {
    MSDevice_Routing::insertOptions(oc);
    MSDevice_HBEFA::insertOptions(oc);
    MSDevice_Example::insertOptions(oc);
}


void
MSDevice::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    MSDevice_Vehroutes::buildVehicleDevices(v, into);
    MSDevice_Tripinfo::buildVehicleDevices(v, into);
    MSDevice_Routing::buildVehicleDevices(v, into);
    MSDevice_HBEFA::buildVehicleDevices(v, into);
    MSDevice_Example::buildVehicleDevices(v, into);
}


void
MSDevice::insertDefaultAssignmentOptions(const std::string& deviceName, const std::string& optionsTopic, OptionsCont& oc) {
    oc.doRegister("device." + deviceName + ".probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device." + deviceName + ".probability", optionsTopic, "The probability for a vehicle to have a '" + deviceName + "' device");

    oc.doRegister("device." + deviceName + ".explicit", new Option_String());//!!! describe
    oc.addSynonyme("device." + deviceName + ".explicit", "device." + deviceName + ".knownveh", true);
    oc.addDescription("device." + deviceName + ".explicit", optionsTopic, "Assign a '" + deviceName + "' device to named vehicles");

    oc.doRegister("device." + deviceName + ".deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device." + deviceName + ".deterministic", optionsTopic, "The '" + deviceName + "' devices are set deterministic using a fraction of 1000");
}


bool
MSDevice::equippedByDefaultAssignmentOptions(const OptionsCont& oc, const std::string& deviceName, SUMOVehicle& v) {
    // assignment by number
    bool haveByNumber = false;
    if (oc.exists("device." + deviceName + ".deterministic") && oc.getBool("device." + deviceName + ".deterministic")) {
        haveByNumber = MSNet::getInstance()->getVehicleControl().isInQuota(oc.getFloat("device." + deviceName + ".probability"));
    } else {
        if (oc.exists("device." + deviceName + ".probability") && oc.getFloat("device." + deviceName + ".probability") != 0) {
            haveByNumber = RandHelper::rand() <= oc.getFloat("device." + deviceName + ".probability");
        }
    }
    // assignment by name
    bool haveByName = false;
    if (oc.exists("device." + deviceName + ".explicit") && oc.isSet("device." + deviceName + ".explicit")) {
        if (myExplicitIDs.find(deviceName) == myExplicitIDs.end()) {
            myExplicitIDs[deviceName] = std::set<std::string>();
            const std::vector<std::string> idList = OptionsCont::getOptions().getStringVector("device." + deviceName + ".explicit");
            myExplicitIDs[deviceName].insert(idList.begin(), idList.end());
        }
        haveByName = myExplicitIDs[deviceName].count(v.getID()) > 0;
    }
    return haveByNumber || haveByName;
}


/****************************************************************************/

