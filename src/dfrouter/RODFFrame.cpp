/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    RODFFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Thu, 16.03.2006
///
// Sets and checks options for df-routing
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/SystemFrame.h>
#include "RODFFrame.h"
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>



// ===========================================================================
// method definitions
// ===========================================================================
void
RODFFrame::fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>", "run routing with options from file");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Defaults");
    oc.addOptionSubTopic("Time");

    // register the options
    // register input-options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", TL("Loads the SUMO-network FILE"));

    /* never used
    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.addDescription("route-files", "Input", TL("Loads SUMO-routes from FILE"));
    */

    oc.doRegister("detector-files", 'd', new Option_FileName());
    oc.addSynonyme("detector-files", "detectors");
    oc.addDescription("detector-files", "Input", TL("Loads detector descriptions from FILE"));

    oc.doRegister("measure-files", 'f', new Option_FileName());
    oc.addSynonyme("measure-files", "detflows");
    oc.addSynonyme("measure-files", "detector-flow-files", true);
    oc.addDescription("measure-files", "Input", TL("Loads detector flows from FILE(s)"));

    // need to do this here to be able to check for network and route input options
    SystemFrame::addReportOptions(oc);

    // register output options
    oc.doRegister("routes-output", 'o', new Option_FileName());
    oc.addDescription("routes-output", "Output", TL("Saves computed routes to FILE"));

    oc.doRegister("routes-for-all", new Option_Bool(false));
    oc.addDescription("routes-for-all", "Output", TL("Forces dfrouter to compute routes for in-between detectors"));

    oc.doRegister("detector-output", new Option_FileName());
    oc.addSynonyme("detector-output", "detectors-output", true);
    oc.addDescription("detector-output", "Output", TL("Saves typed detectors to FILE"));

    oc.doRegister("detectors-poi-output", new Option_FileName());
    oc.addDescription("detectors-poi-output", "Output", TL("Saves detector positions as pois to FILE"));

    oc.doRegister("emitters-output", new Option_FileName());
    oc.addDescription("emitters-output", "Output", TL("Saves emitter definitions for source detectors to FILE"));

    oc.doRegister("vtype", new Option_Bool(false));
    oc.addDescription("vtype", "Output", TL("Add vehicle types to the emitters file (PKW, LKW)"));

    oc.doRegister("vtype-output", new Option_FileName(StringVector({ "" })));
    oc.addDescription("vtype-output", "Output", TL("Write generated vehicle types into separate FILE instead of including them into the emitters-output"));

    oc.doRegister("emitters-poi-output", new Option_FileName()); // !!! describe
    oc.addDescription("emitters-poi-output", "Output", TL("Saves emitter positions as pois to FILE"));

    oc.doRegister("variable-speed-sign-output", new Option_FileName());
    oc.addSynonyme("variable-speed-sign-output", "speed-trigger-output", true);
    oc.addDescription("variable-speed-sign-output", "Output", TL("Saves variable seed sign definitions for sink detectors to FILE"));

    oc.doRegister("end-reroute-output", new Option_FileName());
    oc.addDescription("end-reroute-output", "Output", TL("Saves rerouter definitions for sink detectors to FILE"));

    oc.doRegister("validation-output", new Option_FileName());
    oc.addDescription("validation-output", "Output", TL("Write induction loop definitions for the given detector positions to FILE"));

    oc.doRegister("validation-output.add-sources", new Option_Bool(false));
    oc.addDescription("validation-output.add-sources", "Output", TL("Add source detectors to the validation output"));


    // register processing options
    // to guess empty flows
    oc.doRegister("guess-empty-flows", new Option_Bool(false));
    oc.addDescription("guess-empty-flows", "Processing", TL("Derive missing flow values from upstream or downstream (not working!)"));

    // for guessing source/sink detectors
    oc.doRegister("highway-mode", 'h', new Option_Bool(false)); // !!! describe
    oc.addDescription("highway-mode", "Processing", TL("Switches to highway-mode"));

    // for detector reading
    oc.doRegister("ignore-invalid-detectors", new Option_Bool(false));
    oc.addDescription("ignore-invalid-detectors", "Processing", TL("Only warn about unparseable detectors"));

    // for detector type computation
    oc.doRegister("revalidate-detectors", new Option_Bool(false));
    oc.addDescription("revalidate-detectors", "Processing", TL("Recomputes detector types even if given"));

    // for route computation
    oc.doRegister("revalidate-routes", new Option_Bool(false));
    oc.addDescription("revalidate-routes", "Processing", TL("Recomputes routes even if given"));

    oc.doRegister("keep-unfinished-routes", new Option_Bool(false));
    oc.addSynonyme("keep-unfinished-routes", "keep-unfound-ends", true);
    oc.addDescription("keep-unfinished-routes", "Processing", TL("Keeps routes even if they have exhausted max-search-depth"));

    oc.doRegister("keep-longer-routes", new Option_Bool(false));
    oc.addDescription("keep-longer-routes", "Processing", TL("Keeps routes even if a shorter one exists"));

    oc.doRegister("max-search-depth", new Option_Integer(30));
    oc.addSynonyme("max-search-depth", "max-nodet-follower", true);
    oc.addDescription("max-search-depth", "Processing", TL("Number of edges to follow a route without passing a detector"));

    oc.doRegister("emissions-only", new Option_Bool(false));
    oc.addDescription("emissions-only", "Processing", TL("Writes only emission times"));

    oc.doRegister("disallowed-edges", new Option_StringVector(StringVector({ "" })));
    oc.addDescription("disallowed-edges", "Processing", TL("Do not route on these edges"));

    oc.doRegister("vclass", new Option_String("ignoring"));
    oc.addDescription("vclass", "Processing", TL("Only route on edges allowing the given vclass"));

    oc.doRegister("keep-turnarounds", new Option_Bool(false));
    oc.addDescription("keep-turnarounds", "Processing", TL("Allow turnarounds as route continuations"));

    oc.doRegister("min-route-length", new Option_Float(-1));
    oc.addSynonyme("min-route-length", "min-dist", true);
    oc.addSynonyme("min-route-length", "min-distance", true);
    oc.addDescription("min-route-length", "Processing", TL("Minimum distance in meters between start and end node of every route"));

    oc.doRegister("randomize-flows", new Option_Bool(false));
    oc.addDescription("randomize-flows", "Processing", TL("generate random departure times for emitted vehicles"));

    // flow reading
    oc.doRegister("time-factor", new Option_String("60", "TIME"));
    oc.addDescription("time-factor", "Processing", TL("Multiply flow times with TIME to get seconds"));

    oc.doRegister("time-offset", new Option_String("0", "TIME"));
    oc.addDescription("time-offset", "Processing", TL("Subtracts TIME seconds from (scaled) flow times"));

    oc.doRegister("time-step", new Option_String("60", "TIME"));
    oc.addDescription("time-step", "Processing", TL("Expected distance between two successive data sets"));

    // saving further structures
    oc.doRegister("calibrator-output", new Option_Bool(false));
    oc.addSynonyme("calibrator-output", "write-calibrators", true);
    oc.addDescription("calibrator-output", "Processing", TL("Write calibrators to FILE"));

    oc.doRegister("include-unused-routes", new Option_Bool(false));
    oc.addDescription("include-unused-routes", "Processing", TL("Include routes in the output which have no vehicles"));

    //
    oc.doRegister("revalidate-flows", new Option_Bool(false));
    oc.addDescription("revalidate-flows", "Processing", TL("Checks whether detectors with calculated flow 0 can attract additional traffic"));

    oc.doRegister("remove-empty-detectors", new Option_Bool(false));
    oc.addDescription("remove-empty-detectors", "Processing", TL("Removes empty detectors from the list"));

    oc.doRegister("strict-sources", new Option_Bool(false));
    oc.addDescription("strict-sources", "Processing", TL("Whether edges with unknown status can prevent an edge from becoming a source"));

    oc.doRegister("respect-concurrent-inflows", new Option_Bool(false));
    oc.addDescription("respect-concurrent-inflows", "Processing", TL("Try to determine further inflows to an inbetween detector when computing split probabilities"));

    //
    oc.doRegister("scale", new Option_Float(1.));
    oc.addDescription("scale", "Processing", TL("Scale factor for flows"));

    // register defaults options
    oc.doRegister("departlane", new Option_String());
    oc.addDescription("departlane", "Defaults", TL("Assigns a default depart lane"));

    oc.doRegister("departpos", new Option_String());
    oc.addDescription("departpos", "Defaults", TL("Assigns a default depart position"));

    oc.doRegister("departspeed", new Option_String());
    oc.addDescription("departspeed", "Defaults", TL("Assigns a default depart speed"));

    oc.doRegister("arrivallane", new Option_String());
    oc.addDescription("arrivallane", "Defaults", TL("Assigns a default arrival lane"));

    oc.doRegister("arrivalpos", new Option_String());
    oc.addDescription("arrivalpos", "Defaults", TL("Assigns a default arrival position"));

    oc.doRegister("arrivalspeed", new Option_String());
    oc.addDescription("arrivalspeed", "Defaults", TL("Assigns a default arrival speed"));

    oc.doRegister("speeddev", new Option_Float(0.1));
    oc.addDescription("speeddev", "Defaults", TL("The default speed deviation of vehicles"));

    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
    oc.addDescription("begin", "Time", TL("Defines the begin time; Previous defs will be discarded"));

    oc.doRegister("end", 'e', new Option_String("86400", "TIME"));
    oc.addDescription("end", "Time", TL("Defines the end time; Later defs will be discarded; Defaults to one day"));


    // register report options
    oc.doRegister("report-empty-detectors", new Option_Bool(false));
    oc.addDescription("report-empty-detectors", "Report", TL("Lists detectors with no flow (enable -v)"));

    oc.doRegister("print-absolute-flows", new Option_Bool(false));
    oc.addDescription("print-absolute-flows", "Report", TL("Prints aggregated detector flows"));

    // register report options
    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Report", TL("Disable console output of route parsing step"));

    RandHelper::insertRandOptions();
}


bool
RODFFrame::checkOptions() {
    return true;
}


/****************************************************************************/
