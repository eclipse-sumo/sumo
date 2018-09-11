/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_TrafficLight.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting traffic light values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <libsumo/TrafficLight.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_TrafficLight.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_TrafficLight::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                        tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_TL_VARIABLE, variable, id);
    try {
        if (!libsumo::TrafficLight::handleVariable(id, variable, &server)) {
            switch (variable) {
                case TL_COMPLETE_DEFINITION_RYG: {
                    std::vector<libsumo::TraCILogic> logics = libsumo::TrafficLight::getCompleteRedYellowGreenDefinition(id);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt((int)logics.size());
                    ++cnt;
                    for (const libsumo::TraCILogic& logic : logics) {
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(logic.subID);
                        ++cnt;
                        // type (always 0 by now)
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt(logic.type);
                        ++cnt;
                        // subparameter (always 0 by now)
                        tempContent.writeUnsignedByte(TYPE_COMPOUND);
                        tempContent.writeInt(0);
                        ++cnt;
                        // (current) phase index
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt(logic.currentPhaseIndex);
                        ++cnt;
                        // phase number
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt((int)logic.phases.size());
                        ++cnt;
                        for (const libsumo::TraCIPhase& phase : logic.phases) {
                            tempContent.writeUnsignedByte(TYPE_DOUBLE);
                            tempContent.writeDouble(phase.duration);
                            ++cnt;
                            tempContent.writeUnsignedByte(TYPE_DOUBLE);
                            tempContent.writeDouble(phase.duration1);
                            ++cnt; // not implemented
                            tempContent.writeUnsignedByte(TYPE_DOUBLE);
                            tempContent.writeDouble(phase.duration2);
                            ++cnt; // not implemented
                            tempContent.writeUnsignedByte(TYPE_STRING);
                            tempContent.writeString(phase.phase);
                            ++cnt;
                        }
                    }
                    server.getWrapperStorage().writeInt((int)cnt);
                    server.getWrapperStorage().writeStorage(tempContent);
                    break;
                }
                case TL_CONTROLLED_LINKS: {
                    const std::vector<std::vector<libsumo::TraCILink> > links = libsumo::TrafficLight::getControlledLinks(id);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt((int)links.size());
                    for (const std::vector<libsumo::TraCILink>& sublinks : links) {
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt((int)sublinks.size());
                        ++cnt;
                        for (const libsumo::TraCILink& link : sublinks) {
                            tempContent.writeUnsignedByte(TYPE_STRINGLIST);
                            tempContent.writeStringList(std::vector<std::string>({ link.fromLane, link.toLane, link.viaLane }));
                            ++cnt;
                        }
                    }
                    server.getWrapperStorage().writeInt(cnt);
                    server.getWrapperStorage().writeStorage(tempContent);
                    break;
                }
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_TL_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(libsumo::TrafficLight::getParameter(id, paramName));
                    break;
                }
                case TL_EXTERNAL_STATE: {
                    if (!MSNet::getInstance()->getTLSControl().knows(id)) {
                        throw libsumo::TraCIException("Traffic light '" + id + "' is not known");
                    }
                    MSTrafficLightLogic* tls = MSNet::getInstance()->getTLSControl().get(id).getActive();
                    const std::string& state = tls->getCurrentPhaseDef().getState();
                    const std::map<std::string, std::string>& params = tls->getParametersMap();
                    int num = 0;
                    for (std::map<std::string, std::string>::const_iterator i = params.begin(); i != params.end(); ++i) {
                        if ("connection:" == (*i).first.substr(0, 11)) {
                            ++num;
                        }
                    }

                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    server.getWrapperStorage().writeUnsignedByte(TYPE_INTEGER);
                    server.getWrapperStorage().writeInt(num * 2);
                    for (std::map<std::string, std::string>::const_iterator i = params.begin(); i != params.end(); ++i) {
                        if ("connection:" != (*i).first.substr(0, 11)) {
                            continue;
                        }
                        server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                        server.getWrapperStorage().writeString((*i).second); // foreign id
                        std::string connection = (*i).first.substr(11);
                        std::string from, to;
                        const std::string::size_type b = connection.find("->");
                        if (b == std::string::npos) {
                            from = connection;
                        } else {
                            from = connection.substr(0, b);
                            to = connection.substr(b + 2);
                        }
                        bool denotesEdge = from.find("_") == std::string::npos;
                        MSLane* fromLane = 0;
                        const MSTrafficLightLogic::LaneVectorVector& lanes = tls->getLaneVectors();
                        MSTrafficLightLogic::LaneVectorVector::const_iterator j = lanes.begin();
                        for (; j != lanes.end() && fromLane == 0;) {
                            for (MSTrafficLightLogic::LaneVector::const_iterator k = (*j).begin(); k != (*j).end() && fromLane == 0;) {
                                if (denotesEdge && (*k)->getEdge().getID() == from) {
                                    fromLane = *k;
                                } else if (!denotesEdge && (*k)->getID() == from) {
                                    fromLane = *k;
                                }
                                if (fromLane == 0) {
                                    ++k;
                                }
                            }
                            if (fromLane == 0) {
                                ++j;
                            }
                        }
                        if (fromLane == 0) {
                            return server.writeErrorStatusCmd(CMD_GET_TL_VARIABLE, "Could not find edge or lane '" + from + "' in traffic light '" + id + "'.", outputStorage);
                        }
                        int pos = (int)std::distance(lanes.begin(), j);
                        server.getWrapperStorage().writeUnsignedByte(TYPE_UBYTE);
                        server.getWrapperStorage().writeUnsignedByte(state[pos]); // state
                    }
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_TL_VARIABLE, "Get TLS Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_TL_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_TL_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_TrafficLight::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                        tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    const int variable = inputStorage.readUnsignedByte();
    if (variable != TL_PHASE_INDEX && variable != TL_PROGRAM && variable != TL_PHASE_DURATION
            && variable != TL_RED_YELLOW_GREEN_STATE && variable != TL_COMPLETE_PROGRAM_RYG
            && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "Change TLS State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    const std::string id = inputStorage.readString();
    try {
        switch (variable) {
            case TL_PHASE_INDEX: {
                int index = 0;
                if (!server.readTypeCheckingInt(inputStorage, index)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase index must be given as an integer.", outputStorage);
                }
                libsumo::TrafficLight::setPhase(id, index);
            }
            break;
            case TL_PROGRAM: {
                std::string subID;
                if (!server.readTypeCheckingString(inputStorage, subID)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The program must be given as a string.", outputStorage);
                }
                libsumo::TrafficLight::setProgram(id, subID);
            }
            break;
            case TL_PHASE_DURATION: {
                double duration = 0.;
                if (!server.readTypeCheckingDouble(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase duration must be given as a double.", outputStorage);
                }
                libsumo::TrafficLight::setPhaseDuration(id, duration);
            }
            break;
            case TL_RED_YELLOW_GREEN_STATE: {
                std::string state;
                if (!server.readTypeCheckingString(inputStorage, state)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase must be given as a string.", outputStorage);
                }
                libsumo::TrafficLight::setRedYellowGreenState(id, state);
            }
            break;
            case TL_COMPLETE_PROGRAM_RYG: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "A compound object is needed for setting a new program.", outputStorage);
                }
                //read itemNo
                inputStorage.readInt();
                libsumo::TraCILogic logic;
                int numPhases = 0;
                if (!server.readTypeCheckingString(inputStorage, logic.subID)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 1. parameter (subid) must be a string.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, logic.type)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 2. parameter (type) must be an int.", outputStorage);
                }
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 3. parameter (subparams) must be a compound object.", outputStorage);
                }
                inputStorage.readInt();
                if (!server.readTypeCheckingInt(inputStorage, logic.currentPhaseIndex)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 4. parameter (index) must be an int.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, numPhases)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 5. parameter (phase number) must be an int.", outputStorage);
                }
                for (int j = 0; j < numPhases; ++j) {
                    double duration = 0., minDuration = 0., maxDuration = 0.;
                    if (!server.readTypeCheckingDouble(inputStorage, duration)) {
                        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.1. parameter (duration) must be a double.", outputStorage);
                    }
                    if (!server.readTypeCheckingDouble(inputStorage, minDuration)) {
                        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.2. parameter (min duration) must be a double.", outputStorage);
                    }
                    if (!server.readTypeCheckingDouble(inputStorage, maxDuration)) {
                        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.3. parameter (max duration) must be a double.", outputStorage);
                    }
                    std::string state;
                    if (!server.readTypeCheckingString(inputStorage, state)) {
                        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.4. parameter (phase) must be a string.", outputStorage);
                    }
                    logic.phases.emplace_back(libsumo::TraCIPhase(duration, minDuration, maxDuration, state));
                }
                libsumo::TrafficLight::setCompleteRedYellowGreenDefinition(id, logic);
            }
            break;
            case VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
                }
                //read itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
                }
                libsumo::TrafficLight::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
