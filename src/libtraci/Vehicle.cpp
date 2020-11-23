/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
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
/// @file    Vehicle.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>
#include <sstream>

#define LIBTRACI 1
#include <libsumo/Vehicle.h>
#include "Connection.h"
#include "Domain.h"

// TODO remove the following line once the implementation is mature
#ifdef _MSC_VER
#pragma warning(disable: 4100)
#else
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace libtraci {

typedef Domain<libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::CMD_SET_VEHICLE_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Vehicle::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
Vehicle::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Vehicle, VEHICLE)
LIBTRACI_PARAMETER_IMPLEMENTATION(Vehicle, VEHICLE)

double
Vehicle::getSpeed(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_SPEED, vehicleID);
}

double
Vehicle::getLateralSpeed(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_SPEED_LAT, vehicleID);
}

double
Vehicle::getAcceleration(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ACCELERATION, vehicleID);
}


double
Vehicle::getSpeedWithoutTraCI(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_SPEED_WITHOUT_TRACI, vehicleID);
}


libsumo::TraCIPosition
Vehicle::getPosition(const std::string& vehicleID, const bool includeZ) {
    return includeZ ? getPosition3D(vehicleID) : Dom::getPos(libsumo::VAR_POSITION, vehicleID);
}


libsumo::TraCIPosition
Vehicle::getPosition3D(const std::string& vehicleID) {
    return Dom::getPos3D(libsumo::VAR_POSITION3D, vehicleID);
}


double
Vehicle::getAngle(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ANGLE, vehicleID);
}


double
Vehicle::getSlope(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_SLOPE, vehicleID);
}


std::string
Vehicle::getRoadID(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_ROAD_ID, vehicleID);
}


std::string
Vehicle::getLaneID(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_LANE_ID, vehicleID);
}


int
Vehicle::getLaneIndex(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_LANE_INDEX, vehicleID);
}


std::string
Vehicle::getTypeID(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_TYPE, vehicleID);
}


std::string
Vehicle::getRouteID(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_ROUTE_ID, vehicleID);
}


int
Vehicle::getRouteIndex(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_ROUTE_INDEX, vehicleID);
}


libsumo::TraCIColor
Vehicle::getColor(const std::string& vehicleID) {
    return Dom::getCol(libsumo::VAR_COLOR, vehicleID);
}

double
Vehicle::getLanePosition(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_LANEPOSITION, vehicleID);
}

double
Vehicle::getLateralLanePosition(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_LANEPOSITION_LAT, vehicleID);
}

double
Vehicle::getCO2Emission(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_CO2EMISSION, vehicleID);
}

double
Vehicle::getCOEmission(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_COEMISSION, vehicleID);
}

double
Vehicle::getHCEmission(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_HCEMISSION, vehicleID);
}

double
Vehicle::getPMxEmission(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_PMXEMISSION, vehicleID);
}

double
Vehicle::getNOxEmission(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_NOXEMISSION, vehicleID);
}

double
Vehicle::getFuelConsumption(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_FUELCONSUMPTION, vehicleID);
}

double
Vehicle::getNoiseEmission(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_NOISEEMISSION, vehicleID);
}

double
Vehicle::getElectricityConsumption(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ELECTRICITYCONSUMPTION, vehicleID);
}

int
Vehicle::getPersonNumber(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_PERSON_NUMBER, vehicleID);
}

int
Vehicle::getPersonCapacity(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_PERSON_CAPACITY, vehicleID);
}

std::vector<std::string>
Vehicle::getPersonIDList(const std::string& vehicleID) {
    return Dom::getStringVector(libsumo::LAST_STEP_PERSON_ID_LIST, vehicleID);
}

std::pair<std::string, double>
Vehicle::getLeader(const std::string& vehicleID, double dist) {
    tcpip::Storage content;
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(dist);
    tcpip::Storage& ret = Connection::getActive().doCommand(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_LEADER, vehicleID, &content);
    if (Connection::getActive().processGet(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::TYPE_COMPOUND)) {
        ret.readInt(); // components
        ret.readUnsignedByte();
        const std::string leaderID = ret.readString();
        ret.readUnsignedByte();
        const double gap = ret.readDouble();
        return std::make_pair(leaderID, gap);
    }
    return std::make_pair("", libsumo::INVALID_DOUBLE_VALUE);
}


std::pair<std::string, double>
Vehicle::getFollower(const std::string& vehicleID, double dist) {
    tcpip::Storage content;
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(dist);
    tcpip::Storage& ret = Connection::getActive().doCommand(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_FOLLOWER, vehicleID, &content);
    if (Connection::getActive().processGet(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::TYPE_COMPOUND)) {
        ret.readInt(); // components
        ret.readUnsignedByte();
        const std::string leaderID = ret.readString();
        ret.readUnsignedByte();
        const double gap = ret.readDouble();
        return std::make_pair(leaderID, gap);
    }
    return std::make_pair("", libsumo::INVALID_DOUBLE_VALUE);
}


double
Vehicle::getWaitingTime(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_WAITING_TIME, vehicleID);
}


double
Vehicle::getAccumulatedWaitingTime(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ACCUMULATED_WAITING_TIME, vehicleID);
}


double
Vehicle::getAdaptedTraveltime(const std::string& vehicleID, double time, const std::string& edgeID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(time);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    return Dom::getDouble(libsumo::VAR_EDGE_TRAVELTIME, vehicleID, &content);
}


double
Vehicle::getEffort(const std::string& vehicleID, double time, const std::string& edgeID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(time);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    return Dom::getDouble(libsumo::VAR_EDGE_EFFORT, edgeID, &content);
}


bool
Vehicle::isRouteValid(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_ROUTE_VALID, vehicleID) != 0;
}


std::vector<std::string>
Vehicle::getRoute(const std::string& vehicleID) {
    return Dom::getStringVector(libsumo::VAR_ROUTE, vehicleID);
}


int
Vehicle::getSignals(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_SIGNALS, vehicleID) != 0;
}


std::vector<libsumo::TraCIBestLanesData>
Vehicle::getBestLanes(const std::string& vehicleID) {
    std::vector<libsumo::TraCIBestLanesData> result;
    tcpip::Storage& ret = Connection::getActive().doCommand(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_BEST_LANES, vehicleID);
    if (Connection::getActive().processGet(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::TYPE_COMPOUND)) {
        ret.readInt();
        ret.readUnsignedByte();

        const int n = ret.readInt(); // number of following edge information
        for (int i = 0; i < n; ++i) {
            libsumo::TraCIBestLanesData info;
            ret.readUnsignedByte();
            info.laneID = ret.readString();

            ret.readUnsignedByte();
            info.length = ret.readDouble();

            ret.readUnsignedByte();
            info.occupation = ret.readDouble();

            ret.readUnsignedByte();
            info.bestLaneOffset = ret.readByte();

            ret.readUnsignedByte();
            info.allowsContinuation = (ret.readUnsignedByte() == 1);

            ret.readUnsignedByte();
            const int m = ret.readInt();
            for (int i = 0; i < m; ++i) {
                info.continuationLanes.push_back(ret.readString());
            }
            result.push_back(info);
        }
    }
    return result;
}


std::vector<libsumo::TraCINextTLSData>
Vehicle::getNextTLS(const std::string& vehicleID) {
    std::vector<libsumo::TraCINextTLSData> result;
    tcpip::Storage& ret = Connection::getActive().doCommand(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_NEXT_TLS, vehicleID);
    if (Connection::getActive().processGet(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::TYPE_COMPOUND)) {
        ret.readInt(); // components
        // number of items
        ret.readUnsignedByte();
        const int n = ret.readInt();
        for (int i = 0; i < n; ++i) {
            libsumo::TraCINextTLSData d;
            ret.readUnsignedByte();
            d.id = ret.readString();

            ret.readUnsignedByte();
            d.tlIndex = ret.readInt();

            ret.readUnsignedByte();
            d.dist = ret.readDouble();

            ret.readUnsignedByte();
            d.state = (char)ret.readByte();

            result.push_back(d);
        }
    }
    return result;
}

std::vector<libsumo::TraCINextStopData>
Vehicle::getNextStops(const std::string& vehicleID) {
    return getStops(vehicleID, 0);
}

std::vector<libsumo::TraCINextStopData>
Vehicle::getStops(const std::string& vehicleID, int limit) {
    std::vector<libsumo::TraCINextStopData> result;
    tcpip::Storage& ret = Connection::getActive().doCommand(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::VAR_NEXT_STOPS2, vehicleID);
    if (Connection::getActive().processGet(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::TYPE_COMPOUND)) {
        ret.readInt(); // components
        // number of items
        ret.readUnsignedByte();
        const int n = ret.readInt();
        for (int i = 0; i < n; ++i) {
            libsumo::TraCINextStopData s;
            ret.readUnsignedByte();
            s.lane = ret.readString();

            ret.readUnsignedByte();
            s.endPos = ret.readDouble();

            ret.readUnsignedByte();
            s.stoppingPlaceID = ret.readString();

            ret.readUnsignedByte();
            s.stopFlags = ret.readInt();

            ret.readUnsignedByte();
            s.duration = ret.readDouble();

            ret.readUnsignedByte();
            s.until = ret.readDouble();

            ret.readUnsignedByte();
            s.startPos = ret.readDouble();

            ret.readUnsignedByte();
            s.intendedArrival = ret.readDouble();

            ret.readUnsignedByte();
            s.arrival = ret.readDouble();

            ret.readUnsignedByte();
            s.depart = ret.readDouble();

            ret.readUnsignedByte();
            s.split = ret.readString();

            ret.readUnsignedByte();
            s.join = ret.readString();

            ret.readUnsignedByte();
            s.actType = ret.readString();

            ret.readUnsignedByte();
            s.tripId = ret.readString();

            ret.readUnsignedByte();
            s.line = ret.readString();

            ret.readUnsignedByte();
            s.speed = ret.readDouble();

            result.push_back(s);
        }
    }
    return result;
}


int
Vehicle::getStopState(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_STOPSTATE, vehicleID);
}


double
Vehicle::getDistance(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_DISTANCE, vehicleID);
}


double
Vehicle::getDrivingDistance(const std::string& vehicleID, const std::string& edgeID, double position, int /* laneIndex */) {
    /// XXX
    return libsumo::INVALID_DOUBLE_VALUE;
}


double
Vehicle::getDrivingDistance2D(const std::string& vehicleID, double x, double y) {
    /// XXX
    return libsumo::INVALID_DOUBLE_VALUE;
}


double
Vehicle::getAllowedSpeed(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ALLOWED_SPEED, vehicleID);
}


double
Vehicle::getSpeedFactor(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_SPEED_FACTOR, vehicleID);
}


int
Vehicle::getSpeedMode(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_SPEEDSETMODE, vehicleID);
}


int
Vehicle::getLaneChangeMode(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_LANECHANGE_MODE, vehicleID);
}


int
Vehicle::getRoutingMode(const std::string& vehicleID) {
    return Dom::getInt(libsumo::VAR_ROUTING_MODE, vehicleID);
}


std::string
Vehicle::getLine(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_LINE, vehicleID);
}



std::vector<std::string>
Vehicle::getVia(const std::string& vehicleID) {
    return Dom::getStringVector(libsumo::VAR_VIA, vehicleID);
}


std::pair<int, int>
Vehicle::getLaneChangeState(const std::string& vehicleID, int direction) {
    // XXX
    //tcpip::Storage content;
    //content.writeByte(libsumo::TYPE_INTEGER);
    //content.writeInt(direction);
    //Connection::getActive().doCommand(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::CMD_CHANGELANE, vehicleID, &content);
    //if (Connection::getActive().processGet(libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::TYPE_COMPOUND)) {
    //    ret.readInt(); // components
    //    ret.readUnsignedByte();
    //    const int stateWithoutTraCI = ret.readInt();
    //    ret.readUnsignedByte();
    //    const int state = ret.readInt();
    //    return std::make_pair(stateWithoutTraCI, state);
    //}
    return std::make_pair(libsumo::INVALID_INT_VALUE, libsumo::INVALID_INT_VALUE);
}


std::vector<std::pair<std::string, double> >
Vehicle::getNeighbors(const std::string& vehicleID, const int mode) {
    std::vector<std::pair<std::string, double> > neighs;
    // XXX
    return neighs;
}


double
Vehicle::getFollowSpeed(const std::string& vehicleID, double speed, double gap, double leaderSpeed, double leaderMaxDecel, const std::string& leaderID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(5);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(gap);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(leaderSpeed);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(leaderMaxDecel);
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(leaderID);
    return Dom::getDouble(libsumo::VAR_FOLLOW_SPEED, vehicleID, &content);
}


double
Vehicle::getSecureGap(const std::string& vehicleID, double speed, double leaderSpeed, double leaderMaxDecel, const std::string& leaderID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(4);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(leaderSpeed);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(leaderMaxDecel);
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(leaderID);
    return Dom::getDouble(libsumo::VAR_SECURE_GAP, vehicleID, &content);
}


double
Vehicle::getStopSpeed(const std::string& vehicleID, const double speed, double gap) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(gap);
    return Dom::getDouble(libsumo::VAR_STOP_SPEED, vehicleID, &content);
}

double
Vehicle::getStopDelay(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_STOP_DELAY, vehicleID);
}

double
Vehicle::getStopArrivalDelay(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_STOP_ARRIVALDELAY, vehicleID);
}

std::vector<std::string>
Vehicle::getTaxiFleet(int taxiState) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(1);
    content.writeByte(libsumo::TYPE_INTEGER);
    content.writeInt(taxiState);
    return Dom::getStringVector(libsumo::VAR_TAXI_FLEET, "", &content);
}

std::string
Vehicle::getEmissionClass(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_EMISSIONCLASS, vehicleID);
}

std::string
Vehicle::getShapeClass(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_SHAPECLASS, vehicleID);
}


double
Vehicle::getLength(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_LENGTH, vehicleID);
}


double
Vehicle::getAccel(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ACCEL, vehicleID);
}


double
Vehicle::getDecel(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_DECEL, vehicleID);
}


double Vehicle::getEmergencyDecel(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_EMERGENCY_DECEL, vehicleID);
}


double Vehicle::getApparentDecel(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_APPARENT_DECEL, vehicleID);
}


double Vehicle::getActionStepLength(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_ACTIONSTEPLENGTH, vehicleID);
}


double Vehicle::getLastActionTime(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_LASTACTIONTIME, vehicleID);
}


double
Vehicle::getTau(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_TAU, vehicleID);
}


double
Vehicle::getImperfection(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_IMPERFECTION, vehicleID);
}


double
Vehicle::getSpeedDeviation(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_SPEED_DEVIATION, vehicleID);
}


std::string
Vehicle::getVehicleClass(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_VEHICLECLASS, vehicleID);
}


double
Vehicle::getMinGap(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_MINGAP, vehicleID);
}


double
Vehicle::getMinGapLat(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_MINGAP_LAT, vehicleID);
}


double
Vehicle::getMaxSpeed(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED, vehicleID);
}


double
Vehicle::getMaxSpeedLat(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED_LAT, vehicleID);
}


std::string
Vehicle::getLateralAlignment(const std::string& vehicleID) {
    return Dom::getString(libsumo::VAR_LATALIGNMENT, vehicleID);
}


double
Vehicle::getWidth(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_WIDTH, vehicleID);
}


double
Vehicle::getHeight(const std::string& vehicleID) {
    return Dom::getDouble(libsumo::VAR_HEIGHT, vehicleID);
}


void
Vehicle::setStop(const std::string& vehicleID,
                 const std::string& edgeID,
                 double pos,
                 int laneIndex,
                 double duration,
                 int flags,
                 double startPos,
                 double until) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(7);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(pos);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeByte(laneIndex);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeByte(flags);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(startPos);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(until);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_STOP, vehicleID, &content);
}


void
Vehicle::replaceStop(const std::string& vehicleID,
                     int nextStopIndex,
                     const std::string& edgeID,
                     double pos,
                     int laneIndex,
                     double duration,
                     int flags,
                     double startPos,
                     double until) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(8);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(pos);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeByte(laneIndex);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeByte(flags);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(startPos);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(until);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeDouble(nextStopIndex);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_REPLACE_STOP, vehicleID, &content);
}


void
Vehicle::rerouteParkingArea(const std::string& vehicleID, const std::string& parkingAreaID) {
    Dom::setString(libsumo::CMD_REROUTE_TO_PARKING, vehicleID, parkingAreaID);
}

void
Vehicle::resume(const std::string& vehicleID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(0);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_RESUME, vehicleID, &content);
}


void
Vehicle::changeTarget(const std::string& vehicleID, const std::string& edgeID) {
    Dom::setString(libsumo::CMD_CHANGETARGET, vehicleID, edgeID);
}


void
Vehicle::changeLane(const std::string& vehicleID, int laneIndex, double duration) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeInt(laneIndex);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_CHANGELANE, vehicleID, &content);
}

void
Vehicle::changeLaneRelative(const std::string& vehicleID, int indexOffset, double duration) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(3);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeInt(indexOffset);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeDouble(1);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_CHANGELANE, vehicleID, &content);
}


void
Vehicle::changeSublane(const std::string& vehicleID, double latDist) {
    Dom::setDouble(libsumo::CMD_CHANGESUBLANE, vehicleID, latDist);
}


void
Vehicle::add(const std::string& vehicleID,
             const std::string& routeID,
             const std::string& typeID,
             const std::string& depart,
             const std::string& departLane,
             const std::string& departPos,
             const std::string& departSpeed,
             const std::string& arrivalLane,
             const std::string& arrivalPos,
             const std::string& arrivalSpeed,
             const std::string& fromTaz,
             const std::string& toTaz,
             const std::string& line,
             int personCapacity,
             int personNumber) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(14);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(routeID);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(typeID);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(depart);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(departLane);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(departPos);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(departSpeed);

    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(arrivalLane);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(arrivalPos);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(arrivalSpeed);

    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(fromTaz);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(toTaz);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(line);

    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(personCapacity);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(personNumber);

    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::ADD_FULL, vehicleID, &content);
}


void
Vehicle::moveToXY(const std::string& vehicleID, const std::string& edgeID, const int laneIndex,
                  const double x, const double y, double angle, const int keepRoute) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(6);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(laneIndex);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(x);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(y);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(angle);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeByte(keepRoute);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::MOVE_TO_XY, vehicleID, &content);
}

void
Vehicle::slowDown(const std::string& vehicleID, double speed, double duration) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_SLOWDOWN, vehicleID, &content);
}

void
Vehicle::openGap(const std::string& vehicleID, double newTimeHeadway, double newSpaceHeadway, double duration, double changeRate, double maxDecel, const std::string& referenceVehID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(6);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(newTimeHeadway);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(newSpaceHeadway);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(changeRate);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(maxDecel);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(referenceVehID);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_OPENGAP, vehicleID, &content);
}

void
Vehicle::deactivateGapControl(const std::string& vehicleID) {
    openGap(vehicleID, -1, -1, -1, -1, -1, "");
}

void
Vehicle::requestToC(const std::string& vehID, double leadTime) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss << std::setprecision(2);
    oss << leadTime;
    setParameter(vehID, "device.toc.requestToC", oss.str());
}

void
Vehicle::setSpeed(const std::string& vehicleID, double speed) {
    Dom::setDouble(libsumo::VAR_SPEED, vehicleID, speed);
}

void
Vehicle::setPreviousSpeed(const std::string& vehicleID, double prevspeed) {
    Dom::setDouble(libsumo::VAR_PREV_SPEED, vehicleID, prevspeed);
}

void
Vehicle::setSpeedMode(const std::string& vehicleID, int speedMode) {
    Dom::setInt(libsumo::VAR_SPEEDSETMODE, vehicleID, speedMode);
}

void
Vehicle::setLaneChangeMode(const std::string& vehicleID, int laneChangeMode) {
    Dom::setInt(libsumo::VAR_LANECHANGE_MODE, vehicleID, laneChangeMode);
}

void
Vehicle::setRoutingMode(const std::string& vehicleID, int routingMode) {
    Dom::setInt(libsumo::VAR_ROUTING_MODE, vehicleID, routingMode);
}

void
Vehicle::setType(const std::string& vehicleID, const std::string& typeID) {
    Dom::setString(libsumo::VAR_TYPE, vehicleID, typeID);
}

void
Vehicle::setRouteID(const std::string& vehicleID, const std::string& routeID) {
    Dom::setString(libsumo::VAR_ROUTE_ID, vehicleID, routeID);
}

void
Vehicle::setRoute(const std::string& vehicleID, const std::string& edgeID) {
    setRoute(vehicleID, std::vector<std::string>({edgeID}));
}

void
Vehicle::setRoute(const std::string& vehicleID, const std::vector<std::string>& edgeIDs) {
    Dom::setStringVector(libsumo::VAR_ROUTE, vehicleID, edgeIDs);
}

void
Vehicle::updateBestLanes(const std::string& vehicleID) {
    // XXX
}


void
Vehicle::setAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID,
                              double time, double begSeconds, double endSeconds) {
    tcpip::Storage content;
    if (time == libsumo::INVALID_DOUBLE_VALUE) {
        // reset
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(1);
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(edgeID);
    } else if (begSeconds == libsumo::INVALID_DOUBLE_VALUE) {
        // set value for the whole simulation
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(2);
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(edgeID);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(time);
    } else {
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(4);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(begSeconds);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(endSeconds);
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(edgeID);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(time);
    }
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_EDGE_TRAVELTIME, vehicleID, &content);
}


void
Vehicle::setEffort(const std::string& vehicleID, const std::string& edgeID,
                   double effort, double begSeconds, double endSeconds) {
    tcpip::Storage content;
    if (effort == libsumo::INVALID_DOUBLE_VALUE) {
        // reset
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(1);
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(edgeID);
    } else if (begSeconds == libsumo::INVALID_DOUBLE_VALUE) {
        // set value for the whole simulation
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(2);
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(edgeID);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(effort);
    } else {
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(4);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(begSeconds);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(endSeconds);
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(edgeID);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(effort);
    }
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_EDGE_EFFORT, vehicleID, &content);
}


void
Vehicle::rerouteTraveltime(const std::string& vehicleID, const bool currentTravelTimes) {
    // UNUSED_PARAMETER(currentTravelTimes); // !!! see #5943
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(0);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_REROUTE_TRAVELTIME, vehicleID, &content);
}


void
Vehicle::rerouteEffort(const std::string& vehicleID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(0);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_REROUTE_EFFORT, vehicleID, &content);
}


void
Vehicle::setSignals(const std::string& vehicleID, int signals) {
    Dom::setInt(libsumo::VAR_SIGNALS, vehicleID, signals);
}


void
Vehicle::moveTo(const std::string& vehicleID, const std::string& laneID, double position, int reason) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(3);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(laneID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(position);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(reason);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_MOVE_TO, vehicleID, &content);
}


void
Vehicle::setActionStepLength(const std::string& vehicleID, double actionStepLength, bool resetActionOffset) {
    //if (actionStepLength < 0) {
    //    raise TraCIException("Invalid value for actionStepLength. Given value must be non-negative.")
    //{
    // Use negative value to indicate resetActionOffset == False
    if (!resetActionOffset) {
        actionStepLength *= -1;
    }
    Dom::setDouble(libsumo::VAR_ACTIONSTEPLENGTH, vehicleID, actionStepLength);
}


void
Vehicle::remove(const std::string& vehicleID, char reason) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeUnsignedByte(reason);
    Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::REMOVE, vehicleID, &content);
}


void
Vehicle::setColor(const std::string& vehicleID, const libsumo::TraCIColor& color) {
    Dom::setCol(libsumo::VAR_COLOR, vehicleID, color);
}


void
Vehicle::setSpeedFactor(const std::string& vehicleID, double factor) {
    Dom::setDouble(libsumo::VAR_SPEED_FACTOR, vehicleID, factor);
}


void
Vehicle::setLine(const std::string& vehicleID, const std::string& line) {
    Dom::setString(libsumo::VAR_LINE, vehicleID, line);
}


void
Vehicle::setVia(const std::string& vehicleID, const std::vector<std::string>& via) {
    Dom::setStringVector(libsumo::VAR_VIA, vehicleID, via);
}


void
Vehicle::setLength(const std::string& vehicleID, double length) {
    Dom::setDouble(libsumo::VAR_LENGTH, vehicleID, length);
}


void
Vehicle::setMaxSpeed(const std::string& vehicleID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED, vehicleID, speed);
}


void
Vehicle::setVehicleClass(const std::string& vehicleID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_VEHICLECLASS, vehicleID, clazz);
}


void
Vehicle::setShapeClass(const std::string& vehicleID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_SHAPECLASS, vehicleID, clazz);
}


void
Vehicle::setEmissionClass(const std::string& vehicleID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_EMISSIONCLASS, vehicleID, clazz);
}


void
Vehicle::setWidth(const std::string& vehicleID, double width) {
    Dom::setDouble(libsumo::VAR_WIDTH, vehicleID, width);
}


void
Vehicle::setHeight(const std::string& vehicleID, double height) {
    Dom::setDouble(libsumo::VAR_HEIGHT, vehicleID, height);
}


void
Vehicle::setMinGap(const std::string& vehicleID, double minGap) {
    Dom::setDouble(libsumo::VAR_MINGAP, vehicleID, minGap);
}


void
Vehicle::setAccel(const std::string& vehicleID, double accel) {
    Dom::setDouble(libsumo::VAR_ACCEL, vehicleID, accel);
}


void
Vehicle::setDecel(const std::string& vehicleID, double decel) {
    Dom::setDouble(libsumo::VAR_DECEL, vehicleID, decel);
}


void
Vehicle::setEmergencyDecel(const std::string& vehicleID, double decel) {
    Dom::setDouble(libsumo::VAR_EMERGENCY_DECEL, vehicleID, decel);
}


void
Vehicle::setApparentDecel(const std::string& vehicleID, double decel) {
    Dom::setDouble(libsumo::VAR_APPARENT_DECEL, vehicleID, decel);
}


void
Vehicle::setImperfection(const std::string& vehicleID, double imperfection) {
    Dom::setDouble(libsumo::VAR_IMPERFECTION, vehicleID, imperfection);
}


void
Vehicle::setTau(const std::string& vehicleID, double tau) {
    Dom::setDouble(libsumo::VAR_TAU, vehicleID, tau);
}


void
Vehicle::setMinGapLat(const std::string& vehicleID, double minGapLat) {
    Dom::setDouble(libsumo::VAR_MINGAP_LAT, vehicleID, minGapLat);
}


void
Vehicle::setMaxSpeedLat(const std::string& vehicleID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED_LAT, vehicleID, speed);
}


void
Vehicle::setLateralAlignment(const std::string& vehicleID, const std::string& latAlignment) {
    Dom::setString(libsumo::VAR_LATALIGNMENT, vehicleID, latAlignment);
}


void
Vehicle::highlight(const std::string& vehicleID, const libsumo::TraCIColor& col, double size, const int alphaMax, const double duration, const int type) {
    tcpip::Storage content;
    if (alphaMax > 0) {
        //self._setCmd(tc.VAR_HIGHLIGHT, vehID, "tcdBdB", 5, color, size, alphaMax, duration, type)
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(5);
        content.writeUnsignedByte(libsumo::TYPE_COLOR);
        content.writeUnsignedByte(col.r);
        content.writeUnsignedByte(col.g);
        content.writeUnsignedByte(col.b);
        content.writeUnsignedByte(col.a);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(size);
        content.writeUnsignedByte(libsumo::TYPE_BYTE);
        content.writeByte(alphaMax);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(size);
        content.writeUnsignedByte(libsumo::TYPE_BYTE);
        content.writeByte(type);
        Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_HIGHLIGHT, vehicleID, &content);
    } else {
        //self._setCmd(tc.VAR_HIGHLIGHT, vehID, "tcd", 2, color, size)
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(2);
        content.writeUnsignedByte(libsumo::TYPE_COLOR);
        content.writeUnsignedByte(col.r);
        content.writeUnsignedByte(col.g);
        content.writeUnsignedByte(col.b);
        content.writeUnsignedByte(col.a);
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(size);
        Connection::getActive().doCommand(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_HIGHLIGHT, vehicleID, &content);
    }
}

void
Vehicle::dispatchTaxi(const std::string& vehicleID,  const std::vector<std::string>& reservations) {
    Dom::setStringVector(libsumo::CMD_TAXI_DISPATCH, vehicleID, reservations);
}


void
Vehicle::subscribeLeader(const std::string& vehicleID, double dist, double beginTime, double endTime) {
}


void
Vehicle::addSubscriptionFilterLanes(const std::vector<int>& lanes, bool noOpposite, double downstreamDist, double upstreamDist) {
}


void
Vehicle::addSubscriptionFilterNoOpposite() {
}


void
Vehicle::addSubscriptionFilterDownstreamDistance(double dist) {
}


void
Vehicle::addSubscriptionFilterUpstreamDistance(double dist) {
}


void
Vehicle::addSubscriptionFilterCFManeuver(double downstreamDist, double upstreamDist) {
}


void
Vehicle::addSubscriptionFilterLCManeuver(int direction, bool noOpposite, double downstreamDist, double upstreamDist) {
}


void
Vehicle::addSubscriptionFilterLeadFollow(const std::vector<int>& lanes) {
}


void
Vehicle::addSubscriptionFilterTurn(double downstreamDist, double upstreamDist) {
}


void
Vehicle::addSubscriptionFilterVClass(const std::vector<std::string>& vClasses) {
}


void
Vehicle::addSubscriptionFilterVType(const std::vector<std::string>& vTypes) {
}


void
Vehicle::addSubscriptionFilterFieldOfVision(double openingAngle) {
}


void
Vehicle::addSubscriptionFilterLateralDistance(double lateralDist, double downstreamDist, double upstreamDist) {
}


}


/****************************************************************************/
