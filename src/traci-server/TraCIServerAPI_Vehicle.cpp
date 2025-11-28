/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_Vehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @author  Bjoern Hendriks
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @author  Robert Hilbrich
/// @author  Lara Codeca
/// @author  Mirko Barthauer
/// @date    07.05.2009
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/
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
#include <utils/router/DijkstraRouter.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/Vehicle.h>
#include <libsumo/VehicleType.h>
#include "TraCIServerAPI_Simulation.h"
#include "TraCIServerAPI_Vehicle.h"
#include "TraCIServerAPI_VehicleType.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Vehicle::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::CMD_STOP && variable != libsumo::CMD_CHANGELANE
            && variable != libsumo::CMD_REROUTE_TO_PARKING
            && variable != libsumo::CMD_CHANGESUBLANE && variable != libsumo::CMD_OPENGAP
            && variable != libsumo::CMD_REPLACE_STOP
            && variable != libsumo::CMD_INSERT_STOP
            && variable != libsumo::VAR_STOP_PARAMETER
            && variable != libsumo::CMD_SLOWDOWN && variable != libsumo::CMD_CHANGETARGET && variable != libsumo::CMD_RESUME
            && variable != libsumo::VAR_TYPE && variable != libsumo::VAR_ROUTE_ID && variable != libsumo::VAR_ROUTE
            && variable != libsumo::VAR_LANEPOSITION_LAT
            && variable != libsumo::VAR_UPDATE_BESTLANES
            && variable != libsumo::VAR_EDGE_TRAVELTIME && variable != libsumo::VAR_EDGE_EFFORT
            && variable != libsumo::CMD_REROUTE_TRAVELTIME && variable != libsumo::CMD_REROUTE_EFFORT
            && variable != libsumo::VAR_SIGNALS && variable != libsumo::VAR_MOVE_TO
            && variable != libsumo::VAR_LENGTH && variable != libsumo::VAR_MAXSPEED && variable != libsumo::VAR_VEHICLECLASS
            && variable != libsumo::VAR_SPEED_FACTOR && variable != libsumo::VAR_EMISSIONCLASS
            && variable != libsumo::VAR_WIDTH && variable != libsumo::VAR_MINGAP && variable != libsumo::VAR_SHAPECLASS
            && variable != libsumo::VAR_ACCEL && variable != libsumo::VAR_DECEL && variable != libsumo::VAR_IMPERFECTION
            && variable != libsumo::VAR_APPARENT_DECEL && variable != libsumo::VAR_EMERGENCY_DECEL
            && variable != libsumo::VAR_ACTIONSTEPLENGTH
            && variable != libsumo::VAR_TAU && variable != libsumo::VAR_LANECHANGE_MODE
            && variable != libsumo::VAR_SPEED && variable != libsumo::VAR_ACCELERATION && variable != libsumo::VAR_PREV_SPEED && variable != libsumo::VAR_SPEEDSETMODE && variable != libsumo::VAR_COLOR
            && variable != libsumo::ADD && variable != libsumo::ADD_FULL && variable != libsumo::REMOVE
            && variable != libsumo::VAR_HEIGHT
            && variable != libsumo::VAR_MASS
            && variable != libsumo::VAR_ROUTING_MODE
            && variable != libsumo::VAR_LATALIGNMENT
            && variable != libsumo::VAR_MAXSPEED_LAT
            && variable != libsumo::VAR_MINGAP_LAT
            && variable != libsumo::VAR_LINE
            && variable != libsumo::VAR_VIA
            && variable != libsumo::VAR_IMPATIENCE
            && variable != libsumo::VAR_BOARDING_DURATION
            && variable != libsumo::VAR_HIGHLIGHT
            && variable != libsumo::CMD_TAXI_DISPATCH
            && variable != libsumo::MOVE_TO_XY && variable != libsumo::VAR_PARAMETER/* && variable != libsumo::VAR_SPEED_TIME_LINE && variable != libsumo::VAR_LANE_TIME_LINE*/
       ) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Change Vehicle State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
#ifdef DEBUG_MOVEXY
    std::cout << SIMTIME << " processSet veh=" << id << "\n";
#endif
    const bool shouldExist = variable != libsumo::ADD && variable != libsumo::ADD_FULL;
    SUMOVehicle* sumoVehicle = MSNet::getInstance()->getVehicleControl().getVehicle(id);
    if (sumoVehicle == nullptr) {
        if (shouldExist) {
            return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Vehicle '" + id + "' is not known", outputStorage);
        }
    }
    MSBaseVehicle* v = dynamic_cast<MSBaseVehicle*>(sumoVehicle);
    if (v == nullptr && shouldExist) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_VEHICLE_VARIABLE, "Vehicle '" + id + "' is not a proper vehicle", outputStorage);
    }
    try {
        switch (variable) {
            case libsumo::CMD_STOP: {
                const int compoundSize = StoHelp::readCompound(inputStorage, -1, "Stop needs a compound object description.");
                if (compoundSize < 4 || compoundSize > 7) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Stop needs a compound object description of four to seven items.", outputStorage);
                }
                // read road map position
                const std::string edgeID = StoHelp::readTypedString(inputStorage, "The first stop parameter must be the edge id given as a string.");
                const double pos = StoHelp::readTypedDouble(inputStorage, "The second stop parameter must be the end position along the edge given as a double.");
                const int laneIndex = StoHelp::readTypedByte(inputStorage, "The third stop parameter must be the lane index given as a byte.");
                const double duration = StoHelp::readTypedDouble(inputStorage, "The fourth stop parameter must be the stopping duration given as a double.");
                int stopFlags = 0;
                if (compoundSize >= 5) {
                    stopFlags = StoHelp::readTypedByte(inputStorage, "The fifth stop parameter must be a byte indicating its parking/triggered status.");
                }
                double startPos = libsumo::INVALID_DOUBLE_VALUE;
                if (compoundSize >= 6) {
                    startPos = StoHelp::readTypedDouble(inputStorage, "The sixth stop parameter must be the start position along the edge given as a double.");
                }
                double until = libsumo::INVALID_DOUBLE_VALUE;
                if (compoundSize >= 7) {
                    until = StoHelp::readTypedDouble(inputStorage, "The seventh stop parameter must be the minimum departure time given as a double.");
                }
                libsumo::Vehicle::setStop(id, edgeID, pos, laneIndex, duration, stopFlags, startPos, until);
            }
            break;
            case libsumo::CMD_REPLACE_STOP:
                if (!insertReplaceStop(server, inputStorage, outputStorage, id, true)) {
                    return false;
                }
                break;
            case libsumo::CMD_INSERT_STOP:
                if (!insertReplaceStop(server, inputStorage, outputStorage, id, false)) {
                    return false;
                }
                break;
            case libsumo::VAR_STOP_PARAMETER: {
                const int compoundSize = StoHelp::readCompound(inputStorage, -1, "Setting stop parameter needs a compound object description.");
                if (compoundSize != 3 && compoundSize != 4) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Setting a stop parameter needs a compound object description of 3 or 4 items.", outputStorage);
                }
                const int nextStopIndex = StoHelp::readTypedInt(inputStorage, "The first setStopParameter parameter must be the nextStopIndex given as an integer.");
                const std::string param = StoHelp::readTypedString(inputStorage, "The second setStopParameter parameter must be the param given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The third setStopParameter parameter must be the value given as a string.");
                int customParam = 0;
                if (compoundSize == 4) {
                    customParam = StoHelp::readTypedByte(inputStorage, "The fourth setStopParameter parameter must be the customParam flag given as a byte.");
                }
                libsumo::Vehicle::setStopParameter(id, nextStopIndex, param, value, customParam != 0);
            }
            break;
            case libsumo::CMD_REROUTE_TO_PARKING: {
                StoHelp::readCompound(inputStorage, 1, "Reroute to stop needs a compound object description of 1 item.");
                libsumo::Vehicle::rerouteParkingArea(id, StoHelp::readTypedString(inputStorage, "The first reroute to stop parameter must be the parking area id given as a string."));
            }
            break;
            case libsumo::CMD_RESUME: {
                StoHelp::readCompound(inputStorage, 0, "Resuming requires an empty compound object.");
                libsumo::Vehicle::resume(id);
            }
            break;
            case libsumo::CMD_CHANGELANE: {
                const int compoundSize = StoHelp::readCompound(inputStorage, -1, "Lane change needs a compound object description.");
                if (compoundSize != 3 && compoundSize != 2) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Lane change needs a compound object description of two or three items.", outputStorage);
                }
                const int laneIndex = StoHelp::readTypedByte(inputStorage, "The first lane change parameter must be the lane index given as a byte.");
                const double duration = StoHelp::readTypedDouble(inputStorage, "The second lane change parameter must be the duration given as a double.");
                // relative lane change
                int relative = 0;
                if (compoundSize == 3) {
                    relative = StoHelp::readTypedByte(inputStorage, "The third lane change parameter must be a Byte for defining whether a relative lane change should be applied.");
                }

                if ((laneIndex < 0 || laneIndex >= (int)v->getEdge()->getLanes().size()) && relative < 1) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "No lane with index '" + toString(laneIndex) + "' on road '" + v->getEdge()->getID() + "'.", outputStorage);
                }

                if (relative < 1) {
                    libsumo::Vehicle::changeLane(id, laneIndex, duration);
                } else {
                    libsumo::Vehicle::changeLaneRelative(id, laneIndex, duration);
                }
            }
            break;
            case libsumo::CMD_CHANGESUBLANE: {
                libsumo::Vehicle::changeSublane(id, StoHelp::readTypedDouble(inputStorage, "Sublane-changing requires a double."));
            }
            break;
            case libsumo::CMD_SLOWDOWN: {
                StoHelp::readCompound(inputStorage, 2, "Slow down needs a compound object description of two items.");
                const double newSpeed = StoHelp::readTypedDouble(inputStorage, "The first slow down parameter must be the speed given as a double.");
                if (newSpeed < 0.) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Speed must not be negative", outputStorage);
                }
                const double duration = StoHelp::readTypedDouble(inputStorage, "The second slow down parameter must be the duration given as a double.");
                if (duration < 0. || SIMTIME + duration > STEPS2TIME(SUMOTime_MAX - DELTA_T)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Invalid time interval", outputStorage);
                }
                libsumo::Vehicle::slowDown(id, newSpeed, duration);
            }
            break;
            case libsumo::CMD_CHANGETARGET: {
                libsumo::Vehicle::changeTarget(id, StoHelp::readTypedString(inputStorage, "Change target requires a string containing the id of the new destination edge as parameter."));
            }
            break;
            case libsumo::CMD_OPENGAP: {
                const int compoundSize = StoHelp::readCompound(inputStorage, -1, "Create gap needs a compound object description.");
                if (compoundSize != 5 && compoundSize != 6) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Create gap needs a compound object description of five or six items.", outputStorage);
                }
                const double newTimeHeadway = StoHelp::readTypedDouble(inputStorage, "The first create gap parameter must be the new desired time headway (tau) given as a double.");
                double newSpaceHeadway = StoHelp::readTypedDouble(inputStorage, "The second create gap parameter must be the new desired space headway given as a double.");
                const double duration = StoHelp::readTypedDouble(inputStorage, "The third create gap parameter must be the duration given as a double.");
                const double changeRate = StoHelp::readTypedDouble(inputStorage, "The fourth create gap parameter must be the change rate given as a double.");
                const double maxDecel = StoHelp::readTypedDouble(inputStorage, "The fifth create gap parameter must be the maximal braking rate given as a double.");

                if (newTimeHeadway == -1 && newSpaceHeadway == -1 && duration == -1 && changeRate == -1 && maxDecel == -1) {
                    libsumo::Vehicle::deactivateGapControl(id);
                } else {
                    if (newTimeHeadway <= 0) {
                        if (newTimeHeadway != -1) {
                            return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "The value for the new desired time headway (tau) must be positive for create gap", outputStorage);
                        } // else if == -1: keep vehicles current headway, see libsumo::Vehicle::openGap
                    }
                    if (newSpaceHeadway < 0) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "The value for the new desired space headway must be non-negative for create gap", outputStorage);
                    }
                    if ((duration < 0 && duration != -1)  || SIMTIME + duration > STEPS2TIME(SUMOTime_MAX - DELTA_T)) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Invalid time interval for create gap", outputStorage);
                    }
                    if (changeRate <= 0) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "The value for the change rate must be positive for the openGap command", outputStorage);
                    }
                    if (maxDecel <= 0 && maxDecel != -1 && maxDecel != libsumo::INVALID_DOUBLE_VALUE) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "The value for the maximal braking rate must be positive for the openGap command", outputStorage);
                    } // else if <= 0: don't limit cf model's suggested brake rate, see libsumo::Vehicle::openGap
                    std::string refVehID = "";
                    if (compoundSize == 6) {
                        refVehID = StoHelp::readTypedString(inputStorage, "The sixth create gap parameter must be a reference vehicle's ID given as a string.");
                    }
                    libsumo::Vehicle::openGap(id, newTimeHeadway, newSpaceHeadway, duration, changeRate, maxDecel, refVehID);
                }
            }
            break;
            case libsumo::VAR_TYPE: {
                libsumo::Vehicle::setType(id, StoHelp::readTypedString(inputStorage, "The vehicle type id must be given as a string."));
            }
            break;
            case libsumo::VAR_ROUTE_ID: {
                libsumo::Vehicle::setRouteID(id, StoHelp::readTypedString(inputStorage, "The route id must be given as a string."));
            }
            break;
            case libsumo::VAR_ROUTE: {
                libsumo::Vehicle::setRoute(id, StoHelp::readTypedStringList(inputStorage, "A route must be defined as a list of edge ids."));
            }
            break;
            case libsumo::VAR_EDGE_TRAVELTIME: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Setting travel time requires a compound object.");
                std::string edgeID;
                double begTime = 0.;
                double endTime = std::numeric_limits<double>::max();
                double value = libsumo::INVALID_DOUBLE_VALUE;
                if (parameterCount == 4) {
                    begTime = StoHelp::readTypedDouble(inputStorage, "Setting travel time using 4 parameters requires the begin time as first parameter.");
                    endTime = StoHelp::readTypedDouble(inputStorage, "Setting travel time using 4 parameters requires the end time as second parameter.");
                    edgeID = StoHelp::readTypedString(inputStorage, "Setting travel time using 4 parameters requires the referenced edge as third parameter.");
                    value = StoHelp::readTypedDouble(inputStorage, "Setting travel time using 4 parameters requires the travel time as double as fourth parameter.");
                } else if (parameterCount == 2) {
                    edgeID = StoHelp::readTypedString(inputStorage, "Setting travel time using 2 parameters requires the referenced edge as first parameter.");
                    value = StoHelp::readTypedDouble(inputStorage, "Setting travel time using 2 parameters requires the travel time as double as second parameter.");
                } else if (parameterCount == 1) {
                    edgeID = StoHelp::readTypedString(inputStorage, "Setting travel time using 1 parameter requires the referenced edge as first parameter.");
                } else {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Setting travel time requires 1, 2, or 4 parameters.", outputStorage);
                }
                libsumo::Vehicle::setAdaptedTraveltime(id, edgeID, value, begTime, endTime);
            }
            break;
            case libsumo::VAR_EDGE_EFFORT: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Setting travel time requires a compound object.");
                std::string edgeID;
                double begTime = 0.;
                double endTime = std::numeric_limits<double>::max();
                double value = libsumo::INVALID_DOUBLE_VALUE;
                if (parameterCount == 4) {
                    begTime = StoHelp::readTypedDouble(inputStorage, "Setting effort using 4 parameters requires the begin time as first parameter.");
                    endTime = StoHelp::readTypedDouble(inputStorage, "Setting effort using 4 parameters requires the end time as second parameter.");
                    edgeID = StoHelp::readTypedString(inputStorage, "Setting effort using 4 parameters requires the referenced edge as third parameter.");
                    value = StoHelp::readTypedDouble(inputStorage, "Setting effort using 4 parameters requires the effort as double as fourth parameter.");
                } else if (parameterCount == 2) {
                    edgeID = StoHelp::readTypedString(inputStorage, "Setting effort using 2 parameters requires the referenced edge as first parameter.");
                    value = StoHelp::readTypedDouble(inputStorage, "Setting effort using 2 parameters requires the effort as double as second parameter.");
                } else if (parameterCount == 1) {
                    edgeID = StoHelp::readTypedString(inputStorage, "Setting effort using 1 parameter requires the referenced edge as first parameter.");
                } else {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Setting effort requires 1, 2, or 4 parameters.", outputStorage);
                }
                libsumo::Vehicle::setEffort(id, edgeID, value, begTime, endTime);
            }
            break;
            case libsumo::CMD_REROUTE_TRAVELTIME: {
                StoHelp::readCompound(inputStorage, 0, "Rerouting by travel time requires an empty compound object.");
                libsumo::Vehicle::rerouteTraveltime(id, false);
            }
            break;
            case libsumo::CMD_REROUTE_EFFORT: {
                StoHelp::readCompound(inputStorage, 0, "Rerouting by effort requires an empty compound object.");
                libsumo::Vehicle::rerouteEffort(id);
            }
            break;
            case libsumo::VAR_SIGNALS:
                libsumo::Vehicle::setSignals(id, StoHelp::readTypedInt(inputStorage, "Setting signals requires an integer."));
                break;
            case libsumo::VAR_MOVE_TO: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Setting position requires a compound object.");
                if (parameterCount < 2 || parameterCount > 3) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Setting position should obtain the lane id and the position and optionally the reason.", outputStorage);
                }
                const std::string laneID = StoHelp::readTypedString(inputStorage, "The first parameter for setting a position must be the lane ID given as a string.");
                const double position = StoHelp::readTypedDouble(inputStorage, "The second parameter for setting a position must be the position given as a double.");
                int reason = libsumo::MOVE_AUTOMATIC;
                if (parameterCount == 3) {
                    reason = StoHelp::readTypedInt(inputStorage, "The third parameter for setting a position must be the reason given as an int.");
                }
                // process
                libsumo::Vehicle::moveTo(id, laneID, position, reason);
            }
            break;
            case libsumo::VAR_IMPATIENCE: {
                libsumo::Vehicle::setImpatience(id, StoHelp::readTypedDouble(inputStorage, "Setting impatience requires a double."));
            }
            break;
            case libsumo::VAR_SPEED: {
                libsumo::Vehicle::setSpeed(id, StoHelp::readTypedDouble(inputStorage, "Setting speed requires a double."));
            }
            break;
            case libsumo::VAR_ACCELERATION: {
                StoHelp::readCompound(inputStorage, 2, "Setting acceleration requires 2 parameters.");
                const double accel = StoHelp::readTypedDouble(inputStorage, "Setting acceleration requires the acceleration as first parameter given as a double.");
                const double duration = StoHelp::readTypedDouble(inputStorage, "Setting acceleration requires the duration as second parameter given as a double.");
                if (duration < 0) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Duration must not be negative.", outputStorage);
                }
                libsumo::Vehicle::setAcceleration(id, accel, duration);
            }
            break;
            case libsumo::VAR_PREV_SPEED: {
                double prevSpeed = 0;
                double prevAcceleration = libsumo::INVALID_DOUBLE_VALUE;
                int inputtype = inputStorage.readUnsignedByte();
                if (inputtype == libsumo::TYPE_COMPOUND) {
                    // Setting previous speed with 2 parameters, uses a compound object description
                    const int parameterCount = inputStorage.readInt();
                    if (parameterCount == 2) {
                        prevSpeed = StoHelp::readTypedDouble(inputStorage, "Setting previous speed using 2 parameters requires the previous speed as first parameter given as a double.");
                        prevAcceleration = StoHelp::readTypedDouble(inputStorage, "Setting previous speed using 2 parameters requires the previous acceleration as second parameter given as a double.");
                    } else if (parameterCount == 1) {
                        prevSpeed = StoHelp::readTypedDouble(inputStorage, "Setting previous speed using 1 parameter requires the previous speed as first parameter given as a double.");
                    } else {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Setting previous speed requires 1 or 2 parameters.", outputStorage);
                    }
                } else if (inputtype == libsumo::TYPE_DOUBLE) {
                    // Setting previous speed with 1 parameter (double), no compound object description
                    prevSpeed = inputStorage.readDouble();
                } else {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Setting previous speed requires 1 parameter given as a double or 2 parameters as compound object description.", outputStorage);
                }
                if (prevSpeed < 0) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Previous speed must not be negative.", outputStorage);
                }
                libsumo::Vehicle::setPreviousSpeed(id, prevSpeed, prevAcceleration);
            }
            break;
            case libsumo::VAR_SPEEDSETMODE:
                libsumo::Vehicle::setSpeedMode(id, StoHelp::readTypedInt(inputStorage, "Setting speed mode requires an integer."));
                break;
            case libsumo::VAR_LANECHANGE_MODE:
                libsumo::Vehicle::setLaneChangeMode(id, StoHelp::readTypedInt(inputStorage, "Setting lane change mode requires an integer."));
                break;
            case libsumo::VAR_ROUTING_MODE:
                libsumo::Vehicle::setRoutingMode(id, StoHelp::readTypedInt(inputStorage, "Setting routing mode requires an integer."));
                break;
            case libsumo::VAR_COLOR: {
                libsumo::Vehicle::setColor(id, StoHelp::readTypedColor(inputStorage, "The color must be given using the according type."));
                break;
            }
            case libsumo::ADD: {
                StoHelp::readCompound(inputStorage, 6, "Adding a vehicle needs six parameters.");
                const std::string vTypeID = StoHelp::readTypedString(inputStorage, "First parameter (type) requires a string.");
                const std::string routeID = StoHelp::readTypedString(inputStorage, "Second parameter (route) requires a string.");
                const int departCode = StoHelp::readTypedInt(inputStorage, "Third parameter (depart) requires an integer.");
                std::string depart = toString(STEPS2TIME(departCode));
                if (-departCode == static_cast<int>(DepartDefinition::TRIGGERED)) {
                    depart = "triggered";
                } else if (-departCode == static_cast<int>(DepartDefinition::CONTAINER_TRIGGERED)) {
                    depart = "containerTriggered";
                } else if (-departCode == static_cast<int>(DepartDefinition::NOW)) {
                    depart = "now";
                } else if (-departCode == static_cast<int>(DepartDefinition::SPLIT)) {
                    depart = "split";
                } else if (-departCode == static_cast<int>(DepartDefinition::BEGIN)) {
                    depart = "begin";
                }

                const double departPosCode = StoHelp::readTypedDouble(inputStorage, "Fourth parameter (position) requires a double.");
                std::string departPos = toString(departPosCode);
                if (-departPosCode == (int)DepartPosDefinition::RANDOM) {
                    departPos = "random";
                } else if (-departPosCode == (int)DepartPosDefinition::RANDOM_FREE) {
                    departPos = "random_free";
                } else if (-departPosCode == (int)DepartPosDefinition::FREE) {
                    departPos = "free";
                } else if (-departPosCode == (int)DepartPosDefinition::BASE) {
                    departPos = "base";
                } else if (-departPosCode == (int)DepartPosDefinition::LAST) {
                    departPos = "last";
                } else if (-departPosCode == (int)DepartPosDefinition::GIVEN) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Invalid departure position.", outputStorage);
                }

                const double departSpeedCode = StoHelp::readTypedDouble(inputStorage, "Fifth parameter (speed) requires a double.");
                std::string departSpeed = toString(departSpeedCode);
                if (-departSpeedCode == (int)DepartSpeedDefinition::RANDOM) {
                    departSpeed = "random";
                } else if (-departSpeedCode == (int)DepartSpeedDefinition::MAX) {
                    departSpeed = "max";
                } else if (-departSpeedCode == (int)DepartSpeedDefinition::DESIRED) {
                    departSpeed = "desired";
                } else if (-departSpeedCode == (int)DepartSpeedDefinition::LIMIT) {
                    departSpeed = "speedLimit";
                } else if (-departSpeedCode == (int)DepartSpeedDefinition::LAST) {
                    departSpeed = "last";
                } else if (-departSpeedCode == (int)DepartSpeedDefinition::AVG) {
                    departSpeed = "avg";
                }

                const int departLaneCode = StoHelp::readTypedByte(inputStorage, "Sixth parameter (lane) requires a byte.");
                std::string departLane = toString(departLaneCode);
                if (-departLaneCode == (int)DepartLaneDefinition::RANDOM) {
                    departLane = "random";
                } else if (-departLaneCode == (int)DepartLaneDefinition::FREE) {
                    departLane = "free";
                } else if (-departLaneCode == (int)DepartLaneDefinition::ALLOWED_FREE) {
                    departLane = "allowed";
                } else if (-departLaneCode == (int)DepartLaneDefinition::BEST_FREE) {
                    departLane = "best";
                } else if (-departLaneCode == (int)DepartLaneDefinition::FIRST_ALLOWED) {
                    departLane = "first";
                }
                libsumo::Vehicle::add(id, routeID, vTypeID, depart, departLane, departPos, departSpeed);
            }
            break;
            case libsumo::ADD_FULL: {
                StoHelp::readCompound(inputStorage, 14, "Adding a fully specified vehicle needs fourteen parameters.");
                const std::string routeID = StoHelp::readTypedString(inputStorage, "First parameter (route) requires a string.");
                const std::string vTypeID = StoHelp::readTypedString(inputStorage, "Second parameter (type) requires a string.");
                const std::string depart = StoHelp::readTypedString(inputStorage, "Third parameter (depart) requires an string.");
                const std::string departLane = StoHelp::readTypedString(inputStorage, "Fourth parameter (depart lane) requires a string.");
                const std::string departPos = StoHelp::readTypedString(inputStorage, "Fifth parameter (depart position) requires a string.");
                const std::string departSpeed = StoHelp::readTypedString(inputStorage, "Sixth parameter (depart speed) requires a string.");
                const std::string arrivalLane = StoHelp::readTypedString(inputStorage, "Seventh parameter (arrival lane) requires a string.");
                const std::string arrivalPos = StoHelp::readTypedString(inputStorage, "Eighth parameter (arrival position) requires a string.");
                const std::string arrivalSpeed = StoHelp::readTypedString(inputStorage, "Ninth parameter (arrival speed) requires a string.");
                const std::string fromTaz = StoHelp::readTypedString(inputStorage, "Tenth parameter (from taz) requires a string.");
                const std::string toTaz = StoHelp::readTypedString(inputStorage, "Eleventh parameter (to taz) requires a string.");
                const std::string line = StoHelp::readTypedString(inputStorage, "Twelth parameter (line) requires a string.");
                const int personCapacity = StoHelp::readTypedInt(inputStorage, "13th parameter (person capacity) requires an int.");
                const int personNumber = StoHelp::readTypedInt(inputStorage, "14th parameter (person number) requires an int.");
                libsumo::Vehicle::add(id, routeID, vTypeID, depart, departLane, departPos, departSpeed, arrivalLane, arrivalPos, arrivalSpeed,
                                      fromTaz, toTaz, line, personCapacity, personNumber);
            }
            break;
            case libsumo::REMOVE: {
                libsumo::Vehicle::remove(id, (char)StoHelp::readTypedByte(inputStorage, "Removing a vehicle requires a byte."));
            }
            break;
            case libsumo::MOVE_TO_XY: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "MoveToXY vehicle requires a compound object.");
                if (parameterCount < 5 || parameterCount > 7) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "MoveToXY vehicle should obtain: edgeID, lane, x, y, angle and optionally keepRouteFlag and matchThreshold.", outputStorage);
                }
                const std::string edgeID = StoHelp::readTypedString(inputStorage, "The first parameter for moveToXY must be the edge ID given as a string.");
                const int laneIndex = StoHelp::readTypedInt(inputStorage, "The second parameter for moveToXY must be lane given as an int.");
                const double x = StoHelp::readTypedDouble(inputStorage, "The third parameter for moveToXY must be the x-position given as a double.");
                const double y = StoHelp::readTypedDouble(inputStorage, "The fourth parameter for moveToXY must be the y-position given as a double.");
                const double angle = StoHelp::readTypedDouble(inputStorage, "The fifth parameter for moveToXY must be the angle given as a double.");
                int keepRouteFlag = 1;
                if (parameterCount >= 6) {
                    keepRouteFlag = StoHelp::readTypedByte(inputStorage, "The sixth parameter for moveToXY must be the keepRouteFlag given as a byte.");
                }
                double matchThreshold = 100.;
                if (parameterCount == 7) {
                    matchThreshold = StoHelp::readTypedDouble(inputStorage, "The seventh parameter for moveToXY must be the matchThreshold given as a double.");
                }
                libsumo::Vehicle::moveToXY(id, edgeID, laneIndex, x, y, angle, keepRouteFlag, matchThreshold);
            }
            break;
            case libsumo::VAR_SPEED_FACTOR: {
                libsumo::Vehicle::setSpeedFactor(id, StoHelp::readTypedDouble(inputStorage, "Setting speed factor requires a double."));
            }
            break;
            case libsumo::VAR_LINE: {
                libsumo::Vehicle::setLine(id, StoHelp::readTypedString(inputStorage, "The line must be given as a string."));
            }
            break;
            case libsumo::VAR_VIA: {
                libsumo::Vehicle::setVia(id, StoHelp::readTypedStringList(inputStorage, "Vias must be defined as a list of edge ids."));
            }
            break;
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Vehicle::setParameter(id, name, value);
            }
            break;
            case libsumo::VAR_HIGHLIGHT: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "A compound object is needed for highlighting an object.");
                if (parameterCount > 5) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Highlighting an object needs zero to five parameters.", outputStorage);
                }

                libsumo::TraCIColor col = libsumo::TraCIColor(255, 0, 0);
                if (parameterCount > 0) {
                    col = StoHelp::readTypedColor(inputStorage, "The first parameter for highlighting must be the highlight color.");
                }
                double size = -1;
                if (parameterCount > 1) {
                    size = StoHelp::readTypedDouble(inputStorage, "The second parameter for highlighting must be the highlight size.");
                }
                int alphaMax = -1;
                if (parameterCount > 2) {
                    alphaMax = StoHelp::readTypedUnsignedByte(inputStorage, "The third parameter for highlighting must be maximal alpha.");
                }
                double duration = -1;
                if (parameterCount > 3) {
                    duration = StoHelp::readTypedDouble(inputStorage, "The fourth parameter for highlighting must be the highlight duration.");
                }
                int type = 0;
                if (parameterCount > 4) {
                    type = StoHelp::readTypedUnsignedByte(inputStorage, "The fifth parameter for highlighting must be the highlight type id as ubyte.");
                }
                libsumo::Vehicle::highlight(id, col, size, alphaMax, duration, type);
            }
            break;
            case libsumo::CMD_TAXI_DISPATCH: {
                libsumo::Vehicle::dispatchTaxi(id, StoHelp::readTypedStringList(inputStorage, "A dispatch command  must be defined as a list of reservation ids."));
            }
            break;
            case libsumo::VAR_ACTIONSTEPLENGTH: {
                const double value = StoHelp::readTypedDouble(inputStorage, "Setting action step length requires a double.");
                if (fabs(value) == std::numeric_limits<double>::infinity()) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Invalid action step length.", outputStorage);
                }
                bool resetActionOffset = value >= 0.0;
                libsumo::Vehicle::setActionStepLength(id, fabs(value), resetActionOffset);
            }
            break;
            case libsumo::VAR_LANEPOSITION_LAT: {
                libsumo::Vehicle::setLateralLanePosition(id, StoHelp::readTypedDouble(inputStorage, "Setting lateral lane position requires a double."));
            }
            break;
            case libsumo::VAR_UPDATE_BESTLANES: {
                libsumo::Vehicle::updateBestLanes(id);
            }
            break;
            case libsumo::VAR_MINGAP: {
                const double value = StoHelp::readTypedDouble(inputStorage, "Setting minimum gap requires a double.");
                if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Invalid minimum gap.", outputStorage);
                }
                libsumo::Vehicle::setMinGap(id, value);
            }
            break;
            case libsumo::VAR_MINGAP_LAT: {
                const double value = StoHelp::readTypedDouble(inputStorage, "Setting minimum lateral gap requires a double.");
                if (value < 0.0 || fabs(value) == std::numeric_limits<double>::infinity()) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, "Invalid minimum lateral gap.", outputStorage);
                }
                libsumo::Vehicle::setMinGapLat(id, value);
            }
            break;
            default: {
                try {
                    if (!TraCIServerAPI_VehicleType::setVariable(libsumo::CMD_SET_VEHICLE_VARIABLE, variable, v->getSingularType().getID(), server, inputStorage, outputStorage)) {
                        return false;
                    }
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                } catch (libsumo::TraCIException& e) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
                }
            }
            break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Vehicle::insertReplaceStop(TraCIServer& server, tcpip::Storage& inputStorage, tcpip::Storage& outputStorage, const std::string& id, bool replace) {
    const std::string m1 = replace ? "Replacing" : "Inserting";
    const std::string m2 = replace ? "replacement" : "insertion";
    const int parameterCount = StoHelp::readCompound(inputStorage, -1, m1 + " stop needs a compound object description.");
    if (parameterCount != 8 && parameterCount != 9) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_VEHICLE_VARIABLE, m1 + " stop needs a compound object description of eight or nine items.", outputStorage);
    }
    // read road map position
    const std::string edgeID = StoHelp::readTypedString(inputStorage, "The first stop " + m2 + " parameter must be the edge id given as a string.");
    const double pos = StoHelp::readTypedDouble(inputStorage, "The second stop " + m2 + " parameter must be the end position along the edge given as a double.");
    const int laneIndex = StoHelp::readTypedByte(inputStorage, "The third stop " + m2 + " parameter must be the lane index given as a byte.");
    const double duration = StoHelp::readTypedDouble(inputStorage, "The fourth stop " + m2 + " parameter must be the stopping duration given as a double.");
    const int stopFlags = StoHelp::readTypedInt(inputStorage, "The fifth stop " + m2 + " parameter must be an int indicating its parking/triggered status.");
    const double startPos = StoHelp::readTypedDouble(inputStorage, "The sixth stop " + m2 + " parameter must be the start position along the edge given as a double.");
    const double until = StoHelp::readTypedDouble(inputStorage, "The seventh stop " + m2 + " parameter must be the minimum departure time given as a double.");
    const int nextStopIndex = StoHelp::readTypedInt(inputStorage, "The eighth stop " + m2 + " parameter must be the replacement index given as an int.");
    int teleport = 0;
    if (parameterCount == 9) {
        teleport = StoHelp::readTypedByte(inputStorage, "The ninth stop " + m2 + " parameter must be the teleport flag given as a byte.");
    }
    if (replace) {
        libsumo::Vehicle::replaceStop(id, nextStopIndex, edgeID, pos, laneIndex, duration, stopFlags, startPos, until, teleport);
    } else {
        libsumo::Vehicle::insertStop(id, nextStopIndex, edgeID, pos, laneIndex, duration, stopFlags, startPos, until, teleport);
    }
    return true;
}


/****************************************************************************/
