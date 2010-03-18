/****************************************************************************/
/// @file    TraCIServerAPI_VehicleType.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle type values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_VehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace traci;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_VehicleType::processGet(tcpip::Storage &inputStorage,
                                       tcpip::Storage &outputStorage,
                                       bool withStatus) throw(TraCIException) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_LENGTH&&variable!=VAR_MAXSPEED&&variable!=VAR_ACCEL&&variable!=VAR_DECEL
            &&variable!=VAR_TAU&&variable!=VAR_VEHICLECLASS&&variable!=VAR_EMISSIONCLASS&&variable!=VAR_SHAPECLASS
            &&variable!=VAR_GUIOFFSET&&variable!=VAR_WIDTH&&variable!=VAR_COLOR) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
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
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_ERR, "Vehicle type '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_LENGTH:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getLength());
            break;
        case VAR_MAXSPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getMaxSpeed());
            break;
        case VAR_ACCEL:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getMaxAccel(0));
            break;
        case VAR_DECEL:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getMaxDecel());
            break;
        case VAR_TAU:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getTau());
            break;
        case VAR_VEHICLECLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleClassName(v->getVehicleClass()));
            break;
        case VAR_EMISSIONCLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleEmissionTypeName(v->getEmissionClass()));
            break;
        case VAR_SHAPECLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleShapeName(v->getGuiShape()));
            break;
        case VAR_GUIOFFSET:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getGuiOffset());
            break;
        case VAR_WIDTH:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getGuiWidth());
            break;
        case VAR_COLOR:
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            tempMsg.writeUnsignedByte((int)(v->getColor().red()*255.));
            tempMsg.writeUnsignedByte((int)(v->getColor().green()*255.));
            tempMsg.writeUnsignedByte((int)(v->getColor().blue()*255.));
            tempMsg.writeUnsignedByte(255);
            break;
        default:
            break;
        }
    }
    if (withStatus) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLETYPE_VARIABLE, RTYPE_OK, warning, outputStorage);
    }
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_VehicleType::processSet(tcpip::Storage &inputStorage,
                                       tcpip::Storage &outputStorage) throw(TraCIException) {
    return true;
}



/****************************************************************************/

