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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include "ROJTRFrame.h"
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

    ROFrame::fillOptions(oc, false);

    oc.doRegister("turn-ratio-files", 't', new Option_FileName());
    oc.addSynonyme("turn-ratio-files", "turns");
    oc.addSynonyme("turn-ratio-files", "turn-definitions", true);
    oc.addDescription("turn-ratio-files", "Input", "Read turning ratios from FILE(s)");

    oc.doRegister("exit-times", new Option_Bool(false));
    oc.addDescription("exit-times", "Output", "Write exit times (weights) for each edge");

    oc.doRegister("max-edges-factor", new Option_Float(2.0));
    oc.addDescription("max-edges-factor", "Processing", "");

    oc.doRegister("turn-defaults", 'T', new Option_String("30,50,20"));
    oc.addDescription("turn-defaults", "Processing", "Use STR as default turn definition");

    oc.doRegister("sink-edges", 's', new Option_String());
    oc.addSynonyme("sink-edges", "sinks");
    oc.addDescription("sink-edges", "Processing", "Use STR as list of sink edges");

    oc.doRegister("accept-all-destinations", 'A', new Option_Bool(false));
    oc.addDescription("accept-all-destinations", "Processing", "Whether all edges are allowed as sink edges");

    oc.doRegister("ignore-vclasses", 'i', new Option_Bool(false));
    oc.addSynonyme("ignore-vclasses", "ignore-classes", true);
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
    ok &= (!oc.isSet("departlane") || SUMOVehicleParameter::departlaneValidate(oc.getString("departlane")));
    ok &= (!oc.isSet("departpos") || SUMOVehicleParameter::departposValidate(oc.getString("departpos")));
    ok &= (!oc.isSet("departspeed") || SUMOVehicleParameter::departspeedValidate(oc.getString("departspeed")));
    ok &= (!oc.isSet("arrivallane") || SUMOVehicleParameter::arrivallaneValidate(oc.getString("arrivallane")));
    ok &= (!oc.isSet("arrivalpos") || SUMOVehicleParameter::arrivalposValidate(oc.getString("arrivalpos")));
    ok &= (!oc.isSet("arrivalspeed") || SUMOVehicleParameter::arrivalspeedValidate(oc.getString("arrivalspeed")));
    return ok;
}



/****************************************************************************/

