/****************************************************************************/
/// @file    NLDiscreteEventBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Sep, 2003
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NLDiscreteEventBuilder.h"
#include <utils/xml/AttributesHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <microsim/MSNet.h>
#include <microsim/actions/Command_SaveTLSState.h>
#include <microsim/actions/Command_SaveTLSSwitches.h>
#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>

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
        : AttributesHandler(sumoattrs, noSumoAttrs),
        myNet(net)
{
    myActions["SaveTLSStates"] = EV_SAVETLSTATE;
    myActions["SaveTLSSwitchTimes"] = EV_SAVETLSWITCHES;
}


NLDiscreteEventBuilder::~NLDiscreteEventBuilder()
{}


void
NLDiscreteEventBuilder::addAction(const Attributes &attrs,
                                  const std::string &basePath)
{
    string type = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
    // check whether the type was given
    if (type=="") {
        MsgHandler::getErrorInstance()->inform("An action's type is not given.");
        return;
    }
    // get the numerical representation
    KnownActions::iterator i = myActions.find(type);
    if (i==myActions.end()) {
        MsgHandler::getErrorInstance()->inform("The action type '" + type + "' is not known.");
        return;
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
    default:
        throw 1;
    }
}


Command *
NLDiscreteEventBuilder::buildSaveTLStateCommand(const Attributes &attrs,
        const std::string &basePath)
{
    // get the parameter
    string dest = getStringSecure(attrs, SUMO_ATTR_DEST, "");
    string source = getStringSecure(attrs, SUMO_ATTR_SOURCE, "*");
    // check the parameter
    if (dest==""||source=="") {
        MsgHandler::getErrorInstance()->inform("Incomplete description of an 'SaveTLSState'-action occured.");
        return 0;
    }
    if (!FileHelpers::isAbsolute(dest)) {
        dest = FileHelpers::getConfigurationRelative(basePath, dest);
    }
    // get the logics
    if (!myNet.getTLSControl().knows(source)) {
        MsgHandler::getErrorInstance()->inform("The traffic light logic to save (" + source +  ") is not given.");
        throw ProcessError();
    }
    const MSTLLogicControl::TLSLogicVariants &logics = myNet.getTLSControl().get(source);
    // build the action
    return new Command_SaveTLSState(logics, dest);
}


Command *
NLDiscreteEventBuilder::buildSaveTLSwitchesCommand(const Attributes &attrs,
        const std::string &basePath)
{
    // get the parameter
    string dest = getStringSecure(attrs, SUMO_ATTR_DEST, "");
    string source = getStringSecure(attrs, SUMO_ATTR_SOURCE, "*");
    // check the parameter
    if (dest==""||source=="") {
        MsgHandler::getErrorInstance()->inform("Incomplete description of an 'SaveTLSState'-action occured.");
        return 0;
    }
    if (!FileHelpers::isAbsolute(dest)) {
        dest = FileHelpers::getConfigurationRelative(basePath, dest);
    }
    // get the logics
    if (!myNet.getTLSControl().knows(source)) {
        MsgHandler::getErrorInstance()->inform("The traffic light logic to save (" + source +  ") is not given.");
        throw ProcessError();
    }
    const MSTLLogicControl::TLSLogicVariants &logics = myNet.getTLSControl().get(source);
    // build the action
    return new Command_SaveTLSSwitches(logics, dest);
}



/****************************************************************************/

