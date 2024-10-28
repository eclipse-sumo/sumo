/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    NBFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    09.05.2011
///
// Sets and checks options for netbuild
/****************************************************************************/
#include <config.h>

#include <string>
#include <fstream>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "NBFrame.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include "NBRequest.h"
#include "NBTypeCont.h"

// ===========================================================================
// method definitions
// ===========================================================================

void
NBFrame::fillOptions(OptionsCont& oc, bool forNetgen) {
    // register building defaults
    oc.doRegister("default.lanenumber", 'L', new Option_Integer(1));
    oc.addSynonyme("default.lanenumber", "lanenumber", true);
    oc.addDescription("default.lanenumber", "Building Defaults", TL("The default number of lanes in an edge"));

    oc.doRegister("default.lanewidth", new Option_Float(NBEdge::UNSPECIFIED_WIDTH));
    oc.addSynonyme("default.lanewidth", "lanewidth", true);
    oc.addDescription("default.lanewidth", "Building Defaults", TL("The default width of lanes"));

    oc.doRegister("default.spreadtype", new Option_String("right"));
    oc.addDescription("default.spreadtype", "Building Defaults", TL("The default method for computing lane shapes from edge shapes"));

    oc.doRegister("default.speed", 'S', new Option_Float((double) 13.89));
    oc.addSynonyme("default.speed", "speed", true);
    oc.addDescription("default.speed", "Building Defaults", TL("The default speed on an edge (in m/s)"));

    oc.doRegister("default.friction", new Option_Float(NBEdge::UNSPECIFIED_FRICTION));
    oc.addSynonyme("default.friction", "friction", true);
    oc.addDescription("default.friction", "Building Defaults", TL("The default friction on an edge"));

    oc.doRegister("default.priority", 'P', new Option_Integer(-1));
    oc.addSynonyme("default.priority", "priority", true);
    oc.addDescription("default.priority", "Building Defaults", TL("The default priority of an edge"));

    oc.doRegister("default.type", new Option_String());
    oc.addDescription("default.type", "Building Defaults", TL("The default edge type"));

    oc.doRegister("default.sidewalk-width", new Option_Float((double) 2.0));
    oc.addDescription("default.sidewalk-width", "Building Defaults", TL("The default width of added sidewalks"));

    oc.doRegister("default.bikelane-width", new Option_Float((double) 1.0));
    oc.addDescription("default.bikelane-width", "Building Defaults", TL("The default width of added bike lanes"));

    oc.doRegister("default.crossing-width", new Option_Float((double) 4.0));
    oc.addDescription("default.crossing-width", "Building Defaults", TL("The default width of a pedestrian crossing"));

    oc.doRegister("default.crossing-speed", new Option_Float(2.78));
    oc.addDescription("default.crossing-speed", "Building Defaults", TL("The default speed 'limit' on a pedestrian crossing (in m/s)"));

    oc.doRegister("default.walkingarea-speed", new Option_Float(2.78));
    oc.addDescription("default.walkingarea-speed", "Building Defaults", TL("The default speed 'limit' on a pedestrian walkingarea (in m/s)"));

    oc.doRegister("default.allow", new Option_String());
    oc.addDescription("default.allow", "Building Defaults", TL("The default for allowed vehicle classes"));

    oc.doRegister("default.disallow", new Option_String());
    oc.addDescription("default.disallow", "Building Defaults", TL("The default for disallowed vehicle classes"));

    oc.doRegister("default.junctions.keep-clear", new Option_Bool(true));
    oc.addDescription("default.junctions.keep-clear", "Building Defaults", TL("Whether junctions should be kept clear by default"));

    oc.doRegister("default.junctions.radius", new Option_Float(4));
    oc.addDescription("default.junctions.radius", "Building Defaults", TL("The default turning radius of intersections"));

    oc.doRegister("default.connection-length", new Option_Float((double) NBEdge::UNSPECIFIED_LOADED_LENGTH));
    oc.addDescription("default.connection-length", "Building Defaults", TL("The default length when overriding connection lengths"));

    oc.doRegister("default.connection.cont-pos", new Option_Float((double)NBEdge::UNSPECIFIED_CONTPOS));
    oc.addDescription("default.connection.cont-pos", "Building Defaults", TL("Whether/where connections should have an internal junction"));

    oc.doRegister("default.right-of-way", new Option_String("default"));
    oc.addDescription("default.right-of-way", "Building Defaults", TL("The default algorithm for computing right of way rules ('default', 'edgePriority')"));

    oc.doRegister("junctions.right-before-left.speed-threshold", new Option_Float(49 / 3.6));
    oc.addDescription("junctions.right-before-left.speed-threshold", "Junctions", TL("Allow building right-before-left junctions when the incoming edge speeds are below FLOAT (m/s)"));

    oc.doRegister("junctions.left-before-right", new Option_Bool(false));
    oc.addDescription("junctions.left-before-right", "Junctions", TL("Build left-before-right junctions instead of right-before-left junctions"));

    // register the data processing options
    oc.doRegister("no-internal-links", new Option_Bool(false)); // !!! not described
    oc.addDescription("no-internal-links", "Junctions", TL("Omits internal links"));

    oc.doRegister("numerical-ids", new Option_Bool(false));
    oc.addDescription("numerical-ids", "Processing", TL("Remaps alphanumerical IDs of nodes and edges to ensure that all IDs are integers"));

    oc.doRegister("numerical-ids.node-start", new Option_Integer(std::numeric_limits<int>::max()));
    oc.addDescription("numerical-ids.node-start", "Processing", TL("Remaps IDs of nodes to integers starting at INT"));

    oc.doRegister("numerical-ids.edge-start", new Option_Integer(std::numeric_limits<int>::max()));
    oc.addDescription("numerical-ids.edge-start", "Processing", TL("Remaps IDs of edges to integers starting at INT"));

    /// @todo not working for netgen
    oc.doRegister("reserved-ids", new Option_FileName());
    oc.addDescription("reserved-ids", "Processing", TL("Ensures that generated ids do not included any of the typed IDs from FILE (sumo-gui selection file format)"));

    if (!forNetgen) {
        oc.doRegister("dismiss-vclasses", new Option_Bool(false));
        oc.addDescription("dismiss-vclasses", "Processing", TL("Removes vehicle class restrictions from imported edges"));
    }

    oc.doRegister("no-turnarounds", new Option_Bool(false));
    oc.addDescription("no-turnarounds", "Junctions", TL("Disables building turnarounds"));

    oc.doRegister("no-turnarounds.tls", new Option_Bool(false));
    oc.addSynonyme("no-turnarounds.tls", "no-tls-turnarounds", true);
    oc.addDescription("no-turnarounds.tls", "Junctions", TL("Disables building turnarounds at tls-controlled junctions"));

    oc.doRegister("no-turnarounds.geometry", new Option_Bool(true));
    oc.addDescription("no-turnarounds.geometry", "Junctions", TL("Disables building turnarounds at geometry-like junctions"));

    oc.doRegister("no-turnarounds.except-deadend", new Option_Bool(false));
    oc.addDescription("no-turnarounds.except-deadend", "Junctions", TL("Disables building turnarounds except at dead end junctions"));

    oc.doRegister("no-turnarounds.except-turnlane", new Option_Bool(false));
    oc.addDescription("no-turnarounds.except-turnlane", "Junctions", TL("Disables building turnarounds except at at junctions with a dedicated turning lane"));

    oc.doRegister("no-turnarounds.fringe", new Option_Bool(false));
    oc.addDescription("no-turnarounds.fringe", "Junctions", TL("Disables building turnarounds at fringe junctions"));

    oc.doRegister("no-left-connections", new Option_Bool(false));
    oc.addDescription("no-left-connections", "Junctions", TL("Disables building connections to left"));

    oc.doRegister("geometry.split", new Option_Bool(false)); // !!!not described
    oc.addSynonyme("geometry.split", "split-geometry", true);
    oc.addDescription("geometry.split", "Processing", TL("Splits edges across geometry nodes"));

    oc.doRegister("geometry.remove", 'R', new Option_Bool(false));
    oc.addSynonyme("geometry.remove", "remove-geometry", true);
    oc.addDescription("geometry.remove", "Processing", TL("Replace nodes which only define edge geometry by geometry points (joins edges)"));

    oc.doRegister("geometry.remove.keep-edges.explicit", new Option_StringVector());
    oc.addDescription("geometry.remove.keep-edges.explicit", "Processing", TL("Ensure that the given list of edges is not modified"));

    oc.doRegister("geometry.remove.keep-edges.input-file", new Option_FileName());
    oc.addDescription("geometry.remove.keep-edges.input-file", "Processing",
                      "Ensure that the edges in FILE are not modified (Each id on a single line. Selection files from sumo-gui are also supported)");

    if (!forNetgen) {
        oc.doRegister("geometry.remove.keep-ptstops", new Option_Bool(false));
        oc.addDescription("geometry.remove.keep-ptstops", "Processing", TL("Ensure that edges with public transport stops are not modified"));
    }

    oc.doRegister("geometry.remove.min-length", new Option_Float(0));
    oc.addDescription("geometry.remove.min-length", "Processing",
                      "Allow merging edges with differing attributes when their length is below min-length");

    oc.doRegister("geometry.remove.width-tolerance", new Option_Float(0));
    oc.addDescription("geometry.remove.width-tolerance", "Processing",
                      "Allow merging edges with differing lane widths if the difference is below FLOAT");

    oc.doRegister("geometry.remove.max-junction-size", new Option_Float(-1));
    oc.addDescription("geometry.remove.max-junction-size", "Processing",
                      "Prevent removal of junctions with a size above FLOAT as defined by custom edge endpoints");

    oc.doRegister("geometry.max-segment-length", new Option_Float(0));
    oc.addDescription("geometry.max-segment-length", "Processing", TL("splits geometry to restrict segment length"));

    if (!forNetgen) {
        oc.doRegister("geometry.min-dist", new Option_Float(-1));
        oc.addDescription("geometry.min-dist", "Processing", TL("reduces too similar geometry points"));

        oc.doRegister("geometry.max-angle", new Option_Float(99));
        oc.addDescription("geometry.max-angle", "Processing", TL("Warn about edge geometries with an angle above DEGREES in successive segments"));

        oc.doRegister("geometry.max-angle.fix", new Option_Bool(false));
        oc.addDescription("geometry.max-angle.fix", "Processing", TL("Straighten edge geometries with an angle above max-angle successive segments"));

        oc.doRegister("geometry.min-radius", new Option_Float(9));
        oc.addDescription("geometry.min-radius", "Processing", TL("Warn about edge geometries with a turning radius less than METERS at the start or end"));

        oc.doRegister("geometry.min-radius.fix", new Option_Bool(false));
        oc.addDescription("geometry.min-radius.fix", "Processing", TL("Straighten edge geometries to avoid turning radii less than geometry.min-radius"));

        oc.doRegister("geometry.min-radius.fix.railways", new Option_Bool(true));
        oc.addDescription("geometry.min-radius.fix.railways", "Processing", TL("Straighten edge geometries to avoid turning radii less than geometry.min-radius (only railways)"));

        oc.doRegister("geometry.junction-mismatch-threshold", new Option_Float(20));
        oc.addDescription("geometry.junction-mismatch-threshold", "Processing", TL("Warn if the junction shape is to far away from the original node position"));

        oc.doRegister("geometry.check-overlap", new Option_Float(0));
        oc.addDescription("geometry.check-overlap", "Processing", TL("Warn if edges overlap by more than the given threshold value"));

        oc.doRegister("geometry.check-overlap.vertical-threshold", new Option_Float(4));
        oc.addDescription("geometry.check-overlap.vertical-threshold", "Processing", TL("Ignore overlapping edges if they are separated vertically by the given threshold."));

        oc.doRegister("geometry.avoid-overlap", new Option_Bool(true));
        oc.addDescription("geometry.avoid-overlap", "Processing", TL("Modify edge geometries to avoid overlap at junctions"));

        oc.doRegister("join-lanes", new Option_Bool(false));
        oc.addDescription("join-lanes", "Processing", TL("join adjacent lanes that have the same permissions and which do not admit lane-changing (sidewalks and disallowed lanes)"));

        oc.doRegister("ptline.match-dist", new Option_Float(100));
        oc.addDescription("ptline.match-dist", "Processing", TL("Matches stops outside the road network to the referencing pt line when below the given distance"));
        // railway processing options

        oc.doRegister("railway.topology.repair", new Option_Bool(false));
        oc.addDescription("railway.topology.repair", "Railway", TL("Repair topology of the railway network"));

        oc.doRegister("railway.topology.repair.minimal", new Option_Bool(false));
        oc.addDescription("railway.topology.repair.minimal", "Railway", TL("Repair topology of the railway network just enough to let loaded public transport lines to work"));

        oc.doRegister("railway.topology.repair.connect-straight", new Option_Bool(false));
        oc.addDescription("railway.topology.repair.connect-straight", "Railway", TL("Allow bidirectional rail use wherever rails with opposite directions meet at a straight angle"));

        oc.doRegister("railway.topology.repair.stop-turn", new Option_Bool(false));
        oc.addDescription("railway.topology.repair.stop-turn", "Railway", TL("Add turn-around connections at all loaded stops."));

        oc.doRegister("railway.topology.repair.bidi-penalty", new Option_Float(1.2));
        oc.addDescription("railway.topology.repair.bidi-penalty", "Railway", TL("Penalty factor for adding new bidi edges to connect public transport stops"));

        oc.doRegister("railway.topology.all-bidi", new Option_Bool(false));
        oc.addDescription("railway.topology.all-bidi", "Railway", TL("Make all rails usable in both direction"));

        oc.doRegister("railway.topology.all-bidi.input-file", new Option_FileName());
        oc.addDescription("railway.topology.all-bidi.input-file", "Railway", TL("Make all rails edge ids from FILE usable in both direction"));

        oc.doRegister("railway.topology.direction-priority", new Option_Bool(false));
        oc.addDescription("railway.topology.direction-priority", "Railway", TL("Set edge priority values based on estimated main direction"));

        oc.doRegister("railway.topology.extend-priority", new Option_Bool(false));
        oc.addDescription("railway.topology.extend-priority", "Railway", TL("Extend loaded edge priority values based on estimated main direction"));

        oc.doRegister("railway.geometry.straighten", new Option_Bool(false));
        oc.addDescription("railway.geometry.straighten", "Railway", TL("Move junctions to straighten a sequence of rail edges"));

        oc.doRegister("railway.signal.guess.by-stops", new Option_Bool(false));
        oc.addDescription("railway.signal.guess.by-stops", "Railway", TL("Guess signals that guard public transport stops"));

        oc.doRegister("railway.access-distance", new Option_Float(150.f));
        oc.addDescription("railway.access-distance", "Railway", TL("The search radius for finding suitable road accesses for rail stops"));
        oc.addSynonyme("railway.access-distance", "osm.stop-output.footway-access-distance", true);

        oc.doRegister("railway.max-accesses", new Option_Integer(5));
        oc.addDescription("railway.max-accesses", "Railway", TL("The maximum road accesses registered per rail stops"));
        oc.addSynonyme("railway.max-accesses", "osm.stop-output.footway-max-accesses", true);

        oc.doRegister("railway.access-factor", new Option_Float(1.5));
        oc.addDescription("railway.access-factor", "Railway", TL("The walking length of the access is computed as air-line distance multiplied by FLOAT"));
        oc.addSynonyme("railway.access-factor", "osm.stop-output.footway-access-factor", true);

        oc.doRegister("ptstop-output.no-bidi", new Option_Bool(false));
        oc.addDescription("ptstop-output.no-bidi", "Processing", TL("Skips automatic generation of stops on the bidi-edge of a loaded stop"));
    }

    oc.doRegister("geometry.max-grade", new Option_Float(10));
    oc.addDescription("geometry.max-grade", "Processing", TL("Warn about edge geometries with a grade in % above FLOAT."));

    oc.doRegister("geometry.max-grade.fix", new Option_Bool(true));
    oc.addDescription("geometry.max-grade.fix", "Processing", TL("Smooth edge geometries with a grade above the warning threshold."));

    oc.doRegister("offset.disable-normalization", new Option_Bool(false));
    oc.addSynonyme("offset.disable-normalization", "disable-normalize-node-positions", true);
    oc.addDescription("offset.disable-normalization", "Processing", TL("Turn off normalizing node positions"));

    oc.doRegister("offset.x", new Option_Float(0));
    oc.addSynonyme("offset.x", "x-offset-to-apply", true);
    oc.addDescription("offset.x", "Processing", TL("Adds FLOAT to net x-positions"));

    oc.doRegister("offset.y", new Option_Float(0));
    oc.addSynonyme("offset.y", "y-offset-to-apply", true);
    oc.addDescription("offset.y", "Processing", TL("Adds FLOAT to net y-positions"));

    oc.doRegister("offset.z", new Option_Float(0));
    oc.addDescription("offset.z", "Processing", TL("Adds FLOAT to net z-positions"));

    oc.doRegister("flip-y-axis", new Option_Bool(false));
    oc.addSynonyme("flip-y-axis", "flip-y");
    oc.addDescription("flip-y-axis", "Processing", TL("Flips the y-coordinate along zero"));

    oc.doRegister("roundabouts.guess", new Option_Bool(true));
    oc.addSynonyme("roundabouts.guess", "guess-roundabouts", true);
    oc.addDescription("roundabouts.guess", "Processing", TL("Enable roundabout-guessing"));

    // The Putrajaya Roundabout (Malaysia) holds the Guinness record for the world’s largest roundabout with 3.4km.
    oc.doRegister("roundabouts.guess.max-length", new Option_Float(3500));
    oc.addDescription("roundabouts.guess.max-length", "Processing", TL("Structures with a circumference above FLOAT threshold are not classified as roundabout"));

    oc.doRegister("roundabouts.visibility-distance", new Option_Float(9));
    oc.addDescription("roundabouts.visibility-distance", "Processing", TL("Default visibility when approaching a roundabout"));

    oc.doRegister("opposites.guess", new Option_Bool(false));
    oc.addDescription("opposites.guess", "Processing", TL("Enable guessing of opposite direction lanes usable for overtaking"));

    oc.doRegister("opposites.guess.fix-lengths", new Option_Bool(true));
    oc.addDescription("opposites.guess.fix-lengths", "Processing", TL("Ensure that opposite edges have the same length"));

    oc.doRegister("fringe.guess", new Option_Bool(false));
    oc.addDescription("fringe.guess", "Processing", TL("Enable guessing of network fringe nodes"));

    oc.doRegister("fringe.guess.speed-threshold", new Option_Float(50 / 3.6));
    oc.addDescription("fringe.guess.speed-threshold", "Processing", TL("Guess disconnected edges above the given speed as outer fringe"));

    oc.doRegister("lefthand", new Option_Bool(false));
    oc.addDescription("lefthand", "Processing", TL("Assumes left-hand traffic on the network"));

    oc.doRegister("edges.join", new Option_Bool(false));
    oc.addDescription("edges.join", "Processing",
                      "Merges edges which connect the same nodes and are close to each other (recommended for VISSIM import)");

    oc.doRegister("junctions.join", new Option_Bool(false));
    oc.addDescription("junctions.join", "Junctions",
                      "Joins junctions that are close to each other (recommended for OSM import)");

    oc.doRegister("junctions.join-dist", new Option_Float(10));
    oc.addDescription("junctions.join-dist", "Junctions",
                      "Determines the maximal distance for joining junctions (defaults to 10)");

    if (!forNetgen) {
        oc.doRegister("junctions.join-exclude", new Option_StringVector());
        oc.addDescription("junctions.join-exclude", "Junctions", TL("Interprets STR[] as list of junctions to exclude from joining"));
    }

    oc.doRegister("junctions.join-same", new Option_Bool(false));
    oc.addDescription("junctions.join-same", "Junctions",
                      "Joins junctions that have the same coordinates even if not connected");

    oc.doRegister("max-join-ids", new Option_Integer(4));
    oc.addDescription("max-join-ids", "Junctions", "Abbreviate junction or TLS id if it joins more than INT junctions");

    if (!forNetgen) {
        oc.doRegister("speed.offset", new Option_Float(0));
        oc.addDescription("speed.offset", "Processing", TL("Modifies all edge speeds by adding FLOAT"));

        oc.doRegister("speed.factor", new Option_Float(1));
        oc.addDescription("speed.factor", "Processing", TL("Modifies all edge speeds by multiplying by FLOAT"));

        oc.doRegister("speed.minimum", new Option_Float(0));
        oc.addDescription("speed.minimum", "Processing", TL("Modifies all edge speeds to at least FLOAT"));

        oc.doRegister("edges.join-tram-dist", new Option_Float(-1));
        oc.addDescription("edges.join-tram-dist", "Processing",
                          "Joins tram edges into road lanes with similar geometry (within FLOAT distance)");
    }

    oc.doRegister("junctions.corner-detail", new Option_Integer(5));
    oc.addDescription("junctions.corner-detail", "Junctions", TL("Generate INT intermediate points to smooth out intersection corners"));

    oc.doRegister("junctions.internal-link-detail", new Option_Integer(5));
    oc.addDescription("junctions.internal-link-detail", "Junctions", TL("Generate INT intermediate points to smooth out lanes within the intersection"));

    oc.doRegister("junctions.scurve-stretch", new Option_Float(0));
    oc.addDescription("junctions.scurve-stretch", "Junctions", TL("Generate longer intersections to allow for smooth s-curves when the number of lanes changes"));

    oc.doRegister("junctions.join-turns", new Option_Bool(false));
    oc.addDescription("junctions.join-turns", "Junctions",
                      "Builds common edges for turning connections with common from- and to-edge. This causes discrepancies between geometrical length and assigned length due to averaging but enables lane-changing while turning");

    oc.doRegister("junctions.limit-turn-speed", new Option_Float(5.5));
    oc.addDescription("junctions.limit-turn-speed", "Junctions",
                      "Limits speed on junctions to an average lateral acceleration of at most FLOAT (m/s^2)");

    oc.doRegister("junctions.limit-turn-speed.min-angle", new Option_Float(15));
    oc.addDescription("junctions.limit-turn-speed.min-angle", "Junctions",
                      "Do not limit turn speed for angular changes below FLOAT (degrees). The value is subtracted from the geometric angle before computing the turning radius.");

    oc.doRegister("junctions.limit-turn-speed.min-angle.railway", new Option_Float(35));
    oc.addDescription("junctions.limit-turn-speed.min-angle.railway", "Junctions",
                      "Do not limit turn speed for angular changes below FLOAT (degrees) on railway edges. The value is subtracted from the geometric angle before computing the turning radius.");

    oc.doRegister("junctions.limit-turn-speed.warn.straight", new Option_Float(5));
    oc.addDescription("junctions.limit-turn-speed.warn.straight", "Junctions",
                      "Warn about turn speed limits that reduce the speed of straight connections by more than FLOAT");

    oc.doRegister("junctions.limit-turn-speed.warn.turn", new Option_Float(22));
    oc.addDescription("junctions.limit-turn-speed.warn.turn", "Junctions",
                      "Warn about turn speed limits that reduce the speed of turning connections (no u-turns) by more than FLOAT");

    oc.doRegister("junctions.small-radius", new Option_Float(1.5));
    oc.addDescription("junctions.small-radius", "Junctions",
                      "Default radius for junctions that do not require wide vehicle turns");

    oc.doRegister("junctions.higher-speed", new Option_Bool(false));
    oc.addDescription("junctions.higher-speed", "Junctions",
                      "Use maximum value of incoming and outgoing edge speed on junction instead of average");

    oc.doRegister("junctions.minimal-shape", new Option_Bool(false));
    oc.addDescription("junctions.minimal-shape", "Junctions",
                      "Build junctions with minimal shapes (ignoring edge overlap)");

    oc.doRegister("junctions.endpoint-shape", new Option_Bool(false));
    oc.addDescription("junctions.endpoint-shape", "Junctions",
                      "Build junction shapes based on edge endpoints (ignoring edge overlap)");

    oc.doRegister("internal-junctions.vehicle-width", new Option_Float(1.8));
    oc.addDescription("internal-junctions.vehicle-width", "Junctions",
                      "Assumed vehicle width for computing internal junction positions");

    oc.doRegister("rectangular-lane-cut", new Option_Bool(false));
    oc.addDescription("rectangular-lane-cut", "Junctions", TL("Forces rectangular cuts between lanes and intersections"));

    oc.doRegister("check-lane-foes.roundabout", new Option_Bool(true));
    oc.addDescription("check-lane-foes.roundabout", "Junctions",
                      "Allow driving onto a multi-lane road if there are foes on other lanes (at roundabouts)");

    oc.doRegister("check-lane-foes.all", new Option_Bool(false));
    oc.addDescription("check-lane-foes.all", "Junctions",
                      "Allow driving onto a multi-lane road if there are foes on other lanes (everywhere)");

    oc.doRegister("sidewalks.guess", new Option_Bool(false));
    oc.addDescription("sidewalks.guess", "Pedestrian",
                      "Guess pedestrian sidewalks based on edge speed");

    oc.doRegister("sidewalks.guess.max-speed", new Option_Float((double) 13.89));
    oc.addDescription("sidewalks.guess.max-speed", "Pedestrian",
                      "Add sidewalks for edges with a speed equal or below the given limit");

    oc.doRegister("sidewalks.guess.min-speed", new Option_Float((double) 5.8));
    oc.addDescription("sidewalks.guess.min-speed", "Pedestrian",
                      "Add sidewalks for edges with a speed above the given limit");

    oc.doRegister("sidewalks.guess.from-permissions", new Option_Bool(false));
    oc.addDescription("sidewalks.guess.from-permissions", "Pedestrian",
                      "Add sidewalks for edges that allow pedestrians on any of their lanes regardless of speed");

    oc.doRegister("sidewalks.guess.exclude", new Option_StringVector());
    oc.addDescription("sidewalks.guess.exclude", "Pedestrian",
                      "Do not guess sidewalks for the given list of edges");

    oc.doRegister("bikelanes.guess", new Option_Bool(false));
    oc.addDescription("bikelanes.guess", "Bicycle",
                      "Guess bike lanes based on edge speed");

    oc.doRegister("bikelanes.guess.max-speed", new Option_Float((double) 22.22));
    oc.addDescription("bikelanes.guess.max-speed", "Bicycle",
                      "Add bike lanes for edges with a speed equal or below the given limit");

    oc.doRegister("bikelanes.guess.min-speed", new Option_Float((double) 5.8));
    oc.addDescription("bikelanes.guess.min-speed", "Bicycle",
                      "Add bike lanes for edges with a speed above the given limit");

    oc.doRegister("bikelanes.guess.from-permissions", new Option_Bool(false));
    oc.addDescription("bikelanes.guess.from-permissions", "Bicycle",
                      "Add bike lanes for edges that allow bicycles on any of their lanes regardless of speed");

    oc.doRegister("bikelanes.guess.exclude", new Option_StringVector());
    oc.addDescription("bikelanes.guess.exclude", "Bicycle",
                      "Do not guess bikelanes for the given list of edges");

    oc.doRegister("crossings.guess", new Option_Bool(false));
    oc.addDescription("crossings.guess", "Pedestrian",
                      "Guess pedestrian crossings based on the presence of sidewalks");

    oc.doRegister("crossings.guess.speed-threshold", new Option_Float(13.89));
    oc.addDescription("crossings.guess.speed-threshold", "Pedestrian",
                      "At uncontrolled nodes, do not build crossings across edges with a speed above the threshold");

    oc.doRegister("walkingareas", new Option_Bool(false));
    oc.addDescription("walkingareas", "Pedestrian", TL("Always build walking areas even if there are no crossings"));

    oc.doRegister("walkingareas.join-dist", new Option_Float(15));
    oc.addDescription("walkingareas.join-dist", "Pedestrian", TL("Do not create a walkingarea between sidewalks that are connected by a pedestrian junction within FLOAT"));

    // tls setting options
    // explicit tls
    oc.doRegister("tls.set", new Option_StringVector());
    oc.addSynonyme("tls.set", "explicite-tls", true);
    oc.addDescription("tls.set", "TLS Building", "Interprets STR[] as list of junctions to be controlled by TLS");

    oc.doRegister("tls.unset", new Option_StringVector());
    oc.addSynonyme("tls.unset", "explicite-no-tls", true);
    oc.addDescription("tls.unset", "TLS Building", "Interprets STR[] as list of junctions to be not controlled by TLS");

    // tls-guessing
    oc.doRegister("tls.guess", new Option_Bool(false));
    oc.addSynonyme("tls.guess", "guess-tls", true);
    oc.addDescription("tls.guess", "TLS Building", "Turns on TLS guessing");

    oc.doRegister("tls.guess.threshold", new Option_Float(250 / 3.6));
    oc.addDescription("tls.guess.threshold", "TLS Building", "Sets minimum value for the sum of all incoming lane speeds when guessing TLS");

    if (!forNetgen) {
        oc.doRegister("tls.taz-nodes", new Option_Bool(false));
        oc.addSynonyme("tls.taz-nodes", "tls-guess.district-nodes", true);
        oc.addDescription("tls.taz-nodes", "TLS Building", "Sets district nodes as tls-controlled"); // !!! describe
    }

    oc.doRegister("tls.guess.joining", new Option_Bool(false));
    oc.addSynonyme("tls.guess.joining", "tls-guess.joining", true);
    oc.addDescription("tls.guess.joining", "TLS Building", "Includes node clusters into guess"); // !!! describe

    oc.doRegister("tls.join", new Option_Bool(false));
    oc.addSynonyme("tls.join", "try-join-tls", true);
    oc.addDescription("tls.join", "TLS Building", "Tries to cluster tls-controlled nodes"); // !!! describe

    oc.doRegister("tls.join-dist", new Option_Float(20));
    oc.addDescription("tls.join-dist", "TLS Building",
                      "Determines the maximal distance for joining traffic lights (defaults to 20)");

    oc.doRegister("tls.join-exclude", new Option_StringVector());
    oc.addDescription("tls.join-exclude", "TLS Building", TL("Interprets STR[] as list of tls ids to exclude from joining"));

    oc.doRegister("tls.uncontrolled-within", new Option_Bool(false));
    oc.addDescription("tls.uncontrolled-within", "TLS Building",
                      "Do not control edges that lie fully within a joined traffic light. This may cause collisions but allows old traffic light plans to be used");

    oc.doRegister("tls.ignore-internal-junction-jam", new Option_Bool(false));
    oc.addDescription("tls.ignore-internal-junction-jam", "TLS Building",
                      "Do not build mutually conflicting response matrix, potentially ignoring vehicles that are stuck at an internal junction when their phase has ended");

    if (!forNetgen) {
        oc.doRegister("tls.guess-signals", new Option_Bool(false));
        oc.addDescription("tls.guess-signals", "TLS Building", "Interprets tls nodes surrounding an intersection as signal positions for a larger TLS. This is typical pattern for OSM-derived networks");

        oc.doRegister("tls.guess-signals.dist", new Option_Float(25));
        oc.addDescription("tls.guess-signals.dist", "TLS Building", "Distance for interpreting nodes as signal locations");

        oc.doRegister("tls.guess-signals.slack", new Option_Integer(0));
        oc.addDescription("tls.guess-signals.slack", "TLS Building", "Number of uncontrolled entry edges to accept and still consider the central node as a traffic light");
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

    oc.doRegister("tls.red.time", new Option_Integer(5));
    oc.addDescription("tls.red.time", "TLS Building", "Set INT as fixed time for red phase duration at traffic lights that do not have a conflicting flow");

    oc.doRegister("tls.allred.time", new Option_Integer(0));
    oc.addDescription("tls.allred.time", "TLS Building", "Set INT as fixed time for intermediate red phase after every switch");

    oc.doRegister("tls.minor-left.max-speed", new Option_Float(19.44)); // 70km/h
    oc.addDescription("tls.minor-left.max-speed", "TLS Building", "Use FLOAT as threshold for allowing left-turning vehicles to move in the same phase as oncoming straight-going vehicles");

    oc.doRegister("tls.left-green.time", new Option_Integer(6));
    oc.addDescription("tls.left-green.time", "TLS Building", "Use INT as green phase duration for left turns (s). Setting this value to 0 disables additional left-turning phases");

    oc.doRegister("tls.nema.vehExt", new Option_Integer(2));
    oc.addDescription("tls.nema.vehExt", "TLS Building", "Set INT as fixed time for intermediate vehext phase after every switch");

    oc.doRegister("tls.nema.yellow", new Option_Integer(3));
    oc.addDescription("tls.nema.yellow", "TLS Building", "Set INT as fixed time for intermediate NEMA yellow phase after every switch");

    oc.doRegister("tls.nema.red", new Option_Integer(2));
    oc.addDescription("tls.nema.red", "TLS Building", "Set INT as fixed time for intermediate NEMA red phase after every switch");

    oc.doRegister("tls.crossing-min.time", new Option_Integer(4));
    oc.addDescription("tls.crossing-min.time", "TLS Building", "Use INT as minimum green duration for pedestrian crossings (s).");

    oc.doRegister("tls.crossing-clearance.time", new Option_Integer(5));
    oc.addDescription("tls.crossing-clearance.time", "TLS Building", "Use INT as clearance time for pedestrian crossings (s).");

    oc.doRegister("tls.scramble.time", new Option_Integer(5));
    oc.addDescription("tls.scramble.time", "TLS Building", "Use INT as green phase duration for pedestrian scramble phase (s).");

    // tls-shifts
    oc.doRegister("tls.half-offset", new Option_StringVector());
    oc.addSynonyme("tls.half-offset", "tl-logics.half-offset", true);
    oc.addDescription("tls.half-offset", "TLS Building", "TLSs in STR[] will be shifted by half-phase");

    oc.doRegister("tls.quarter-offset", new Option_StringVector());
    oc.addSynonyme("tls.quarter-offset", "tl-logics.quarter-offset", true);
    oc.addDescription("tls.quarter-offset", "TLS Building", "TLSs in STR[] will be shifted by quarter-phase");

    // tls type
    oc.doRegister("tls.default-type", new Option_String("static"));
    oc.addDescription("tls.default-type", "TLS Building", "TLSs with unspecified type will use STR as their algorithm");

    oc.doRegister("tls.layout", new Option_String("opposites"));
    oc.addDescription("tls.layout", "TLS Building", "Set phase layout four grouping opposite directions or grouping all movements for one incoming edge ['opposites', 'incoming']");

    oc.doRegister("tls.no-mixed", new Option_Bool(false));
    oc.addDescription("tls.no-mixed", "TLS Building", "Avoid phases with green and red signals for different connections from the same lane");

    oc.doRegister("tls.min-dur", new Option_Integer(5));
    oc.addDescription("tls.min-dur", "TLS Building", "Default minimum phase duration for traffic lights with variable phase length");

    oc.doRegister("tls.max-dur", new Option_Integer(50));
    oc.addDescription("tls.max-dur", "TLS Building", "Default maximum phase duration for traffic lights with variable phase length");

    oc.doRegister("tls.group-signals", new Option_Bool(false));
    oc.addDescription("tls.group-signals", "TLS Building", "Assign the same tls link index to connections that share the same states");

    oc.doRegister("tls.ungroup-signals", new Option_Bool(false));
    oc.addDescription("tls.ungroup-signals", "TLS Building", "Assign a distinct tls link index to every connection");

    oc.doRegister("tls.rebuild", new Option_Bool(false));
    oc.addDescription("tls.rebuild", "TLS Building", "rebuild all traffic light plans in the network");

    // edge pruning
    oc.doRegister("keep-edges.min-speed", new Option_Float(-1));
    oc.addSynonyme("keep-edges.min-speed", "edges-min-speed", true);
    oc.addDescription("keep-edges.min-speed", "Edge Removal", TL("Only keep edges with speed in meters/second > FLOAT"));

    oc.doRegister("remove-edges.explicit", new Option_StringVector());
    oc.addSynonyme("remove-edges.explicit", "remove-edges");
    oc.addDescription("remove-edges.explicit", "Edge Removal", TL("Remove edges in STR[]"));

    oc.doRegister("keep-edges.explicit", new Option_StringVector());
    oc.addSynonyme("keep-edges.explicit", "keep-edges");
    oc.addDescription("keep-edges.explicit", "Edge Removal", TL("Only keep edges in STR[] or those which are kept due to other keep-edges or remove-edges options"));

    oc.doRegister("keep-edges.input-file", new Option_FileName());
    oc.addDescription("keep-edges.input-file", "Edge Removal", TL("Only keep edges in FILE (Each id on a single line. Selection files from sumo-gui are also supported) or those which are kept due to other keep-edges or remove-edges options"));

    oc.doRegister("remove-edges.input-file", new Option_FileName());
    oc.addDescription("remove-edges.input-file", "Edge Removal", TL("Remove edges in FILE. (Each id on a single line. Selection files from sumo-gui are also supported)"));

    if (!forNetgen) {
        oc.doRegister("keep-edges.postload", new Option_Bool(false));
        oc.addDescription("keep-edges.postload", "Edge Removal", TL("Remove edges after loading, patching and joining"));
    }

    oc.doRegister("keep-edges.in-boundary", new Option_StringVector());
    oc.addDescription("keep-edges.in-boundary", "Edge Removal", TL("Only keep edges which are located within the given boundary (given either as CARTESIAN corner coordinates <xmin,ymin,xmax,ymax> or as polygon <x0,y0,x1,y1,...>)"));

    oc.doRegister("keep-edges.in-geo-boundary", new Option_StringVector());
    oc.addDescription("keep-edges.in-geo-boundary", "Edge Removal", TL("Only keep edges which are located within the given boundary (given either as GEODETIC corner coordinates <lon-min,lat-min,lon-max,lat-max> or as polygon <lon0,lat0,lon1,lat1,...>)"));

    if (!forNetgen) {
        oc.doRegister("keep-edges.by-vclass", new Option_StringVector());
        oc.addDescription("keep-edges.by-vclass", "Edge Removal", TL("Only keep edges which allow one of the vclasses in STR[]"));

        oc.doRegister("remove-edges.by-vclass", new Option_StringVector());
        oc.addDescription("remove-edges.by-vclass", "Edge Removal", TL("Remove edges which allow only vclasses from STR[]"));

        oc.doRegister("keep-edges.by-type", new Option_StringVector());
        oc.addDescription("keep-edges.by-type", "Edge Removal", TL("Only keep edges where type is in STR[]"));

        oc.doRegister("keep-edges.components", new Option_Integer(0));
        oc.addDescription("keep-edges.components", "Edge Removal", TL("Only keep the INT largest weakly connected components"));

        oc.doRegister("remove-edges.by-type", new Option_StringVector());
        oc.addDescription("remove-edges.by-type", "Edge Removal", TL("Remove edges where type is in STR[]"));

        oc.doRegister("remove-edges.isolated", new Option_Bool(false));
        oc.addSynonyme("remove-edges.isolated", "remove-isolated", true);
        oc.addDescription("remove-edges.isolated", "Edge Removal", TL("Removes isolated edges"));
    }

    oc.doRegister("keep-lanes.min-width", new Option_Float(0.01));
    oc.addDescription("keep-lanes.min-width", "Edge Removal", TL("Only keep lanes with width in meters > FLOAT"));


    // unregulated nodes options
    oc.doRegister("keep-nodes-unregulated", new Option_Bool(false));
    oc.addSynonyme("keep-nodes-unregulated", "keep-unregulated");
    oc.addDescription("keep-nodes-unregulated", "Unregulated Nodes", TL("All nodes will be unregulated"));

    oc.doRegister("keep-nodes-unregulated.explicit", new Option_StringVector());
    oc.addSynonyme("keep-nodes-unregulated.explicit", "keep-unregulated.explicit");
    oc.addSynonyme("keep-nodes-unregulated.explicit", "keep-unregulated.nodes", true);
    oc.addDescription("keep-nodes-unregulated.explicit", "Unregulated Nodes", TL("Do not regulate nodes in STR[]"));

    oc.doRegister("keep-nodes-unregulated.district-nodes", new Option_Bool(false));
    oc.addSynonyme("keep-nodes-unregulated.district-nodes", "keep-unregulated.district-nodes");
    oc.addDescription("keep-nodes-unregulated.district-nodes", "Unregulated Nodes", TL("Do not regulate district nodes"));

    // ramp guessing options
    if (!forNetgen) {
        oc.doRegister("ramps.guess", new Option_Bool(false));
        oc.addSynonyme("ramps.guess", "guess-ramps", true);
        oc.addDescription("ramps.guess", "Ramp Guessing", TL("Enable ramp-guessing"));

        oc.doRegister("ramps.guess-acceleration-lanes", new Option_Bool(true));
        oc.addDescription("ramps.guess-acceleration-lanes", "Ramp Guessing", TL("Guess on-ramps and mark acceleration lanes if they exist but do not add new lanes"));

        oc.doRegister("ramps.max-ramp-speed", new Option_Float(-1));
        oc.addSynonyme("ramps.max-ramp-speed", "ramp-guess.max-ramp-speed", true);
        oc.addDescription("ramps.max-ramp-speed", "Ramp Guessing", TL("Treat edges with speed > FLOAT as no ramps"));

        oc.doRegister("ramps.min-highway-speed", new Option_Float((double)(79 / 3.6)));
        oc.addSynonyme("ramps.min-highway-speed", "ramp-guess.min-highway-speed", true);
        oc.addDescription("ramps.min-highway-speed", "Ramp Guessing", TL("Treat edges with speed < FLOAT as no highways"));

        oc.doRegister("ramps.ramp-length", new Option_Float(100));
        oc.addSynonyme("ramps.ramp-length", "ramp-guess.ramp-length", true);
        oc.addDescription("ramps.ramp-length", "Ramp Guessing", TL("Use FLOAT as ramp-length"));

        //The Weaving Length Limit for Short Free Onramps, Chiu Liu, Zhongren WangPhD even suggest 70m
        oc.doRegister("ramps.min-weave-length", new Option_Float(50));
        oc.addDescription("ramps.min-weave-length", "Ramp Guessing", TL("Use FLOAT as minimum ramp-length"));

        oc.doRegister("ramps.set", new Option_StringVector());
        oc.addSynonyme("ramps.set", "ramp-guess.explicite", true);
        oc.addDescription("ramps.set", "Ramp Guessing", TL("Tries to handle the given edges as ramps"));

        oc.doRegister("ramps.unset", new Option_StringVector());
        oc.addDescription("ramps.unset", "Ramp Guessing", TL("Do not consider the given edges as ramps"));

        oc.doRegister("ramps.no-split", new Option_Bool(false));
        oc.addSynonyme("ramps.no-split", "ramp-guess.no-split", true);
        oc.addDescription("ramps.no-split", "Ramp Guessing", TL("Avoids edge splitting"));
    }
}


bool
NBFrame::checkOptions(OptionsCont& oc) {
    bool ok = true;
    //
    if (!SUMOXMLDefinitions::TrafficLightTypes.hasString(oc.getString("tls.default-type"))) {
        WRITE_ERRORF(TL("unsupported value '%' for option '--tls.default-type'"), oc.getString("tls.default-type"));
        ok = false;
    }
    if (oc.isSet("keep-edges.in-boundary") && oc.isSet("keep-edges.in-geo-boundary")) {
        WRITE_ERROR(TL("only one of the options 'keep-edges.in-boundary' or 'keep-edges.in-geo-boundary' may be given"));
        ok = false;
    }
    if (oc.getBool("no-internal-links") && oc.getBool("crossings.guess")) {
        WRITE_ERROR(TL("only one of the options 'no-internal-links' or 'crossings.guess' may be given"));
        ok = false;
    }
    if (oc.getBool("no-internal-links") && oc.getBool("walkingareas")) {
        WRITE_ERROR(TL("only one of the options 'no-internal-links' or 'walkareas' may be given"));
        ok = false;
    }
    if (!oc.isDefault("tls.green.time") && !oc.isDefault("tls.cycle.time")) {
        WRITE_ERROR(TL("only one of the options 'tls.green.time' or 'tls.cycle.time' may be given"));
        ok = false;
    }
    if (oc.getInt("default.lanenumber") < 1) {
        WRITE_ERROR(TL("default.lanenumber must be at least 1"));
        ok = false;
    }
    if (!oc.isDefault("default.lanewidth") && oc.getFloat("default.lanewidth") < POSITION_EPS) {
        WRITE_ERROR("default.lanewidth must be at least " + toString(POSITION_EPS));
        ok = false;
    }
    if (!oc.isDefault("default.disallow") && !oc.isDefault("default.allow")) {
        WRITE_ERROR(TL("only one of the options 'default.disallow' or 'default.allow' may be given"));
        ok = false;
    }
    if (oc.getInt("junctions.internal-link-detail") < 2) {
        WRITE_ERROR(TL("junctions.internal-link-detail must >= 2"));
        ok = false;
    }
    if (oc.getFloat("junctions.scurve-stretch") > 0) {
        if (oc.getBool("no-internal-links")) {
            WRITE_WARNING(TL("Option 'junctions.scurve-stretch' requires internal lanes to work. Option '--no-internal-links' will be disabled."));
        }
        // make sure the option is set so heuristics cannot ignore it
        oc.set("no-internal-links", "false");
    }
    if (oc.getFloat("junctions.small-radius") > oc.getFloat("default.junctions.radius") && oc.getFloat("default.junctions.radius") >= 0) {
        if (!oc.isDefault("junctions.small-radius")) {
            WRITE_WARNING(TL("option 'default.junctions.radius' is smaller than option 'junctions.small-radius'"));
        } else {
            oc.setDefault("junctions.small-radius", oc.getValueString("default.junctions.radius"));
        }
    }
    if (oc.getString("tls.layout") != "opposites"
            && oc.getString("tls.layout") != "incoming"
            && oc.getString("tls.layout") != "alternateOneWay") {
        WRITE_ERROR(TL("tls.layout must be 'opposites', 'incoming' or 'alternateOneWay'"));
        ok = false;
    }
    if (!oc.isDefault("default.right-of-way") &&
            !SUMOXMLDefinitions::RightOfWayValues.hasString(oc.getString("default.right-of-way"))) {
        WRITE_ERRORF(TL("default.right-of-way must be one of '%'"), toString(SUMOXMLDefinitions::RightOfWayValues.getStrings()));
        ok = false;
    }
    if (oc.getFloat("roundabouts.visibility-distance") < 0 && oc.getFloat("roundabouts.visibility-distance") != NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE) {
        WRITE_ERROR(TL("roundabouts.visibility-distance must be positive or -1"));
        ok = false;
    }
    if (oc.isDefault("railway.topology.repair") && oc.getBool("railway.topology.repair.connect-straight")) {
        oc.setDefault("railway.topology.repair", "true");
    }
    if (oc.isDefault("railway.topology.repair") && oc.getBool("railway.topology.repair.minimal")) {
        oc.setDefault("railway.topology.repair", "true");
    }
    if (oc.isDefault("railway.topology.all-bidi") && !oc.isDefault("railway.topology.all-bidi.input-file")) {
        oc.setDefault("railway.topology.all-bidi", "true");
    }
    if (oc.isDefault("railway.topology.repair.stop-turn") && !oc.isDefault("railway.topology.repair")) {
        oc.setDefault("railway.topology.repair.stop-turn", "true");
    }
    if (!SUMOXMLDefinitions::LaneSpreadFunctions.hasString(oc.getString("default.spreadtype"))) {
        WRITE_ERRORF(TL("Unknown value for default.spreadtype '%'."), oc.getString("default.spreadtype"));
        ok = false;
    }
    return ok;
}

/****************************************************************************/
