/****************************************************************************/
/// @file    MSFullExport.cpp
/// @author  Mario Krumnow
/// @date    2012-04-26
/// @version $Id$
///
// Dumping a hugh List of Parameters available in the Simulation
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
    of.openTag("data") << " timestep=\"" << time2string(timestep) << "\"";
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
    of.openTag("vehicles");
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();
    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);
        if (veh->isOnRoad()) {
            std::string fclass = veh->getVehicleType().getID();
            fclass = fclass.substr(0, fclass.find_first_of("@"));
            Position pos = veh->getLane()->getShape().positionAtOffset(veh->getPositionOnLane());
            of.openTag("vehicle").writeAttr("id", veh->getID()).writeAttr("eclass", toString(veh->getVehicleType().getEmissionClass()));
            of.writeAttr("CO2", veh->getCO2Emissions()).writeAttr("CO", veh->getCOEmissions()).writeAttr("HC", veh->getHCEmissions());
            of.writeAttr("NOx", veh->getNOxEmissions()).writeAttr("PMx", veh->getPMxEmissions()).writeAttr("noise", veh->getHarmonoise_NoiseEmissions());
            of.writeAttr("route", veh->getRoute().getID()).writeAttr("type", fclass).writeAttr("waiting", veh->getWaitingSeconds());
            of.writeAttr("lane", veh->getLane()->getID()).writeAttr("pos_lane", veh->getPositionOnLane()).writeAttr("speed", veh->getSpeed());
            of.writeAttr("angle", veh->getAngle()).writeAttr("x", pos.x()).writeAttr("y", pos.y());
            // !!! fuel is missing here
            of.closeTag();
        }
    }
    of.closeTag();
}

void
MSFullExport::writeEdge(OutputDevice& of) {
    of.openTag("edges");
    MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
    const std::vector<MSEdge*>& edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        MSEdge& edge = **e;
        of.openTag("edge").writeAttr("id", edge.getID()).writeAttr("traveltime", edge.getCurrentTravelTime());
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

    of.openTag("lane").writeAttr("id", lane.getID()).writeAttr("CO", lane.getCOEmissions()).writeAttr("CO2", lane.getCO2Emissions());
    of.writeAttr("NOx", lane.getNOxEmissions()).writeAttr("PMx", lane.getPMxEmissions()).writeAttr("HC", lane.getHCEmissions());
    of.writeAttr("noise", lane.getHarmonoise_NoiseEmissions()).writeAttr("fuel", lane.getFuelConsumption()).writeAttr("maxspeed", lane.getSpeedLimit());
    of.writeAttr("meanspeed", lane.getMeanSpeed() * 3.6).writeAttr("occupancy", lane.getNettoOccupancy()).writeAttr("vehicle_count", lane.getVehicleNumber());
    of.closeTag();
}


void
MSFullExport::writeTLS(OutputDevice& of, SUMOTime /* timestep */) {
    of.openTag("tls");
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
        of.openTag("trafficlight").writeAttr("id", *id_it).writeAttr("state", state).closeTag();
    }
    of.closeTag();
}

