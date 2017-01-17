/****************************************************************************/
/// @file    NLDiscreteEventBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sep, 2003
/// @version $Id$
///
// }
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

#include "NLDiscreteEventBuilder.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <microsim/MSNet.h>
#include <microsim/actions/Command_SaveTLSState.h>
#include <microsim/actions/Command_SaveTLSSwitches.h>
#include <microsim/actions/Command_SaveTLSSwitchStates.h>
#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLDiscreteEventBuilder::NLDiscreteEventBuilder(MSNet& net)
    : myNet(net) {
    myActions["SaveTLSStates"] = EV_SAVETLSTATE;
    myActions["SaveTLSSwitchTimes"] = EV_SAVETLSWITCHES;
    myActions["SaveTLSSwitchStates"] = EV_SAVETLSWITCHSTATES;
}


NLDiscreteEventBuilder::~NLDiscreteEventBuilder() {}


void
NLDiscreteEventBuilder::addAction(const SUMOSAXAttributes& attrs,
                                  const std::string& basePath) {
    bool ok = true;
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, 0, ok, "");
    // check whether the type was given
    if (type == "" || !ok) {
        throw InvalidArgument("An action's type is not given.");
    }
    // get the numerical representation
    KnownActions::iterator i = myActions.find(type);
    if (i == myActions.end()) {
        throw InvalidArgument("The action type '" + type + "' is not known.");
    }
    // build the action
    switch ((*i).second) {
        case EV_SAVETLSTATE:
            buildSaveTLStateCommand(attrs, basePath);
            break;
        case EV_SAVETLSWITCHES:
            buildSaveTLSwitchesCommand(attrs, basePath);
            break;
        case EV_SAVETLSWITCHSTATES:
            buildSaveTLSwitchStatesCommand(attrs, basePath);
            break;
    }
}


void
NLDiscreteEventBuilder::buildSaveTLStateCommand(const SUMOSAXAttributes& attrs,
        const std::string& basePath) {
    bool ok = true;
    const std::string dest = attrs.getOpt<std::string>(SUMO_ATTR_DEST, 0, ok, "");
    const std::string source = attrs.getOpt<std::string>(SUMO_ATTR_SOURCE, 0, ok, "");
    // check the parameter
    if (dest == "" || !ok) {
        throw InvalidArgument("Incomplete description of an 'SaveTLSState'-action occured.");
    }
    if (source == "") {
        const std::vector<std::string> ids = myNet.getTLSControl().getAllTLIds();
        for (std::vector<std::string>::const_iterator tls = ids.begin(); tls != ids.end(); ++tls) {
            const MSTLLogicControl::TLSLogicVariants& logics = myNet.getTLSControl().get(*tls);
            new Command_SaveTLSState(logics, OutputDevice::getDevice(FileHelpers::checkForRelativity(dest, basePath)));
        }
    } else {
        // get the logic
        if (!myNet.getTLSControl().knows(source)) {
            throw InvalidArgument("The traffic light logic to save (" + source +  ") is not known.");
        }
        const MSTLLogicControl::TLSLogicVariants& logics = myNet.getTLSControl().get(source);
        // build the action
        new Command_SaveTLSState(logics, OutputDevice::getDevice(FileHelpers::checkForRelativity(dest, basePath)));
    }
}


void
NLDiscreteEventBuilder::buildSaveTLSwitchesCommand(const SUMOSAXAttributes& attrs,
        const std::string& basePath) {
    bool ok = true;
    const std::string dest = attrs.getOpt<std::string>(SUMO_ATTR_DEST, 0, ok, "");
    const std::string source = attrs.getOpt<std::string>(SUMO_ATTR_SOURCE, 0, ok, "");
    // check the parameter
    if (dest == "" || !ok) {
        throw InvalidArgument("Incomplete description of an 'SaveTLSSwitchTimes'-action occured.");
    }
    if (source == "") {
        const std::vector<std::string> ids = myNet.getTLSControl().getAllTLIds();
        for (std::vector<std::string>::const_iterator tls = ids.begin(); tls != ids.end(); ++tls) {
            const MSTLLogicControl::TLSLogicVariants& logics = myNet.getTLSControl().get(*tls);
            new Command_SaveTLSSwitches(logics, OutputDevice::getDevice(FileHelpers::checkForRelativity(dest, basePath)));
        }
    } else {
        // get the logic
        if (!myNet.getTLSControl().knows(source)) {
            throw InvalidArgument("The traffic light logic to save (" + source +  ") is not known.");
        }
        const MSTLLogicControl::TLSLogicVariants& logics = myNet.getTLSControl().get(source);
        // build the action
        new Command_SaveTLSSwitches(logics, OutputDevice::getDevice(FileHelpers::checkForRelativity(dest, basePath)));
    }
}


void
NLDiscreteEventBuilder::buildSaveTLSwitchStatesCommand(const SUMOSAXAttributes& attrs,
        const std::string& basePath) {
    bool ok = true;
    const std::string dest = attrs.getOpt<std::string>(SUMO_ATTR_DEST, 0, ok, "");
    const std::string source = attrs.getOpt<std::string>(SUMO_ATTR_SOURCE, 0, ok, "");
    // check the parameter
    if (dest == "" || !ok) {
        throw InvalidArgument("Incomplete description of an 'SaveTLSSwitchStates'-action occured.");
    }
    if (source == "") {
        const std::vector<std::string> ids = myNet.getTLSControl().getAllTLIds();
        for (std::vector<std::string>::const_iterator tls = ids.begin(); tls != ids.end(); ++tls) {
            const MSTLLogicControl::TLSLogicVariants& logics = myNet.getTLSControl().get(*tls);
            new Command_SaveTLSSwitchStates(logics, OutputDevice::getDevice(FileHelpers::checkForRelativity(dest, basePath)));
        }
    } else {
        // get the logic
        if (!myNet.getTLSControl().knows(source)) {
            throw InvalidArgument("The traffic light logic to save (" + source +  ") is not known.");
        }
        const MSTLLogicControl::TLSLogicVariants& logics = myNet.getTLSControl().get(source);
        // build the action
        new Command_SaveTLSSwitchStates(logics, OutputDevice::getDevice(FileHelpers::checkForRelativity(dest, basePath)));
    }
}


/****************************************************************************/
