/****************************************************************************/
/// @file    GUIVisualizationSettings.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Stores the information about how to visualize structures
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

#include <map>
#include <vector>
#include "GUIVisualizationSettings.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUIVisualizationSettings::GUIVisualizationSettings() throw()
        : name(""), antialiase(false), dither(false),
        backgroundColor(RGBColor(1, 1, 1)),
        showGrid(false), gridXSize(100), gridYSize(100),
        laneShowBorders(false), showLinkDecals(true), showRails(true),
        edgeName(false, 50, RGBColor(1, .5, 0)),
        internalEdgeName(false, 40, RGBColor(.5, .25, 0)),
        streetName(false, 55, RGBColor(1, 1, 0)),
        hideConnectors(false), vehicleQuality(0),
        minVehicleSize(1), vehicleExaggeration(1), showBlinker(true),
        drawLaneChangePreference(false),
        vehicleName(false, 50, RGBColor(.8, .6, 0)),
        junctionMode(0), drawLinkTLIndex(false), drawLinkJunctionIndex(false),
        junctionName(false, 50, RGBColor(0, 1, .5)),
        internalJunctionName(false, 50, RGBColor(0, .8, .5)),
        showLane2Lane(false), addMode(0), minAddSize(1), addExaggeration(1),
        addName(false, 50, RGBColor(1., 0, .5)),
        poiName(false, 50, RGBColor(1., 0, .5)),
        minPOISize(0), poiExaggeration(1), 
        showSizeLegend(true),
        gaming(false)
{
}


size_t
GUIVisualizationSettings::getLaneEdgeMode() const {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        return edgeColorer.getActive();
    }
#endif
    return laneColorer.getActive();
}


GUIColorScheme&
GUIVisualizationSettings::getLaneEdgeScheme() {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        return edgeColorer.getScheme();
    }
#endif
    return laneColorer.getScheme();
}


void
GUIVisualizationSettings::save(OutputDevice &dev) const throw(IOError) {
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
GUIVisualizationSettings::operator==(const GUIVisualizationSettings &v2) {
    if (antialiase!=v2.antialiase) return false;
    if (dither!=v2.dither) return false;
    if (backgroundColor!=v2.backgroundColor) return false;
    if (showBackgroundDecals!=v2.showBackgroundDecals) return false;
    if (showGrid!=v2.showGrid) return false;
    if (gridXSize!=v2.gridXSize) return false;
    if (gridYSize!=v2.gridYSize) return false;

#ifdef HAVE_MESOSIM
    if (!(edgeColorer==v2.edgeColorer)) return false;
#endif
    if (!(laneColorer==v2.laneColorer)) return false;
    if (laneShowBorders!=v2.laneShowBorders) return false;
    if (showLinkDecals!=v2.showLinkDecals) return false;
    if (showRails!=v2.showRails) return false;
    if (edgeName!=v2.edgeName) return false;
    if (internalEdgeName!=v2.internalEdgeName) return false;
    if (streetName!=v2.streetName) return false;
    if (hideConnectors!=v2.hideConnectors) return false;

    if (!(vehicleColorer==v2.vehicleColorer)) return false;
    if (vehicleQuality!=v2.vehicleQuality) return false;
    if (minVehicleSize!=v2.minVehicleSize) return false;
    if (vehicleExaggeration!=v2.vehicleExaggeration) return false;
    if (showBlinker!=v2.showBlinker) return false;
    if (drawLaneChangePreference!=v2.drawLaneChangePreference) return false;
    if (vehicleName!=v2.vehicleName) return false;
    if (junctionMode!=v2.junctionMode) return false;
    if (drawLinkTLIndex!=v2.drawLinkTLIndex) return false;
    if (drawLinkJunctionIndex!=v2.drawLinkJunctionIndex) return false;
    if (junctionName!=v2.junctionName) return false;
    if (internalJunctionName!=v2.internalJunctionName) return false;

    if (showLane2Lane!=v2.showLane2Lane) return false;

    if (addMode!=v2.addMode) return false;
    if (minAddSize!=v2.minAddSize) return false;
    if (addExaggeration!=v2.addExaggeration) return false;
    if (addName!=v2.addName) return false;

    if (minPOISize!=v2.minPOISize) return false;
    if (poiExaggeration!=v2.poiExaggeration) return false;
    if (poiName!=v2.poiName) return false;

    if (showSizeLegend!=v2.showSizeLegend) return false;

    return true;
}



/****************************************************************************/

