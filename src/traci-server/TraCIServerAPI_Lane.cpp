/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Lane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Mario Krumnow
/// @author  Leonhard Luecken
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting lane values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSTransportable.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Lane.h"
#include <libsumo/Lane.h>


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Lane::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != LANE_LINK_NUMBER && variable != LANE_EDGE_ID && variable != VAR_LENGTH
            && variable != VAR_MAXSPEED && variable != LANE_LINKS && variable != VAR_SHAPE
            && variable != VAR_CO2EMISSION && variable != VAR_COEMISSION && variable != VAR_HCEMISSION && variable != VAR_PMXEMISSION
            && variable != VAR_NOXEMISSION && variable != VAR_FUELCONSUMPTION && variable != VAR_NOISEEMISSION
            && variable != VAR_ELECTRICITYCONSUMPTION && variable != VAR_WAITING_TIME
            && variable != LAST_STEP_MEAN_SPEED && variable != LAST_STEP_VEHICLE_NUMBER
            && variable != LAST_STEP_VEHICLE_ID_LIST && variable != LAST_STEP_OCCUPANCY && variable != LAST_STEP_VEHICLE_HALTING_NUMBER
            && variable != LAST_STEP_LENGTH && variable != VAR_CURRENT_TRAVELTIME
            && variable != LANE_ALLOWED && variable != LANE_DISALLOWED && variable != VAR_FOES
            && variable != VAR_WIDTH && variable != ID_COUNT && variable != VAR_PARAMETER
       ) {
        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Get Lane Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_LANE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable == ID_LIST) {
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(libsumo::Lane::getIDList());
    } else if (variable == ID_COUNT) {
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt(libsumo::Lane::getIDCount());
    } else {
        try {
            switch (variable) {
                case LANE_LINK_NUMBER:
                    tempMsg.writeUnsignedByte(TYPE_UBYTE);
                    tempMsg.writeUnsignedByte(libsumo::Lane::getLinkNumber(id));
                    break;
                case LANE_EDGE_ID:
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(libsumo::Lane::getEdgeID(id));
                    break;
                case VAR_LENGTH:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getLength(id));
                    break;
                case VAR_MAXSPEED:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getMaxSpeed(id));
                    break;
                case LANE_LINKS: {
                    tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                    const std::vector<libsumo::TraCIConnection> links = libsumo::Lane::getLinks(id);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt((int) links.size());
                    ++cnt;
                    for (std::vector<libsumo::TraCIConnection>::const_iterator i = links.begin(); i != links.end(); ++i) {
                        // approached non-internal lane (if any)
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->approachedLane);
                        ++cnt;
                        // approached "via", internal lane (if any)
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->approachedInternal);
                        ++cnt;
                        // priority
                        tempContent.writeUnsignedByte(TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->hasPrio);
                        ++cnt;
                        // opened
                        tempContent.writeUnsignedByte(TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->isOpen);
                        ++cnt;
                        // approaching foe
                        tempContent.writeUnsignedByte(TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->hasFoe);
                        ++cnt;
                        // state (not implemented, yet)
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->state);
                        ++cnt;
                        // direction
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->direction);
                        ++cnt;
                        // length
                        tempContent.writeUnsignedByte(TYPE_DOUBLE);
                        tempContent.writeDouble(i->length);
                        ++cnt;
                    }
                    tempMsg.writeInt(cnt);
                    tempMsg.writeStorage(tempContent);
                }
                break;
                case LANE_ALLOWED: {
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    tempMsg.writeStringList(libsumo::Lane::getAllowed(id));
                }
                break;
                case LANE_DISALLOWED: {
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    tempMsg.writeStringList(libsumo::Lane::getDisallowed(id));
                }
                break;
                case VAR_FOES: {
                    std::string toLane;
                    if (!server.readTypeCheckingString(inputStorage, toLane)) {
                        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "foe retrieval requires a string.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    if (toLane == "") {
                        tempMsg.writeStringList(libsumo::Lane::getInternalFoes(id));
                    } else {
                        tempMsg.writeStringList(libsumo::Lane::getFoes(id, toLane));
                    }
                }
                break;
                case VAR_SHAPE: {
                    tempMsg.writeUnsignedByte(TYPE_POLYGON);
                    libsumo::TraCIPositionVector shp = libsumo::Lane::getShape(id);
                    tempMsg.writeUnsignedByte(MIN2(255, (int) shp.size()));
                    for (int iPoint = 0; iPoint < MIN2(255, (int) shp.size()); ++iPoint) {
                        tempMsg.writeDouble(shp[iPoint].x);
                        tempMsg.writeDouble(shp[iPoint].y);
                    }
                }
                break;
                case VAR_CO2EMISSION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getCO2Emission(id));
                    break;
                case VAR_COEMISSION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getCOEmission(id));
                    break;
                case VAR_HCEMISSION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getHCEmission(id));
                    break;
                case VAR_PMXEMISSION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getPMxEmission(id));
                    break;
                case VAR_NOXEMISSION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getNOxEmission(id));
                    break;
                case VAR_FUELCONSUMPTION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getFuelConsumption(id));
                    break;
                case VAR_NOISEEMISSION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getNoiseEmission(id));
                    break;
                case VAR_ELECTRICITYCONSUMPTION:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getElectricityConsumption(id));
                    break;
                case LAST_STEP_VEHICLE_NUMBER:
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(libsumo::Lane::getLastStepVehicleNumber(id));
                    break;
                case LAST_STEP_MEAN_SPEED:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getLastStepMeanSpeed(id));
                    break;
                case LAST_STEP_VEHICLE_ID_LIST: {
                    tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                    tempMsg.writeStringList(libsumo::Lane::getLastStepVehicleIDs(id));
                }
                break;
                case LAST_STEP_OCCUPANCY:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getLastStepOccupancy(id));
                    break;
                case LAST_STEP_VEHICLE_HALTING_NUMBER: {
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(libsumo::Lane::getLastStepHaltingNumber(id));
                }
                break;
                case LAST_STEP_LENGTH: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getLastStepLength(id));
                }
                break;
                case VAR_WAITING_TIME: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getWaitingTime(id));
                }
                break;
                case VAR_CURRENT_TRAVELTIME: {
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getTraveltime(id));
                }
                break;
                case VAR_WIDTH:
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(libsumo::Lane::getWidth(id));
                    break;
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(libsumo::Lane::getParameter(id, paramName));
                }
                break;
                default:
                    break;
            }
        } catch (libsumo::TraCIException& e) {
            return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, e.what(), outputStorage);
        }
    }
    server.writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Lane::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_MAXSPEED && variable != VAR_LENGTH && variable != LANE_ALLOWED && variable != LANE_DISALLOWED
            && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Change Lane State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    MSLane* l = MSLane::dictionary(id);
    if (l == 0) {
        return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Lane '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        case VAR_MAXSPEED: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The speed must be given as a double.", outputStorage);
            }
            libsumo::Lane::setMaxSpeed(id, value);
        }
        break;
        case VAR_LENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The length must be given as a double.", outputStorage);
            }
            libsumo::Lane::setLength(id, value);
        }
        break;
        case LANE_ALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Allowed classes must be given as a list of strings.", outputStorage);
            }
            libsumo::Lane::setAllowed(id, classes);
        }
        break;
        case LANE_DISALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Not allowed classes must be given as a list of strings.", outputStorage);
            }
            libsumo::Lane::setDisallowed(id, classes);
        }
        break;
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
            }
            libsumo::Lane::setParameter(id, name, value);
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Lane::getShape(const std::string& id, PositionVector& shape) {
    const MSLane* const l = MSLane::dictionary(id);
    if (l == 0) {
        return false;
    }
    shape = l->getShape();
    return true;
}


/****************************************************************************/

