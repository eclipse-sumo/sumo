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
#include <type_traits>

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

/** @brief Turns the exception being handled into a proper JavaScript Error.
 *
 * Without this every libsumo error would reach JavaScript as an opaque
 * WebAssembly.Exception without any readable message.
 */
[[noreturn]] void
rethrowCurrentAsJsError() {
    std::string what = "unknown libsumo error";
    try {
        throw;
    } catch (const std::exception& e) {
        what = e.what();
    } catch (...) {}
    val error = val::global("Error").new_(what);
    error.set("name", std::string("TraCIError"));
    error.throw_();
}


template <typename T> constexpr bool isVector = false;
template <typename T> constexpr bool isVector<std::vector<T> > = true;


/** @brief Wraps a libsumo function for embind.
 *
 * C++ exceptions surface as JavaScript errors and results which would need an
 * embind wrapper object the caller has to free by hand become plain JavaScript
 * values instead: a std::vector becomes an Array, a position vector an Array
 * of positions.
 */
template <auto F> struct Guard;

template <typename R, typename... A, R(*F)(A...)>
struct Guard<F> {
    static auto call(A... args) {
        using Result = std::remove_cv_t<R>;
        try {
            if constexpr (std::is_void_v<R>) {
                F(args...);
            } else if constexpr (isVector<Result>) {
                return val::array(F(args...));
            } else if constexpr (std::is_same_v<Result, libsumo::TraCIPositionVector>) {
                return val::array(F(args...).value);
            } else {
                return F(args...);
            }
        } catch (...) {
            rethrowCurrentAsJsError();
        }
    }
};


std::vector<std::string>
toStringVector(const val& array) {
    return emscripten::vecFromJSArray<std::string>(array);
}

// ===========================================================================
// value types which have no direct embind representation
// ===========================================================================
/// @brief the result of Vehicle.getLeader / Vehicle.getFollower
struct JsNeighbor {
    std::string id;
    double dist;
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
// accessors for the value object members embind cannot map by itself
// ===========================================================================
// they turn the member into a plain JavaScript value, so that nothing the
// caller would have to free by hand ends up in a result
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
    phase.next = emscripten::vecFromJSArray<int>(next);
}

val bestLanesGetContinuationLanes(const libsumo::TraCIBestLanesData& data) {
    return val::array(data.continuationLanes);
}

void bestLanesSetContinuationLanes(libsumo::TraCIBestLanesData& data, val lanes) {
    data.continuationLanes = toStringVector(lanes);
}

// the signal state of a single link is a char, which JavaScript has no type for
std::string nextTLSGetState(const libsumo::TraCINextTLSData& data) {
    return std::string(1, data.state);
}

void nextTLSSetState(libsumo::TraCINextTLSData& data, const std::string& state) {
    data.state = state.empty() ? ' ' : state[0];
}

val logicGetPhases(const libsumo::TraCILogic& logic) {
    std::vector<libsumo::TraCIPhase> phases;
    phases.reserve(logic.phases.size());
    for (const std::shared_ptr<libsumo::TraCIPhase>& phase : logic.phases) {
        phases.push_back(*phase);
    }
    return val::array(phases);
}

void logicSetPhases(libsumo::TraCILogic& logic, val phases) {
    logic.phases.clear();
    for (const libsumo::TraCIPhase& phase : emscripten::vecFromJSArray<libsumo::TraCIPhase>(phases)) {
        logic.phases.push_back(std::make_shared<libsumo::TraCIPhase>(phase));
    }
}

val logicGetSubParameter(const libsumo::TraCILogic& logic) {
    val result = val::object();
    for (const auto& item : logic.subParameter) {
        result.set(item.first, item.second);
    }
    return result;
}

void logicSetSubParameter(libsumo::TraCILogic& logic, val subParameter) {
    logic.subParameter.clear();
    for (const std::string& key : toStringVector(val::global("Object").call<val>("keys", subParameter))) {
        logic.subParameter[key] = subParameter[key].as<std::string>();
    }
}

// ===========================================================================
// shims for calls with default arguments, overloads or unbindable types
// ===========================================================================
/// @brief passes a JavaScript array where libsumo wants a list of strings
template <void(*F)(const std::string&, std::vector<std::string>)>
void setStringList(const std::string& objectID, const val& list) {
    F(objectID, toStringVector(list));
}

/// @brief the same for the functions taking that list by const reference
template <void(*F)(const std::string&, const std::vector<std::string>&)>
void setStringListRef(const std::string& objectID, const val& list) {
    F(objectID, toStringVector(list));
}

/// @brief keeps the action offset, which is what the other clients default to
template <void(*F)(const std::string&, double, bool)>
void setActionStepLength(const std::string& objectID, double actionStepLength) {
    F(objectID, actionStepLength, true);
}

// --- Simulation ---
void simulationLoad(const val& args) {
    libsumo::Simulation::load(toStringVector(args));
}

void simulationClose() {
    libsumo::Simulation::close();
}

JsVersion simulationGetVersion() {
    std::pair<int, std::string> version = libsumo::Simulation::getVersion();
    return JsVersion{version.first, std::move(version.second)};
}

libsumo::TraCIStage simulationFindRoute(const std::string& fromEdge, const std::string& toEdge,
                                        const std::string& vType, double depart, int routingMode) {
    return libsumo::Simulation::findRoute(fromEdge, toEdge, vType, depart, routingMode);
}

// --- Edge ---
double edgeGetAngle(const std::string& edgeID) {
    return libsumo::Edge::getAngle(edgeID);
}

// --- Lane ---
double laneGetAngle(const std::string& laneID) {
    return libsumo::Lane::getAngle(laneID);
}

// --- Junction ---
libsumo::TraCIPosition junctionGetPosition(const std::string& junctionID) {
    return libsumo::Junction::getPosition(junctionID);
}

// --- Vehicle ---
libsumo::TraCIPosition vehicleGetPosition(const std::string& vehID) {
    return libsumo::Vehicle::getPosition(vehID);
}

JsNeighbor vehicleGetLeader(const std::string& vehID, double dist) {
    std::pair<std::string, double> leader = libsumo::Vehicle::getLeader(vehID, dist);
    return JsNeighbor{std::move(leader.first), leader.second};
}

JsNeighbor vehicleGetFollower(const std::string& vehID, double dist) {
    std::pair<std::string, double> follower = libsumo::Vehicle::getFollower(vehID, dist);
    return JsNeighbor{std::move(follower.first), follower.second};
}

void vehicleAdd(const std::string& vehID, const std::string& routeID, const std::string& typeID,
                const std::string& depart, const std::string& departLane, const std::string& departPos,
                const std::string& departSpeed) {
    libsumo::Vehicle::add(vehID, routeID, typeID, depart, departLane, departPos, departSpeed);
}

void vehicleRemove(const std::string& vehID, int reason) {
    libsumo::Vehicle::remove(vehID, (char)reason);
}

double vehicleGetDrivingDistance(const std::string& vehID, const std::string& edgeID, double pos) {
    return libsumo::Vehicle::getDrivingDistance(vehID, edgeID, pos);
}

// --- Person ---
libsumo::TraCIPosition personGetPosition(const std::string& personID) {
    return libsumo::Person::getPosition(personID);
}

void personAppendWalkingStage(const std::string& personID, const val& edges, double arrivalPos,
                              double duration, double speed, const std::string& stopID) {
    libsumo::Person::appendWalkingStage(personID, toStringVector(edges), arrivalPos, duration, speed, stopID);
}

void personRemove(const std::string& personID, int reason) {
    libsumo::Person::remove(personID, (char)reason);
}

// --- TrafficLight ---
// the inner vector of links is not registered, so it needs a manual conversion
val trafficLightGetControlledLinks(const std::string& tlsID) {
    val result = val::array();
    int index = 0;
    for (const std::vector<libsumo::TraCILink>& links : libsumo::TrafficLight::getControlledLinks(tlsID)) {
        result.set(index++, val::array(links));
    }
    return result;
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

// --- Polygon ---
libsumo::TraCIPositionVector toPositionVector(const val& array) {
    libsumo::TraCIPositionVector shape;
    shape.value = emscripten::vecFromJSArray<libsumo::TraCIPosition>(array);
    return shape;
}

void polygonSetShape(const std::string& polygonID, const val& shape) {
    libsumo::Polygon::setShape(polygonID, toPositionVector(shape));
}

void polygonAdd(const std::string& polygonID, const val& shape, const libsumo::TraCIColor& color,
                bool fill, const std::string& polygonType, int layer, double lineWidth) {
    libsumo::Polygon::add(polygonID, toPositionVector(shape), color, fill, polygonType, layer, lineWidth);
}

}  // namespace


// ===========================================================================
// bindings
// ===========================================================================
#define FN(NAME, FUNC) class_function(NAME, &Guard<FUNC>::call)

// getIDList / getIDCount / getParameter / setParameter of a libsumo domain.
// getParameterWithKey of the C++ API is left out, it only exists to carry the
// key through a subscription and subscriptions are not bound here.
#define ID_PARAMETER_API(CLASS) \
    FN("getIDList", &libsumo::CLASS::getIDList) \
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

    emscripten::value_object<libsumo::TraCINextTLSData>("NextTLS")
    .field("id", &libsumo::TraCINextTLSData::id)
    .field("tlIndex", &libsumo::TraCINextTLSData::tlIndex)
    .field("dist", &libsumo::TraCINextTLSData::dist)
    .field("state", &nextTLSGetState, &nextTLSSetState);

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

    emscripten::value_object<libsumo::TraCILogic>("Logic")
    .field("programID", &libsumo::TraCILogic::programID)
    .field("type", &libsumo::TraCILogic::type)
    .field("currentPhaseIndex", &libsumo::TraCILogic::currentPhaseIndex)
    .field("phases", &logicGetPhases, &logicSetPhases)
    .field("subParameter", &logicGetSubParameter, &logicSetSubParameter);

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
    .FN("step", &libsumo::Simulation::step)
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
    .FN("getLoadedIDList", &libsumo::Simulation::getLoadedIDList)
    .FN("getDepartedNumber", &libsumo::Simulation::getDepartedNumber)
    .FN("getDepartedIDList", &libsumo::Simulation::getDepartedIDList)
    .FN("getArrivedNumber", &libsumo::Simulation::getArrivedNumber)
    .FN("getArrivedIDList", &libsumo::Simulation::getArrivedIDList)
    .FN("getCollidingVehiclesNumber", &libsumo::Simulation::getCollidingVehiclesNumber)
    .FN("getCollidingVehiclesIDList", &libsumo::Simulation::getCollidingVehiclesIDList)
    .FN("getStartingTeleportNumber", &libsumo::Simulation::getStartingTeleportNumber)
    .FN("getStartingTeleportIDList", &libsumo::Simulation::getStartingTeleportIDList)
    .FN("getEndingTeleportNumber", &libsumo::Simulation::getEndingTeleportNumber)
    .FN("getEndingTeleportIDList", &libsumo::Simulation::getEndingTeleportIDList)
    .FN("getDepartedPersonNumber", &libsumo::Simulation::getDepartedPersonNumber)
    .FN("getDepartedPersonIDList", &libsumo::Simulation::getDepartedPersonIDList)
    .FN("getArrivedPersonNumber", &libsumo::Simulation::getArrivedPersonNumber)
    .FN("getArrivedPersonIDList", &libsumo::Simulation::getArrivedPersonIDList)
    .FN("getBusStopIDList", &libsumo::Simulation::getBusStopIDList)
    .FN("getBusStopWaiting", &libsumo::Simulation::getBusStopWaiting)
    .FN("getBusStopWaitingIDList", &libsumo::Simulation::getBusStopWaitingIDList)
    .FN("getPendingVehicles", &libsumo::Simulation::getPendingVehicles)
    .FN("getNetBoundary", &libsumo::Simulation::getNetBoundary)
    .FN("convert2D", &libsumo::Simulation::convert2D)
    .FN("convertGeo", &libsumo::Simulation::convertGeo)
    .FN("convertRoad", &libsumo::Simulation::convertRoad)
    .FN("getDistance2D", &libsumo::Simulation::getDistance2D)
    .FN("getDistanceRoad", &libsumo::Simulation::getDistanceRoad)
    .class_function("findRoute", &Guard<&simulationFindRoute>::call)
    .FN("getScale", &libsumo::Simulation::getScale)
    .FN("setScale", &libsumo::Simulation::setScale)
    .FN("clearPending", &libsumo::Simulation::clearPending)
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
    .FN("getLastStepPersonIDs", &libsumo::Edge::getLastStepPersonIDs)
    .FN("getLastStepVehicleIDs", &libsumo::Edge::getLastStepVehicleIDs)
    .FN("getPendingVehicles", &libsumo::Edge::getPendingVehicles)
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
    .FN("setAllowed", &setStringList<&libsumo::Edge::setAllowed>)
    .FN("setDisallowed", &setStringList<&libsumo::Edge::setDisallowed>)
    .FN("adaptTraveltime", &libsumo::Edge::adaptTraveltime)
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
    .FN("getAllowed", &libsumo::Lane::getAllowed)
    .FN("getDisallowed", &libsumo::Lane::getDisallowed)
    .FN("getChangePermissions", &libsumo::Lane::getChangePermissions)
    .FN("getLinks", &libsumo::Lane::getLinks)
    .FN("getFoes", &libsumo::Lane::getFoes)
    .FN("getInternalFoes", &libsumo::Lane::getInternalFoes)
    .FN("getLastStepVehicleIDs", &libsumo::Lane::getLastStepVehicleIDs)
    .FN("getPendingVehicles", &libsumo::Lane::getPendingVehicles)
    .FN("getShape", &libsumo::Lane::getShape)
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
    .FN("setAllowed", &setStringList<&libsumo::Lane::setAllowed>)
    .FN("setDisallowed", &setStringList<&libsumo::Lane::setDisallowed>)
    .FN("setMaxSpeed", &libsumo::Lane::setMaxSpeed)
    .FN("setLength", &libsumo::Lane::setLength)
    .FN("setFriction", &libsumo::Lane::setFriction);

    emscripten::class_<JsJunction>("Junction")
    .ID_PARAMETER_API(Junction)
    .class_function("getPosition", &Guard<&junctionGetPosition>::call)
    .FN("getShape", &libsumo::Junction::getShape)
    .FN("getIncomingEdges", &libsumo::Junction::getIncomingEdges)
    .FN("getOutgoingEdges", &libsumo::Junction::getOutgoingEdges);

    emscripten::class_<JsRoute>("Route")
    .ID_PARAMETER_API(Route)
    .FN("getEdges", &libsumo::Route::getEdges)
    .FN("add", &setStringListRef<&libsumo::Route::add>)
    .FN("remove", &libsumo::Route::remove);
}


EMSCRIPTEN_BINDINGS(libsumo_vehicle) {
    emscripten::class_<JsVehicle>("Vehicle")
    .ID_PARAMETER_API(Vehicle)
    .VEHICLE_TYPE_GETTER(Vehicle)
    .VEHICLE_TYPE_SETTER(Vehicle)
    .FN("getLoadedIDList", &libsumo::Vehicle::getLoadedIDList)
    .FN("getTeleportingIDList", &libsumo::Vehicle::getTeleportingIDList)
    .FN("getTaxiFleet", &libsumo::Vehicle::getTaxiFleet)
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
    .FN("getRoute", &libsumo::Vehicle::getRoute)
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
    .FN("getPersonIDList", &libsumo::Vehicle::getPersonIDList)
    .class_function("getLeader", &Guard<&vehicleGetLeader>::call)
    .class_function("getFollower", &Guard<&vehicleGetFollower>::call)
    .FN("getWaitingTime", &libsumo::Vehicle::getWaitingTime)
    .FN("getAccumulatedWaitingTime", &libsumo::Vehicle::getAccumulatedWaitingTime)
    .FN("getTimeLoss", &libsumo::Vehicle::getTimeLoss)
    .FN("isRouteValid", &libsumo::Vehicle::isRouteValid)
    .FN("getSignals", &libsumo::Vehicle::getSignals)
    .FN("getBestLanes", &libsumo::Vehicle::getBestLanes)
    .FN("getNextTLS", &libsumo::Vehicle::getNextTLS)
    .FN("getNextStops", &libsumo::Vehicle::getNextStops)
    .FN("getStops", &libsumo::Vehicle::getStops)
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
    .FN("getVia", &libsumo::Vehicle::getVia)
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
    .FN("setRoute", &setStringListRef<&libsumo::Vehicle::setRoute>)
    .FN("setVia", &setStringListRef<&libsumo::Vehicle::setVia>)
    .FN("setLateralLanePosition", &libsumo::Vehicle::setLateralLanePosition)
    .FN("updateBestLanes", &libsumo::Vehicle::updateBestLanes)
    .FN("rerouteTraveltime", &libsumo::Vehicle::rerouteTraveltime)
    .FN("rerouteEffort", &libsumo::Vehicle::rerouteEffort)
    .FN("setSignals", &libsumo::Vehicle::setSignals)
    .FN("moveTo", &libsumo::Vehicle::moveTo)
    .FN("moveToXY", &libsumo::Vehicle::moveToXY)
    .FN("setLine", &libsumo::Vehicle::setLine)
    .FN("resume", &libsumo::Vehicle::resume)
    .FN("setActionStepLength", &setActionStepLength<&libsumo::Vehicle::setActionStepLength>);

    emscripten::class_<JsVehicleType>("VehicleType")
    .ID_PARAMETER_API(VehicleType)
    .VEHICLE_TYPE_GETTER(VehicleType)
    .VEHICLE_TYPE_SETTER(VehicleType)
    .FN("copy", &libsumo::VehicleType::copy)
    .FN("getScale", &libsumo::VehicleType::getScale)
    .FN("setScale", &libsumo::VehicleType::setScale)
    .FN("setSpeedDeviation", &libsumo::VehicleType::setSpeedDeviation)
    .FN("setActionStepLength", &setActionStepLength<&libsumo::VehicleType::setActionStepLength>);

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
    .FN("getStage", &libsumo::Person::getStage)
    .FN("getEdges", &libsumo::Person::getEdges)
    .FN("add", &libsumo::Person::add)
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
    .FN("setActionStepLength", &setActionStepLength<&libsumo::Person::setActionStepLength>);
}


EMSCRIPTEN_BINDINGS(libsumo_infrastructure) {
    emscripten::class_<JsTrafficLight>("TrafficLight")
    .ID_PARAMETER_API(TrafficLight)
    .FN("getRedYellowGreenState", &libsumo::TrafficLight::getRedYellowGreenState)
    .FN("getControlledJunctions", &libsumo::TrafficLight::getControlledJunctions)
    .FN("getControlledLanes", &libsumo::TrafficLight::getControlledLanes)
    .class_function("getControlledLinks", &Guard<&trafficLightGetControlledLinks>::call)
    .FN("getAllProgramLogics", &libsumo::TrafficLight::getAllProgramLogics)
    .FN("getProgram", &libsumo::TrafficLight::getProgram)
    .FN("getPhase", &libsumo::TrafficLight::getPhase)
    .FN("getPhaseName", &libsumo::TrafficLight::getPhaseName)
    .FN("getPhaseDuration", &libsumo::TrafficLight::getPhaseDuration)
    .FN("getNextSwitch", &libsumo::TrafficLight::getNextSwitch)
    .FN("getSpentDuration", &libsumo::TrafficLight::getSpentDuration)
    .FN("getServedPersonCount", &libsumo::TrafficLight::getServedPersonCount)
    .FN("getBlockingVehicles", &libsumo::TrafficLight::getBlockingVehicles)
    .FN("getRivalVehicles", &libsumo::TrafficLight::getRivalVehicles)
    .FN("getPriorityVehicles", &libsumo::TrafficLight::getPriorityVehicles)
    .FN("setRedYellowGreenState", &libsumo::TrafficLight::setRedYellowGreenState)
    .FN("setPhase", &libsumo::TrafficLight::setPhase)
    .FN("setPhaseName", &libsumo::TrafficLight::setPhaseName)
    .FN("setProgram", &libsumo::TrafficLight::setProgram)
    .FN("setPhaseDuration", &libsumo::TrafficLight::setPhaseDuration)
    .FN("setProgramLogic", &libsumo::TrafficLight::setProgramLogic);

    emscripten::class_<JsInductionLoop>("InductionLoop")
    .ID_PARAMETER_API(InductionLoop)
    .FN("getPosition", &libsumo::InductionLoop::getPosition)
    .FN("getLaneID", &libsumo::InductionLoop::getLaneID)
    .FN("getLastStepVehicleNumber", &libsumo::InductionLoop::getLastStepVehicleNumber)
    .FN("getLastStepMeanSpeed", &libsumo::InductionLoop::getLastStepMeanSpeed)
    .FN("getLastStepVehicleIDs", &libsumo::InductionLoop::getLastStepVehicleIDs)
    .FN("getLastStepOccupancy", &libsumo::InductionLoop::getLastStepOccupancy)
    .FN("getLastStepMeanLength", &libsumo::InductionLoop::getLastStepMeanLength)
    .FN("getTimeSinceDetection", &libsumo::InductionLoop::getTimeSinceDetection)
    .FN("getVehicleData", &libsumo::InductionLoop::getVehicleData)
    .FN("getIntervalOccupancy", &libsumo::InductionLoop::getIntervalOccupancy)
    .FN("getIntervalMeanSpeed", &libsumo::InductionLoop::getIntervalMeanSpeed)
    .FN("getIntervalVehicleNumber", &libsumo::InductionLoop::getIntervalVehicleNumber)
    .FN("getIntervalVehicleIDs", &libsumo::InductionLoop::getIntervalVehicleIDs)
    .FN("getLastIntervalOccupancy", &libsumo::InductionLoop::getLastIntervalOccupancy)
    .FN("getLastIntervalMeanSpeed", &libsumo::InductionLoop::getLastIntervalMeanSpeed)
    .FN("getLastIntervalVehicleNumber", &libsumo::InductionLoop::getLastIntervalVehicleNumber)
    .FN("getLastIntervalVehicleIDs", &libsumo::InductionLoop::getLastIntervalVehicleIDs)
    .FN("overrideTimeSinceDetection", &libsumo::InductionLoop::overrideTimeSinceDetection);

    emscripten::class_<JsLaneArea>("LaneArea")
    .ID_PARAMETER_API(LaneArea)
    .FN("getPosition", &libsumo::LaneArea::getPosition)
    .FN("getLaneID", &libsumo::LaneArea::getLaneID)
    .FN("getLength", &libsumo::LaneArea::getLength)
    .FN("getJamLengthVehicle", &libsumo::LaneArea::getJamLengthVehicle)
    .FN("getJamLengthMeters", &libsumo::LaneArea::getJamLengthMeters)
    .FN("getLastStepMeanSpeed", &libsumo::LaneArea::getLastStepMeanSpeed)
    .FN("getLastStepVehicleIDs", &libsumo::LaneArea::getLastStepVehicleIDs)
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
    .FN("getEntryLanes", &libsumo::MultiEntryExit::getEntryLanes)
    .FN("getExitLanes", &libsumo::MultiEntryExit::getExitLanes)
    .FN("getEntryPositions", &libsumo::MultiEntryExit::getEntryPositions)
    .FN("getExitPositions", &libsumo::MultiEntryExit::getExitPositions)
    .FN("getLastStepVehicleNumber", &libsumo::MultiEntryExit::getLastStepVehicleNumber)
    .FN("getLastStepMeanSpeed", &libsumo::MultiEntryExit::getLastStepMeanSpeed)
    .FN("getLastStepVehicleIDs", &libsumo::MultiEntryExit::getLastStepVehicleIDs)
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
    .FN("remove", &libsumo::POI::remove);

    emscripten::class_<JsPolygon>("Polygon")
    .ID_PARAMETER_API(Polygon)
    .FN("getType", &libsumo::Polygon::getType)
    .FN("getShape", &libsumo::Polygon::getShape)
    .FN("getColor", &libsumo::Polygon::getColor)
    .FN("getFilled", &libsumo::Polygon::getFilled)
    .FN("getLineWidth", &libsumo::Polygon::getLineWidth)
    .FN("setType", &libsumo::Polygon::setType)
    .class_function("setShape", &Guard<&polygonSetShape>::call)
    .FN("setColor", &libsumo::Polygon::setColor)
    .FN("setFilled", &libsumo::Polygon::setFilled)
    .FN("setLineWidth", &libsumo::Polygon::setLineWidth)
    .class_function("add", &Guard<&polygonAdd>::call)
    .FN("remove", &libsumo::Polygon::remove);
}


/****************************************************************************/
