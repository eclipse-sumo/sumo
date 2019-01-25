/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROJTRFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for jtr-routing
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include "ROJTRFrame.h"
#include <router/ROFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/vehicle/SUMOVehicleParameter.h>


// ===========================================================================
// method definitions
// ===========================================================================
void
ROJTRFrame::fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>", "run with routing options defined in file");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Defaults");
    oc.addOptionSubTopic("Time");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too

    ROFrame::fillOptions(oc);

    oc.doRegister("turn-ratio-files", 't', new Option_FileName());
    oc.addSynonyme("turn-ratio-files", "turns");
    oc.addDescription("turn-ratio-files", "Input", "Read turning ratios from FILE(s)");

    oc.doRegister("exit-times", new Option_Bool(false));
    oc.addDescription("exit-times", "Output", "Write exit times (weights) for each edge");

    oc.doRegister("max-edges-factor", new Option_Float(2.0));
    oc.addDescription("max-edges-factor", "Processing", "Routes are cut off when the route edges to net edges ratio is larger than FLOAT");

    oc.doRegister("turn-defaults", 'T', new Option_String("30,50,20"));
    oc.addDescription("turn-defaults", "Processing", "Use STR as default turn definition");

    oc.doRegister("sink-edges", new Option_String());
    oc.addSynonyme("sink-edges", "sinks");
    oc.addDescription("sink-edges", "Processing", "Use STR as list of sink edges");

    oc.doRegister("accept-all-destinations", 'A', new Option_Bool(false));
    oc.addDescription("accept-all-destinations", "Processing", "Whether all edges are allowed as sink edges");

    oc.doRegister("ignore-vclasses", 'i', new Option_Bool(false));
    oc.addDescription("ignore-vclasses", "Processing", "Ignore road restrictions based on vehicle class");

    oc.doRegister("allow-loops", new Option_Bool(false));
    oc.addDescription("allow-loops", "Processing", "Allow to re-use a road");

    // add rand options
    RandHelper::insertRandOptions();
}


bool
ROJTRFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = ROFrame::checkOptions(oc);
    if (oc.isDefault("no-internal-links")) {
        oc.set("no-internal-links", "true");
    }

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
    return ok;
}


/****************************************************************************/
