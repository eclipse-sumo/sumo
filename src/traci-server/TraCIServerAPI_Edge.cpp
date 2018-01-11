/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Edge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Jerome Haerri
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Mario Krumnow
/// @author  Gregor Laemmel
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting edge values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/pedestrians/MSPerson.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Edge.h"
#include <microsim/MSEdgeWeightsStorage.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <libsumo/Edge.h>


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Edge::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_EDGE_TRAVELTIME && variable != VAR_EDGE_EFFORT
            && variable != VAR_CURRENT_TRAVELTIME
            && variable != VAR_CO2EMISSION && variable != VAR_COEMISSION && variable != VAR_HCEMISSION
            && variable != VAR_PMXEMISSION
            && variable != VAR_NOXEMISSION && variable != VAR_FUELCONSUMPTION && variable != VAR_NOISEEMISSION
            && variable != VAR_ELECTRICITYCONSUMPTION && variable != VAR_WAITING_TIME
            && variable != LAST_STEP_VEHICLE_NUMBER && variable != LAST_STEP_MEAN_SPEED
            && variable != LAST_STEP_OCCUPANCY
            && variable != LAST_STEP_VEHICLE_HALTING_NUMBER && variable != LAST_STEP_LENGTH
            && variable != LAST_STEP_PERSON_ID_LIST
            && variable != LAST_STEP_VEHICLE_ID_LIST 
            && variable != VAR_LANE_INDEX
            && variable != ID_COUNT 
            && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                          "Get Edge Variable: unsupported variable " + toHex(variable, 2)
                                          + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_EDGE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request

    try {
        if (variable == ID_LIST) {
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(libsumo::Edge::getIDList());
        } else if (variable == ID_COUNT) {
            ;
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(libsumo::Edge::getIDCount());
        } else {
            switch (variable) {
                case VAR_EDGE_TRAVELTIME: {
                    // time
                    int time = 0;
                    if (!server.readTypeCheckingInt(inputStorage, time)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "The message must contain the time definition.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::retrieveExistingTravelTime(id, time));
                }
                break;
                case VAR_EDGE_EFFORT: {
                    // time
                    int time = 0;
                    if (!server.readTypeCheckingInt(inputStorage, time)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "The message must contain the time definition.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::retrieveExistingEffort(id, time));
                }
                break;
                case VAR_CURRENT_TRAVELTIME:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getCurrentTravelTime(id));
                    break;
                case VAR_WAITING_TIME: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getWaitingSeconds(id));
                }
                break;
                case LAST_STEP_PERSON_ID_LIST: {
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    tempMsg.writeStringList(libsumo::Edge::getPersonIDs(id));
                }
                break;
                case LAST_STEP_VEHICLE_ID_LIST: {
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    tempMsg.writeStringList(libsumo::Edge::getVehicleIDs(id));
                }
                break;
                case VAR_CO2EMISSION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getCO2Emissions(id));
                }
                break;
                case VAR_COEMISSION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getCOEmissions(id));
                }
                break;
                case VAR_HCEMISSION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getHCEmissions(id));
                }
                break;
                case VAR_PMXEMISSION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getPMxEmissions(id));
                }
                break;
                case VAR_NOXEMISSION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getNOxEmissions(id));
                }
                break;
                case VAR_FUELCONSUMPTION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getFuelConsumption(id));
                }
                break;
                case VAR_NOISEEMISSION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getNoiseEmissions(id));
                }
                break;
                case VAR_ELECTRICITYCONSUMPTION: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getElectricityConsumption(id));
                }
                break;
                case LAST_STEP_VEHICLE_NUMBER: {
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(libsumo::Edge::getVehicleNumber(id));
                }
                break;
                case LAST_STEP_MEAN_SPEED: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getMeanSpeed(id));
                }
                break;
                case LAST_STEP_OCCUPANCY: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getOccupancy(id));

                }
                break;
                case LAST_STEP_VEHICLE_HALTING_NUMBER: {
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(libsumo::Edge::getVehicleHaltingNumber(id));
                }
                break;
                case LAST_STEP_LENGTH: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Edge::getVehicleAverageLength(id));
                }
                break;
                case VAR_LANE_INDEX: {
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(libsumo::Edge::getLaneNumber(id));
                }
                break;
                case VAR_PARAMETER: {
                    std::string paramName;
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "Retrieval of a parameter requires its name.",
                                                          outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(libsumo::Edge::getParameter(id, paramName));
                }
                break;
                default:
                    break;
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Edge::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    std::string warning; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_EDGE_TRAVELTIME && variable != VAR_EDGE_EFFORT && variable != VAR_MAXSPEED
            && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                          "Change Edge State: unsupported variable " + toHex(variable, 2)
                                          + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    try {
        // process
        switch (variable) {

            case LANE_ALLOWED: {
                // read and set allowed vehicle classes
                std::vector<std::string> classes;
                if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                    return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                      "Allowed vehicle classes must be given as a list of strings.",
                                                      outputStorage);
                }
                libsumo::Edge::setAllowedVehicleClasses(id, classes);
            }
            break;
            case LANE_DISALLOWED: {
                // read and set disallowed vehicle classes
                std::vector<std::string> classes;
                if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                    return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                      "Not allowed vehicle classes must be given as a list of strings.",
                                                      outputStorage);
                }
                libsumo::Edge::setDisallowedVehicleClasses(id, classes);
            }
            break;
            case VAR_EDGE_TRAVELTIME: {
                // read and set travel time
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE,
                                                      "Setting travel time requires a compound object.", outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                if (parameterCount == 3) {
                    // bound by time
                    int begTime = 0, endTime = 0;
                    double value = 0;
                    if (!server.readTypeCheckingInt(inputStorage, begTime)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "The first variable must be the begin time given as int.",
                                                          outputStorage);
                    }
                    if (!server.readTypeCheckingInt(inputStorage, endTime)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "The second variable must be the end time given as int.",
                                                          outputStorage);
                    }
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                          "The third variable must be the value given as double",
                                                          outputStorage);
                    }
                    libsumo::Edge::addTravelTime(id, begTime, endTime, value);
                } else if (parameterCount == 1) {
                    // unbound
                    double value = 0;
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                          "The variable must be the value given as double", outputStorage);
                    }
                    libsumo::Edge::addTravelTime(id, 0, double(SUMOTime_MAX), value);
                } else {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE,
                                                      "Setting travel time requires either begin time, end time, and value, or only value as parameter.",
                                                      outputStorage);
                }
            }
            break;
            case VAR_EDGE_EFFORT: {
                // read and set effort
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE,
                                                      "Setting effort requires a compound object.",
                                                      outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                if (parameterCount == 3) {
                    // bound by time
                    int begTime = 0, endTime = 0;
                    double value = 0;
                    if (!server.readTypeCheckingInt(inputStorage, begTime)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "The first variable must be the begin time given as int.",
                                                          outputStorage);
                    }
                    if (!server.readTypeCheckingInt(inputStorage, endTime)) {
                        return server.writeErrorStatusCmd(CMD_GET_EDGE_VARIABLE,
                                                          "The second variable must be the end time given as int.",
                                                          outputStorage);
                    }
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                          "The third variable must be the value given as double",
                                                          outputStorage);
                    }
                    libsumo::Edge::addEffort(id, begTime, endTime, value);
                } else if (parameterCount == 1) {
                    // unbound
                    double value = 0;
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                          "The variable must be the value given as double", outputStorage);
                    }
                    libsumo::Edge::addEffort(id, 0., double(SUMOTime_MAX), value);
                } else {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE,
                                                      "Setting effort requires either begin time, end time, and value, or only value as parameter.",
                                                      outputStorage);
                }
            }
            break;
            case VAR_MAXSPEED: {
                // read and set max. speed
                double value = 0;
                if (!server.readTypeCheckingDouble(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE, "The speed must be given as a double.",
                                                      outputStorage);
                }
                libsumo::Edge::setMaxSpeed(id, value);
            }
            break;
            case VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                      "A compound object is needed for setting a parameter.",
                                                      outputStorage);
                }
                //readt itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                      "The name of the parameter must be given as a string.",
                                                      outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE,
                                                      "The value of the parameter must be given as a string.",
                                                      outputStorage);
                }
                libsumo::Edge::setParameter(id, name, value);

            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_EDGE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Edge::getShape(const std::string& id, PositionVector& shape) {
    try {
        libsumo::Edge::getShape(id, shape);
    } catch (libsumo::TraCIException& e) {
        WRITE_WARNING(e.what());
        return false;
    }
    return true;

}


/****************************************************************************/

