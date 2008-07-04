/****************************************************************************/
/// @file    ROFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for routing
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
#include "ROFrame.h"

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
ROFrame::fillOptions(OptionsCont &oc)
{
    // register options
    // register I/O options
    oc.doRegister("output", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "Write generated routes to FILE");

    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Use FILE as SUMO-network to route on");

    oc.doRegister("alternatives", 'a', new Option_FileName());
    oc.addDescription("alternatives", "Input", "Read alternatives from FILE");

    oc.doRegister("weights", 'w', new Option_FileName());
    oc.addSynonyme("weights", "weight-files");
    oc.addDescription("weights", "Input", "Read network weights from FILE");

    oc.doRegister("lane-weights", new Option_FileName());
    oc.addDescription("lane-weights", "Input", "Read lane-weights from FILE");


    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Defines the begin time; Previous trips will be discarded");

    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.addDescription("end", "Time", "Defines the end time; Later trips will be discarded");


    // register the processing options
    oc.doRegister("continue-on-unbuild", new Option_Bool(false));
    oc.addDescription("continue-on-unbuild", "Processing", "Continue if a route could not be build");

    oc.doRegister("unsorted", new Option_Bool(false));
    oc.addDescription("unsorted", "Processing", "Assume input is unsorted");

    oc.doRegister("randomize-flows", new Option_Bool(false)); // !!! undescibed
    oc.addDescription("randomize-flows", "Processing", "");

    oc.doRegister("max-alternatives", new Option_Integer(5));
    oc.addDescription("max-alternatives", "Processing", "Prune the number of alternatives to INT");

    // add possibility to insert random vehicles
    oc.doRegister("random-per-second", 'R', new Option_Float());
    oc.addDescription("random-per-second", "Processing", "Emit FLOAT random vehicles per second");

    oc.doRegister("prune-random", new Option_Bool(false));
    oc.addDescription("prune-random", "Processing", "");

    oc.doRegister("remove-loops", new Option_Bool(false));
    oc.addDescription("remove-loops", "Processing", "Remove loops at start and end of the route");


    // register report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output");

    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.addDescription("suppress-warnings", "Report", "Disables output of warnings");

    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.addDescription("print-options", "Report", "Prints option values before processing");

    oc.doRegister("help", '?', new Option_Bool(false));
    oc.addDescription("help", "Report", "Prints this screen");

    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.addDescription("log-file", "Report", "Writes all messages to FILE");

    oc.doRegister("stats-period", new Option_Integer(-1));
    oc.addDescription("stats-period", "Report", "Defines how often statistics shall be printed");
}



bool
ROFrame::checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if (!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    //
    if (oc.getInt("max-alternatives")<2) {
        MsgHandler::getErrorInstance()->inform("At least two alternatives should be enabled");
        return false;
    }
    //
    return true;
}



/****************************************************************************/

