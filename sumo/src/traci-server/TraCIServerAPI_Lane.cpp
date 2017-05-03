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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Lane.h"
#include "lib/TraCI_Lane.h"


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
            && variable != LANE_ALLOWED && variable != LANE_DISALLOWED && variable != VAR_WIDTH && variable != ID_COUNT
            && variable != VAR_PARAMETER
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
        tempMsg.writeStringList(TraCI_Lane::getIDList());
    } else if (variable == ID_COUNT) {
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt(TraCI_Lane::getIDCount());
    } else {
        MSLane* lane = MSLane::dictionary(id);
        if (lane == 0) {
            return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Lane '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case LANE_LINK_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_UBYTE);
                tempMsg.writeUnsignedByte(TraCI_Lane::getLinkNumber(id));
                break;
            case LANE_EDGE_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(TraCI_Lane::getEdgeID(id));
                break;
            case VAR_LENGTH:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getLength(id));
                break;
            case VAR_MAXSPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getMaxSpeed(id));
                break;
            case LANE_LINKS: {
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                const std::vector<TraCIConnection> links = TraCI_Lane::getLinks(id);
                tcpip::Storage tempContent;
                int cnt = 0;
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                tempContent.writeInt((int) links.size());
                ++cnt;
                for (std::vector<TraCIConnection>::const_iterator i = links.begin(); i != links.end(); ++i) {
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
                tempMsg.writeStringList(TraCI_Lane::getAllowed(id));
            }
            break;
            case LANE_DISALLOWED: {
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(TraCI_Lane::getDisallowed(id));
            }
            break;
            case VAR_SHAPE: {
                tempMsg.writeUnsignedByte(TYPE_POLYGON);
                TraCIPositionVector shp = TraCI_Lane::getShape(id);
                tempMsg.writeUnsignedByte(MIN2(255, (int) shp.size()));
                for (int iPoint = 0; iPoint < MIN2(255, (int) shp.size()); ++iPoint) {
                    tempMsg.writeDouble(shp[iPoint].x);
                    tempMsg.writeDouble(shp[iPoint].y);
                }
            }
            break;
            case VAR_CO2EMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getCO2Emission(id));
                break;
            case VAR_COEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getCOEmission(id));
                break;
            case VAR_HCEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getHCEmission(id));
                break;
            case VAR_PMXEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getPMxEmission(id));
                break;
            case VAR_NOXEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getNOxEmission(id));
                break;
            case VAR_FUELCONSUMPTION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getFuelConsumption(id));
                break;
            case VAR_NOISEEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getNoiseEmission(id));
                break;
            case VAR_ELECTRICITYCONSUMPTION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getElectricityConsumption(id));
                break;
            case LAST_STEP_VEHICLE_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_Lane::getLastStepVehicleNumber(id));
                break;
            case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getLastStepMeanSpeed(id));
                break;
            case LAST_STEP_VEHICLE_ID_LIST: {
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(TraCI_Lane::getLastStepVehicleIDs(id));
            }
            break;
            case LAST_STEP_OCCUPANCY:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getLastStepOccupancy(id));
                break;
            case LAST_STEP_VEHICLE_HALTING_NUMBER: {
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_Lane::getLastStepHaltingNumber(id));
            }
            break;
            case LAST_STEP_LENGTH: {
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getLastStepLength(id));
            }
            break;
            case VAR_WAITING_TIME: {
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getWaitingTime(id));
            }
            break;
            case VAR_CURRENT_TRAVELTIME: {
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getTraveltime(id));
            }
            break;
            case VAR_WIDTH:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_Lane::getWidth(id));
                break;
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(RESPONSE_GET_LANE_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(TraCI_Lane::getParameter(id, paramName));
            }
            break;
            default:
                break;
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
            TraCI_Lane::setMaxSpeed(id, value);
        }
        break;
        case VAR_LENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The length must be given as a double.", outputStorage);
            }
            TraCI_Lane::setLength(id, value);
        }
        break;
        case LANE_ALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Allowed classes must be given as a list of strings.", outputStorage);
            }
            TraCI_Lane::setAllowed(id, classes);
        }
        break;
        case LANE_DISALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Not allowed classes must be given as a list of strings.", outputStorage);
            }
            TraCI_Lane::setDisallowed(id, classes);
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
            TraCI_Lane::setParameter(id, name, value);
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


void
TraCIServerAPI_Lane::StoringVisitor::add(const MSLane* const l) const {
    switch (myDomain) {
        case CMD_GET_VEHICLE_VARIABLE: {
            const MSLane::VehCont& vehs = l->getVehiclesSecure();
            for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                if (myShape.distance2D((*j)->getPosition()) <= myRange) {
                    myIDs.insert((*j)->getID());
                }
            }
            l->releaseVehicles();
        }
        break;
        case CMD_GET_EDGE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance2D(myShape[0]) <= myRange) {
                myIDs.insert(l->getEdge().getID());
            }
        }
        break;
        case CMD_GET_LANE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance2D(myShape[0]) <= myRange) {
                myIDs.insert(l->getID());
            }
        }
        break;
        default:
            break;

    }
}


#endif


/****************************************************************************/

