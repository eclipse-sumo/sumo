/****************************************************************************/
/// @file    RODUAFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for dua-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>", "run routing with options from file");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Defaults");
    oc.addOptionSubTopic("Time");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too

    // insert options
    ROFrame::fillOptions(oc, true);
    addImportOptions();
    addDUAOptions();
    // add rand options
    RandHelper::insertRandOptions();
}


void
RODUAFrame::addImportOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    // register import options
    oc.doRegister("trip-files", 't', new Option_FileName());
    oc.addSynonyme("trips", "trip-files");
    oc.addSynonyme("trip-defs", "trip-files", true);
    oc.addDescription("trip-files", "Input", "Read trip-definitions from FILE(s)");

    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.addSynonyme("route-files", "sumo-input", true);
    oc.addSynonyme("route-files", "sumo", true);
    oc.addDescription("route-files", "Input", "Read sumo-routes from FILE(s)");

    // register further processing options
    // ! The subtopic "Processing" must be initialised earlier !
    oc.doRegister("weights.expand", new Option_Bool(false));
    oc.addSynonyme("weights.expand", "expand-weights", true);
    oc.addDescription("weights.expand", "Processing", "Expand weights behind the simulation's end");
}


void
RODUAFrame::addDUAOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    // register Gawron's DUE-settings
    oc.doRegister("gawron.beta", new Option_Float(SUMOReal(0.3)));
    oc.addSynonyme("gawron.beta", "gBeta", true);
    oc.addDescription("gawron.beta", "Processing", "Use FLOAT as Gawron's beta");

    oc.doRegister("gawron.a", new Option_Float(SUMOReal(0.05)));
    oc.addSynonyme("gawron.a", "gA", true);
    oc.addDescription("gawron.a", "Processing", "Use FLOAT as Gawron's a");

    oc.doRegister("exit-times", new Option_Bool(false));
    oc.addDescription("exit-times", "Output", "Write exit times (weights) for each edge");

    oc.doRegister("keep-all-routes", new Option_Bool(false));
    oc.addDescription("keep-all-routes", "Processing", "Save routes with near zero probability");

    oc.doRegister("skip-new-routes", new Option_Bool(false));
    oc.addDescription("skip-new-routes", "Processing", "Only reuse routes from input, do not calculate new ones");

    oc.doRegister("logit", new Option_Bool(false)); // deprecated
    oc.addDescription("logit", "Processing", "Use c-logit model (deprecated in favor of --route-choice-method logit)");

    oc.doRegister("route-choice-method", new Option_String("gawron"));
    oc.addDescription("route-choice-method", "Processing", "Choose a route choice method: gawron, logit, or lohse");

    oc.doRegister("logit.beta", new Option_Float(SUMOReal(-1)));
    oc.addSynonyme("logit.beta", "lBeta", true);
    oc.addDescription("logit.beta", "Processing", "Use FLOAT as logit's beta");

    oc.doRegister("logit.gamma", new Option_Float(SUMOReal(1)));
    oc.addSynonyme("logit.gamma", "lGamma", true);
    oc.addDescription("logit.gamma", "Processing", "Use FLOAT as logit's gamma");

    oc.doRegister("logit.theta", new Option_Float(SUMOReal(-1)));
    oc.addSynonyme("logit.theta", "lTheta", true);
    oc.addDescription("logit.theta", "Processing", "Use FLOAT as logit's theta (negative values mean auto-estimation)");

}


bool
RODUAFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = ROFrame::checkOptions(oc);

    SUMOVehicleParameter p;
    std::string error;
    if (oc.isSet("departlane") && !SUMOVehicleParameter::parseDepartLane(oc.getString("departlane"), "option", "departlane", p.departLane, p.departLaneProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("departpos") && !SUMOVehicleParameter::parseDepartPos(oc.getString("departpos"), "option", "departpos", p.departPos, p.departPosProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("departspeed") && !SUMOVehicleParameter::parseDepartSpeed(oc.getString("departspeed"), "option", "departspeed", p.departSpeed, p.departSpeedProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("arrivallane") && !SUMOVehicleParameter::parseArrivalLane(oc.getString("arrivallane"), "option", "arrivallane", p.arrivalLane, p.arrivalLaneProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("arrivalpos") && !SUMOVehicleParameter::parseArrivalPos(oc.getString("arrivalpos"), "option", "arrivalpos", p.arrivalPos, p.arrivalPosProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("arrivalspeed") && !SUMOVehicleParameter::parseArrivalSpeed(oc.getString("arrivalspeed"), "option", "arrivalspeed", p.arrivalSpeed, p.arrivalSpeedProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }

    if (oc.getString("routing-algorithm") != "dijkstra" && oc.getString("weight-attribute") != "traveltime") {
        WRITE_ERROR("Routing algorithm '" + oc.getString("routing-algorithm") + "' does not support weight-attribute '" + oc.getString("weight-attribute") + "'.");
        return false;
    }

    if (oc.getString("route-choice-method") != "gawron" && oc.getString("route-choice-method") != "logit") {
        WRITE_ERROR("Invalid route choice method '" + oc.getString("route-choice-method") + "'.");
        return false;
    }

    if (oc.getBool("logit")) {
        WRITE_WARNING("The --logit option is deprecated, please use --route-choice-method logit.");
        oc.set("route-choice-method", "logit");
    }
    return ok;
}



/****************************************************************************/

