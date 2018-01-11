/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Simulation.cpp
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

#include <utils/common/StdDefs.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/MSStoppingPlace.h>
#include <netload/NLBuilder.h>
#include "Simulation.h"
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
/* void
Simulation::connect(const std::string& host, int port) {
}*/

void
Simulation::load(const std::vector<std::string>& args) {
    XMLSubSys::init(); // this may be not good for multiple loads
    OptionsIO::setArgs(args);
    NLBuilder::init();
}


void
Simulation::simulationStep(const SUMOTime time) {
    if (time == 0) {
        MSNet::getInstance()->simulationStep();
    } else {
        while (MSNet::getInstance()->getCurrentTimeStep() < time) {
            MSNet::getInstance()->simulationStep();
        }
    }
}


void
Simulation::close() {
}


/* void
Simulation::subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const {
}

void
Simulation::subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<
int>& vars) const {
} */


const Simulation::SubscribedValues
Simulation::getSubscriptionResults() const {
    return mySubscribedValues;
}


const Simulation::TraCIValues
Simulation::getSubscriptionResults(const std::string& objID) const {
    if (mySubscribedValues.find(objID) != mySubscribedValues.end()) {
        return mySubscribedValues.find(objID)->second;
    } else {
        return TraCIValues();
    }
}


const Simulation::SubscribedContextValues
Simulation::getContextSubscriptionResults() const {
    return mySubscribedContextValues;
}


const Simulation::SubscribedValues
Simulation::getContextSubscriptionResults(const std::string& objID) const {
    if (mySubscribedContextValues.find(objID) != mySubscribedContextValues.end()) {
        return mySubscribedContextValues.find(objID)->second;
    } else {
        return SubscribedValues();
    }
}


SUMOTime
Simulation::getCurrentTime() {
    return MSNet::getInstance()->getCurrentTimeStep();
}


SUMOTime
Simulation::getDeltaT() {
    return DELTA_T;
}


TraCIBoundary
Simulation::getNetBoundary() {
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
Simulation::getMinExpectedNumber() {
    return MSNet::getInstance()->getVehicleControl().getActiveVehicleCount() + MSNet::getInstance()->getInsertionControl().getPendingFlowCount();
}


TraCIStage
Simulation::findRoute(const std::string& from, const std::string& to, const std::string& typeID, const SUMOTime depart, const int routingMode) {
    UNUSED_PARAMETER(routingMode);
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
Simulation::findIntermodalRoute(const std::string& from, const std::string& to,
                                const std::string& modes, const SUMOTime depart, const int routingMode, const double speed, const double walkFactor,
                                const double departPos, const double arrivalPos, const double departPosLat,
                                const std::string& pType, const std::string& vehType) {
    UNUSED_PARAMETER(routingMode);
    UNUSED_PARAMETER(departPosLat);
    std::vector<TraCIStage> result;
    const MSEdge* const fromEdge = MSEdge::dictionary(from);
    if (fromEdge == 0) {
        throw TraCIException("Unknown from edge '" + from + "'.");
    }
    const MSEdge* const toEdge = MSEdge::dictionary(to);
    if (toEdge == 0) {
        throw TraCIException("Unknown to edge '" + to + "'.");
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
    const MSVehicleType* pedType = vehControl.hasVType(pType) ? vehControl.getVType(pType) : vehControl.getVType(DEFAULT_PEDTYPE_ID);
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
Simulation::getParameter(const std::string& objectID, const std::string& key) {
    if (StringUtils::startsWith(key, "chargingStation.")) {
        const std::string attrName = key.substr(16);
        MSChargingStation* cs = static_cast<MSChargingStation*>(MSNet::getInstance()->getStoppingPlace(objectID, SUMO_TAG_CHARGING_STATION));
        if (cs == 0) {
            throw TraCIException("Invalid chargingStation '" + objectID + "'");
        }
        if (attrName == toString(SUMO_ATTR_TOTALENERGYCHARGED)) {
            return toString(cs->getTotalCharged());
        } else {
            throw TraCIException("Invalid chargingStation parameter '" + attrName + "'");
        }
    } else if (StringUtils::startsWith(key, "parkingArea.")) {
        const std::string attrName = key.substr(12);
        MSParkingArea* pa = static_cast<MSParkingArea*>(MSNet::getInstance()->getStoppingPlace(objectID, SUMO_TAG_PARKING_AREA));
        if (pa == 0) {
            throw TraCIException("Invalid parkingArea '" + objectID + "'");
        }
        if (attrName == "capacity") {
            return toString(pa->getCapacity());
        } else if (attrName == "occupancy") {
            return toString(pa->getOccupancy());
        } else {
            throw TraCIException("Invalid parkingArea parameter '" + attrName + "'");
        }
    } else {
        throw TraCIException("Parameter '" + key + "' is not supported.");
    }
}
}


/****************************************************************************/
