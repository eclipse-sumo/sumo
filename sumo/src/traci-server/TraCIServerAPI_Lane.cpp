/****************************************************************************/
/// @file    TraCIServerAPI_Lane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting lane values via TraCI
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

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Lane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
            && variable != VAR_NOXEMISSION && variable != VAR_FUELCONSUMPTION && variable != VAR_NOISEEMISSION && variable != VAR_WAITING_TIME
            && variable != LAST_STEP_MEAN_SPEED && variable != LAST_STEP_VEHICLE_NUMBER
            && variable != LAST_STEP_VEHICLE_ID_LIST && variable != LAST_STEP_OCCUPANCY && variable != LAST_STEP_VEHICLE_HALTING_NUMBER
            && variable != LAST_STEP_LENGTH && variable != VAR_CURRENT_TRAVELTIME
            && variable != LANE_ALLOWED && variable != LANE_DISALLOWED && variable != VAR_WIDTH && variable != ID_COUNT
       ) {
        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Get Lane Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_LANE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable == ID_LIST) {
        std::vector<std::string> ids;
        MSLane::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids;
        MSLane::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        MSLane* lane = MSLane::dictionary(id);
        if (lane == 0) {
            return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Lane '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case LANE_LINK_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_UBYTE);
                tempMsg.writeUnsignedByte((int) lane->getLinkCont().size());
                break;
            case LANE_EDGE_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(lane->getEdge().getID());
                break;
            case VAR_LENGTH:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getLength());
                break;
            case VAR_MAXSPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getSpeedLimit());
                break;
            case LANE_LINKS: {
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tcpip::Storage tempContent;
                unsigned int cnt = 0;
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                const MSLinkCont& links = lane->getLinkCont();
                tempContent.writeInt((int) links.size());
                ++cnt;
                const SUMOTime currTime = MSNet::getInstance()->getCurrentTimeStep();
                for (MSLinkCont::const_iterator i = links.begin(); i != links.end(); ++i) {
                    MSLink* link = (*i);
                    // approached non-internal lane (if any)
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(link->getLane() != 0 ? link->getLane()->getID() : "");
                    ++cnt;
                    // approached "via", internal lane (if any)
                    tempContent.writeUnsignedByte(TYPE_STRING);
#ifdef HAVE_INTERNAL_LANES
                    tempContent.writeString(link->getViaLane() != 0 ? link->getViaLane()->getID() : "");
#else
                    tempContent.writeString("");
#endif
                    ++cnt;
                    // priority
                    tempContent.writeUnsignedByte(TYPE_UBYTE);
                    tempContent.writeUnsignedByte(link->havePriority() ? 1 : 0);
                    ++cnt;
                    // opened
                    tempContent.writeUnsignedByte(TYPE_UBYTE);
                    const SUMOReal speed = MIN2(lane->getSpeedLimit(), link->getLane()->getSpeedLimit());
                    tempContent.writeUnsignedByte(link->opened(currTime, speed, speed, DEFAULT_VEH_LENGTH, 0.0, DEFAULT_VEH_DECEL, 0) ? 1 : 0);
                    ++cnt;
                    // approaching foe
                    tempContent.writeUnsignedByte(TYPE_UBYTE);
                    tempContent.writeUnsignedByte(link->hasApproachingFoe(currTime, currTime, 0) ? 1 : 0);
                    ++cnt;
                    // state (not implemented, yet)
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(SUMOXMLDefinitions::LinkStates.getString(link->getState()));
                    ++cnt;
                    // direction
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(SUMOXMLDefinitions::LinkDirections.getString(link->getDirection()));
                    ++cnt;
                    // length
                    tempContent.writeUnsignedByte(TYPE_DOUBLE);
                    tempContent.writeDouble(link->getLength());
                    ++cnt;
                }
                tempMsg.writeInt((int) cnt);
                tempMsg.writeStorage(tempContent);
            }
            break;
            case LANE_ALLOWED: {
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                SVCPermissions permissions = lane->getPermissions();
                if (permissions == SVCFreeForAll) {  // special case: write nothing
                    permissions = 0;
                }
                tempMsg.writeStringList(getAllowedVehicleClassNamesList(permissions));
            }
            case LANE_DISALLOWED: {
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(getAllowedVehicleClassNamesList(~(lane->getPermissions()))); // negation yields disallowed
            }
            break;
            case VAR_SHAPE:
                tempMsg.writeUnsignedByte(TYPE_POLYGON);
                tempMsg.writeUnsignedByte((int)MIN2(static_cast<size_t>(255), lane->getShape().size()));
                for (unsigned int iPoint = 0; iPoint < MIN2(static_cast<size_t>(255), lane->getShape().size()); ++iPoint) {
                    tempMsg.writeDouble(lane->getShape()[iPoint].x());
                    tempMsg.writeDouble(lane->getShape()[iPoint].y());
                }
                break;
            case VAR_CO2EMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHBEFA_CO2Emissions());
                break;
            case VAR_COEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHBEFA_COEmissions());
                break;
            case VAR_HCEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHBEFA_HCEmissions());
                break;
            case VAR_PMXEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHBEFA_PMxEmissions());
                break;
            case VAR_NOXEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHBEFA_NOxEmissions());
                break;
            case VAR_FUELCONSUMPTION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHBEFA_FuelConsumption());
                break;
            case VAR_NOISEEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getHarmonoise_NoiseEmissions());
                break;
            case LAST_STEP_VEHICLE_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) lane->getVehicleNumber());
                break;
            case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getMeanSpeed());
                break;
            case LAST_STEP_VEHICLE_ID_LIST: {
                std::vector<std::string> vehIDs;
                const MSLane::VehCont& vehs = lane->getVehiclesSecure();
                for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                    vehIDs.push_back((*j)->getID());
                }
                lane->releaseVehicles();
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(vehIDs);
            }
            break;
            case LAST_STEP_OCCUPANCY:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getNettoOccupancy());
                break;
            case LAST_STEP_VEHICLE_HALTING_NUMBER: {
                int halting = 0;
                const MSLane::VehCont& vehs = lane->getVehiclesSecure();
                for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                    if ((*j)->getSpeed() < SUMO_const_haltingSpeed) {
                        ++halting;
                    }
                }
                lane->releaseVehicles();
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(halting);
            }
            break;
            case LAST_STEP_LENGTH: {
                SUMOReal lengthSum = 0;
                const MSLane::VehCont& vehs = lane->getVehiclesSecure();
                for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                    lengthSum += (*j)->getVehicleType().getLength();
                }
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                if (vehs.size() == 0) {
                    tempMsg.writeDouble(0);
                } else {
                    tempMsg.writeDouble(lengthSum / (SUMOReal) vehs.size());
                }
                lane->releaseVehicles();
            }
            break;
            case VAR_WAITING_TIME: {
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getWaitingSeconds());
            }
            break;
            case VAR_CURRENT_TRAVELTIME: {
                SUMOReal meanSpeed = lane->getMeanSpeed();
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                if (meanSpeed != 0) {
                    tempMsg.writeDouble(lane->getLength() / meanSpeed);
                } else {
                    tempMsg.writeDouble(1000000.);
                }
            }
            break;
            case VAR_WIDTH:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(lane->getWidth());
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
    if (variable != VAR_MAXSPEED && variable != VAR_LENGTH && variable != LANE_ALLOWED && variable != LANE_DISALLOWED) {
        return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Change Lane State: unsupported variable specified", outputStorage);
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
            l->setMaxSpeed(value);
        }
        break;
        case VAR_LENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The length must be given as a double.", outputStorage);
            }
            l->setLength(value);
        }
        break;
        case LANE_ALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Allowed classes must be given as a list of strings.", outputStorage);
            }
            l->setPermissions(parseVehicleClasses(classes));
            l->getEdge().rebuildAllowedLanes();
        }
        break;
        case LANE_DISALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Not allowed classes must be given as a list of strings.", outputStorage);
            }
            l->setPermissions(~parseVehicleClasses(classes)); // negation yields allowed
            l->getEdge().rebuildAllowedLanes();
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
    shape.push_back(l->getShape());
    return true;
}


void
TraCIServerAPI_Lane::StoringVisitor::add(const MSLane* const l) const {
    switch (myDomain) {
        case CMD_GET_VEHICLE_VARIABLE: {
            const MSLane::VehCont& vehs = l->getVehiclesSecure();
            for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                if (myShape.distance((*j)->getPosition()) <= myRange) {
                    myIDs.insert((*j)->getID());
                }
            }
            l->releaseVehicles();
        }
        break;
        case CMD_GET_EDGE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance(myShape[0]) <= myRange) {
                myIDs.insert(l->getEdge().getID());
            }
        }
        break;
        case CMD_GET_LANE_VARIABLE: {
            if (myShape.size() != 1 || l->getShape().distance(myShape[0]) <= myRange) {
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

