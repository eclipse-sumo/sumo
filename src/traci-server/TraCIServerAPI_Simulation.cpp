/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StdDefs.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/MSStoppingPlace.h>
#include <libsumo/Helper.h>
#include <libsumo/Simulation.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Simulation.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Simulation::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_SIM_VARIABLE, variable, id);
    try {
        switch (variable) {
            case VAR_TIME:
                server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                server.getWrapperStorage().writeDouble(SIMTIME);
                break;
            case VAR_TIME_STEP:
                server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                server.getWrapperStorage().writeInt((int)libsumo::Simulation::getCurrentTime());
                break;
            case VAR_LOADED_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_BUILT);
                break;
            case VAR_LOADED_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_BUILT);
                break;
            case VAR_DEPARTED_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_DEPARTED);
                break;
            case VAR_DEPARTED_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_DEPARTED);
                break;
            case VAR_TELEPORT_STARTING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_STARTING_TELEPORT);
                break;
            case VAR_TELEPORT_STARTING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_STARTING_TELEPORT);
                break;
            case VAR_TELEPORT_ENDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ENDING_TELEPORT);
                break;
            case VAR_TELEPORT_ENDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ENDING_TELEPORT);
                break;
            case VAR_ARRIVED_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ARRIVED);
                break;
            case VAR_ARRIVED_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ARRIVED);
                break;
            case VAR_PARKING_STARTING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_STARTING_PARKING);
                break;
            case VAR_PARKING_STARTING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_STARTING_PARKING);
                break;
            case VAR_PARKING_ENDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ENDING_PARKING);
                break;
            case VAR_PARKING_ENDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ENDING_PARKING);
                break;
            case VAR_STOP_STARTING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_STARTING_STOP);
                break;
            case VAR_STOP_STARTING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_STARTING_STOP);
                break;
            case VAR_STOP_ENDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ENDING_STOP);
                break;
            case VAR_STOP_ENDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_ENDING_STOP);
                break;
            case VAR_COLLIDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_COLLISION);
                break;
            case VAR_COLLIDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_COLLISION);
                break;
            case VAR_EMERGENCYSTOPPING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_EMERGENCYSTOP);
                break;
            case VAR_EMERGENCYSTOPPING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VEHICLE_STATE_EMERGENCYSTOP);
                break;
            case VAR_DELTA_T:
                server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                server.getWrapperStorage().writeDouble(libsumo::Simulation::getDeltaT());
                break;
            case VAR_MIN_EXPECTED_VEHICLES:
                server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                server.getWrapperStorage().writeInt(libsumo::Simulation::getMinExpectedNumber());
                break;
            case VAR_BUS_STOP_WAITING:
                server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                server.getWrapperStorage().writeInt(libsumo::Simulation::getBusStopWaiting(id));
                break;
            case VAR_NET_BOUNDING_BOX: {
                server.getWrapperStorage().writeUnsignedByte(TYPE_POLYGON);
                libsumo::TraCIPositionVector tb = libsumo::Simulation::getNetBoundary();
                server.getWrapperStorage().writeByte(2);
                server.getWrapperStorage().writeDouble(tb[0].x);
                server.getWrapperStorage().writeDouble(tb[0].y);
                server.getWrapperStorage().writeDouble(tb[1].x);
                server.getWrapperStorage().writeDouble(tb[1].y);
                break;
            }
            case POSITION_CONVERSION:
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Position conversion requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Position conversion requires a source position and a position type as parameter.", outputStorage);
                }
                if (!commandPositionConversion(server, inputStorage, server.getWrapperStorage(), CMD_GET_SIM_VARIABLE)) {
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
                if (!commandDistanceRequest(server, inputStorage, server.getWrapperStorage(), CMD_GET_SIM_VARIABLE)) {
                    return false;
                }
                break;
            case FIND_ROUTE: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 5) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires five parameter.", outputStorage);
                }
                std::string from, to, vtype;
                double depart;
                int routingMode;
                if (!server.readTypeCheckingString(inputStorage, from)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as first parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, to)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as second parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, vtype)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as third parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, depart)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as fourth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, routingMode)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires an integer as fifth parameter.", outputStorage);
                }
                writeStage(server.getWrapperStorage(), libsumo::Simulation::findRoute(from, to, vtype, depart, routingMode));
                break;
            }
            case FIND_INTERMODAL_ROUTE: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of an intermodal route requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 13) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of an intermodal route requires thirteen parameters.", outputStorage);
                }
                std::string from, to, modes, ptype, vtype, destStop;
                double depart, speed, walkFactor, departPos, arrivalPos, departPosLat;
                int routingMode;
                if (!server.readTypeCheckingString(inputStorage, from)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as first parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, to)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as second parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, modes)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as third parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, depart)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as fourth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, routingMode)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires an integer as fifth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as sixth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, walkFactor)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as seventh parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, departPos)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as eigth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, arrivalPos)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as nineth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, departPosLat)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as tenth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, ptype)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as eleventh parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, vtype)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as twelvth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, destStop)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as thirteenth parameter.", outputStorage);
                }
                const std::vector<libsumo::TraCIStage> result = libsumo::Simulation::findIntermodalRoute(from, to, modes, depart, routingMode, speed, walkFactor, departPos, arrivalPos, departPosLat, ptype, vtype, destStop);
                server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                server.getWrapperStorage().writeInt((int)result.size());
                for (const libsumo::TraCIStage s : result) {
                    writeStage(server.getWrapperStorage(), s);
                }
                break;
            }
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                server.getWrapperStorage().writeString(libsumo::Simulation::getParameter(id, paramName));
                break;
            }
            default:
                return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, "Get Simulation Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_SIM_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Simulation::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != CMD_CLEAR_PENDING_VEHICLES
            && variable != CMD_SAVE_SIMSTATE) {
        return server.writeErrorStatusCmd(CMD_SET_SIM_VARIABLE, "Set Simulation Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    // process
    try {
        switch (variable) {
            case CMD_CLEAR_PENDING_VEHICLES: {
                //clear any pending vehicle insertions
                std::string route;
                if (!server.readTypeCheckingString(inputStorage, route)) {
                    return server.writeErrorStatusCmd(CMD_SET_SIM_VARIABLE, "A string is needed for clearing pending vehicles.", outputStorage);
                }
                libsumo::Simulation::clearPending(route);
            }
            break;
            case CMD_SAVE_SIMSTATE: {
                //save current simulation state
                std::string file;
                if (!server.readTypeCheckingString(inputStorage, file)) {
                    return server.writeErrorStatusCmd(CMD_SET_SIM_VARIABLE, "A string is needed for saving simulation state.", outputStorage);
                }
                libsumo::Simulation::saveState(file);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_SIM_VARIABLE, e.what(), outputStorage);
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


void
TraCIServerAPI_Simulation::writeStage(tcpip::Storage& outputStorage, const libsumo::TraCIStage& stage) {
    outputStorage.writeUnsignedByte(TYPE_COMPOUND);
    outputStorage.writeInt(13);
    outputStorage.writeUnsignedByte(TYPE_INTEGER);
    outputStorage.writeInt(stage.type);
    outputStorage.writeUnsignedByte(TYPE_STRING);
    outputStorage.writeString(stage.vType);
    outputStorage.writeUnsignedByte(TYPE_STRING);
    outputStorage.writeString(stage.line);
    outputStorage.writeUnsignedByte(TYPE_STRING);
    outputStorage.writeString(stage.destStop);
    outputStorage.writeUnsignedByte(TYPE_STRINGLIST);
    outputStorage.writeStringList(stage.edges);
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(stage.travelTime);
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(stage.cost);
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(stage.length);
    outputStorage.writeUnsignedByte(TYPE_STRING);
    outputStorage.writeString(stage.intended);
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(stage.depart);
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(stage.departPos);
    outputStorage.writeUnsignedByte(TYPE_DOUBLE);
    outputStorage.writeDouble(stage.arrivalPos);
    outputStorage.writeUnsignedByte(TYPE_STRING);
    outputStorage.writeString(stage.description);
}


bool
TraCIServerAPI_Simulation::commandPositionConversion(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage, int commandId) {
    std::pair<MSLane*, double> roadPos;
    Position cartesianPos;
    Position geoPos;
    double z = 0;

    // actual position type that will be converted
    int srcPosType = inputStorage.readUnsignedByte();

    switch (srcPosType) {
        case POSITION_2D:
        case POSITION_3D:
        case POSITION_LON_LAT:
        case POSITION_LON_LAT_ALT: {
            double x = inputStorage.readDouble();
            double y = inputStorage.readDouble();
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
            double pos = inputStorage.readDouble();
            int laneIdx = inputStorage.readUnsignedByte();
            try {
                // convert edge,offset,laneIdx to cartesian position
                cartesianPos = geoPos = libsumo::Helper::getLaneChecking(roadID, laneIdx, pos)->getShape().positionAtOffset(pos);
                z = cartesianPos.z();
                GeoConvHelper::getFinal().cartesian2geo(geoPos);
            } catch (libsumo::TraCIException& e) {
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
            roadPos = libsumo::Helper::convertCartesianToRoadMap(cartesianPos);
            // write result that is added to response msg
            outputStorage.writeUnsignedByte(POSITION_ROADMAP);
            outputStorage.writeString(roadPos.first->getEdge().getID());
            outputStorage.writeDouble(roadPos.second);
            const std::vector<MSLane*> lanes = roadPos.first->getEdge().getLanes();
            outputStorage.writeUnsignedByte((int)distance(lanes.begin(), std::find(lanes.begin(), lanes.end(), roadPos.first)));
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
    std::pair<const MSLane*, double> roadPos1;
    std::pair<const MSLane*, double> roadPos2;

    // read position 1
    int posType = inputStorage.readUnsignedByte();
    switch (posType) {
        case POSITION_ROADMAP:
            try {
                std::string roadID = inputStorage.readString();
                roadPos1.second = inputStorage.readDouble();
                roadPos1.first = libsumo::Helper::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos1.second);
                pos1 = roadPos1.first->getShape().positionAtOffset(roadPos1.second);
            } catch (libsumo::TraCIException& e) {
                server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
                return false;
            }
            break;
        case POSITION_2D:
        case POSITION_3D: {
            double p1x = inputStorage.readDouble();
            double p1y = inputStorage.readDouble();
            pos1.set(p1x, p1y);
        }
        if (posType == POSITION_3D) {
            inputStorage.readDouble();// z value is ignored
        }
        roadPos1 = libsumo::Helper::convertCartesianToRoadMap(pos1);
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
                roadPos2.first = libsumo::Helper::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos2.second);
                pos2 = roadPos2.first->getShape().positionAtOffset(roadPos2.second);
            } catch (libsumo::TraCIException& e) {
                server.writeStatusCmd(commandId, RTYPE_ERR, e.what());
                return false;
            }
            break;
        case POSITION_2D:
        case POSITION_3D: {
            double p2x = inputStorage.readDouble();
            double p2y = inputStorage.readDouble();
            pos2.set(p2x, p2y);
        }
        if (posType == POSITION_3D) {
            inputStorage.readDouble();// z value is ignored
        }
        roadPos2 = libsumo::Helper::convertCartesianToRoadMap(pos2);
        break;
        default:
            server.writeStatusCmd(commandId, RTYPE_ERR, "Unknown position format used for distance request");
            return false;
    }

    // read distance type
    int distType = inputStorage.readUnsignedByte();

    double distance = 0.0;
    if (distType == REQUEST_DRIVINGDIST) {
        // compute driving distance
        if ((roadPos1.first == roadPos2.first) && (roadPos1.second <= roadPos2.second)) {
            // same edge
            distance = roadPos2.second - roadPos1.second;
        } else {
            ConstMSEdgeVector newRoute;
            if (roadPos2.first->isInternal()) {
                distance = roadPos2.second;
                roadPos2.first = roadPos2.first->getLogicalPredecessorLane();
                roadPos2.second = roadPos2.first->getLength();
            }
            MSNet::getInstance()->getRouterTT().compute(
                &roadPos1.first->getEdge(), &roadPos2.first->getEdge(), nullptr, MSNet::getInstance()->getCurrentTimeStep(), newRoute, true);
            if (newRoute.size() == 0) {
                distance = INVALID_DOUBLE_VALUE;
            } else {
                MSRoute route("", newRoute, false, nullptr, std::vector<SUMOVehicleParameter::Stop>());
                distance += route.getDistanceBetween(roadPos1.second, roadPos2.second, &roadPos1.first->getEdge(), &roadPos2.first->getEdge());
            }
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


/****************************************************************************/
