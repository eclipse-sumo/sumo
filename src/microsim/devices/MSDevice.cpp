/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    14.08.2013
/// @version $Id$
///
// Abstract in-vehicle device
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
#include <utils/common/TplConvert.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include "MSDevice.h"
#include "MSDevice_Vehroutes.h"
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Routing.h"
#include "MSDevice_Emissions.h"
#include "MSDevice_BTreceiver.h"
#include "MSDevice_BTsender.h"
#include "MSDevice_Example.h"
#include "MSDevice_Battery.h"
#include "MSDevice_SSM.h"
#include "MSDevice_Bluelight.h"


// ===========================================================================
// static member variables
// ===========================================================================
std::map<std::string, std::set<std::string> > MSDevice::myExplicitIDs;
std::mt19937 MSDevice::myEquipmentRNG;

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice::insertOptions(OptionsCont& oc) {
    MSDevice_Routing::insertOptions(oc);
    MSDevice_Emissions::insertOptions();
    MSDevice_BTreceiver::insertOptions(oc);
    MSDevice_BTsender::insertOptions(oc);
    MSDevice_Example::insertOptions(oc);
    MSDevice_Battery::insertOptions(oc);
    MSDevice_SSM::insertOptions(oc);
    MSDevice_Bluelight::insertOptions(oc);
}


bool
MSDevice::checkOptions(OptionsCont& oc) {
    bool ok = true;
    ok &= MSDevice_Routing::checkOptions(oc);
    return ok;
}


void
MSDevice::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    MSDevice_Vehroutes::buildVehicleDevices(v, into);
    MSDevice_Tripinfo::buildVehicleDevices(v, into);
    MSDevice_Routing::buildVehicleDevices(v, into);
    MSDevice_Emissions::buildVehicleDevices(v, into);
    MSDevice_BTreceiver::buildVehicleDevices(v, into);
    MSDevice_BTsender::buildVehicleDevices(v, into);
    MSDevice_Example::buildVehicleDevices(v, into);
    MSDevice_Battery::buildVehicleDevices(v, into);
    MSDevice_SSM::buildVehicleDevices(v, into);
    MSDevice_Bluelight::buildVehicleDevices(v, into);
}

void
MSDevice::cleanupAll() {
    MSDevice_Routing::cleanup();
    MSDevice_Tripinfo::cleanup();
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
        haveByNumber = MSNet::getInstance()->getVehicleControl().getQuota(oc.getFloat("device." + deviceName + ".probability")) == 1;
    } else {
        if (oc.exists("device." + deviceName + ".probability") && oc.getFloat("device." + deviceName + ".probability") != 0) {
            haveByNumber = RandHelper::rand(&myEquipmentRNG) <= oc.getFloat("device." + deviceName + ".probability");
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
    // assignment by abstract parameters
    bool haveByParameter = false;
    bool parameterGiven = false;
    const std::string key = "has." + deviceName + ".device";
    if (v.getParameter().knowsParameter(key)) {
        parameterGiven = true;
        haveByParameter = TplConvert::_2bool(v.getParameter().getParameter(key, "false").c_str());
    } else if (v.getVehicleType().getParameter().knowsParameter(key)) {
        parameterGiven = true;
        haveByParameter = TplConvert::_2bool(v.getVehicleType().getParameter().getParameter(key, "false").c_str());
    }
    return (haveByNumber && !parameterGiven) || haveByName || haveByParameter;
}


void
MSDevice::saveState(OutputDevice& /* out */) const {
    WRITE_WARNING("Device '" + getID() + "' cannot save state");
}


void
MSDevice::loadState(const SUMOSAXAttributes& /* attrs */) {
}


/****************************************************************************/
