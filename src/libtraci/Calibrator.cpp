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
    return Dom::getString(libsumo::LANE_EDGE_ID, "");
}

std::string
Calibrator::getLaneID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_LANE_ID, "");
}

double
Calibrator::getVehsPerHour(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_VEHSPERHOUR, "");
}

double
Calibrator::getSpeed(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_SPEED, "");
}

std::string
Calibrator::getTypeID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_TYPE, "");
}

double
Calibrator::getBegin(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_BEGIN, "");
}

double
Calibrator::getEnd(const std::string& calibratorID) {
    return Dom::getDouble(libsumo::VAR_END, "");
}

std::string
Calibrator::getRouteID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_ROUTE_ID, "");
}

std::string
Calibrator::getRouteProbeID(const std::string& calibratorID) {
    return Dom::getString(libsumo::VAR_ROUTE_PROBE, "");
}

std::vector<std::string>
Calibrator::getVTypes(const std::string& calibratorID) {
    return Dom::getStringVector(libsumo::VAR_VTYPES, calibratorID);
}


int
Calibrator::getPassed(const std::string& calibratorID) {
    return Dom::getInt(libsumo::VAR_PASSED, "");
}

int
Calibrator::getInserted(const std::string& calibratorID) {
    return Dom::getInt(libsumo::VAR_INSERTED, "");
}

int
Calibrator::getRemoved(const std::string& calibratorID) {
    return Dom::getInt(libsumo::VAR_REMOVED, "");
}

std::string
Calibrator::getParameter(const std::string& calibratorID, const std::string& param) {
    return Dom::getString(libsumo::VAR_PARAMETER, "");
}

//LIBTRACI_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(Calibrator, CALIBRATOR)
LIBTRACI_PARAMETER_IMPLEMENTATION(Calibrator, CALIBRATOR)

void
Calibrator::setParameter(const std::string& calibratorID, const std::string& key, const std::string& value) {
    Dom::setString(libsumo::VAR_PARAMETER, calibratorID, key);
}

//void
//Calibrator::setFlow(const std::string& calibratorID, double begin, double end, double vehsPerHour, double speed, const std::string& typeID,
//                    const std::string& routeID,
//                    const std::string& departLane,
//                    const std::string& departSpeed) {
//
//    Dom::setString(libsumo::CMD_SET_FLOW, calibratorID, );
//    std::string error;
//    SUMOVehicleParameter vehicleParams;
//    vehicleParams.vtypeid = typeID;
//    vehicleParams.routeid = routeID;
//    MSVehicleType* t = MSNet::getInstance()->getVehicleControl().getVType(typeID);
//    if (t == nullptr) {
//        throw libsumo::TraCIException("Vehicle type '" + typeID + "' is not known");
//    }
//    if (!SUMOVehicleParameter::parseDepartLane(departLane, "calibrator", calibratorID, vehicleParams.departLane, vehicleParams.departLaneProcedure, error)) {
//        throw libsumo::TraCIException(error);
//    }
//    if (!SUMOVehicleParameter::parseDepartSpeed(departSpeed, "calibrator", calibratorID, vehicleParams.departSpeed, vehicleParams.departSpeedProcedure, error)) {
//        throw libsumo::TraCIException::TraCIException(error);
//    }
//    getCalibrator(calibratorID)->setFlow(TIME2STEPS(begin), TIME2STEPS(end), vehsPerHour, speed, vehicleParams);
//}
//
//
//LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Calibrator, CALIBRATOR)
//
//
//MSCalibrator*
//Calibrator::getCalibrator(const std::string& id) {
//    const auto& dict = MSCalibrator::getInstances();
//    auto it = dict.find(id);
//    if (it == dict.end()) {
//        throw libsumo::TraCIException::TraCIException("Calibrator '" + id + "' is not known");
//    }
//    return it->second;
//}
//
//MSCalibrator::AspiredState
//Calibrator::getCalibratorState(const MSCalibrator* c) {
//    try {
//        return c->getCurrentStateInterval();
//    }
//    catch (ProcessError& e) {
//        throw libsumo::TraCIException::TraCIException(e.what());
//    }
//}


/****************************************************************************/
