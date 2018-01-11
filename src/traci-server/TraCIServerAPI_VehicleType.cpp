/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <limits>
#include <utils/emissions/PollutantsInterface.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include <libsumo/VehicleType.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_VehicleType.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_VehicleType::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                       tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_LENGTH && variable != VAR_MAXSPEED && variable != VAR_ACCEL
            && variable != VAR_DECEL && variable != VAR_EMERGENCY_DECEL && variable != VAR_APPARENT_DECEL
            && variable != VAR_TAU && variable != VAR_VEHICLECLASS && variable != VAR_EMISSIONCLASS
            && variable != VAR_SHAPECLASS && variable != VAR_ACTIONSTEPLENGTH
            && variable != VAR_SPEED_FACTOR && variable != VAR_SPEED_DEVIATION && variable != VAR_IMPERFECTION
            && variable != VAR_MINGAP && variable != VAR_WIDTH && variable != VAR_COLOR && variable != ID_COUNT
            && variable != VAR_HEIGHT
            && variable != VAR_MINGAP_LAT
            && variable != VAR_MAXSPEED_LAT
            && variable != VAR_LATALIGNMENT
            && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE,
                                          "Get Vehicle Type Variable: unsupported variable " + toHex(variable, 2)
                                          + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_VEHICLETYPE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == ID_LIST) {
        std::vector<std::string> ids = libsumo::VehicleType::getIDList();
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids = libsumo::VehicleType::getIDList();
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        try {
            switch (variable) {
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE,
                                                          "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(libsumo::VehicleType::getParameter(id, paramName));
                }
                break;
                default:
                    getVariable(variable, id, tempMsg);
                    break;
            }
        } catch (libsumo::TraCIException& e) {
            return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, e.what(), outputStorage);
        }
    }
    server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

bool
TraCIServerAPI_VehicleType::getVariable(const int variable, const std::string& id, tcpip::Storage& tempMsg) {
    switch (variable) {
        case VAR_LENGTH: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getLength(id));
        }
        break;
        case VAR_HEIGHT: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getHeight(id));
        }
        break;
        case VAR_MINGAP: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getMinGap(id));
        }
        break;
        case VAR_MAXSPEED: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getMaxSpeed(id));
        }
        break;
        case VAR_ACCEL: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getAccel(id));
        }
        break;
        case VAR_DECEL: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getDecel(id));
        }
        break;
        case VAR_EMERGENCY_DECEL: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getEmergencyDecel(id));
        }
        break;
        case VAR_APPARENT_DECEL: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getApparentDecel(id));
        }
        break;
        case VAR_ACTIONSTEPLENGTH: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getActionStepLength(id));
        }
        break;
        case VAR_IMPERFECTION: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getImperfection(id));
        }
        break;
        case VAR_TAU: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getTau(id));
        }
        break;
        case VAR_SPEED_FACTOR: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getSpeedFactor(id));
        }
        break;
        case VAR_SPEED_DEVIATION: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getSpeedDeviation(id));
        }
        break;
        case VAR_VEHICLECLASS: {
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(libsumo::VehicleType::getVehicleClass(id));
        }
        break;
        case VAR_EMISSIONCLASS: {
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(libsumo::VehicleType::getEmissionClass(id));
        }
        break;
        case VAR_SHAPECLASS: {
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(libsumo::VehicleType::getShapeClass(id));
        }
        break;
        case VAR_WIDTH: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getWidth(id));
        }
        break;
        case VAR_COLOR: {
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            const libsumo::TraCIColor& col = libsumo::VehicleType::getColor(id);
            tempMsg.writeUnsignedByte(col.r);
            tempMsg.writeUnsignedByte(col.g);
            tempMsg.writeUnsignedByte(col.b);
            tempMsg.writeUnsignedByte(col.a);
        }
        break;
        case VAR_MINGAP_LAT: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getMinGapLat(id));
        }
        break;
        case VAR_MAXSPEED_LAT: {
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(libsumo::VehicleType::getMaxSpeedLat(id));
        }
        break;
        case VAR_LATALIGNMENT: {
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(libsumo::VehicleType::getLateralAlignment(id));
        }
        break;
        default:
            break;
    }
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
            } catch (InvalidArgument e) {
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
            } catch (InvalidArgument e) {
                return server.writeErrorStatusCmd(cmd, "Unknown emission class '" + eclass + "'.", outputStorage);
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
            } catch (InvalidArgument e) {
                return server.writeErrorStatusCmd(cmd, "Unknown vehicle shape " + sclass + "'.", outputStorage);
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
