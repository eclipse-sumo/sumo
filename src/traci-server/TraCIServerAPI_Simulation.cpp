/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
                                      const std::string& objID, const int variable) {
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
                throw libsumo::TraCIException("Position conversion requires a compound object.");
            }
            const int compoundSize = inputStorage.readInt();
            if (compoundSize < 2 || compoundSize > 3) {
                throw libsumo::TraCIException("Position conversion requires a source position and a position type as parameter.");
            }
            commandPositionConversion(inputStorage, compoundSize, server.getWrapperStorage());
            break;
        }
        case libsumo::DISTANCE_REQUEST:
            if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                throw libsumo::TraCIException("Retrieval of distance requires a compound object.");
            }
            if (inputStorage.readInt() != 3) {
                throw libsumo::TraCIException("Retrieval of distance requires two positions and a distance type as parameter.");
            }
            commandDistanceRequest(inputStorage, server.getWrapperStorage());
            break;
        case libsumo::FIND_ROUTE: {
            const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Retrieval of a route requires a compound object.");
            if (parameterCount < 5 || parameterCount > 7) {
                throw libsumo::TraCIException("Retrieval of a route requires between five to seven parameters.");
            }
            const std::string from = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as first parameter.");
            const std::string to = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as second parameter.");
            const std::string vtype = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as third parameter.");
            const double depart = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as fourth parameter.");
            const int routingMode = StoHelp::readTypedInt(inputStorage, "Retrieval of a route requires an integer as fifth parameter.");
            double departPos = 0.;
            if (parameterCount > 5) {
                departPos = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as sixth parameter.");
            }
            double arrivalPos = libsumo::INVALID_DOUBLE_VALUE;
            if (parameterCount > 6) {
                arrivalPos = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as seventh parameter.");
            }
            StoHelp::writeStage(server.getWrapperStorage(), libsumo::Simulation::findRoute(from, to, vtype, depart, routingMode, departPos, arrivalPos));
            break;
        }
        case libsumo::FIND_INTERMODAL_ROUTE: {
            StoHelp::readCompound(inputStorage, 13, "Retrieval of an intermodal route requires thirteen parameters.");
            const std::string from = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as first parameter.");
            const std::string to = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as second parameter.");
            const std::string modes = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as third parameter.");
            const double depart = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as fourth parameter.");
            const int routingMode = StoHelp::readTypedInt(inputStorage, "Retrieval of a route requires an integer as fifth parameter.");
            const double speed = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as sixth parameter.");
            const double walkFactor = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as seventh parameter.");
            const double departPos = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as eighth parameter.");
            const double arrivalPos = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as ninth parameter.");
            const double departPosLat = StoHelp::readTypedDouble(inputStorage, "Retrieval of a route requires a double as tenth parameter.");
            const std::string ptype = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as eleventh parameter.");
            const std::string vtype = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as twelfth parameter.");
            const std::string destStop = StoHelp::readTypedString(inputStorage, "Retrieval of a route requires a string as thirteenth parameter.");
            const std::vector<libsumo::TraCIStage>& result = libsumo::Simulation::findIntermodalRoute(from, to, modes, depart, routingMode, speed, walkFactor, departPos, arrivalPos, departPosLat, ptype, vtype, destStop);
            server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_COMPOUND);
            server.getWrapperStorage().writeInt((int)result.size());
            for (const libsumo::TraCIStage& s : result) {
                StoHelp::writeStage(server.getWrapperStorage(), s);
            }
            break;
        }
        default:
            if (!libsumo::Simulation::handleVariable(objID, variable, &server, &inputStorage)) {
                throw libsumo::TraCIException("Get Simulation Variable: unsupported variable " + toHex(variable, 2) + " specified");
            }
    }
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
                const double value = StoHelp::readTypedDouble(inputStorage, "A double is needed for setting traffic scale.");
                if (value < 0.0) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_SIM_VARIABLE, "Traffic scale may not be negative.", outputStorage);
                }
                libsumo::Simulation::setScale(value);
            }
            break;
            case libsumo::CMD_CLEAR_PENDING_VEHICLES:
                //clear any pending vehicle insertions
                libsumo::Simulation::clearPending(StoHelp::readTypedString(inputStorage, "A string is needed for clearing pending vehicles."));
                break;
            case libsumo::CMD_SAVE_SIMSTATE:
                //save current simulation state
                libsumo::Simulation::saveState(StoHelp::readTypedString(inputStorage, "A string is needed for saving simulation state."));
                break;
            case libsumo::CMD_LOAD_SIMSTATE: {
                //quick-load simulation state
                const double time = libsumo::Simulation::loadState(StoHelp::readTypedString(inputStorage, "A string is needed for loading simulation state."));
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
            case libsumo::CMD_MESSAGE:
                libsumo::Simulation::writeMessage(StoHelp::readTypedString(inputStorage, "A string is needed for adding a log message."));
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
    StoHelp::writeTypedInt(outputStorage, (int)server.getVehicleStateChanges().find(state)->second.size());
}


void
TraCIServerAPI_Simulation::writeVehicleStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state) {
    StoHelp::writeTypedStringList(outputStorage, server.getVehicleStateChanges().find(state)->second);
}


void
TraCIServerAPI_Simulation::writeTransportableStateNumber(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::TransportableState state) {
    StoHelp::writeTypedInt(outputStorage, (int)server.getTransportableStateChanges().find(state)->second.size());
}


void
TraCIServerAPI_Simulation::writeTransportableStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::TransportableState state) {
    StoHelp::writeTypedStringList(outputStorage, server.getTransportableStateChanges().find(state)->second);
}


void
TraCIServerAPI_Simulation::commandPositionConversion(tcpip::Storage& inputStorage,
        const int compoundSize, tcpip::Storage& outputStorage) {
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
            // convert edge,offset,laneIdx to cartesian position
            cartesianPos = geoPos = libsumo::Helper::getLaneChecking(roadID, laneIdx, pos)->geometryPositionAtOffset(pos);
            z = cartesianPos.z();
            GeoConvHelper::getFinal().cartesian2geo(geoPos);
        }
        break;
        default:
            throw libsumo::TraCIException("Source position type not supported");
    }

    const int destPosType = StoHelp::readTypedUnsignedByte(inputStorage, "Destination position type must be of type ubyte.");
    SUMOVehicleClass vClass = SVC_IGNORING;
    if (compoundSize == 3) {
        inputStorage.readUnsignedByte();
        const std::string& vClassString = inputStorage.readString();
        if (!SumoVehicleClassStrings.hasString(vClassString)) {
            throw libsumo::TraCIException("Unknown vehicle class '" + vClassString + "'.");
        }
        vClass = SumoVehicleClassStrings.get(vClassString);
    }

    switch (destPosType) {
        case libsumo::POSITION_ROADMAP: {
            // convert cartesion position to edge,offset,lane_index
            roadPos = libsumo::Helper::convertCartesianToRoadMap(cartesianPos, vClass);
            if (roadPos.first == nullptr) {
                throw libsumo::TraCIException("No matching lane found.");
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
            throw libsumo::TraCIException("Destination position type not supported");
    }
}


void
TraCIServerAPI_Simulation::commandDistanceRequest(tcpip::Storage& inputStorage, tcpip::Storage& outputStorage) {
    Position pos1;
    Position pos2;
    std::pair<const MSLane*, double> roadPos1;
    std::pair<const MSLane*, double> roadPos2;

    // read position 1
    int posType = inputStorage.readUnsignedByte();
    switch (posType) {
        case libsumo::POSITION_ROADMAP: {
            std::string roadID = inputStorage.readString();
            roadPos1.second = inputStorage.readDouble();
            roadPos1.first = libsumo::Helper::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos1.second);
            pos1 = roadPos1.first->geometryPositionAtOffset(roadPos1.second);
            break;
        }
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
            throw libsumo::TraCIException("Unknown position format used for distance request");
    }

    // read position 2
    posType = inputStorage.readUnsignedByte();
    switch (posType) {
        case libsumo::POSITION_ROADMAP: {
            std::string roadID = inputStorage.readString();
            roadPos2.second = inputStorage.readDouble();
            roadPos2.first = libsumo::Helper::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos2.second);
            pos2 = roadPos2.first->geometryPositionAtOffset(roadPos2.second);
            break;
        }
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
            throw libsumo::TraCIException("Unknown position format used for distance request");
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
    StoHelp::writeTypedDouble(outputStorage, distance);
}


/****************************************************************************/
