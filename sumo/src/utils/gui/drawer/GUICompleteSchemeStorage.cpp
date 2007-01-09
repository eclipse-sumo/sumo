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

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
        vs.laneShowBorders = false;
        vs.showLinkDecals = true;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = true;
        vs.drawEdgeName = false;
        vs.edgeNameSize = 50;

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 1;
        vs.vehicleExaggeration = 1;
        vs.showBlinker = true;
        vs.drawcC2CRadius = true;
        vs.drawLaneChangePreference = true;
        vs.drawVehicleName = false;
        vs.vehicleNameSize = 50;

        vs.junctionMode = 0;
        vs.drawLinkTLIndex = false;
        vs.drawLinkJunctionIndex = false;
        vs.drawJunctionName = false;
        vs.junctionNameSize = 50;

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;
        vs.drawAddName = false;
        vs.addNameSize = 50;

        vs.minPOISize = 0;
        vs.poiExaggeration = 1;
        vs.drawPOIName = false;
        vs.poiNameSize = 50;

        gSchemeStorage.add("standard", vs);
    }
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.antialiase = false;
        vs.dither = false;

        vs.backgroundColor = RGBColor(1, 1, 1);

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
        vs.laneShowBorders = false;
        vs.showLinkDecals = false;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = false;
        vs.drawEdgeName = false;
        vs.edgeNameSize = 50;

        vs.vehicleMode = 0;
        vs.vehicleColorings = vehColMap;
        vs.minVehicleSize = 1;
        vs.vehicleExaggeration = 1;
        vs.showBlinker = true;
        vs.drawcC2CRadius = true;
        vs.drawLaneChangePreference = false;
        vs.drawVehicleName = false;
        vs.vehicleNameSize = 50;

        vs.junctionMode = 0;
        vs.drawLinkTLIndex = false;
        vs.drawLinkJunctionIndex = false;
        vs.drawJunctionName = false;
        vs.junctionNameSize = 50;

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;
        vs.drawAddName = false;
        vs.addNameSize = 50;

        vs.poiExaggeration = 1;
        vs.minPOISize = 0;
        vs.drawPOIName = false;
        vs.poiNameSize = 50;

        gSchemeStorage.add("faster standard", vs);
    }
    {
        GUISUMOAbstractView::VisualizationSettings vs;
        vs.antialiase = true;
        vs.dither = true;

        vs.backgroundColor = RGBColor((SUMOReal) .2, (SUMOReal) .5, (SUMOReal) .2);

        vs.laneEdgeMode = 0;
        vs.laneColorings = laneColMap;
        vs.laneShowBorders = true;
        vs.showLinkDecals = true;
        vs.laneEdgeExaggMode = 0; // !!! used?
        vs.minExagg = 1; // !!! used?
        vs.maxExagg = 1; // !!! used?
        vs.showRails = true;
        vs.drawEdgeName = false;
        vs.edgeNameSize = 50;

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
        vs.vehicleNameSize = 50;

        vs.junctionMode = 0;
        vs.drawLinkTLIndex = false;
        vs.drawLinkJunctionIndex = false;
        vs.drawJunctionName = false;
        vs.junctionNameSize = 50;

        vs.showLane2Lane = false;

        vs.addMode = 0;
        vs.minAddSize = 1;
        vs.addExaggeration = 1;
        vs.drawAddName = false;
        vs.addNameSize = 50;

        vs.minPOISize = 0;
        vs.poiExaggeration = 1;
        vs.drawPOIName = false;
        vs.poiNameSize = 50;

        gSchemeStorage.add("real world", vs);
    }
}

GUICompleteSchemeStorage gSchemeStorage;
