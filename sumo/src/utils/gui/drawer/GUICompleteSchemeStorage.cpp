/****************************************************************************/
/// @file    GUICompleteSchemeStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id$
///
// missing_desc
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

#include "GUICompleteSchemeStorage.h"
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/RGBColor.h>
#include <utils/foxtools/MFXUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static variable definitions
// ===========================================================================
GUICompleteSchemeStorage gSchemeStorage;


// ===========================================================================
// method definitions
// ===========================================================================
GUICompleteSchemeStorage::GUICompleteSchemeStorage()
{ }


GUICompleteSchemeStorage::~GUICompleteSchemeStorage()
{ }



void
GUICompleteSchemeStorage::add(GUIVisualizationSettings &scheme)
{
    std::string name = scheme.name;
    if (std::find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name)==mySortedSchemeNames.end()) {
        mySortedSchemeNames.push_back(name);
    }
    mySettings[name] = scheme;
}


GUIVisualizationSettings &
GUICompleteSchemeStorage::get(const std::string &name)
{
    return mySettings.find(name)->second;
}


bool
GUICompleteSchemeStorage::contains(const std::string &name) const
{
    return mySettings.find(name)!=mySettings.end();
}


void
GUICompleteSchemeStorage::remove(const std::string &name)
{
    if (!contains(name)) {
        return;
    }
    mySortedSchemeNames.erase(find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name));
    mySettings.erase(mySettings.find(name));
}


const std::vector<std::string> &
GUICompleteSchemeStorage::getNames() const
{
    return mySortedSchemeNames;
}


const std::map<std::string, GUIVisualizationSettings> &
GUICompleteSchemeStorage::getItems() const
{
    return mySettings;
}


RGBColor
convert(const FXColor c)
{
    return RGBColor(
               (SUMOReal) FXREDVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXGREENVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXBLUEVAL(c) / (SUMOReal) 255.);
}


void
GUICompleteSchemeStorage::init(FXApp *app,
                               const map<int, vector<RGBColor> > &vehColMap,
                               const map<int, vector<RGBColor> > &laneColMap,
                               const map<int, vector<RGBColor> > &edgeColMap)
{
    {
        GUIVisualizationSettings vs;
        vs.name = "standard";

        vs.antialiase = false;
        vs.dither = false;
        vs.vehicleQuality = 0;

        vs.backgroundColor = RGBColor((SUMOReal) 1, (SUMOReal) 1, (SUMOReal) 1);
        vs.showGrid = false;
        vs.gridXSize = 100;
        vs.gridYSize = 100;

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
#ifdef HAVE_MESOSIM
        vs.edgeColorings = edgeColMap;
#endif
        vs.laneShowBorders = false;
        vs.showLinkDecals = true;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = true;
        vs.drawEdgeName = false;
        vs.edgeNameSize = 50;
        vs.edgeNameColor = RGBColor((SUMOReal) 1, (SUMOReal) .5, (SUMOReal) 0);

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 1;
        vs.vehicleExaggeration = 1;
        vs.showBlinker = true;
        vs.drawcC2CRadius = false;
        vs.drawLaneChangePreference = false;
        vs.drawVehicleName = false;
        vs.vehicleNameSize = 50;
        vs.vehicleNameColor = RGBColor((SUMOReal) .8, (SUMOReal) .6, (SUMOReal) 0);

        vs.junctionMode = 0;
        vs.drawLinkTLIndex = false;
        vs.drawLinkJunctionIndex = false;
        vs.drawJunctionName = false;
        vs.junctionNameSize = 50;
        vs.junctionNameColor = RGBColor((SUMOReal) 0, (SUMOReal) 1, (SUMOReal) .5);

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;
        vs.drawAddName = false;
        vs.addNameSize = 50;
        //vs.addNameColor = RGBColor(0, .5, 1);

        vs.minPOISize = 0;
        vs.poiExaggeration = 1;
        vs.drawPOIName = false;
        vs.poiNameSize = 50;
        vs.poiNameColor = RGBColor((SUMOReal) 1., (SUMOReal) 0, (SUMOReal) .5);

        vs.showSizeLegend = true;

        gSchemeStorage.add(vs);
    }
    {
        GUIVisualizationSettings vs;
        vs.name = "faster standard";

        vs.antialiase = false;
        vs.dither = false;
        vs.vehicleQuality = 0;

        vs.backgroundColor = RGBColor((SUMOReal) 1, (SUMOReal) 1, (SUMOReal) 1);
        vs.showGrid = false;
        vs.gridXSize = 100;
        vs.gridYSize = 100;

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
#ifdef HAVE_MESOSIM
        vs.edgeColorings = edgeColMap;
#endif
        vs.laneShowBorders = false;
        vs.showLinkDecals = false;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = false;
        vs.drawEdgeName = false;
        vs.edgeNameSize = 50;
        vs.edgeNameColor = RGBColor((SUMOReal) 1, (SUMOReal) .5, (SUMOReal) 0);

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 1;
        vs.vehicleExaggeration = 1;
        vs.showBlinker = true;
        vs.drawcC2CRadius = false;
        vs.drawLaneChangePreference = false;
        vs.drawVehicleName = false;
        vs.vehicleNameSize = 50;
        vs.vehicleNameColor = RGBColor((SUMOReal) .8, (SUMOReal) .6, (SUMOReal) 0);

        vs.junctionMode = 0;
        vs.drawLinkTLIndex = false;
        vs.drawLinkJunctionIndex = false;
        vs.drawJunctionName = false;
        vs.junctionNameSize = 50;
        vs.junctionNameColor = RGBColor((SUMOReal) 0, (SUMOReal) 1, (SUMOReal) .5);

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;
        vs.drawAddName = false;
        vs.addNameSize = 50;
        //vs.addNameColor = RGBColor(0, .5, 1);

        vs.poiExaggeration = 1;
        vs.minPOISize = 0;
        vs.drawPOIName = false;
        vs.poiNameSize = 50;
        vs.poiNameColor = RGBColor((SUMOReal) 1., (SUMOReal) 0, (SUMOReal) .5);

        vs.showSizeLegend = true;

        gSchemeStorage.add(vs);
    }
    {
        GUIVisualizationSettings vs;
        vs.name = "real world";

        vs.antialiase = false;
        vs.dither = false;
        vs.vehicleQuality = 2;

        vs.backgroundColor = RGBColor((SUMOReal) .2, (SUMOReal) .5, (SUMOReal) .2);
        vs.showGrid = false;
        vs.gridXSize = 100;
        vs.gridYSize = 100;

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
#ifdef HAVE_MESOSIM
        vs.edgeColorings = edgeColMap;
#endif
        vs.laneShowBorders = true;
        vs.showLinkDecals = true;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = true;
        vs.drawEdgeName = false;
        vs.edgeNameSize = 50;
        vs.edgeNameColor = RGBColor((SUMOReal) 1, (SUMOReal) .5, (SUMOReal) 0);

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 0;
        vs.vehicleExaggeration = 1;
        vs.minVehicleSize = 1;
        /*
        vs.minVehicleColor = RGBColor(1,0,0);
        vs.maxVehicleColor = RGBColor(0,1,0);
        */
        vs.showBlinker = true;
        vs.drawcC2CRadius = false;
        vs.drawLaneChangePreference = false;
        vs.drawVehicleName = false;
        vs.vehicleNameSize = 50;
        vs.vehicleNameColor = RGBColor((SUMOReal) .8, (SUMOReal) .6, (SUMOReal) 0);

        vs.junctionMode = 0;
        vs.drawLinkTLIndex = false;
        vs.drawLinkJunctionIndex = false;
        vs.drawJunctionName = false;
        vs.junctionNameSize = 50;
        vs.junctionNameColor = RGBColor((SUMOReal) 0, (SUMOReal) 1, (SUMOReal) .5);

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;
        vs.drawAddName = false;
        vs.addNameSize = 50;
        //vs.addNameColor = RGBColor(0, .5, 1);

        vs.minPOISize = 0;
        vs.poiExaggeration = 1;
        vs.drawPOIName = false;
        vs.poiNameSize = 50;
        vs.poiNameColor = RGBColor((SUMOReal) 1., (SUMOReal) 0, (SUMOReal) .5);

        vs.showSizeLegend = true;

        gSchemeStorage.add(vs);
    }
    // add saved settings
    int noSaved = app->reg().readIntEntry("VisualizationSettings", "settingNo", 0);
    string setting1 = app->reg().readStringEntry("VisualizationSettings", "visset#0", "");

    for (int i=0; i<noSaved; ++i) {
        string name = "visset#" + toString(i);
        string setting = app->reg().readStringEntry("VisualizationSettings",name.c_str(), "");
        if (setting!="") {
            size_t j, k;
            GUIVisualizationSettings vs;

            vs.name = setting;
            app->reg().readStringEntry("VisualizationSettings",name.c_str(), "");

            vs.antialiase = app->reg().readIntEntry(name.c_str(), "antialiase", 0)!=0 ? 1 : 0;
            vs.dither = app->reg().readIntEntry(name.c_str(), "dither", 0)!=0 ? 1 : 0;

            vs.backgroundColor = convert(app->reg().readIntEntry(name.c_str(), "backgroundColor", FXRGB(255, 255, 255)));
            vs.showGrid = app->reg().readIntEntry(name.c_str(), "showGrid", 0)!=0 ? 1 : 0;
            vs.gridXSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "gridXSize", 1000);
            vs.gridYSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "gridYSize", 1000);

            vs.laneEdgeMode = app->reg().readIntEntry(name.c_str(), "laneEdgeMode", 0);
            size_t nlc = app->reg().readIntEntry(name.c_str(), "noLaneCols", 0);
            for (j=0; j<nlc; ++j) {
                size_t nlc1 = (size_t) app->reg().readIntEntry(name.c_str(), ("nlcN" + toString(j)).c_str(), 0);
                size_t nlc2 = (size_t) app->reg().readIntEntry(name.c_str(), ("nlcS" + toString(j)).c_str(), 0);
                std::vector<RGBColor> cols;
                for (k=0; k<nlc2; ++k) {
                    cols.push_back(convert(app->reg().readIntEntry(name.c_str(), ("nlcC" + toString(j) + "_" + toString(k)).c_str(), FXRGB(255, 255, 255))));
                }
                vs.laneColorings[nlc1] = cols;
            }
            vs.laneShowBorders = app->reg().readIntEntry(name.c_str(), "laneShowBorders", 0)!=0 ? 1 : 0;
            vs.showLinkDecals = app->reg().readIntEntry(name.c_str(), "showLinkDecals", 0)!=0 ? 1 : 0;
            vs.laneEdgeExaggMode = app->reg().readIntEntry(name.c_str(), "laneEdgeExaggMode", 0);
            vs.minExagg = (SUMOReal) app->reg().readRealEntry(name.c_str(), "minExagg", 1);
            vs.maxExagg = (SUMOReal) app->reg().readRealEntry(name.c_str(), "maxExagg", 1);
            vs.showRails = app->reg().readIntEntry(name.c_str(), "showRails", 0)!=0 ? 1 : 0;
            vs.drawEdgeName = app->reg().readIntEntry(name.c_str(), "drawEdgeName", 0)!=0 ? 1 : 0;
            vs.edgeNameSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "edgeNameSize", 50);
            vs.edgeNameColor = convert(app->reg().readIntEntry(name.c_str(), "edgeNameColor", FXRGB(255, 255, 255)));

            vs.vehicleMode = app->reg().readIntEntry(name.c_str(), "vehicleMode", 0);
            vs.vehicleQuality = app->reg().readIntEntry(name.c_str(), "vehicleQuality", 0);
            size_t nvc = app->reg().readIntEntry(name.c_str(), "noVehCols", 0);
            for (j=0; j<nvc; ++j) {
                size_t nvc1 = (size_t) app->reg().readIntEntry(name.c_str(), ("nvcN" + toString(j)).c_str(), 0);
                size_t nvc2 = (size_t) app->reg().readIntEntry(name.c_str(), ("nvcS" + toString(j)).c_str(), 0);
                std::vector<RGBColor> cols;
                for (k=0; k<nvc2; ++k) {
                    cols.push_back(convert(app->reg().readIntEntry(name.c_str(), ("nvcC" + toString(j) + "_" + toString(k)).c_str(), FXRGB(255, 255, 255))));
                }
                vs.vehicleColorings[nvc1] = cols;
            }
            vs.minVehicleSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "minVehicleSize", 1);
            vs.vehicleExaggeration = (SUMOReal) app->reg().readRealEntry(name.c_str(), "vehicleExaggeration", 1);
            vs.showBlinker = app->reg().readIntEntry(name.c_str(), "showBlinker", 0)!=0 ? 1 : 0;
            vs.drawcC2CRadius = app->reg().readIntEntry(name.c_str(), "drawcC2CRadius", 0)!=0 ? 1 : 0;
            vs.drawLaneChangePreference = app->reg().readIntEntry(name.c_str(), "drawLaneChangePreference", 0)!=0 ? 1 : 0;
            vs.drawVehicleName = app->reg().readIntEntry(name.c_str(), "drawVehicleName", 0)!=0 ? 1 : 0;
            vs.vehicleNameSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "vehicleNameSize", 50);
            vs.vehicleNameColor = convert(app->reg().readIntEntry(name.c_str(), "vehicleNameColor", FXRGB(255, 255, 255)));

            vs.junctionMode = app->reg().readIntEntry(name.c_str(), "junctionMode", 0);
            vs.drawLinkTLIndex = app->reg().readIntEntry(name.c_str(), "drawLinkTLIndex", 0)!=0 ? 1 : 0;
            vs.drawLinkJunctionIndex = app->reg().readIntEntry(name.c_str(), "drawLinkJunctionIndex", 0)!=0 ? 1 : 0;
            vs.drawJunctionName = app->reg().readIntEntry(name.c_str(), "drawJunctionName", 0)!=0 ? 1 : 0;
            vs.junctionNameSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "junctionNameSize", 50);
            vs.junctionNameColor = convert(app->reg().readIntEntry(name.c_str(), "junctionNameColor", FXRGB(255, 255, 255)));

            vs.showLane2Lane = app->reg().readIntEntry(name.c_str(), "showLane2Lane", 0)!=0 ? 1 : 0;

            vs.addMode = app->reg().readIntEntry(name.c_str(), "addMode", 0);
            vs.minAddSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "minAddSize", 1);
            vs.addExaggeration = (SUMOReal) app->reg().readRealEntry(name.c_str(), "addExaggeration", 1);
            vs.drawAddName = app->reg().readIntEntry(name.c_str(), "drawAddName", 0)!=0 ? 1 : 0;
            vs.addNameSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "addNameSize", 50);
            //vs.addNameColor = RGBColor(0, .5, 1);

            vs.poiExaggeration = (SUMOReal) app->reg().readRealEntry(name.c_str(), "poiExaggeration", 1);
            vs.minPOISize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "minPOISize", 1);
            vs.drawPOIName = app->reg().readIntEntry(name.c_str(), "drawPOIName", 0)!=0 ? 1 : 0;
            vs.poiNameSize = (SUMOReal) app->reg().readRealEntry(name.c_str(), "poiNameSize", 50);
            vs.poiNameColor = convert(app->reg().readIntEntry(name.c_str(), "poiNameColor", FXRGB(255, 255, 255)));

            vs.showSizeLegend = app->reg().readIntEntry(name.c_str(), "showSizeLegend", 0)!=0 ? 1 : 0;

            gSchemeStorage.add(vs);
        }
    }
}



/****************************************************************************/

