/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    TraCIServerAPI_Vehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @author  Bjoern Hendriks
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Robert Hilbrich
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle values via TraCI
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

#include <microsim/MSNet.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/geom/PositionVector.h>
#include <utils/vehicle/DijkstraRouter.h>
#include <utils/vehicle/DijkstraRouter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <libsumo/Vehicle.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Simulation.h"
#include "TraCIServerAPI_Vehicle.h"
#include "TraCIServerAPI_VehicleType.h"


// ===========================================================================
// static member variables
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Vehicle::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_SPEED && variable != VAR_SPEED_WITHOUT_TRACI
            && variable != VAR_POSITION && variable != VAR_ANGLE && variable != VAR_POSITION3D
            && variable != VAR_ROAD_ID && variable != VAR_LANE_ID && variable != VAR_LANE_INDEX
            && variable != VAR_TYPE && variable != VAR_ROUTE_ID && variable != VAR_COLOR
            && variable != VAR_LANEPOSITION
            && variable != VAR_LANEPOSITION_LAT
            && variable != VAR_CO2EMISSION && variable != VAR_COEMISSION
            && variable != VAR_HCEMISSION && variable != VAR_PMXEMISSION
            && variable != VAR_NOXEMISSION && variable != VAR_FUELCONSUMPTION && variable != VAR_NOISEEMISSION
            && variable != VAR_ELECTRICITYCONSUMPTION && variable != VAR_PERSON_NUMBER && variable != VAR_LEADER
            && variable != VAR_EDGE_TRAVELTIME && variable != VAR_EDGE_EFFORT
            && variable != VAR_ROUTE_VALID && variable != VAR_EDGES
            && variable != VAR_SIGNALS && variable != VAR_DISTANCE
            && variable != VAR_LENGTH && variable != VAR_MAXSPEED && variable != VAR_VEHICLECLASS
            && variable != VAR_SPEED_FACTOR && variable != VAR_SPEED_DEVIATION
            && variable != VAR_ALLOWED_SPEED && variable != VAR_EMISSIONCLASS
            && variable != VAR_WIDTH && variable != VAR_MINGAP && variable != VAR_SHAPECLASS
            && variable != VAR_ACCEL && variable != VAR_DECEL && variable != VAR_IMPERFECTION
            && variable != VAR_APPARENT_DECEL && variable != VAR_EMERGENCY_DECEL
            && variable != VAR_ACTIONSTEPLENGTH
            && variable != VAR_TAU && variable != VAR_BEST_LANES && variable != DISTANCE_REQUEST
            && variable != VAR_LATALIGNMENT
            && variable != VAR_MAXSPEED_LAT
            && variable != VAR_MINGAP_LAT
            && variable != ID_COUNT && variable != VAR_STOPSTATE
            && variable != VAR_WAITING_TIME && variable != VAR_ACCUMULATED_WAITING_TIME
            && variable != VAR_ROUTE_INDEX
            && variable != VAR_PARAMETER
            && variable != VAR_SPEEDSETMODE
            && variable != VAR_LANECHANGE_MODE
            && variable != VAR_ROUTING_MODE
            && variable != VAR_NEXT_TLS
            && variable != VAR_SLOPE
            && variable != VAR_HEIGHT
            && variable != VAR_LINE
            && variable != VAR_VIA
            && variable != CMD_CHANGELANE
       ) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Get Vehicle Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_VEHICLE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::Vehicle::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getIDCount());
                break;
            case VAR_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getSpeed(id));
                break;
            case VAR_SPEED_WITHOUT_TRACI:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getSpeedWithoutTraCI(id));
                break;
            case VAR_POSITION: {
                tempMsg.writeUnsignedByte(POSITION_2D);
                libsumo::TraCIPosition pos = libsumo::Vehicle::getPosition(id);
                tempMsg.writeDouble(pos.x);
                tempMsg.writeDouble(pos.y);
                break;
            }
            case VAR_POSITION3D: {
                tempMsg.writeUnsignedByte(POSITION_3D);
                libsumo::TraCIPosition pos = libsumo::Vehicle::getPosition(id);
                tempMsg.writeDouble(pos.x);
                tempMsg.writeDouble(pos.y);
                tempMsg.writeDouble(pos.z);
                break;
            }
            case VAR_ANGLE:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getAngle(id));
                break;
            case VAR_SLOPE:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getSlope(id));
                break;
            case VAR_ROAD_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Vehicle::getRoadID(id));
                break;
            case VAR_LANE_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Vehicle::getLaneID(id));
                break;
            case VAR_LANE_INDEX:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getLaneIndex(id));
                break;
            case VAR_TYPE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Vehicle::getTypeID(id));
                break;
            case VAR_ROUTE_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Vehicle::getRouteID(id));
                break;
            case VAR_ROUTE_INDEX:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getRouteIndex(id));
                break;
            case VAR_COLOR: {
                libsumo::TraCIColor color = libsumo::Vehicle::getColor(id);
                tempMsg.writeUnsignedByte(TYPE_COLOR);
                tempMsg.writeUnsignedByte(color.r);
                tempMsg.writeUnsignedByte(color.g);
                tempMsg.writeUnsignedByte(color.b);
                tempMsg.writeUnsignedByte(color.a);
                break;
            }
            case VAR_LANEPOSITION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getLanePosition(id));
                break;
            case VAR_LANEPOSITION_LAT:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getLateralLanePosition(id));
                break;
            case VAR_CO2EMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getCO2Emission(id));
                break;
            case VAR_COEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getCOEmission(id));
                break;
            case VAR_HCEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getHCEmission(id));
                break;
            case VAR_PMXEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getPMxEmission(id));
                break;
            case VAR_NOXEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getNOxEmission(id));
                break;
            case VAR_FUELCONSUMPTION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getFuelConsumption(id));
                break;
            case VAR_NOISEEMISSION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getNoiseEmission(id));
                break;
            case VAR_ELECTRICITYCONSUMPTION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getElectricityConsumption(id));
                break;
            case VAR_PERSON_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getPersonNumber(id));
                break;
            case VAR_LEADER: {
                double dist = 0;
                if (!server.readTypeCheckingDouble(inputStorage, dist)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Leader retrieval requires a double.", outputStorage);
                }
                std::pair<std::string, double> leaderInfo = libsumo::Vehicle::getLeader(id, dist);
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tempMsg.writeInt(2);
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(leaderInfo.first);
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(leaderInfo.second);
            }
            break;
            case VAR_WAITING_TIME:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getWaitingTime(id));
                break;
            case VAR_ACCUMULATED_WAITING_TIME:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getAccumulatedWaitingTime(id));
                break;
            case VAR_EDGE_TRAVELTIME: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires time and edge as parameter.", outputStorage);
                }
                // time
                int time = 0;
                if (!server.readTypeCheckingInt(inputStorage, time)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires the referenced time as first parameter.", outputStorage);
                }
                // edge
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires the referenced edge as second parameter.", outputStorage);
                }
                // retrieve
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getAdaptedTraveltime(id, edgeID, time));
            }
            break;
            case VAR_EDGE_EFFORT: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires time and edge as parameter.", outputStorage);
                }
                // time
                int time = 0;
                if (!server.readTypeCheckingInt(inputStorage, time)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires the referenced time as first parameter.", outputStorage);
                }
                // edge
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires the referenced edge as second parameter.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getEffort(id, edgeID, time));
            }
            break;
            case VAR_ROUTE_VALID: {
                std::string msg;
                tempMsg.writeUnsignedByte(TYPE_UBYTE);
                tempMsg.writeUnsignedByte(libsumo::Vehicle::isRouteValid(id));
            }
            break;
            case VAR_EDGES:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::Vehicle::getEdges(id));
                break;
            case VAR_SIGNALS:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getSignalStates(id));
                break;
            case VAR_BEST_LANES: {
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tcpip::Storage tempContent;
                int cnt = 0;
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                std::vector<libsumo::TraCIBestLanesData> bestLanes = libsumo::Vehicle::getBestLanes(id);
                tempContent.writeInt((int) bestLanes.size());
                ++cnt;
                for (std::vector<libsumo::TraCIBestLanesData>::const_iterator i = bestLanes.begin(); i != bestLanes.end(); ++i) {
                    const libsumo::TraCIBestLanesData& bld = *i;
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(bld.laneID);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_DOUBLE);
                    tempContent.writeDouble(bld.length);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_DOUBLE);
                    tempContent.writeDouble(bld.occupation);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_BYTE);
                    tempContent.writeByte(bld.bestLaneOffset);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_UBYTE);
                    bld.allowsContinuation ? tempContent.writeUnsignedByte(1) : tempContent.writeUnsignedByte(0);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_STRINGLIST);
                    tempContent.writeStringList(bld.continuationLanes);
                    ++cnt;
                }
                tempMsg.writeInt((int) cnt);
                tempMsg.writeStorage(tempContent);
            }
            break;
            case VAR_NEXT_TLS: {
                std::vector<libsumo::TraCINextTLSData> nextTLS = libsumo::Vehicle::getNextTLS(id);
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                const int cnt = 1 + (int)nextTLS.size() * 4;
                tempMsg.writeInt(cnt);
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int)nextTLS.size());
                for (std::vector<libsumo::TraCINextTLSData>::iterator it = nextTLS.begin(); it != nextTLS.end(); ++it) {
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(it->id);
                    tempMsg.writeUnsignedByte(TYPE_INTEGER);
                    tempMsg.writeInt(it->tlIndex);
                    tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                    tempMsg.writeDouble(it->dist);
                    tempMsg.writeUnsignedByte(TYPE_BYTE);
                    tempMsg.writeByte(it->state);
                }
            }
            break;
            case VAR_STOPSTATE:
                tempMsg.writeUnsignedByte(TYPE_UBYTE);
                tempMsg.writeUnsignedByte(libsumo::Vehicle::getStopState(id));
                break;
            case VAR_DISTANCE:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getDistance(id));
                break;
            case DISTANCE_REQUEST: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of distance requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of distance requires position and distance type as parameter.", outputStorage);
                }

                double edgePos;
                std::string roadID;
                int laneIndex;
                // read position
                int posType = inputStorage.readUnsignedByte();
                switch (posType) {
                    case POSITION_ROADMAP:
                        try {
                            std::string roadID = inputStorage.readString();
                            edgePos = inputStorage.readDouble();
                            laneIndex = inputStorage.readUnsignedByte();
                            tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                            tempMsg.writeDouble(libsumo::Vehicle::getDrivingDistance(id, roadID, edgePos, laneIndex));
                            break;
                        } catch (libsumo::TraCIException& e) {
                            return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, e.what(), outputStorage);
                        }
                    case POSITION_2D:
                    case POSITION_3D: {
                        const double p1x = inputStorage.readDouble();
                        const double p1y = inputStorage.readDouble();
                        if (posType == POSITION_3D) {
                            inputStorage.readDouble();        // z value is ignored
                        }
                        tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                        tempMsg.writeDouble(libsumo::Vehicle::getDrivingDistance2D(id, p1x, p1y));
                    }
                    break;
                    default:
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Unknown position format used for distance request", outputStorage);
                }
                // read distance type
                int distType = inputStorage.readUnsignedByte();
                if (distType != REQUEST_DRIVINGDIST) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Only driving distance is supported for vehicles.", outputStorage);
                }
                break;
            }
            case VAR_ALLOWED_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getAllowedSpeed(id));
                break;
            case VAR_SPEED_FACTOR:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getSpeedFactor(id));
                break;
            case VAR_SPEEDSETMODE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getSpeedMode(id));
                break;
            case VAR_LANECHANGE_MODE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getLanechangeMode(id));
                break;
            case VAR_ROUTING_MODE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Vehicle::getRoutingMode(id));
                break;
            case VAR_LINE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Vehicle::getLine(id));
                break;
            case VAR_VIA:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::Vehicle::getVia(id));
                break;
            case CMD_CHANGELANE: {
                int direction = 0;
                if (!server.readTypeCheckingInt(inputStorage, direction)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of lane change state requires a direction as int.", outputStorage);
                }
                const std::pair<int, int> state = libsumo::Vehicle::getLaneChangeState(id, direction);
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tempMsg.writeInt(2);
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(state.first);
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(state.second);
            }
            break;
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Vehicle::getParameter(id, paramName));
            }
            break;
            case VAR_LASTACTIONTIME: {
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::Vehicle::getLastActionTime(id));
            }
            break;
            default:
                TraCIServerAPI_VehicleType::getVariable(variable, libsumo::Vehicle::getVehicleType(id).getID(), tempMsg);
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Vehicle::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != CMD_STOP && variable != CMD_CHANGELANE
            && variable != CMD_CHANGESUBLANE
            && variable != CMD_SLOWDOWN && variable != CMD_CHANGETARGET && variable != CMD_RESUME
            && variable != VAR_TYPE && variable != VAR_ROUTE_ID && variable != VAR_ROUTE
            && variable != VAR_EDGE_TRAVELTIME && variable != VAR_EDGE_EFFORT
            && variable != CMD_REROUTE_TRAVELTIME && variable != CMD_REROUTE_EFFORT
            && variable != VAR_SIGNALS && variable != VAR_MOVE_TO
            && variable != VAR_LENGTH && variable != VAR_MAXSPEED && variable != VAR_VEHICLECLASS
            && variable != VAR_SPEED_FACTOR && variable != VAR_EMISSIONCLASS
            && variable != VAR_WIDTH && variable != VAR_MINGAP && variable != VAR_SHAPECLASS
            && variable != VAR_ACCEL && variable != VAR_DECEL && variable != VAR_IMPERFECTION
            && variable != VAR_APPARENT_DECEL && variable != VAR_EMERGENCY_DECEL
            && variable != VAR_ACTIONSTEPLENGTH
            && variable != VAR_TAU && variable != VAR_LANECHANGE_MODE
            && variable != VAR_SPEED && variable != VAR_SPEEDSETMODE && variable != VAR_COLOR
            && variable != ADD && variable != ADD_FULL && variable != REMOVE
            && variable != VAR_HEIGHT
            && variable != VAR_ROUTING_MODE
            && variable != VAR_LATALIGNMENT
            && variable != VAR_MAXSPEED_LAT
            && variable != VAR_MINGAP_LAT
            && variable != VAR_LINE
            && variable != VAR_VIA
            && variable != MOVE_TO_XY && variable != VAR_PARAMETER/* && variable != VAR_SPEED_TIME_LINE && variable != VAR_LANE_TIME_LINE*/
       ) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Change Vehicle State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
#ifdef DEBUG_MOVEXY
    std::cout << SIMTIME << " processSet veh=" << id << "\n";
#endif
    const bool shouldExist = variable != ADD && variable != ADD_FULL;
    SUMOVehicle* sumoVehicle = MSNet::getInstance()->getVehicleControl().getVehicle(id);
    if (sumoVehicle == 0) {
        if (shouldExist) {
            return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Vehicle '" + id + "' is not known", outputStorage);
        }
    }
    MSVehicle* v = dynamic_cast<MSVehicle*>(sumoVehicle);
    if (v == 0 && shouldExist) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Vehicle '" + id + "' is not a micro-simulation vehicle", outputStorage);
    }
    try {
        switch (variable) {
            case CMD_STOP: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Stop needs a compound object description.", outputStorage);
                }
                int compoundSize = inputStorage.readInt();
                if (compoundSize < 4 || compoundSize > 7) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Stop needs a compound object description of four to seven items.", outputStorage);
                }
                // read road map position
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first stop parameter must be the edge id given as a string.", outputStorage);
                }
                double pos = 0;
                if (!server.readTypeCheckingDouble(inputStorage, pos)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second stop parameter must be the end position along the edge given as a double.", outputStorage);
                }
                int laneIndex = 0;
                if (!server.readTypeCheckingByte(inputStorage, laneIndex)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The third stop parameter must be the lane index given as a byte.", outputStorage);
                }
                // waitTime
                int waitTime = -1;
                if (!server.readTypeCheckingInt(inputStorage, waitTime)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "The fourth stop parameter must be the waiting time given as an integer.", outputStorage);
                }
                int stopFlags = 0;
                if (compoundSize >= 5) {
                    if (!server.readTypeCheckingByte(inputStorage, stopFlags)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The fifth stop parameter must be a byte indicating its parking/triggered status.", outputStorage);
                    }
                }
                double startPos = pos - POSITION_EPS;
                if (compoundSize >= 6) {
                    double tmp;
                    if (!server.readTypeCheckingDouble(inputStorage, tmp)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The sixth stop parameter must be the start position along the edge given as a double.", outputStorage);
                    }
                    if (tmp != INVALID_DOUBLE_VALUE) {
                        startPos = tmp;
                    }
                }
                int until = -1;
                if (compoundSize >= 7) {
                    if (!server.readTypeCheckingInt(inputStorage, until)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The seventh stop parameter must be the minimum departure time given as integer.", outputStorage);
                    }
                }
                libsumo::Vehicle::setStop(id, edgeID, pos, laneIndex, waitTime, stopFlags, startPos, until);
            }
            break;
            case CMD_RESUME: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Resuming requires a compound object.", outputStorage);
                    return false;
                }
                if (inputStorage.readInt() != 0) {
                    server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Resuming should obtain an empty compound object.", outputStorage);
                    return false;
                }
                libsumo::Vehicle::resume(id);
            }
            break;
            case CMD_CHANGELANE: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Lane change needs a compound object description.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Lane change needs a compound object description of two items.", outputStorage);
                }
                // Lane ID
                int laneIndex = 0;
                if (!server.readTypeCheckingByte(inputStorage, laneIndex)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first lane change parameter must be the lane index given as a byte.", outputStorage);
                }
                // duration
                int duration = 0;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second lane change parameter must be the duration given as an integer.", outputStorage);
                }
                if ((laneIndex < 0) || (laneIndex >= (int)(v->getEdge()->getLanes().size()))) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "No lane with index '" + toString(laneIndex) + "' on road '" + v->getEdge()->getID() + "'.", outputStorage);
                }
                libsumo::Vehicle::changeLane(id, laneIndex, duration);
            }
            break;
            case CMD_CHANGESUBLANE: {
                double latDist = 0;
                if (!server.readTypeCheckingDouble(inputStorage, latDist)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Sublane-changing requires a double.", outputStorage);
                }
                libsumo::Vehicle::changeSublane(id, latDist);
            }
            break;
            case CMD_SLOWDOWN: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Slow down needs a compound object description.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Slow down needs a compound object description of two items.", outputStorage);
                }
                double newSpeed = 0;
                if (!server.readTypeCheckingDouble(inputStorage, newSpeed)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first slow down parameter must be the speed given as a double.", outputStorage);
                }
                if (newSpeed < 0) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Speed must not be negative", outputStorage);
                }
                int duration = 0;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second slow down parameter must be the duration given as an integer.", outputStorage);
                }
                if (duration < 0 || MSNet::getInstance()->getCurrentTimeStep() + (SUMOTime)duration > SUMOTime_MAX - DELTA_T) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid time interval", outputStorage);
                }
                libsumo::Vehicle::slowDown(id, newSpeed, (SUMOTime)duration);
            }
            break;
            case CMD_CHANGETARGET: {
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Change target requires a string containing the id of the new destination edge as parameter.", outputStorage);
                }
                libsumo::Vehicle::changeTarget(id, edgeID);
            }
            break;
            case VAR_TYPE: {
                std::string vTypeID;
                if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The vehicle type id must be given as a string.", outputStorage);
                }
                libsumo::Vehicle::setType(id, vTypeID);
            }
            break;
            case VAR_ROUTE_ID: {
                std::string rid;
                if (!server.readTypeCheckingString(inputStorage, rid)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The route id must be given as a string.", outputStorage);
                }
                libsumo::Vehicle::setRouteID(id, rid);
            }
            break;
            case VAR_ROUTE: {
                std::vector<std::string> edgeIDs;
                if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "A route must be defined as a list of edge ids.", outputStorage);
                }
                libsumo::Vehicle::setRoute(id, edgeIDs);
            }
            break;
            case VAR_EDGE_TRAVELTIME: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time requires a compound object.", outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                std::string edgeID;
                int begTime = 0;
                int endTime = -1;
                double value = INVALID_DOUBLE_VALUE;
                if (parameterCount == 4) {
                    // begin time
                    if (!server.readTypeCheckingInt(inputStorage, begTime)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 4 parameters requires the begin time as first parameter.", outputStorage);
                    }
                    // begin time
                    if (!server.readTypeCheckingInt(inputStorage, endTime)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 4 parameters requires the end time as second parameter.", outputStorage);
                    }
                    // edge
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 4 parameters requires the referenced edge as third parameter.", outputStorage);
                    }
                    // value
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 4 parameters requires the travel time as double as fourth parameter.", outputStorage);
                    }
                } else if (parameterCount == 2) {
                    // edge
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 2 parameters requires the referenced edge as first parameter.", outputStorage);
                    }
                    // value
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 2 parameters requires the travel time as second parameter.", outputStorage);
                    }
                } else if (parameterCount == 1) {
                    // edge
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 1 parameter requires the referenced edge as first parameter.", outputStorage);
                    }
                } else {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time requires 1, 2, or 4 parameters.", outputStorage);
                }
                libsumo::Vehicle::setAdaptedTraveltime(id, edgeID, value, (double)begTime, endTime == -1 ? std::numeric_limits<double>::max() : (double)endTime);
            }
            break;
            case VAR_EDGE_EFFORT: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort requires a compound object.", outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                std::string edgeID;
                int begTime = 0;
                int endTime = -1;
                double value = INVALID_DOUBLE_VALUE;
                if (parameterCount == 4) {
                    // begin time
                    if (!server.readTypeCheckingInt(inputStorage, begTime)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 4 parameters requires the begin time as first parameter.", outputStorage);
                    }
                    // begin time
                    if (!server.readTypeCheckingInt(inputStorage, endTime)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 4 parameters requires the end time as second parameter.", outputStorage);
                    }
                    // edge
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 4 parameters requires the referenced edge as third parameter.", outputStorage);
                    }
                    // value
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 4 parameters requires the travel time as fourth parameter.", outputStorage);
                    }
                } else if (parameterCount == 2) {
                    // edge
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 2 parameters requires the referenced edge as first parameter.", outputStorage);
                    }
                    if (!server.readTypeCheckingDouble(inputStorage, value)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 2 parameters requires the travel time as second parameter.", outputStorage);
                    }
                } else if (parameterCount == 1) {
                    // edge
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 1 parameter requires the referenced edge as first parameter.", outputStorage);
                    }
                } else {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort requires 1, 2, or 4 parameters.", outputStorage);
                }
                // retrieve
                libsumo::Vehicle::setEffort(id, edgeID, value, (double)begTime, endTime == -1 ? std::numeric_limits<double>::max() : (double)endTime);
            }
            break;
            case CMD_REROUTE_TRAVELTIME: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Rerouting requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 0) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Rerouting should obtain an empty compound object.", outputStorage);
                }
                libsumo::Vehicle::rerouteTraveltime(id);
            }
            break;
            case CMD_REROUTE_EFFORT: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Rerouting requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 0) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Rerouting should obtain an empty compound object.", outputStorage);
                }
                libsumo::Vehicle::rerouteEffort(id);
            }
            break;
            case VAR_SIGNALS: {
                int signals = 0;
                if (!server.readTypeCheckingInt(inputStorage, signals)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting signals requires an integer.", outputStorage);
                }
                libsumo::Vehicle::setSignals(id, signals);
            }
            break;
            case VAR_MOVE_TO: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting position requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 2) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting position should obtain the lane id and the position.", outputStorage);
                }
                // lane ID
                std::string laneID;
                if (!server.readTypeCheckingString(inputStorage, laneID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first parameter for setting a position must be the lane ID given as a string.", outputStorage);
                }
                // position on lane
                double position = 0;
                if (!server.readTypeCheckingDouble(inputStorage, position)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second parameter for setting a position must be the position given as a double.", outputStorage);
                }
                // process
                libsumo::Vehicle::moveTo(id, laneID, position);
            }
            break;
            case VAR_SPEED: {
                double speed = 0;
                if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting speed requires a double.", outputStorage);
                }
                std::vector<std::pair<SUMOTime, double> > speedTimeLine;
                if (speed >= 0) {
                    speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), speed));
                    speedTimeLine.push_back(std::make_pair(SUMOTime_MAX - DELTA_T, speed));
                }
                v->getInfluencer().setSpeedTimeLine(speedTimeLine);
            }
            break;
            case VAR_SPEEDSETMODE: {
                int speedMode = 0;
                if (!server.readTypeCheckingInt(inputStorage, speedMode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting speed mode requires an integer.", outputStorage);
                }
                v->getInfluencer().setSpeedMode(speedMode);
            }
            break;
            case VAR_LANECHANGE_MODE: {
                int laneChangeMode = 0;
                if (!server.readTypeCheckingInt(inputStorage, laneChangeMode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting lane change mode requires an integer.", outputStorage);
                }
                v->getInfluencer().setLaneChangeMode(laneChangeMode);
            }
            break;
            case VAR_ROUTING_MODE: {
                int routingMode = 0;
                if (!server.readTypeCheckingInt(inputStorage, routingMode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting routing mode requires an integer.", outputStorage);
                }
                v->getInfluencer().setRoutingMode(routingMode);
            }
            break;
            case VAR_COLOR: {
                libsumo::TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The color must be given using the according type.", outputStorage);
                }
                v->getParameter().color.set(col.r, col.g, col.b, col.a);
                v->getParameter().parametersSet |= VEHPARS_COLOR_SET;
            }
            break;
            case ADD: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a vehicle requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 6) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a vehicle needs six parameters.", outputStorage);
                }
                std::string vTypeID;
                if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "First parameter (type) requires a string.", outputStorage);
                }
                std::string routeID;
                if (!server.readTypeCheckingString(inputStorage, routeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Second parameter (route) requires a string.", outputStorage);
                }
                int depart;
                if (!server.readTypeCheckingInt(inputStorage, depart)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Third parameter (depart) requires an integer.", outputStorage);
                }
                double departPos;
                if (!server.readTypeCheckingDouble(inputStorage, departPos)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fourth parameter (position) requires a double.", outputStorage);
                }
                double departSpeed;
                if (!server.readTypeCheckingDouble(inputStorage, departSpeed)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fifth parameter (speed) requires a double.", outputStorage);
                }
                int departLane;
                if (!server.readTypeCheckingByte(inputStorage, departLane)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Sixth parameter (lane) requires a byte.", outputStorage);
                }
                libsumo::Vehicle::add(id, routeID, vTypeID, depart, departLane, departPos, departSpeed);
            }
            break;
            case ADD_FULL: {
                if (v != 0) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The vehicle " + id + " to add already exists.", outputStorage);
                }
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a vehicle requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 14) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a fully specified vehicle needs fourteen parameters.", outputStorage);
                }
                SUMOVehicleParameter vehicleParams;
                vehicleParams.id = id;

                std::string routeID;
                if (!server.readTypeCheckingString(inputStorage, routeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Second parameter (route) requires a string.", outputStorage);
                }
                const MSRoute* route = MSRoute::dictionary(routeID);
                if (!route) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid route '" + routeID + "' for vehicle: '" + id + "'", outputStorage);
                }

                std::string vTypeID;
                if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "First parameter (type) requires a string.", outputStorage);
                }
                MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(vTypeID);
                if (!vehicleType) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid type '" + vTypeID + "' for vehicle '" + id + "'", outputStorage);
                }

                std::string helper;
                std::string error;
                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Third parameter (depart) requires an string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseDepart(helper, "vehicle", id, vehicleParams.depart, vehicleParams.departProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }
                if (vehicleParams.departProcedure == DEPART_GIVEN && vehicleParams.depart < MSNet::getInstance()->getCurrentTimeStep()) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Departure time in the past.", outputStorage);
                }

                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fourth parameter (depart lane) requires a string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseDepartLane(helper, "vehicle", id, vehicleParams.departLane, vehicleParams.departLaneProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fifth parameter (depart position) requires a string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseDepartPos(helper, "vehicle", id, vehicleParams.departPos, vehicleParams.departPosProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Sixth parameter (depart speed) requires a string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseDepartSpeed(helper, "vehicle", id, vehicleParams.departSpeed, vehicleParams.departSpeedProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }

                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Seventh parameter (arrival lane) requires a string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseArrivalLane(helper, "vehicle", id, vehicleParams.arrivalLane, vehicleParams.arrivalLaneProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Eighth parameter (arrival position) requires a string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseArrivalPos(helper, "vehicle", id, vehicleParams.arrivalPos, vehicleParams.arrivalPosProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, helper)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Ninth parameter (arrival speed) requires a string.", outputStorage);
                }
                if (!SUMOVehicleParameter::parseArrivalSpeed(helper, "vehicle", id, vehicleParams.arrivalSpeed, vehicleParams.arrivalSpeedProcedure, error)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, error, outputStorage);
                }

                if (!server.readTypeCheckingString(inputStorage, vehicleParams.fromTaz)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Tenth parameter (from taz) requires a string.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, vehicleParams.toTaz)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Eleventh parameter (to taz) requires a string.", outputStorage);
                }
                if (!server.readTypeCheckingString(inputStorage, vehicleParams.line)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Twelth parameter (line) requires a string.", outputStorage);
                }

                int num;
                if (!server.readTypeCheckingInt(inputStorage, num)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "13th parameter (person capacity) requires an int.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, num)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "14th parameter (person number) requires an int.", outputStorage);
                }
                vehicleParams.personNumber = num;

                SUMOVehicleParameter* params = new SUMOVehicleParameter(vehicleParams);
                try {
                    SUMOVehicle* vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(params, route, vehicleType, true, false);
                    MSNet::getInstance()->getVehicleControl().addVehicle(vehicleParams.id, vehicle);
                    MSNet::getInstance()->getInsertionControl().add(vehicle);
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                }
            }
            break;
            case REMOVE: {
                int why = 0;
                if (!server.readTypeCheckingByte(inputStorage, why)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Removing a vehicle requires a byte.", outputStorage);
                }
                MSMoveReminder::Notification n = MSMoveReminder::NOTIFICATION_ARRIVED;
                switch (why) {
                    case REMOVE_TELEPORT:
                        // XXX semantics unclear
                        // n = MSMoveReminder::NOTIFICATION_TELEPORT;
                        n = MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED;
                        break;
                    case REMOVE_PARKING:
                        // XXX semantics unclear
                        // n = MSMoveReminder::NOTIFICATION_PARKING;
                        n = MSMoveReminder::NOTIFICATION_ARRIVED;
                        break;
                    case REMOVE_ARRIVED:
                        n = MSMoveReminder::NOTIFICATION_ARRIVED;
                        break;
                    case REMOVE_VAPORIZED:
                        n = MSMoveReminder::NOTIFICATION_VAPORIZED;
                        break;
                    case REMOVE_TELEPORT_ARRIVED:
                        n = MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED;
                        break;
                    default:
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Unknown removal status.", outputStorage);
                }
                if (v->hasDeparted()) {
                    v->onRemovalFromNet(n);
                    if (v->getLane() != 0) {
                        v->getLane()->removeVehicle(v, n);
                    }
                    MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(v);
                } else {
                    MSNet::getInstance()->getInsertionControl().alreadyDeparted(v);
                    MSNet::getInstance()->getVehicleControl().deleteVehicle(v, true);
                }
            }
            break;
            case MOVE_TO_XY: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "MoveToXY vehicle requires a compound object.", outputStorage);
                }
                const int numArgs = inputStorage.readInt();
                if (numArgs != 5 && numArgs != 6) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "MoveToXY vehicle should obtain: edgeID, lane, x, y, angle and optionally keepRouteFlag.", outputStorage);
                }
                // edge ID
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first parameter for moveToXY must be the edge ID given as a string.", outputStorage);
                }
                // lane index
                int laneNum = 0;
                if (!server.readTypeCheckingInt(inputStorage, laneNum)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second parameter for moveToXY must be lane given as an int.", outputStorage);
                }
                // x
                double x = 0;
                if (!server.readTypeCheckingDouble(inputStorage, x)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The third parameter for moveToXY must be the x-position given as a double.", outputStorage);
                }
                // y
                double y = 0;
                if (!server.readTypeCheckingDouble(inputStorage, y)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The fourth parameter for moveToXY must be the y-position given as a double.", outputStorage);
                }
                // angle
                double angle = 0;
                if (!server.readTypeCheckingDouble(inputStorage, angle)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The fifth parameter for moveToXY must be the angle given as a double.", outputStorage);
                }

                int keepRouteFlag = 1;
                if (numArgs == 6) {
                    if (!server.readTypeCheckingByte(inputStorage, keepRouteFlag)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The sixth parameter for moveToXY must be the keepRouteFlag given as a byte.", outputStorage);
                    }
                }
                libsumo::Vehicle::moveToXY(id, edgeID, laneNum, x, y, angle, keepRouteFlag);
            }
            break;
            case VAR_SPEED_FACTOR: {
                double factor = 0;
                if (!server.readTypeCheckingDouble(inputStorage, factor)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting speed factor requires a double.", outputStorage);
                }
                v->setChosenSpeedFactor(factor);
            }
            break;
            case VAR_LINE: {
                std::string line;
                if (!server.readTypeCheckingString(inputStorage, line)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The line must be given as a string.", outputStorage);
                }
                v->getParameter().line = line;
            }
            break;
            case VAR_VIA: {
                std::vector<std::string> edgeIDs;
                if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Vias must be defined as a list of edge ids.", outputStorage);
                }
                try {
                    // ensure edges exist
                    ConstMSEdgeVector edges;
                    MSEdge::parseEdgesList(edgeIDs, edges, "<via-edges>");
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                }
                v->getParameter().via = edgeIDs;
            }
            break;
            case VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
                }
                //readt itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
                }
                try {
                    /// XXX but a big try/catch around all retrieval cases
                    libsumo::Vehicle::setParameter(id, name, value);
                } catch (libsumo::TraCIException& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                }
            }
            break;
            case VAR_ACTIONSTEPLENGTH: {
                double value = 0;
                if (!server.readTypeCheckingDouble(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting action step length requires a double.", outputStorage);
                }
                if (fabs(value) == std::numeric_limits<double>::infinity()) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid action step length.", outputStorage);
                }
                bool resetActionOffset = value >= 0.0;
                libsumo::Vehicle::setActionStepLength(id, fabs(value), resetActionOffset);
            }
            break;
            default:
                try {
                    const MSVehicleType& type = v->getSingularType();
                    if (!TraCIServerAPI_VehicleType::setVariable(CMD_SET_VEHICLE_VARIABLE, variable, type.getID(), server, inputStorage, outputStorage)) {
                        return false;
                    }
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                } catch (libsumo::TraCIException& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                }
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}



// ------ helper functions ------
bool
TraCIServerAPI_Vehicle::getPosition(const std::string& id, Position& p) {
    MSVehicle* v = dynamic_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle(id));
    if (v == 0) {
        return false;
    }
    p = v->getPosition();
    return true;
}


#endif


/****************************************************************************/

