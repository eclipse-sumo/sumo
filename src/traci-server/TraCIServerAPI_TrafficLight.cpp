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
/// @file    TraCIServerAPI_TrafficLight.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    07.05.2009
///
// APIs for getting/setting traffic light values via TraCI
/****************************************************************************/
#include <config.h>

#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/TrafficLight.h>
#include "TraCIServerAPI_TrafficLight.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_TrafficLight::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                        tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_TL_VARIABLE, variable, id);
    try {
        if (!libsumo::TrafficLight::handleVariable(id, variable, &server, &inputStorage)) {
            switch (variable) {
                case libsumo::TL_CONSTRAINT_SWAP: {
                    StoHelp::readCompound(inputStorage, 3, "A compound object of size 3 is needed for swapping constraints.");
                    const std::string tripId = StoHelp::readTypedString(inputStorage, "The tripId must be given as a string.");
                    const std::string foeSignal = StoHelp::readTypedString(inputStorage, "The foeSignal id must be given as a string.");
                    const std::string foeId = StoHelp::readTypedString(inputStorage, "The foe tripId must be given as a string.");
                    server.wrapSignalConstraintVector(id, variable, libsumo::TrafficLight::swapConstraints(id, tripId, foeSignal, foeId));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_TL_VARIABLE, "Get TLS Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_TL_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_TL_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_TrafficLight::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                        tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    const int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::TL_PHASE_INDEX && variable != libsumo::TL_PROGRAM && variable != libsumo::TL_PHASE_DURATION
            && variable != libsumo::TL_RED_YELLOW_GREEN_STATE && variable != libsumo::TL_COMPLETE_PROGRAM_RYG
            && variable != libsumo::VAR_NAME
            && variable != libsumo::TL_CONSTRAINT_REMOVE
            && variable != libsumo::TL_CONSTRAINT_UPDATE
            && variable != libsumo::TL_CONSTRAINT_ADD
            && variable != libsumo::VAR_PARAMETER) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_TL_VARIABLE, "Change TLS State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    const std::string id = inputStorage.readString();
    try {
        switch (variable) {
            case libsumo::TL_PHASE_INDEX: {
                libsumo::TrafficLight::setPhase(id, StoHelp::readTypedInt(inputStorage, "The phase index must be given as an integer."));
            }
            break;
            case libsumo::VAR_NAME:
                libsumo::TrafficLight::setPhaseName(id, StoHelp::readTypedString(inputStorage, "The phase name must be given as a string."));
                break;
            case libsumo::TL_PROGRAM:
                libsumo::TrafficLight::setProgram(id, StoHelp::readTypedString(inputStorage, "The program must be given as a string."));
                break;
            case libsumo::TL_PHASE_DURATION:
                libsumo::TrafficLight::setPhaseDuration(id, StoHelp::readTypedDouble(inputStorage, "The phase duration must be given as a double."));
                break;
            case libsumo::TL_RED_YELLOW_GREEN_STATE:
                libsumo::TrafficLight::setRedYellowGreenState(id, StoHelp::readTypedString(inputStorage, "The phase must be given as a string."));
                break;
            case libsumo::TL_COMPLETE_PROGRAM_RYG: {
                StoHelp::readCompound(inputStorage, -1, "A compound object is needed for setting a new program.");
                libsumo::TraCILogic logic;
                logic.programID = StoHelp::readTypedString(inputStorage, "set program: 1. parameter (programID) must be a string.");
                logic.type = StoHelp::readTypedInt(inputStorage, "set program: 2. parameter (type) must be an int.");
                logic.currentPhaseIndex = StoHelp::readTypedInt(inputStorage, "set program: 3. parameter (index) must be an int.");

                const int numPhases = StoHelp::readCompound(inputStorage, -1, "A compound object is needed for the phases.");
                for (int j = 0; j < numPhases; ++j) {
                    const int items = StoHelp::readCompound(inputStorage, -1, "A compound object is needed for every phase.");
                    if (items != 6 && items != 5) {
                        return server.writeErrorStatusCmd(libsumo::CMD_SET_TL_VARIABLE, "A phase compound object requires 5 or 6 items.", outputStorage);
                    }
                    const double duration = StoHelp::readTypedDouble(inputStorage, "set program: 4.1. parameter (duration) must be a double.");
                    const std::string state = StoHelp::readTypedString(inputStorage, "set program: 4.2. parameter (phase) must be a string.");
                    const double minDuration = StoHelp::readTypedDouble(inputStorage, "set program: 4.3. parameter (min duration) must be a double.");
                    const double maxDuration = StoHelp::readTypedDouble(inputStorage, "set program: 4.4. parameter (max duration) must be a double.");
                    const int numNext = StoHelp::readCompound(inputStorage, -1, "set program 4.5 parameter (next) must be a compound (list of ints).");
                    std::vector<int> next;
                    for (int k = 0; k < numNext; ++k) {
                        next.push_back(StoHelp::readTypedInt(inputStorage, "set program: 4.5. parameter (next) must be a list of int."));
                    }
                    std::string name;
                    if (items == 6) {
                        name = StoHelp::readTypedString(inputStorage, "set program: 4.6. parameter (name) must be a string.");
                    }
                    logic.phases.emplace_back(new libsumo::TraCIPhase(duration, state, minDuration, maxDuration, next, name));
                }
                const int numParams = StoHelp::readCompound(inputStorage, -1, "set program: 5. parameter (subparams) must be a compound object.");
                for (int j = 0; j < numParams; ++j) {
                    const std::vector<std::string> par = StoHelp::readTypedStringList(inputStorage);
                    logic.subParameter[par[0]] = par[1];
                }
                libsumo::TrafficLight::setCompleteRedYellowGreenDefinition(id, logic);
            }
            break;
            case libsumo::TL_CONSTRAINT_REMOVE: {
                StoHelp::readCompound(inputStorage, 3, "A compound object of size 3 is needed for removing constraints.");
                const std::string tripId = StoHelp::readTypedString(inputStorage, "The tripId must be given as a string.");
                const std::string foeSignal = StoHelp::readTypedString(inputStorage, "The foeSignal id must be given as a string.");
                const std::string foeId = StoHelp::readTypedString(inputStorage, "The foe tripId must be given as a string.");
                libsumo::TrafficLight::removeConstraints(id, tripId, foeSignal, foeId);
            }
            break;
            case libsumo::TL_CONSTRAINT_UPDATE:
                libsumo::TrafficLight::updateConstraints(id, StoHelp::readTypedString(inputStorage, "The tripId index must be given as a string."));
                break;
            case libsumo::TL_CONSTRAINT_ADD: {
                StoHelp::readCompound(inputStorage, 5, "A compound object of size 5 is needed for adding constraints.");
                const std::string tripId = StoHelp::readTypedString(inputStorage, "The tripId must be given as a string.");
                const std::string foeSignal = StoHelp::readTypedString(inputStorage, "The foe signal must be given as a string.");
                const std::string foeId = StoHelp::readTypedString(inputStorage, "The foe tripId must be given as a string.");
                const int type = StoHelp::readTypedInt(inputStorage, "The type must be an int.");
                const int limit = StoHelp::readTypedInt(inputStorage, "The limit must be an int.");
                libsumo::TrafficLight::addConstraint(id, tripId, foeSignal, foeId, type, limit);
            }
            break;
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::TrafficLight::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_TL_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_TL_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


void
TraCIServerAPI_TrafficLight::writeConstraint(TraCIServer& server, const libsumo::TraCISignalConstraint& c) {
    StoHelp::writeTypedString(server.getWrapperStorage(), c.signalId);
    StoHelp::writeTypedString(server.getWrapperStorage(), c.tripId);
    StoHelp::writeTypedString(server.getWrapperStorage(), c.foeId);
    StoHelp::writeTypedString(server.getWrapperStorage(), c.foeSignal);
    StoHelp::writeTypedInt(server.getWrapperStorage(), c.limit);
    StoHelp::writeTypedInt(server.getWrapperStorage(), c.type);
    StoHelp::writeTypedByte(server.getWrapperStorage(), c.mustWait);
    StoHelp::writeTypedByte(server.getWrapperStorage(), c.active);
    std::vector<std::string> paramItems;
    for (auto item : c.param) {
        paramItems.push_back(item.first);
        paramItems.push_back(item.second);
    }
    StoHelp::writeTypedStringList(server.getWrapperStorage(), paramItems);
}


/****************************************************************************/
