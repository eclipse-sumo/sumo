/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
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
/// @file    ChargingStation.cpp
/// @author  Angelo Banse
/// @date    10.11.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/TraCIConstants.h>
#include <libsumo/ChargingStation.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_CHARGINGSTATION_VARIABLE, libsumo::CMD_SET_CHARGINGSTATION_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================

std::vector<std::string>
ChargingStation::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}

int
ChargingStation::getIDCount() {
    return (int)Dom::getStringVector(libsumo::TRACI_ID_LIST, "").size();
}


std::string
ChargingStation::getLaneID(const std::string& stopID) {
    return Dom::getString(libsumo::VAR_LANE_ID, stopID);
}

double
ChargingStation::getStartPos(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_POSITION, stopID);
}

double
ChargingStation::getEndPos(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_LANEPOSITION, stopID);
}

std::string
ChargingStation::getName(const std::string& stopID) {
    return Dom::getString(libsumo::VAR_NAME, stopID);
}

int
ChargingStation::getVehicleCount(const std::string& stopID) {
    return Dom::getInt(libsumo::VAR_STOP_STARTING_VEHICLES_NUMBER, stopID);
}

std::vector<std::string>
ChargingStation::getVehicleIDs(const std::string& stopID) {
    return Dom::getStringVector(libsumo::VAR_STOP_STARTING_VEHICLES_IDS, stopID);
}

double
ChargingStation::getChargingPower(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_CS_POWER, stopID);
}

double
ChargingStation::getEfficiency(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_CS_EFFICIENCY, stopID);
}

double
ChargingStation::getChargeDelay(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_CS_CHARGE_DELAY, stopID);
}

int
ChargingStation::getChargeInTransit(const std::string& stopID) {
    return Dom::getInt(libsumo::VAR_CS_CHARGE_IN_TRANSIT, stopID);
}

LIBTRACI_PARAMETER_IMPLEMENTATION(ChargingStation, CHARGINGSTATION)

void
ChargingStation::setChargingPower(const std::string& stopID, double chargingpower) {
    Dom::setDouble(libsumo::VAR_CS_POWER, stopID, chargingpower);
}

void
ChargingStation::setEfficiency(const std::string& stopID, double efficiency) {
    Dom::setDouble(libsumo::VAR_CS_EFFICIENCY, stopID, efficiency);
}

void
ChargingStation::setChargeDelay(const std::string& stopID, double delay) {
    Dom::setDouble(libsumo::VAR_CS_CHARGE_DELAY, stopID, delay);
}

void
ChargingStation::setChargeInTransit(const std::string& stopID, bool value) {
    Dom::setInt(libsumo::VAR_CS_CHARGE_IN_TRANSIT, stopID, value);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(ChargingStation, CHARGINGSTATION)

}


/****************************************************************************/
