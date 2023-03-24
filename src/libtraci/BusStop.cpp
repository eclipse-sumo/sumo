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
/// @file    BusStop.cpp
/// @author  Angelo Banse
/// @date    10.11.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/TraCIConstants.h>
#include <libsumo/BusStop.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_BUSSTOP_VARIABLE, libsumo::CMD_SET_BUSSTOP_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
BusStop::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}

int
BusStop::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


std::string
BusStop::getLaneID(const std::string& stopID) {
    return Dom::getString(libsumo::VAR_LANE_ID, stopID);
}

double
BusStop::getStartPos(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_POSITION, stopID);
}

double
BusStop::getEndPos(const std::string& stopID) {
    return Dom::getDouble(libsumo::VAR_LANEPOSITION, stopID);
}

std::string
BusStop::getName(const std::string& stopID) {
    return Dom::getString(libsumo::VAR_NAME, stopID);
}

int
BusStop::getVehicleCount(const std::string& stopID) {
    return Dom::getInt(libsumo::VAR_STOP_STARTING_VEHICLES_NUMBER, stopID);
}

std::vector<std::string>
BusStop::getVehicleIDs(const std::string& stopID) {
    return Dom::getStringVector(libsumo::VAR_STOP_STARTING_VEHICLES_IDS, stopID);
}

int
BusStop::getPersonCount(const std::string& stopID) {
    return Dom::getInt(libsumo::VAR_BUS_STOP_WAITING, stopID);
}

std::vector<std::string>
BusStop::getPersonIDs(const std::string& stopID) {
    return Dom::getStringVector(libsumo::VAR_BUS_STOP_WAITING_IDS, stopID);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(BusStop, BUSSTOP)
LIBTRACI_PARAMETER_IMPLEMENTATION(BusStop, BUSSTOP)


}


/****************************************************************************/
