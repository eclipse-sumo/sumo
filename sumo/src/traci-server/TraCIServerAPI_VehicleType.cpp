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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
// used namespaces
// ===========================================================================
using namespace traci;


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
        server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Get Vehicle Type Variable: unsupported variable specified", outputStorage);
        return false;
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
            server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Vehicle type '" + id + "' is not known", outputStorage);
            return false;
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
            tempMsg.writeUnsignedByte(static_cast<int>(v.getColor().red() * 255. + 0.5));
            tempMsg.writeUnsignedByte(static_cast<int>(v.getColor().green() * 255. + 0.5));
            tempMsg.writeUnsignedByte(static_cast<int>(v.getColor().blue() * 255. + 0.5));
            tempMsg.writeUnsignedByte(255);
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
        server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Change Vehicle Type State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    MSVehicleType* v = MSNet::getInstance()->getVehicleControl().getVType(id);
    if (v == 0) {
        server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Vehicle type '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    try {
        if (setVariable(CMD_SET_VEHICLETYPE_VARIABLE, variable, inputStorage.readUnsignedByte(),
                        *v, server, inputStorage, outputStorage)) {
            server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_OK, warning, outputStorage);
            return true;
        }
    } catch (ProcessError& e) {
        server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_ERR, e.what(), outputStorage);
    }
    return false;
}


bool
TraCIServerAPI_VehicleType::setVariable(const int cmd, const int variable, const int valueDataType,
                                        MSVehicleType& v, traci::TraCIServer& server,
                                        tcpip::Storage& inputStorage, tcpip::Storage& outputStorage) {
    switch (variable) {
        case VAR_LENGTH: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting length requires a double.", outputStorage);
                return false;
            }
            double val = inputStorage.readDouble();
            if (val == 0.0 || fabs(val) == std::numeric_limits<double>::infinity()) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Invalid length.", outputStorage);
                return false;
            }
            v.setLength(val);
        }
        break;
        case VAR_MAXSPEED: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting maximum speed requires a double.", outputStorage);
                return false;
            }
            double val = inputStorage.readDouble();
            if (val == 0.0 || fabs(val) == std::numeric_limits<double>::infinity()) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Invalid maximum speed.", outputStorage);
                return false;
            }
            v.setMaxSpeed(val);
        }
        break;
        case VAR_VEHICLECLASS: {
            if (valueDataType != TYPE_STRING) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting vehicle class requires a string.", outputStorage);
                return false;
            }
            v.setVClass(getVehicleClassID(inputStorage.readString()));
        }
        break;
        case VAR_SPEED_FACTOR: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting speed factor requires a double.", outputStorage);
                return false;
            }
            v.setSpeedFactor(inputStorage.readDouble());
        }
        break;
        case VAR_SPEED_DEVIATION: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting speed deviation requires a double.", outputStorage);
                return false;
            }
            v.setSpeedDeviation(inputStorage.readDouble());
        }
        break;
        case VAR_EMISSIONCLASS: {
            if (valueDataType != TYPE_STRING) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting emission class requires a string.", outputStorage);
                return false;
            }
            v.setEmissionClass(getVehicleEmissionTypeID(inputStorage.readString()));
        }
        break;
        case VAR_WIDTH: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting width requires a double.", outputStorage);
                return false;
            }
            v.setWidth(inputStorage.readDouble());
        }
        break;
        case VAR_MINGAP: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting minimum gap requires a double.", outputStorage);
                return false;
            }
            v.setMinGap(inputStorage.readDouble());
        }
        break;
        case VAR_SHAPECLASS: {
            if (valueDataType != TYPE_STRING) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting vehicle shape requires a string.", outputStorage);
                return false;
            }
            v.setShape(getVehicleShapeID(inputStorage.readString()));
        }
        break;
        case VAR_ACCEL: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting acceleration requires a double.", outputStorage);
                return false;
            }
            v.getCarFollowModel().setMaxAccel(inputStorage.readDouble());
        }
        break;
        case VAR_DECEL: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting deceleration requires a double.", outputStorage);
                return false;
            }
            v.getCarFollowModel().setMaxDecel(inputStorage.readDouble());
        }
        break;
        case VAR_IMPERFECTION: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting driver imperfection requires a double.", outputStorage);
                return false;
            }
            v.getCarFollowModel().setImperfection(inputStorage.readDouble());
        }
        break;
        case VAR_TAU: {
            if (valueDataType != TYPE_DOUBLE) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "Setting headway time requires a double.", outputStorage);
                return false;
            }
            v.getCarFollowModel().setHeadwayTime(inputStorage.readDouble());
        }
        break;
        case VAR_COLOR: {
            if (valueDataType != TYPE_COLOR) {
                server.writeStatusCmd(cmd, RTYPE_ERR, "The color must be given using the according type.", outputStorage);
                return false;
            }
            SUMOReal r = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            SUMOReal g = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            SUMOReal b = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
            inputStorage.readUnsignedByte(); // skip alpha level
            RGBColor col(r, g, b);
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
