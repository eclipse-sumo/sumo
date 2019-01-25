/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Lane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Mario Krumnow
/// @author  Leonhard Luecken
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting lane values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSTransportable.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Lane.h"
#include <libsumo/Lane.h>


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Lane::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_LANE_VARIABLE, variable, id);
    try {
        if (!libsumo::Lane::handleVariable(id, variable, &server)) {
            switch (variable) {
                case LANE_LINKS: {
                    server.getWrapperStorage().writeUnsignedByte(TYPE_COMPOUND);
                    const std::vector<libsumo::TraCIConnection> links = libsumo::Lane::getLinks(id);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt((int) links.size());
                    ++cnt;
                    for (std::vector<libsumo::TraCIConnection>::const_iterator i = links.begin(); i != links.end(); ++i) {
                        // approached non-internal lane (if any)
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->approachedLane);
                        ++cnt;
                        // approached "via", internal lane (if any)
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->approachedInternal);
                        ++cnt;
                        // priority
                        tempContent.writeUnsignedByte(TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->hasPrio);
                        ++cnt;
                        // opened
                        tempContent.writeUnsignedByte(TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->isOpen);
                        ++cnt;
                        // approaching foe
                        tempContent.writeUnsignedByte(TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->hasFoe);
                        ++cnt;
                        // state (not implemented, yet)
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->state);
                        ++cnt;
                        // direction
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(i->direction);
                        ++cnt;
                        // length
                        tempContent.writeUnsignedByte(TYPE_DOUBLE);
                        tempContent.writeDouble(i->length);
                        ++cnt;
                    }
                    server.getWrapperStorage().writeInt(cnt);
                    server.getWrapperStorage().writeStorage(tempContent);
                    break;
                }
                case VAR_FOES: {
                    std::string toLane;
                    if (!server.readTypeCheckingString(inputStorage, toLane)) {
                        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "foe retrieval requires a string.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRINGLIST);
                    if (toLane == "") {
                        server.getWrapperStorage().writeStringList(libsumo::Lane::getInternalFoes(id));
                    } else {
                        server.getWrapperStorage().writeStringList(libsumo::Lane::getFoes(id, toLane));
                    }
                    break;
                }
                case VAR_SHAPE:
                    server.writePositionVector(server.getWrapperStorage(), libsumo::Lane::getShape(id));
                    break;
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    server.getWrapperStorage().writeUnsignedByte(TYPE_STRING);
                    server.getWrapperStorage().writeString(libsumo::Lane::getParameter(id, paramName));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, "Get Lane Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_LANE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Lane::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_MAXSPEED && variable != VAR_LENGTH && variable != LANE_ALLOWED && variable != LANE_DISALLOWED
            && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Change Lane State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    MSLane* l = MSLane::dictionary(id);
    if (l == nullptr) {
        return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Lane '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        case VAR_MAXSPEED: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The speed must be given as a double.", outputStorage);
            }
            libsumo::Lane::setMaxSpeed(id, value);
        }
        break;
        case VAR_LENGTH: {
            double value = 0;
            if (!server.readTypeCheckingDouble(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The length must be given as a double.", outputStorage);
            }
            libsumo::Lane::setLength(id, value);
        }
        break;
        case LANE_ALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Allowed classes must be given as a list of strings.", outputStorage);
            }
            libsumo::Lane::setAllowed(id, classes);
        }
        break;
        case LANE_DISALLOWED: {
            std::vector<std::string> classes;
            if (!server.readTypeCheckingStringList(inputStorage, classes)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "Not allowed classes must be given as a list of strings.", outputStorage);
            }
            libsumo::Lane::setDisallowed(id, classes);
        }
        break;
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_LANE_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
            }
            libsumo::Lane::setParameter(id, name, value);
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/

