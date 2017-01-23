/****************************************************************************/
/// @file    TraCIServerAPI_Person.cpp
/// @author  Daniel Krajzewicz
/// @date    26.05.2014
/// @version $Id$
///
// APIs for getting/setting person values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSTransportableControl.h>
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
    if (variable != ID_LIST && variable != ID_COUNT
            && variable != VAR_POSITION && variable != VAR_POSITION3D && variable != VAR_ANGLE && variable != VAR_SPEED
            && variable != VAR_ROAD_ID && variable != VAR_LANEPOSITION
            && variable != VAR_WIDTH && variable != VAR_LENGTH && variable != VAR_MINGAP
            && variable != VAR_TYPE && variable != VAR_SHAPECLASS && variable != VAR_COLOR
            && variable != VAR_WAITING_TIME && variable != VAR_PARAMETER
            && variable != VAR_NEXT_EDGE
            && variable != VAR_EDGES
            && variable != VAR_STAGE
            && variable != VAR_VEHICLE
       ) {
        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Get Person Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_PERSON_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    if (variable == ID_LIST || variable == ID_COUNT) {
        if (variable == ID_LIST) {
            std::vector<std::string> ids;
            for (MSTransportableControl::constVehIt i = c.loadedBegin(); i != c.loadedEnd(); ++i) {
                if (i->second->getCurrentStageType() != MSTransportable::WAITING_FOR_DEPART) {
                    ids.push_back(i->first);
                }
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        } else {
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) c.size());
        }
    } else {
        MSTransportable* p = c.get(id);
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
            case VAR_POSITION3D:
                tempMsg.writeUnsignedByte(POSITION_3D);
                tempMsg.writeDouble(p->getPosition().x());
                tempMsg.writeDouble(p->getPosition().y());
                tempMsg.writeDouble(p->getPosition().z());
                break;
            case VAR_ANGLE:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(GeomHelper::naviDegree(p->getAngle()));
                break;
            case VAR_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getSpeed());
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
            case VAR_WAITING_TIME:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getWaitingSeconds());
                break;
            case VAR_TYPE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getVehicleType().getID());
                break;
            case VAR_NEXT_EDGE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(dynamic_cast<MSPerson*>(p)->getNextEdge());
                break;
            case VAR_EDGES: {
                ConstMSEdgeVector edges = p->getEdges();
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeInt(edges.size());
                for (ConstMSEdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
                    tempMsg.writeString((*i)->getID());
                }
                break;
            }
            case VAR_STAGE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(p->getCurrentStageType());
                break;
            case VAR_VEHICLE: {
                const SUMOVehicle* veh = p->getVehicle();
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(veh == 0 ? "" : veh->getID());
                break;
            }
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getParameter().getParameter(paramName, ""));
            }
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
    if (variable != VAR_PARAMETER
       ) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Change Person State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    std::string id = inputStorage.readString();
    MSTransportable* p = c.get(id);
    if (p == 0) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
            }
            ((SUMOVehicleParameter&) p->getParameter()).addParameter(name, value);
        }
        break;
        default:
            /*
            try {
                if (!TraCIServerAPI_VehicleType::setVariable(CMD_SET_PERSON_VARIABLE, variable, getSingularType(v), server, inputStorage, outputStorage)) {
                    return false;
                }
            } catch (ProcessError& e) {
                return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
            }
            */
            break;
    }
    server.writeStatusCmd(CMD_SET_PERSON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Person::getPosition(const std::string& id, Position& p) {
    MSPerson* person = dynamic_cast<MSPerson*>(MSNet::getInstance()->getPersonControl().get(id));
    if (person == 0) {
        return false;
    }
    p = person->getPosition();
    return true;
}


/*
MSVehicleType&
TraCIServerAPI_Person::getSingularType(MSPerson* const person) {
    const MSVehicleType& oType = person->getVehicleType();
    std::string newID = oType.getID().find('@') == std::string::npos ? oType.getID() + "@" + person->getID() : oType.getID();
    MSVehicleType* type = MSVehicleType::build(newID, &oType);
    person->replaceVehicleType(type);
    return *type;
}
*/



#endif


/****************************************************************************/

