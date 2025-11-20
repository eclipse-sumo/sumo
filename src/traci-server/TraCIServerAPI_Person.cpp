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
/// @file    TraCIServerAPI_Person.cpp
/// @author  Daniel Krajzewicz
/// @date    26.05.2014
///
// APIs for getting/setting person values via TraCI
/****************************************************************************/
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <libsumo/Person.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/VehicleType.h>
#include "TraCIServer.h"
#include "TraCIServerAPI_VehicleType.h"
#include "TraCIServerAPI_Person.h"
#include "TraCIServerAPI_Simulation.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Person::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                  tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_PERSON_VARIABLE, variable, id);
    try {
        if (!libsumo::Person::handleVariable(id, variable, &server, &inputStorage)) {
            return server.writeErrorStatusCmd(libsumo::CMD_GET_PERSON_VARIABLE, "Get Person Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_PERSON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_PERSON_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Person::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                  tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_PARAMETER
            && variable != libsumo::ADD
            && variable != libsumo::REMOVE
            && variable != libsumo::APPEND_STAGE
            && variable != libsumo::REPLACE_STAGE
            && variable != libsumo::REMOVE_STAGE
            && variable != libsumo::CMD_REROUTE_TRAVELTIME
            && variable != libsumo::VAR_MOVE_TO
            && variable != libsumo::MOVE_TO_XY
            && variable != libsumo::VAR_SPEED
            && variable != libsumo::VAR_TYPE
            && variable != libsumo::VAR_SPEED_FACTOR
            && variable != libsumo::VAR_LENGTH
            && variable != libsumo::VAR_WIDTH
            && variable != libsumo::VAR_HEIGHT
            && variable != libsumo::VAR_MINGAP
            && variable != libsumo::VAR_COLOR
       ) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "Change Person State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }

    try {
        // TODO: remove declaration of c after completion
        MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
        // id
        std::string id = inputStorage.readString();
        // TODO: remove declaration of p after completion
        const bool shouldExist = variable != libsumo::ADD;
        MSTransportable* p = c.get(id);
        if (p == nullptr && shouldExist) {
            return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
        }
        // process
        switch (variable) {
            case libsumo::VAR_SPEED: {
                // set the speed for all present and future (walking) stages and modify the vType so that stages added later are also affected
                libsumo::Person::setSpeed(id, StoHelp::readTypedDouble(inputStorage, "Setting speed requires a double."));
            }
            break;
            case libsumo::VAR_TYPE: {
                libsumo::Person::setType(id, StoHelp::readTypedString(inputStorage, "The vehicle type id must be given as a string."));
                break;
            }
            case libsumo::VAR_SPEED_FACTOR: {
                libsumo::Person::setSpeedFactor(id, StoHelp::readTypedDouble(inputStorage, "Setting SpeedFactor requires a double."));
            }
            break;
            case libsumo::VAR_COLOR: {
                libsumo::Person::setColor(id, StoHelp::readTypedColor(inputStorage, "The color must be given using the according type."));
                break;
            }
            case libsumo::ADD: {
                StoHelp::readCompound(inputStorage, 4, "Adding a person needs four parameters.");
                const std::string vTypeID = StoHelp::readTypedString(inputStorage, "First parameter (type) requires a string.");
                const std::string edgeID = StoHelp::readTypedString(inputStorage, "Second parameter (edge) requires a string.");
                const double depart = StoHelp::readTypedDouble(inputStorage, "Third parameter (depart) requires a double.");
                const double pos = StoHelp::readTypedDouble(inputStorage, "Fourth parameter (position) requires a double.");
                libsumo::Person::add(id, edgeID, pos, depart, vTypeID);
            }
            break;
            case libsumo::REMOVE: {
                libsumo::Person::remove(id, (char)StoHelp::readTypedByte(inputStorage, "Removing a person requires a byte."));
            }
            break;
            case libsumo::APPEND_STAGE: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Adding a person stage requires a compound object.");
                if (parameterCount == 13) {
                    libsumo::TraCIStage stage;
                    StoHelp::readStage(inputStorage, stage);
                    libsumo::Person::appendStage(id, stage);
                } else {
                    const int stageType = StoHelp::readTypedInt(inputStorage, "The first parameter for adding a stage must be the stage type given as int.");
                    if (stageType == libsumo::STAGE_DRIVING) {
                        // append driving stage
                        if (parameterCount != 4) {
                            return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "Adding a driving stage needs four parameters.", outputStorage);
                        }
                        const std::string edgeID = StoHelp::readTypedString(inputStorage, "Second parameter (edge) requires a string.");
                        const std::string lines = StoHelp::readTypedString(inputStorage, "Third parameter (lines) requires a string.");
                        const std::string stopID = StoHelp::readTypedString(inputStorage, "Fourth parameter (stopID) requires a string.");
                        libsumo::Person::appendDrivingStage(id, edgeID, lines, stopID);
                    } else if (stageType == libsumo::STAGE_WAITING) {
                        // append waiting stage
                        if (parameterCount != 4) {
                            return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "Adding a waiting stage needs four parameters.", outputStorage);
                        }
                        const double duration = StoHelp::readTypedDouble(inputStorage, "Second parameter (duration) requires a double.");
                        const std::string description = StoHelp::readTypedString(inputStorage, "Third parameter (description) requires a string.");
                        const std::string stopID = StoHelp::readTypedString(inputStorage, "Fourth parameter (stopID) requires a string.");
                        libsumo::Person::appendWaitingStage(id, duration, description, stopID);
                    } else if (stageType == libsumo::STAGE_WALKING) {
                        // append walking stage
                        if (parameterCount != 6) {
                            return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "Adding a walking stage needs six parameters.", outputStorage);
                        }
                        const std::vector<std::string> edgeIDs = StoHelp::readTypedStringList(inputStorage, "Second parameter (edges) route must be defined as a list of edge ids.");
                        const double arrivalPos = StoHelp::readTypedDouble(inputStorage, "Third parameter (arrivalPos) requires a double.");
                        const double duration = StoHelp::readTypedDouble(inputStorage, "Fourth parameter (duration) requires a double.");
                        const double speed = StoHelp::readTypedDouble(inputStorage, "Fifth parameter (speed) requires a double.");
                        const std::string stopID = StoHelp::readTypedString(inputStorage, "Sixth parameter (stopID) requires a string.");
                        libsumo::Person::appendWalkingStage(id, edgeIDs, arrivalPos, duration, speed, stopID);
                    } else {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "Invalid stage type for person '" + id + "'", outputStorage);
                    }
                }
            }
            break;
            case libsumo::REPLACE_STAGE: {
                StoHelp::readCompound(inputStorage, 2, "Replacing a person stage requires a compound object of size 2.");
                const int nextStageIndex = StoHelp::readTypedInt(inputStorage, "First parameter of replace stage should be an integer");
                StoHelp::readCompound(inputStorage, 13, "Second parameter of replace stage should be a compound object of size 13");
                libsumo::TraCIStage stage;
                StoHelp::readStage(inputStorage, stage);
                libsumo::Person::replaceStage(id, nextStageIndex, stage);
            }
            break;
            case libsumo::REMOVE_STAGE: {
                libsumo::Person::removeStage(id, StoHelp::readTypedInt(inputStorage, "The message must contain the stage index."));
            }
            break;
            case libsumo::CMD_REROUTE_TRAVELTIME: {
                StoHelp::readCompound(inputStorage, 0, "Resuming requires an empty compound object.");
                libsumo::Person::rerouteTraveltime(id);
            }
            break;
            case libsumo::VAR_MOVE_TO: {
                StoHelp::readCompound(inputStorage, 3, "Setting position should obtain the edge id, the position and the lateral position.");
                const std::string laneID = StoHelp::readTypedString(inputStorage, "The first parameter for setting a position must be the laneID given as a string.");
                const double position = StoHelp::readTypedDouble(inputStorage, "The second parameter for setting a position must be the position given as a double.");
                const double posLat = StoHelp::readTypedDouble(inputStorage, "The third parameter for setting a position must be the lateral position given as a double.");
                libsumo::Person::moveTo(id, laneID, position, posLat);
            }
            break;
            case libsumo::MOVE_TO_XY: {
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "MoveToXY person requires a compound object.");
                if (parameterCount != 5 && parameterCount != 6) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, "MoveToXY person should obtain: edgeID, x, y, angle, keepRouteFlag and optionally matchThreshold.", outputStorage);
                }
                const std::string edgeID = StoHelp::readTypedString(inputStorage, "The first parameter for moveToXY must be the edge ID given as a string.");
                const double x = StoHelp::readTypedDouble(inputStorage, "The second parameter for moveToXY must be the x-position given as a double.");
                const double y = StoHelp::readTypedDouble(inputStorage, "The third parameter for moveToXY must be the y-position given as a double.");
                const double angle = StoHelp::readTypedDouble(inputStorage, "The fourth parameter for moveToXY must be the angle given as a double.");
                const int keepRouteFlag = StoHelp::readTypedByte(inputStorage, "The fifth parameter for moveToXY must be the keepRouteFlag given as a byte.");
                double matchThreshold = 100.;
                if (parameterCount == 6) {
                    matchThreshold = StoHelp::readTypedDouble(inputStorage, "The sixth parameter for moveToXY must be the matchThreshold given as a double.");
                }
                libsumo::Person::moveToXY(id, edgeID, x, y, angle, keepRouteFlag, matchThreshold);
            }
            break;
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Person::setParameter(id, name, value);
            }
            break;
            default:
                try {
                    if (!TraCIServerAPI_VehicleType::setVariable(libsumo::CMD_SET_PERSON_VARIABLE, variable, p->getSingularType().getID(), server, inputStorage, outputStorage)) {
                        return false;
                    }
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
                }
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_PERSON_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
