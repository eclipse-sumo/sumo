/****************************************************************************/
/// @file    NBFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    09.05.2011
/// @version $Id$
///
// Sets and checks options for netbuild
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <fstream>
#include "NBFrame.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
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
#include <utils/xml/SUMOXMLDefinitions.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
NBFrame::fillOptions(bool forNetgen) {
    OptionsCont& oc = OptionsCont::getOptions();
    // register building defaults
    oc.doRegister("default.lanenumber", 'L', new Option_Integer(1));
    oc.addSynonyme("default.lanenumber", "lanenumber", true);
    oc.addDescription("default.lanenumber", "Building Defaults", "The default number of lanes in an edge");

    oc.doRegister("default.lanewidth", new Option_Float(NBEdge::UNSPECIFIED_WIDTH));
    oc.addSynonyme("default.lanewidth", "lanewidth", true);
    oc.addDescription("default.lanewidth", "Building Defaults", "The default width of lanes");

    oc.doRegister("default.speed", 'S', new Option_Float((SUMOReal) 13.9));
    oc.addSynonyme("default.speed", "speed", true);
    oc.addDescription("default.speed", "Building Defaults", "The default speed on an edge (in m/s)");

    oc.doRegister("default.priority", 'P', new Option_Integer(-1));
    oc.addSynonyme("default.priority", "priority", true);
    oc.addDescription("default.priority", "Building Defaults", "The default priority of an edge");

    oc.doRegister("default.sidewalk-width", new Option_Float((SUMOReal) 2.0));
    oc.addDescription("default.sidewalk-width", "Building Defaults", "The default width of added sidewalks");

    oc.doRegister("default.disallow", new Option_String());
    oc.addDescription("default.disallow", "Building Defaults", "The default for disallowed vehicle classes");

    oc.doRegister("default.junctions.keep-clear", new Option_Bool(true));
    oc.addDescription("default.junctions.keep-clear", "Building Defaults", "Whether junctions should be kept clear by default");

    oc.doRegister("default.junctions.radius", new Option_Float(1.5));
    oc.addDescription("default.junctions.radius", "Building Defaults", "The default turning radius of intersections");

    // register the data processing options
    oc.doRegister("no-internal-links", new Option_Bool(false)); // !!! not described
    oc.addDescription("no-internal-links", "Processing", "Omits internal links");

    oc.doRegister("numerical-ids", new Option_Bool(false));
    oc.addDescription("numerical-ids", "Processing", "Remaps alphanumerical IDs of nodes and edges to ensure that all IDs are integers");

    /// @todo not working for netgen
    oc.doRegister("reserved-ids", new Option_FileName());
    oc.addDescription("reserved-ids", "Processing", "Ensures that generated ids do not included any of the typed IDs from FILE (SUMO-GUI selection file format)");

    if (!forNetgen) {
        oc.doRegister("dismiss-vclasses", new Option_Bool(false));
        oc.addDescription("dismiss-vclasses", "Processing", "Removes vehicle class restrictions from imported edges");
    }

    oc.doRegister("no-turnarounds", new Option_Bool(false));
    oc.addDescription("no-turnarounds", "Processing", "Disables building turnarounds");

    oc.doRegister("no-turnarounds.tls", new Option_Bool(false));
    oc.addSynonyme("no-turnarounds.tls", "no-tls-turnarounds", true);
    oc.addDescription("no-turnarounds.tls", "Processing", "Disables building turnarounds at tls-controlled junctions");

    oc.doRegister("no-left-connections", new Option_Bool(false));
    oc.addDescription("no-left-connections", "Processing", "Disables building connections to left");

    if (!forNetgen) {
        oc.doRegister("geometry.split", new Option_Bool(false)); // !!!not described
        oc.addSynonyme("geometry.split", "split-geometry", true);
        oc.addDescription("geometry.split", "Processing", "Splits edges across geometry nodes");

        oc.doRegister("geometry.remove", 'R', new Option_Bool(false));
        oc.addSynonyme("geometry.remove", "remove-geometry", true);
        oc.addDescription("geometry.remove", "Processing", "Replace nodes which only define edge geometry by geometry points (joins edges)");

        oc.doRegister("geometry.remove.keep-edges.explicit", new Option_String());
        oc.addDescription("geometry.remove.keep-edges.explicit", "Processing", "Ensure that the given list of edges is not modified");

        oc.doRegister("geometry.remove.keep-edges.input-file", new Option_FileName());
        oc.addDescription("geometry.remove.keep-edges.input-file", "Processing",
                          "Ensure that the edges in FILE are not modified (Each id on a single line. Selection files from SUMO-GUI are also supported)");

        oc.doRegister("geometry.max-segment-length", new Option_Float(0));
        oc.addDescription("geometry.max-segment-length", "Processing", "splits geometry to restrict segment length");

        oc.doRegister("geometry.min-dist", new Option_Float(-1));
        oc.addDescription("geometry.min-dist", "Processing", "reduces too similar geometry points");

        oc.doRegister("geometry.max-angle", new Option_Float(99));
        oc.addDescription("geometry.max-angle", "Processing", "Warn about edge geometries with an angle above DEGREES in successive segments");

        oc.doRegister("geometry.min-radius", new Option_Float(9));
        oc.addDescription("geometry.min-radius", "Processing", "Warn about edge geometries with a turning radius less than METERS at the start or end");

        oc.doRegister("geometry.min-radius.fix", new Option_Bool(false));
        oc.addDescription("geometry.min-radius.fix", "Processing", "Straighten edge geometries to avoid turning radii less than geometry.min-radius");

        oc.doRegister("geometry.junction-mismatch-threshold", new Option_Float(20));
        oc.addDescription("geometry.junction-mismatch-threshold", "Processing", "Warn if the junction shape is to far away from the original node position");

        oc.doRegister("geometry.check-overlap", new Option_Float(0));
        oc.addDescription("geometry.check-overlap", "Processing", "Warn if edges overlap by more than the given threshold value");

        oc.doRegister("geometry.check-overlap.vertical-threshold", new Option_Float(4));
        oc.addDescription("geometry.check-overlap.vertical-threshold", "Processing", "Ignore overlapping edges if they are separated vertically by the given threshold.");

        oc.doRegister("geometry.max-grade", new Option_Float(10));
        oc.addDescription("geometry.max-grade", "Processing", "Warn about edge geometries with a grade in % above FLOAT. The threshold applies to roads with a speed limit of 50km/h and is scaled according to road speed.");
    }

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

    oc.doRegister("opposites.guess", new Option_Bool(false));
    oc.addDescription("opposites.guess", "Processing", "Enable guessing of opposite direction lanes usable for overtaking");

    oc.doRegister("lefthand", new Option_Bool(false));
    oc.addDescription("lefthand", "Processing", "Assumes left-hand traffic on the network");

    oc.doRegister("edges.join", new Option_Bool(false));
    oc.addDescription("edges.join", "Processing",
                      "Merges edges which connect the same nodes and are close to each other (recommended for VISSIM import)");

    oc.doRegister("junctions.join", new Option_Bool(false));
    oc.addDescription("junctions.join", "Processing",
                      "Joins junctions that are close to each other (recommended for OSM import)");

    oc.doRegister("junctions.join-dist", new Option_Float(10));
    oc.addDescription("junctions.join-dist", "Processing",
                      "Determines the maximal distance for joining junctions (defaults to 10)");

    if (!forNetgen) {
        oc.doRegister("junctions.join-exclude", new Option_String());
        oc.addDescription("junctions.join-exclude", "Processing", "Interprets STR as list of junctions to exclude from joining");

        oc.doRegister("speed.offset", new Option_Float(0));
        oc.addDescription("speed.offset", "Processing", "Modifies all edge speeds by adding FLOAT");

        oc.doRegister("speed.factor", new Option_Float(1));
        oc.addDescription("speed.factor", "Processing", "Modifies all edge speeds by multiplying by FLOAT");

        oc.doRegister("speed.minimum", new Option_Float(0));
        oc.addDescription("speed.minimum", "Processing", "Modifies all edge speeds to at least FLOAT");
    }

    oc.doRegister("junctions.corner-detail", new Option_Integer(0));
    oc.addDescription("junctions.corner-detail", "Processing", "Generate INT intermediate points to smooth out intersection corners");

    oc.doRegister("junctions.internal-link-detail", new Option_Integer(5));
    oc.addDescription("junctions.internal-link-detail", "Processing", "Generate INT intermediate points to smooth out lanes within the intersection");

    oc.doRegister("junctions.scurve-stretch", new Option_Float(0));
    oc.addDescription("junctions.scurve-stretch", "Processing", "Generate longer intersections to allow for smooth s-curves when the number of lanes changes");

    oc.doRegister("rectangular-lane-cut", new Option_Bool(false));
    oc.addDescription("rectangular-lane-cut", "Processing", "Forces rectangular cuts between lanes and intersections");

    oc.doRegister("check-lane-foes.roundabout", new Option_Bool(true));
    oc.addDescription("check-lane-foes.roundabout", "Processing",
                      "Allow driving onto a multi-lane road if there are foes on other lanes (at roundabouts)");

    oc.doRegister("check-lane-foes.all", new Option_Bool(false));
    oc.addDescription("check-lane-foes.all", "Processing",
                      "Allow driving onto a multi-lane road if there are foes on other lanes (everywhere)");

    oc.doRegister("sidewalks.guess", new Option_Bool(false));
    oc.addDescription("sidewalks.guess", "Processing",
                      "Guess pedestrian sidewalks based on edge speed");

    oc.doRegister("sidewalks.guess.max-speed", new Option_Float((SUMOReal) 13.89));
    oc.addDescription("sidewalks.guess.max-speed", "Processing",
                      "Add sidewalks for edges with a speed equal or below the given limit");

    oc.doRegister("sidewalks.guess.min-speed", new Option_Float((SUMOReal) 5.8));
    oc.addDescription("sidewalks.guess.min-speed", "Processing",
                      "Add sidewalks for edges with a speed above the given limit");

    oc.doRegister("sidewalks.guess.from-permissions", new Option_Bool(false));
    oc.addDescription("sidewalks.guess.from-permissions", "Processing",
                      "Add sidewalks for edges that allow pedestrians on any of their lanes regardless of speed");

    oc.doRegister("sidewalks.guess.exclude", new Option_String());
    oc.addDescription("sidewalks.guess.exclude", "Processing",
                      "Do not guess sidewalks for the given list of edges");

    oc.doRegister("crossings.guess", new Option_Bool(false));
    oc.addDescription("crossings.guess", "Processing",
                      "Guess pedestrian crossings based on the presence of sidewalks");

    oc.doRegister("crossings.guess.speed-threshold", new Option_Float(13.89));
    oc.addDescription("crossings.guess.speed-threshold", "Processing",
                      "At uncontrolled nodes, do not build crossings across edges with a speed above the threshold");

    // tls setting options
    // explicit tls
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

    if (!forNetgen) {
        oc.doRegister("tls.taz-nodes", new Option_Bool(false));
        oc.addSynonyme("tls.taz-nodes", "tls-guess.district-nodes", true);
        oc.addDescription("tls.taz-nodes", "TLS Building", "Sets district nodes as tls-controlled"); // !!! describe
    }

    oc.doRegister("tls-guess.joining", new Option_Bool(false));
    oc.addDescription("tls-guess.joining", "TLS Building", "Includes node clusters into guess"); // !!! describe

    oc.doRegister("tls.join", new Option_Bool(false));
    oc.addSynonyme("tls.join", "try-join-tls", true);
    oc.addDescription("tls.join", "TLS Building", "Tries to cluster tls-controlled nodes"); // !!! describe

    oc.doRegister("tls.join-dist", new Option_Float(20));
    oc.addDescription("tls.join-dist", "TLS Building",
                      "Determines the maximal distance for joining traffic lights (defaults to 20)");

    oc.doRegister("tls.uncontrolled-within", new Option_Bool(false));
    oc.addDescription("tls.uncontrolled-within", "TLS Building",
                      "Do not control edges that lie fully within a joined traffic light. This may cause collisions but allows old traffic light plans to be used");

    if (!forNetgen) {
        oc.doRegister("tls.guess-signals", new Option_Bool(false));
        oc.addDescription("tls.guess-signals", "TLS Building", "Interprets tls nodes surrounding an intersection as signal positions for a larger TLS. This is typical pattern for OSM-derived networks");

        oc.doRegister("tls.guess-signals.dist", new Option_Float(25));
        oc.addDescription("tls.guess-signals.dist", "TLS Building", "Distance for interpreting nodes as signal locations");
    }


    // computational
    oc.doRegister("tls.cycle.time", new Option_Integer(90));
    oc.addDescription("tls.cycle.time", "TLS Building", "Use INT as cycle duration");

    oc.doRegister("tls.green.time", new Option_Integer(31));
    oc.addSynonyme("tls.green.time", "traffic-light-green", true);
    oc.addDescription("tls.green.time", "TLS Building", "Use INT as green phase duration");

    oc.doRegister("tls.yellow.min-decel", 'D', new Option_Float(3.0));
    oc.addSynonyme("tls.yellow.min-decel", "min-decel", true);
    oc.addDescription("tls.yellow.min-decel", "TLS Building", "Defines smallest vehicle deceleration");

    oc.doRegister("tls.yellow.patch-small", new Option_Bool(false));
    oc.addSynonyme("tls.yellow.patch-small", "patch-small-tyellow", true);
    oc.addDescription("tls.yellow.patch-small", "TLS Building", "Given yellow times are patched even if being too short");

    oc.doRegister("tls.yellow.time", new Option_Integer(-1));
    oc.addSynonyme("tls.yellow.time", "traffic-light-yellow", true);
    oc.addDescription("tls.yellow.time", "TLS Building", "Set INT as fixed time for yellow phase durations");

    oc.doRegister("tls.left-green.time", new Option_Integer(6));
    oc.addDescription("tls.left-green.time", "TLS Building", "Use INT as green phase duration for left turns (s). Setting this value to 0 disables additional left-turning phases");

    // tls-shifts
    oc.doRegister("tls.half-offset", new Option_String());
    oc.addSynonyme("tls.half-offset", "tl-logics.half-offset", true);
    oc.addDescription("tls.half-offset", "TLS Building", "TLSs in STR will be shifted by half-phase");

    oc.doRegister("tls.quarter-offset", new Option_String());
    oc.addSynonyme("tls.quarter-offset", "tl-logics.quarter-offset", true);
    oc.addDescription("tls.quarter-offset", "TLS Building", "TLSs in STR will be shifted by quarter-phase");

    // tls type
    oc.doRegister("tls.default-type", new Option_String("static"));
    oc.addDescription("tls.default-type", "TLS Building", "TLSs with unspecified type will use STR as their algorithm");


    // edge pruning
    oc.doRegister("keep-edges.min-speed", new Option_Float(-1));
    oc.addSynonyme("keep-edges.min-speed", "edges-min-speed", true);
    oc.addDescription("keep-edges.min-speed", "Edge Removal", "Only keep edges with speed in meters/second > FLOAT");

    oc.doRegister("remove-edges.explicit", new Option_String());
    oc.addSynonyme("remove-edges.explicit", "remove-edges");
    oc.addDescription("remove-edges.explicit", "Edge Removal", "Remove edges in STR");

    oc.doRegister("keep-edges.explicit", new Option_String());
    oc.addSynonyme("keep-edges.explicit", "keep-edges");
    oc.addDescription("keep-edges.explicit", "Edge Removal", "Only keep edges in STR or those which are kept due to other keep-edges or remove-edges options");

    oc.doRegister("keep-edges.input-file", new Option_FileName());
    oc.addDescription("keep-edges.input-file", "Edge Removal", "Only keep edges in FILE (Each id on a single line. Selection files from SUMO-GUI are also supported) or those which are kept due to other keep-edges or remove-edges options");

    oc.doRegister("remove-edges.input-file", new Option_FileName());
    oc.addDescription("remove-edges.input-file", "Edge Removal", "Remove edges in FILE. (Each id on a single line. Selection files from SUMO-GUI are also supported)");

    if (!forNetgen) {
        oc.doRegister("keep-edges.postload", new Option_Bool(false));
        oc.addDescription("keep-edges.postload", "Edge Removal", "Remove edges after joining");
    }

    oc.doRegister("keep-edges.in-boundary", new Option_String());
    oc.addDescription("keep-edges.in-boundary", "Edge Removal", "Only keep edges which are located within the given boundary (given either as CARTESIAN corner coordinates <xmin,ymin,xmax,ymax> or as polygon <x0,y0,x1,y1,...>)");

    oc.doRegister("keep-edges.in-geo-boundary", new Option_String());
    oc.addDescription("keep-edges.in-geo-boundary", "Edge Removal", "Only keep edges which are located within the given boundary (given either as GEODETIC corner coordinates <lon-min,lat-min,lon-max,lat-max> or as polygon <lon0,lat0,lon1,lat1,...>)");

    if (!forNetgen) {
        oc.doRegister("keep-edges.by-vclass", new Option_String());
        oc.addDescription("keep-edges.by-vclass", "Edge Removal", "Only keep edges which allow one of the vclasss in STR");

        oc.doRegister("remove-edges.by-vclass", new Option_String());
        oc.addDescription("remove-edges.by-vclass", "Edge Removal", "Remove edges which allow only vclasses from STR");

        oc.doRegister("keep-edges.by-type", new Option_String());
        oc.addDescription("keep-edges.by-type", "Edge Removal", "Only keep edges where type is in STR");

        oc.doRegister("remove-edges.by-type", new Option_String());
        oc.addDescription("remove-edges.by-type", "Edge Removal", "Remove edges where type is in STR");

        oc.doRegister("remove-edges.isolated", new Option_Bool(false));
        oc.addSynonyme("remove-edges.isolated", "remove-isolated", true);
        oc.addDescription("remove-edges.isolated", "Edge Removal", "Removes isolated edges");

    }


    // unregulated nodes options
    oc.doRegister("keep-nodes-unregulated", new Option_Bool(false));
    oc.addSynonyme("keep-nodes-unregulated", "keep-unregulated");
    oc.addDescription("keep-nodes-unregulated", "Unregulated Nodes", "All nodes will be unregulated");

    oc.doRegister("keep-nodes-unregulated.explicit", new Option_String());
    oc.addSynonyme("keep-nodes-unregulated.explicit", "keep-unregulated.explicit");
    oc.addSynonyme("keep-nodes-unregulated.explicit", "keep-unregulated.nodes", true);
    oc.addDescription("keep-nodes-unregulated.explicit", "Unregulated Nodes", "Do not regulate nodes in STR");

    oc.doRegister("keep-nodes-unregulated.district-nodes", new Option_Bool(false));
    oc.addSynonyme("keep-nodes-unregulated.district-nodes", "keep-unregulated.district-nodes");
    oc.addDescription("keep-nodes-unregulated.district-nodes", "Unregulated Nodes", "Do not regulate district nodes");

    // ramp guessing options
    if (!forNetgen) {
        oc.doRegister("ramps.guess", new Option_Bool(false));
        oc.addSynonyme("ramps.guess", "guess-ramps", true);
        oc.addDescription("ramps.guess", "Ramp Guessing", "Enable ramp-guessing");

        oc.doRegister("ramps.max-ramp-speed", new Option_Float(-1));
        oc.addSynonyme("ramps.max-ramp-speed", "ramp-guess.max-ramp-speed", true);
        oc.addDescription("ramps.max-ramp-speed", "Ramp Guessing", "Treat edges with speed > FLOAT as no ramps");

        oc.doRegister("ramps.min-highway-speed", new Option_Float((SUMOReal)(79 / 3.6)));
        oc.addSynonyme("ramps.min-highway-speed", "ramp-guess.min-highway-speed", true);
        oc.addDescription("ramps.min-highway-speed", "Ramp Guessing", "Treat edges with speed < FLOAT as no highways");

        oc.doRegister("ramps.ramp-length", new Option_Float(100));
        oc.addSynonyme("ramps.ramp-length", "ramp-guess.ramp-length", true);
        oc.addDescription("ramps.ramp-length", "Ramp Guessing", "Use FLOAT as ramp-length");

        oc.doRegister("ramps.set", new Option_String());
        oc.addSynonyme("ramps.set", "ramp-guess.explicite", true);
        oc.addDescription("ramps.set", "Ramp Guessing", "Tries to handle the given edges as ramps");

        oc.doRegister("ramps.unset", new Option_String());
        oc.addDescription("ramps.unset", "Ramp Guessing", "Do not consider the given edges as ramps");

        oc.doRegister("ramps.no-split", new Option_Bool(false));
        oc.addSynonyme("ramps.no-split", "ramp-guess.no-split", true);
        oc.addDescription("ramps.no-split", "Ramp Guessing", "Avoids edge splitting");
    }
}


bool
NBFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = true;
    //
    if (!oc.isDefault("tls-guess.joining")) {
        WRITE_WARNING("'--tls-guess.joining' was joined with '--tls.join'.\n Please use '--tls.join' in future only.");
        if (!oc.isSet("tls.join")) {
            oc.set("tls.join", "true");
        }
    }
    if (!SUMOXMLDefinitions::TrafficLightTypes.hasString(oc.getString("tls.default-type"))) {
        WRITE_ERROR("unsupported value '" + oc.getString("tls.default-type") + "' for option '--tls.default-type'");
        ok = false;
    }
    if (oc.isSet("keep-edges.in-boundary") && oc.isSet("keep-edges.in-geo-boundary")) {
        WRITE_ERROR("only one of the options 'keep-edges.in-boundary' or 'keep-edges.in-geo-boundary' may be given");
        ok = false;
    }
    if (oc.getBool("no-internal-links") && oc.getBool("crossings.guess")) {
        WRITE_ERROR("only one of the options 'no-internal-links' or 'crossings.guess' may be given");
        ok = false;
    }
    if (!oc.isDefault("tls.green.time") && !oc.isDefault("tls.cycle.time")) {
        WRITE_ERROR("only one of the options 'tls.green.time' or 'tls.cycle.time' may be given");
        ok = false;
    }
    if (oc.getInt("junctions.internal-link-detail") < 2) {
        WRITE_ERROR("junctions.internal-link-detail must >= 2");
        ok = false;
    }
    if (oc.getFloat("junctions.scurve-stretch") > 0) {
        if (oc.getBool("no-internal-links")) {
            WRITE_WARNING("option 'junctions.scurve-stretch' requires internal lanes to work. Option '--no-internal-links' was disabled.");
        }
        // make sure the option is set so heuristics cannot ignore it
        oc.set("no-internal-links", "false");
    }
    return ok;
}


/****************************************************************************/
