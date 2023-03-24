/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    ROFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Sets and checks options for routing
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SystemFrame.h>
#include "ROFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
ROFrame::fillOptions(OptionsCont& oc, const bool isDUA, const bool isMA) {
    // register options
    // register I/O options
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", TL("Write generated routes to FILE"));

    oc.doRegister("vtype-output", new Option_FileName());
    if (!isMA) {
        oc.addSynonyme("vtype-output", "vtype");
    }
    oc.addDescription("vtype-output", "Output", TL("Write used vehicle types into separate FILE"));

    oc.doRegister("keep-vtype-distributions", new Option_Bool(false));
    oc.addDescription("keep-vtype-distributions", "Output", TL("Keep vTypeDistribution ids when writing vehicles and their types"));

    oc.doRegister("emissions.volumetric-fuel", new Option_Bool(false));
    oc.addDescription("emissions.volumetric-fuel", "Output", TL("Return fuel consumption values in (legacy) unit l instead of mg"));

    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", TL("Use FILE as SUMO-network to route on"));

    oc.doRegister("named-routes", new Option_Bool(false));
    oc.addDescription("named-routes", "Output", TL("Write vehicles that reference routes by their id"));

    oc.doRegister("additional-files", 'a', new Option_FileName());
    oc.addSynonyme("additional-files", "d", true);
    oc.addSynonyme("additional-files", "additional");
    oc.addSynonyme("additional-files", "taz-files");
    oc.addSynonyme("additional-files", "districts", true);
    oc.addDescription("additional-files", "Input", TL("Read additional network data (districts, bus stops) from FILE(s)"));

    oc.doRegister("route-files", 'r', new Option_FileName());
    oc.addSynonyme("route-files", "flow-files", true);
    oc.addSynonyme("route-files", "flows", true);
    oc.addSynonyme("route-files", "f", true);
    oc.addSynonyme("route-files", "alternative-files", true);
    oc.addSynonyme("route-files", "alternatives-files", true);
    oc.addSynonyme("route-files", "trip-files", true);
    oc.addSynonyme("route-files", "trips", true);
    if (isDUA) {
        // to make the transition from --trip-files easier, but has a conflict with jtrrouter
        oc.addSynonyme("route-files", "t", true);
    }
    oc.addDescription("route-files", "Input", TL("Read sumo routes, alternatives, flows, and trips from FILE(s)"));

    oc.doRegister("phemlight-path", new Option_FileName(StringVector({ "./PHEMlight/" })));
    oc.addDescription("phemlight-path", "Input", TL("Determines where to load PHEMlight definitions from"));

    oc.doRegister("phemlight-year", new Option_Integer(0));
    oc.addDescription("phemlight-year", "Input", TL("Enable fleet age modelling with the given reference year in PHEMlight5"));

    oc.doRegister("phemlight-temperature", new Option_Float(INVALID_DOUBLE));
    oc.addDescription("phemlight-temperature", "Input", TL("Set ambient temperature to correct NOx emissions in PHEMlight5"));

    if (isDUA || isMA) {
        oc.doRegister("weight-files", 'w', new Option_FileName());
        oc.addSynonyme("weight-files", "weights");
        oc.addDescription("weight-files", "Input", TL("Read network weights from FILE(s)"));

        oc.doRegister("lane-weight-files", new Option_FileName());
        oc.addDescription("lane-weight-files", "Input", TL("Read lane-based network weights from FILE(s)"));

        oc.doRegister("weight-attribute", 'x', new Option_String("traveltime"));
        oc.addSynonyme("weight-attribute", "measure", true);
        oc.addDescription("weight-attribute", "Input", TL("Name of the xml attribute which gives the edge weight"));
    }

    // need to do this here to be able to check for network and route input options
    SystemFrame::addReportOptions(oc);

    // register the time settings
    oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
    oc.addDescription("begin", "Time", TL("Defines the begin time; Previous trips will be discarded"));

    oc.doRegister("end", 'e', new Option_String("-1", "TIME"));
    oc.addDescription("end", "Time", TL("Defines the end time; Later trips will be discarded; Defaults to the maximum time that SUMO can represent"));

    // register the processing options
    oc.doRegister("ignore-errors", new Option_Bool(false));
    oc.addSynonyme("ignore-errors", "continue-on-unbuild", true);
    oc.addSynonyme("ignore-errors", "dismiss-loading-errors", true);
    oc.addDescription("ignore-errors", "Report", TL("Continue if a route could not be build"));

    oc.doRegister("max-alternatives", new Option_Integer(5));
    oc.addDescription("max-alternatives", "Processing", TL("Prune the number of alternatives to INT"));

    oc.doRegister("with-taz", new Option_Bool(false));
    oc.addDescription("with-taz", "Processing", TL("Use origin and destination zones (districts) for in- and output"));

    oc.doRegister("junction-taz", new Option_Bool(false));
    oc.addDescription("junction-taz", "Input", TL("Initialize a TAZ for every junction to use attributes toJunction and fromJunction"));

    if (!isMA) {
        oc.doRegister("unsorted-input", new Option_Bool(false));
        oc.addSynonyme("unsorted-input", "unsorted");
        oc.addDescription("unsorted-input", "Processing", TL("Assume input is unsorted"));

        oc.doRegister("route-steps", 's', new Option_String("200", "TIME"));
        oc.addDescription("route-steps", "Processing", TL("Load routes for the next number of seconds ahead"));

        oc.doRegister("no-internal-links", new Option_Bool(false));
        oc.addDescription("no-internal-links", "Processing", TL("Disable (junction) internal links"));

        oc.doRegister("randomize-flows", new Option_Bool(false));
        oc.addDescription("randomize-flows", "Processing", TL("generate random departure times for flow input"));

        oc.doRegister("remove-loops", new Option_Bool(false));
        oc.addDescription("remove-loops", "Processing", TL("Remove loops within the route; Remove turnarounds at start and end of the route"));

        oc.doRegister("repair", new Option_Bool(false));
        oc.addDescription("repair", "Processing", TL("Tries to correct a false route"));

        oc.doRegister("repair.from", new Option_Bool(false));
        oc.addDescription("repair.from", "Processing", TL("Tries to correct an invalid starting edge by using the first usable edge instead"));

        oc.doRegister("repair.to", new Option_Bool(false));
        oc.addDescription("repair.to", "Processing", TL("Tries to correct an invalid destination edge by using the last usable edge instead"));

        oc.doRegister("mapmatch.distance", new Option_Float(100));
        oc.addDescription("mapmatch.distance", "Processing", TL("Maximum distance when mapping input coordinates (fromXY etc.) to the road network"));

        oc.doRegister("mapmatch.junctions", new Option_Bool(false));
        oc.addDescription("mapmatch.junctions", "Processing", TL("Match positions to junctions instead of edges"));

        oc.doRegister("bulk-routing", new Option_Bool(false));
        oc.addDescription("bulk-routing", "Processing", TL("Aggregate routing queries with the same origin"));
    }

    oc.doRegister("routing-threads", new Option_Integer(0));
    oc.addDescription("routing-threads", "Processing", TL("The number of parallel execution threads used for routing"));

    if (isDUA || isMA) {
        oc.doRegister("routing-algorithm", new Option_String("dijkstra"));
        oc.addDescription("routing-algorithm", "Processing", TL("Select among routing algorithms ['dijkstra', 'astar', 'CH', 'CHWrapper']"));
    }

    oc.doRegister("restriction-params", new Option_StringVector());
    oc.addDescription("restriction-params", "Processing", TL("Comma separated list of param keys to compare for additional restrictions"));

    if (isDUA || isMA) {
        oc.doRegister("weights.interpolate", new Option_Bool(false));
        oc.addSynonyme("weights.interpolate", "interpolate", true);
        oc.addDescription("weights.interpolate", "Processing", TL("Interpolate edge weights at interval boundaries"));

        oc.doRegister("weights.expand", new Option_Bool(false));
        oc.addSynonyme("weights.expand", "expand-weights", true);
        oc.addDescription("weights.expand", "Processing", TL("Expand the end of the last loaded weight interval to infinity"));
    }

    oc.doRegister("weights.minor-penalty", new Option_Float(1.5));
    oc.addDescription("weights.minor-penalty", "Processing", TL("Apply the given time penalty when computing routing costs for minor-link internal lanes"));

    if (!isMA) {
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

        oc.doRegister("defaults-override", new Option_Bool(false));
        oc.addDescription("defaults-override", "Defaults", TL("Defaults will override given values"));
    }

    // register report options
    oc.doRegister("stats-period", new Option_Integer(-1));
    oc.addDescription("stats-period", "Report", TL("Defines how often statistics shall be printed"));

    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Report", TL("Disable console output of route parsing step"));
}


bool
ROFrame::checkOptions(OptionsCont& oc) {
    // check whether the output is valid and can be build
    if (!oc.isSet("output-file")) {
        WRITE_ERROR(TL("No output specified."));
        return false;
    }
    //
    if (oc.getInt("max-alternatives") < 1) {
        WRITE_ERROR(TL("A positive number of alternatives must be enabled."));
        return false;
    }
#ifndef HAVE_FOX
    if (oc.getInt("routing-threads") > 1) {
        WRITE_ERROR(TL("Parallel routing is only possible when compiled with Fox."));
        return false;
    }
#endif
    return true;
}


/****************************************************************************/
