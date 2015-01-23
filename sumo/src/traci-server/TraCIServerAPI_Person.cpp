/****************************************************************************/
/// @file    TraCIServerAPI_Person.cpp
/// @author  Daniel Krajzewicz
/// @date    26.05.2014
/// @version $Id$
///
// APIs for getting/setting person values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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

#include <microsim/MSPersonControl.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Person.h"
#include "TraCIServerAPI_VehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Person::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_POSITION && variable != VAR_ANGLE
            && variable != VAR_ROAD_ID && variable != VAR_COLOR && variable != VAR_LANEPOSITION
            && variable != VAR_WIDTH && variable != VAR_MINGAP && variable != VAR_SHAPECLASS
            && variable != VAR_ACCEL && variable != VAR_DECEL && variable != VAR_IMPERFECTION
            && variable != VAR_TAU && variable != VAR_BEST_LANES && variable != DISTANCE_REQUEST
            && variable != ID_COUNT && variable != VAR_STOPSTATE && variable !=  VAR_WAITING_TIME
       ) {
        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Get Person Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_PERSON_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    MSPersonControl& c = MSNet::getInstance()->getPersonControl();
    if (variable == ID_LIST || variable == ID_COUNT) {
        if (variable == ID_LIST) {
            std::vector<std::string> ids;
            for (MSPersonControl::constVehIt i = c.loadedPersonsBegin(); i != c.loadedPersonsEnd(); ++i) {
                ids.push_back((*i).first);
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        } else {
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) c.size());
        }
    } else {
        MSPerson* p = c.get(id);
        if (p == 0) {
            return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case VAR_POSITION: {
                tempMsg.writeUnsignedByte(POSITION_2D);
                tempMsg.writeDouble(p->getPosition().x());
                tempMsg.writeDouble(p->getPosition().y());
                }
                break;
            case VAR_ANGLE:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getAngle());
                break;
            case VAR_ROAD_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getEdge()->getID());
                break;
            case VAR_LANEPOSITION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getEdgePos());
                break;
            case VAR_COLOR:
                tempMsg.writeUnsignedByte(TYPE_COLOR);
                tempMsg.writeUnsignedByte(p->getParameter().color.red());
                tempMsg.writeUnsignedByte(p->getParameter().color.green());
                tempMsg.writeUnsignedByte(p->getParameter().color.blue());
                tempMsg.writeUnsignedByte(p->getParameter().color.alpha());
                break;
            default:
                TraCIServerAPI_VehicleType::getVariable(variable, p->getVehicleType(), tempMsg);
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_PERSON_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Person::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (true) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Change Person State: unsupported variable specified", outputStorage);
    }
    // id
    MSPersonControl& c = MSNet::getInstance()->getPersonControl();
    std::string id = inputStorage.readString();
    MSPerson* p = c.get(id);
    if (p == 0) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_PERSON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


#endif


/****************************************************************************/

