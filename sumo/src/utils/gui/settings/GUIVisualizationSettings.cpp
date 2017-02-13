/****************************************************************************/
/// @file    GUIVisualizationSettings.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Stores the information about how to visualize structures
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

#include <map>
#include <vector>
#include "GUIVisualizationSettings.h"
#include "GUIPropertyScheme.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
bool GUIVisualizationSettings::UseMesoSim = false;

/* -------------------------------------------------------------------------
 * color constants for link states
 * ----------------------------------------------------------------------- */
const RGBColor SUMO_color_TL_GREEN_MAJOR(0, 255, 0);
const RGBColor SUMO_color_TL_GREEN_MINOR(0, 179, 0);
const RGBColor SUMO_color_TL_RED(255, 0, 0);
const RGBColor SUMO_color_TL_REDYELLOW(255, 128, 0);
const RGBColor SUMO_color_TL_YELLOW_MAJOR(255, 255, 0);
const RGBColor SUMO_color_TL_YELLOW_MINOR(255, 255, 0);
const RGBColor SUMO_color_TL_OFF_BLINKING(128, 64, 0);
const RGBColor SUMO_color_TL_OFF_NOSIGNAL(0, 255, 255);
const RGBColor SUMO_color_MAJOR(255, 255, 255);
const RGBColor SUMO_color_MINOR(51, 51, 51);
const RGBColor SUMO_color_EQUAL(128, 128, 128);
const RGBColor SUMO_color_STOP(128, 0, 128);
const RGBColor SUMO_color_ALLWAY_STOP(0, 0, 192);
const RGBColor SUMO_color_ZIPPER(192, 128, 64);
const RGBColor SUMO_color_DEADEND(0, 0, 0);


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVisualizationSettings::GUIVisualizationSettings()
    : name(""), antialiase(false), dither(false),
      backgroundColor(RGBColor::WHITE),
      showGrid(false), gridXSize(100), gridYSize(100),
      laneShowBorders(false), showLinkDecals(true),
      showLinkRules(true), showRails(true),
      edgeName(false, 50, RGBColor(255, 128, 0, 255)),
      internalEdgeName(false, 40, RGBColor(128, 64, 0, 255)),
      cwaEdgeName(false, 50, RGBColor::MAGENTA),
      streetName(false, 55, RGBColor::YELLOW),
      hideConnectors(false),
      laneWidthExaggeration(1),
      laneMinSize(0),
      showLaneDirection(false),
      showSublanes(true),
      vehicleQuality(0), showBlinker(true),
      drawLaneChangePreference(false), drawMinGap(false),
      showBTRange(false), vehicleSize(1),
      vehicleName(false, 50, RGBColor(204, 153, 0, 255)),
      personQuality(0),
      personSize(1),
      personName(false, 50, RGBColor(0, 153, 204, 255)),
      containerQuality(0),
      containerSize(1),
      containerName(false, 50, RGBColor(0, 153, 204, 255)),
      drawLinkTLIndex(false, 50, RGBColor(128, 128, 255, 255)),
      drawLinkJunctionIndex(false, 50, RGBColor(128, 128, 255, 255)),
      junctionName(false, 50, RGBColor(0, 255, 128, 255)),
      internalJunctionName(false, 50, RGBColor(0, 204, 128, 255)),
      showLane2Lane(false),
      drawJunctionShape(true),
      drawCrossingsAndWalkingareas(true),
      junctionSize(1),
      addMode(0),
      addSize(1),
      addName(false, 50, RGBColor(255, 0, 128, 255)),
      poiSize(0), poiName(false, 50, RGBColor(255, 0, 128, 255)),
      poiType(false, 50, RGBColor(255, 0, 128, 255)),
      polySize(0), polyName(false, 50, RGBColor(255, 0, 128, 255)),
      polyType(false, 50, RGBColor(255, 0, 128, 255)),
      showSizeLegend(true),
      gaming(false),
      selectionScale(1),
      drawForSelecting(false) {
    /// add lane coloring schemes
    GUIColorScheme scheme = GUIColorScheme("uniform", RGBColor::BLACK, "road", true);
    scheme.addColor(RGBColor::GREY, 1, "sidewalk");
    scheme.addColor(RGBColor(192, 66, 44), 2, "bike lane");
    scheme.addColor(RGBColor(0, 0, 0, 0), 3, "green verge");
    scheme.addColor(RGBColor(150, 200, 200), 4, "waterway");
    scheme.addColor(RGBColor(92, 92, 92), 5, "no passenger"); // paths, service roads etc
    scheme.addColor(RGBColor::RED, 6, "closed"); // road closing
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection (lane-/streetwise)", RGBColor(128, 128, 128, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 80, 180, 255), 1, "selected");
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by permission code", RGBColor(240, 240, 240), "nobody");
    scheme.addColor(RGBColor(10, 10, 10), (SUMOReal)SVC_PASSENGER, "passenger");
    scheme.addColor(RGBColor(128, 128, 128), (SUMOReal)SVC_PEDESTRIAN, "pedestrian");
    scheme.addColor(RGBColor(80, 80, 80), (SUMOReal)(SVC_PEDESTRIAN | SVC_DELIVERY), "pedestrian_delivery");
    scheme.addColor(RGBColor(192, 66, 44), (SUMOReal)SVC_BICYCLE, "bicycle");
    scheme.addColor(RGBColor(40, 100, 40), (SUMOReal)SVC_BUS, "bus");
    scheme.addColor(RGBColor(166, 147, 26), (SUMOReal)SVC_TAXI, "taxi");
    scheme.addColor(RGBColor::BLACK, (SUMOReal)(SVCAll & ~SVC_NON_ROAD), "normal_road");
    scheme.addColor(RGBColor::BLACK, (SUMOReal)(SVCAll & ~(SVC_PEDESTRIAN | SVC_NON_ROAD)), "disallow_pedestrian");
    scheme.addColor(RGBColor(255, 206, 0), (SUMOReal)(SVCAll & ~(SVC_PEDESTRIAN | SVC_BICYCLE | SVC_MOPED | SVC_NON_ROAD)), "motorway");
    scheme.addColor(RGBColor(150, 200, 200), (SUMOReal)SVC_SHIP, "waterway");
    scheme.addColor(RGBColor::GREEN, (SUMOReal)SVCAll, "all");
    laneColorer.addScheme(scheme);
    // ... traffic states ...
    scheme = GUIColorScheme("by allowed speed (lanewise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (lanewise, brutto)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.95);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (lanewise, netto)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.95);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by first vehicle waiting time (lanewise)", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)30);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)200);
    scheme.addColor(RGBColor::RED, (SUMOReal)300);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by lane number (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)5);
    laneColorer.addScheme(scheme);
    // ... emissions ...
    scheme = GUIColorScheme("by CO2 emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(10. / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by CO emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(0.05 / 7.5 / 2.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by PMx emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(.005 / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by NOx emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(.125 / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by HC emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(.02 / 7.5 / 4.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by fuel consumption", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(.005 / 7.5 * 100.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by noise emissions (Harmonoise)", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)100);
    laneColorer.addScheme(scheme);
    // ... weights (experimental) ...
    scheme = GUIColorScheme("by global travel time", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)100);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by global speed percentage", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)50);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by given length/geometrical length", RGBColor::BLACK);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.25);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)0.5);
    scheme.addColor(RGBColor(179, 179, 179, 255), (SUMOReal)1.0);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)2.0);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)4.0);
    laneColorer.addScheme(scheme);
    laneColorer.addScheme(GUIColorScheme("by angle", RGBColor::YELLOW, "", true));
    scheme = GUIColorScheme("by loaded weight", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)100);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by priority", RGBColor::YELLOW);
    scheme.addColor(RGBColor::RED, (SUMOReal) - 20);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)20);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by height at start", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - 10);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)50);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)200);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by height at geometry-segment start", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - 10);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)50);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)200);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by inclination", RGBColor::GREY);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal) .1);
    scheme.addColor(RGBColor::RED, (SUMOReal) .3);
    scheme.addColor(RGBColor::GREEN, (SUMOReal) - .1);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - .3);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by geometry-segment inclination", RGBColor::GREY);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal) .1);
    scheme.addColor(RGBColor::RED, (SUMOReal) .3);
    scheme.addColor(RGBColor::GREEN, (SUMOReal) - .1);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - .3);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by average speed", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by average relative speed ", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(0.25));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(0.5));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(0.75));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(1));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(1.25));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by routing device assumed speed ", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by electricity consumption", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)(1 / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by insertion-backlog (streetwise)", RGBColor::GREY);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)1);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)10);
    scheme.addColor(RGBColor::RED, (SUMOReal)100);
    laneColorer.addScheme(scheme);


    /// add vehicle coloring schemes
    vehicleColorer.addScheme(GUIColorScheme("given vehicle/type/route color", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("uniform", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("given/assigned vehicle color", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("given/assigned type color", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("given/assigned route color", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("depart position as HSV", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("arrival position as HSV", RGBColor::YELLOW, "", true));
    vehicleColorer.addScheme(GUIColorScheme("direction/distance as HSV", RGBColor::YELLOW, "", true));
    scheme = GUIColorScheme("by speed", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by waiting time", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)30);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)200);
    scheme.addColor(RGBColor::RED, (SUMOReal)300);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by accumulated waiting time", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)25);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)50);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)75);
    scheme.addColor(RGBColor::RED, (SUMOReal)100);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by time since lane change", RGBColor(179, 179, 179, 255), "0");
    scheme.addColor(RGBColor(189, 189, 179, 255), -180);
    scheme.addColor(RGBColor(255, 255, 0, 255), -20);
    scheme.addColor(RGBColor(255,   0, 0, 255), -0.001);
    scheme.addColor(RGBColor(0,   0, 255, 255),  0.001);
    scheme.addColor(RGBColor(0, 255, 255, 255),  20);
    scheme.addColor(RGBColor(179, 189, 189, 255),  180);
    scheme.setAllowsNegativeValues(true);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by max speed", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    vehicleColorer.addScheme(scheme);
    // ... emissions ...
    scheme = GUIColorScheme("by CO2 emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)5.);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by CO emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.05);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by PMx emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal).005);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by NOx emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal).125);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by HC emissions", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal).02);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by fuel consumption", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal).005);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by noise emissions (Harmonoise)", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)100.);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by reroute number", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)1.);
    scheme.addColor(RGBColor::WHITE, (SUMOReal)10.);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection", RGBColor(179, 179, 179, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 102, 204, 255), 1, "selected");
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by offset from best lane", RGBColor(179, 179, 179, 255), "0");
    scheme.addColor(RGBColor(255,   0, 0, 255), -3, "-3");
    scheme.addColor(RGBColor(255, 255, 0, 255), -1, "-1");
    scheme.addColor(RGBColor(0, 255, 255, 255),  1,  "1");
    scheme.addColor(RGBColor(0,   0, 255, 255),  3,  "3");
    scheme.setAllowsNegativeValues(true);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by acceleration", RGBColor(179, 179, 179, 255), "0");
    scheme.addColor(RGBColor(255,   0, 0, 255), -4.5 /* -SUMOVTypeParameter::getDefaultDecel() */);
    scheme.addColor(RGBColor(255, 255, 0, 255), -0.1);
    scheme.addColor(RGBColor(0, 255, 255, 255),  0.1);
    scheme.addColor(RGBColor(0,   0, 255, 255),  2.6 /* SUMOVTypeParameter::getDefaultAccel() */);
    scheme.setAllowsNegativeValues(true);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by time gap on lane", RGBColor(255, 255, 0, 255), "0");
    scheme.addColor(RGBColor(179, 179, 179, 255), -1);
    scheme.addColor(RGBColor(0, 255, 255, 255), 1);
    scheme.addColor(RGBColor(0,   0, 255, 255), 2);
    scheme.setAllowsNegativeValues(true);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by depart delay", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)30);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)200);
    scheme.addColor(RGBColor::RED, (SUMOReal)300);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by electricity consumption", RGBColor::GREEN);
    scheme.addColor(RGBColor::RED, (SUMOReal)5);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by time loss", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)10);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)60);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)180);
    scheme.addColor(RGBColor::RED, (SUMOReal)900);
    vehicleColorer.addScheme(scheme);
    vehicleColorer.addScheme(GUIColorScheme("random", RGBColor::YELLOW, "", true));

    /// add person coloring schemes
    personColorer.addScheme(GUIColorScheme("given person/type color", RGBColor::YELLOW, "", true));
    personColorer.addScheme(GUIColorScheme("uniform", RGBColor::YELLOW, "", true));
    personColorer.addScheme(GUIColorScheme("given/assigned person color", RGBColor::YELLOW, "", true));
    personColorer.addScheme(GUIColorScheme("given/assigned type color", RGBColor::YELLOW, "", true));
    scheme = GUIColorScheme("by speed", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(2.5 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(5 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(10 / 3.6));
    personColorer.addScheme(scheme);
    scheme = GUIColorScheme("by mode", RGBColor::YELLOW); // walking
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(1)); // riding
    scheme.addColor(RGBColor::RED, (SUMOReal)(2)); // stopped
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(3)); // waiting for ride
    personColorer.addScheme(scheme);
    scheme = GUIColorScheme("by waiting time", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)30);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)200);
    scheme.addColor(RGBColor::RED, (SUMOReal)300);
    personColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection", RGBColor(179, 179, 179, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 102, 204, 255), 1, "selected");
    personColorer.addScheme(scheme);
    personColorer.addScheme(GUIColorScheme("by angle", RGBColor::YELLOW, "", true));
    personColorer.addScheme(GUIColorScheme("random", RGBColor::YELLOW, "", true));

    /// add container coloring schemes
    containerColorer.addScheme(GUIColorScheme("given container/type color", RGBColor::YELLOW, "", true));
    containerColorer.addScheme(GUIColorScheme("uniform", RGBColor::YELLOW, "", true));
    containerColorer.addScheme(GUIColorScheme("given/assigned container color", RGBColor::YELLOW, "", true));
    containerColorer.addScheme(GUIColorScheme("given/assigned type color", RGBColor::YELLOW, "", true));
    scheme = GUIColorScheme("by speed", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(2.5 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(5 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(10 / 3.6));
    containerColorer.addScheme(scheme);
    scheme = GUIColorScheme("by mode", RGBColor::YELLOW); // walking
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(1)); // riding
    scheme.addColor(RGBColor::RED, (SUMOReal)(2)); // stopped
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(3)); // waiting for ride
    containerColorer.addScheme(scheme);
    scheme = GUIColorScheme("by waiting time", RGBColor::BLUE);
    scheme.addColor(RGBColor::CYAN, (SUMOReal)30);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)200);
    scheme.addColor(RGBColor::RED, (SUMOReal)300);
    containerColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection", RGBColor(179, 179, 179, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 102, 204, 255), 1, "selected");
    containerColorer.addScheme(scheme);
    containerColorer.addScheme(GUIColorScheme("by angle", RGBColor::YELLOW, "", true));

    /// add junction coloring schemes
    scheme = GUIColorScheme("uniform", RGBColor::BLACK, "", true);
    scheme.addColor(RGBColor(150, 200, 200), 1, "waterway");
    junctionColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection", RGBColor(128, 128, 128, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 80, 180, 255), 1, "selected");
    junctionColorer.addScheme(scheme);
    scheme = GUIColorScheme("by type", RGBColor::GREEN, "traffic_light", true);
    scheme.addColor(RGBColor(0, 128, 0), 1, "traffic_light_unregulated");
    scheme.addColor(RGBColor::YELLOW, 2, "priority");
    scheme.addColor(RGBColor::RED, 3, "priority_stop");
    scheme.addColor(RGBColor::BLUE, 4, "right_before_left");
    scheme.addColor(RGBColor::CYAN, 5, "allway_stop");
    scheme.addColor(RGBColor::GREY, 6, "district");
    scheme.addColor(RGBColor::MAGENTA, 7, "unregulated");
    scheme.addColor(RGBColor::BLACK, 8, "dead_end");
    scheme.addColor(RGBColor::ORANGE, 9, "rail_signal");
    scheme.addColor(RGBColor(192, 128, 64), 10, "zipper"); // light brown
    scheme.addColor(RGBColor(192, 255, 192), 11, "traffic_light_right_on_red"); // light green
    scheme.addColor(RGBColor(128, 0, 128), 12, "rail_crossing"); // dark purple
    junctionColorer.addScheme(scheme);


    /// add lane scaling schemes
    {
        GUIScaleScheme scheme = GUIScaleScheme("default", 1, "uniform", true);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by selection (lane-/streetwise)", 0.5, "unselected", true);
        scheme.addColor(5, 1, "selected");
        laneScaler.addScheme(scheme);
        // ... traffic states ...
        scheme = GUIScaleScheme("by allowed speed (lanewise)", 0);
        scheme.addColor(10, (SUMOReal)(150.0 / 3.6));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by current occupancy (lanewise, brutto)", 0);
        scheme.addColor(10, (SUMOReal)0.95);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by current occupancy (lanewise, netto)", 0);
        scheme.addColor(10, (SUMOReal)0.95);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by first vehicle waiting time (lanewise)", 0);
        scheme.addColor(10, (SUMOReal)300);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by lane number (streetwise)", 1);
        scheme.addColor(10, (SUMOReal)5);
        laneScaler.addScheme(scheme);
        // ... emissions ...
        scheme = GUIScaleScheme("by CO2 emissions", 0);
        scheme.addColor(10, (SUMOReal)(10. / 7.5 / 5.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by CO emissions", 0);
        scheme.addColor(10, (SUMOReal)(0.05 / 7.5 / 2.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by PMx emissions", 0);
        scheme.addColor(10, (SUMOReal)(.005 / 7.5 / 5.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by NOx emissions", 0);
        scheme.addColor(10, (SUMOReal)(.125 / 7.5 / 5.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by HC emissions", 0);
        scheme.addColor(10, (SUMOReal)(.02 / 7.5 / 4.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by fuel consumption", 0);
        scheme.addColor(10, (SUMOReal)(.005 / 7.5 * 100.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by noise emissions (Harmonoise)", 0);
        scheme.addColor(10, (SUMOReal)100);
        laneScaler.addScheme(scheme);
        // ... weights (experimental) ...
        scheme = GUIScaleScheme("by global travel time", 0);
        scheme.addColor(10, (SUMOReal)100);
        scheme.setAllowsNegativeValues(true);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by global speed percentage", 0);
        scheme.addColor(10, (SUMOReal)100);
        scheme.setAllowsNegativeValues(true);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by given length/geometrical length", 0);
        scheme.addColor(10, (SUMOReal)10.0);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by loaded weight", 0);
        scheme.addColor(-1000, (SUMOReal) - 1000);
        scheme.addColor(1000, (SUMOReal)1000);
        scheme.setAllowsNegativeValues(true);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by priority", 1);
        scheme.addColor(0.5, (SUMOReal) - 20);
        scheme.addColor(5, (SUMOReal)20);
        scheme.setAllowsNegativeValues(true);
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by average speed", 0);
        scheme.addColor(10, (SUMOReal)(150.0 / 3.6));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by average relative speed", 0);
        scheme.addColor(0.5, (SUMOReal)(0.5));
        scheme.addColor(2, (SUMOReal)(1));
        scheme.addColor(10, (SUMOReal)(2));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by electricity consumption", 0);
        scheme.addColor(10, (SUMOReal)(1 / 7.5 / 5.));
        laneScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by insertion-backlog (streetwise)", 0);
        scheme.addColor(1, (SUMOReal)1);
        scheme.addColor(10, (SUMOReal)10);
        scheme.addColor(50, (SUMOReal)100);
        laneScaler.addScheme(scheme);
    }


    /// add edge coloring schemes
    edgeColorer.addScheme(GUIColorScheme("uniform (streetwise)", RGBColor::BLACK, "", true));
    scheme = GUIColorScheme("by selection (streetwise)", RGBColor(128, 128, 128, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 80, 180, 255), 1, "selected");
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by purpose (streetwise)", RGBColor(), "normal", true);
    scheme.addColor(RGBColor(128, 0, 128, 255), 1, "connector"); // see MSEdge::EdgeBasicFunction::EDGEFUNCTION_CONNECTOR
    scheme.addColor(RGBColor::BLUE, 2, "internal"); // see MSEdge::EdgeBasicFunction::EDGEFUNCTION_INTERNAL
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by allowed speed (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (streetwise, brutto)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.95);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current speed (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current flow (streetwise)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)5000);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by relative speed (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(0.25));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(0.5));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(0.75));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(1));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(1.25));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by routing device assumed speed", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    edgeColorer.addScheme(scheme);
    edgeColorer.addScheme(GUIColorScheme("by angle", RGBColor::YELLOW, "", true));
    scheme = GUIColorScheme("by segments (alternating)", RGBColor::BLUE, "odd", true);
    scheme.addColor(RGBColor::RED, 1, "even");
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by jammed state (segmentwise)", RGBColor::GREEN, "free", true);
    scheme.addColor(RGBColor::RED, 1, "jammed");
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (segmentwise, brutto)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.95);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current speed (segmentwise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current flow (segmentwise)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)5000);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by relative speed (segmentwise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(0.25));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(0.5));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(0.75));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(1));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(1.25));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by insertion-backlog (streetwise)", RGBColor::GREY);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)1);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)10);
    scheme.addColor(RGBColor::RED, (SUMOReal)100);
    edgeColorer.addScheme(scheme);


    /// add edge scaling schemes
    {
        edgeScaler.addScheme(GUIScaleScheme("uniform", 1, "", true));
        GUIScaleScheme scheme = GUIScaleScheme("by selection (streetwise)", 0.5, "unselected", true);
        scheme.addColor(5, 1, "selected");
        edgeScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by allowed speed (streetwise)", 0);
        scheme.addColor(10, (SUMOReal)(150.0 / 3.6));
        edgeScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by current occupancy (streetwise, brutto)", 0);
        scheme.addColor(10, (SUMOReal)0.95);
        edgeScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by current speed (streetwise)", 0);
        scheme.addColor(10, (SUMOReal)(150.0 / 3.6));
        edgeScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by current flow (streetwise)", 0);
        scheme.addColor(20, (SUMOReal)5000);
        edgeScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by relative speed (streetwise)", 0);
        scheme.addColor(20, (SUMOReal)1);
        edgeScaler.addScheme(scheme);
        scheme = GUIScaleScheme("by insertion-backlog (streetwise)", 0);
        scheme.addColor(1, (SUMOReal)1);
        scheme.addColor(10, (SUMOReal)10);
        scheme.addColor(50, (SUMOReal)100);
        edgeScaler.addScheme(scheme);
    }

}


int
GUIVisualizationSettings::getLaneEdgeMode() const {
    if (UseMesoSim) {
        return edgeColorer.getActive();
    }
    return laneColorer.getActive();
}


int
GUIVisualizationSettings::getLaneEdgeScaleMode() const {
    if (UseMesoSim) {
        return edgeScaler.getActive();
    }
    return laneScaler.getActive();
}


GUIColorScheme&
GUIVisualizationSettings::getLaneEdgeScheme() {
    if (UseMesoSim) {
        return edgeColorer.getScheme();
    }
    return laneColorer.getScheme();
}


GUIScaleScheme&
GUIVisualizationSettings::getLaneEdgeScaleScheme() {
    if (UseMesoSim) {
        return edgeScaler.getScheme();
    }
    return laneScaler.getScheme();
}

void
GUIVisualizationSettings::save(OutputDevice& dev) const {
    dev.openTag(SUMO_TAG_VIEWSETTINGS_SCHEME);
    dev.writeAttr(SUMO_ATTR_NAME, name);
    dev.openTag(SUMO_TAG_VIEWSETTINGS_OPENGL);
    dev.writeAttr("antialiase", antialiase);
    dev.writeAttr("dither", dither);
    dev.closeTag();
    dev.openTag(SUMO_TAG_VIEWSETTINGS_BACKGROUND);
    dev.writeAttr("backgroundColor", backgroundColor);
    dev.writeAttr("showGrid", showGrid);
    dev.writeAttr("gridXSize", gridXSize);
    dev.writeAttr("gridYSize", gridYSize);
    dev.closeTag();
    // edges
    dev.openTag(SUMO_TAG_VIEWSETTINGS_EDGES);
    dev.writeAttr("laneEdgeMode", getLaneEdgeMode());
    dev.writeAttr("scaleMode", getLaneEdgeScaleMode());
    dev.writeAttr("laneShowBorders", laneShowBorders);
    dev.writeAttr("showLinkDecals", showLinkDecals);
    dev.writeAttr("showLinkRules", showLinkRules);
    dev.writeAttr("showRails", showRails);
    dev.writeAttr("hideConnectors", hideConnectors);
    dev.writeAttr("widthExaggeration", laneWidthExaggeration);
    dev.writeAttr("minSize", laneMinSize);
    dev.writeAttr("showDirection", showLaneDirection);
    dev.writeAttr("showSublanes", showSublanes);
    dev.lf();
    dev << "               ";
    edgeName.print(dev, "edgeName");
    dev.lf();
    dev << "               ";
    internalEdgeName.print(dev, "internalEdgeName");
    dev.lf();
    dev << "               ";
    cwaEdgeName.print(dev, "cwaEdgeName");
    dev.lf();
    dev << "               ";
    streetName.print(dev, "streetName");
    laneColorer.save(dev);
    laneScaler.save(dev);
    edgeColorer.save(dev);
    edgeScaler.save(dev);
    dev.closeTag();
    // vehicles
    dev.openTag(SUMO_TAG_VIEWSETTINGS_VEHICLES);
    dev.writeAttr("vehicleMode", vehicleColorer.getActive());
    dev.writeAttr("vehicleQuality", vehicleQuality);
    vehicleSize.print(dev, "vehicle");
    dev.writeAttr("showBlinker", showBlinker);
    dev.lf();
    dev << "                 ";
    vehicleName.print(dev, "vehicleName");
    vehicleColorer.save(dev);
    dev.closeTag();
    // persons
    dev.openTag(SUMO_TAG_VIEWSETTINGS_PERSONS);
    dev.writeAttr("personMode", personColorer.getActive());
    dev.writeAttr("personQuality", personQuality);
    personSize.print(dev, "person");
    dev.lf();
    dev << "                ";
    personName.print(dev, "personName");
    personColorer.save(dev);
    dev.closeTag();
    // persons
    dev.openTag(SUMO_TAG_VIEWSETTINGS_CONTAINERS);
    dev.writeAttr("containerMode", containerColorer.getActive());
    dev.writeAttr("containerQuality", containerQuality);
    personSize.print(dev, "container");
    dev.lf();
    dev << "                ";
    personName.print(dev, "containerName");
    personColorer.save(dev);
    dev.closeTag();
    // junctions
    dev.openTag(SUMO_TAG_VIEWSETTINGS_JUNCTIONS);
    dev.writeAttr("junctionMode", junctionColorer.getActive());
    dev.lf();
    dev << "                  ";
    drawLinkTLIndex.print(dev, "drawLinkTLIndex");
    dev.lf();
    dev << "                  ";
    drawLinkJunctionIndex.print(dev, "drawLinkJunctionIndex");
    dev.lf();
    dev << "                  ";
    junctionName.print(dev, "junctionName");
    dev.lf();
    dev << "                  ";
    internalJunctionName.print(dev, "internalJunctionName");
    dev.lf();
    dev << "                  ";
    dev.writeAttr("showLane2Lane", showLane2Lane);
    dev.writeAttr("drawShape", drawJunctionShape);
    dev.writeAttr("drawCrossingsAndWalkingareas", drawCrossingsAndWalkingareas);
    junctionSize.print(dev, "junction");
    junctionColorer.save(dev);
    dev.closeTag();
    // additionals
    dev.openTag(SUMO_TAG_VIEWSETTINGS_ADDITIONALS);
    dev.writeAttr("addMode", addMode);
    addSize.print(dev, "add");
    addName.print(dev, "addName");
    dev.closeTag();
    // pois
    dev.openTag(SUMO_TAG_VIEWSETTINGS_POIS);
    poiSize.print(dev, "poi");
    poiName.print(dev, "poiName");
    poiType.print(dev, "poiType");
    dev.closeTag();
    // polys
    dev.openTag(SUMO_TAG_VIEWSETTINGS_POLYS);
    polySize.print(dev, "poly");
    polyName.print(dev, "polyName");
    polyType.print(dev, "polyType");
    dev.closeTag();
    // legend
    dev.openTag(SUMO_TAG_VIEWSETTINGS_LEGEND);
    dev.writeAttr("showSizeLegend", showSizeLegend);
    dev.closeTag();

    dev.closeTag();
}


bool
GUIVisualizationSettings::operator==(const GUIVisualizationSettings& v2) {
    if (antialiase != v2.antialiase) {
        return false;
    }
    if (dither != v2.dither) {
        return false;
    }
    if (backgroundColor != v2.backgroundColor) {
        return false;
    }
    if (showGrid != v2.showGrid) {
        return false;
    }
    if (gridXSize != v2.gridXSize) {
        return false;
    }
    if (gridYSize != v2.gridYSize) {
        return false;
    }

    if (!(edgeColorer == v2.edgeColorer)) {
        return false;
    }
    if (!(edgeScaler == v2.edgeScaler)) {
        return false;
    }
    if (!(laneColorer == v2.laneColorer)) {
        return false;
    }
    if (!(laneScaler == v2.laneScaler)) {
        return false;
    }
    if (laneShowBorders != v2.laneShowBorders) {
        return false;
    }
    if (showLinkDecals != v2.showLinkDecals) {
        return false;
    }
    if (showLinkRules != v2.showLinkRules) {
        return false;
    }
    if (showRails != v2.showRails) {
        return false;
    }
    if (edgeName != v2.edgeName) {
        return false;
    }
    if (internalEdgeName != v2.internalEdgeName) {
        return false;
    }
    if (cwaEdgeName != v2.cwaEdgeName) {
        return false;
    }
    if (streetName != v2.streetName) {
        return false;
    }
    if (hideConnectors != v2.hideConnectors) {
        return false;
    }
    if (laneWidthExaggeration != v2.laneWidthExaggeration) {
        return false;
    }
    if (laneMinSize != v2.laneMinSize) {
        return false;
    }
    if (showLaneDirection != v2.showLaneDirection) {
        return false;
    }
    if (showSublanes != v2.showSublanes) {
        return false;
    }
    if (!(vehicleColorer == v2.vehicleColorer)) {
        return false;
    }
    if (vehicleQuality != v2.vehicleQuality) {
        return false;
    }
    if (vehicleSize != v2.vehicleSize) {
        return false;
    }
    if (showBlinker != v2.showBlinker) {
        return false;
    }
    if (drawLaneChangePreference != v2.drawLaneChangePreference) {
        return false;
    }
    if (drawMinGap != v2.drawMinGap) {
        return false;
    }
    if (showBTRange != v2.showBTRange) {
        return false;
    }
    if (vehicleName != v2.vehicleName) {
        return false;
    }
    if (!(personColorer == v2.personColorer)) {
        return false;
    }
    if (personQuality != v2.personQuality) {
        return false;
    }
    if (personSize != v2.personSize) {
        return false;
    }
    if (personName != v2.personName) {
        return false;
    }
    if (!(containerColorer == v2.containerColorer)) {
        return false;
    }
    if (containerQuality != v2.containerQuality) {
        return false;
    }
    if (containerSize != v2.containerSize) {
        return false;
    }
    if (containerName != v2.containerName) {
        return false;
    }
    if (!(junctionColorer == v2.junctionColorer)) {
        return false;
    }
    if (drawLinkTLIndex != v2.drawLinkTLIndex) {
        return false;
    }
    if (drawLinkJunctionIndex != v2.drawLinkJunctionIndex) {
        return false;
    }
    if (junctionName != v2.junctionName) {
        return false;
    }
    if (internalJunctionName != v2.internalJunctionName) {
        return false;
    }

    if (showLane2Lane != v2.showLane2Lane) {
        return false;
    }

    if (drawJunctionShape != v2.drawJunctionShape) {
        return false;
    }

    if (drawCrossingsAndWalkingareas != v2.drawCrossingsAndWalkingareas) {
        return false;
    }
    if (junctionSize != v2.junctionSize) {
        return false;
    }

    if (addMode != v2.addMode) {
        return false;
    }
    if (addSize != v2.addSize) {
        return false;
    }
    if (addName != v2.addName) {
        return false;
    }
    if (poiSize != v2.poiSize) {
        return false;
    }
    if (poiName != v2.poiName) {
        return false;
    }
    if (poiType != v2.poiType) {
        return false;
    }
    if (polySize != v2.polySize) {
        return false;
    }
    if (polyName != v2.polyName) {
        return false;
    }
    if (polyType != v2.polyType) {
        return false;
    }

    if (showSizeLegend != v2.showSizeLegend) {
        return false;
    }

    return true;
}


SUMOReal
GUIVisualizationSizeSettings::getExaggeration(const GUIVisualizationSettings& s, SUMOReal factor) const {
    /// @note should look normal-sized at zoom 1000
    return (constantSize && !s.drawForSelecting) ? MAX2((SUMOReal)exaggeration, exaggeration * factor / s.scale) : exaggeration;
}


const RGBColor&
GUIVisualizationSettings::getLinkColor(const LinkState& ls) {
    switch (ls) {
        case LINKSTATE_TL_GREEN_MAJOR:
            return SUMO_color_TL_GREEN_MAJOR;
        case LINKSTATE_TL_GREEN_MINOR:
            return SUMO_color_TL_GREEN_MINOR;
        case LINKSTATE_TL_RED:
            return SUMO_color_TL_RED;
        case LINKSTATE_TL_REDYELLOW:
            return SUMO_color_TL_REDYELLOW;
        case LINKSTATE_TL_YELLOW_MAJOR:
            return SUMO_color_TL_YELLOW_MAJOR;
        case LINKSTATE_TL_YELLOW_MINOR:
            return SUMO_color_TL_YELLOW_MINOR;
        case LINKSTATE_TL_OFF_BLINKING:
            return SUMO_color_TL_OFF_BLINKING;
        case LINKSTATE_TL_OFF_NOSIGNAL:
            return SUMO_color_TL_OFF_NOSIGNAL;
        case LINKSTATE_MAJOR:
            return SUMO_color_MAJOR;
        case LINKSTATE_MINOR:
            return SUMO_color_MINOR;
        case LINKSTATE_EQUAL:
            return SUMO_color_EQUAL;
        case LINKSTATE_STOP:
            return SUMO_color_STOP;
        case LINKSTATE_ALLWAY_STOP:
            return SUMO_color_ALLWAY_STOP;
        case LINKSTATE_ZIPPER:
            return SUMO_color_ZIPPER;
        case LINKSTATE_DEADEND:
            return SUMO_color_DEADEND;
        default:
            throw ProcessError("No color defined for LinkState '" + std::string(ls, 1) + "'");
    }
}

/****************************************************************************/


