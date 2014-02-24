/****************************************************************************/
/// @file    ROFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for routing
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
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/SUMOTime.h>
#include "ROFrame.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
ROFrame::fillOptions(OptionsCont& oc, bool forDuarouter) {
    // register options
    // register I/O options
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "Write generated routes to FILE");

    oc.doRegister("vtype-output", new Option_FileName(""));
    oc.addSynonyme("vtype-output", "vtype");
    oc.addDescription("vtype-output", "Output", "Write used vehicle types into separate FILE");

    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Use FILE as SUMO-network to route on");

    oc.doRegister("additional-files", 'd', new Option_FileName());
    oc.addSynonyme("additional-files", "additional");
    oc.addSynonyme("additional-files", "taz-files");
    oc.addSynonyme("additional-files", "districts", true);
    oc.addDescription("additional-files", "Input", "Read additional network data (districts, bus stops) from FILE");

    oc.doRegister("alternative-files", 'a', new Option_FileName());
    oc.addSynonyme("alternative-files", "alternatives-files");
    oc.addSynonyme("alternative-files", "alternatives", true);
    oc.addDescription("alternative-files", "Input", "Read alternatives from FILE");

    oc.doRegister("flow-files", 'f', new Option_FileName());
    oc.addSynonyme("flow-files", "flows");
    oc.addSynonyme("flow-files", "flow-definition", true);
    oc.addDescription("flow-files", "Input", "Read flow-definitions from FILE(s)");

    if (forDuarouter) {
        oc.doRegister("weight-files", 'w', new Option_FileName());
        oc.addSynonyme("weight-files", "weights");
        oc.addDescription("weight-files", "Input", "Read network weights from FILE(s)");

        oc.doRegister("lane-weight-files", new Option_FileName());
        oc.addDescription("lane-weight-files", "Input", "Read lane-based network weights from FILE(s)");

        oc.doRegister("weight-attribute", 'x', new Option_String("traveltime"));
        oc.addSynonyme("weight-attribute", "measure", true);
        oc.addDescription("weight-attribute", "Input", "Name of the xml attribute which gives the edge weight");

        oc.doRegister("phemlight-path", new Option_FileName("./PHEMlight/"));
        oc.addDescription("phemlight-path", "Input", "Determines where to load PHEMlight definitions from.");
    }

    // register the time settings
    oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
    oc.addDescription("begin", "Time", "Defines the begin time; Previous trips will be discarded");

    oc.doRegister("end", 'e', new Option_String(SUMOTIME_MAXSTRING, "TIME"));
    oc.addDescription("end", "Time", "Defines the end time; Later trips will be discarded; Defaults to the maximum time that SUMO can represent");

    // register the processing options
    oc.doRegister("ignore-errors", new Option_Bool(false));
    oc.addSynonyme("ignore-errors", "continue-on-unbuild", true);
    oc.addDescription("ignore-errors", "Processing", "Continue if a route could not be build");

    oc.doRegister("unsorted-input", new Option_Bool(false));
    oc.addSynonyme("unsorted-input", "unsorted");
    oc.addDescription("unsorted-input", "Processing", "Assume input is unsorted");

    oc.doRegister("randomize-flows", new Option_Bool(false));
    oc.addDescription("randomize-flows", "Processing", "generate random departure times for flow input");

    oc.doRegister("max-alternatives", new Option_Integer(5));
    oc.addDescription("max-alternatives", "Processing", "Prune the number of alternatives to INT");

    oc.doRegister("remove-loops", new Option_Bool(false));
    oc.addDescription("remove-loops", "Processing", "Remove loops within the route; Remove turnarounds at start and end of the route");

    oc.doRegister("repair", new Option_Bool(false));
    oc.addDescription("repair", "Processing", "Tries to correct a false route");

    oc.doRegister("weights.interpolate", new Option_Bool(false));
    oc.addSynonyme("weights.interpolate", "interpolate", true);
    oc.addDescription("weights.interpolate", "Processing", "Interpolate edge weights at interval boundaries");

    oc.doRegister("with-taz", new Option_Bool(false));
    oc.addDescription("with-taz", "Processing", "Use origin and destination zones (districts) for in- and output");

    if (forDuarouter) {
        oc.doRegister("routing-algorithm", new Option_String("dijkstra"));
        oc.addDescription("routing-algorithm", "Processing",
#ifndef HAVE_INTERNAL // catchall for internal stuff
                          "Select among routing algorithms ['dijkstra', 'astar']"
#else
                          "Select among routing algorithms ['dijkstra', 'astar', 'bulkstar', 'CH', 'CHWrapper']"
#endif
                         );

#ifdef HAVE_INTERNAL // catchall for internal stuff
        oc.doRegister("weight-period", new Option_String("3600", "TIME"));
        oc.addDescription("weight-period", "Processing", "Aggregation period for the given weight files; triggers rebuilding of Contraction Hirarchy");
#endif
    }

    // register defaults options
    oc.doRegister("departlane", new Option_String());
    oc.addDescription("departlane", "Defaults", "Assigns a default depart lane");

    oc.doRegister("departpos", new Option_String());
    oc.addDescription("departpos", "Defaults", "Assigns a default depart position");

    oc.doRegister("departspeed", new Option_String());
    oc.addDescription("departspeed", "Defaults", "Assigns a default depart speed");

    oc.doRegister("arrivallane", new Option_String());
    oc.addDescription("arrivallane", "Defaults", "Assigns a default arrival lane");

    oc.doRegister("arrivalpos", new Option_String());
    oc.addDescription("arrivalpos", "Defaults", "Assigns a default arrival position");

    oc.doRegister("arrivalspeed", new Option_String());
    oc.addDescription("arrivalspeed", "Defaults", "Assigns a default arrival speed");

    oc.doRegister("defaults-override", new Option_Bool(false));
    oc.addDescription("defaults-override", "Defaults", "Defaults will override given values");


    // register report options
    oc.doRegister("stats-period", new Option_Integer(-1));
    oc.addDescription("stats-period", "Report", "Defines how often statistics shall be printed");

    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Report", "Disable console output of route parsing step");
}


bool
ROFrame::checkOptions(OptionsCont& oc) {
    // check whether the output is valid and can be build
    if (!oc.isSet("output-file")) {
        WRITE_ERROR("No output specified.");
        return false;
    }
    //
    if (oc.getInt("max-alternatives") < 2) {
        WRITE_ERROR("At least two alternatives should be enabled");
        return false;
    }
    return true;
}



/****************************************************************************/

