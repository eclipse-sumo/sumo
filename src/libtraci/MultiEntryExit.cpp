/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    MultiEntryExit.cpp
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
#include <libsumo/MultiEntryExit.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_MULTIENTRYEXIT_VARIABLE, libsumo::CMD_SET_MULTIENTRYEXIT_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
MultiEntryExit::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
MultiEntryExit::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


std::vector<std::string>
MultiEntryExit::getEntryLanes(const std::string& detID) {
    return Dom::getStringVector(libsumo::VAR_LANES, detID);
}


std::vector<std::string>
MultiEntryExit::getExitLanes(const std::string& detID) {
    return Dom::getStringVector(libsumo::VAR_EXIT_LANES, detID);
}


std::vector<double>
MultiEntryExit::getEntryPositions(const std::string& detID) {
    return Dom::getDoubleVector(libsumo::VAR_POSITION, detID);
}


std::vector<double>
MultiEntryExit::getExitPositions(const std::string& detID) {
    return Dom::getDoubleVector(libsumo::VAR_EXIT_POSITIONS, detID);
}


int
MultiEntryExit::getLastStepVehicleNumber(const std::string& detID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_NUMBER, detID);
}


double
MultiEntryExit::getLastStepMeanSpeed(const std::string& detID) {
    return Dom::getDouble(libsumo::LAST_STEP_MEAN_SPEED, detID);
}


std::vector<std::string>
MultiEntryExit::getLastStepVehicleIDs(const std::string& detID) {
    return Dom::getStringVector(libsumo::LAST_STEP_VEHICLE_ID_LIST, detID);
}


int
MultiEntryExit::getLastStepHaltingNumber(const std::string& detID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_HALTING_NUMBER, detID);
}


double
MultiEntryExit::getLastIntervalMeanTravelTime(const std::string& detID) {
    return Dom::getDouble(libsumo::VAR_LAST_INTERVAL_TRAVELTIME, detID);
}


double
MultiEntryExit::getLastIntervalMeanHaltsPerVehicle(const std::string& detID) {
    return Dom::getDouble(libsumo::VAR_LAST_INTERVAL_MEAN_HALTING_NUMBER, detID);
}


double
MultiEntryExit::getLastIntervalMeanTimeLoss(const std::string& detID) {
    return Dom::getDouble(libsumo::VAR_TIMELOSS, detID);
}


int
MultiEntryExit::getLastIntervalVehicleSum(const std::string& detID) {
    return Dom::getInt(libsumo::VAR_LAST_INTERVAL_VEHICLE_NUMBER, detID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(MultiEntryExit, MULTIENTRYEXIT)
LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(MultiEntryExit, MULTIENTRYEXIT)


}  // namespace libtraci

/****************************************************************************/
