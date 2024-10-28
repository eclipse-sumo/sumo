/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    TraCIServerAPI_Simulation.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
///
// APIs for getting/setting simulation values via TraCI
/****************************************************************************/
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
#include <libsumo/TraCIConstants.h>
#include <libsumo/StorageHelper.h>
#include "TraCIServerAPI_Simulation.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Simulation::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_SIM_VARIABLE, variable, id);
    try {
        // unlike the other domains we cannot check here first whether libsumo::Simulation can handle it because the implementations for the state variables differ
        switch (variable) {
            case libsumo::VAR_LOADED_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::BUILT);
                break;
            case libsumo::VAR_LOADED_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::BUILT);
                break;
            case libsumo::VAR_DEPARTED_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::DEPARTED);
                break;
            case libsumo::VAR_DEPARTED_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::DEPARTED);
                break;
            case libsumo::VAR_TELEPORT_STARTING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::STARTING_TELEPORT);
                break;
            case libsumo::VAR_TELEPORT_STARTING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::STARTING_TELEPORT);
                break;
            case libsumo::VAR_TELEPORT_ENDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::ENDING_TELEPORT);
                break;
            case libsumo::VAR_TELEPORT_ENDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::ENDING_TELEPORT);
                break;
            case libsumo::VAR_ARRIVED_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::ARRIVED);
                break;
            case libsumo::VAR_ARRIVED_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::ARRIVED);
                break;
            case libsumo::VAR_DEPARTED_PERSONS_NUMBER:
                writeTransportableStateNumber(server, server.getWrapperStorage(), MSNet::TransportableState::PERSON_DEPARTED);
                break;
            case libsumo::VAR_DEPARTED_PERSONS_IDS:
                writeTransportableStateIDs(server, server.getWrapperStorage(), MSNet::TransportableState::PERSON_DEPARTED);
                break;
            case libsumo::VAR_ARRIVED_PERSONS_NUMBER:
                writeTransportableStateNumber(server, server.getWrapperStorage(), MSNet::TransportableState::PERSON_ARRIVED);
                break;
            case libsumo::VAR_ARRIVED_PERSONS_IDS:
                writeTransportableStateIDs(server, server.getWrapperStorage(), MSNet::TransportableState::PERSON_ARRIVED);
                break;
            case libsumo::VAR_PARKING_STARTING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::STARTING_PARKING);
                break;
            case libsumo::VAR_PARKING_STARTING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::STARTING_PARKING);
                break;
            case libsumo::VAR_PARKING_MANEUVERING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::MANEUVERING);
                break;
            case libsumo::VAR_PARKING_MANEUVERING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::MANEUVERING);
                break;
            case libsumo::VAR_PARKING_ENDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::ENDING_PARKING);
                break;
            case libsumo::VAR_PARKING_ENDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::ENDING_PARKING);
                break;
            case libsumo::VAR_STOP_STARTING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::STARTING_STOP);
                break;
            case libsumo::VAR_STOP_STARTING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::STARTING_STOP);
                break;
            case libsumo::VAR_STOP_ENDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::ENDING_STOP);
                break;
            case libsumo::VAR_STOP_ENDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::ENDING_STOP);
                break;
            case libsumo::VAR_COLLIDING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::COLLISION);
                break;
            case libsumo::VAR_COLLIDING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::COLLISION);
                break;
            case libsumo::VAR_EMERGENCYSTOPPING_VEHICLES_NUMBER:
                writeVehicleStateNumber(server, server.getWrapperStorage(), MSNet::VehicleState::EMERGENCYSTOP);
                break;
            case libsumo::VAR_EMERGENCYSTOPPING_VEHICLES_IDS:
                writeVehicleStateIDs(server, server.getWrapperStorage(), MSNet::VehicleState::EMERGENCYSTOP);
                break;
            case libsumo::VAR_COLLISIONS: {
                std::vector<libsumo::TraCICollision> collisions = libsumo::Simulation::getCollisions();
                server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_COMPOUND);
                const int cnt = 1 + (int)collisions.size() * 4;
                server.getWrapperStorage().writeInt(cnt);
                server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_INTEGER);
                server.getWrapperStorage().writeInt((int)collisions.size());
                for (const auto& c : collisions) {
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_STRING);
                    server.getWrapperStorage().writeString(c.collider);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_STRING);
                    server.getWrapperStorage().writeString(c.victim);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_STRING);
                    server.getWrapperStorage().writeString(c.colliderType);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_STRING);
                    server.getWrapperStorage().writeString(c.victimType);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_DOUBLE);
                    server.getWrapperStorage().writeDouble(c.colliderSpeed);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_DOUBLE);
                    server.getWrapperStorage().writeDouble(c.victimSpeed);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_STRING);
                    server.getWrapperStorage().writeString(c.type);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_STRING);
                    server.getWrapperStorage().writeString(c.lane);
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_DOUBLE);
                    server.getWrapperStorage().writeDouble(c.pos);
                }
                break;
            }
            case libsumo::VAR_NET_BOUNDING_BOX: {
                server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_POLYGON);
                libsumo::TraCIPositionVector tb = libsumo::Simulation::getNetBoundary();
                server.getWrapperStorage().writeByte(2);
                server.getWrapperStorage().writeDouble(tb.value[0].x);
                server.getWrapperStorage().writeDouble(tb.value[0].y);
                server.getWrapperStorage().writeDouble(tb.value[1].x);
                server.getWrapperStorage().writeDouble(tb.value[1].y);
                break;
            }
            case libsumo::POSITION_CONVERSION: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Position conversion requires a compound object.", outputStorage);
                }
                const int compoundSize = inputStorage.readInt();
                if (compoundSize < 2 || compoundSize > 3) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Position conversion requires a source position and a position type as parameter.", outputStorage);
                }
                if (!commandPositionConversion(server, inputStorage, compoundSize, server.getWrapperStorage(), libsumo::CMD_GET_SIM_VARIABLE)) {
                    return false;
                }
                break;
            }
            case libsumo::DISTANCE_REQUEST:
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of distance requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 3) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of distance requires two positions and a distance type as parameter.", outputStorage);
                }
                if (!commandDistanceRequest(server, inputStorage, server.getWrapperStorage(), libsumo::CMD_GET_SIM_VARIABLE)) {
                    return false;
                }
                break;
            case libsumo::FIND_ROUTE: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 5) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires five parameter.", outputStorage);
                }
                std::string from, to, vtype;
                double depart;
                int routingMode;
                if (!server.readTypeCheckingString(inputStorage, from)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as first parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, to)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as second parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, vtype)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as third parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, depart)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as fourth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, routingMode)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires an integer as fifth parameter.", outputStorage);
                }
                libsumo::StorageHelper::writeStage(server.getWrapperStorage(), libsumo::Simulation::findRoute(from, to, vtype, depart, routingMode));
                break;
            }
            case libsumo::FIND_INTERMODAL_ROUTE: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of an intermodal route requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 13) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of an intermodal route requires thirteen parameters.", outputStorage);
                }
                std::string from, to, modes, ptype, vtype, destStop;
                double depart, speed, walkFactor, departPos, arrivalPos, departPosLat;
                int routingMode;
                if (!server.readTypeCheckingString(inputStorage, from)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as first parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, to)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as second parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, modes)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as third parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, depart)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as fourth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, routingMode)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires an integer as fifth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as sixth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, walkFactor)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as seventh parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, departPos)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as eigth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, arrivalPos)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as nineth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingDouble(inputStorage, departPosLat)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a double as tenth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, ptype)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as eleventh parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, vtype)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as twelvth parameter.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, destStop)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Retrieval of a route requires a string as thirteenth parameter.", outputStorage);
                }
                const std::vector<libsumo::TraCIStage>& result = libsumo::Simulation::findIntermodalRoute(from, to, modes, depart, routingMode, speed, walkFactor, departPos, arrivalPos, departPosLat, ptype, vtype, destStop);
                server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_COMPOUND);
                server.getWrapperStorage().writeInt((int)result.size());
                for (const libsumo::TraCIStage& s : result) {
                    libsumo::StorageHelper::writeStage(server.getWrapperStorage(), s);
                }
                break;
            }
            default:
                if (!libsumo::Simulation::handleVariable(id, variable, &server, &inputStorage)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, "Get Simulation Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
                }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_SIM_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Simulation::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                      tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::CMD_CLEAR_PENDING_VEHICLES
            && variable != libsumo::CMD_SAVE_SIMSTATE
            && variable != libsumo::CMD_LOAD_SIMSTATE
            && variable != libsumo::VAR_PARAMETER
            && variable != libsumo::VAR_SCALE
            && variable != libsumo::CMD_MESSAGE
       ) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "Set Simulation Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    // process
    try {
        switch (variable) {
            case libsumo::VAR_SCALE: {
                double value;
                if (!server.readTypeCheckingDouble(inputStorage, value)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "A double is needed for setting traffic scale.", outputStorage);
                }
                if (value < 0.0) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "Traffic scale may not be negative.", outputStorage);
                }
                libsumo::Simulation::setScale(value);
            }
            break;
            case libsumo::CMD_CLEAR_PENDING_VEHICLES: {
                //clear any pending vehicle insertions
                std::string route;
                if (!server.readTypeCheckingString(inputStorage, route)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "A string is needed for clearing pending vehicles.", outputStorage);
                }
                libsumo::Simulation::clearPending(route);
            }
            break;
            case libsumo::CMD_SAVE_SIMSTATE: {
                //save current simulation state
                std::string file;
                if (!server.readTypeCheckingString(inputStorage, file)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "A string is needed for saving simulation state.", outputStorage);
                }
                libsumo::Simulation::saveState(file);
            }
            break;
            case libsumo::CMD_LOAD_SIMSTATE: {
                //quick-load simulation state
                std::string file;
                if (!server.readTypeCheckingString(inputStorage, file)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "A string is needed for loading simulation state.", outputStorage);
                }
                double time = libsumo::Simulation::loadState(file);
                TraCIServer::getInstance()->stateLoaded(TIME2STEPS(time));
            }
            break;
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Simulation::setParameter(id, name, value);
                break;
            }
            case libsumo::CMD_MESSAGE: {
                std::string msg;
                if (!server.readTypeCheckingString(inputStorage, msg)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "A string is needed for adding a log message.", outputStorage);
                }
                libsumo::Simulation::writeMessage(msg);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


void
TraCIServerAPI_Simulation::writeVehicleStateNumber(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state) {
    const std::vector<std::string>& ids = server.getVehicleStateChanges().find(state)->second;
    outputStorage.writeUnsignedByte(libsumo::TYPE_INTEGER);
    outputStorage.writeInt((int) ids.size());
}


void
TraCIServerAPI_Simulation::writeVehicleStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state) {
    const std::vector<std::string>& ids = server.getVehicleStateChanges().find(state)->second;
    outputStorage.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
    outputStorage.writeStringList(ids);
}


void
TraCIServerAPI_Simulation::writeTransportableStateNumber(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::TransportableState state) {
    const std::vector<std::string>& ids = server.getTransportableStateChanges().find(state)->second;
    outputStorage.writeUnsignedByte(libsumo::TYPE_INTEGER);
    outputStorage.writeInt((int)ids.size());
}


void
TraCIServerAPI_Simulation::writeTransportableStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::TransportableState state) {
    const std::vector<std::string>& ids = server.getTransportableStateChanges().find(state)->second;
    outputStorage.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
    outputStorage.writeStringList(ids);
}


bool
TraCIServerAPI_Simulation::commandPositionConversion(TraCIServer& server, tcpip::Storage& inputStorage,
        const int compoundSize, tcpip::Storage& outputStorage,
        const int commandId) {
    std::pair<MSLane*, double> roadPos;
    Position cartesianPos;
    Position geoPos;
    double z = 0;

    // actual position type that will be converted
    int srcPosType = inputStorage.readUnsignedByte();

    switch (srcPosType) {
        case libsumo::POSITION_2D:
        case libsumo::POSITION_3D:
        case libsumo::POSITION_LON_LAT:
        case libsumo::POSITION_LON_LAT_ALT: {
            const double x = inputStorage.readDouble();
            const double y = inputStorage.readDouble();
            if (srcPosType != libsumo::POSITION_2D && srcPosType != libsumo::POSITION_LON_LAT) {
                z = inputStorage.readDouble();
            }
            geoPos.set(x, y);
            cartesianPos.set(x, y);
            if (srcPosType == libsumo::POSITION_LON_LAT || srcPosType == libsumo::POSITION_LON_LAT_ALT) {
                GeoConvHelper::getFinal().x2cartesian_const(cartesianPos);
            } else {
                GeoConvHelper::getFinal().cartesian2geo(geoPos);
            }
        }
        break;
        case libsumo::POSITION_ROADMAP: {
            const std::string roadID = inputStorage.readString();
            const double pos = inputStorage.readDouble();
            const int laneIdx = inputStorage.readUnsignedByte();
            try {
                // convert edge,offset,laneIdx to cartesian position
                cartesianPos = geoPos = libsumo::Helper::getLaneChecking(roadID, laneIdx, pos)->geometryPositionAtOffset(pos);
                z = cartesianPos.z();
                GeoConvHelper::getFinal().cartesian2geo(geoPos);
            } catch (libsumo::TraCIException& e) {
                server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, e.what());
                return false;
            }
        }
        break;
        default:
            server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "Source position type not supported");
            return false;
    }

    int destPosType = 0;
    if (!server.readTypeCheckingUnsignedByte(inputStorage, destPosType)) {
        server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "Destination position type must be of type ubyte.");
        return false;
    }

    SUMOVehicleClass vClass = SVC_IGNORING;
    if (compoundSize == 3) {
        inputStorage.readUnsignedByte();
        const std::string& vClassString = inputStorage.readString();
        if (!SumoVehicleClassStrings.hasString(vClassString)) {
            server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "Unknown vehicle class '" + vClassString + "'.");
            return false;
        }
        vClass = SumoVehicleClassStrings.get(vClassString);
    }

    switch (destPosType) {
        case libsumo::POSITION_ROADMAP: {
            // convert cartesion position to edge,offset,lane_index
            roadPos = libsumo::Helper::convertCartesianToRoadMap(cartesianPos, vClass);
            if (roadPos.first == nullptr) {
                server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "No matching lane found.");
                return false;
            }
            // write result that is added to response msg
            outputStorage.writeUnsignedByte(libsumo::POSITION_ROADMAP);
            outputStorage.writeString(roadPos.first->getEdge().getID());
            outputStorage.writeDouble(roadPos.second);
            outputStorage.writeUnsignedByte(roadPos.first->getIndex());
        }
        break;
        case libsumo::POSITION_2D:
        case libsumo::POSITION_3D:
        case libsumo::POSITION_LON_LAT:
        case libsumo::POSITION_LON_LAT_ALT:
            outputStorage.writeUnsignedByte(destPosType);
            if (destPosType == libsumo::POSITION_LON_LAT || destPosType == libsumo::POSITION_LON_LAT_ALT) {
                outputStorage.writeDouble(geoPos.x());
                outputStorage.writeDouble(geoPos.y());
            } else {
                outputStorage.writeDouble(cartesianPos.x());
                outputStorage.writeDouble(cartesianPos.y());
            }
            if (destPosType != libsumo::POSITION_2D && destPosType != libsumo::POSITION_LON_LAT) {
                outputStorage.writeDouble(z);
            }
            break;
        default:
            server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "Destination position type not supported");
            return false;
    }
    return true;
}


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
        case libsumo::POSITION_ROADMAP:
            try {
                std::string roadID = inputStorage.readString();
                roadPos1.second = inputStorage.readDouble();
                roadPos1.first = libsumo::Helper::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos1.second);
                pos1 = roadPos1.first->geometryPositionAtOffset(roadPos1.second);
            } catch (libsumo::TraCIException& e) {
                server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, e.what());
                return false;
            }
            break;
        case libsumo::POSITION_2D:
        case libsumo::POSITION_3D: {
            double p1x = inputStorage.readDouble();
            double p1y = inputStorage.readDouble();
            pos1.set(p1x, p1y);
        }
        if (posType == libsumo::POSITION_3D) {
            inputStorage.readDouble();// z value is ignored
        }
        roadPos1 = libsumo::Helper::convertCartesianToRoadMap(pos1, SVC_IGNORING);
        break;
        case libsumo::POSITION_LON_LAT:
        case libsumo::POSITION_LON_LAT_ALT: {
            double p1x = inputStorage.readDouble();
            double p1y = inputStorage.readDouble();
            pos1.set(p1x, p1y);
            GeoConvHelper::getFinal().x2cartesian_const(pos1);
        }
        if (posType == libsumo::POSITION_LON_LAT_ALT) {
            inputStorage.readDouble();// altitude value is ignored
        }
        roadPos1 = libsumo::Helper::convertCartesianToRoadMap(pos1, SVC_IGNORING);
        break;
        default:
            server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "Unknown position format used for distance request");
            return false;
    }

    // read position 2
    posType = inputStorage.readUnsignedByte();
    switch (posType) {
        case libsumo::POSITION_ROADMAP:
            try {
                std::string roadID = inputStorage.readString();
                roadPos2.second = inputStorage.readDouble();
                roadPos2.first = libsumo::Helper::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos2.second);
                pos2 = roadPos2.first->geometryPositionAtOffset(roadPos2.second);
            } catch (libsumo::TraCIException& e) {
                server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, e.what());
                return false;
            }
            break;
        case libsumo::POSITION_2D:
        case libsumo::POSITION_3D: {
            double p2x = inputStorage.readDouble();
            double p2y = inputStorage.readDouble();
            pos2.set(p2x, p2y);
        }
        if (posType == libsumo::POSITION_3D) {
            inputStorage.readDouble();// z value is ignored
        }
        roadPos2 = libsumo::Helper::convertCartesianToRoadMap(pos2, SVC_IGNORING);
        break;
        case libsumo::POSITION_LON_LAT:
        case libsumo::POSITION_LON_LAT_ALT: {
            double p2x = inputStorage.readDouble();
            double p2y = inputStorage.readDouble();
            pos2.set(p2x, p2y);
            GeoConvHelper::getFinal().x2cartesian_const(pos2);
        }
        if (posType == libsumo::POSITION_LON_LAT_ALT) {
            inputStorage.readDouble();// altitude value is ignored
        }
        roadPos2 = libsumo::Helper::convertCartesianToRoadMap(pos2, SVC_IGNORING);
        break;
        default:
            server.writeStatusCmd(commandId, libsumo::RTYPE_ERR, "Unknown position format used for distance request");
            return false;
    }

    // read distance type
    const int distType = inputStorage.readUnsignedByte();

    double distance = 0.0;
    if (distType == libsumo::REQUEST_DRIVINGDIST) {
        distance = libsumo::Helper::getDrivingDistance(roadPos1, roadPos2);
    } else {
        // compute air distance (default)
        distance = pos1.distanceTo(pos2);
    }
    // write response command
    outputStorage.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    outputStorage.writeDouble(distance);
    return true;
}


/****************************************************************************/
