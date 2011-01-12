/****************************************************************************/
/// @file    RODUAFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for dua-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include "RODUAFrame.h"
#include <router/ROFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/SUMOVehicleParameter.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
RODUAFrame::fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Defaults");
    oc.addOptionSubTopic("Time");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too

    // insert options
    ROFrame::fillOptions(oc);
    addImportOptions();
    addDUAOptions();
    // add rand options
    RandHelper::insertRandOptions();
}


void
RODUAFrame::addImportOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    // register import options
    oc.doRegister("trip-defs", 't', new Option_FileName());
    oc.addSynonyme("trips", "trip-defs");
    oc.addDescription("trip-defs", "Input", "Read trip-definitions from FILE");

    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.addSynonyme("flow-definition", "flows");
    oc.addDescription("flow-definition", "Input", "Read flow-definitions from FILE");

    oc.doRegister("sumo-input", 's', new Option_FileName());
    oc.addSynonyme("sumo", "sumo-input");
    oc.addDescription("sumo-input", "Input", "Read sumo-routes from FILE");

    // register further processing options
    // ! The subtopic "Processing" must be initialised earlier !
    oc.doRegister("expand-weights", new Option_Bool(false));
    oc.addDescription("expand-weights", "Processing", "Expand weights behind the simulation's end");
}


void
RODUAFrame::addDUAOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    // register Gawron's DUE-settings
    oc.doRegister("gBeta", new Option_Float(SUMOReal(0.3)));
    oc.addDescription("gBeta", "Processing", "Use FLOAT as Gawron's beta");

    oc.doRegister("gA", new Option_Float(SUMOReal(0.05)));
    oc.addDescription("gA", "Processing", "Use FLOAT as Gawron's alpha");

    oc.doRegister("exit-times", new Option_Bool(false));
    oc.addDescription("exit-times", "Output", "Write exit times (weights) for each edge");

    oc.doRegister("keep-all-routes", new Option_Bool(false));
    oc.addDescription("keep-all-routes", "Processing", "Save routes with near zero probability");

    oc.doRegister("logit", new Option_Bool(false));
    oc.addDescription("logit", "Processing", "Use c-logit model");

    oc.doRegister("lBeta", new Option_Float(SUMOReal(0.15)));
    oc.addDescription("lBeta", "Processing", "Use FLOAT as logit's beta");

    oc.doRegister("lGamma", new Option_Float(SUMOReal(1)));
    oc.addDescription("lGamma", "Processing", "Use FLOAT as logit's gamma");

}


bool
RODUAFrame::checkOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    bool ok = ROFrame::checkOptions(oc);
    ok &= (!oc.isSet("departlane") || SUMOVehicleParameter::departlaneValidate(oc.getString("departlane")));
    ok &= (!oc.isSet("departpos") || SUMOVehicleParameter::departposValidate(oc.getString("departpos")));
    ok &= (!oc.isSet("departspeed") || SUMOVehicleParameter::departspeedValidate(oc.getString("departspeed")));
    ok &= (!oc.isSet("arrivallane") || SUMOVehicleParameter::arrivallaneValidate(oc.getString("arrivallane")));
    ok &= (!oc.isSet("arrivalpos") || SUMOVehicleParameter::arrivalposValidate(oc.getString("arrivalpos")));
    ok &= (!oc.isSet("arrivalspeed") || SUMOVehicleParameter::arrivalspeedValidate(oc.getString("arrivalspeed")));
    return ok;
}



/****************************************************************************/

