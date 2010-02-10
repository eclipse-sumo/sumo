/****************************************************************************/
/// @file    NLDiscreteEventBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep, 2003
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NLDiscreteEventBuilder::NLDiscreteEventBuilder(MSNet &net)
        : myNet(net) {
    myActions["SaveTLSStates"] = EV_SAVETLSTATE;
    myActions["SaveTLSSwitchTimes"] = EV_SAVETLSWITCHES;
    myActions["SaveTLSSwitchStates"] = EV_SAVETLSWITCHSTATES;
}


NLDiscreteEventBuilder::~NLDiscreteEventBuilder() {}


void
NLDiscreteEventBuilder::addAction(const SUMOSAXAttributes &attrs,
                                  const std::string &basePath) {
    string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    // check whether the type was given
    if (type=="") {
        throw InvalidArgument("An action's type is not given.");
    }
    // get the numerical representation
    KnownActions::iterator i = myActions.find(type);
    if (i==myActions.end()) {
        throw InvalidArgument("The action type '" + type + "' is not known.");
    }
    ActionType at = (*i).second;
    // build the action
    Command *a;
    switch (at) {
    case EV_SAVETLSTATE:
        a = buildSaveTLStateCommand(attrs, basePath);
        break;
    case EV_SAVETLSWITCHES:
        a = buildSaveTLSwitchesCommand(attrs, basePath);
        break;
    case EV_SAVETLSWITCHSTATES:
        a = buildSaveTLSwitchStatesCommand(attrs, basePath);
        break;
    default:
        throw InvalidArgument("Unknown trigger type.");
    }
}


Command *
NLDiscreteEventBuilder::buildSaveTLStateCommand(const SUMOSAXAttributes &attrs,
        const std::string &basePath) {
    // get the parameter
    string dest = attrs.getStringSecure(SUMO_ATTR_DEST, "");
    string source = attrs.getStringSecure(SUMO_ATTR_SOURCE, "*");
    // check the parameter
    if (dest==""||source=="") {
        throw InvalidArgument("Incomplete description of an 'SaveTLSState'-action occured.");
    }
    // get the logic
    if (!myNet.getTLSControl().knows(source)) {
        throw InvalidArgument("The traffic light logic to save (" + source +  ") is not given.");

    }
    const MSTLLogicControl::TLSLogicVariants &logics = myNet.getTLSControl().get(source);
    // build the action
    return new Command_SaveTLSState(logics, OutputDevice::getDevice(dest, basePath));
}


Command *
NLDiscreteEventBuilder::buildSaveTLSwitchesCommand(const SUMOSAXAttributes &attrs,
        const std::string &basePath) {
    // get the parameter
    string dest = attrs.getStringSecure(SUMO_ATTR_DEST, "");
    string source = attrs.getStringSecure(SUMO_ATTR_SOURCE, "*");
    // check the parameter
    if (dest==""||source=="") {
        throw InvalidArgument("Incomplete description of an 'SaveTLSState'-action occured.");
    }
    // get the logic
    if (!myNet.getTLSControl().knows(source)) {
        throw InvalidArgument("The traffic light logic to save (" + source +  ") is not given.");

    }
    const MSTLLogicControl::TLSLogicVariants &logics = myNet.getTLSControl().get(source);
    // build the action
    return new Command_SaveTLSSwitches(logics, OutputDevice::getDevice(dest, basePath));
}


Command *
NLDiscreteEventBuilder::buildSaveTLSwitchStatesCommand(const SUMOSAXAttributes &attrs,
        const std::string &basePath) {
    // get the parameter
    string dest = attrs.getStringSecure(SUMO_ATTR_DEST, "");
    string source = attrs.getStringSecure(SUMO_ATTR_SOURCE, "*");
    // check the parameter
    if (dest==""||source=="") {
        throw InvalidArgument("Incomplete description of an 'SaveTLSState'-action occured.");
    }
    // get the logic
    if (!myNet.getTLSControl().knows(source)) {
        throw InvalidArgument("The traffic light logic to save (" + source +  ") is not given.");

    }
    const MSTLLogicControl::TLSLogicVariants &logics = myNet.getTLSControl().get(source);
    // build the action
    return new Command_SaveTLSSwitchStates(logics, OutputDevice::getDevice(dest, basePath));
}



/****************************************************************************/
