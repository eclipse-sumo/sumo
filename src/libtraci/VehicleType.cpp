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
/// @file    VehicleType.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>
#include <sstream>

#define LIBTRACI 1
#include <libsumo/VehicleType.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_VEHICLETYPE_VARIABLE, libsumo::CMD_SET_VEHICLETYPE_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
VehicleType::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
VehicleType::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(VehicleType, VEHICLETYPE)
LIBTRACI_PARAMETER_IMPLEMENTATION(VehicleType, VEHICLETYPE)


void
VehicleType::copy(const std::string& origTypeID, const std::string& newTypeID)  {
    Dom::setString(libsumo::COPY, origTypeID, newTypeID);
}

double
VehicleType::getSpeedFactor(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_SPEED_FACTOR, typeID);
}

double
VehicleType::getSpeedDeviation(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_SPEED_DEVIATION, typeID);
}


std::string
VehicleType::getEmissionClass(const std::string& typeID) {
    return Dom::getString(libsumo::VAR_EMISSIONCLASS, typeID);
}

std::string
VehicleType::getShapeClass(const std::string& typeID) {
    return Dom::getString(libsumo::VAR_SHAPECLASS, typeID);
}


double
VehicleType::getLength(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_LENGTH, typeID);
}


double
VehicleType::getAccel(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_ACCEL, typeID);
}


double
VehicleType::getDecel(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_DECEL, typeID);
}


double VehicleType::getEmergencyDecel(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_EMERGENCY_DECEL, typeID);
}


double VehicleType::getApparentDecel(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_APPARENT_DECEL, typeID);
}


double VehicleType::getActionStepLength(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_ACTIONSTEPLENGTH, typeID);
}


double
VehicleType::getTau(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_TAU, typeID);
}


double
VehicleType::getImperfection(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_IMPERFECTION, typeID);
}


std::string
VehicleType::getVehicleClass(const std::string& typeID) {
    return Dom::getString(libsumo::VAR_VEHICLECLASS, typeID);
}


double
VehicleType::getMinGap(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_MINGAP, typeID);
}


double
VehicleType::getMinGapLat(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_MINGAP_LAT, typeID);
}


double
VehicleType::getMaxSpeed(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED, typeID);
}


double
VehicleType::getMaxSpeedLat(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED_LAT, typeID);
}


std::string
VehicleType::getLateralAlignment(const std::string& typeID) {
    return Dom::getString(libsumo::VAR_LATALIGNMENT, typeID);
}


double
VehicleType::getWidth(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_WIDTH, typeID);
}


double
VehicleType::getHeight(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_HEIGHT, typeID);
}


libsumo::TraCIColor
VehicleType::getColor(const std::string& typeID) {
    return Dom::getCol(libsumo::VAR_COLOR, typeID);
}


int
VehicleType::getPersonCapacity(const std::string& typeID) {
    return Dom::getInt(libsumo::VAR_PERSON_CAPACITY, typeID);
}


double
VehicleType::getScale(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_SCALE, typeID);
}


double
VehicleType::getBoardingDuration(const std::string& typeID) {
    return Dom::getDouble(libsumo::VAR_BOARDING_DURATION, typeID);
}


void
VehicleType::setActionStepLength(const std::string& typeID, double actionStepLength, bool resetActionOffset) {
    //if (actionStepLength < 0) {
    //    raise TraCIException("Invalid value for actionStepLength. Given value must be non-negative.")
    //{
    // Use negative value to indicate resetActionOffset == False
    if (!resetActionOffset) {
        actionStepLength *= -1;
    }
    Dom::setDouble(libsumo::VAR_ACTIONSTEPLENGTH, typeID, actionStepLength);
}


void
VehicleType::setColor(const std::string& typeID, const libsumo::TraCIColor& col) {
    Dom::setCol(libsumo::VAR_COLOR, typeID, col);
}


void
VehicleType::setSpeedFactor(const std::string& typeID, double factor) {
    Dom::setDouble(libsumo::VAR_SPEED_FACTOR, typeID, factor);
}


void
VehicleType::setSpeedDeviation(const std::string& typeID, double deviation) {
    Dom::setDouble(libsumo::VAR_SPEED_DEVIATION, typeID, deviation);
}


void
VehicleType::setLength(const std::string& typeID, double length) {
    Dom::setDouble(libsumo::VAR_LENGTH, typeID, length);
}


void
VehicleType::setMaxSpeed(const std::string& typeID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED, typeID, speed);
}


void
VehicleType::setVehicleClass(const std::string& typeID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_VEHICLECLASS, typeID, clazz);
}


void
VehicleType::setShapeClass(const std::string& typeID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_SHAPECLASS, typeID, clazz);
}


void
VehicleType::setEmissionClass(const std::string& typeID, const std::string& clazz) {
    Dom::setString(libsumo::VAR_EMISSIONCLASS, typeID, clazz);
}


void
VehicleType::setWidth(const std::string& typeID, double width) {
    Dom::setDouble(libsumo::VAR_WIDTH, typeID, width);
}


void
VehicleType::setHeight(const std::string& typeID, double height) {
    Dom::setDouble(libsumo::VAR_HEIGHT, typeID, height);
}


void
VehicleType::setMinGap(const std::string& typeID, double minGap) {
    Dom::setDouble(libsumo::VAR_MINGAP, typeID, minGap);
}


void
VehicleType::setAccel(const std::string& typeID, double accel) {
    Dom::setDouble(libsumo::VAR_ACCEL, typeID, accel);
}


void
VehicleType::setDecel(const std::string& typeID, double decel) {
    Dom::setDouble(libsumo::VAR_DECEL, typeID, decel);
}


void
VehicleType::setEmergencyDecel(const std::string& typeID, double decel) {
    Dom::setDouble(libsumo::VAR_EMERGENCY_DECEL, typeID, decel);
}


void
VehicleType::setApparentDecel(const std::string& typeID, double decel) {
    Dom::setDouble(libsumo::VAR_APPARENT_DECEL, typeID, decel);
}


void
VehicleType::setImperfection(const std::string& typeID, double imperfection) {
    Dom::setDouble(libsumo::VAR_IMPERFECTION, typeID, imperfection);
}


void
VehicleType::setTau(const std::string& typeID, double tau) {
    Dom::setDouble(libsumo::VAR_TAU, typeID, tau);
}


void
VehicleType::setMinGapLat(const std::string& typeID, double minGapLat) {
    Dom::setDouble(libsumo::VAR_MINGAP_LAT, typeID, minGapLat);
}


void
VehicleType::setMaxSpeedLat(const std::string& typeID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED_LAT, typeID, speed);
}


void
VehicleType::setLateralAlignment(const std::string& typeID, const std::string& latAlignment) {
    Dom::setString(libsumo::VAR_LATALIGNMENT, typeID, latAlignment);
}

void
VehicleType::setScale(const std::string& typeID, double value) {
    Dom::setDouble(libsumo::VAR_SCALE, typeID, value);
}

}


/****************************************************************************/
