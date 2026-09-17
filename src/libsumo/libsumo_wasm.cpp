/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    libsumo_wasm.cpp
/// @author  Masaya Kataoka
/// @date    17.09.2026
///
// Embind based JavaScript bindings for libsumo, used by the WebAssembly build
/****************************************************************************/
#include <config.h>

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <libsumo/TraCIConstants.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/Edge.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/Junction.h>
#include <libsumo/Lane.h>
#include <libsumo/LaneArea.h>
#include <libsumo/MultiEntryExit.h>
#include <libsumo/POI.h>
#include <libsumo/Person.h>
#include <libsumo/Polygon.h>
#include <libsumo/Route.h>
#include <libsumo/Simulation.h>
#include <libsumo/TrafficLight.h>
#include <libsumo/Vehicle.h>
#include <libsumo/VehicleType.h>

using emscripten::val;


// ===========================================================================
// plumbing
// ===========================================================================
namespace {

/** @brief Turns a C++ exception into a proper JavaScript Error object.
 *
 * Without this every libsumo error would reach JavaScript as an opaque
 * WebAssembly.Exception without any readable message.
 */
[[noreturn]] void
throwAsJsError(const std::string& what) {
    val error = val::global("Error").new_(what);
    error.set("name", std::string("TraCIError"));
    error.throw_();
}


[[noreturn]] void
rethrowCurrentAsJsError() {
    try {
        throw;
    } catch (const std::exception& e) {
        throwAsJsError(e.what());
    } catch (...) {
        throwAsJsError("unknown libsumo error");
    }
}


/// @brief Wraps a free function so that C++ exceptions surface as JavaScript errors
template <auto F> struct Guard;

template <typename R, typename... A, R(*F)(A...)>
struct Guard<F> {
    static R call(A... args) {
        try {
            return F(args...);
        } catch (...) {
            rethrowCurrentAsJsError();
        }
    }
};


/** @brief Wraps a function returning a std::vector so that JavaScript receives a
 * plain Array instead of an embind vector which would have to be freed by hand.
 */
template <auto F> struct VecGuard;

template <typename T, typename... A, std::vector<T>(*F)(A...)>
struct VecGuard<F> {
    static val call(A... args) {
        try {
            return val::array(F(args...));
        } catch (...) {
            rethrowCurrentAsJsError();
        }
    }
};

// libsumo declares some of its list getters with a const qualified return type
template <typename T, typename... A, const std::vector<T>(*F)(A...)>
struct VecGuard<F> {
    static val call(A... args) {
        try {
            return val::array(F(args...));
        } catch (...) {
            rethrowCurrentAsJsError();
        }
    }
};


std::vector<std::string>
toStringVector(const val& array) {
    return emscripten::vecFromJSArray<std::string>(array);
}


std::vector<int>
toIntVector(const val& array) {
    return emscripten::vecFromJSArray<int>(array);
}

// ===========================================================================
// value types which have no direct embind representation
// ===========================================================================
/// @brief the result of Vehicle.getLeader / Vehicle.getFollower
struct JsNeighbor {
    std::string id;
    double dist;
};

/// @brief libsumo::TraCINextTLSData with the state as a string instead of a char
struct JsNextTLS {
    std::string id;
    int tlIndex;
    double dist;
    std::string state;
};

/// @brief the SUMO version
struct JsVersion {
    int apiVersion;
    std::string softwareVersion;
};

// ===========================================================================
// the namespaces exposed to JavaScript
// ===========================================================================
struct JsSimulation {};
struct JsEdge {};
struct JsLane {};
struct JsJunction {};
struct JsRoute {};
struct JsVehicle {};
struct JsVehicleType {};
struct JsPerson {};
struct JsTrafficLight {};
struct JsInductionLoop {};
struct JsLaneArea {};
struct JsMultiEntryExit {};
struct JsPoi {};
struct JsPolygon {};

// ===========================================================================
// shims for calls with default arguments, overloads or unbindable types
// ===========================================================================
// --- Simulation ---
void simulationLoad(const val& args) {
    libsumo::Simulation::load(toStringVector(args));
}

void simulationStep(double time) {
    libsumo::Simulation::step(time);
}

void simulationClose() {
    libsumo::Simulation::close();
}

JsVersion simulationGetVersion() {
    const std::pair<int, std::string> version = libsumo::Simulation::getVersion();
    return JsVersion{version.first, version.second};
}

val simulationGetNetBoundary() {
    return val::array(libsumo::Simulation::getNetBoundary().value);
}

libsumo::TraCIPosition simulationConvert2D(const std::string& edgeID, double pos, int laneIndex, bool toGeo) {
    return libsumo::Simulation::convert2D(edgeID, pos, laneIndex, toGeo);
}

libsumo::TraCIPosition simulationConvertGeo(double x, double y, bool fromGeo) {
    return libsumo::Simulation::convertGeo(x, y, fromGeo);
}

libsumo::TraCIRoadPosition simulationConvertRoad(double x, double y, bool isGeo, const std::string& vClass) {
    return libsumo::Simulation::convertRoad(x, y, isGeo, vClass);
}

libsumo::TraCIStage simulationFindRoute(const std::string& fromEdge, const std::string& toEdge,
                                        const std::string& vType, double depart, int routingMode) {
    return libsumo::Simulation::findRoute(fromEdge, toEdge, vType, depart, routingMode);
}

void simulationClearPending(const std::string& routeID) {
    libsumo::Simulation::clearPending(routeID);
}

// --- Edge ---
void edgeSetAllowed(const std::string& edgeID, const val& classes) {
    libsumo::Edge::setAllowed(edgeID, toStringVector(classes));
}

void edgeSetDisallowed(const std::string& edgeID, const val& classes) {
    libsumo::Edge::setDisallowed(edgeID, toStringVector(classes));
}

void edgeAdaptTraveltime(const std::string& edgeID, double time, double beginSeconds, double endSeconds) {
    libsumo::Edge::adaptTraveltime(edgeID, time, beginSeconds, endSeconds);
}

double edgeGetAngle(const std::string& edgeID) {
    return libsumo::Edge::getAngle(edgeID);
}

// --- Lane ---
val laneGetShape(const std::string& laneID) {
    return val::array(libsumo::Lane::getShape(laneID).value);
}

void laneSetAllowed(const std::string& laneID, const val& classes) {
    libsumo::Lane::setAllowed(laneID, toStringVector(classes));
}

void laneSetDisallowed(const std::string& laneID, const val& classes) {
    libsumo::Lane::setDisallowed(laneID, toStringVector(classes));
}

double laneGetAngle(const std::string& laneID) {
    return libsumo::Lane::getAngle(laneID);
}

// --- Junction ---
libsumo::TraCIPosition junctionGetPosition(const std::string& junctionID) {
    return libsumo::Junction::getPosition(junctionID);
}

val junctionGetShape(const std::string& junctionID) {
    return val::array(libsumo::Junction::getShape(junctionID).value);
}

// --- Route ---
void routeAdd(const std::string& routeID, const val& edges) {
    libsumo::Route::add(routeID, toStringVector(edges));
}

// --- Vehicle ---
libsumo::TraCIPosition vehicleGetPosition(const std::string& vehID) {
    return libsumo::Vehicle::getPosition(vehID);
}

JsNeighbor vehicleGetLeader(const std::string& vehID, double dist) {
    const std::pair<std::string, double> leader = libsumo::Vehicle::getLeader(vehID, dist);
    return JsNeighbor{leader.first, leader.second};
}

JsNeighbor vehicleGetFollower(const std::string& vehID, double dist) {
    const std::pair<std::string, double> follower = libsumo::Vehicle::getFollower(vehID, dist);
    return JsNeighbor{follower.first, follower.second};
}

val vehicleGetNextTLS(const std::string& vehID) {
    std::vector<JsNextTLS> result;
    for (const libsumo::TraCINextTLSData& d : libsumo::Vehicle::getNextTLS(vehID)) {
        result.push_back(JsNextTLS{d.id, d.tlIndex, d.dist, std::string(1, d.state)});
    }
    return val::array(result);
}

val vehicleGetStops(const std::string& vehID, int limit) {
    return val::array(libsumo::Vehicle::getStops(vehID, limit));
}

void vehicleAdd(const std::string& vehID, const std::string& routeID, const std::string& typeID,
                const std::string& depart, const std::string& departLane, const std::string& departPos,
                const std::string& departSpeed) {
    libsumo::Vehicle::add(vehID, routeID, typeID, depart, departLane, departPos, departSpeed);
}

void vehicleRemove(const std::string& vehID, int reason) {
    libsumo::Vehicle::remove(vehID, (char)reason);
}

void vehicleSetRoute(const std::string& vehID, const val& edges) {
    libsumo::Vehicle::setRoute(vehID, toStringVector(edges));
}

void vehicleSetVia(const std::string& vehID, const val& edges) {
    libsumo::Vehicle::setVia(vehID, toStringVector(edges));
}

void vehicleMoveTo(const std::string& vehID, const std::string& laneID, double pos, int reason) {
    libsumo::Vehicle::moveTo(vehID, laneID, pos, reason);
}

void vehicleMoveToXY(const std::string& vehID, const std::string& edgeID, int laneIndex,
                     double x, double y, double angle, int keepRoute, double matchThreshold) {
    libsumo::Vehicle::moveToXY(vehID, edgeID, laneIndex, x, y, angle, keepRoute, matchThreshold);
}

void vehicleRerouteTraveltime(const std::string& vehID, bool currentTravelTimes) {
    libsumo::Vehicle::rerouteTraveltime(vehID, currentTravelTimes);
}

double vehicleGetDrivingDistance(const std::string& vehID, const std::string& edgeID, double pos) {
    return libsumo::Vehicle::getDrivingDistance(vehID, edgeID, pos);
}

// --- Person ---
libsumo::TraCIPosition personGetPosition(const std::string& personID) {
    return libsumo::Person::getPosition(personID);
}

libsumo::TraCIStage personGetStage(const std::string& personID, int nextStageIndex) {
    return libsumo::Person::getStage(personID, nextStageIndex);
}

val personGetEdges(const std::string& personID, int nextStageIndex) {
    return val::array(libsumo::Person::getEdges(personID, nextStageIndex));
}

void personAdd(const std::string& personID, const std::string& edgeID, double pos, double depart,
               const std::string& typeID) {
    libsumo::Person::add(personID, edgeID, pos, depart, typeID);
}

void personAppendWalkingStage(const std::string& personID, const val& edges, double arrivalPos,
                              double duration, double speed, const std::string& stopID) {
    libsumo::Person::appendWalkingStage(personID, toStringVector(edges), arrivalPos, duration, speed, stopID);
}

void personRemove(const std::string& personID, int reason) {
    libsumo::Person::remove(personID, (char)reason);
}

// --- TrafficLight ---
val trafficLightGetControlledLinks(const std::string& tlsID) {
    val result = val::array();
    int index = 0;
    for (const std::vector<libsumo::TraCILink>& links : libsumo::TrafficLight::getControlledLinks(tlsID)) {
        result.set(index++, val::array(links));
    }
    return result;
}

val toJsLogic(const libsumo::TraCILogic& logic) {
    val result = val::object();
    result.set("programID", logic.programID);
    result.set("type", logic.type);
    result.set("currentPhaseIndex", logic.currentPhaseIndex);
    val phases = val::array();
    int index = 0;
    for (const std::shared_ptr<libsumo::TraCIPhase>& phase : logic.phases) {
        phases.set(index++, val(*phase));
    }
    result.set("phases", phases);
    val subParameter = val::object();
    for (const auto& item : logic.subParameter) {
        subParameter.set(item.first, item.second);
    }
    result.set("subParameter", subParameter);
    return result;
}

val trafficLightGetAllProgramLogics(const std::string& tlsID) {
    val result = val::array();
    int index = 0;
    for (const libsumo::TraCILogic& logic : libsumo::TrafficLight::getAllProgramLogics(tlsID)) {
        result.set(index++, toJsLogic(logic));
    }
    return result;
}

void trafficLightSetProgramLogic(const std::string& tlsID, const val& jsLogic) {
    libsumo::TraCILogic logic;
    logic.programID = jsLogic["programID"].as<std::string>();
    logic.type = jsLogic["type"].as<int>();
    logic.currentPhaseIndex = jsLogic["currentPhaseIndex"].as<int>();
    const val jsPhases = jsLogic["phases"];
    const int numPhases = jsPhases["length"].as<int>();
    for (int i = 0; i < numPhases; i++) {
        logic.phases.push_back(std::make_shared<libsumo::TraCIPhase>(jsPhases[i].as<libsumo::TraCIPhase>()));
    }
    libsumo::TrafficLight::setProgramLogic(tlsID, logic);
}

// --- POI ---
libsumo::TraCIPosition poiGetPosition(const std::string& poiID) {
    return libsumo::POI::getPosition(poiID);
}

bool poiAdd(const std::string& poiID, double x, double y, const libsumo::TraCIColor& color,
            const std::string& poiType, int layer, const std::string& imgFile,
            double width, double height, double angle) {
    return libsumo::POI::add(poiID, x, y, color, poiType, layer, imgFile, width, height, angle);
}

bool poiRemove(const std::string& poiID, int layer) {
    return libsumo::POI::remove(poiID, layer);
}

// --- Polygon ---
val polygonGetShape(const std::string& polygonID) {
    return val::array(libsumo::Polygon::getShape(polygonID).value);
}

libsumo::TraCIPositionVector toPositionVector(const val& array) {
    libsumo::TraCIPositionVector shape;
    const int length = array["length"].as<int>();
    for (int i = 0; i < length; i++) {
        shape.value.push_back(array[i].as<libsumo::TraCIPosition>());
    }
    return shape;
}

void polygonSetShape(const std::string& polygonID, const val& shape) {
    libsumo::Polygon::setShape(polygonID, toPositionVector(shape));
}

void polygonAdd(const std::string& polygonID, const val& shape, const libsumo::TraCIColor& color,
                bool fill, const std::string& polygonType, int layer, double lineWidth) {
    libsumo::Polygon::add(polygonID, toPositionVector(shape), color, fill, polygonType, layer, lineWidth);
}

void polygonRemove(const std::string& polygonID, int layer) {
    libsumo::Polygon::remove(polygonID, layer);
}

// --- VehicleType ---
void vehicleTypeSetActionStepLength(const std::string& typeID, double actionStepLength) {
    libsumo::VehicleType::setActionStepLength(typeID, actionStepLength);
}

void vehicleSetActionStepLength(const std::string& vehID, double actionStepLength) {
    libsumo::Vehicle::setActionStepLength(vehID, actionStepLength);
}

void personSetActionStepLength(const std::string& personID, double actionStepLength) {
    libsumo::Person::setActionStepLength(personID, actionStepLength);
}

}  // namespace


// ===========================================================================
// bindings
// ===========================================================================
#define FN(NAME, FUNC) class_function(NAME, &Guard<FUNC>::call)
#define VEC_FN(NAME, FUNC) class_function(NAME, &VecGuard<FUNC>::call)

/// @brief getIDList / getIDCount / getParameter / setParameter of a libsumo domain
#define ID_PARAMETER_API(CLASS) \
    VEC_FN("getIDList", &libsumo::CLASS::getIDList) \
    .FN("getIDCount", &libsumo::CLASS::getIDCount) \
    .FN("getParameter", &libsumo::CLASS::getParameter) \
    .FN("setParameter", &libsumo::CLASS::setParameter)

/// @brief the vehicle type getters shared by Vehicle, VehicleType and Person
#define VEHICLE_TYPE_GETTER(CLASS) \
    FN("getLength", &libsumo::CLASS::getLength) \
    .FN("getMaxSpeed", &libsumo::CLASS::getMaxSpeed) \
    .FN("getVehicleClass", &libsumo::CLASS::getVehicleClass) \
    .FN("getSpeedFactor", &libsumo::CLASS::getSpeedFactor) \
    .FN("getAccel", &libsumo::CLASS::getAccel) \
    .FN("getDecel", &libsumo::CLASS::getDecel) \
    .FN("getEmergencyDecel", &libsumo::CLASS::getEmergencyDecel) \
    .FN("getApparentDecel", &libsumo::CLASS::getApparentDecel) \
    .FN("getImperfection", &libsumo::CLASS::getImperfection) \
    .FN("getTau", &libsumo::CLASS::getTau) \
    .FN("getEmissionClass", &libsumo::CLASS::getEmissionClass) \
    .FN("getShapeClass", &libsumo::CLASS::getShapeClass) \
    .FN("getMinGap", &libsumo::CLASS::getMinGap) \
    .FN("getWidth", &libsumo::CLASS::getWidth) \
    .FN("getHeight", &libsumo::CLASS::getHeight) \
    .FN("getMass", &libsumo::CLASS::getMass) \
    .FN("getColor", &libsumo::CLASS::getColor) \
    .FN("getMinGapLat", &libsumo::CLASS::getMinGapLat) \
    .FN("getMaxSpeedLat", &libsumo::CLASS::getMaxSpeedLat) \
    .FN("getLateralAlignment", &libsumo::CLASS::getLateralAlignment) \
    .FN("getPersonCapacity", &libsumo::CLASS::getPersonCapacity) \
    .FN("getActionStepLength", &libsumo::CLASS::getActionStepLength) \
    .FN("getSpeedDeviation", &libsumo::CLASS::getSpeedDeviation) \
    .FN("getBoardingDuration", &libsumo::CLASS::getBoardingDuration) \
    .FN("getImpatience", &libsumo::CLASS::getImpatience)

/// @brief the vehicle type setters shared by Vehicle, VehicleType and Person
#define VEHICLE_TYPE_SETTER(CLASS) \
    FN("setLength", &libsumo::CLASS::setLength) \
    .FN("setMaxSpeed", &libsumo::CLASS::setMaxSpeed) \
    .FN("setVehicleClass", &libsumo::CLASS::setVehicleClass) \
    .FN("setSpeedFactor", &libsumo::CLASS::setSpeedFactor) \
    .FN("setAccel", &libsumo::CLASS::setAccel) \
    .FN("setDecel", &libsumo::CLASS::setDecel) \
    .FN("setEmergencyDecel", &libsumo::CLASS::setEmergencyDecel) \
    .FN("setApparentDecel", &libsumo::CLASS::setApparentDecel) \
    .FN("setImperfection", &libsumo::CLASS::setImperfection) \
    .FN("setTau", &libsumo::CLASS::setTau) \
    .FN("setEmissionClass", &libsumo::CLASS::setEmissionClass) \
    .FN("setShapeClass", &libsumo::CLASS::setShapeClass) \
    .FN("setWidth", &libsumo::CLASS::setWidth) \
    .FN("setHeight", &libsumo::CLASS::setHeight) \
    .FN("setMass", &libsumo::CLASS::setMass) \
    .FN("setColor", &libsumo::CLASS::setColor) \
    .FN("setMinGap", &libsumo::CLASS::setMinGap) \
    .FN("setMinGapLat", &libsumo::CLASS::setMinGapLat) \
    .FN("setMaxSpeedLat", &libsumo::CLASS::setMaxSpeedLat) \
    .FN("setLateralAlignment", &libsumo::CLASS::setLateralAlignment) \
    .FN("setBoardingDuration", &libsumo::CLASS::setBoardingDuration) \
    .FN("setImpatience", &libsumo::CLASS::setImpatience)


EMSCRIPTEN_BINDINGS(libsumo_types) {
    emscripten::value_object<libsumo::TraCIPosition>("Position")
    .field("x", &libsumo::TraCIPosition::x)
    .field("y", &libsumo::TraCIPosition::y)
    .field("z", &libsumo::TraCIPosition::z);

    emscripten::value_object<libsumo::TraCIRoadPosition>("RoadPosition")
    .field("edgeID", &libsumo::TraCIRoadPosition::edgeID)
    .field("pos", &libsumo::TraCIRoadPosition::pos)
    .field("laneIndex", &libsumo::TraCIRoadPosition::laneIndex);

    emscripten::value_object<libsumo::TraCIColor>("Color")
    .field("r", &libsumo::TraCIColor::r)
    .field("g", &libsumo::TraCIColor::g)
    .field("b", &libsumo::TraCIColor::b)
    .field("a", &libsumo::TraCIColor::a);

    emscripten::value_object<libsumo::TraCILink>("Link")
    .field("fromLane", &libsumo::TraCILink::fromLane)
    .field("viaLane", &libsumo::TraCILink::viaLane)
    .field("toLane", &libsumo::TraCILink::toLane);

    emscripten::value_object<libsumo::TraCIConnection>("Connection")
    .field("approachedLane", &libsumo::TraCIConnection::approachedLane)
    .field("hasPrio", &libsumo::TraCIConnection::hasPrio)
    .field("isOpen", &libsumo::TraCIConnection::isOpen)
    .field("hasFoe", &libsumo::TraCIConnection::hasFoe)
    .field("approachedInternal", &libsumo::TraCIConnection::approachedInternal)
    .field("state", &libsumo::TraCIConnection::state)
    .field("direction", &libsumo::TraCIConnection::direction)
    .field("length", &libsumo::TraCIConnection::length);

    emscripten::value_object<libsumo::TraCIVehicleData>("VehicleData")
    .field("id", &libsumo::TraCIVehicleData::id)
    .field("length", &libsumo::TraCIVehicleData::length)
    .field("entryTime", &libsumo::TraCIVehicleData::entryTime)
    .field("leaveTime", &libsumo::TraCIVehicleData::leaveTime)
    .field("typeID", &libsumo::TraCIVehicleData::typeID);

    emscripten::value_object<JsNeighbor>("Neighbor")
    .field("id", &JsNeighbor::id)
    .field("dist", &JsNeighbor::dist);

    emscripten::value_object<JsNextTLS>("NextTLS")
    .field("id", &JsNextTLS::id)
    .field("tlIndex", &JsNextTLS::tlIndex)
    .field("dist", &JsNextTLS::dist)
    .field("state", &JsNextTLS::state);

    emscripten::value_object<JsVersion>("Version")
    .field("apiVersion", &JsVersion::apiVersion)
    .field("softwareVersion", &JsVersion::softwareVersion);

    emscripten::value_object<libsumo::TraCINextStopData>("StopData")
    .field("lane", &libsumo::TraCINextStopData::lane)
    .field("startPos", &libsumo::TraCINextStopData::startPos)
    .field("endPos", &libsumo::TraCINextStopData::endPos)
    .field("stoppingPlaceID", &libsumo::TraCINextStopData::stoppingPlaceID)
    .field("stopFlags", &libsumo::TraCINextStopData::stopFlags)
    .field("duration", &libsumo::TraCINextStopData::duration)
    .field("until", &libsumo::TraCINextStopData::until)
    .field("intendedArrival", &libsumo::TraCINextStopData::intendedArrival)
    .field("arrival", &libsumo::TraCINextStopData::arrival)
    .field("depart", &libsumo::TraCINextStopData::depart)
    .field("split", &libsumo::TraCINextStopData::split)
    .field("join", &libsumo::TraCINextStopData::join)
    .field("actType", &libsumo::TraCINextStopData::actType)
    .field("tripId", &libsumo::TraCINextStopData::tripId)
    .field("line", &libsumo::TraCINextStopData::line)
    .field("speed", &libsumo::TraCINextStopData::speed);
}


namespace {
// the string vector members of the value objects below are mapped to plain
// JavaScript arrays so that no embind vector has to be freed by the caller
val stageGetEdges(const libsumo::TraCIStage& stage) {
    return val::array(stage.edges);
}

void stageSetEdges(libsumo::TraCIStage& stage, val edges) {
    stage.edges = toStringVector(edges);
}

val phaseGetNext(const libsumo::TraCIPhase& phase) {
    return val::array(phase.next);
}

void phaseSetNext(libsumo::TraCIPhase& phase, val next) {
    phase.next = toIntVector(next);
}

val bestLanesGetContinuationLanes(const libsumo::TraCIBestLanesData& data) {
    return val::array(data.continuationLanes);
}

void bestLanesSetContinuationLanes(libsumo::TraCIBestLanesData& data, val lanes) {
    data.continuationLanes = toStringVector(lanes);
}
}  // namespace


EMSCRIPTEN_BINDINGS(libsumo_composite_types) {
    emscripten::value_object<libsumo::TraCIStage>("Stage")
    .field("type", &libsumo::TraCIStage::type)
    .field("vType", &libsumo::TraCIStage::vType)
    .field("line", &libsumo::TraCIStage::line)
    .field("destStop", &libsumo::TraCIStage::destStop)
    .field("edges", &stageGetEdges, &stageSetEdges)
    .field("travelTime", &libsumo::TraCIStage::travelTime)
    .field("cost", &libsumo::TraCIStage::cost)
    .field("length", &libsumo::TraCIStage::length)
    .field("intended", &libsumo::TraCIStage::intended)
    .field("depart", &libsumo::TraCIStage::depart)
    .field("departPos", &libsumo::TraCIStage::departPos)
    .field("arrivalPos", &libsumo::TraCIStage::arrivalPos)
    .field("description", &libsumo::TraCIStage::description);

    emscripten::value_object<libsumo::TraCIPhase>("Phase")
    .field("duration", &libsumo::TraCIPhase::duration)
    .field("state", &libsumo::TraCIPhase::state)
    .field("minDur", &libsumo::TraCIPhase::minDur)
    .field("maxDur", &libsumo::TraCIPhase::maxDur)
    .field("next", &phaseGetNext, &phaseSetNext)
    .field("name", &libsumo::TraCIPhase::name)
    .field("earlyTarget", &libsumo::TraCIPhase::earlyTarget);

    emscripten::value_object<libsumo::TraCIBestLanesData>("BestLanesData")
    .field("laneID", &libsumo::TraCIBestLanesData::laneID)
    .field("length", &libsumo::TraCIBestLanesData::length)
    .field("occupation", &libsumo::TraCIBestLanesData::occupation)
    .field("bestLaneOffset", &libsumo::TraCIBestLanesData::bestLaneOffset)
    .field("allowsContinuation", &libsumo::TraCIBestLanesData::allowsContinuation)
    .field("continuationLanes", &bestLanesGetContinuationLanes, &bestLanesSetContinuationLanes);
}


EMSCRIPTEN_BINDINGS(libsumo_simulation) {
    emscripten::class_<JsSimulation>("Simulation")
    .class_function("load", &Guard<&simulationLoad>::call)
    .FN("isLoaded", &libsumo::Simulation::isLoaded)
    .class_function("step", &Guard<&simulationStep>::call)
    .FN("executeMove", &libsumo::Simulation::executeMove)
    .class_function("close", &Guard<&simulationClose>::call)
    .class_function("getVersion", &Guard<&simulationGetVersion>::call)
    .FN("getOption", &libsumo::Simulation::getOption)
    .FN("getTime", &libsumo::Simulation::getTime)
    .FN("getEndTime", &libsumo::Simulation::getEndTime)
    .FN("getDeltaT", &libsumo::Simulation::getDeltaT)
    .FN("getCurrentTime", &libsumo::Simulation::getCurrentTime)
    .FN("getMinExpectedNumber", &libsumo::Simulation::getMinExpectedNumber)
    .FN("getLoadedNumber", &libsumo::Simulation::getLoadedNumber)
    .VEC_FN("getLoadedIDList", &libsumo::Simulation::getLoadedIDList)
    .FN("getDepartedNumber", &libsumo::Simulation::getDepartedNumber)
    .VEC_FN("getDepartedIDList", &libsumo::Simulation::getDepartedIDList)
    .FN("getArrivedNumber", &libsumo::Simulation::getArrivedNumber)
    .VEC_FN("getArrivedIDList", &libsumo::Simulation::getArrivedIDList)
    .FN("getCollidingVehiclesNumber", &libsumo::Simulation::getCollidingVehiclesNumber)
    .VEC_FN("getCollidingVehiclesIDList", &libsumo::Simulation::getCollidingVehiclesIDList)
    .FN("getStartingTeleportNumber", &libsumo::Simulation::getStartingTeleportNumber)
    .VEC_FN("getStartingTeleportIDList", &libsumo::Simulation::getStartingTeleportIDList)
    .FN("getEndingTeleportNumber", &libsumo::Simulation::getEndingTeleportNumber)
    .VEC_FN("getEndingTeleportIDList", &libsumo::Simulation::getEndingTeleportIDList)
    .FN("getDepartedPersonNumber", &libsumo::Simulation::getDepartedPersonNumber)
    .VEC_FN("getDepartedPersonIDList", &libsumo::Simulation::getDepartedPersonIDList)
    .FN("getArrivedPersonNumber", &libsumo::Simulation::getArrivedPersonNumber)
    .VEC_FN("getArrivedPersonIDList", &libsumo::Simulation::getArrivedPersonIDList)
    .VEC_FN("getBusStopIDList", &libsumo::Simulation::getBusStopIDList)
    .FN("getBusStopWaiting", &libsumo::Simulation::getBusStopWaiting)
    .VEC_FN("getBusStopWaitingIDList", &libsumo::Simulation::getBusStopWaitingIDList)
    .VEC_FN("getPendingVehicles", &libsumo::Simulation::getPendingVehicles)
    .class_function("getNetBoundary", &Guard<&simulationGetNetBoundary>::call)
    .class_function("convert2D", &Guard<&simulationConvert2D>::call)
    .class_function("convertGeo", &Guard<&simulationConvertGeo>::call)
    .class_function("convertRoad", &Guard<&simulationConvertRoad>::call)
    .FN("getDistance2D", &libsumo::Simulation::getDistance2D)
    .FN("getDistanceRoad", &libsumo::Simulation::getDistanceRoad)
    .class_function("findRoute", &Guard<&simulationFindRoute>::call)
    .FN("getScale", &libsumo::Simulation::getScale)
    .FN("setScale", &libsumo::Simulation::setScale)
    .class_function("clearPending", &Guard<&simulationClearPending>::call)
    .FN("saveState", &libsumo::Simulation::saveState)
    .FN("loadState", &libsumo::Simulation::loadState)
    .FN("writeMessage", &libsumo::Simulation::writeMessage)
    .FN("getParameter", &libsumo::Simulation::getParameter)
    .FN("setParameter", &libsumo::Simulation::setParameter);
}


EMSCRIPTEN_BINDINGS(libsumo_network) {
    emscripten::class_<JsEdge>("Edge")
    .ID_PARAMETER_API(Edge)
    .FN("getAdaptedTraveltime", &libsumo::Edge::getAdaptedTraveltime)
    .FN("getEffort", &libsumo::Edge::getEffort)
    .FN("getTraveltime", &libsumo::Edge::getTraveltime)
    .FN("getWaitingTime", &libsumo::Edge::getWaitingTime)
    .VEC_FN("getLastStepPersonIDs", &libsumo::Edge::getLastStepPersonIDs)
    .VEC_FN("getLastStepVehicleIDs", &libsumo::Edge::getLastStepVehicleIDs)
    .VEC_FN("getPendingVehicles", &libsumo::Edge::getPendingVehicles)
    .FN("getCO2Emission", &libsumo::Edge::getCO2Emission)
    .FN("getCOEmission", &libsumo::Edge::getCOEmission)
    .FN("getHCEmission", &libsumo::Edge::getHCEmission)
    .FN("getPMxEmission", &libsumo::Edge::getPMxEmission)
    .FN("getNOxEmission", &libsumo::Edge::getNOxEmission)
    .FN("getFuelConsumption", &libsumo::Edge::getFuelConsumption)
    .FN("getNoiseEmission", &libsumo::Edge::getNoiseEmission)
    .FN("getElectricityConsumption", &libsumo::Edge::getElectricityConsumption)
    .FN("getLastStepVehicleNumber", &libsumo::Edge::getLastStepVehicleNumber)
    .FN("getLastStepMeanSpeed", &libsumo::Edge::getLastStepMeanSpeed)
    .FN("getMeanFriction", &libsumo::Edge::getMeanFriction)
    .FN("getLastStepOccupancy", &libsumo::Edge::getLastStepOccupancy)
    .FN("getLastStepHaltingNumber", &libsumo::Edge::getLastStepHaltingNumber)
    .FN("getLastStepLength", &libsumo::Edge::getLastStepLength)
    .FN("getLaneNumber", &libsumo::Edge::getLaneNumber)
    .FN("getStreetName", &libsumo::Edge::getStreetName)
    .FN("getFromJunction", &libsumo::Edge::getFromJunction)
    .FN("getToJunction", &libsumo::Edge::getToJunction)
    .FN("getBidiEdge", &libsumo::Edge::getBidiEdge)
    .class_function("getAngle", &Guard<&edgeGetAngle>::call)
    .class_function("setAllowed", &Guard<&edgeSetAllowed>::call)
    .class_function("setDisallowed", &Guard<&edgeSetDisallowed>::call)
    .class_function("adaptTraveltime", &Guard<&edgeAdaptTraveltime>::call)
    .FN("setMaxSpeed", &libsumo::Edge::setMaxSpeed)
    .FN("setFriction", &libsumo::Edge::setFriction);

    emscripten::class_<JsLane>("Lane")
    .ID_PARAMETER_API(Lane)
    .FN("getEdgeID", &libsumo::Lane::getEdgeID)
    .FN("getLength", &libsumo::Lane::getLength)
    .FN("getMaxSpeed", &libsumo::Lane::getMaxSpeed)
    .FN("getFriction", &libsumo::Lane::getFriction)
    .FN("getWidth", &libsumo::Lane::getWidth)
    .FN("getLinkNumber", &libsumo::Lane::getLinkNumber)
    .VEC_FN("getAllowed", &libsumo::Lane::getAllowed)
    .VEC_FN("getDisallowed", &libsumo::Lane::getDisallowed)
    .VEC_FN("getChangePermissions", &libsumo::Lane::getChangePermissions)
    .VEC_FN("getLinks", &libsumo::Lane::getLinks)
    .VEC_FN("getFoes", &libsumo::Lane::getFoes)
    .VEC_FN("getInternalFoes", &libsumo::Lane::getInternalFoes)
    .VEC_FN("getLastStepVehicleIDs", &libsumo::Lane::getLastStepVehicleIDs)
    .VEC_FN("getPendingVehicles", &libsumo::Lane::getPendingVehicles)
    .class_function("getShape", &Guard<&laneGetShape>::call)
    .FN("getCO2Emission", &libsumo::Lane::getCO2Emission)
    .FN("getCOEmission", &libsumo::Lane::getCOEmission)
    .FN("getHCEmission", &libsumo::Lane::getHCEmission)
    .FN("getPMxEmission", &libsumo::Lane::getPMxEmission)
    .FN("getNOxEmission", &libsumo::Lane::getNOxEmission)
    .FN("getFuelConsumption", &libsumo::Lane::getFuelConsumption)
    .FN("getNoiseEmission", &libsumo::Lane::getNoiseEmission)
    .FN("getElectricityConsumption", &libsumo::Lane::getElectricityConsumption)
    .FN("getLastStepMeanSpeed", &libsumo::Lane::getLastStepMeanSpeed)
    .FN("getLastStepOccupancy", &libsumo::Lane::getLastStepOccupancy)
    .FN("getLastStepLength", &libsumo::Lane::getLastStepLength)
    .FN("getWaitingTime", &libsumo::Lane::getWaitingTime)
    .FN("getTraveltime", &libsumo::Lane::getTraveltime)
    .FN("getLastStepVehicleNumber", &libsumo::Lane::getLastStepVehicleNumber)
    .FN("getLastStepHaltingNumber", &libsumo::Lane::getLastStepHaltingNumber)
    .FN("getBidiLane", &libsumo::Lane::getBidiLane)
    .class_function("getAngle", &Guard<&laneGetAngle>::call)
    .class_function("setAllowed", &Guard<&laneSetAllowed>::call)
    .class_function("setDisallowed", &Guard<&laneSetDisallowed>::call)
    .FN("setMaxSpeed", &libsumo::Lane::setMaxSpeed)
    .FN("setLength", &libsumo::Lane::setLength)
    .FN("setFriction", &libsumo::Lane::setFriction);

    emscripten::class_<JsJunction>("Junction")
    .ID_PARAMETER_API(Junction)
    .class_function("getPosition", &Guard<&junctionGetPosition>::call)
    .class_function("getShape", &Guard<&junctionGetShape>::call)
    .VEC_FN("getIncomingEdges", &libsumo::Junction::getIncomingEdges)
    .VEC_FN("getOutgoingEdges", &libsumo::Junction::getOutgoingEdges);

    emscripten::class_<JsRoute>("Route")
    .ID_PARAMETER_API(Route)
    .VEC_FN("getEdges", &libsumo::Route::getEdges)
    .class_function("add", &Guard<&routeAdd>::call)
    .FN("remove", &libsumo::Route::remove);
}


EMSCRIPTEN_BINDINGS(libsumo_vehicle) {
    emscripten::class_<JsVehicle>("Vehicle")
    .ID_PARAMETER_API(Vehicle)
    .VEHICLE_TYPE_GETTER(Vehicle)
    .VEHICLE_TYPE_SETTER(Vehicle)
    .VEC_FN("getLoadedIDList", &libsumo::Vehicle::getLoadedIDList)
    .VEC_FN("getTeleportingIDList", &libsumo::Vehicle::getTeleportingIDList)
    .VEC_FN("getTaxiFleet", &libsumo::Vehicle::getTaxiFleet)
    .FN("getSpeed", &libsumo::Vehicle::getSpeed)
    .FN("getLateralSpeed", &libsumo::Vehicle::getLateralSpeed)
    .FN("getAcceleration", &libsumo::Vehicle::getAcceleration)
    .FN("getSpeedWithoutTraCI", &libsumo::Vehicle::getSpeedWithoutTraCI)
    .class_function("getPosition", &Guard<&vehicleGetPosition>::call)
    .FN("getPosition3D", &libsumo::Vehicle::getPosition3D)
    .FN("getAngle", &libsumo::Vehicle::getAngle)
    .FN("getSlope", &libsumo::Vehicle::getSlope)
    .FN("getRoadID", &libsumo::Vehicle::getRoadID)
    .FN("getLaneID", &libsumo::Vehicle::getLaneID)
    .FN("getLaneIndex", &libsumo::Vehicle::getLaneIndex)
    .FN("getTypeID", &libsumo::Vehicle::getTypeID)
    .FN("getRouteID", &libsumo::Vehicle::getRouteID)
    .FN("getRouteIndex", &libsumo::Vehicle::getRouteIndex)
    .VEC_FN("getRoute", &libsumo::Vehicle::getRoute)
    .FN("getDeparture", &libsumo::Vehicle::getDeparture)
    .FN("getDepartDelay", &libsumo::Vehicle::getDepartDelay)
    .FN("getLanePosition", &libsumo::Vehicle::getLanePosition)
    .FN("getLateralLanePosition", &libsumo::Vehicle::getLateralLanePosition)
    .FN("getCO2Emission", &libsumo::Vehicle::getCO2Emission)
    .FN("getCOEmission", &libsumo::Vehicle::getCOEmission)
    .FN("getHCEmission", &libsumo::Vehicle::getHCEmission)
    .FN("getPMxEmission", &libsumo::Vehicle::getPMxEmission)
    .FN("getNOxEmission", &libsumo::Vehicle::getNOxEmission)
    .FN("getFuelConsumption", &libsumo::Vehicle::getFuelConsumption)
    .FN("getNoiseEmission", &libsumo::Vehicle::getNoiseEmission)
    .FN("getElectricityConsumption", &libsumo::Vehicle::getElectricityConsumption)
    .FN("getPersonNumber", &libsumo::Vehicle::getPersonNumber)
    .VEC_FN("getPersonIDList", &libsumo::Vehicle::getPersonIDList)
    .class_function("getLeader", &Guard<&vehicleGetLeader>::call)
    .class_function("getFollower", &Guard<&vehicleGetFollower>::call)
    .FN("getWaitingTime", &libsumo::Vehicle::getWaitingTime)
    .FN("getAccumulatedWaitingTime", &libsumo::Vehicle::getAccumulatedWaitingTime)
    .FN("getTimeLoss", &libsumo::Vehicle::getTimeLoss)
    .FN("isRouteValid", &libsumo::Vehicle::isRouteValid)
    .FN("getSignals", &libsumo::Vehicle::getSignals)
    .VEC_FN("getBestLanes", &libsumo::Vehicle::getBestLanes)
    .class_function("getNextTLS", &Guard<&vehicleGetNextTLS>::call)
    .VEC_FN("getNextStops", &libsumo::Vehicle::getNextStops)
    .class_function("getStops", &Guard<&vehicleGetStops>::call)
    .FN("getStopState", &libsumo::Vehicle::getStopState)
    .FN("getStopDelay", &libsumo::Vehicle::getStopDelay)
    .FN("getStopArrivalDelay", &libsumo::Vehicle::getStopArrivalDelay)
    .FN("getDistance", &libsumo::Vehicle::getDistance)
    .class_function("getDrivingDistance", &Guard<&vehicleGetDrivingDistance>::call)
    .FN("getDrivingDistance2D", &libsumo::Vehicle::getDrivingDistance2D)
    .FN("getAllowedSpeed", &libsumo::Vehicle::getAllowedSpeed)
    .FN("getSpeedMode", &libsumo::Vehicle::getSpeedMode)
    .FN("getLaneChangeMode", &libsumo::Vehicle::getLaneChangeMode)
    .FN("getRoutingMode", &libsumo::Vehicle::getRoutingMode)
    .FN("getLine", &libsumo::Vehicle::getLine)
    .VEC_FN("getVia", &libsumo::Vehicle::getVia)
    .FN("getLastActionTime", &libsumo::Vehicle::getLastActionTime)
    .class_function("add", &Guard<&vehicleAdd>::call)
    .class_function("remove", &Guard<&vehicleRemove>::call)
    .FN("changeTarget", &libsumo::Vehicle::changeTarget)
    .FN("changeLane", &libsumo::Vehicle::changeLane)
    .FN("changeLaneRelative", &libsumo::Vehicle::changeLaneRelative)
    .FN("changeSublane", &libsumo::Vehicle::changeSublane)
    .FN("slowDown", &libsumo::Vehicle::slowDown)
    .FN("setSpeed", &libsumo::Vehicle::setSpeed)
    .FN("setAcceleration", &libsumo::Vehicle::setAcceleration)
    .FN("setSpeedMode", &libsumo::Vehicle::setSpeedMode)
    .FN("setLaneChangeMode", &libsumo::Vehicle::setLaneChangeMode)
    .FN("setRoutingMode", &libsumo::Vehicle::setRoutingMode)
    .FN("setType", &libsumo::Vehicle::setType)
    .FN("setRouteID", &libsumo::Vehicle::setRouteID)
    .class_function("setRoute", &Guard<&vehicleSetRoute>::call)
    .class_function("setVia", &Guard<&vehicleSetVia>::call)
    .FN("setLateralLanePosition", &libsumo::Vehicle::setLateralLanePosition)
    .FN("updateBestLanes", &libsumo::Vehicle::updateBestLanes)
    .class_function("rerouteTraveltime", &Guard<&vehicleRerouteTraveltime>::call)
    .FN("rerouteEffort", &libsumo::Vehicle::rerouteEffort)
    .FN("setSignals", &libsumo::Vehicle::setSignals)
    .class_function("moveTo", &Guard<&vehicleMoveTo>::call)
    .class_function("moveToXY", &Guard<&vehicleMoveToXY>::call)
    .FN("setLine", &libsumo::Vehicle::setLine)
    .FN("resume", &libsumo::Vehicle::resume)
    .class_function("setActionStepLength", &Guard<&vehicleSetActionStepLength>::call);

    emscripten::class_<JsVehicleType>("VehicleType")
    .ID_PARAMETER_API(VehicleType)
    .VEHICLE_TYPE_GETTER(VehicleType)
    .VEHICLE_TYPE_SETTER(VehicleType)
    .FN("copy", &libsumo::VehicleType::copy)
    .FN("getScale", &libsumo::VehicleType::getScale)
    .FN("setScale", &libsumo::VehicleType::setScale)
    .FN("setSpeedDeviation", &libsumo::VehicleType::setSpeedDeviation)
    .class_function("setActionStepLength", &Guard<&vehicleTypeSetActionStepLength>::call);

    emscripten::class_<JsPerson>("Person")
    .ID_PARAMETER_API(Person)
    .VEHICLE_TYPE_GETTER(Person)
    .VEHICLE_TYPE_SETTER(Person)
    .FN("getSpeed", &libsumo::Person::getSpeed)
    .class_function("getPosition", &Guard<&personGetPosition>::call)
    .FN("getPosition3D", &libsumo::Person::getPosition3D)
    .FN("getAngle", &libsumo::Person::getAngle)
    .FN("getSlope", &libsumo::Person::getSlope)
    .FN("getRoadID", &libsumo::Person::getRoadID)
    .FN("getLaneID", &libsumo::Person::getLaneID)
    .FN("getTypeID", &libsumo::Person::getTypeID)
    .FN("getLanePosition", &libsumo::Person::getLanePosition)
    .FN("getWaitingTime", &libsumo::Person::getWaitingTime)
    .FN("getNextEdge", &libsumo::Person::getNextEdge)
    .FN("getVehicle", &libsumo::Person::getVehicle)
    .FN("getRemainingStages", &libsumo::Person::getRemainingStages)
    .class_function("getStage", &Guard<&personGetStage>::call)
    .class_function("getEdges", &Guard<&personGetEdges>::call)
    .class_function("add", &Guard<&personAdd>::call)
    .class_function("remove", &Guard<&personRemove>::call)
    .FN("appendStage", &libsumo::Person::appendStage)
    .FN("replaceStage", &libsumo::Person::replaceStage)
    .FN("appendWaitingStage", &libsumo::Person::appendWaitingStage)
    .class_function("appendWalkingStage", &Guard<&personAppendWalkingStage>::call)
    .FN("appendDrivingStage", &libsumo::Person::appendDrivingStage)
    .FN("removeStage", &libsumo::Person::removeStage)
    .FN("rerouteTraveltime", &libsumo::Person::rerouteTraveltime)
    .FN("moveTo", &libsumo::Person::moveTo)
    .FN("moveToXY", &libsumo::Person::moveToXY)
    .FN("setSpeed", &libsumo::Person::setSpeed)
    .FN("setType", &libsumo::Person::setType)
    .class_function("setActionStepLength", &Guard<&personSetActionStepLength>::call);
}


EMSCRIPTEN_BINDINGS(libsumo_infrastructure) {
    emscripten::class_<JsTrafficLight>("TrafficLight")
    .ID_PARAMETER_API(TrafficLight)
    .FN("getRedYellowGreenState", &libsumo::TrafficLight::getRedYellowGreenState)
    .VEC_FN("getControlledJunctions", &libsumo::TrafficLight::getControlledJunctions)
    .VEC_FN("getControlledLanes", &libsumo::TrafficLight::getControlledLanes)
    .class_function("getControlledLinks", &Guard<&trafficLightGetControlledLinks>::call)
    .class_function("getAllProgramLogics", &Guard<&trafficLightGetAllProgramLogics>::call)
    .FN("getProgram", &libsumo::TrafficLight::getProgram)
    .FN("getPhase", &libsumo::TrafficLight::getPhase)
    .FN("getPhaseName", &libsumo::TrafficLight::getPhaseName)
    .FN("getPhaseDuration", &libsumo::TrafficLight::getPhaseDuration)
    .FN("getNextSwitch", &libsumo::TrafficLight::getNextSwitch)
    .FN("getSpentDuration", &libsumo::TrafficLight::getSpentDuration)
    .FN("getServedPersonCount", &libsumo::TrafficLight::getServedPersonCount)
    .VEC_FN("getBlockingVehicles", &libsumo::TrafficLight::getBlockingVehicles)
    .VEC_FN("getRivalVehicles", &libsumo::TrafficLight::getRivalVehicles)
    .VEC_FN("getPriorityVehicles", &libsumo::TrafficLight::getPriorityVehicles)
    .FN("setRedYellowGreenState", &libsumo::TrafficLight::setRedYellowGreenState)
    .FN("setPhase", &libsumo::TrafficLight::setPhase)
    .FN("setPhaseName", &libsumo::TrafficLight::setPhaseName)
    .FN("setProgram", &libsumo::TrafficLight::setProgram)
    .FN("setPhaseDuration", &libsumo::TrafficLight::setPhaseDuration)
    .class_function("setProgramLogic", &Guard<&trafficLightSetProgramLogic>::call);

    emscripten::class_<JsInductionLoop>("InductionLoop")
    .ID_PARAMETER_API(InductionLoop)
    .FN("getPosition", &libsumo::InductionLoop::getPosition)
    .FN("getLaneID", &libsumo::InductionLoop::getLaneID)
    .FN("getLastStepVehicleNumber", &libsumo::InductionLoop::getLastStepVehicleNumber)
    .FN("getLastStepMeanSpeed", &libsumo::InductionLoop::getLastStepMeanSpeed)
    .VEC_FN("getLastStepVehicleIDs", &libsumo::InductionLoop::getLastStepVehicleIDs)
    .FN("getLastStepOccupancy", &libsumo::InductionLoop::getLastStepOccupancy)
    .FN("getLastStepMeanLength", &libsumo::InductionLoop::getLastStepMeanLength)
    .FN("getTimeSinceDetection", &libsumo::InductionLoop::getTimeSinceDetection)
    .VEC_FN("getVehicleData", &libsumo::InductionLoop::getVehicleData)
    .FN("getIntervalOccupancy", &libsumo::InductionLoop::getIntervalOccupancy)
    .FN("getIntervalMeanSpeed", &libsumo::InductionLoop::getIntervalMeanSpeed)
    .FN("getIntervalVehicleNumber", &libsumo::InductionLoop::getIntervalVehicleNumber)
    .VEC_FN("getIntervalVehicleIDs", &libsumo::InductionLoop::getIntervalVehicleIDs)
    .FN("getLastIntervalOccupancy", &libsumo::InductionLoop::getLastIntervalOccupancy)
    .FN("getLastIntervalMeanSpeed", &libsumo::InductionLoop::getLastIntervalMeanSpeed)
    .FN("getLastIntervalVehicleNumber", &libsumo::InductionLoop::getLastIntervalVehicleNumber)
    .VEC_FN("getLastIntervalVehicleIDs", &libsumo::InductionLoop::getLastIntervalVehicleIDs)
    .FN("overrideTimeSinceDetection", &libsumo::InductionLoop::overrideTimeSinceDetection);

    emscripten::class_<JsLaneArea>("LaneArea")
    .ID_PARAMETER_API(LaneArea)
    .FN("getPosition", &libsumo::LaneArea::getPosition)
    .FN("getLaneID", &libsumo::LaneArea::getLaneID)
    .FN("getLength", &libsumo::LaneArea::getLength)
    .FN("getJamLengthVehicle", &libsumo::LaneArea::getJamLengthVehicle)
    .FN("getJamLengthMeters", &libsumo::LaneArea::getJamLengthMeters)
    .FN("getLastStepMeanSpeed", &libsumo::LaneArea::getLastStepMeanSpeed)
    .VEC_FN("getLastStepVehicleIDs", &libsumo::LaneArea::getLastStepVehicleIDs)
    .FN("getLastStepOccupancy", &libsumo::LaneArea::getLastStepOccupancy)
    .FN("getLastStepVehicleNumber", &libsumo::LaneArea::getLastStepVehicleNumber)
    .FN("getLastStepHaltingNumber", &libsumo::LaneArea::getLastStepHaltingNumber)
    .FN("getIntervalOccupancy", &libsumo::LaneArea::getIntervalOccupancy)
    .FN("getIntervalMeanSpeed", &libsumo::LaneArea::getIntervalMeanSpeed)
    .FN("getIntervalMeanTimeLoss", &libsumo::LaneArea::getIntervalMeanTimeLoss)
    .FN("getIntervalMaxJamLengthInMeters", &libsumo::LaneArea::getIntervalMaxJamLengthInMeters)
    .FN("getIntervalVehicleNumber", &libsumo::LaneArea::getIntervalVehicleNumber)
    .FN("getLastIntervalOccupancy", &libsumo::LaneArea::getLastIntervalOccupancy)
    .FN("getLastIntervalMeanSpeed", &libsumo::LaneArea::getLastIntervalMeanSpeed)
    .FN("getLastIntervalMeanTimeLoss", &libsumo::LaneArea::getLastIntervalMeanTimeLoss)
    .FN("getLastIntervalMaxJamLengthInMeters", &libsumo::LaneArea::getLastIntervalMaxJamLengthInMeters)
    .FN("getLastIntervalVehicleNumber", &libsumo::LaneArea::getLastIntervalVehicleNumber)
    .FN("overrideVehicleNumber", &libsumo::LaneArea::overrideVehicleNumber);

    emscripten::class_<JsMultiEntryExit>("MultiEntryExit")
    .ID_PARAMETER_API(MultiEntryExit)
    .VEC_FN("getEntryLanes", &libsumo::MultiEntryExit::getEntryLanes)
    .VEC_FN("getExitLanes", &libsumo::MultiEntryExit::getExitLanes)
    .VEC_FN("getEntryPositions", &libsumo::MultiEntryExit::getEntryPositions)
    .VEC_FN("getExitPositions", &libsumo::MultiEntryExit::getExitPositions)
    .FN("getLastStepVehicleNumber", &libsumo::MultiEntryExit::getLastStepVehicleNumber)
    .FN("getLastStepMeanSpeed", &libsumo::MultiEntryExit::getLastStepMeanSpeed)
    .VEC_FN("getLastStepVehicleIDs", &libsumo::MultiEntryExit::getLastStepVehicleIDs)
    .FN("getLastStepHaltingNumber", &libsumo::MultiEntryExit::getLastStepHaltingNumber)
    .FN("getLastIntervalMeanTravelTime", &libsumo::MultiEntryExit::getLastIntervalMeanTravelTime)
    .FN("getLastIntervalMeanHaltsPerVehicle", &libsumo::MultiEntryExit::getLastIntervalMeanHaltsPerVehicle)
    .FN("getLastIntervalMeanTimeLoss", &libsumo::MultiEntryExit::getLastIntervalMeanTimeLoss)
    .FN("getLastIntervalVehicleSum", &libsumo::MultiEntryExit::getLastIntervalVehicleSum);

    emscripten::class_<JsPoi>("Poi")
    .ID_PARAMETER_API(POI)
    .FN("getType", &libsumo::POI::getType)
    .class_function("getPosition", &Guard<&poiGetPosition>::call)
    .FN("getColor", &libsumo::POI::getColor)
    .FN("getWidth", &libsumo::POI::getWidth)
    .FN("getHeight", &libsumo::POI::getHeight)
    .FN("getAngle", &libsumo::POI::getAngle)
    .FN("getImageFile", &libsumo::POI::getImageFile)
    .FN("setType", &libsumo::POI::setType)
    .FN("setColor", &libsumo::POI::setColor)
    .FN("setPosition", &libsumo::POI::setPosition)
    .FN("setWidth", &libsumo::POI::setWidth)
    .FN("setHeight", &libsumo::POI::setHeight)
    .FN("setAngle", &libsumo::POI::setAngle)
    .FN("setImageFile", &libsumo::POI::setImageFile)
    .class_function("add", &Guard<&poiAdd>::call)
    .class_function("remove", &Guard<&poiRemove>::call);

    emscripten::class_<JsPolygon>("Polygon")
    .ID_PARAMETER_API(Polygon)
    .FN("getType", &libsumo::Polygon::getType)
    .class_function("getShape", &Guard<&polygonGetShape>::call)
    .FN("getColor", &libsumo::Polygon::getColor)
    .FN("getFilled", &libsumo::Polygon::getFilled)
    .FN("getLineWidth", &libsumo::Polygon::getLineWidth)
    .FN("setType", &libsumo::Polygon::setType)
    .class_function("setShape", &Guard<&polygonSetShape>::call)
    .FN("setColor", &libsumo::Polygon::setColor)
    .FN("setFilled", &libsumo::Polygon::setFilled)
    .FN("setLineWidth", &libsumo::Polygon::setLineWidth)
    .class_function("add", &Guard<&polygonAdd>::call)
    .class_function("remove", &Guard<&polygonRemove>::call);
}


/****************************************************************************/
