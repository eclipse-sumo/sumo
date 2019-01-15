/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
/// @author  Lara Codeca
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
#include <libsumo/VehicleType.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Simulation.h"
#include "TraCIServerAPI_Vehicle.h"
#include "TraCIServerAPI_VehicleType.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Vehicle::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_VEHICLE_VARIABLE, variable, id);
    try {
        if (!libsumo::Vehicle::handleVariable(id, variable, &server) &&
                !libsumo::VehicleType::handleVariable(libsumo::Vehicle::getTypeID(id), variable, &server)) {
            switch (variable) {
                case VAR_LEADER: {
                    double dist = 0;
                    if (!server.readTypeCheckingDouble(inputStorage, dist)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Leader retrieval requires a double.", outputStorage);
                    }
                    std::pair<std::string, double> leaderInfo = libsumo::Vehicle::getLeader(id, dist);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    server.getWrapperStorage().writeInt(2);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(leaderInfo.first);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                    server.getWrapperStorage().writeDouble(leaderInfo.second);
                    break;
                }
                case VAR_EDGE_TRAVELTIME: {
                    if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires a compound object.", outputStorage);
                    }
                    if (inputStorage.readInt() != 2) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires time and edge as parameter.", outputStorage);
                    }
                    double time = 0.;
                    if (!server.readTypeCheckingDouble(inputStorage, time)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires the referenced time as first parameter.", outputStorage);
                    }
                    // edge
                    std::string edgeID;
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of travel time requires the referenced edge as second parameter.", outputStorage);
                    }
                    // retrieve
                    server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                    server.getWrapperStorage().writeDouble(libsumo::Vehicle::getAdaptedTraveltime(id, time, edgeID));
                    break;
                }
                case VAR_EDGE_EFFORT: {
                    if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires a compound object.", outputStorage);
                    }
                    if (inputStorage.readInt() != 2) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires time and edge as parameter.", outputStorage);
                    }
                    double time = 0.;
                    if (!server.readTypeCheckingDouble(inputStorage, time)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires the referenced time as first parameter.", outputStorage);
                    }
                    // edge
                    std::string edgeID;
                    if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of effort requires the referenced edge as second parameter.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                    server.getWrapperStorage().writeDouble(libsumo::Vehicle::getEffort(id, time, edgeID));
                    break;
                }
                case VAR_BEST_LANES: {
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    std::vector<libsumo::TraCIBestLanesData> bestLanes = libsumo::Vehicle::getBestLanes(id);
                    tempContent.writeInt((int)bestLanes.size());
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
                    server.getWrapperStorage().writeInt((int)cnt);
                    server.getWrapperStorage().writeStorage(tempContent);
                    break;
                }
                case VAR_NEXT_TLS: {
                    std::vector<libsumo::TraCINextTLSData> nextTLS = libsumo::Vehicle::getNextTLS(id);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    const int cnt = 1 + (int)nextTLS.size() * 4;
                    server.getWrapperStorage().writeInt(cnt);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                    server.getWrapperStorage().writeInt((int)nextTLS.size());
                    for (std::vector<libsumo::TraCINextTLSData>::iterator it = nextTLS.begin(); it != nextTLS.end(); ++it) {
                        server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                        server.getWrapperStorage().writeString(it->id);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                        server.getWrapperStorage().writeInt(it->tlIndex);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                        server.getWrapperStorage().writeDouble(it->dist);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_BYTE);
                        server.getWrapperStorage().writeByte(it->state);
                    }
                    break;
                }
                case VAR_NEXT_STOPS: {
                    std::vector<libsumo::TraCINextStopData> nextStops = libsumo::Vehicle::getNextStops(id);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    const int cnt = 1 + (int)nextStops.size() * 4;
                    server.getWrapperStorage().writeInt(cnt);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                    server.getWrapperStorage().writeInt((int)nextStops.size());
                    for (std::vector<libsumo::TraCINextStopData>::iterator it = nextStops.begin(); it != nextStops.end(); ++it) {
                        server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                        server.getWrapperStorage().writeString(it->lane);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                        server.getWrapperStorage().writeDouble(it->endPos);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                        server.getWrapperStorage().writeString(it->stoppingPlaceID);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                        server.getWrapperStorage().writeInt(it->stopFlags);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                        server.getWrapperStorage().writeDouble(it->duration);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                        server.getWrapperStorage().writeDouble(it->until);
                    }
                    break;
                }
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
                                server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                                server.getWrapperStorage().writeDouble(libsumo::Vehicle::getDrivingDistance(id, roadID, edgePos, laneIndex));
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
                            server.getWrapperStorage().writeUnsignedByte(TYPE_DOUBLE);
                            server.getWrapperStorage().writeDouble(libsumo::Vehicle::getDrivingDistance2D(id, p1x, p1y));
                            break;
                        }
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
                case CMD_CHANGELANE: {
                    int direction = 0;
                    if (!server.readTypeCheckingInt(inputStorage, direction)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of lane change state requires a direction as int.", outputStorage);
                    }
                    const std::pair<int, int> state = libsumo::Vehicle::getLaneChangeState(id, direction);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    server.getWrapperStorage().writeInt(2);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                    server.getWrapperStorage().writeInt(state.first);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                    server.getWrapperStorage().writeInt(state.second);
                    break;
                }
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(libsumo::Vehicle::getParameter(id, paramName));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "Get Vehicle Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Vehicle::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != CMD_STOP && variable != CMD_CHANGELANE
            && variable != CMD_REROUTE_TO_PARKING
            && variable != CMD_CHANGESUBLANE && variable != CMD_OPENGAP
            && variable != CMD_SLOWDOWN && variable != CMD_CHANGETARGET && variable != CMD_RESUME
            && variable != VAR_TYPE && variable != VAR_ROUTE_ID && variable != VAR_ROUTE
            && variable != VAR_UPDATE_BESTLANES
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
    if (sumoVehicle == nullptr) {
        if (shouldExist) {
            return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Vehicle '" + id + "' is not known", outputStorage);
        }
    }
    MSVehicle* v = dynamic_cast<MSVehicle*>(sumoVehicle);
    if (v == nullptr && shouldExist) {
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
                double duration = INVALID_DOUBLE_VALUE;
                if (!server.readTypeCheckingDouble(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_GET_VEHICLE_VARIABLE, "The fourth stop parameter must be the stopping duration given as a double.", outputStorage);
                }
                int stopFlags = 0;
                if (compoundSize >= 5) {
                    if (!server.readTypeCheckingByte(inputStorage, stopFlags)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The fifth stop parameter must be a byte indicating its parking/triggered status.", outputStorage);
                    }
                }
                double startPos = INVALID_DOUBLE_VALUE;
                if (compoundSize >= 6) {
                    if (!server.readTypeCheckingDouble(inputStorage, startPos)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The sixth stop parameter must be the start position along the edge given as a double.", outputStorage);
                    }
                }
                double until = INVALID_DOUBLE_VALUE;
                if (compoundSize >= 7) {
                    if (!server.readTypeCheckingDouble(inputStorage, until)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The seventh stop parameter must be the minimum departure time given as a double.", outputStorage);
                    }
                }
                libsumo::Vehicle::setStop(id, edgeID, pos, laneIndex, duration, stopFlags, startPos, until);
            }
            break;
            case CMD_REROUTE_TO_PARKING: {
                // read variables
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Reroute to stop needs a compound object description.", outputStorage);
                }
                int compoundSize = inputStorage.readInt();
                if (compoundSize != 1) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Reroute to stop needs a compound object description of 1 item.", outputStorage);
                }
                std::string parkingAreaID;
                if (!server.readTypeCheckingString(inputStorage, parkingAreaID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first reroute to stop parameter must be the parking area id given as a string.", outputStorage);
                }
                libsumo::Vehicle::rerouteParkingArea(id, parkingAreaID);
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
                int compounds = inputStorage.readInt();
                if (compounds != 3 && compounds != 2) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Lane change needs a compound object description of two or three items.", outputStorage);
                }
                // Lane ID
                int laneIndex = 0;
                if (!server.readTypeCheckingByte(inputStorage, laneIndex)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first lane change parameter must be the lane index given as a byte.", outputStorage);
                }
                // duration
                double duration = 0.;
                if (!server.readTypeCheckingDouble(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second lane change parameter must be the duration given as a double.", outputStorage);
                }
                // relativelanechange
                int relative = 0;
                if (compounds == 3) {
                    if (!server.readTypeCheckingByte(inputStorage, relative)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The third lane change parameter must be a Byte for defining whether a relative lane change should be applied.", outputStorage);
                    }
                }

                if ((laneIndex < 0) || (laneIndex >= (int)(v->getEdge()->getLanes().size()) && relative < 1)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "No lane with index '" + toString(laneIndex) + "' on road '" + v->getEdge()->getID() + "'.", outputStorage);
                }

                if (relative < 1) {
                    libsumo::Vehicle::changeLane(id, laneIndex, duration);
                } else {
                    libsumo::Vehicle::changeLaneRelative(id, laneIndex, duration);
                }
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
                double duration = 0.;
                if (!server.readTypeCheckingDouble(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second slow down parameter must be the duration given as a double.", outputStorage);
                }
                if (duration < 0 || SIMTIME + duration > STEPS2TIME(SUMOTime_MAX - DELTA_T)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid time interval", outputStorage);
                }
                libsumo::Vehicle::slowDown(id, newSpeed, duration);
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
            case CMD_OPENGAP: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Create gap needs a compound object description.", outputStorage);
                }
                const int nParameter = inputStorage.readInt();
                if (nParameter != 4 && nParameter != 5) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Create gap needs a compound object description of four or five items.", outputStorage);
                }
                double newTimeHeadway = 0;
                if (!server.readTypeCheckingDouble(inputStorage, newTimeHeadway)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first create gap parameter must be the new desired time headway (tau) given as a double.", outputStorage);
                }
                double newSpaceHeadway = 0;
                if (!server.readTypeCheckingDouble(inputStorage, newSpaceHeadway)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The second create gap parameter must be the new desired space headway given as a double.", outputStorage);
                }
                double duration = 0.;
                if (!server.readTypeCheckingDouble(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The third create gap parameter must be the duration given as a double.", outputStorage);
                }
                double changeRate = 0;
                if (!server.readTypeCheckingDouble(inputStorage, changeRate)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The fourth create gap parameter must be the change rate given as a double.", outputStorage);
                }

                if (newTimeHeadway == -1 && newSpaceHeadway == -1 && duration == -1 && changeRate == -1) {
                    libsumo::Vehicle::deactivateGapControl(id);
                } else {
                    if (newTimeHeadway <= 0) {
                        if (newTimeHeadway != -1) {
                            return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The value for the new desired time headway (tau) must be positive for create gap", outputStorage);
                        } // else if == -1: keep vehicles current headway, see libsumo::Vehicle::openGap
                    }
                    if (newSpaceHeadway < 0) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The value for the new desired space headway must be non-negative for create gap", outputStorage);
                    }
                    if ((duration < 0 && duration != -1)  || SIMTIME + duration > STEPS2TIME(SUMOTime_MAX - DELTA_T)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid time interval for create gap", outputStorage);
                    }
                    if (changeRate <= 0) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The value for the change rate must be positive for create gap", outputStorage);
                    }
                    double maxDecel = -1;
                    if (nParameter == 5) {
                        if (!server.readTypeCheckingDouble(inputStorage, maxDecel)) {
                            return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The fifth create gap parameter must be the maximal deceleration given as a double.", outputStorage);
                        }
                        if (changeRate <= 0) {
                            return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The value for the maximal deceleration must be positive for create gap", outputStorage);
                        }
                    }
                    libsumo::Vehicle::openGap(id, newTimeHeadway, newSpaceHeadway, duration, changeRate, maxDecel);
                }
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
                double begTime = 0.;
                double endTime = std::numeric_limits<double>::max();
                double value = INVALID_DOUBLE_VALUE;
                if (parameterCount == 4) {
                    // begin time
                    if (!server.readTypeCheckingDouble(inputStorage, begTime)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting travel time using 4 parameters requires the begin time as first parameter.", outputStorage);
                    }
                    // begin time
                    if (!server.readTypeCheckingDouble(inputStorage, endTime)) {
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
                libsumo::Vehicle::setAdaptedTraveltime(id, edgeID, value, begTime, endTime);
            }
            break;
            case VAR_EDGE_EFFORT: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort requires a compound object.", outputStorage);
                }
                int parameterCount = inputStorage.readInt();
                std::string edgeID;
                double begTime = 0.;
                double endTime = std::numeric_limits<double>::max();
                double value = INVALID_DOUBLE_VALUE;
                if (parameterCount == 4) {
                    // begin time
                    if (!server.readTypeCheckingDouble(inputStorage, begTime)) {
                        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting effort using 4 parameters requires the begin time as first parameter.", outputStorage);
                    }
                    // begin time
                    if (!server.readTypeCheckingDouble(inputStorage, endTime)) {
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
                libsumo::Vehicle::setEffort(id, edgeID, value, begTime, endTime);
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
                libsumo::Vehicle::setSpeed(id, speed);
            }
            break;
            case VAR_SPEEDSETMODE: {
                int speedMode = 0;
                if (!server.readTypeCheckingInt(inputStorage, speedMode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting speed mode requires an integer.", outputStorage);
                }
                libsumo::Vehicle::setSpeedMode(id, speedMode);
            }
            break;
            case VAR_LANECHANGE_MODE: {
                int laneChangeMode = 0;
                if (!server.readTypeCheckingInt(inputStorage, laneChangeMode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting lane change mode requires an integer.", outputStorage);
                }
                libsumo::Vehicle::setLaneChangeMode(id, laneChangeMode);
            }
            break;
            case VAR_ROUTING_MODE: {
                int routingMode = 0;
                if (!server.readTypeCheckingInt(inputStorage, routingMode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Setting routing mode requires an integer.", outputStorage);
                }
                libsumo::Vehicle::setRoutingMode(id, routingMode);
            }
            break;
            case VAR_COLOR: {
                libsumo::TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The color must be given using the according type.", outputStorage);
                }
                libsumo::Vehicle::setColor(id, col);
                break;
            }
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
                int departCode;
                if (!server.readTypeCheckingInt(inputStorage, departCode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Third parameter (depart) requires an integer.", outputStorage);
                }
                std::string depart = toString(STEPS2TIME(departCode));
                if (-departCode == DEPART_TRIGGERED) {
                    depart = "triggered";
                } else if (-departCode == DEPART_CONTAINER_TRIGGERED) {
                    depart = "containerTriggered";
                } else if (-departCode == DEPART_NOW) {
                    depart = "now";
                }

                double departPosCode;
                if (!server.readTypeCheckingDouble(inputStorage, departPosCode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fourth parameter (position) requires a double.", outputStorage);
                }
                std::string departPos = toString(departPosCode);
                if (-departPosCode == DEPART_POS_RANDOM) {
                    departPos = "random";
                } else if (-departPosCode == DEPART_POS_RANDOM_FREE) {
                    departPos = "random_free";
                } else if (-departPosCode == DEPART_POS_FREE) {
                    departPos = "free";
                } else if (-departPosCode == DEPART_POS_BASE) {
                    departPos = "base";
                } else if (-departPosCode == DEPART_POS_LAST) {
                    departPos = "last";
                } else if (-departPosCode == DEPART_POS_GIVEN) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Invalid departure position.", outputStorage);
                }

                double departSpeedCode;
                if (!server.readTypeCheckingDouble(inputStorage, departSpeedCode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fifth parameter (speed) requires a double.", outputStorage);
                }
                std::string departSpeed = toString(departSpeedCode);
                if (-departSpeedCode == DEPART_SPEED_RANDOM) {
                    departSpeed = "random";
                } else if (-departSpeedCode == DEPART_SPEED_MAX) {
                    departSpeed = "max";
                }

                int departLaneCode;
                if (!server.readTypeCheckingByte(inputStorage, departLaneCode)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Sixth parameter (lane) requires a byte.", outputStorage);
                }
                std::string departLane = toString(departLaneCode);
                if (-departLaneCode == DEPART_LANE_RANDOM) {
                    departLane = "random";
                } else if (-departLaneCode == DEPART_LANE_FREE) {
                    departLane = "free";
                } else if (-departLaneCode == DEPART_LANE_ALLOWED_FREE) {
                    departLane = "allowed";
                } else if (-departLaneCode == DEPART_LANE_BEST_FREE) {
                    departLane = "best";
                } else if (-departLaneCode == DEPART_LANE_FIRST_ALLOWED) {
                    departLane = "first";
                }
                libsumo::Vehicle::add(id, routeID, vTypeID, depart, departLane, departPos, departSpeed);
            }
            break;
            case ADD_FULL: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a vehicle requires a compound object.", outputStorage);
                }
                if (inputStorage.readInt() != 14) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Adding a fully specified vehicle needs fourteen parameters.", outputStorage);
                }
                std::string routeID;
                if (!server.readTypeCheckingString(inputStorage, routeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Second parameter (route) requires a string.", outputStorage);
                }
                std::string vTypeID;
                if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "First parameter (type) requires a string.", outputStorage);
                }
                std::string depart;
                if (!server.readTypeCheckingString(inputStorage, depart)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Third parameter (depart) requires an string.", outputStorage);
                }
                std::string departLane;
                if (!server.readTypeCheckingString(inputStorage, departLane)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fourth parameter (depart lane) requires a string.", outputStorage);
                }
                std::string departPos;
                if (!server.readTypeCheckingString(inputStorage, departPos)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Fifth parameter (depart position) requires a string.", outputStorage);
                }
                std::string departSpeed;
                if (!server.readTypeCheckingString(inputStorage, departSpeed)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Sixth parameter (depart speed) requires a string.", outputStorage);
                }
                std::string arrivalLane;
                if (!server.readTypeCheckingString(inputStorage, arrivalLane)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Seventh parameter (arrival lane) requires a string.", outputStorage);
                }
                std::string arrivalPos;
                if (!server.readTypeCheckingString(inputStorage, arrivalPos)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Eighth parameter (arrival position) requires a string.", outputStorage);
                }
                std::string arrivalSpeed;
                if (!server.readTypeCheckingString(inputStorage, arrivalSpeed)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Ninth parameter (arrival speed) requires a string.", outputStorage);
                }
                std::string fromTaz;
                if (!server.readTypeCheckingString(inputStorage, fromTaz)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Tenth parameter (from taz) requires a string.", outputStorage);
                }
                std::string toTaz;
                if (!server.readTypeCheckingString(inputStorage, toTaz)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Eleventh parameter (to taz) requires a string.", outputStorage);
                }
                std::string line;
                if (!server.readTypeCheckingString(inputStorage, line)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Twelth parameter (line) requires a string.", outputStorage);
                }
                int personCapacity;
                if (!server.readTypeCheckingInt(inputStorage, personCapacity)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "13th parameter (person capacity) requires an int.", outputStorage);
                }
                int personNumber;
                if (!server.readTypeCheckingInt(inputStorage, personNumber)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "14th parameter (person number) requires an int.", outputStorage);
                }
                libsumo::Vehicle::add(id, routeID, vTypeID, depart, departLane, departPos, departSpeed, arrivalLane, arrivalPos, arrivalSpeed,
                                      fromTaz, toTaz, line, personCapacity, personNumber);
            }
            break;
            case REMOVE: {
                int why = 0;
                if (!server.readTypeCheckingByte(inputStorage, why)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Removing a vehicle requires a byte.", outputStorage);
                }
                libsumo::Vehicle::remove(id, (char)why);
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
                libsumo::Vehicle::setSpeedFactor(id, factor);
            }
            break;
            case VAR_LINE: {
                std::string line;
                if (!server.readTypeCheckingString(inputStorage, line)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The line must be given as a string.", outputStorage);
                }
                libsumo::Vehicle::setLine(id, line);
            }
            break;
            case VAR_VIA: {
                std::vector<std::string> edgeIDs;
                if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Vias must be defined as a list of edge ids.", outputStorage);
                }
                libsumo::Vehicle::setVia(id, edgeIDs);
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
            case VAR_UPDATE_BESTLANES: {
                libsumo::Vehicle::updateBestLanes(id);
            }
            break;
            default: {
                try {
                    if (!TraCIServerAPI_VehicleType::setVariable(CMD_SET_VEHICLE_VARIABLE, variable, v->getSingularType().getID(), server, inputStorage, outputStorage)) {
                        return false;
                    }
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                } catch (libsumo::TraCIException& e) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                }
            }
            break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
