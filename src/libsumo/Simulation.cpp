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
#include <config.h>

#include <utils/options/OptionsCont.h>
#include <utils/common/StdDefs.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/SystemFrame.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/options/OptionsIO.h>
#include <utils/vehicle/IntermodalRouter.h>
#include <utils/vehicle/PedestrianRouter.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <netload/NLBuilder.h>
#include <traci-server/TraCIConstants.h>
#include "Simulation.h"
#include <libsumo/TraCIDefs.h>


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Simulation::mySubscriptionResults;
ContextSubscriptionResults Simulation::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
void
Simulation::load(const std::vector<std::string>& args) {
    close();
    XMLSubSys::init();
    OptionsIO::setArgs(args);
    NLBuilder::init();
}


bool
Simulation::isLoaded() {
    return MSNet::hasInstance();
}


void
Simulation::step(const SUMOTime time) {
    if (time == 0) {
        MSNet::getInstance()->simulationStep();
    } else {
        while (MSNet::getInstance()->getCurrentTimeStep() < time) {
            MSNet::getInstance()->simulationStep();
        }
    }
    Helper::handleSubscriptions(time);
}


void
Simulation::close() {
    if (MSNet::hasInstance()) {
        MSNet::getInstance()->closeSimulation(0);
        delete MSNet::getInstance();
        XMLSubSys::close();
        SystemFrame::close();
    }
}


void
Simulation::subscribe(const std::string& objID, const std::vector<int>& vars, SUMOTime beginTime, SUMOTime endTime) {
    libsumo::Helper::subscribe(CMD_SUBSCRIBE_SIM_VARIABLE, objID, vars, beginTime, endTime);
}

void
Simulation::subscribeContext(const std::string& objID, int domain, double range, const std::vector<int>& vars, SUMOTime beginTime, SUMOTime endTime) {
    libsumo::Helper::subscribe(CMD_SUBSCRIBE_SIM_CONTEXT, objID, vars, beginTime, endTime, domain, range);
}


const SubscriptionResults
Simulation::getSubscriptionResults() {
    return mySubscriptionResults;
}


const TraCIResults
Simulation::getSubscriptionResults(const std::string& objID) {
    return mySubscriptionResults[objID];
}


const ContextSubscriptionResults
Simulation::getContextSubscriptionResults() {
    return myContextSubscriptionResults;
}


const SubscriptionResults
Simulation::getContextSubscriptionResults(const std::string& objID) {
    return myContextSubscriptionResults[objID];
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
    MSNet* net = MSNet::getInstance();
    return (net->getVehicleControl().getActiveVehicleCount() 
            + net->getInsertionControl().getPendingFlowCount()
            + (net->hasPersons() ? net->getPersonControl().getActiveCount() : 0)
            + (net->hasContainers() ? net->getContainerControl().getActiveCount() : 0));
}


TraCIPosition 
Simulation::convert3D(const std::string& edgeID, double pos, int laneIndex, bool toGeo) {
    Position result = Helper::getLaneChecking(edgeID, laneIndex, pos)->getShape().positionAtOffset(pos);
    if (toGeo) {
        GeoConvHelper::getFinal().cartesian2geo(result);
    }
    return Helper::makeTraCIPosition(result);
}


TraCIStage
Simulation::findRoute(const std::string& from, const std::string& to, const std::string& typeID, const SUMOTime depart, const int routingMode) {
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
        try {
            const MSRoute* const routeDummy = new MSRoute("", ConstMSEdgeVector({ fromEdge }), false, 0, std::vector<SUMOVehicleParameter::Stop>());
            vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(pars, routeDummy, type, false);
        } catch (ProcessError& e) {
            throw TraCIException("Invalid departure edge for vehicle type '" + typeID + "' (" + e.what() + ")");
        }
    }
    ConstMSEdgeVector edges;
    const SUMOTime dep = depart < 0 ? MSNet::getInstance()->getCurrentTimeStep() : depart;
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = routingMode == ROUTING_MODE_AGGREGATED ? MSDevice_Routing::getRouterTT() : MSNet::getInstance()->getRouterTT();
    router.compute(fromEdge, toEdge, vehicle, dep, edges);
    for (const MSEdge* e : edges) {
        result.edges.push_back(e->getID());
    }
    result.travelTime = result.cost = router.recomputeCosts(edges, vehicle, dep);
    if (vehicle != 0) {
        MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
    }
    return result;
}


std::vector<TraCIStage>
Simulation::findIntermodalRoute(const std::string& from, const std::string& to,
                                const std::string& modes, SUMOTime depart, const int routingMode, double speed, double walkFactor,
                                double departPos, double arrivalPos, const double departPosLat,
                                const std::string& pType, const std::string& vType, const std::string& destStop) {
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
    std::vector<SUMOVehicleParameter*> pars;
    if (vType != "") {
        pars.push_back(new SUMOVehicleParameter());
        pars.back()->vtypeid = vType;
        pars.back()->id = vType;
        modeSet |= SVC_PASSENGER;
    }
    for (StringTokenizer st(modes); st.hasNext();) {
        const std::string mode = st.next();
        if (mode == "car") {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_VTYPE_ID;
            pars.back()->id = mode;
            modeSet |= SVC_PASSENGER;
        } else if (mode == "bicycle") {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_BIKETYPE_ID;
            pars.back()->id = mode;
            modeSet |= SVC_BICYCLE;
        } else if (mode == "public") {
            pars.push_back(nullptr);
            modeSet |= SVC_BUS;
        } else {
            throw TraCIException("Unknown person mode '" + mode + "'.");
        }
    }
    if (pars.empty()) {
        pars.push_back(nullptr);
    }
    // interpret default arguments
    const MSVehicleType* pedType = vehControl.hasVType(pType) ? vehControl.getVType(pType) : vehControl.getVType(DEFAULT_PEDTYPE_ID);
    if (depart < 0) {
        depart = MSNet::getInstance()->getCurrentTimeStep();
    }
    if (speed < 0) {
        speed =  pedType->getMaxSpeed();
    }
    if (walkFactor < 0) {
        walkFactor = OptionsCont::getOptions().getFloat("persontrip.walkfactor");
    }
    if (departPos < 0) {
        departPos += fromEdge->getLength();
    }
    if (arrivalPos == INVALID_DOUBLE_VALUE) {
        arrivalPos = toEdge->getLength() / 2;
    } else if (arrivalPos < 0) {
        arrivalPos += toEdge->getLength();
    }
    if (departPos < 0 || departPos >= fromEdge->getLength()) {
        throw TraCIException("Invalid depart position " + toString(departPos) + " for edge '" + to + "'.");
    }
    if (arrivalPos < 0 || arrivalPos >= toEdge->getLength()) {
        throw TraCIException("Invalid arrival position " + toString(arrivalPos) + " for edge '" + to + "'.");
    }
    double minCost = std::numeric_limits<double>::max();
    for (SUMOVehicleParameter* vehPar : pars) {
        std::vector<TraCIStage> resultCand;
        SUMOVehicle* vehicle = nullptr;
        if (vehPar != nullptr) {
            MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(vehPar->vtypeid);
            if (type == 0) {
                throw TraCIException("Unknown vehicle type '" + vehPar->vtypeid + "'.");
            }
            if (type->getVehicleClass() != SVC_IGNORING && (fromEdge->getPermissions() & type->getVehicleClass()) == 0) {
                WRITE_WARNING("Ignoring vehicle type '" + type->getID() + "' when performing intermodal routing because it is not allowed on the start edge '" + from + "'.");
            } else {
                const MSRoute* const routeDummy = new MSRoute(vehPar->id, ConstMSEdgeVector({ fromEdge }), false, 0, std::vector<SUMOVehicleParameter::Stop>());
                vehicle = vehControl.buildVehicle(vehPar, routeDummy, type, !MSGlobals::gCheckRoutes);
            }
        }
        std::vector<MSNet::MSIntermodalRouter::TripItem> items;
        if (MSNet::getInstance()->getIntermodalRouter().compute(fromEdge, toEdge, departPos, arrivalPos, destStop,
                    speed * walkFactor, vehicle, modeSet, depart, items)) {
            double cost = 0;
            for (std::vector<MSNet::MSIntermodalRouter::TripItem>::iterator it = items.begin(); it != items.end(); ++it) {
                if (!it->edges.empty()) {
                    resultCand.push_back(TraCIStage(it->line == "" 
                                ? MSTransportable::MOVING_WITHOUT_VEHICLE
                                : MSTransportable::DRIVING));
                    resultCand.back().destStop = it->destStop;
                    resultCand.back().line = it->line;
                    for (const MSEdge* e : it->edges) {
                        resultCand.back().edges.push_back(e->getID());
                    }
                    resultCand.back().travelTime = resultCand.back().cost = it->cost;
                    resultCand.back().intended = it->intended;
                    resultCand.back().depart = it->depart;
                }
                cost += it->cost;
            }
            if (cost < minCost) {
                minCost = cost;
                result = resultCand;
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
