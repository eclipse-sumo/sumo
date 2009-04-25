/****************************************************************************/
/// @file    RODFFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Sets and checks options for df-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/SystemFrame.h>
#include "RODFFrame.h"
#include <router/ROFrame.h>
#include <utils/common/RandHelper.h>


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
void
RODFFrame::fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Time");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too


    // register the options
    // register input-options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Loads the SUMO-network FILE");

    oc.doRegister("routes-input", 'r', new Option_FileName());
    oc.addDescription("routes-input", "Input", "Loads SUMO-routes from FILE");

    oc.doRegister("detector-files", 'd', new Option_FileName());
    oc.addSynonyme("detector-files", "detectors");
    oc.addDescription("detector-files", "Input", "Loads detector descriptions from FILE");

    oc.doRegister("detector-flow-files", 'f', new Option_FileName());
    oc.addSynonyme("detector-flow-files", "detflows");
    oc.addDescription("detector-flow-files", "Input", "Loads detector flows from FILE(s)");


    // register output options
    oc.doRegister("routes-output", 'o', new Option_FileName());
    oc.addDescription("routes-output", "Output", "Saves computed routes to FILE");

    oc.doRegister("routes-for-all", new Option_Bool(false));
    oc.addDescription("routes-for-all", "Output", "Forces DFROUTER to compute routes for in-between detectors");

    oc.doRegister("detectors-output", new Option_FileName());
    oc.addDescription("detectors-output", "Output", "Saves typed detectors to FILE");

    oc.doRegister("detectors-poi-output", new Option_FileName());
    oc.addDescription("detectors-poi-output", "Output", "Saves detector positions as pois to FILE");

    oc.doRegister("emitters-output", new Option_FileName());
    oc.addDescription("emitters-output", "Output", "Saves emitter definitions for source detectors to FILE");

    oc.doRegister("emitters-poi-output", new Option_FileName()); // !!! describe
    oc.addDescription("emitters-poi-output", "Output", "Saves emitter positions as pois to FILE");

    oc.doRegister("speed-trigger-output", new Option_FileName());
    oc.addDescription("speed-trigger-output", "Output", "Saves variable seed sign definitions for sink detectors to FILE");

    oc.doRegister("end-reroute-output", new Option_FileName());
    oc.addDescription("end-reroute-output", "Output", "Saves rerouter definitions for sink detectors to FILE");

    oc.doRegister("validation-output", new Option_FileName());
    oc.addDescription("validation-output", "Output", "");

    oc.doRegister("validation-output.add-sources", new Option_Bool(false));
    oc.addDescription("validation-output.add-sources", "Output", "");


    // register processing options
    // to guess empty flows
    oc.doRegister("guess-empty-flows", new Option_Bool(false)); // !!! describe
    oc.addDescription("guess-empty-flows", "Processing", "");

    // for guessing source/sink detectors
    oc.doRegister("highway-mode", 'h', new Option_Bool(false)); // !!! describe
    oc.addDescription("highway-mode", "Processing", "Switches to highway-mode");

    // for detector reading
    oc.doRegister("ignore-invalid-detectors", new Option_Bool(false));
    oc.addDescription("ignore-invalid-detectors", "Processing", "Only warn about unparseable detectors");

    // for detector type computation
    oc.doRegister("revalidate-detectors", new Option_Bool(false));
    oc.addDescription("revalidate-detectors", "Processing", "Recomputes detector types even if given");

    // for route computation
    oc.doRegister("revalidate-routes", new Option_Bool(false));
    oc.addDescription("revalidate-routes", "Processing", "Recomputes routes even if given");

    oc.doRegister("all-end-follower", new Option_Bool(false));
    oc.addDescription("all-end-follower", "Processing", "Continues routes till the first street after a sink");

    oc.doRegister("keep-unfound-ends", new Option_Bool(false));
    oc.addDescription("keep-unfound-ends", "Processing", "Keeps routes ending at no sinks");

    oc.doRegister("keep-longer-routes", new Option_Bool(false));
    oc.addDescription("keep-longer-routes", "Processing", "Keeps routes even if a shorter one exists");

    oc.doRegister("max-nodet-follower", new Option_Integer(30));
    oc.addDescription("max-nodet-follower", "Processing", "Number of edges until which a route is followed");

    oc.doRegister("emissions-only", new Option_Bool(false));
    oc.addDescription("emissions-only", "Processing", "Writes only emission times");

    oc.doRegister("disallowed-edges", new Option_String(""));
    oc.addDescription("disallowed-edges", "Processing", "Do not route on these edges");

    oc.doRegister("keep-turnarounds", new Option_Bool(false));
    oc.addDescription("keep-turnarounds", "Processing", "Allow turnarounds as route continuations");

    oc.doRegister("min-distance", new Option_Float(-1));
    oc.addSynonyme("min-distance", "min-dist");
    oc.addDescription("min-distance", "Processing", "According sink must be at least that far away");


    // flow reading
    oc.doRegister("time-offset", new Option_Integer(0));
    oc.addDescription("time-offset", "Processing", "Subtracts INT from flow times");

    // saving further structures
    oc.doRegister("write-calibrators", new Option_Bool(false)); // !!!undescribed
    oc.addDescription("write-calibrators", "Processing", "");

    oc.doRegister("include-unused-routes", new Option_Bool(false)); // !!!undescribed
    oc.addDescription("include-unused-routes", "Processing", "");

    //
    oc.doRegister("revalidate-flows", new Option_Bool(false));
    oc.addDescription("revalidate-flows", "Processing", "");

    oc.doRegister("remove-empty-detectors", new Option_Bool(false));
    oc.addDescription("remove-empty-detectors", "Processing", "Removes empty detectors from the list");

    oc.doRegister("strict-sources", new Option_Bool(false)); // !!!undescribed
    oc.addDescription("strict-sources", "Processing", "");

    //
    oc.doRegister("mesosim", new Option_Bool(false));
    oc.addDescription("mesosim", "Processing", "Joins detectors lying on same height");

    //
    oc.doRegister("scale", new Option_Float(1.));
    oc.addDescription("scale", "Processing", "Scale factor for flows");



    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Defines the begin time; Previous defs will be discarded");

    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.addDescription("end", "Time", "Defines the end time; Later defs will be discarded");


    // register report options
    oc.doRegister("report-empty-detectors", new Option_Bool(false));
    oc.addDescription("report-empty-detectors", "Report", "Lists detectors with no flow (enable -v)");

    oc.doRegister("print-absolute-flows", new Option_Bool(false));
    oc.addDescription("print-absolute-flows", "Report", "Prints aggregated detector flows");

    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.addDescription("suppress-warnings", "Report", "Disables output of warnings");

    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.addDescription("log-file", "Report", "Writes all messages to FILE");

    RandHelper::insertRandOptions();
}


bool
RODFFrame::checkOptions() {
    return true;
}



/****************************************************************************/

