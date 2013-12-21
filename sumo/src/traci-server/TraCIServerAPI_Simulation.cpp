/****************************************************************************/
/// @file    TraCIServerAPI_Simulation.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting edge values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/StdDefs.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Simulation.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Simulation::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != VAR_TIME_STEP
            && variable != VAR_LOADED_VEHICLES_NUMBER && variable != VAR_LOADED_VEHICLES_IDS
            && variable != VAR_DEPARTED_VEHICLES_NUMBER && variable != VAR_DEPARTED_VEHICLES_IDS
            && variable != VAR_TELEPORT_STARTING_VEHICLES_NUMBER && variable != VAR_TELEPORT_STARTING_VEHICLES_IDS
            && variable != VAR_TELEPORT_ENDING_VEHICLES_NUMBER && variable != VAR_TELEPORT_ENDING_VEHICLES_IDS
            && variable != VAR_ARRIVED_VEHICLES_NUMBER && variable != VAR_ARRIVED_VEHICLES_IDS
            && variable != VAR_DELTA_T && variable != VAR_NET_BOUNDING_BOX
            && variable != VAR_MIN_EXPECTED_VEHICLES
            && variable != POSITION_CONVERSION && variable != DISTANCE_REQUEST
            && variable != VAR_BUS_STOP_WAITING
            && variable != VAR_PARKING_STARTING_VEHICLES_NUMBER && variable != VAR_PARKING_STARTING_VEHICLES_IDS
            && variable != VAR_PARKING_ENDING_VEHICLES_NUMBER && variable != VAR_PARKING_ENDING_VEHICLES_IDS
            && variable != VAR_STOP_STARTING_VEHICLES_NUMBER && variable != VAR_STOP_STARTING_VEHICLES_IDS
            && variable != VAR_STOP_ENDING_VEHICLES_NUMBER && variable != VAR_STOP_ENDING_VEHICLES_IDS
       ) {
        return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Get Simulation Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_SIM_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    switch (variable) {
        case VAR_TIME_STEP:
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(MSNet::getInstance()->getCurrentTimeStep());
            break;
        case VAR_LOADED_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_BUILT);
            break;
        case VAR_LOADED_VEHICLES_IDS: {
            const std::vector<std::string>& ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_BUILT)->second;
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        }
        break;
        case VAR_DEPARTED_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_DEPARTED);
            break;
        case VAR_DEPARTED_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_DEPARTED);
            break;
        case VAR_TELEPORT_STARTING_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_STARTING_TELEPORT);
            break;
        case VAR_TELEPORT_STARTING_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_STARTING_TELEPORT);
            break;
        case VAR_TELEPORT_ENDING_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_ENDING_TELEPORT);
            break;
        case VAR_TELEPORT_ENDING_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_ENDING_TELEPORT);
            break;
        case VAR_ARRIVED_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_ARRIVED);
            break;
        case VAR_ARRIVED_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_ARRIVED);
            break;
        case VAR_PARKING_STARTING_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_STARTING_PARKING);
            break;
        case VAR_PARKING_STARTING_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_STARTING_PARKING);
            break;
        case VAR_PARKING_ENDING_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_ENDING_PARKING);
            break;
        case VAR_PARKING_ENDING_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_ENDING_PARKING);
            break;
        case VAR_STOP_STARTING_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_STARTING_STOP);
            break;
        case VAR_STOP_STARTING_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_STARTING_STOP);
            break;
        case VAR_STOP_ENDING_VEHICLES_NUMBER:
            writeVehicleStateNumber(server, tempMsg, MSNet::VEHICLE_STATE_ENDING_STOP);
            break;
        case VAR_STOP_ENDING_VEHICLES_IDS:
            writeVehicleStateIDs(server, tempMsg, MSNet::VEHICLE_STATE_ENDING_STOP);
            break;
        case VAR_DELTA_T:
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(DELTA_T);
            break;
        case VAR_NET_BOUNDING_BOX: {
            tempMsg.writeUnsignedByte(TYPE_BOUNDINGBOX);
            Boundary b = GeoConvHelper::getFinal().getConvBoundary();
            tempMsg.writeDouble(b.xmin());
            tempMsg.writeDouble(b.ymin());
            tempMsg.writeDouble(b.xmax());
            tempMsg.writeDouble(b.ymax());
            break;
        }
        break;
        case VAR_MIN_EXPECTED_VEHICLES:
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(MSNet::getInstance()->getVehicleControl().getActiveVehicleCount() + MSNet::getInstance()->getInsertionControl().getPendingFlowCount());
            break;
        case POSITION_CONVERSION:
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Position conversion requires a compound object.", outputStorage);
            }
            if (inputStorage.readInt() != 2) {
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Position conversion requires a source position and a position type as parameter.", outputStorage);
            }
            if (!commandPositionConversion(server, inputStorage, tempMsg, CMD_GET_SIM_VARIABLE)) {
                return false;
            }
            break;
        case DISTANCE_REQUEST:
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of distance requires a compound object.", outputStorage);
            }
            if (inputStorage.readInt() != 3) {
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of distance requires two positions and a distance type as parameter.", outputStorage);
            }
            if (!commandDistanceRequest(server, inputStorage, tempMsg, CMD_GET_SIM_VARIABLE)) {
                return false;
            }
            break;
        case VAR_BUS_STOP_WAITING: {
            std::string id;
            if (!server.readTypeCheckingString(inputStorage, id)) {
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of persons at busstop requires a string.", outputStorage);
            }
            MSBusStop* s = MSNet::getInstance()->getBusStop(id);
            if (s == 0) {
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Unknown bus stop '" + id + "'.", outputStorage);
            }
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(s->getPersonNumber());
            break;
        }
        default:
            break;
    }
    server.writeStatusCmd(CMD_GET_SIM_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Simulation::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != CMD_CLEAR_PENDING_VEHICLES) {
        return server.writeErrorStatusCmd(CMD_SET_SIM_VARIABLE, "Set Simulation Variable: unsupported variable specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    // process
    switch (variable) {
        case CMD_CLEAR_PENDING_VEHICLES: {
            //clear any pending vehicle insertions
            std::string route;
            if (!server.readTypeCheckingString(inputStorage, route)) {
                return server.writeErrorStatusCmd(CMD_SET_SIM_VARIABLE, "A string is needed for clearing pending vehicles.", outputStorage);
            }
            MSNet::getInstance()->getInsertionControl().clearPendingVehicles(route);
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_SIM_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


void
TraCIServerAPI_Simulation::writeVehicleStateNumber(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state) {
    const std::vector<std::string>& ids = server.getVehicleStateChanges().find(state)->second;
    outputStorage.writeUnsignedByte(TYPE_INTEGER);
    outputStorage.writeInt((int) ids.size());
}


void
TraCIServerAPI_Simulation::writeVehicleStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state) {
    const std::vector<std::string>& ids = server.getVehicleStateChanges().find(state)->second;
    outputStorage.writeUnsignedByte(TYPE_STRINGLIST);
    outputStorage.writeStringList(ids);
}


std::pair<MSLane*, SUMOReal>
TraCIServerAPI_Simulation::convertCartesianToRoadMap(Position pos) {
    std::pair<MSLane*, SUMOReal> result;
    std::vector<std::string> allEdgeIds;
    SUMOReal minDistance = std::numeric_limits<SUMOReal>::max();

    allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();
    for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
        const std::vector<MSLane*>& allLanes = MSEdge::dictionary((*itId))->getLanes();
        for (std::vector<MSLane*>::const_iterator itLane = allLanes.begin(); itLane != allLanes.end(); itLane++) {
            const SUMOReal newDistance = (*itLane)->getShape().distance(pos);
            if (newDistance < minDistance) {
                minDistance = newDistance;
                result.first = (*itLane);
            }
        }
    }
    // @todo this may be a place where 3D is required but 2D is delivered
    result.second = result.first->getShape().nearest_offset_to_point2D(pos, false);
    return result;
}


const MSLane*
TraCIServerAPI_Simulation::getLaneChecking(std::string roadID, int laneIndex, SUMOReal pos) {
    const MSEdge* edge = MSEdge::dictionary(roadID);
    if (edge == 0) {
        throw TraCIException("Unknown edge " + roadID);
    }
    if (laneIndex < 0 || laneIndex >= (int)edge->getLanes().size()) {
        throw TraCIException("Invalid lane index for " + roadID);
    }
    const MSLane* lane = edge->getLanes()[laneIndex];
    if (pos < 0 || pos > lane->getLength()) {
        throw TraCIException("Position on lane invalid");
    }
    return lane;
}


bool
TraCIServerAPI_Simulation::commandPositionConversion(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage, int commandId) {
    std::pair<MSLane*, SUMOReal> roadPos;
    Position cartesianPos;
    Position geoPos;
    SUMOReal z = 0;

    // actual position type that will be converted
    int srcPosType = inputStorage.readUnsignedByte();

    switch (srcPosType) {
        case POSITION_2D:
        case POSITION_3D:
        case POSITION_LON_LAT:
        case POSITION_LON_LAT_ALT: {
            SUMOReal x = inputStorage.readDouble();
            SUMOReal y = inputStorage.readDouble();
            if (srcPosType != POSITION_2D && srcPosType != POSITION_LON_LAT) {
                z = inputStorage.readDouble();
            }
            geoPos.set(x, y);
            cartesianPos.set(x, y);
            if (srcPosType == POSITION_LON_LAT || srcPosType == POSITION_LON_LAT_ALT) {
                GeoConvHelper::getFinal().x2cartesian_const(cartesianPos);
            } else {
                GeoConvHelper::getFinal().cartesian2geo(geoPos);
            }
        }
        break;
        case POSITION_ROADMAP: {
            std::string roadID = inputStorage.readString();
            SUMOReal pos = inputStorage.readDouble();
            int laneIdx = inputStorage.readUnsignedByte();
            try {
                // convert edge,offset,laneIdx to cartesian position
                cartesianPos = geoPos = getLaneChecking(roadID, laneIdx, pos)->getShape().positionAtOffset(pos);
                z = cartesianPos.z();
                GeoConvHelper::getFinal().cartesian2geo(geoPos);
            } catch (TraCIException& e) {
                server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
                return false;
            }
        }
        break;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR, "Source position type not supported");
            return false;
    }

    int destPosType = 0;
    if (!server.readTypeCheckingUnsignedByte(inputStorage, destPosType)) {
        server.writeStatusCmd(commandId, RTYPE_ERR, "Destination position type must be of type ubyte.");
        return false;
    }

    switch (destPosType) {
        case POSITION_ROADMAP: {
            // convert cartesion position to edge,offset,lane_index
            roadPos = convertCartesianToRoadMap(cartesianPos);
            // write result that is added to response msg
            outputStorage.writeUnsignedByte(POSITION_ROADMAP);
            outputStorage.writeString(roadPos.first->getEdge().getID());
            outputStorage.writeDouble(roadPos.second);
            const std::vector<MSLane*> lanes = roadPos.first->getEdge().getLanes();
            outputStorage.writeUnsignedByte((int)distance(lanes.begin(), find(lanes.begin(), lanes.end(), roadPos.first)));
        }
        break;
        case POSITION_2D:
        case POSITION_3D:
        case POSITION_LON_LAT:
        case POSITION_LON_LAT_ALT:
            outputStorage.writeUnsignedByte(destPosType);
            if (destPosType == POSITION_LON_LAT || destPosType == POSITION_LON_LAT_ALT) {
                outputStorage.writeDouble(geoPos.x());
                outputStorage.writeDouble(geoPos.y());
            } else {
                outputStorage.writeDouble(cartesianPos.x());
                outputStorage.writeDouble(cartesianPos.y());
            }
            if (destPosType != POSITION_2D && destPosType != POSITION_LON_LAT) {
                outputStorage.writeDouble(z);
            }
            break;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR, "Destination position type not supported");
            return false;
    }
    return true;
}

/****************************************************************************/

bool
TraCIServerAPI_Simulation::commandDistanceRequest(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage, int commandId) {
    Position pos1;
    Position pos2;
    std::pair<const MSLane*, SUMOReal> roadPos1;
    std::pair<const MSLane*, SUMOReal> roadPos2;

    // read position 1
    int posType = inputStorage.readUnsignedByte();
    switch (posType) {
        case POSITION_ROADMAP:
            try {
                std::string roadID = inputStorage.readString();
                roadPos1.second = inputStorage.readDouble();
                roadPos1.first = getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos1.second);
                pos1 = roadPos1.first->getShape().positionAtOffset(roadPos1.second);
            } catch (TraCIException& e) {
                server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
                return false;
            }
            break;
        case POSITION_2D:
        case POSITION_3D: {
            SUMOReal p1x = inputStorage.readDouble();
            SUMOReal p1y = inputStorage.readDouble();
            pos1.set(p1x, p1y);
        }
        if (posType == POSITION_3D) {
            inputStorage.readDouble();		// z value is ignored
        }
        roadPos1 = convertCartesianToRoadMap(pos1);
        break;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR, "Unknown position format used for distance request");
            return false;
    }

    // read position 2
    posType = inputStorage.readUnsignedByte();
    switch (posType) {
        case POSITION_ROADMAP:
            try {
                std::string roadID = inputStorage.readString();
                roadPos2.second = inputStorage.readDouble();
                roadPos2.first = getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos2.second);
                pos2 = roadPos2.first->getShape().positionAtOffset(roadPos2.second);
            } catch (TraCIException& e) {
                server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
                return false;
            }
            break;
        case POSITION_2D:
        case POSITION_3D: {
            SUMOReal p2x = inputStorage.readDouble();
            SUMOReal p2y = inputStorage.readDouble();
            pos2.set(p2x, p2y);
        }
        if (posType == POSITION_3D) {
            inputStorage.readDouble();		// z value is ignored
        }
        roadPos2 = convertCartesianToRoadMap(pos2);
        break;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR, "Unknown position format used for distance request");
            return false;
    }

    // read distance type
    int distType = inputStorage.readUnsignedByte();

    SUMOReal distance = 0.0;
    if (distType == REQUEST_DRIVINGDIST) {
        // compute driving distance
        if ((roadPos1.first == roadPos2.first) && (roadPos1.second <= roadPos2.second)) {
            // same edge
            distance = roadPos2.second - roadPos1.second;
        } else {
            MSEdgeVector newRoute;
            MSNet::getInstance()->getRouterTT().compute(
                &roadPos1.first->getEdge(), &roadPos2.first->getEdge(), 0, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
            MSRoute route("", newRoute, false, 0, std::vector<SUMOVehicleParameter::Stop>());
            distance = route.getDistanceBetween(roadPos1.second, roadPos2.second, &roadPos1.first->getEdge(), &roadPos2.first->getEdge());
        }
    } else {
        // compute air distance (default)
        distance = pos1.distanceTo(pos2);
    }
    // write response command
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(distance);
    return true;
}


#endif

/****************************************************************************/
