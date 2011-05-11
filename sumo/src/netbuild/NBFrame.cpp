/****************************************************************************/
/// @file    NBFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    09.05.2011
/// @version $Id$
///
// Sets and checks options for netbuild
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

#include <string>
#include <fstream>
#include "NBFrame.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"
#include "NBDistribution.h"
#include "NBRequest.h"
#include "NBTypeCont.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NBFrame::fillOptions(bool forNetgen) {
    OptionsCont &oc = OptionsCont::getOptions();
    // register building defaults
    oc.doRegister("default.lanenumber", 'L', new Option_Integer(1));
    oc.addSynonyme("default.lanenumber", "lanenumber", true);
    oc.addDescription("default.lanenumber", "Building Defaults", "The default number of lanes in an edge");

    oc.doRegister("default.speed", 'S', new Option_Float((SUMOReal) 13.9));
    oc.addSynonyme("default.speed", "speed", true);
    oc.addDescription("default.speed", "Building Defaults", "The default speed on an edge (in m/s)");

    oc.doRegister("default.priority", 'P', new Option_Integer(-1));
    oc.addSynonyme("default.priority", "priority", true);
    oc.addDescription("default.priority", "Building Defaults", "The default priority of an edge");


    // register the data processing options
    oc.doRegister("dismiss-vclasses", new Option_Bool(false));
    oc.addDescription("dismiss-vclasses", "Processing", "Removes vehicle class restrictions from imported edges.");

    oc.doRegister("remove-geometry", 'R', new Option_Bool(false));
    oc.addDescription("remove-geometry", "Processing", "Removes geometry information from edges");

    oc.doRegister("remove-isolated", new Option_Bool(false));
    oc.addDescription("remove-isolated", "Processing", "Removes isolated edges");

    oc.doRegister("no-turnarounds", new Option_Bool(false));
    oc.addDescription("no-turnarounds", "Processing", "Disables building turnarounds");
    
    oc.doRegister("no-tls-turnarounds", new Option_Bool(false));
    oc.addDescription("no-tls-turnarounds", "Processing", "Disables building turnarounds at tls-controlled junctions");

    oc.doRegister("no-internal-links", new Option_Bool(false)); // !!! not described
    oc.addDescription("no-internal-links", "Processing", "Omits internal links");

    oc.doRegister("split-geometry", new Option_Bool(false)); // !!!not described
    oc.addDescription("split-geometry", "Processing", "Splits edges across geometry nodes");

    oc.doRegister("offset.disable-normalization", new Option_Bool(false));
    oc.addSynonyme("offset.disable-normalization", "disable-normalize-node-positions", true);
    oc.addDescription("offset.disable-normalization", "Processing", "Turn off normalizing node positions");

    oc.doRegister("offset.x", new Option_Float(0));
    oc.addSynonyme("offset.x", "x-offset-to-apply", true);
    oc.addDescription("offset.x", "Processing", "Adds FLOAT to net x-positions");

    oc.doRegister("offset.y", new Option_Float(0));
    oc.addSynonyme("offset.y", "y-offset-to-apply", true);
    oc.addDescription("offset.y", "Processing", "Adds FLOAT to net y-positions");

    oc.doRegister("flip-y-axis", new Option_Bool(false));
    oc.addSynonyme("flip-y-axis", "flip-y");
    oc.addDescription("flip-y-axis", "Processing", "Flips the y-coordinate along zero");

    oc.doRegister("roundabouts.guess", new Option_Bool(false));
    oc.addSynonyme("roundabouts.guess", "guess-roundabouts", true);
    oc.addDescription("roundabouts.guess", "Processing", "Enable roundabout-guessing");

    oc.doRegister("lefthand", new Option_Bool(false));
    oc.addDescription("lefthand", "Processing", "Assumes left-hand traffic on the network");



    // tls setting options
    // explicite tls
    oc.doRegister("tls.set", new Option_String());
    oc.addSynonyme("tls.set", "explicite-tls", true);
    oc.addDescription("tls.set", "TLS Building", "Interprets STR as list of junctions to be controlled by TLS");

    oc.doRegister("tls.unset", new Option_String());
    oc.addSynonyme("tls.unset", "explicite-no-tls", true);
    oc.addDescription("tls.unset", "TLS Building", "Interprets STR as list of junctions to be not controlled by TLS");

    // tls-guessing
    oc.doRegister("tls.guess", new Option_Bool(false));
    oc.addSynonyme("tls.guess", "guess-tls", true);
    oc.addDescription("tls.guess", "TLS Building", "Turns on TLS guessing");

    if(!forNetgen) {
        oc.doRegister("tls.taz-nodes", new Option_Bool(false));
        oc.addSynonyme("tls.taz-nodes", "tls-guess.district-nodes", true);
        oc.addDescription("tls.taz-nodes", "TLS Building", "Sets district nodes as tls-controlled"); // !!! describe
    }

    oc.doRegister("tls-guess.joining", new Option_Bool(false));
    oc.addDescription("tls-guess.joining", "TLS Building", "Includes node clusters into guess"); // !!! describe

    oc.doRegister("tls.join", new Option_Bool(false));
    oc.addSynonyme("tls.join", "try-join-tls", true);
    oc.addDescription("tls.join", "TLS Building", "Tries to cluster tls-controlled nodes"); // !!! describe

    // computational
    oc.doRegister("tls.green.time", new Option_Integer(31));
    oc.addSynonyme("tls.green.time", "traffic-light-green", true);
    oc.addDescription("tls.green.time", "TLS Building", "Use INT as green phase duration");

    oc.doRegister("tls.yellow.min-decel", 'D', new Option_Float(3.0));
    oc.addSynonyme("tls.yellow.min-decel", "min-decel", true);
    oc.addDescription("tls.yellow.min-decel", "TLS Building", "Defines smallest vehicle deceleration");

    oc.doRegister("tls.yellow.patch-small", new Option_Bool(false));
    oc.addSynonyme("tls.yellow.patch-small", "patch-small-tyellow", true);
    oc.addDescription("tls.yellow.patch-small", "TLS Building", "Given yellow times are patched even if being too short");

    oc.doRegister("tls.yellow.time", new Option_Integer());
    oc.addSynonyme("tls.yellow.time", "traffic-light-yellow", true);
    oc.addDescription("tls.yellow.time", "TLS Building", "Set INT as fixed time for yellow phase durations");

    // tls-shifts
    oc.doRegister("tls.half-offset", new Option_String());
    oc.addSynonyme("tls.half-offset", "tl-logics.half-offset", true);
    oc.addDescription("tls.half-offset", "TLS Building", "TLSs in STR will be shifted by half-phase");

    oc.doRegister("tls.quarter-offset", new Option_String());
    oc.addSynonyme("tls.quarter-offset", "tl-logics.quarter-offset", true);
    oc.addDescription("tls.quarter-offset", "TLS Building", "TLSs in STR will be shifted by quarter-phase");



    // edge constraints
    oc.doRegister("edges-min-speed", new Option_Float());
    oc.addDescription("edges-min-speed", "Edge Removal", "Remove edges with speed < FLOAT");

    oc.doRegister("remove-edges", new Option_String());
    oc.addDescription("remove-edges", "Edge Removal", "Remove edges in STR");

    oc.doRegister("keep-edges", new Option_String());
    oc.addDescription("keep-edges", "Edge Removal", "Remove edges not in STR");

    oc.doRegister("keep-edges.input-file", new Option_FileName());
    oc.addDescription("keep-edges.input-file", "Edge Removal", "Removed edges not in FILE");

    oc.doRegister("keep-edges.postload", new Option_Bool(false));
    oc.addDescription("keep-edges.postload", "Edge Removal", "Remove edges after joining");

    oc.doRegister("keep-edges.in-boundary", new Option_String());
    oc.addDescription("keep-edges.in-boundary", "Edge Removal", "Keeps edges which are located within the given boundary");

    oc.doRegister("keep-edges.by-vclass", new Option_String());
    oc.addDescription("keep-edges.by-vclass", "Edge Removal", "Keep edges that allow any of the vclasss in STR");

    oc.doRegister("remove-edges.by-vclass", new Option_String());
    oc.addDescription("remove-edges.by-vclass", "Edge Removal", "Remove edges where vclass def is not in STR");


    // unregulated nodes options
    oc.doRegister("keep-unregulated", new Option_Bool(false));
    oc.addDescription("keep-unregulated", "Unregulated Nodes", "All nodes will be not regulated");

    oc.doRegister("keep-unregulated.nodes", new Option_String());
    oc.addDescription("keep-unregulated.nodes", "Unregulated Nodes", "Do not regulate nodes in STR");

    oc.doRegister("keep-unregulated.district-nodes", new Option_Bool(false));
    oc.addDescription("keep-unregulated.district-nodes", "Unregulated Nodes", "Do not regulate district nodes");


    // ramp guessing options
    if(!forNetgen) {
        oc.doRegister("ramps.guess", new Option_Bool(false));
        oc.addSynonyme("ramps.guess", "guess-ramps", true);
        oc.addDescription("ramps.guess", "Ramp Guessing", "Enable ramp-guessing");

        oc.doRegister("ramps.max-ramp-speed", new Option_Float(-1));
        oc.addSynonyme("ramps.max-ramp-speed", "ramp-guess.max-ramp-speed", true);
        oc.addDescription("ramps.max-ramp-speed", "Ramp Guessing", "Treat edges with speed > FLOAT as no ramps");

        oc.doRegister("ramps.min-highway-speed", new Option_Float((SUMOReal)(79/3.6)));
        oc.addSynonyme("ramps.min-highway-speed", "ramp-guess.min-highway-speed", true);
        oc.addDescription("ramps.min-highway-speed", "Ramp Guessing", "Treat edges with speed < FLOAT as no highways");

        oc.doRegister("ramps.ramp-length", new Option_Float(100));
        oc.addSynonyme("ramps.ramp-length", "ramp-guess.ramp-length", true);
        oc.addDescription("ramps.ramp-length", "Ramp Guessing", "Use FLOAT as ramp-length");

        oc.doRegister("ramps.set", new Option_String());
        oc.addSynonyme("ramps.set", "ramp-guess.explicite", true);
        oc.addDescription("ramps.set", "Ramp Guessing", "Tries to handle the given edges as ramps");
    }
}


bool
NBFrame::checkOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    //
    if(!oc.isDefault("tls-guess.joining")) {
        MsgHandler::getWarningInstance()->inform("'--tls-guess.joining' was joined with '--tls.join'.\n Please use '--tls.join' in future only.");
        if(!oc.isSet("tls.join")) {
            oc.set("tls.join", "true");
        }
    }
    return true;
}


/****************************************************************************/
