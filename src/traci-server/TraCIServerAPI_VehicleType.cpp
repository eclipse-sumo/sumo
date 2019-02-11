/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_VehicleType.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Robert Hilbrich
/// @author  Gregor Laemmel
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle type values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <limits>
#include <utils/emissions/PollutantsInterface.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/VehicleType.h>
#include "TraCIServerAPI_VehicleType.h"

using namespace libsumo;

// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_VehicleType::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                       tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_VEHICLETYPE_VARIABLE, variable, id);
    try {
        if (!libsumo::VehicleType::handleVariable(id, variable, &server)) {
            switch (variable) {
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE,
                                                          "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(libsumo::VehicleType::getParameter(id, paramName));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE,
                                                      "Get Vehicle Type Variable: unsupported variable " + toHex(variable, 2)
                                                      + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_VehicleType::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                       tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_LENGTH && variable != VAR_MAXSPEED && variable != VAR_VEHICLECLASS
            && variable != VAR_SPEED_FACTOR && variable != VAR_SPEED_DEVIATION && variable != VAR_EMISSIONCLASS
            && variable != VAR_WIDTH && variable != VAR_MINGAP && variable != VAR_SHAPECLASS
            && variable != VAR_ACCEL && variable != VAR_IMPERFECTION
            && variable != VAR_DECEL && variable != VAR_EMERGENCY_DECEL && variable != VAR_APPARENT_DECEL
            && variable != VAR_TAU && variable != VAR_COLOR && variable != VAR_ACTIONSTEPLENGTH
            && variable != VAR_HEIGHT
            && variable != VAR_MINGAP_LAT
            && variable != VAR_MAXSPEED_LAT
            && variable != VAR_LATALIGNMENT
            && variable != VAR_PARAMETER
            && variable != COPY
       ) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE,
                                          "Change Vehicle Type State: unsupported variable " + toHex(variable, 2)
                                          + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
//    MSVehicleType* v = libsumo::VehicleType::getVType(id);
//    if (v == 0) {
//        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, "Vehicle type '" + id + "' is not known",
//                                          outputStorage);
//    }
    // process
    try {
        if (setVariable(CMD_SET_VEHICLETYPE_VARIABLE, variable, id, server, inputStorage, outputStorage)) {
            server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_OK, warning, outputStorage);
            return true;
        }
    } catch (ProcessError& e) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, e.what(), outputStorage);
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, e.what(), outputStorage);
    }
    return false;
}


bool
TraCIServerAPI_VehicleType::setVariable(const int cmd, const int variable,
                                        const std::string& id, TraCIServer& server,
                                        tcpip::Storage& inputStorage, tcpip::Storage& outputStorage) {
    switch (variable) {
        case VAR_LENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting length requires a double.", outputStorage);
            }
            if (value <= 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid length.", outputStorage);
            }
            libsumo::VehicleType::setLength(id, value);
        }
        break;
        case VAR_HEIGHT: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting height requires a double.", outputStorage);
            }
            if (value <= 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid height.", outputStorage);
            }
            libsumo::VehicleType::setHeight(id, value);
        }
        break;
        case VAR_MAXSPEED: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting maximum speed requires a double.", outputStorage);
            }
            if (value <= 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid maximum speed.", outputStorage);
            }
            libsumo::VehicleType::setMaxSpeed(id, value);
        }
        break;
        case VAR_VEHICLECLASS: {
            std::string vclass;
            if (!server.readTypeCheckingString(inputStorage, vclass)) {
                return server.writeErrorStatusCmd(cmd, "Setting vehicle class requires a string.", outputStorage);
            }
            try {
                libsumo::VehicleType::setVehicleClass(id, vclass);
            } catch (InvalidArgument&) {
                return server.writeErrorStatusCmd(cmd, "Unknown vehicle class '" + vclass + "'.", outputStorage);
            }
        }
        break;
        case VAR_SPEED_FACTOR: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting speed factor requires a double.", outputStorage);
            }
            if (value <= 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid speed factor.", outputStorage);
            }
            libsumo::VehicleType::setSpeedFactor(id, value);
        }
        break;
        case VAR_SPEED_DEVIATION: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting speed deviation requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid speed deviation.", outputStorage);
            }
            libsumo::VehicleType::setSpeedDeviation(id, value);
        }
        break;
        case VAR_EMISSIONCLASS: {
            std::string eclass;
            if (!server.readTypeCheckingString(inputStorage, eclass)) {
                return server.writeErrorStatusCmd(cmd, "Setting emission class requires a string.", outputStorage);
            }
            try {
                libsumo::VehicleType::setEmissionClass(id, eclass);
            } catch (InvalidArgument& e) {
                return server.writeErrorStatusCmd(cmd, e.what(), outputStorage);
            }
        }
        break;
        case VAR_WIDTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting width requires a double.", outputStorage);
            }
            if (value <= 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid width.", outputStorage);
            }
            libsumo::VehicleType::setWidth(id, value);
        }
        break;
        case VAR_MINGAP: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting minimum gap requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid minimum gap.", outputStorage);
            }
            libsumo::VehicleType::setMinGap(id, value);
        }
        break;
        case VAR_MINGAP_LAT: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting minimum lateral gap requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid minimum lateral gap.", outputStorage);
            }
            libsumo::VehicleType::setMinGapLat(id, value);
        }
        break;
        case VAR_MAXSPEED_LAT: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting maximum lateral speed requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid maximum lateral speed.", outputStorage);
            }
            libsumo::VehicleType::setMaxSpeedLat(id, value);
        }
        break;
        case VAR_LATALIGNMENT: {
            std::string latAlign;
            if (!server.readTypeCheckingString(inputStorage, latAlign)) {
                return server.writeErrorStatusCmd(cmd, "Setting preferred lateral alignment requires a string.",
                                                  outputStorage);
            }
            if (SUMOXMLDefinitions::LateralAlignments.hasString(latAlign)) {
                libsumo::VehicleType::setLateralAlignment(id, latAlign);
            } else {
                return server.writeErrorStatusCmd(cmd, "Unknown lateral alignment " + latAlign + "'.", outputStorage);
            }
        }
        break;
        case VAR_SHAPECLASS: {
            std::string sclass;
            if (!server.readTypeCheckingString(inputStorage, sclass)) {
                return server.writeErrorStatusCmd(cmd, "Setting vehicle shape requires a string.", outputStorage);
            }
            try {
                libsumo::VehicleType::setShapeClass(id, sclass);
            } catch (InvalidArgument& e) {
                return server.writeErrorStatusCmd(cmd, e.what(), outputStorage);
            }
        }
        break;
        case VAR_ACCEL: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting acceleration requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid acceleration.", outputStorage);
            }
            libsumo::VehicleType::setAccel(id, value);
        }
        break;
        case VAR_DECEL: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting deceleration requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid deceleration.", outputStorage);
            }
            libsumo::VehicleType::setDecel(id, value);
        }
        break;
        case VAR_EMERGENCY_DECEL: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting deceleration requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid deceleration.", outputStorage);
            }
            libsumo::VehicleType::setEmergencyDecel(id, value);
        }
        break;
        case VAR_APPARENT_DECEL: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting deceleration requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid deceleration.", outputStorage);
            }
            libsumo::VehicleType::setApparentDecel(id, value);
        }
        break;
        case VAR_ACTIONSTEPLENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting action step length requires a double.", outputStorage);
            }
            if (fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid action step length.", outputStorage);
            }
            bool resetActionOffset = value >= 0.0;
            libsumo::VehicleType::setActionStepLength(id, fabs(value), resetActionOffset);
        }
        break;
        case VAR_IMPERFECTION: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting driver imperfection requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid driver imperfection.", outputStorage);
            }
            libsumo::VehicleType::setImperfection(id, value);
        }
        break;
        case VAR_TAU: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting headway time requires a double.", outputStorage);
            }
            if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid headway time.", outputStorage);
            }
            libsumo::VehicleType::setTau(id, value);
        }
        break;
        case VAR_COLOR: {
            libsumo::TraCIColor col;
            if (!server.readTypeCheckingColor(inputStorage, col)) {
                return server.writeErrorStatusCmd(cmd, "The color must be given using the according type.", outputStorage);
            }
            libsumo::VehicleType::setColor(id, col);
        }
        break;
        case COPY: {
            std::string newTypeID;
            if (!server.readTypeCheckingString(inputStorage, newTypeID)) {
                return server.writeErrorStatusCmd(cmd, "copying a vehicle type requires a string.",
                                                  outputStorage);
            }
            libsumo::VehicleType::copy(id, newTypeID);
        }
        break;
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(cmd, "A compound object is needed for setting a parameter.",
                                                  outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(cmd, "The name of the parameter must be given as a string.",
                                                  outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "The value of the parameter must be given as a string.",
                                                  outputStorage);
            }
            libsumo::VehicleType::setParameter(id, name, value);
        }
        break;
        default:
            break;
    }
    return true;
}


/****************************************************************************/
