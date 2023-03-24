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
/// @file    Calibrator.cpp
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/TraCIConstants.h>
#include <libsumo/Calibrator.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_CALIBRATOR_VARIABLE, libsumo::CMD_SET_CALIBRATOR_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Calibrator::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}

int
Calibrator::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}

std::string
Calibrator::getEdgeID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_ROAD_ID, calibratorID);
}

std::string
Calibrator::getLaneID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_LANE_ID, calibratorID);
}

double
Calibrator::getVehsPerHour(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_VEHSPERHOUR, calibratorID);
}

double
Calibrator::getSpeed(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_SPEED, calibratorID);
}

std::string
Calibrator::getTypeID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_TYPE, calibratorID);
}

double
Calibrator::getBegin(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_BEGIN, calibratorID);
}

double
Calibrator::getEnd(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_END, calibratorID);
}

std::string
Calibrator::getRouteID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_ROUTE_ID, calibratorID);
}

std::string
Calibrator::getRouteProbeID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_ROUTE_PROBE, calibratorID);
}

std::vector<std::string>
Calibrator::getVTypes(const std::string& calibratorID) {
    return Dom::getStringVector(libsumo::VAR_VTYPES, calibratorID);
}


int
Calibrator::getPassed(const std::string& calibratorID) {
    return Dom::getInt(libsumo::VAR_PASSED, calibratorID);
}

int
Calibrator::getInserted(const std::string& calibratorID) {
    return Dom::getInt(libsumo::VAR_INSERTED, calibratorID);
}

int
Calibrator::getRemoved(const std::string& calibratorID) {
    return Dom::getInt(libsumo::VAR_REMOVED, calibratorID);
}

LIBTRACI_PARAMETER_IMPLEMENTATION(Calibrator, CALIBRATOR)

void
Calibrator::setFlow(const std::string& calibratorID, double begin, double end, double vehsPerHour, double speed,
                    const std::string& typeID, const std::string& routeID, const std::string& departLane, const std::string& departSpeed) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(8);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(begin);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(end);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(vehsPerHour);
    content.writeByte(libsumo::TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(typeID);
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(routeID);
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(departLane);
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(departSpeed);
    Dom::set(libsumo::CMD_SET_FLOW, calibratorID, &content);
}

LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Calibrator, CALIBRATOR)


}


/****************************************************************************/
