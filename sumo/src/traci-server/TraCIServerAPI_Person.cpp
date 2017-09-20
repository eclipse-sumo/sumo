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

#include <utils/common/StringTokenizer.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Person.h"
#include "lib/TraCI_Person.h"
#include "TraCIServerAPI_VehicleType.h"


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
            && variable != VAR_STAGES_REMAINING
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

    try {
        if (variable == ID_LIST || variable == ID_COUNT) {
            if (variable == ID_LIST) {
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(TraCI_Person::getIDList());
            } else {
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_Person::getIDCount());
            }
        } else {
            switch (variable) {
                case VAR_POSITION: {
                    TraCIPosition pos = TraCI_Person::getPosition(id);
                    tempMsg.writeUnsignedByte(POSITION_2D);
                    tempMsg.writeDouble(pos.x);
                    tempMsg.writeDouble(pos.y);
                }
                break;
                case VAR_POSITION3D: {
                    TraCIPosition pos = TraCI_Person::getPosition(id);
                    tempMsg.writeUnsignedByte(POSITION_3D);
                    tempMsg.writeDouble(pos.x);
                    tempMsg.writeDouble(pos.y);
                    tempMsg.writeDouble(pos.z);
                }
                break;
                case VAR_ANGLE:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(TraCI_Person::getAngle(id));
                    break;
                case VAR_SPEED:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(TraCI_Person::getSpeed(id));
                    break;
                case VAR_ROAD_ID:
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Person::getRoadID(id));
                    break;
                case VAR_LANEPOSITION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(TraCI_Person::getLanePosition(id));
                    break;
                case VAR_COLOR: {
                    TraCIColor col = TraCI_Person::getColor(id);
                    tempMsg.writeUnsignedByte(TYPE_COLOR);
                    tempMsg.writeUnsignedByte(col.r);
                    tempMsg.writeUnsignedByte(col.g);
                    tempMsg.writeUnsignedByte(col.b);
                    tempMsg.writeUnsignedByte(col.a);
                }
                break;
                case VAR_WAITING_TIME:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(TraCI_Person::getWaitingTime(id));
                    break;
                case VAR_TYPE:
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Person::getTypeID(id));
                    break;
                case VAR_NEXT_EDGE:
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Person::getNextEdge(id));
                    break;
                case VAR_EDGES: {
                    int nextStageIndex = 0;
                    if (!server.readTypeCheckingInt(inputStorage, nextStageIndex)) {
                        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The message must contain the stage index.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    tempMsg.writeStringList(TraCI_Person::getEdges(id, nextStageIndex));
                    break;
                }
                case VAR_STAGE: {
                    int nextStageIndex = 0;
                    if (!server.readTypeCheckingInt(inputStorage, nextStageIndex)) {
                        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The message must contain the stage index.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(TraCI_Person::getStage(id, nextStageIndex));
                    break;
                }
                case VAR_STAGES_REMAINING:
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(TraCI_Person::getRemainingStages(id));
                    break;
                case VAR_VEHICLE: {
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Person::getVehicle(id));
                    break;
                }
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Person::getParameter(id, paramName));
                    break;
                }
                default:
                    TraCIServerAPI_VehicleType::getVariable(variable, TraCI_Person::getTypeID(id), tempMsg);
                    break;
            }
        }
    } catch (TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, e.what(), outputStorage);
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
            && variable != ADD
            && variable != APPEND_STAGE
            && variable != REMOVE_STAGE
            && variable != CMD_REROUTE_TRAVELTIME
            && variable != VAR_SPEED
            && variable != VAR_TYPE
            && variable != VAR_LENGTH
            && variable != VAR_WIDTH
            && variable != VAR_HEIGHT
            && variable != VAR_MINGAP
            && variable != VAR_COLOR
       ) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Change Person State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }

    try {
    // TODO: remove declaration of c after completion
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    // id
    std::string id = inputStorage.readString();
    // TODO: remove declaration of p after completion
    const bool shouldExist = variable != ADD;
    MSTransportable* p = c.get(id);
    if (p == 0 && shouldExist) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        case VAR_SPEED: {
            double speed = 0;
            if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Setting speed requires a double.", outputStorage);
            }
            // set the speed for all (walking) stages
            TraCI_Person::setSpeed(id,speed);
            // modify the vType so that stages added later are also affected
            TraCIServerAPI_VehicleType::setVariable(CMD_SET_VEHICLE_VARIABLE, variable, TraCI_Person::getSingularVType(id), server, inputStorage, outputStorage);
        }
        break;
        case VAR_TYPE: {
            std::string vTypeID;
            if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The vehicle type id must be given as a string.", outputStorage);
            }
            TraCI_Person::setType(id,vTypeID);
            break;
        }
        case ADD: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a person requires a compound object.", outputStorage);
            }
            if (inputStorage.readInt() != 4) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a person needs four parameters.", outputStorage);
            }
            std::string vTypeID;
            if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "First parameter (type) requires a string.", outputStorage);
            }
            std::string edgeID;
            if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Second parameter (edge) requires a string.", outputStorage);
            }
            int depart;
            if (!server.readTypeCheckingInt(inputStorage, depart)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (depart) requires an integer.", outputStorage);
            }
            double pos;
            if (!server.readTypeCheckingDouble(inputStorage, pos)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (position) requires a double.", outputStorage);
            }
            TraCI_Person::add(id, edgeID, pos, STEPS2TIME(depart), vTypeID);
        }
        break;
        case APPEND_STAGE: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a person stage requires a compound object.", outputStorage);
            }
            int numParameters = inputStorage.readInt();
            int stageType;
            if (!server.readTypeCheckingInt(inputStorage, stageType)) {
                return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first parameter for adding a stage must be the stage type given as int.", outputStorage);
            }
            if (stageType == MSTransportable::DRIVING) {
                // append driving stage
                if (numParameters != 4) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a driving stage needs four parameters.", outputStorage);
                }
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Second parameter (edge) requires a string.", outputStorage);
                }
                std::string lines;
                if (!server.readTypeCheckingString(inputStorage, lines)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (lines) requires a string.", outputStorage);
                }
                std::string stopID;
                if (!server.readTypeCheckingString(inputStorage, stopID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (stopID) requires a string.", outputStorage);
                }
                TraCI_Person::appendDrivingStage(id,edgeID,lines,stopID);
            } else if (stageType == MSTransportable::WAITING) {
                // append waiting stage
                if (numParameters != 4) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a waiting stage needs four parameters.", outputStorage);
                }
                int duration;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Second parameter (duration) requires an int.", outputStorage);
                }
                std::string description;
                if (!server.readTypeCheckingString(inputStorage, description)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (description) requires a string.", outputStorage);
                }
                std::string stopID;
                if (!server.readTypeCheckingString(inputStorage, stopID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (stopID) requires a string.", outputStorage);
                }
                TraCI_Person::appendWaitingStage(id, STEPS2TIME(duration), description, stopID);
            } else if (stageType == MSTransportable::MOVING_WITHOUT_VEHICLE) {
                // append walking stage
                if (numParameters != 6) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a walking stage needs six parameters.", outputStorage);
                }
                std::vector<std::string> edgeIDs;
                if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Second parameter (edges) route must be defined as a list of edge ids.", outputStorage);
                }
                double arrivalPos;
                if (!server.readTypeCheckingDouble(inputStorage, arrivalPos)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (arrivalPos) requires a double.", outputStorage);
                }
                int duration;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (duration) requires an int.", outputStorage);
                }
                double speed;
                if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fifth parameter (speed) requires a double.", outputStorage);
                }
                std::string stopID;
                if (!server.readTypeCheckingString(inputStorage, stopID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (stopID) requires a string.", outputStorage);
                }
                TraCI_Person::appendWalkingStage(id,edgeIDs,arrivalPos, STEPS2TIME(duration), speed,stopID);
            } else {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid stage type for person '" + id + "'", outputStorage);
            }
        }
        break;
        case REMOVE_STAGE: {
            int nextStageIndex = 0;
            if (!server.readTypeCheckingInt(inputStorage, nextStageIndex)) {
                return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The message must contain the stage index.", outputStorage);
            }
            TraCI_Person::removeStage(id, nextStageIndex);
        }
        break;
        case CMD_REROUTE_TRAVELTIME: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Rerouting requires a compound object.", outputStorage);
            }
            if (inputStorage.readInt() != 0) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Rerouting should obtain an empty compound object.", outputStorage);
            }
            TraCI_Person::rerouteTraveltime(id);
        }
        break;
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
            }
            //read itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
            }
            TraCI_Person::setParameter(id, name, value);
        }
        break;
        default:
            try {
                if (!TraCIServerAPI_VehicleType::setVariable(CMD_SET_PERSON_VARIABLE, variable, TraCI_Person::getSingularVType(id), server, inputStorage, outputStorage)) {
                    return false;
                }
            } catch (ProcessError& e) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
            }
            break;
    }
    } catch (TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
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


#endif


/****************************************************************************/

