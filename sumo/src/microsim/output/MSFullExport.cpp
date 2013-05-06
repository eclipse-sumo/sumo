/****************************************************************************/
/// @file    MSFullExport.cpp
/// @author  Mario Krumnow
/// @version $Id$
///
// Dumping a hugh List of Parameters available in the Simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSFullExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSFullExport::write(OutputDevice& of, SUMOTime timestep) {

    of.openTag("data") << " timestep=\"" << time2string(timestep) << "\">\n";

    //Vehicles
    writeVehicles(of);

    //Edges
    writeEdge(of);

    //TrafficLights
    writeTLS(of, timestep);


    of.closeTag();
}

void
MSFullExport::writeVehicles(OutputDevice& of) {

    of.openTag("vehicles") << ">\n";

    const std::string indent("    ");
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();


    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);

        if (veh->isOnRoad()) {

            std::string fclass = veh->getVehicleType().getID();
            fclass = fclass.substr(0, fclass.find_first_of("@"));

            Position pos = veh->getLane()->getShape().positionAtOffset(veh->getPositionOnLane());

            of.openTag("vehicle") << " id=\"" << veh->getID() << "\" eclass=\"" <<  veh->getVehicleType().getEmissionClass() << "\" co2=\"" << veh->getHBEFA_CO2Emissions()
                                  << "\" co=\"" <<  veh->getHBEFA_COEmissions() << "\" hc=\"" <<  veh->getHBEFA_HCEmissions()
                                  << "\" nox=\"" <<  veh->getHBEFA_NOxEmissions() << "\" pmx=\"" <<  veh->getHBEFA_PMxEmissions()
                                  << "\" noise=\"" <<  veh->getHarmonoise_NoiseEmissions() << "\" route=\"" << veh->getRoute().getID()
                                  << "\" type=\"" <<  fclass << "\" waiting=\"" <<  veh->getWaitingSeconds()
                                  << "\" lane=\"" <<  veh->getLane()->getID()
                                  << "\" pos_lane=\"" << veh->getPositionOnLane() << "\" speed=\"" << veh->getSpeed() * 3.6
                                  << "\" angle=\"" << veh->getAngle() << "\" x=\"" << pos.x() << "\" y=\"" << pos.y() << "\"";

            of.closeTag();

        }

    }

    of.closeTag();



}

void
MSFullExport::writeEdge(OutputDevice& of) {

    of.openTag("edges")  << ">\n";

    MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();

    const std::vector<MSEdge*>& edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {

        MSEdge& edge = **e;

        of.openTag("edge") << " id=\"" << edge.getID() << "\" traveltime=\"" << edge.getCurrentTravelTime() << "\">\n";

        const std::vector<MSLane*>& lanes = edge.getLanes();
        for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {

            writeLane(of, **lane);

        }

        of.closeTag();

    }

    of.closeTag();

}

void
MSFullExport::writeLane(OutputDevice& of, const MSLane& lane) {

    of.openTag("lane")
            << " id=\"" << lane.getID()
            << "\" co=\"" << lane.getHBEFA_COEmissions()
            << "\" co2=\"" << lane.getHBEFA_CO2Emissions()
            << "\" nox=\"" << lane.getHBEFA_NOxEmissions()
            << "\" pmx=\"" << lane.getHBEFA_PMxEmissions()
            << "\" hc=\"" << lane.getHBEFA_HCEmissions()
            << "\" noise=\"" << lane.getHarmonoise_NoiseEmissions()
            << "\" fuel=\"" << lane.getHBEFA_FuelConsumption()
            << "\" maxspeed=\"" << lane.getSpeedLimit() * 3.6
            << "\" meanspeed=\"" << lane.getMeanSpeed() * 3.6
            << "\" occupancy=\"" << lane.getOccupancy()
            << "\" vehicle_count=\"" << lane.getVehicleNumber() << "\"";

    of.closeTag();

}

void
MSFullExport::writeTLS(OutputDevice& of, SUMOTime /* timestep */) {
    of.openTag("tls") << ">\n";
    MSTLLogicControl& vc = MSNet::getInstance()->getTLSControl();
    std::vector<std::string> ids = vc.getAllTLIds();
    for (std::vector<std::string>::const_iterator id_it = ids.begin(); id_it != ids.end(); ++id_it) {
        MSTLLogicControl::TLSLogicVariants& vars = MSNet::getInstance()->getTLSControl().get(*id_it);
        const MSTrafficLightLogic::LaneVectorVector& lanes = vars.getActive()->getLanes();

        std::vector<std::string> laneIDs;
        for (MSTrafficLightLogic::LaneVectorVector::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            const MSTrafficLightLogic::LaneVector& llanes = (*i);
            for (MSTrafficLightLogic::LaneVector::const_iterator j = llanes.begin(); j != llanes.end(); ++j) {
                laneIDs.push_back((*j)->getID());
            }
        }

        std::string lane_output = "";
        for (unsigned int i1 = 0; i1 < laneIDs.size(); ++i1) {
            lane_output += laneIDs[i1] + " ";
        }

        std::string state = vars.getActive()->getCurrentPhaseDef().getState();
        of.openTag("trafficlight") << " id=\"" << *id_it << "\" state=\"" << state << "\"";
        of.closeTag();

    }

    of.closeTag();

}
