/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2022 German Aerospace Center (DLR) and others.
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
/// @file    LaneArea.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/LaneArea.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_LANEAREA_VARIABLE, libsumo::CMD_SET_LANEAREA_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
LaneArea::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
LaneArea::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


int
LaneArea::getJamLengthVehicle(const std::string& detID) {
    return Dom::getInt(libsumo::JAM_LENGTH_VEHICLE, detID);
}


double
LaneArea::getJamLengthMeters(const std::string& detID) {
    return Dom::getDouble(libsumo::JAM_LENGTH_METERS, detID);
}


double
LaneArea::getLastStepMeanSpeed(const std::string& detID) {
    return Dom::getDouble(libsumo::LAST_STEP_MEAN_SPEED, detID);
}


std::vector<std::string>
LaneArea::getLastStepVehicleIDs(const std::string& detID) {
    return Dom::getStringVector(libsumo::LAST_STEP_VEHICLE_ID_LIST, detID);
}


double
LaneArea::getLastStepOccupancy(const std::string& detID) {
    return Dom::getDouble(libsumo::LAST_STEP_OCCUPANCY, detID);
}


double
LaneArea::getPosition(const std::string& detID) {
    return Dom::getDouble(libsumo::VAR_POSITION, detID);
}


std::string
LaneArea::getLaneID(const std::string& detID) {
    return Dom::getString(libsumo::VAR_LANE_ID, detID);
}


double
LaneArea::getLength(const std::string& detID) {
    return Dom::getDouble(libsumo::VAR_LENGTH, detID);
}


int
LaneArea::getLastStepVehicleNumber(const std::string& detID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_NUMBER, detID);
}


int
LaneArea::getLastStepHaltingNumber(const std::string& detID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_HALTING_NUMBER, detID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(LaneArea, LANEAREA)
LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(LaneArea, LANEAREA)


}  // namespace libtraci

/****************************************************************************/
