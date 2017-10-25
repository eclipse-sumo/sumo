/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    TraCI_Simulation.cpp
/// @author  Laura Bieker-Walz
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <utils/common/StdDefs.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/MSStoppingPlace.h>
#include <traci-server/lib/TraCI.h>
#include "TraCI_Simulation.h"
#include <traci-server/TraCIDefs.h>

// ===========================================================================
// member definitions
// ===========================================================================
SUMOTime 
TraCI_Simulation::getCurrentTime(){
    return MSNet::getInstance()->getCurrentTimeStep();
}


SUMOTime 
TraCI_Simulation::getDeltaT(){
    return DELTA_T;
}


TraCIBoundary
TraCI_Simulation::getNetBoundary() {
	Boundary b = GeoConvHelper::getFinal().getConvBoundary();
	TraCIBoundary tb;
	tb.xMin = b.xmin();
	tb.xMax = b.xmax();
	tb.yMin = b.ymin();
	tb.yMax = b.ymax();
	tb.zMin = b.zmin();
	tb.zMax = b.zmax();
	return tb;
}

int
TraCI_Simulation::getMinExpectedNumber() {
    return MSNet::getInstance()->getVehicleControl().getActiveVehicleCount() + MSNet::getInstance()->getInsertionControl().getPendingFlowCount();
}


TraCIStage
TraCI_Simulation::findRoute(const std::string& from, const std::string& to, const std::string& typeID, const SUMOTime depart, const int routingMode) {
    TraCIStage result(MSTransportable::DRIVING);
    const MSEdge* const fromEdge = MSEdge::dictionary(from);
    if (fromEdge == 0) {
        throw TraCIException("Unknown from edge '" + from + "'.");
    }
    const MSEdge* const toEdge = MSEdge::dictionary(to);
    if (toEdge == 0) {
        throw TraCIException("Unknown to edge '" + from + "'.");
    }
    SUMOVehicle* vehicle = 0;
    if (typeID != "") {
        SUMOVehicleParameter* pars = new SUMOVehicleParameter();
        MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(typeID);
        if (type == 0) {
            throw TraCIException("The vehicle type '" + typeID + "' is not known.");
        }
        const MSRoute* const routeDummy = new MSRoute("", ConstMSEdgeVector({ fromEdge }), false, 0, std::vector<SUMOVehicleParameter::Stop>());
        vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(pars, routeDummy, type, false);
    }
    ConstMSEdgeVector edges;
    const SUMOTime dep = depart < 0 ? MSNet::getInstance()->getCurrentTimeStep() : depart;
    MSNet::getInstance()->getRouterTT().compute(fromEdge, toEdge, vehicle, dep, edges);
    for (const MSEdge* e : edges) {
        result.edges.push_back(e->getID());
    }
    result.travelTime = result.cost = MSNet::getInstance()->getRouterTT().recomputeCosts(edges, vehicle, dep);
    if (vehicle != 0) {
        MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
    }
    return result;
}


std::vector<TraCIStage>
TraCI_Simulation::findIntermodalRoute(const std::string& from, const std::string& to,
    const std::string modes, const SUMOTime depart, const int routingMode, const double speed, const double walkFactor,
    const double departPos, const double arrivalPos, const double departPosLat,
    const std::string& pType, const std::string& vehType) {
    std::vector<TraCIStage> result;
    const MSEdge* const fromEdge = MSEdge::dictionary(from);
    if (fromEdge == 0) {
        throw TraCIException("Unknown from edge '" + from + "'.");
    }
    const MSEdge* const toEdge = MSEdge::dictionary(to);
    if (toEdge == 0) {
        throw TraCIException("Unknown to edge '" + from + "'.");
    }
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    SVCPermissions modeSet = 0;
    for (StringTokenizer st(modes); st.hasNext();) {
        const std::string mode = st.next();
        if (mode == "car") {
            modeSet |= SVC_PASSENGER;
        } else if (mode == "bicycle") {
            modeSet |= SVC_BICYCLE;
        } else if (mode == "public") {
            modeSet |= SVC_BUS;
        } else {
            throw TraCIException("Unknown person mode '" + mode + "'.");
        }
    }
    const MSVehicleType* pedType = vehControl.getVType(pType);
    const SUMOTime dep = depart < 0 ? MSNet::getInstance()->getCurrentTimeStep() : depart;
    if (modeSet == 0) {
        ConstMSEdgeVector edges;
        const double cost = MSNet::getInstance()->getPedestrianRouter().compute(fromEdge, toEdge, departPos, arrivalPos, speed > 0 ? speed : pedType->getMaxSpeed(), dep, 0, edges);
        if (cost < 0) {
            return result;
        }
        result.emplace_back(TraCIStage(MSTransportable::MOVING_WITHOUT_VEHICLE));
        for (const MSEdge* e : edges) {
            result.back().edges.push_back(e->getID());
        }
        result.back().travelTime = result.back().cost = cost;
    } else {
        SUMOVehicleParameter* pars = new SUMOVehicleParameter();
        SUMOVehicle* vehicle = 0;
        if (vehType != "") {
            MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(vehType);
            if (type->getVehicleClass() != SVC_IGNORING && (fromEdge->getPermissions() & type->getVehicleClass()) == 0) {
                throw TraCIException("Invalid vehicle type '" + type->getID() + "', it is not allowed on the start edge.");
            }
            const MSRoute* const routeDummy = new MSRoute("", ConstMSEdgeVector({ fromEdge }), false, 0, std::vector<SUMOVehicleParameter::Stop>());
            vehicle = vehControl.buildVehicle(pars, routeDummy, type, !MSGlobals::gCheckRoutes);
        }
        std::vector<MSNet::MSIntermodalRouter::TripItem> items;
        if (MSNet::getInstance()->getIntermodalRouter().compute(fromEdge, toEdge, departPos, arrivalPos,
            pedType->getMaxSpeed() * walkFactor, vehicle, modeSet, dep, items)) {
            for (std::vector<MSNet::MSIntermodalRouter::TripItem>::iterator it = items.begin(); it != items.end(); ++it) {
                if (!it->edges.empty()) {
                    if (it->line == "") {
                        result.emplace_back(TraCIStage(MSTransportable::MOVING_WITHOUT_VEHICLE));
                    } else if (vehicle != 0 && it->line == vehicle->getID()) {
                        result.emplace_back(TraCIStage(MSTransportable::DRIVING));
                    }
                    result.back().destStop = it->destStop;
                    result.back().line = it->line;
                    for (const MSEdge* e : it->edges) {
                        result.back().edges.push_back(e->getID());
                    }
                    result.back().travelTime = result.back().cost = it->cost;
                }
            }
        }
        if (vehicle != 0) {
            vehControl.deleteVehicle(vehicle, true);
        }
    }
    return result;
}


std::string
TraCI_Simulation::getParameter(const std::string& objectID, const std::string& key) {
    if (StringUtils::startsWith(key, "chargingStation.")) {
        const std::string attrName = key.substr(16);
        MSChargingStation* cs = MSNet::getInstance()->getChargingStation(objectID);
        if (cs == 0) {
            throw TraCIException("Invalid chargingStation '" + objectID + "'");
        }
        if (attrName == toString(SUMO_ATTR_TOTALENERGYCHARGED)) {
            return toString(cs->getTotalCharged());
        } else {
            throw TraCIException("Invalid chargingStation parameter '" + attrName + "'");
        }
    } else {
        throw TraCIException("Parameter '" + key + "' is not supported.");
    }
}


#endif


/****************************************************************************/

