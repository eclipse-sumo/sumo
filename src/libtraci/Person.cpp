/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    Person.cpp
/// @author  Leonhard Luecken
/// @date    15.09.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include "Domain.h"
#include <libsumo/Person.h>
#include <libsumo/StorageHelper.h>

namespace libtraci {

typedef Domain<libsumo::CMD_GET_PERSON_VARIABLE, libsumo::CMD_SET_PERSON_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Person::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
Person::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


libsumo::TraCIPosition
Person::getPosition(const std::string& personID, const bool includeZ) {
    return includeZ ? getPosition3D(personID) : Dom::getPos(libsumo::VAR_POSITION, personID);
}


libsumo::TraCIPosition
Person::getPosition3D(const std::string& personID) {
    return Dom::getPos3D(libsumo::VAR_POSITION3D, personID);
}


double
Person::getAngle(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_ANGLE, personID);
}


double
Person::getSlope(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_SLOPE, personID);
}


double
Person::getSpeed(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_SPEED, personID);
}


std::string
Person::getRoadID(const std::string& personID) {
    return Dom::getString(libsumo::VAR_ROAD_ID, personID);
}


std::string
Person::getLaneID(const std::string& personID) {
    return Dom::getString(libsumo::VAR_LANE_ID, personID);
}


double
Person::getLanePosition(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_LANEPOSITION, personID);
}


std::vector<libsumo::TraCIReservation>
Person::getTaxiReservations(int onlyNew) {
    std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
    tcpip::Storage content;
    StoHelp::writeTypedInt(content, onlyNew);
    tcpip::Storage& ret = Dom::get(libsumo::VAR_TAXI_RESERVATIONS, "", &content);
    std::vector<libsumo::TraCIReservation> result;
    int numReservations = ret.readInt();
    while (numReservations-- > 0) {
        libsumo::TraCIReservation r;
        StoHelp::readCompound(ret, 10);
        r.id = StoHelp::readTypedString(ret);
        r.persons = StoHelp::readTypedStringList(ret);
        r.group = StoHelp::readTypedString(ret);
        r.fromEdge = StoHelp::readTypedString(ret);
        r.toEdge = StoHelp::readTypedString(ret);
        r.departPos = StoHelp::readTypedDouble(ret);
        r.arrivalPos = StoHelp::readTypedDouble(ret);
        r.depart = StoHelp::readTypedDouble(ret);
        r.reservationTime = StoHelp::readTypedDouble(ret);
        r.state = StoHelp::readTypedInt(ret);
        result.emplace_back(r);
    }
    return result;
}


std::string
Person::splitTaxiReservation(std::string reservationID, const std::vector<std::string>& personIDs) {
    tcpip::Storage content;
    StoHelp::writeTypedStringList(content, personIDs);
    return Dom::getString(libsumo::SPLIT_TAXI_RESERVATIONS, reservationID, &content);
}


libsumo::TraCIColor
Person::getColor(const std::string& personID) {
    return Dom::getCol(libsumo::VAR_COLOR, personID);
}


std::string
Person::getTypeID(const std::string& personID) {
    return Dom::getString(libsumo::VAR_TYPE, personID);
}


double
Person::getWaitingTime(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_WAITING_TIME, personID);
}


std::string
Person::getNextEdge(const std::string& personID) {
    return Dom::getString(libsumo::VAR_NEXT_EDGE, personID);
}


std::vector<std::string>
Person::getEdges(const std::string& personID, int nextStageIndex) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(nextStageIndex);
    return Dom::getStringVector(libsumo::VAR_EDGES, personID, &content);
}


libsumo::TraCIStage
Person::getStage(const std::string& personID, int nextStageIndex) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(nextStageIndex);
    return Dom::getTraCIStage(libsumo::VAR_STAGE, personID, &content);
}


int
Person::getRemainingStages(const std::string& personID) {
    return Dom::getInt(libsumo::VAR_STAGES_REMAINING, personID);
}


std::string
Person::getVehicle(const std::string& personID) {
    return Dom::getString(libsumo::VAR_VEHICLE, personID);
}


std::string
Person::getEmissionClass(const std::string& personID) {
    return Dom::getString(libsumo::VAR_EMISSIONCLASS, personID);
}


std::string
Person::getShapeClass(const std::string& personID) {
    return Dom::getString(libsumo::VAR_SHAPECLASS, personID);
}


double
Person::getLength(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_LENGTH, personID);
}


double
Person::getSpeedFactor(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_SPEED_FACTOR, personID);
}


double
Person::getAccel(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_ACCEL, personID);
}


double
Person::getDecel(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_DECEL, personID);
}


double Person::getEmergencyDecel(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_EMERGENCY_DECEL, personID);
}


double Person::getApparentDecel(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_APPARENT_DECEL, personID);
}


double Person::getActionStepLength(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_ACTIONSTEPLENGTH, personID);
}


double
Person::getTau(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_TAU, personID);
}


double
Person::getImperfection(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_IMPERFECTION, personID);
}


double
Person::getSpeedDeviation(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_SPEED_DEVIATION, personID);
}


std::string
Person::getVehicleClass(const std::string& personID) {
    return Dom::getString(libsumo::VAR_VEHICLECLASS, personID);
}


double
Person::getMinGap(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_MINGAP, personID);
}


double
Person::getMinGapLat(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_MINGAP_LAT, personID);
}


double
Person::getMaxSpeed(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED, personID);
}


double
Person::getMaxSpeedLat(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED_LAT, personID);
}


std::string
Person::getLateralAlignment(const std::string& personID) {
    return Dom::getString(libsumo::VAR_LATALIGNMENT, personID);
}


double
Person::getWidth(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_WIDTH, personID);
}


double
Person::getHeight(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_HEIGHT, personID);
}


int
Person::getPersonCapacity(const std::string& personID) {
    return Dom::getInt(libsumo::VAR_PERSON_CAPACITY, personID);
}


double
Person::getBoardingDuration(const std::string& personID) {
    return Dom::getDouble(libsumo::VAR_BOARDING_DURATION, personID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(Person, PERSON)


void
Person::setSpeed(const std::string& personID, double speed) {
    Dom::setDouble(libsumo::VAR_SPEED, personID, speed);
}


void
Person::setType(const std::string& personID, const std::string& typeID) {
    Dom::setString(libsumo::VAR_TYPE, personID, typeID);
}


void
Person::add(const std::string& personID, const std::string& edgeID, double pos, double departInSecs, const std::string typeID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(typeID);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(departInSecs);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(pos);
    Dom::set(libsumo::ADD, personID, &content);
}


void
Person::appendStage(const std::string& personID, const libsumo::TraCIStage& stage) {
    tcpip::Storage content;
    libsumo::StorageHelper::writeStage(content, stage);
    Dom::set(libsumo::APPEND_STAGE, personID, &content);
}


void
Person::replaceStage(const std::string& personID, const int stageIndex, const libsumo::TraCIStage& stage) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(stageIndex);
    libsumo::StorageHelper::writeStage(content, stage);
    Dom::set(libsumo::REPLACE_STAGE, personID, &content);
}


void
Person::appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(libsumo::STAGE_DRIVING);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(toEdge);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(lines);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(stopID);
    Dom::set(libsumo::APPEND_STAGE, personID, &content);
}


void
Person::appendWaitingStage(const std::string& personID, double duration, const std::string& description, const std::string& stopID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(libsumo::STAGE_WAITING);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(description);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(stopID);
    Dom::set(libsumo::APPEND_STAGE, personID, &content);
}


void
Person::appendWalkingStage(const std::string& personID, const std::vector<std::string>& edges, double arrivalPos, double duration, double speed, const std::string& stopID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(6);
    content.writeUnsignedByte(libsumo::TYPE_INTEGER);
    content.writeInt(libsumo::STAGE_WALKING);
    content.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
    content.writeStringList(edges);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(arrivalPos);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(duration);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(stopID);
    Dom::set(libsumo::APPEND_STAGE, personID, &content);
}


void
Person::removeStage(const std::string& personID, int nextStageIndex) {
    Dom::setInt(libsumo::REMOVE_STAGE, personID, nextStageIndex);
}


void
Person::rerouteTraveltime(const std::string& personID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(0);
    Dom::set(libsumo::CMD_REROUTE_TRAVELTIME, personID, &content);
}


void
Person::moveTo(const std::string& personID, const std::string& laneID, double pos, double posLat) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(3);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(laneID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(pos);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(posLat);
    Dom::set(libsumo::VAR_MOVE_TO, personID, &content);
}


void
Person::moveToXY(const std::string& personID, const std::string& edgeID, const double x, const double y, double angle, const int keepRoute, double matchThreshold) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(6);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(x);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(y);
    content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(angle);
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeByte(keepRoute);
    StoHelp::writeTypedDouble(content, matchThreshold);
    Dom::set(libsumo::MOVE_TO_XY, personID, &content);
}


/** untested setter functions which alter the person's vtype ***/

void
Person::setLength(const std::string& personID, double length) {
    Dom::setDouble(libsumo::VAR_LENGTH, personID, length);
}


void
Person::setMaxSpeed(const std::string& personID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED, personID, speed);
}


void
Person::setVehicleClass(const std::string& personID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_VEHICLECLASS, personID, clazz);
}


void
Person::setShapeClass(const std::string& personID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_SHAPECLASS, personID, clazz);
}


void
Person::setEmissionClass(const std::string& personID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_EMISSIONCLASS, personID, clazz);
}


void
Person::setWidth(const std::string& personID, double width) {
    Dom::setDouble(libsumo::VAR_WIDTH, personID, width);
}


void
Person::setHeight(const std::string& personID, double height) {
    Dom::setDouble(libsumo::VAR_HEIGHT, personID, height);
}


void
Person::setMinGap(const std::string& personID, double minGap) {
    Dom::setDouble(libsumo::VAR_MINGAP, personID, minGap);
}


void
Person::setAccel(const std::string& personID, double accel) {
    Dom::setDouble(libsumo::VAR_ACCEL, personID, accel);
}


void
Person::setDecel(const std::string& personID, double decel) {
    Dom::setDouble(libsumo::VAR_DECEL, personID, decel);
}


void
Person::setEmergencyDecel(const std::string& personID, double decel) {
    Dom::setDouble(libsumo::VAR_EMERGENCY_DECEL, personID, decel);
}


void
Person::setApparentDecel(const std::string& personID, double decel) {
    Dom::setDouble(libsumo::VAR_APPARENT_DECEL, personID, decel);
}


void
Person::setImperfection(const std::string& personID, double imperfection) {
    Dom::setDouble(libsumo::VAR_IMPERFECTION, personID, imperfection);
}


void
Person::setTau(const std::string& personID, double tau) {
    Dom::setDouble(libsumo::VAR_TAU, personID, tau);
}


void
Person::setMinGapLat(const std::string& personID, double minGapLat) {
    Dom::setDouble(libsumo::VAR_MINGAP_LAT, personID, minGapLat);
}


void
Person::setMaxSpeedLat(const std::string& personID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED_LAT, personID, speed);
}


void
Person::setLateralAlignment(const std::string& personID, const std::string& latAlignment) {
    Dom::setString(libsumo::VAR_LATALIGNMENT, personID, latAlignment);
}


void
Person::setSpeedFactor(const std::string& personID, double factor) {
    Dom::setDouble(libsumo::VAR_SPEED_FACTOR, personID, factor);
}


void
Person::setActionStepLength(const std::string& personID, double actionStepLength, bool resetActionOffset) {
    if (!resetActionOffset) {
        actionStepLength *= -1;
    }
    Dom::setDouble(libsumo::VAR_ACTIONSTEPLENGTH, personID, actionStepLength);
}

void
Person::remove(const std::string& personID, char reason) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_BYTE);
    content.writeUnsignedByte(reason);
    Dom::set(libsumo::REMOVE, personID, &content);
}


void
Person::setColor(const std::string& personID, const libsumo::TraCIColor& color) {
    Dom::setCol(libsumo::VAR_COLOR, personID, color);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Person, PERSON)


}


/****************************************************************************/
