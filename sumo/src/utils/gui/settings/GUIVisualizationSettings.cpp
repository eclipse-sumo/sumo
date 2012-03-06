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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include "GUIColorScheme.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
#ifdef HAVE_MESOSIM
bool GUIVisualizationSettings::UseMesoSim = false;
#endif

// ===========================================================================
// member method definitions
// ===========================================================================
GUIVisualizationSettings::GUIVisualizationSettings()
    : name(""), antialiase(false), dither(false),
      backgroundColor(RGBColor(1, 1, 1)),
      showGrid(false), gridXSize(100), gridYSize(100),
      laneShowBorders(false), showLinkDecals(true), showRails(true),
      edgeName(false, 50, RGBColor(1, .5, 0)),
      internalEdgeName(false, 40, RGBColor(.5, .25, 0)),
      streetName(false, 55, RGBColor(1, 1, 0)),
      hideConnectors(false), vehicleQuality(0),
      minVehicleSize(1), vehicleExaggeration(1), showBlinker(true),
      drawLaneChangePreference(false), drawMinGap(false),
      vehicleName(false, 50, RGBColor(.8, .6, 0)),
      junctionMode(0), drawLinkTLIndex(false), drawLinkJunctionIndex(false),
      junctionName(false, 50, RGBColor(0, 1, .5)),
      internalJunctionName(false, 50, RGBColor(0, .8, .5)),
      showLane2Lane(false), addMode(0), minAddSize(1), addExaggeration(1),
      addName(false, 50, RGBColor(1., 0, .5)),
      poiName(false, 50, RGBColor(1., 0, .5)),
      minPOISize(0), poiExaggeration(1),
      showSizeLegend(true),
      gaming(false),
      selectionScale(1) {
    /// add lane coloring schemes
    GUIColorScheme scheme = GUIColorScheme("uniform", RGBColor(0, 0, 0), "", true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection (lane-/streetwise)", RGBColor(0.7f, 0.7f, 0.7f), "unselected", true);
    scheme.addColor(RGBColor(0, .4f, .8f), 1, "selected");
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by vclass", RGBColor(0, 0, 0), "all", true);
    scheme.addColor(RGBColor(0, .1f, .5f), 1, "public");
    laneColorer.addScheme(scheme);
    // ... traffic states ...
    scheme = GUIColorScheme("by allowed speed (lanewise)", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0 / 3.6));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (lanewise)", RGBColor(0, 0, 1));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)0.95);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by first vehicle waiting time (lanewise)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)200);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by lane number (streetwise)", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)5);
    laneColorer.addScheme(scheme);
    // ... emissions ...
    scheme = GUIColorScheme("by CO2 emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(10. / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by CO emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(0.05 / 7.5 / 2.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by PMx emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(.005 / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by NOx emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(.125 / 7.5 / 5.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by HC emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(.02 / 7.5 / 4.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by fuel consumption (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(.005 / 7.5 * 100.));
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by noise emissions (Harmonoise)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)100);
    laneColorer.addScheme(scheme);
    // ... weights (experimental) ...
    scheme = GUIColorScheme("by global travel time", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)100);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by global speed percentage", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(1, 1, 0), (SUMOReal)50);
    scheme.addColor(RGBColor(0, 1, 0), (SUMOReal)100);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);


    /// add vehicle coloring schemes
    vehicleColorer.addScheme(GUIColorScheme("uniform", RGBColor(1, 1, 0), "", true));
    vehicleColorer.addScheme(GUIColorScheme("given/assigned vehicle color", RGBColor(1, 1, 0), "", true));
    vehicleColorer.addScheme(GUIColorScheme("given/assigned type color", RGBColor(1, 1, 0), "", true));
    vehicleColorer.addScheme(GUIColorScheme("given/assigned route color", RGBColor(1, 1, 0), "", true));
    vehicleColorer.addScheme(GUIColorScheme("depart position as HSV", RGBColor(1, 1, 0), "", true));
    vehicleColorer.addScheme(GUIColorScheme("arrival position as HSV", RGBColor(1, 1, 0), "", true));
    vehicleColorer.addScheme(GUIColorScheme("direction/distance as HSV", RGBColor(1, 1, 0), "", true));
    scheme = GUIColorScheme("by speed", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0 / 3.6));
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by waiting time", RGBColor(0, 0, 1));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)(5 * 60));
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by time since last lanechange", RGBColor(1, 1, 1));
    scheme.addColor(RGBColor(.5, .5, .5), (SUMOReal)(5 * 60));
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by max speed", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0 / 3.6));
    vehicleColorer.addScheme(scheme);
    // ... emissions ...
    scheme = GUIColorScheme("by CO2 emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)5.);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by CO emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)0.05);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by PMx emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal).005);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by NOx emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal).125);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by HC emissions (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal).02);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by fuel consumption (HBEFA)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal).005);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by noise emissions (Harmonoise)", RGBColor(0, 1, 0));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)100.);
    vehicleColorer.addScheme(scheme);
    scheme = GUIColorScheme("by reroute number", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(1, 1, 0), (SUMOReal)1.);
    scheme.addColor(RGBColor(1, 1, 1), (SUMOReal)10.);
    vehicleColorer.addScheme(scheme);


#ifdef HAVE_MESOSIM
    /// add edge coloring schemes
    edgeColorer.addScheme(GUIColorScheme("uniform (streetwise)", RGBColor(0, 0, 0), "", true));
    scheme = GUIColorScheme("by selection (streetwise)", RGBColor(0.7f, 0.7f, 0.7f), "unselected", true);
    scheme.addColor(RGBColor(0, .4f, .8f), 1, "selected");
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by purpose (streetwise)", RGBColor(0, 0, 0), "normal", true);
    scheme.addColor(RGBColor(.5, 0, .5), 1, "connector"); // see MSEdge::EdgeBasicFunction::EDGEFUNCTION_CONNECTOR
    scheme.addColor(RGBColor(0, 0, 1), 2, "internal"); // see MSEdge::EdgeBasicFunction::EDGEFUNCTION_INTERNAL
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by allowed speed (streetwise)", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current occupancy (streetwise)", RGBColor(0, 0, 1));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)0.95);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current speed (streetwise)", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0 / 3.6));
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by current flow (streetwise)", RGBColor(0, 0, 1));
    scheme.addColor(RGBColor(1, 0, 0), (SUMOReal)5000);
    edgeColorer.addScheme(scheme);
    scheme = GUIColorScheme("by relative speed (streetwise)", RGBColor(1, 0, 0));
    scheme.addColor(RGBColor(0, 0, 1), (SUMOReal)1);
    edgeColorer.addScheme(scheme);
#endif
}


size_t
GUIVisualizationSettings::getLaneEdgeMode() const {
#ifdef HAVE_MESOSIM
    if (UseMesoSim) {
        return edgeColorer.getActive();
    }
#endif
    return laneColorer.getActive();
}


GUIColorScheme&
GUIVisualizationSettings::getLaneEdgeScheme() {
#ifdef HAVE_MESOSIM
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
        << "               " << streetName.print("streetName") << ">\n";
    laneColorer.save(dev);
#ifdef HAVE_MESOSIM
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

    dev << "        <junctions junctionMode=\"" << junctionMode
        << "\" drawLinkTLIndex=\"" << drawLinkTLIndex
        << "\" drawLinkJunctionIndex=\"" << drawLinkJunctionIndex << "\"\n"
        << "                  " << junctionName.print("junctionName") << "\n"
        << "                  " << internalJunctionName.print("internalJunctionName") << "\n"
        << " showLane2Lane=\"" << showLane2Lane << "\"/>\n";

    dev << "        <additionals addMode=\"" << addMode
        << "\" minAddSize=\"" << minAddSize
        << "\" addExaggeration=\"" << addExaggeration << "\"\n"
        << "                  " << addName.print("addName")
        << "/>\n";

    dev << "        <pois poiExaggeration=\"" << poiExaggeration
        << "\" minPOISize=\"" << minPOISize << "\"\n"
        << "                  " << poiName.print("poiName")
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
    if (showBackgroundDecals != v2.showBackgroundDecals) {
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

#ifdef HAVE_MESOSIM
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
    if (streetName != v2.streetName) {
        return false;
    }
    if (hideConnectors != v2.hideConnectors) {
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
    if (junctionMode != v2.junctionMode) {
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

    if (showSizeLegend != v2.showSizeLegend) {
        return false;
    }

    return true;
}



/****************************************************************************/

