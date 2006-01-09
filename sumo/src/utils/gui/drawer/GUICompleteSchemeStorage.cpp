/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "GUICompleteSchemeStorage.h"

void
GUICompleteSchemeStorage::init()
{
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.addExaggeration = 1;
        vs.addMode = 0;
        vs.backgroundColor = RGBColor(1, 1, 1);
        vs.antialiase = false;
        vs.dither = false;
        vs.junctionMode = 0;
        vs.laneEdgeMode = 0;
        vs.laneShowBorders = false;
        vs.showLinkDecals = true;
        vs.maxVehicleColor = RGBColor(0,1,0);
        vs.minAddSize = 1;
        vs.minVehicleColor = RGBColor(1,0,0);
        vs.minVehicleSize = 1;
        vs.showLane2Lane = false;
        vs.singleLaneColor = RGBColor(0,0,0);
        vs.vehicleExaggeration = 1;
        vs.vehicleMode = 0;
        gSchemeStorage.add("standard", vs);
    }
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.addExaggeration = 1;
        vs.addMode = 0;
        vs.backgroundColor = RGBColor(1, 1, 1);
        vs.antialiase = false;
        vs.dither = false;
        vs.junctionMode = 0;
        vs.laneEdgeMode = 0;
        vs.laneShowBorders = false;
        vs.showLinkDecals = false;
        vs.maxVehicleColor = RGBColor(0,1,0);
        vs.minAddSize = 1;
        vs.minVehicleColor = RGBColor(1,0,0);
        vs.minVehicleSize = 1;
        vs.showLane2Lane = false;
        vs.singleLaneColor = RGBColor(0,0,0);
        vs.vehicleExaggeration = 1;
        vs.vehicleMode = 0;
        gSchemeStorage.add("faster standard", vs);
    }
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.addExaggeration = 1;
        vs.addMode = 0;
        vs.backgroundColor = RGBColor((SUMOReal) .2, (SUMOReal) .5, (SUMOReal) .2);
        vs.antialiase = true;
        vs.dither = true;
        vs.junctionMode = 0;
        vs.laneEdgeMode = 0;
        vs.laneShowBorders = true;
        vs.showLinkDecals = true;
        vs.maxVehicleColor = RGBColor(0,1,0);
        vs.minAddSize = 1;
        vs.minVehicleColor = RGBColor(1,0,0);
        vs.minVehicleSize = 1;
        vs.showLane2Lane = false;
        vs.singleLaneColor = RGBColor((SUMOReal) .3, (SUMOReal) .3, (SUMOReal) .4);
        vs.vehicleExaggeration = 1;
        vs.vehicleMode = 0;
        gSchemeStorage.add("real world", vs);
    }
}

GUICompleteSchemeStorage gSchemeStorage;
