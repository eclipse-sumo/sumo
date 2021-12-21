/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
/// @file    TraCIDefs.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
// we do not include config.h here, since we should be independent of a special sumo build
#include <libsumo/TraCIConstants.h>
#include <vector>
#include <limits>
#include <map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <cstring>


// ===========================================================================
// common declarations
// ===========================================================================
namespace libsumo {
class VariableWrapper;
}
namespace tcpip {
class Storage;
}


// ===========================================================================
// global definitions
// ===========================================================================
#ifdef LIBTRACI
#define LIBSUMO_NAMESPACE libtraci
#else
#define LIBSUMO_NAMESPACE libsumo
#endif

#define LIBSUMO_SUBSCRIPTION_API \
static void subscribe(const std::string& objectID, const std::vector<int>& varIDs = std::vector<int>({-1}), \
                      double begin = libsumo::INVALID_DOUBLE_VALUE, double end = libsumo::INVALID_DOUBLE_VALUE, const libsumo::TraCIResults& params = libsumo::TraCIResults()); \
static void unsubscribe(const std::string& objectID); \
static void subscribeContext(const std::string& objectID, int domain, double dist, const std::vector<int>& varIDs = std::vector<int>({-1}), \
                             double begin = libsumo::INVALID_DOUBLE_VALUE, double end = libsumo::INVALID_DOUBLE_VALUE, const libsumo::TraCIResults& params = libsumo::TraCIResults()); \
static void unsubscribeContext(const std::string& objectID, int domain, double dist); \
static const libsumo::SubscriptionResults getAllSubscriptionResults(); \
static const libsumo::TraCIResults getSubscriptionResults(const std::string& objectID); \
static const libsumo::ContextSubscriptionResults getAllContextSubscriptionResults(); \
static const libsumo::SubscriptionResults getContextSubscriptionResults(const std::string& objectID); \
static void subscribeParameterWithKey(const std::string& objectID, const std::string& key, double beginTime = libsumo::INVALID_DOUBLE_VALUE, double endTime = libsumo::INVALID_DOUBLE_VALUE);

#define LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(CLASS, DOMAIN) \
void \
CLASS::subscribe(const std::string& objectID, const std::vector<int>& varIDs, double begin, double end, const libsumo::TraCIResults& params) { \
    libsumo::Helper::subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_VARIABLE, objectID, varIDs, begin, end, params); \
} \
void \
CLASS::unsubscribe(const std::string& objectID) { \
    libsumo::Helper::subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_VARIABLE, objectID, std::vector<int>(), libsumo::INVALID_DOUBLE_VALUE, libsumo::INVALID_DOUBLE_VALUE, libsumo::TraCIResults()); \
} \
void \
CLASS::subscribeContext(const std::string& objectID, int domain, double dist, const std::vector<int>& varIDs, double begin, double end, const TraCIResults& params) { \
    libsumo::Helper::subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_CONTEXT, objectID, varIDs, begin, end, params, domain, dist); \
} \
void \
CLASS::unsubscribeContext(const std::string& objectID, int domain, double dist) { \
    libsumo::Helper::subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_CONTEXT, objectID, std::vector<int>(), libsumo::INVALID_DOUBLE_VALUE, libsumo::INVALID_DOUBLE_VALUE, libsumo::TraCIResults(), domain, dist); \
} \
const libsumo::SubscriptionResults \
CLASS::getAllSubscriptionResults() { \
    return mySubscriptionResults; \
} \
const libsumo::TraCIResults \
CLASS::getSubscriptionResults(const std::string& objectID) { \
    return mySubscriptionResults[objectID]; \
} \
const libsumo::ContextSubscriptionResults \
CLASS::getAllContextSubscriptionResults() { \
    return myContextSubscriptionResults; \
} \
const libsumo::SubscriptionResults \
CLASS::getContextSubscriptionResults(const std::string& objectID) { \
    return myContextSubscriptionResults[objectID]; \
} \
void \
CLASS::subscribeParameterWithKey(const std::string& objectID, const std::string& key, double beginTime, double endTime) { \
    libsumo::Helper::subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_VARIABLE, objectID, std::vector<int>({libsumo::VAR_PARAMETER_WITH_KEY}), beginTime, endTime, libsumo::TraCIResults {{libsumo::VAR_PARAMETER_WITH_KEY, std::make_shared<libsumo::TraCIString>(key)}}); \
}


#define LIBSUMO_ID_PARAMETER_API \
static std::vector<std::string> getIDList(); \
static int getIDCount(); \
static std::string getParameter(const std::string& objectID, const std::string& param); \
static const std::pair<std::string, std::string> getParameterWithKey(const std::string& objectID, const std::string& key); \
static void setParameter(const std::string& objectID, const std::string& param, const std::string& value);

#define LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(CLASS) \
const std::pair<std::string, std::string> \
CLASS::getParameterWithKey(const std::string& objectID, const std::string& key) { \
    return std::make_pair(key, getParameter(objectID, key)); \
}


// ===========================================================================
// class and type definitions
// ===========================================================================
namespace libsumo {
/**
 * @class TraCIException
 * @brief An error which allows to continue
 */
class TraCIException : public std::runtime_error {
public:
    /** constructor */
    TraCIException(std::string what)
        : std::runtime_error(what) {}
};

/**
 * @class FatalTraCIError
 * @brief An error which is not recoverable
 */
class FatalTraCIError : public std::runtime_error {
public:
    /** constructor */
    FatalTraCIError(std::string what)
        : std::runtime_error(what) {}
};

/// @name Structures definitions
/// @{

struct TraCIResult {
    virtual ~TraCIResult() {}
    virtual std::string getString() const {
        return "";
    }
    virtual int getType() const {
        return -1;
    }
};

/** @struct TraCIPosition
 * @brief A 3D-position
 */
struct TraCIPosition : TraCIResult {
    std::string getString() const {
        std::ostringstream os;
        os << "TraCIPosition(" << x << "," << y << "," << z << ")";
        return os.str();
    }
    double x = INVALID_DOUBLE_VALUE, y = INVALID_DOUBLE_VALUE, z = INVALID_DOUBLE_VALUE;
};

/** @struct TraCIRoadPosition
 * @brief An edgeId, position and laneIndex
 */
struct TraCIRoadPosition : TraCIResult {
    TraCIRoadPosition() {}
    TraCIRoadPosition(const std::string e, const double p) : edgeID(e), pos(p) {}
    std::string getString() const {
        std::ostringstream os;
        os << "TraCIRoadPosition(" << edgeID << "_" << laneIndex << "," << pos << ")";
        return os.str();
    }
    std::string edgeID = "";
    double pos = INVALID_DOUBLE_VALUE;
    int laneIndex = INVALID_INT_VALUE;
};

/** @struct TraCIColor
 * @brief A color
 */
struct TraCIColor : TraCIResult {
    TraCIColor() : r(0), g(0), b(0), a(255) {}
    TraCIColor(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
    std::string getString() const {
        std::ostringstream os;
        os << "TraCIColor(" << r << "," << g << "," << b << "," << a << ")";
        return os.str();
    }
    int r, g, b, a;
};


/** @struct TraCIPositionVector
 * @brief A list of positions
 */
struct TraCIPositionVector : TraCIResult {
    std::string getString() const {
        std::ostringstream os;
        os << "[";
        for (const TraCIPosition& v : value) {
            os << "(" << v.x << "," << v.y << "," << v.z << ")";
        }
        os << "]";
        return os.str();
    }
    std::vector<TraCIPosition> value;
};


struct TraCIInt : TraCIResult {
    TraCIInt() : value(0) {}
    TraCIInt(int v) : value(v) {}
    std::string getString() const {
        std::ostringstream os;
        os << value;
        return os.str();
    }
    int value;
};


struct TraCIDouble : TraCIResult {
    TraCIDouble() : value(0.) {}
    TraCIDouble(double v) : value(v) {}
    std::string getString() const {
        std::ostringstream os;
        os << value;
        return os.str();
    }
    int getType() const {
        return libsumo::TYPE_DOUBLE;
    }
    double value;
};


struct TraCIString : TraCIResult {
    TraCIString() : value("") {}
    TraCIString(std::string v) : value(v) {}
    std::string getString() const {
        return value;
    }
    int getType() const {
        return libsumo::TYPE_STRING;
    }
    std::string value;
};


struct TraCIStringList : TraCIResult {
    std::string getString() const {
        std::ostringstream os;
        os << "[";
        for (std::string v : value) {
            os << v << ",";
        }
        os << "]";
        return os.str();
    }
    std::vector<std::string> value;
};


/// @brief {variable->value}
typedef std::map<int, std::shared_ptr<libsumo::TraCIResult> > TraCIResults;
/// @brief {object->{variable->value}}
typedef std::map<std::string, libsumo::TraCIResults> SubscriptionResults;
typedef std::map<std::string, libsumo::SubscriptionResults> ContextSubscriptionResults;


class TraCIPhase {
public:
    TraCIPhase() {}
    TraCIPhase(const double _duration, const std::string& _state, const double _minDur = libsumo::INVALID_DOUBLE_VALUE,
               const double _maxDur = libsumo::INVALID_DOUBLE_VALUE,
               const std::vector<int>& _next = std::vector<int>(),
               const std::string& _name = "") :
        duration(_duration), state(_state), minDur(_minDur), maxDur(_maxDur), next(_next), name(_name) {}
    ~TraCIPhase() {}

    double duration;
    std::string state;
    double minDur, maxDur;
    std::vector<int> next;
    std::string name;
};
}


#ifdef SWIG
%template(TraCIPhaseVector) std::vector<std::shared_ptr<libsumo::TraCIPhase> >; // *NOPAD*
#endif


namespace libsumo {
class TraCILogic {
public:
    TraCILogic() {}
    TraCILogic(const std::string& _programID, const int _type, const int _currentPhaseIndex,
               const std::vector<std::shared_ptr<libsumo::TraCIPhase> >& _phases = std::vector<std::shared_ptr<libsumo::TraCIPhase> >())
        : programID(_programID), type(_type), currentPhaseIndex(_currentPhaseIndex), phases(_phases) {}
    ~TraCILogic() {}

    std::string programID;
    int type;
    int currentPhaseIndex;
    std::vector<std::shared_ptr<libsumo::TraCIPhase> > phases;
    std::map<std::string, std::string> subParameter;
};


class TraCILink {
public:
    TraCILink() {}
    TraCILink(const std::string& _from, const std::string& _via, const std::string& _to)
        : fromLane(_from), viaLane(_via), toLane(_to) {}
    ~TraCILink() {}

    std::string fromLane;
    std::string viaLane;
    std::string toLane;
};


class TraCIConnection {
public:
    TraCIConnection() {} // this is needed by SWIG when building a vector of this type, please don't use it
    TraCIConnection(const std::string& _approachedLane, const bool _hasPrio, const bool _isOpen, const bool _hasFoe,
                    const std::string _approachedInternal, const std::string _state, const std::string _direction, const double _length)
        : approachedLane(_approachedLane), hasPrio(_hasPrio), isOpen(_isOpen), hasFoe(_hasFoe),
          approachedInternal(_approachedInternal), state(_state), direction(_direction), length(_length) {}
    ~TraCIConnection() {}

    std::string approachedLane;
    bool hasPrio;
    bool isOpen;
    bool hasFoe;
    std::string approachedInternal;
    std::string state;
    std::string direction;
    double length;
};


/// @brief mirrors MSInductLoop::VehicleData
struct TraCIVehicleData {
    /// @brief The id of the vehicle
    std::string id;
    /// @brief Length of the vehicle
    double length;
    /// @brief Entry-time of the vehicle in [s]
    double entryTime;
    /// @brief Leave-time of the vehicle in [s]
    double leaveTime;
    /// @brief Type of the vehicle in
    std::string typeID;
};


struct TraCINextTLSData {
    /// @brief The id of the next tls
    std::string id;
    /// @brief The tls index of the controlled link
    int tlIndex;
    /// @brief The distance to the tls
    double dist;
    /// @brief The current state of the tls
    char state;
};


struct TraCINextStopData : TraCIResult {

    TraCINextStopData(const std::string& lane = "",
                      double startPos = INVALID_DOUBLE_VALUE,
                      double endPos = INVALID_DOUBLE_VALUE,
                      const std::string& stoppingPlaceID = "",
                      int stopFlags = 0,
                      double duration = INVALID_DOUBLE_VALUE,
                      double until = INVALID_DOUBLE_VALUE,
                      double intendedArrival = INVALID_DOUBLE_VALUE,
                      double arrival = INVALID_DOUBLE_VALUE,
                      double depart = INVALID_DOUBLE_VALUE,
                      const std::string& split = "",
                      const std::string& join = "",
                      const std::string& actType = "",
                      const std::string& tripId = "",
                      const std::string& line = "",
                      double speed = 0):
        lane(lane),
        startPos(startPos),
        endPos(endPos),
        stoppingPlaceID(stoppingPlaceID),
        stopFlags(stopFlags),
        duration(duration),
        until(until),
        intendedArrival(intendedArrival),
        arrival(arrival),
        depart(depart),
        split(split),
        join(join),
        actType(actType),
        tripId(tripId),
        line(line),
        speed(speed)
    {}

    std::string getString() const {
        std::ostringstream os;
        os << "TraCINextStopData(" << lane << "," << endPos << "," << stoppingPlaceID
           << "," << stopFlags << "," << duration << "," << until
           << "," << arrival << ")";
        return os.str();
    }

    /// @brief The lane to stop at
    std::string lane;
    /// @brief The stopping position start
    double startPos;
    /// @brief The stopping position end
    double endPos;
    /// @brief Id assigned to the stop
    std::string stoppingPlaceID;
    /// @brief Stop flags
    int stopFlags;
    /// @brief The intended (minimum) stopping duration
    double duration;
    /// @brief The time at which the vehicle may continue its journey
    double until;
    /// @brief The intended arrival time
    double intendedArrival;
    /// @brief The actual arrival time (only for past stops)
    double arrival;
    /// @brief The time at which this stop was ended
    double depart;
    /// @brief the id of the vehicle (train portion) that splits of upon reaching this stop
    std::string split;
    /// @brief the id of the vehicle (train portion) to which this vehicle shall be joined
    std::string join;
    /// @brief additional information for this stop
    std::string actType;
    /// @brief id of the trip within a cyclical public transport route
    std::string tripId;
    /// @brief the new line id of the trip within a cyclical public transport route
    std::string line;
    /// @brief the speed at which this stop counts as reached (waypoint mode)
    double speed;
};


/** @struct TraCINextStopDataVector
 * @brief A list of vehicle stops
 * @see TraCINextStopData
 */
struct TraCINextStopDataVector : TraCIResult {
    std::string getString() const {
        std::ostringstream os;
        os << "TraCINextStopDataVector[";
        for (TraCINextStopData v : value) {
            os << v.getString() << ",";
        }
        os << "]";
        return os.str();
    }

    std::vector<TraCINextStopData> value;
};


struct TraCIBestLanesData {
    /// @brief The id of the lane
    std::string laneID;
    /// @brief The length than can be driven from that lane without lane change
    double length;
    /// @brief The traffic density along length
    double occupation;
    /// @brief The offset of this lane from the best lane
    int bestLaneOffset;
    /// @brief Whether this lane allows continuing the route
    bool allowsContinuation;
    /// @brief The sequence of lanes that best allows continuing the route without lane change
    std::vector<std::string> continuationLanes;
};


class TraCIStage {
public:
    TraCIStage(int type = INVALID_INT_VALUE, const std::string& vType = "", const std::string& line = "", const std::string& destStop = "",
               const std::vector<std::string>& edges = std::vector<std::string>(),
               double travelTime = INVALID_DOUBLE_VALUE, double cost = INVALID_DOUBLE_VALUE, double length = INVALID_DOUBLE_VALUE,
               const std::string& intended = "", double depart = INVALID_DOUBLE_VALUE, double departPos = INVALID_DOUBLE_VALUE,
               double arrivalPos = INVALID_DOUBLE_VALUE, const std::string& description = "") :
        type(type), vType(vType), line(line), destStop(destStop), edges(edges), travelTime(travelTime), cost(cost),
        length(length), intended(intended), depart(depart), departPos(departPos), arrivalPos(arrivalPos), description(description) {}
    /// @brief The type of stage (walking, driving, ...)
    int type;
    /// @brief The vehicle type when using a private car or bike
    std::string vType;
    /// @brief The line or the id of the vehicle type
    std::string line;
    /// @brief The id of the destination stop
    std::string destStop;
    /// @brief The sequence of edges to travel
    std::vector<std::string> edges;
    /// @brief duration of the stage in seconds
    double travelTime;
    /// @brief effort needed
    double cost;
    /// @brief length in m
    double length;
    /// @brief id of the intended vehicle for public transport ride
    std::string intended;
    /// @brief intended depart time for public transport ride or INVALID_DOUBLE_VALUE
    double depart;
    /// @brief position on the lane when starting the stage
    double departPos;
    /// @brief position on the lane when ending the stage
    double arrivalPos;
    /// @brief arbitrary description string
    std::string description;
};



class TraCIReservation {
public:
    TraCIReservation() {}
    TraCIReservation(const std::string& id,
                     const std::vector<std::string>& persons,
                     const std::string& group,
                     const std::string& fromEdge,
                     const std::string& toEdge,
                     double departPos,
                     double arrivalPos,
                     double depart,
                     double reservationTime,
                     int state) :
        id(id), persons(persons), group(group), fromEdge(fromEdge), toEdge(toEdge), departPos(departPos), arrivalPos(arrivalPos),
        depart(depart), reservationTime(reservationTime), state(state) {}
    /// @brief The id of the taxi reservation (usable for traci.vehicle.dispatchTaxi)
    std::string id;
    /// @brief The persons ids that are part of this reservation
    std::vector<std::string> persons;
    /// @brief The group id of this reservation
    std::string group;
    /// @brief The origin edge id
    std::string fromEdge;
    /// @brief The destination edge id
    std::string toEdge;
    /// @brief pickup position on the origin edge
    double departPos;
    /// @brief drop-off position on the destination edge
    double arrivalPos;
    /// @brief pickup-time
    double depart;
    /// @brief time when the reservation was made
    double reservationTime;
    /// @brief the state of this reservation
    int state;
};

struct TraCICollision {
    /// @brief The ids of the participating vehicles and persons
    std::string collider;
    std::string victim;
    std::string colliderType;
    std::string victimType;
    double colliderSpeed;
    double victimSpeed;
    /// @brief The type of collision
    std::string type;
    /// @brief The lane where the collision happended
    std::string lane;
    /// @brief The position of the collision along the lane
    double pos;
};


struct TraCISignalConstraint {
    /// @brief the idea of the rail signal where this constraint is active
    std::string signalId;
    /// @brief the tripId or vehicle id of the train that is constrained
    std::string tripId;
    /// @brief the tripId or vehicle id of the train that must pass first
    std::string foeId;
    /// @brief the tlsID of the rail signla that the foe must pass first
    std::string foeSignal;
    /// @brief the number of trains that must be recorded at the foeSignal
    int limit;
    /// @brief the type of constraint (predecessor:0, insertionPredecessor:1)
    int type;
    /// @brief whether tripId must still wait for foeId to pass foeSignal
    bool mustWait;

    std::string getString() const {
        std::ostringstream os;
        os << "TraCISignalConstraint(signalId=" << signalId << ", tripid=" << tripId << ", foeSignal=" << foeSignal << ", foeId=" << foeId << ")";
        return os.str();
    }
};

}
