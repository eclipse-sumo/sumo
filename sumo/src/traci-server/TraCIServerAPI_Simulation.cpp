/****************************************************************************/
/// @file    TraCIServerAPI_Simulation.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting edge values via TraCI
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

#include <utils/common/StdDefs.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include "TraCIConstants.h"
#include "TraCIDijkstraRouter.h"
#include "TraCIServerAPI_Simulation.h"

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
TraCIServerAPI_Simulation::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                      tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=VAR_TIME_STEP
            &&variable!=VAR_LOADED_VEHICLES_NUMBER&&variable!=VAR_LOADED_VEHICLES_IDS
            &&variable!=VAR_DEPARTED_VEHICLES_NUMBER&&variable!=VAR_DEPARTED_VEHICLES_IDS
            &&variable!=VAR_TELEPORT_STARTING_VEHICLES_NUMBER&&variable!=VAR_TELEPORT_STARTING_VEHICLES_IDS
            &&variable!=VAR_TELEPORT_ENDING_VEHICLES_NUMBER&&variable!=VAR_TELEPORT_ENDING_VEHICLES_IDS
            &&variable!=VAR_ARRIVED_VEHICLES_NUMBER&&variable!=VAR_ARRIVED_VEHICLES_IDS
            &&variable!=VAR_DELTA_T&&variable!=VAR_NET_BOUNDING_BOX
            &&variable!=POSITION_CONVERSION&&variable!=DISTANCE_REQUEST
       ) {
        server.writeStatusCmd(CMD_GET_SIM_VARIABLE, RTYPE_ERR, "Get Simulation Variable: unsupported variable specified", outputStorage);
        return false;
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
    case VAR_LOADED_VEHICLES_NUMBER: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_BUILT)->second;
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    }
    break;
    case VAR_LOADED_VEHICLES_IDS: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_BUILT)->second;
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    }
    break;
    case VAR_DEPARTED_VEHICLES_NUMBER: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_DEPARTED)->second;
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    }
    break;
    case VAR_DEPARTED_VEHICLES_IDS: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_DEPARTED)->second;
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    }
    break;
    case VAR_TELEPORT_STARTING_VEHICLES_NUMBER: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_STARTING_TELEPORT)->second;
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    }
    break;
    case VAR_TELEPORT_STARTING_VEHICLES_IDS: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_STARTING_TELEPORT)->second;
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    }
    break;
    case VAR_TELEPORT_ENDING_VEHICLES_NUMBER: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_ENDING_TELEPORT)->second;
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    }
    break;
    case VAR_TELEPORT_ENDING_VEHICLES_IDS: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_ENDING_TELEPORT)->second;
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    }
    break;
    case VAR_ARRIVED_VEHICLES_NUMBER: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_ARRIVED)->second;
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    }
    break;
    case VAR_ARRIVED_VEHICLES_IDS: {
        const std::vector<std::string> &ids = server.getVehicleStateChanges().find(MSNet::VEHICLE_STATE_ARRIVED)->second;
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    }
    break;
    case VAR_DELTA_T:
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt(DELTA_T);
        break;
    case VAR_NET_BOUNDING_BOX: {
        tempMsg.writeUnsignedByte(TYPE_BOUNDINGBOX);
        Boundary b = GeoConvHelper::getConvBoundary();
        tempMsg.writeFloat(b.xmin());
        tempMsg.writeFloat(b.ymin());
        tempMsg.writeFloat(b.xmax());
        tempMsg.writeFloat(b.ymax());
        break;
    }
    break;
    case POSITION_CONVERSION:
        if (!commandPositionConversion(server, inputStorage, tempMsg, CMD_GET_SIM_VARIABLE)) {
            return false;
        }
        break;
    case DISTANCE_REQUEST:
        if (!commandDistanceRequest(server, inputStorage, tempMsg, CMD_GET_SIM_VARIABLE)) {
            return false;
        }
        break;
    default:
        break;
    }
    server.writeStatusCmd(CMD_GET_SIM_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + (int)tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


std::pair<MSLane*, SUMOReal>
TraCIServerAPI_Simulation::convertCartesianToRoadMap(Position2D pos) {
    std::pair<MSLane*, SUMOReal> result;
    std::vector<std::string> allEdgeIds;
    SUMOReal minDistance = std::numeric_limits<SUMOReal>::max();

    allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();
    for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
        const std::vector<MSLane*> &allLanes = MSEdge::dictionary((*itId))->getLanes();
        for (std::vector<MSLane*>::const_iterator itLane = allLanes.begin(); itLane != allLanes.end(); itLane++) {
            const SUMOReal newDistance = (*itLane)->getShape().distance(pos);
            if (newDistance < minDistance) {
                minDistance = newDistance;
                result.first = (*itLane);
            }
        }
    }
    result.second = result.first->getShape().nearest_position_on_line_to_point(pos, false);
    return result;
}


const MSLane*
TraCIServerAPI_Simulation::getLaneChecking(std::string roadID, int laneIndex, SUMOReal pos) {
    const MSEdge* edge = MSEdge::dictionary(roadID);
    if (edge == 0) {
        throw TraCIException("Unknown edge " + roadID);
    }
    if (laneIndex < 0 || laneIndex >= edge->getLanes().size()) {
        throw TraCIException("Invalid lane index for " + roadID);
    }
    const MSLane* lane = edge->getLanes()[laneIndex];
    if (pos < 0 ||pos > lane->getLength()) {
        throw TraCIException("Position on lane invalid");
    }
    return lane;
}


bool
TraCIServerAPI_Simulation::commandPositionConversion(traci::TraCIServer &server, tcpip::Storage &inputStorage,
                                                     tcpip::Storage &outputStorage, int commandId) {
    tcpip::Storage tmpResult;
    std::pair<MSLane*, SUMOReal> roadPos;
    Position2D cartesianPos;
    float x = 0;
    float y = 0;
    float z = 0;
    int destPosType;

    // actual position type that will be converted
    int srcPosType = inputStorage.readUnsignedByte();

    switch (srcPosType) {
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D:
        x = inputStorage.readFloat();
        y = inputStorage.readFloat();
        if (srcPosType != POSITION_2D) {
            z = inputStorage.readFloat();
        }
        // destination position type
        destPosType = inputStorage.readUnsignedByte();

        switch (destPosType) {
        case POSITION_ROADMAP: {
            // convert road map to 3D position
            roadPos = convertCartesianToRoadMap(Position2D(x, y));

            // write result that is added to response msg
            tmpResult.writeUnsignedByte(POSITION_ROADMAP);
            tmpResult.writeString(roadPos.first->getEdge().getID());
            tmpResult.writeFloat(roadPos.second);
            const std::vector<MSLane*> lanes = roadPos.first->getEdge().getLanes();
            tmpResult.writeUnsignedByte(distance(lanes.begin(), find(lanes.begin(), lanes.end(), roadPos.first)));
                               }
            break;
        case POSITION_3D:
            server.writeStatusCmd(commandId, RTYPE_ERR,
                           "Destination position type is same as source position type");
            return false;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR,
                           "Destination position type not supported");
            return false;
        }
        break;
    case POSITION_ROADMAP: {
        std::string roadID = inputStorage.readString();
        SUMOReal pos = inputStorage.readFloat();
        int laneIdx = inputStorage.readUnsignedByte();

        // destination position type
        destPosType = inputStorage.readUnsignedByte();

        switch (destPosType) {
        case POSITION_2D:
        case POSITION_2_5D:
        case POSITION_3D:
            //convert 3D to road map position
            try {
                Position2D result = getLaneChecking(roadID, laneIdx, pos)->getShape().positionAtLengthPosition(pos);
                x = (float)result.x();
                y = (float)result.y();
            } catch (TraCIException &e) {
                server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
                return false;
            }

            // write result that is added to response msg
            tmpResult.writeUnsignedByte(destPosType);
            tmpResult.writeFloat(x);
            tmpResult.writeFloat(y);
            if (destPosType != POSITION_2D) {
                tmpResult.writeFloat(z);
            }
            break;
        case POSITION_ROADMAP:
            server.writeStatusCmd(commandId, RTYPE_ERR,
                           "Destination position type is same as source position type");
            return false;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR,
                           "Destination position type not supported");
            return false;
        }
                           }
        break;
    default:
        server.writeStatusCmd(commandId, RTYPE_ERR,
                       "Source position type not supported");
        return false;
    }
    if (commandId == CMD_POSITIONCONVERSION) {
        // add converted Position to response
        outputStorage.writeUnsignedByte(1 + 1 + (int)tmpResult.size() + 1);	// length
        outputStorage.writeUnsignedByte(commandId);	// command id
        outputStorage.writeStorage(tmpResult);	// position dependant part
        outputStorage.writeUnsignedByte(destPosType);	// destination type
    } else {
        outputStorage.writeStorage(tmpResult);	// position dependant part
    }
    return true;
}

/****************************************************************************/

bool
TraCIServerAPI_Simulation::commandDistanceRequest(traci::TraCIServer &server, tcpip::Storage &inputStorage,
                                                  tcpip::Storage &outputStorage, int commandId) {
    Position2D pos1;
    Position2D pos2;
    std::pair<const MSLane*, SUMOReal> roadPos1;
    std::pair<const MSLane*, SUMOReal> roadPos2;

    // read position 1
    int posType = inputStorage.readUnsignedByte();
    switch (posType) {
    case POSITION_ROADMAP:
        try {
            std::string roadID = inputStorage.readString();
            roadPos1.second = inputStorage.readFloat();
            roadPos1.first = getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos1.second);
            pos1 = roadPos1.first->getShape().positionAtLengthPosition(roadPos1.second);
        } catch (TraCIException &e) {
            server.writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_ERR, e.what());
            return false;
        }
        break;
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D: {
        float p1x = inputStorage.readFloat();
        float p1y = inputStorage.readFloat();
        pos1.set(p1x, p1y);
    }
    if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
        inputStorage.readFloat();		// z value is ignored
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
            roadPos2.second = inputStorage.readFloat();
            roadPos2.first = getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos2.second);
            pos2 = roadPos2.first->getShape().positionAtLengthPosition(roadPos2.second);
        } catch (TraCIException &e) {
            server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
            return false;
        }
        break;
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D: {
        float p2x = inputStorage.readFloat();
        float p2y = inputStorage.readFloat();
        pos2.set(p2x, p2y);
    }
    if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
        inputStorage.readFloat();		// z value is ignored
    }
    roadPos2 = convertCartesianToRoadMap(pos2);
    break;
    default:
        server.writeStatusCmd(commandId, RTYPE_ERR, "Unknown position format used for distance request");
        return false;
    }

    // read distance type
    int distType = inputStorage.readUnsignedByte();

    float distance = 0.0;
    if (distType == REQUEST_DRIVINGDIST) {
        // compute driving distance
        std::vector<const MSEdge*> edges;
        TraCIDijkstraRouter<MSEdge> router(MSEdge::dictSize());

        if ((roadPos1.first == roadPos2.first)
                && (roadPos1.second <= roadPos2.second)) {
            distance = roadPos2.second - roadPos1.second;
        } else {
            router.compute(&roadPos1.first->getEdge(), &roadPos2.first->getEdge(), NULL,
                           MSNet::getInstance()->getCurrentTimeStep(), edges);
            MSRoute route("", edges, false, RGBColor::DEFAULT_COLOR, std::vector<SUMOVehicleParameter::Stop>());
            distance = static_cast<float>(route.getDistanceBetween(roadPos1.second, roadPos2.second,
                                          &roadPos1.first->getEdge(), &roadPos2.first->getEdge()));
        }
    } else {
        // compute air distance (default)
        // correct the distance type in case it was not valid
        distType = REQUEST_AIRDIST;
        distance = static_cast<float>(pos1.distanceTo(pos2));
    }
    // write response command
    if (commandId == CMD_DISTANCEREQUEST) {
        outputStorage.writeUnsignedByte(1 + 1 + 1 + 4);	// length
        outputStorage.writeUnsignedByte(commandId);
        outputStorage.writeUnsignedByte(distType);
    } else {
        outputStorage.writeUnsignedByte(TYPE_FLOAT);
    }
    outputStorage.writeFloat(distance);	// distance;
    return true;
}
