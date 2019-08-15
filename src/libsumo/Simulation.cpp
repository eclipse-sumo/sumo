/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
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
#include <utils/router/IntermodalRouter.h>
#include <utils/router/PedestrianRouter.h>
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
#include <microsim/MSParkingArea.h>
#include <microsim/devices/MSRoutingEngine.h>
#include <netload/NLBuilder.h>
#include <libsumo/TraCIConstants.h>
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
    Helper::registerVehicleStateListener();
}


bool
Simulation::isLoaded() {
    return MSNet::hasInstance();
}


void
Simulation::step(const double time) {
    Helper::clearVehicleStates();
    const SUMOTime t = TIME2STEPS(time);
    if (t == 0) {
        MSNet::getInstance()->simulationStep();
    } else {
        while (MSNet::getInstance()->getCurrentTimeStep() < t) {
            MSNet::getInstance()->simulationStep();
        }
    }
    Helper::handleSubscriptions(t);
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


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(Simulation, SIM)


void
Simulation::subscribe(const std::vector<int>& vars, double beginTime, double endTime) {
    libsumo::Helper::subscribe(CMD_SUBSCRIBE_SIM_VARIABLE, "", vars, beginTime, endTime);
}


const TraCIResults
Simulation::getSubscriptionResults() {
    return mySubscriptionResults[""];
}


int
Simulation::getCurrentTime() {
    return (int)MSNet::getInstance()->getCurrentTimeStep();
}


double
Simulation::getTime() {
    return SIMTIME;
}


int
Simulation::getLoadedNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_BUILT).size();
}


std::vector<std::string>
Simulation::getLoadedIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_BUILT);
}


int
Simulation::getDepartedNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_DEPARTED).size();
}


std::vector<std::string>
Simulation::getDepartedIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_DEPARTED);
}


int
Simulation::getArrivedNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ARRIVED).size();
}


std::vector<std::string>
Simulation::getArrivedIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ARRIVED);
}


int
Simulation::getParkingStartingVehiclesNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_STARTING_PARKING).size();
}


std::vector<std::string>
Simulation::getParkingStartingVehiclesIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_STARTING_PARKING);
}


int
Simulation::getParkingEndingVehiclesNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ENDING_PARKING).size();
}


std::vector<std::string>
Simulation::getParkingEndingVehiclesIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ENDING_PARKING);
}


int
Simulation::getStopStartingVehiclesNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_STARTING_STOP).size();
}


std::vector<std::string>
Simulation::getStopStartingVehiclesIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_STARTING_STOP);
}


int
Simulation::getStopEndingVehiclesNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ENDING_STOP).size();
}


std::vector<std::string>
Simulation::getStopEndingVehiclesIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ENDING_STOP);
}


int
Simulation::getCollidingVehiclesNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_COLLISION).size();
}


std::vector<std::string>
Simulation::getCollidingVehiclesIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_COLLISION);
}


int
Simulation::getEmergencyStoppingVehiclesNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_EMERGENCYSTOP).size();
}


std::vector<std::string>
Simulation::getEmergencyStoppingVehiclesIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_EMERGENCYSTOP);
}


int
Simulation::getStartingTeleportNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_STARTING_TELEPORT).size();
}


std::vector<std::string>
Simulation::getStartingTeleportIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_STARTING_TELEPORT);
}


int
Simulation::getEndingTeleportNumber() {
    return (int)Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ENDING_TELEPORT).size();
}


std::vector<std::string>
Simulation::getEndingTeleportIDList() {
    return Helper::getVehicleStateChanges(MSNet::VEHICLE_STATE_ENDING_TELEPORT);
}

std::vector<std::string>
Simulation::getBusStopIDList() {
    std::vector<std::string> result;
    for (const auto pair : MSNet::getInstance()->getStoppingPlaces(SUMO_TAG_BUS_STOP)) {
        result.push_back(pair.first);
    }
    return result;
}

int
Simulation::getBusStopWaiting(const std::string& id) {
    MSStoppingPlace* s = MSNet::getInstance()->getStoppingPlace(id, SUMO_TAG_BUS_STOP);
    if (s == nullptr) {
        throw TraCIException("Unknown bus stop '" + id + "'.");
    }
    return s->getTransportableNumber();
}

std::vector<std::string>
Simulation::getBusStopWaitingIDList(const std::string& id) {
    MSStoppingPlace* s = MSNet::getInstance()->getStoppingPlace(id, SUMO_TAG_BUS_STOP);
    std::vector<MSTransportable*> transportables = s->getTransportables();
    std::vector<std::string> result;
    for (std::vector<MSTransportable*>::iterator it = transportables.begin(); it != transportables.end(); it++) {
        result.push_back((*it)->getID());
    }
    return result;
}


double
Simulation::getDeltaT() {
    return TS;
}


TraCIPositionVector
Simulation::getNetBoundary() {
    Boundary b = GeoConvHelper::getFinal().getConvBoundary();
    TraCIPositionVector tb({ TraCIPosition(), TraCIPosition() });
    tb[0].x = b.xmin();
    tb[1].x = b.xmax();
    tb[0].y = b.ymin();
    tb[1].y = b.ymax();
    tb[0].z = b.zmin();
    tb[1].z = b.zmax();
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
Simulation::convert2D(const std::string& edgeID, double pos, int laneIndex, bool toGeo) {
    Position result = Helper::getLaneChecking(edgeID, laneIndex, pos)->geometryPositionAtOffset(pos);
    if (toGeo) {
        GeoConvHelper::getFinal().cartesian2geo(result);
    }
    result.setz(0.);
    return Helper::makeTraCIPosition(result);
}


TraCIPosition
Simulation::convert3D(const std::string& edgeID, double pos, int laneIndex, bool toGeo) {
    Position result = Helper::getLaneChecking(edgeID, laneIndex, pos)->geometryPositionAtOffset(pos);
    if (toGeo) {
        GeoConvHelper::getFinal().cartesian2geo(result);
    }
    return Helper::makeTraCIPosition(result, true);
}


TraCIRoadPosition
Simulation::convertRoad(double x, double y, bool isGeo, const std::string& vClass) {
    Position pos(x, y);
    if (isGeo) {
        GeoConvHelper::getFinal().x2cartesian_const(pos);
    }
    if (!SumoVehicleClassStrings.hasString(vClass)) {
        throw TraCIException("Unknown vehicle class '" + vClass + "'.");
    }
    const SUMOVehicleClass vc = SumoVehicleClassStrings.get(vClass);
    std::pair<MSLane*, double> roadPos = libsumo::Helper::convertCartesianToRoadMap(pos, vc);
    if (roadPos.first == nullptr) {
        throw TraCIException("Cannot convert position to road.");
    }
    TraCIRoadPosition result;
    result.edgeID = roadPos.first->getEdge().getID();
    result.laneIndex = roadPos.first->getIndex();
    result.pos = roadPos.second;
    return result;
}


TraCIPosition
Simulation::convertGeo(double x, double y, bool fromGeo) {
    Position pos(x, y);
    if (fromGeo) {
        GeoConvHelper::getFinal().x2cartesian_const(pos);
    } else {
        GeoConvHelper::getFinal().cartesian2geo(pos);
    }
    return Helper::makeTraCIPosition(pos);
}


double
Simulation::getDistance2D(double x1, double y1, double x2, double y2, bool isGeo, bool isDriving) {
    Position pos1(x1, y1);
    Position pos2(x2, y2);
    if (isGeo) {
        GeoConvHelper::getFinal().x2cartesian_const(pos1);
        GeoConvHelper::getFinal().x2cartesian_const(pos2);
    }
    if (isDriving) {
        std::pair<const MSLane*, double> roadPos1 = libsumo::Helper::convertCartesianToRoadMap(pos1, SVC_IGNORING);
        std::pair<const MSLane*, double> roadPos2 = libsumo::Helper::convertCartesianToRoadMap(pos2, SVC_IGNORING);
        if ((roadPos1.first == roadPos2.first) && (roadPos1.second <= roadPos2.second)) {
            // same edge
            return roadPos2.second - roadPos1.second;
        } else {
            double distance = 0.;
            ConstMSEdgeVector newRoute;
            if (roadPos2.first->isInternal()) {
                distance = roadPos2.second;
                roadPos2.first = roadPos2.first->getLogicalPredecessorLane();
                roadPos2.second = roadPos2.first->getLength();
            }
            MSNet::getInstance()->getRouterTT().compute(
                &roadPos1.first->getEdge(), &roadPos2.first->getEdge(), nullptr, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
            MSRoute route("", newRoute, false, nullptr, std::vector<SUMOVehicleParameter::Stop>());
            return distance + route.getDistanceBetween(roadPos1.second, roadPos2.second, &roadPos1.first->getEdge(), &roadPos2.first->getEdge());
        }
    } else {
        return pos1.distanceTo(pos2);
    }
}


double
Simulation::getDistanceRoad(const std::string& edgeID1, double pos1, const std::string& edgeID2, double pos2, bool isDriving) {
    std::pair<const MSLane*, double> roadPos1 = std::make_pair(libsumo::Helper::getLaneChecking(edgeID1, 0, pos1), pos1);
    std::pair<const MSLane*, double> roadPos2 = std::make_pair(libsumo::Helper::getLaneChecking(edgeID2, 0, pos2), pos2);
    if (isDriving) {
        if ((roadPos1.first == roadPos2.first) && (roadPos1.second <= roadPos2.second)) {
            // same edge
            return roadPos2.second - roadPos1.second;
        } else {
            double distance = 0.;
            ConstMSEdgeVector newRoute;
            if (roadPos2.first->isInternal()) {
                distance = roadPos2.second;
                roadPos2.first = roadPos2.first->getLogicalPredecessorLane();
                roadPos2.second = roadPos2.first->getLength();
            }
            MSNet::getInstance()->getRouterTT().compute(
                &roadPos1.first->getEdge(), &roadPos2.first->getEdge(), nullptr, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
            MSRoute route("", newRoute, false, nullptr, std::vector<SUMOVehicleParameter::Stop>());
            return distance + route.getDistanceBetween(roadPos1.second, roadPos2.second, &roadPos1.first->getEdge(), &roadPos2.first->getEdge());
        }
    } else {
        const Position pos1 = roadPos1.first->geometryPositionAtOffset(roadPos1.second);
        const Position pos2 = roadPos2.first->geometryPositionAtOffset(roadPos2.second);
        return pos1.distanceTo(pos2);
    }
}


TraCIStage
Simulation::findRoute(const std::string& from, const std::string& to, const std::string& typeID, const double depart, const int routingMode) {
    TraCIStage result(MSTransportable::DRIVING);
    const MSEdge* const fromEdge = MSEdge::dictionary(from);
    if (fromEdge == nullptr) {
        throw TraCIException("Unknown from edge '" + from + "'.");
    }
    const MSEdge* const toEdge = MSEdge::dictionary(to);
    if (toEdge == nullptr) {
        throw TraCIException("Unknown to edge '" + from + "'.");
    }
    SUMOVehicle* vehicle = nullptr;
    if (typeID != "") {
        SUMOVehicleParameter* pars = new SUMOVehicleParameter();
        MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(typeID);
        if (type == nullptr) {
            throw TraCIException("The vehicle type '" + typeID + "' is not known.");
        }
        try {
            const MSRoute* const routeDummy = new MSRoute("", ConstMSEdgeVector({ fromEdge }), false, nullptr, std::vector<SUMOVehicleParameter::Stop>());
            vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(pars, routeDummy, type, false);
            // we need to fix the speed factor here for deterministic results
            vehicle->setChosenSpeedFactor(type->getSpeedFactor().getParameter()[0]);
        } catch (ProcessError& e) {
            throw TraCIException("Invalid departure edge for vehicle type '" + typeID + "' (" + e.what() + ")");
        }
    }
    ConstMSEdgeVector edges;
    const SUMOTime dep = depart < 0 ? MSNet::getInstance()->getCurrentTimeStep() : TIME2STEPS(depart);
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = routingMode == ROUTING_MODE_AGGREGATED ? MSRoutingEngine::getRouterTT() : MSNet::getInstance()->getRouterTT();
    router.compute(fromEdge, toEdge, vehicle, dep, edges);
    for (const MSEdge* e : edges) {
        result.edges.push_back(e->getID());
    }
    result.travelTime = result.cost = router.recomputeCosts(edges, vehicle, dep, &result.length);
    if (vehicle != nullptr) {
        MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
    }
    return result;
}


std::vector<TraCIStage>
Simulation::findIntermodalRoute(const std::string& from, const std::string& to,
                                const std::string& modes, double depart, const int routingMode, double speed, double walkFactor,
                                double departPos, double arrivalPos, const double departPosLat,
                                const std::string& pType, const std::string& vType, const std::string& destStop) {
    UNUSED_PARAMETER(departPosLat);
    std::vector<TraCIStage> result;
    const MSEdge* const fromEdge = MSEdge::dictionary(from);
    if (fromEdge == nullptr) {
        throw TraCIException("Unknown from edge '" + from + "'.");
    }
    const MSEdge* const toEdge = MSEdge::dictionary(to);
    if (toEdge == nullptr) {
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
        if (mode == toString(PERSONMODE_CAR)) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_VTYPE_ID;
            pars.back()->id = mode;
            modeSet |= SVC_PASSENGER;
        } else if (mode == toString(PERSONMODE_BICYCLE)) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_BIKETYPE_ID;
            pars.back()->id = mode;
            modeSet |= SVC_BICYCLE;
        } else if (mode == toString(PERSONMODE_PUBLIC)) {
            pars.push_back(nullptr);
            modeSet |= SVC_BUS;
        } else if (mode == toString(PERSONMODE_WALK)) {
            // do nothing
        } else {
            throw TraCIException("Unknown person mode '" + mode + "'.");
        }
    }
    if (pars.empty()) {
        pars.push_back(nullptr);
    }
    // interpret default arguments
    const MSVehicleType* pedType = vehControl.hasVType(pType) ? vehControl.getVType(pType) : vehControl.getVType(DEFAULT_PEDTYPE_ID);
    SUMOTime departStep = TIME2STEPS(depart);
    if (depart < 0) {
        departStep = MSNet::getInstance()->getCurrentTimeStep();
    }
    if (speed < 0) {
        speed =  pedType->getMaxSpeed();
    }
    if (walkFactor < 0) {
        walkFactor = OptionsCont::getOptions().getFloat("persontrip.walkfactor");
    }
    const double externalFactor = StringUtils::toDouble(pedType->getParameter().getParameter("externalEffortFactor", "100"));
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
    MSNet::MSIntermodalRouter& router = MSNet::getInstance()->getIntermodalRouter(routingMode);
    for (SUMOVehicleParameter* vehPar : pars) {
        std::vector<TraCIStage> resultCand;
        SUMOVehicle* vehicle = nullptr;
        if (vehPar != nullptr) {
            MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(vehPar->vtypeid);
            if (type == nullptr) {
                throw TraCIException("Unknown vehicle type '" + vehPar->vtypeid + "'.");
            }
            if (type->getVehicleClass() != SVC_IGNORING && (fromEdge->getPermissions() & type->getVehicleClass()) == 0) {
                WRITE_WARNING("Ignoring vehicle type '" + type->getID() + "' when performing intermodal routing because it is not allowed on the start edge '" + from + "'.");
            } else {
                const MSRoute* const routeDummy = new MSRoute(vehPar->id, ConstMSEdgeVector({ fromEdge }), false, nullptr, std::vector<SUMOVehicleParameter::Stop>());
                vehicle = vehControl.buildVehicle(vehPar, routeDummy, type, !MSGlobals::gCheckRoutes);
                // we need to fix the speed factor here for deterministic results
                vehicle->setChosenSpeedFactor(type->getSpeedFactor().getParameter()[0]);
            }
        }
        std::vector<MSNet::MSIntermodalRouter::TripItem> items;
        if (router.compute(fromEdge, toEdge, departPos, arrivalPos, destStop,
                           speed * walkFactor, vehicle, modeSet, departStep, items, externalFactor)) {
            double cost = 0;
            for (std::vector<MSNet::MSIntermodalRouter::TripItem>::iterator it = items.begin(); it != items.end(); ++it) {
                if (!it->edges.empty()) {
                    resultCand.push_back(TraCIStage(it->line == ""
                                                    ? MSTransportable::MOVING_WITHOUT_VEHICLE
                                                    : MSTransportable::DRIVING));
                    resultCand.back().vType = it->vType;
                    resultCand.back().line = it->line;
                    resultCand.back().destStop = it->destStop;
                    for (const MSEdge* e : it->edges) {
                        resultCand.back().edges.push_back(e->getID());
                    }
                    resultCand.back().travelTime = it->traveltime;
                    resultCand.back().cost = it->cost;
                    resultCand.back().length = it->length;
                    resultCand.back().intended = it->intended;
                    resultCand.back().depart = it->depart;
                    resultCand.back().departPos = it->departPos;
                    resultCand.back().arrivalPos = it->arrivalPos;
                    resultCand.back().description = it->description;
                }
                cost += it->cost;
            }
            if (cost < minCost) {
                minCost = cost;
                result = resultCand;
            }
        }
        if (vehicle != nullptr) {
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
        if (cs == nullptr) {
            throw TraCIException("Invalid chargingStation '" + objectID + "'");
        }
        if (attrName == toString(SUMO_ATTR_TOTALENERGYCHARGED)) {
            return toString(cs->getTotalCharged());
        } else if (attrName == toString(SUMO_ATTR_NAME)) {
            return toString(cs->getMyName());
        } else if (attrName == "lane") {
            return cs->getLane().getID();
        } else if (cs->knowsParameter(attrName)) {
            return cs->getParameter(attrName);
        } else {
            throw TraCIException("Invalid chargingStation parameter '" + attrName + "'");
        }
    } else if (StringUtils::startsWith(key, "parkingArea.")) {
        const std::string attrName = key.substr(12);
        MSParkingArea* pa = static_cast<MSParkingArea*>(MSNet::getInstance()->getStoppingPlace(objectID, SUMO_TAG_PARKING_AREA));
        if (pa == nullptr) {
            throw TraCIException("Invalid parkingArea '" + objectID + "'");
        }
        if (attrName == "capacity") {
            return toString(pa->getCapacity());
        } else if (attrName == "occupancy") {
            return toString(pa->getOccupancyIncludingBlocked());
        } else if (attrName == toString(SUMO_ATTR_NAME)) {
            return toString(pa->getMyName());
        } else if (attrName == "lane") {
            return pa->getLane().getID();
        } else if (pa->knowsParameter(attrName)) {
            return pa->getParameter(attrName);
        } else {
            throw TraCIException("Invalid parkingArea parameter '" + attrName + "'");
        }
    } else if (StringUtils::startsWith(key, "busStop.")) {
        const std::string attrName = key.substr(8);
        MSStoppingPlace* bs = static_cast<MSStoppingPlace*>(MSNet::getInstance()->getStoppingPlace(objectID, SUMO_TAG_BUS_STOP));
        if (bs == nullptr) {
            throw TraCIException("Invalid busStop '" + objectID + "'");
        }
        if (attrName == toString(SUMO_ATTR_NAME)) {
            return toString(bs->getMyName());
        } else if (attrName == "lane") {
            return bs->getLane().getID();
        } else if (bs->knowsParameter(attrName)) {
            return bs->getParameter(attrName);
        } else {
            throw TraCIException("Invalid busStop parameter '" + attrName + "'");
        }
    } else {
        throw TraCIException("Parameter '" + key + "' is not supported.");
    }
}


void
Simulation::clearPending(const std::string& routeID) {
    MSNet::getInstance()->getInsertionControl().clearPendingVehicles(routeID);
}


void
Simulation::saveState(const std::string& fileName) {
    MSStateHandler::saveState(fileName, MSNet::getInstance()->getCurrentTimeStep());
}


std::shared_ptr<VariableWrapper>
Simulation::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Simulation::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
        case VAR_TIME:
            return wrapper->wrapDouble(objID, variable, getTime());
        case VAR_TIME_STEP:
            return wrapper->wrapInt(objID, variable, (int)getCurrentTime());
        case VAR_LOADED_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getLoadedNumber());
        case VAR_LOADED_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getLoadedIDList());
        case VAR_DEPARTED_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getDepartedNumber());
        case VAR_DEPARTED_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getDepartedIDList());
        case VAR_TELEPORT_STARTING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getStartingTeleportNumber());
        case VAR_TELEPORT_STARTING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getStartingTeleportIDList());
        case VAR_TELEPORT_ENDING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getEndingTeleportNumber());
        case VAR_TELEPORT_ENDING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getEndingTeleportIDList());
        case VAR_ARRIVED_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getArrivedNumber());
        case VAR_ARRIVED_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getArrivedIDList());
        case VAR_PARKING_STARTING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getParkingStartingVehiclesNumber());
        case VAR_PARKING_STARTING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getParkingStartingVehiclesIDList());
        case VAR_PARKING_ENDING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getParkingEndingVehiclesNumber());
        case VAR_PARKING_ENDING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getParkingEndingVehiclesIDList());
        case VAR_STOP_STARTING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getStopStartingVehiclesNumber());
        case VAR_STOP_STARTING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getStopStartingVehiclesIDList());
        case VAR_STOP_ENDING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getStopEndingVehiclesNumber());
        case VAR_STOP_ENDING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getStopEndingVehiclesIDList());
        case VAR_COLLIDING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getCollidingVehiclesNumber());
        case VAR_COLLIDING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getCollidingVehiclesIDList());
        case VAR_EMERGENCYSTOPPING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getEmergencyStoppingVehiclesNumber());
        case VAR_EMERGENCYSTOPPING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getEmergencyStoppingVehiclesIDList());
        case VAR_DELTA_T:
            return wrapper->wrapDouble(objID, variable, getDeltaT());
        case VAR_MIN_EXPECTED_VEHICLES:
            return wrapper->wrapInt(objID, variable, getMinExpectedNumber());
        case VAR_BUS_STOP_WAITING:
            return wrapper->wrapInt(objID, variable, getBusStopWaiting(objID));
        default:
            return false;
    }
}


}


/****************************************************************************/
