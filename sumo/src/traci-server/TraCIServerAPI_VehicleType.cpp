/****************************************************************************/
/// @file    TraCIServerAPI_VehicleType.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle type values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <limits>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_VehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
    if (variable != ID_LIST && variable != VAR_LENGTH && variable != VAR_MAXSPEED && variable != VAR_ACCEL && variable != VAR_DECEL
            && variable != VAR_TAU && variable != VAR_VEHICLECLASS && variable != VAR_EMISSIONCLASS && variable != VAR_SHAPECLASS
            && variable != VAR_SPEED_FACTOR && variable != VAR_SPEED_DEVIATION && variable != VAR_IMPERFECTION
            && variable != VAR_MINGAP && variable != VAR_WIDTH && variable != VAR_COLOR && variable != ID_COUNT) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, "Get Vehicle Type Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_VEHICLETYPE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getVehicleControl().insertVTypeIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getVehicleControl().insertVTypeIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        MSVehicleType* v = MSNet::getInstance()->getVehicleControl().getVType(id);
        if (v == 0) {
            return server.writeErrorStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, "Vehicle type '" + id + "' is not known", outputStorage);
        }
        getVariable(variable, *v, tempMsg);
    }
    server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

bool
TraCIServerAPI_VehicleType::getVariable(const int variable, const MSVehicleType& v, tcpip::Storage& tempMsg) {
    switch (variable) {
        case VAR_LENGTH:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getLength());
            break;
        case VAR_MINGAP:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getMinGap());
            break;
        case VAR_MAXSPEED:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getMaxSpeed());
            break;
        case VAR_ACCEL:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getCarFollowModel().getMaxAccel());
            break;
        case VAR_DECEL:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getCarFollowModel().getMaxDecel());
            break;
        case VAR_IMPERFECTION:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getCarFollowModel().getImperfection());
            break;
        case VAR_TAU:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getCarFollowModel().getHeadwayTime());
            break;
        case VAR_SPEED_FACTOR:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getSpeedFactor());
            break;
        case VAR_SPEED_DEVIATION:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getSpeedDeviation());
            break;
        case VAR_VEHICLECLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(toString(v.getVehicleClass()));
            break;
        case VAR_EMISSIONCLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleEmissionTypeName(v.getEmissionClass()));
            break;
        case VAR_SHAPECLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleShapeName(v.getGuiShape()));
            break;
        case VAR_WIDTH:
            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
            tempMsg.writeDouble(v.getWidth());
            break;
        case VAR_COLOR:
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            tempMsg.writeUnsignedByte(v.getColor().red());
            tempMsg.writeUnsignedByte(v.getColor().green());
            tempMsg.writeUnsignedByte(v.getColor().blue());
            tempMsg.writeUnsignedByte(v.getColor().alpha());
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
            && variable != VAR_ACCEL && variable != VAR_DECEL && variable != VAR_IMPERFECTION
            && variable != VAR_TAU && variable != VAR_COLOR
       ) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, "Change Vehicle Type State: unsupported variable specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    MSVehicleType* v = MSNet::getInstance()->getVehicleControl().getVType(id);
    if (v == 0) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, "Vehicle type '" + id + "' is not known", outputStorage);
    }
    // process
    try {
        if (setVariable(CMD_SET_VEHICLETYPE_VARIABLE, variable, *v, server, inputStorage, outputStorage)) {
            server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_OK, warning, outputStorage);
            return true;
        }
    } catch (ProcessError& e) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, e.what(), outputStorage);
    }
    return false;
}


bool
TraCIServerAPI_VehicleType::setVariable(const int cmd, const int variable,
                                        MSVehicleType& v, TraCIServer& server,
                                        tcpip::Storage& inputStorage, tcpip::Storage& outputStorage) {
    switch (variable) {
        case VAR_LENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting length requires a double.", outputStorage);
            }
            if (value == 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid length.", outputStorage);
            }
            v.setLength(value);
        }
        break;
        case VAR_MAXSPEED: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting maximum speed requires a double.", outputStorage);
            }
            if (value == 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                return server.writeErrorStatusCmd(cmd, "Invalid maximum speed.", outputStorage);
            }
            v.setMaxSpeed(value);
        }
        break;
        case VAR_VEHICLECLASS: {
            std::string vclass;
            if (!server.readTypeCheckingString(inputStorage, vclass)) {
                return server.writeErrorStatusCmd(cmd, "Setting vehicle class requires a string.", outputStorage);
            }
            v.setVClass(getVehicleClassID(vclass));
        }
        break;
        case VAR_SPEED_FACTOR: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting speed factor requires a double.", outputStorage);
            }
            v.setSpeedFactor(value);
        }
        break;
        case VAR_SPEED_DEVIATION: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting speed deviation requires a double.", outputStorage);
            }
            v.setSpeedDeviation(value);
        }
        break;
        case VAR_EMISSIONCLASS: {
            std::string eclass;
            if (!server.readTypeCheckingString(inputStorage, eclass)) {
                return server.writeErrorStatusCmd(cmd, "Setting emission class requires a string.", outputStorage);
            }
            v.setEmissionClass(getVehicleEmissionTypeID(eclass));
        }
        break;
        case VAR_WIDTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage,  value)) {
                return server.writeErrorStatusCmd(cmd, "Setting width requires a double.", outputStorage);
            }
            v.setWidth(value);
        }
        break;
        case VAR_MINGAP: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting minimum gap requires a double.", outputStorage);
            }
            v.setMinGap(value);
        }
        break;
        case VAR_SHAPECLASS: {
            std::string sclass;
            if (!server.readTypeCheckingString(inputStorage, sclass)) {
                return server.writeErrorStatusCmd(cmd, "Setting vehicle shape requires a string.", outputStorage);
            }
            v.setShape(getVehicleShapeID(sclass));
        }
        break;
        case VAR_ACCEL: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting acceleration requires a double.", outputStorage);
            }
            v.getCarFollowModel().setMaxAccel(value);
        }
        break;
        case VAR_DECEL: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting deceleration requires a double.", outputStorage);
            }
            v.getCarFollowModel().setMaxDecel(value);
        }
        break;
        case VAR_IMPERFECTION: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting driver imperfection requires a double.", outputStorage);
            }
            v.getCarFollowModel().setImperfection(value);
        }
        break;
        case VAR_TAU: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(cmd, "Setting headway time requires a double.", outputStorage);
            }
            v.getCarFollowModel().setHeadwayTime(value);
        }
        break;
        case VAR_COLOR: {
            RGBColor col;
            if (!server.readTypeCheckingColor(inputStorage, col)) {
                return server.writeErrorStatusCmd(cmd, "The color must be given using the according type.", outputStorage);
            }
            v.setColor(col);
        }
        break;
        default:
            break;
    }
    return true;
}

#endif


/****************************************************************************/
