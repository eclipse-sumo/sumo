/****************************************************************************/
/// @file    TraCIServerAPI_VehicleType.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle type values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
TraCIServerAPI_VehicleType::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                       tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_LENGTH&&variable!=VAR_MAXSPEED&&variable!=VAR_ACCEL&&variable!=VAR_DECEL
            &&variable!=VAR_TAU&&variable!=VAR_VEHICLECLASS&&variable!=VAR_EMISSIONCLASS&&variable!=VAR_SHAPECLASS
            &&variable!=VAR_GUIOFFSET&&variable!=VAR_WIDTH&&variable!=VAR_COLOR) {
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
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getVehicleControl().insertVTypeIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSVehicleType *v = MSNet::getInstance()->getVehicleControl().getVType(id);
        if (v==0) {
            server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Vehicle type '" + id + "' is not known", outputStorage);
            return false;
        }
        getVariable(variable, *v, tempMsg);
    }
    server.writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + (int)tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}

bool
TraCIServerAPI_VehicleType::getVariable(const int variable, const MSVehicleType &v, tcpip::Storage &tempMsg) {
    switch (variable) {
    case VAR_LENGTH:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getLength()));
        break;
    case VAR_MAXSPEED:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getMaxSpeed()));
        break;
    case VAR_ACCEL:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getCarFollowModel().getMaxAccel()));
        break;
    case VAR_DECEL:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getCarFollowModel().getMaxDecel()));
        break;
    case VAR_IMPERFECTION:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getCarFollowModel().getImperfection()));
        break;
    case VAR_TAU:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getCarFollowModel().getTau()));
        break;
    case VAR_SPEED_FACTOR:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getSpeedFactor()));
        break;
    case VAR_SPEED_DEVIATION:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getSpeedDeviation()));
        break;
    case VAR_VEHICLECLASS:
        tempMsg.writeUnsignedByte(TYPE_STRING);
        tempMsg.writeString(getVehicleClassName(v.getVehicleClass()));
        break;
    case VAR_EMISSIONCLASS:
        tempMsg.writeUnsignedByte(TYPE_STRING);
        tempMsg.writeString(getVehicleEmissionTypeName(v.getEmissionClass()));
        break;
    case VAR_SHAPECLASS:
        tempMsg.writeUnsignedByte(TYPE_STRING);
        tempMsg.writeString(getVehicleShapeName(v.getGuiShape()));
        break;
    case VAR_GUIOFFSET:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getGuiOffset()));
        break;
    case VAR_WIDTH:
        tempMsg.writeUnsignedByte(TYPE_FLOAT);
        tempMsg.writeFloat((float)(v.getGuiWidth()));
        break;
    case VAR_COLOR:
        tempMsg.writeUnsignedByte(TYPE_COLOR);
        tempMsg.writeUnsignedByte((int)(v.getColor().red()*255.));
        tempMsg.writeUnsignedByte((int)(v.getColor().green()*255.));
        tempMsg.writeUnsignedByte((int)(v.getColor().blue()*255.));
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
    if (variable!=CMD_SETMAXSPEED
            &&variable!=VAR_LENGTH&&variable!=VAR_MAXSPEED&&variable!=VAR_VEHICLECLASS
            &&variable!=VAR_SPEED_FACTOR&&variable!=VAR_SPEED_DEVIATION&&variable!=VAR_EMISSIONCLASS
            &&variable!=VAR_WIDTH&&variable!=VAR_GUIOFFSET&&variable!=VAR_SHAPECLASS
            &&variable!=VAR_ACCEL&&variable!=VAR_DECEL&&variable!=VAR_IMPERFECTION
            &&variable!=VAR_TAU&&variable!=VAR_COLOR
       ) {
        server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Change Vehicle Type State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    MSVehicleType *v = MSNet::getInstance()->getVehicleControl().getVType(id);
    if (v==0) {
        server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Vehicle type '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    if (setVariable(CMD_SET_VEHICLETYPE_VARIABLE, variable, inputStorage.readUnsignedByte(),
                    *v, server, inputStorage, outputStorage)) {
        server.writeStatusCmd(CMD_SET_VEHICLETYPE_VARIABLE, RTYPE_OK, warning, outputStorage);
        return true;
    }
    return false;
}


bool
TraCIServerAPI_VehicleType::setVariable(const int cmd, const int variable, const int valueDataType,
                                        MSVehicleType &v, traci::TraCIServer &server,
                                        tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) {
    switch (variable) {
    case CMD_SETMAXSPEED: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The speed must be given as a float.", outputStorage);
            return false;
        }
        v.setMaxSpeed(inputStorage.readFloat());
    }
    break;
    case VAR_LENGTH: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting length requires a float.", outputStorage);
            return false;
        }
        v.setLength(inputStorage.readFloat());
    }
    break;
    case VAR_MAXSPEED: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting maximum speed requires a float.", outputStorage);
            return false;
        }
        v.setMaxSpeed(inputStorage.readFloat());
    }
    break;
    case VAR_VEHICLECLASS: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting vehicle class requires a string.", outputStorage);
            return false;
        }
        v.setVClass(getVehicleClassID(inputStorage.readString()));
    }
    break;
    case VAR_SPEED_FACTOR: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting speed factor requires a float.", outputStorage);
            return false;
        }
        v.setSpeedFactor(inputStorage.readFloat());
    }
    break;
    case VAR_SPEED_DEVIATION: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting speed deviation requires a float.", outputStorage);
            return false;
        }
        v.setSpeedDeviation(inputStorage.readFloat());
    }
    break;
    case VAR_EMISSIONCLASS: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting emission class requires a string.", outputStorage);
            return false;
        }
        v.setEmissionClass(getVehicleEmissionTypeID(inputStorage.readString()));
    }
    break;
    case VAR_WIDTH: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting width requires a float.", outputStorage);
            return false;
        }
        v.setWidth(inputStorage.readFloat());
    }
    break;
    case VAR_GUIOFFSET: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting gui offset requires a float.", outputStorage);
            return false;
        }
        v.setOffset(inputStorage.readFloat());
    }
    break;
    case VAR_SHAPECLASS: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting vehicle shape requires a string.", outputStorage);
            return false;
        }
        v.setShape(getVehicleShapeID(inputStorage.readString()));
    }
    break;
    case VAR_ACCEL: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting acceleration requires a float.", outputStorage);
            return false;
        }
        v.getCarFollowModel().setMaxAccel(inputStorage.readFloat());
    }
    break;
    case VAR_DECEL: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting deceleration requires a float.", outputStorage);
            return false;
        }
        v.getCarFollowModel().setMaxDecel(inputStorage.readFloat());
    }
    break;
    case VAR_IMPERFECTION: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting driver imperfection requires a float.", outputStorage);
            return false;
        }
        v.getCarFollowModel().setImperfection(inputStorage.readFloat());
    }
    break;
    case VAR_TAU: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting reaction time requires a float.", outputStorage);
            return false;
        }
        v.getCarFollowModel().setTau(inputStorage.readFloat());
    }
    break;
    default:
        break;
    }
    return true;
}

/****************************************************************************/
