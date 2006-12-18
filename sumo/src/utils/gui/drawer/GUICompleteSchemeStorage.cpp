/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "GUICompleteSchemeStorage.h"

using namespace std;

void
GUICompleteSchemeStorage::init(const map<int, vector<RGBColor> > &vehColMap,
                               const map<int, vector<RGBColor> > &laneColMap)
{
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.antialiase = false;
        vs.dither = false;

        vs.backgroundColor = RGBColor(1, 1, 1);
        vs.showLinkDecals = true;

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
//        vs.singleLaneColor = RGBColor(0,0,0);
        vs.laneShowBorders = false;
        vs.showLinkDecals = true; // !!! used?
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = true;

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 1;
        vs.vehicleExaggeration = 1;
        /*
        vs.minVehicleColor = RGBColor(1,0,0);
        vs.maxVehicleColor = RGBColor(0,1,0);
        */
        vs.showBlinker = true;
        vs.drawcC2CRadius = true;
        vs.drawLaneChangePreference = true;
        vs.drawVehicleName = false;

        vs.junctionMode = 0;

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;

        vs.minPOISize = 0;
        vs.poiExaggeration = 1;
        gSchemeStorage.add("standard", vs);
    }
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.antialiase = false;
        vs.dither = false;

        vs.backgroundColor = RGBColor(1, 1, 1);
        vs.showLinkDecals = false;

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
//        vs.singleLaneColor = RGBColor(0,0,0);
        vs.laneShowBorders = false;
        vs.showLinkDecals = true; // !!! used?
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = false;

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 1;
        vs.vehicleExaggeration = 1;
        /*
        vs.minVehicleColor = RGBColor(1,0,0);
        vs.maxVehicleColor = RGBColor(0,1,0);
        */
        vs.showBlinker = true;
        vs.drawcC2CRadius = true;
        vs.drawLaneChangePreference = false;
        vs.drawVehicleName = false;

        vs.junctionMode = 0;

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;

        vs.poiExaggeration = 1;
        vs.minPOISize = 0;
        gSchemeStorage.add("faster standard", vs);
    }
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.antialiase = true;
        vs.dither = true;

        vs.backgroundColor = RGBColor((SUMOReal) .2, (SUMOReal) .5, (SUMOReal) .2);
        vs.showLinkDecals = true;

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
//        vs.singleLaneColor = RGBColor((SUMOReal) .3, (SUMOReal) .3, (SUMOReal) .4);
        vs.laneShowBorders = true;
        vs.showLinkDecals = true;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = true;

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
        vs.drawcC2CRadius = true;
        vs.drawLaneChangePreference = false;
        vs.drawVehicleName = false;

        vs.junctionMode = 0;

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;

        vs.minPOISize = 0;
        vs.poiExaggeration = 1;

        gSchemeStorage.add("real world", vs);
    }
}

GUICompleteSchemeStorage gSchemeStorage;
