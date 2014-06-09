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

#include <map>
#include <vector>
#include <utils/common/SUMOVTypeParameter.h>
#include "GUIVisualizationSettings.h"
#include "GUIColorScheme.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
#ifdef HAVE_INTERNAL
bool GUIVisualizationSettings::UseMesoSim = false;
#endif

// ===========================================================================
// member method definitions
// ===========================================================================
GUIVisualizationSettings::GUIVisualizationSettings()
    : name(""), antialiase(false), dither(false),
      backgroundColor(RGBColor::WHITE),
      showGrid(false), gridXSize(100), gridYSize(100),
      laneShowBorders(false), showLinkDecals(true), showRails(true),
      edgeName(false, 50, RGBColor(255, 128, 0, 255)),
      internalEdgeName(false, 40, RGBColor(128, 64, 0, 255)),
      cwaEdgeName(false, 50, RGBColor::MAGENTA),
      streetName(false, 55, RGBColor::YELLOW),
      hideConnectors(false), laneWidthExaggeration(1),
      vehicleQuality(0), minVehicleSize(1), vehicleExaggeration(1), showBlinker(true),
      drawLaneChangePreference(false), drawMinGap(false),
      vehicleName(false, 50, RGBColor(204, 153, 0, 255)),
      personQuality(0), minPersonSize(1), personExaggeration(1),
      personName(false, 50, RGBColor(0, 153, 204, 255)),
      drawLinkTLIndex(false), drawLinkJunctionIndex(false),
      junctionName(false, 50, RGBColor(0, 255, 128, 255)),
      internalJunctionName(false, 50, RGBColor(0, 204, 128, 255)),
      showLane2Lane(false), drawJunctionShape(true), addMode(0), minAddSize(1), addExaggeration(1),
      addName(false, 50, RGBColor(255, 0, 128, 255)),
      minPOISize(0), poiExaggeration(1), poiName(false, 50, RGBColor(255, 0, 128, 255)),
      minPolySize(0), polyExaggeration(1), polyName(false, 50, RGBColor(255, 0, 128, 255)),
      showSizeLegend(true),
      gaming(false),
      selectionScale(1) {
    /// add lane coloring schemes
    GUIColorScheme scheme = GUIColorScheme("uniform", RGBColor::BLACK, "road", true);
    scheme.addColor(RGBColor::GREY, 1, "sidewalk");
    scheme.addColor(RGBColor(192, 66, 44), 2, "bike lane");
    scheme.addColor(RGBColor(0, 0, 0, 0), 3, "green verge");
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection (lane-/streetwise)", RGBColor(179, 179, 179, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 102, 204, 255), 1, "selected");
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by permission code", RGBColor(240, 240, 240), "nobody");
    scheme.addColor(RGBColor(10, 10, 10), (SUMOReal)SVC_PASSENGER, "passenger");
    scheme.addColor(RGBColor(128, 128, 128), (SUMOReal)SVC_PEDESTRIAN, "pedestrian");
    scheme.addColor(RGBColor(80, 80, 80), (SUMOReal)(SVC_PEDESTRIAN | SVC_DELIVERY), "pedestrian_delivery");
    scheme.addColor(RGBColor(192, 66, 44), (SUMOReal)SVC_BICYCLE, "bicycle");
    scheme.addColor(RGBColor(40, 100, 40), (SUMOReal)SVC_BUS, "bus");
    scheme.addColor(RGBColor(166, 147, 26), (SUMOReal)SVC_TAXI, "taxi");
    scheme.addColor(RGBColor::BLACK, (SUMOReal)(SVCAll & ~SVC_PEDESTRIAN), "disallow_pedestrian");
    scheme.addColor(RGBColor(64, 0, 86), (SUMOReal)(SVCAll & ~(SVC_PEDESTRIAN | SVC_BICYCLE | SVC_MOPED)), "disallow_apedestrian_bicycle");
    scheme.addColor(RGBColor::GREEN, (SUMOReal)SVCAll, "all");
    laneColorer.addScheme(scheme);
    // ... traffic states ...
    scheme = GUIColorScheme("by allowed speed (lanewise)", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(150.0 / 3.6));
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
    scheme = GUIColorScheme("by time since last lanechange", RGBColor::WHITE);
    scheme.addColor(RGBColor::GREY, (SUMOReal)(5 * 60));
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
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by acceleration", RGBColor(179, 179, 179, 255), "0");
    scheme.addColor(RGBColor(255,   0, 0, 255), -SUMOVTypeParameter::getDefaultDecel());
    scheme.addColor(RGBColor(255, 255, 0, 255), -0.1);
    scheme.addColor(RGBColor(0, 255, 255, 255),  0.1);
    scheme.addColor(RGBColor(0,   0, 255, 255),  SUMOVTypeParameter::getDefaultAccel());
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by time gap", RGBColor(255, 255, 0, 255), "0");
    scheme.addColor(RGBColor(179, 179, 179, 255), -1);
    scheme.addColor(RGBColor(0, 255, 255, 255), 1);
    scheme.addColor(RGBColor(0,   0, 255, 255), 2);
    vehicleColorer.addScheme(scheme);

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


#ifdef HAVE_INTERNAL
    /// add edge coloring schemes
    edgeColorer.addScheme(GUIColorScheme("uniform (streetwise)", RGBColor::BLACK, "", true));
    scheme = GUIColorScheme("by selection (streetwise)", RGBColor(179, 179, 179, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 102, 204, 255), 1, "selected");
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by purpose (streetwise)", RGBColor(), "normal", true);
    scheme.addColor(RGBColor(128, 0, 128, 255), 1, "connector"); // see MSEdge::EdgeBasicFunction::EDGEFUNCTION_CONNECTOR
    scheme.addColor(RGBColor::BLUE, 2, "internal"); // see MSEdge::EdgeBasicFunction::EDGEFUNCTION_INTERNAL
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by allowed speed (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(150.0 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (streetwise)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.95);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current speed (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(150.0 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current flow (streetwise)", RGBColor::BLUE);
    scheme.addColor(RGBColor::RED, (SUMOReal)5000);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by relative speed (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)1);
    edgeColorer.addScheme(scheme);
#endif

    junctionColorer.addScheme(GUIColorScheme("uniform", RGBColor::BLACK, "", true));
    scheme = GUIColorScheme("by selection", RGBColor(179, 179, 179, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 102, 204, 255), 1, "selected");
    junctionColorer.addScheme(scheme);
}


size_t
GUIVisualizationSettings::getLaneEdgeMode() const {
#ifdef HAVE_INTERNAL
    if (UseMesoSim) {
        return edgeColorer.getActive();
    }
#endif
    return laneColorer.getActive();
}


GUIColorScheme&
GUIVisualizationSettings::getLaneEdgeScheme() {
#ifdef HAVE_INTERNAL
    if (UseMesoSim) {
        return edgeColorer.getScheme();
    }
#endif
    return laneColorer.getScheme();
}


void
GUIVisualizationSettings::save(OutputDevice& dev) const {
    dev << "<viewsettings>\n";
    dev << "    <scheme name=\"" << name << "\">\n";
    dev << "        <opengl antialiase=\"" << antialiase << "\" dither=\"" << dither << "\"/>\n";
    dev << "        <background backgroundColor=\"" << backgroundColor << "\"\n"
        << "                    showGrid=\"" << showGrid
        << "\" gridXSize=\"" << gridXSize << "\" gridYSize=\"" << gridYSize << "\"/>\n";
    dev << "        <edges laneEdgeMode=\"" << getLaneEdgeMode()
        << "\" laneShowBorders=\"" << laneShowBorders
        << "\" showLinkDecals=\"" << showLinkDecals
        << "\" showRails=\"" << showRails
        << "\" hideConnectors=\"" << hideConnectors << "\"\n"
        << "               " << edgeName.print("edgeName") << "\n"
        << "               " << internalEdgeName.print("internalEdgeName") << "\n"
        << "               " << cwaEdgeName.print("cwaEdgeName") << "\n"
        << "               " << streetName.print("streetName") << ">\n";
    laneColorer.save(dev);
#ifdef HAVE_INTERNAL
    edgeColorer.save(dev);
#endif
    dev << "        </edges>\n";

    dev << "        <vehicles vehicleMode=\"" << vehicleColorer.getActive()
        << "\" vehicleQuality=\"" << vehicleQuality
        << "\" minVehicleSize=\"" << minVehicleSize
        << "\" vehicleExaggeration=\"" << vehicleExaggeration
        << "\" showBlinker=\"" << showBlinker << "\"\n"
        << "                  " << vehicleName.print("vehicleName")
        << ">\n";
    vehicleColorer.save(dev);
    dev << "        </vehicles>\n";
    dev << "        <persons personMode=\"" << personColorer.getActive()
        << "\" personQuality=\"" << personQuality
        << "\" minPersonSize=\"" << minPersonSize
        << "\" personExaggeration=\"" << personExaggeration
        << "\" " << personName.print("personName")
        << ">\n";
    personColorer.save(dev);
    dev << "        </persons>\n";

    dev << "        <junctions junctionMode=\"" << junctionColorer.getActive()
        << "\" drawLinkTLIndex=\"" << drawLinkTLIndex
        << "\" drawLinkJunctionIndex=\"" << drawLinkJunctionIndex << "\"\n"
        << "                  " << junctionName.print("junctionName") << "\n"
        << "                  " << internalJunctionName.print("internalJunctionName") << "\n"
        << " showLane2Lane=\"" << showLane2Lane << "\""
        << " drawShape=\"" << drawJunctionShape << "\">\n";
    junctionColorer.save(dev);
    dev << "        </junctions>\n";

    dev << "        <additionals addMode=\"" << addMode
        << "\" minAddSize=\"" << minAddSize
        << "\" addExaggeration=\"" << addExaggeration << "\"\n"
        << "                  " << addName.print("addName")
        << "/>\n";

    dev << "        <pois poiExaggeration=\"" << poiExaggeration
        << "\" minPOISize=\"" << minPOISize << "\"\n"
        << "                  " << poiName.print("poiName")
        << "/>\n";

    dev << "        <polys polyExaggeration=\"" << polyExaggeration
        << "\" minPolySize=\"" << minPolySize << "\"\n"
        << "                  " << polyName.print("polyName")
        << "/>\n";

    dev << "        <legend showSizeLegend=\"" << showSizeLegend << "\"/>\n";
    dev << "    </scheme>\n";
    dev << "</viewsettings>\n";
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

#ifdef HAVE_INTERNAL
    if (!(edgeColorer == v2.edgeColorer)) {
        return false;
    }
#endif
    if (!(laneColorer == v2.laneColorer)) {
        return false;
    }
    if (laneShowBorders != v2.laneShowBorders) {
        return false;
    }
    if (showLinkDecals != v2.showLinkDecals) {
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
    if (!(vehicleColorer == v2.vehicleColorer)) {
        return false;
    }
    if (vehicleQuality != v2.vehicleQuality) {
        return false;
    }
    if (minVehicleSize != v2.minVehicleSize) {
        return false;
    }
    if (vehicleExaggeration != v2.vehicleExaggeration) {
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
    if (vehicleName != v2.vehicleName) {
        return false;
    }
    if (!(personColorer == v2.personColorer)) {
        return false;
    }
    if (personQuality != v2.personQuality) {
        return false;
    }
    if (minPersonSize != v2.minPersonSize) {
        return false;
    }
    if (personExaggeration != v2.personExaggeration) {
        return false;
    }
    if (personName != v2.personName) {
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

    if (addMode != v2.addMode) {
        return false;
    }
    if (minAddSize != v2.minAddSize) {
        return false;
    }
    if (addExaggeration != v2.addExaggeration) {
        return false;
    }
    if (addName != v2.addName) {
        return false;
    }

    if (minPOISize != v2.minPOISize) {
        return false;
    }
    if (poiExaggeration != v2.poiExaggeration) {
        return false;
    }
    if (poiName != v2.poiName) {
        return false;
    }

    if (minPolySize != v2.minPolySize) {
        return false;
    }
    if (polyExaggeration != v2.polyExaggeration) {
        return false;
    }
    if (polyName != v2.polyName) {
        return false;
    }

    if (showSizeLegend != v2.showSizeLegend) {
        return false;
    }

    return true;
}



/****************************************************************************/

