/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    InductionLoop.cpp
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
#include <libsumo/InductionLoop.h>
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, libsumo::CMD_SET_INDUCTIONLOOP_VARIABLE> Dom;


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
InductionLoop::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
InductionLoop::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


double
InductionLoop::getPosition(const std::string& loopID) {
    return Dom::getDouble(libsumo::VAR_POSITION, loopID);
}


std::string
InductionLoop::getLaneID(const std::string& loopID) {
    return Dom::getString(libsumo::VAR_LANE_ID, loopID);
}


int
InductionLoop::getLastStepVehicleNumber(const std::string& loopID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_NUMBER, loopID);
}


double
InductionLoop::getLastStepMeanSpeed(const std::string& loopID) {
    return Dom::getDouble(libsumo::LAST_STEP_MEAN_SPEED, loopID);
}


std::vector<std::string>
InductionLoop::getLastStepVehicleIDs(const std::string& loopID) {
    return Dom::getStringVector(libsumo::LAST_STEP_VEHICLE_ID_LIST, loopID);
}


double
InductionLoop::getLastStepOccupancy(const std::string& loopID) {
    return Dom::getDouble(libsumo::LAST_STEP_OCCUPANCY, loopID);
}


double
InductionLoop::getLastStepMeanLength(const std::string& loopID) {
    return Dom::getDouble(libsumo::LAST_STEP_LENGTH, loopID);
}


double
InductionLoop::getTimeSinceDetection(const std::string& loopID) {
    return Dom::getDouble(libsumo::LAST_STEP_TIME_SINCE_DETECTION, loopID);
}


std::vector<libsumo::TraCIVehicleData>
InductionLoop::getVehicleData(const std::string& loopID) {
    std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
    std::vector<libsumo::TraCIVehicleData> result;
    tcpip::Storage& ret = Dom::get(libsumo::LAST_STEP_VEHICLE_DATA, loopID);
    ret.readInt();  // components
    // number of items
    ret.readUnsignedByte();
    const int n = ret.readInt();
    for (int i = 0; i < n; ++i) {
        libsumo::TraCIVehicleData vd;

        ret.readUnsignedByte();
        vd.id = ret.readString();

        ret.readUnsignedByte();
        vd.length = ret.readDouble();

        ret.readUnsignedByte();
        vd.entryTime = ret.readDouble();

        ret.readUnsignedByte();
        vd.leaveTime = ret.readDouble();

        ret.readUnsignedByte();
        vd.typeID = ret.readString();

        result.push_back(vd);
    }
    return result;
}


double
InductionLoop::getIntervalOccupancy(const std::string& loopID) {
    return Dom::getDouble(libsumo::VAR_INTERVAL_OCCUPANCY, loopID);
}


double
InductionLoop::getIntervalMeanSpeed(const std::string& loopID) {
    return Dom::getDouble(libsumo::VAR_INTERVAL_SPEED, loopID);
}


int
InductionLoop::getIntervalVehicleNumber(const std::string& loopID) {
    return Dom::getInt(libsumo::VAR_INTERVAL_NUMBER, loopID);
}


std::vector<std::string>
InductionLoop::getIntervalVehicleIDs(const std::string& loopID) {
    return Dom::getStringVector(libsumo::VAR_INTERVAL_IDS, loopID);
}


double
InductionLoop::getLastIntervalOccupancy(const std::string& loopID) {
    return Dom::getDouble(libsumo::VAR_LAST_INTERVAL_OCCUPANCY, loopID);
}


double
InductionLoop::getLastIntervalMeanSpeed(const std::string& loopID) {
    return Dom::getDouble(libsumo::VAR_LAST_INTERVAL_SPEED, loopID);
}


int
InductionLoop::getLastIntervalVehicleNumber(const std::string& loopID) {
    return Dom::getInt(libsumo::VAR_LAST_INTERVAL_NUMBER, loopID);
}


std::vector<std::string>
InductionLoop::getLastIntervalVehicleIDs(const std::string& loopID) {
    return Dom::getStringVector(libsumo::VAR_LAST_INTERVAL_IDS, loopID);
}


void
InductionLoop::overrideTimeSinceDetection(const std::string& loopID, double time) {
    Dom::setDouble(libsumo::VAR_VIRTUAL_DETECTION, loopID, time);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(InductionLoop, INDUCTIONLOOP)
LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(InductionLoop, INDUCTIONLOOP)


}  // namespace libtraci

/****************************************************************************/
