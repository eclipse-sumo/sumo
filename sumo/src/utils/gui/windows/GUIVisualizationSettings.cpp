/****************************************************************************/
/// @file    GUIVisualizationSettings.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Stores the information about how to visualize structures
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "GUIVisualizationSettings.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
bool
GUIVisualizationSettings::operator==(const GUIVisualizationSettings &v2)
{
    if (antialiase!=v2.antialiase) return false;
    if (dither!=v2.dither) return false;
    if (backgroundColor!=v2.backgroundColor) return false;
    if (showBackgroundDecals!=v2.showBackgroundDecals) return false;
    if (showGrid!=v2.showGrid) return false;
    if (gridXSize!=v2.gridXSize) return false;
    if (gridYSize!=v2.gridYSize) return false;

    if (laneEdgeMode!=v2.laneEdgeMode) return false;
    if (laneColorings!=v2.laneColorings) return false;
    if (laneShowBorders!=v2.laneShowBorders) return false;
    if (showLinkDecals!=v2.showLinkDecals) return false;
    if (laneEdgeExaggMode!=v2.laneEdgeExaggMode) return false;
    if (minExagg!=v2.minExagg) return false;
    if (maxExagg!=v2.maxExagg) return false;
    if (showRails!=v2.showRails) return false;
    if (drawEdgeName!=v2.drawEdgeName) return false;
    if (edgeNameSize!=v2.edgeNameSize) return false;
    if (edgeNameColor!=v2.edgeNameColor) return false;
    if (hideConnectors!=v2.hideConnectors) return false;

    if (vehicleMode!=v2.vehicleMode) return false;
    if (vehicleQuality!=v2.vehicleQuality) return false;
    if (minVehicleSize!=v2.minVehicleSize) return false;
    if (vehicleExaggeration!=v2.vehicleExaggeration) return false;
    if (vehicleColorings!=v2.vehicleColorings) return false;
    if (showBlinker!=v2.showBlinker) return false;
    if (drawcC2CRadius!=v2.drawcC2CRadius) return false;
    if (drawLaneChangePreference!=v2.drawLaneChangePreference) return false;
    if (drawVehicleName!=v2.drawVehicleName) return false;
    if (vehicleNameSize!=v2.vehicleNameSize) return false;
    if (vehicleNameColor!=v2.vehicleNameColor) return false;

    if (junctionMode!=v2.junctionMode) return false;
    if (drawLinkTLIndex!=v2.drawLinkTLIndex) return false;
    if (drawLinkJunctionIndex!=v2.drawLinkJunctionIndex) return false;
    if (drawJunctionName!=v2.drawJunctionName) return false;
    if (junctionNameSize!=v2.junctionNameSize) return false;
    if (junctionNameColor!=v2.junctionNameColor) return false;

    if (showLane2Lane!=v2.showLane2Lane) return false;

    if (addMode!=v2.addMode) return false;
    if (minAddSize!=v2.minAddSize) return false;
    if (addExaggeration!=v2.addExaggeration) return false;
    if (drawAddName!=v2.drawAddName) return false;
    if (addNameSize!=v2.addNameSize) return false;

    if (minPOISize!=v2.minPOISize) return false;
    if (poiExaggeration!=v2.poiExaggeration) return false;
    if (drawPOIName!=v2.drawPOIName) return false;
    if (poiNameSize!=v2.poiNameSize) return false;
    if (poiNameColor!=v2.poiNameColor) return false;

    if (showSizeLegend!=v2.showSizeLegend) return false;

    return true;
}



/****************************************************************************/

