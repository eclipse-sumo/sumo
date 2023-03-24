/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_Lane.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Mario Krumnow
/// @author  Leonhard Luecken
/// @author  Mirko Barthauer
/// @date    07.05.2009
///
// APIs for getting/setting lane values via TraCI
/****************************************************************************/
#include <config.h>

#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/transportables/MSTransportable.h>
#include <libsumo/Lane.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/StorageHelper.h>
#include "TraCIServer.h"
#include "TraCIServerAPI_Lane.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Lane::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_LANE_VARIABLE, variable, id);
    try {
        if (!libsumo::Lane::handleVariable(id, variable, &server, &inputStorage)) {
            switch (variable) {
                case libsumo::LANE_LINKS: {
                    server.getWrapperStorage().writeUnsignedByte(libsumo::TYPE_COMPOUND);
                    const std::vector<libsumo::TraCIConnection> links = libsumo::Lane::getLinks(id);
                    tcpip::Storage tempContent;
                    int cnt = 0;
                    tempContent.writeUnsignedByte(libsumo::TYPE_INTEGER);
                    tempContent.writeInt((int) links.size());
                    ++cnt;
                    for (std::vector<libsumo::TraCIConnection>::const_iterator i = links.begin(); i != links.end(); ++i) {
                        // approached non-internal lane (if any)
                        tempContent.writeUnsignedByte(libsumo::TYPE_STRING);
                        tempContent.writeString(i->approachedLane);
                        ++cnt;
                        // approached "via", internal lane (if any)
                        tempContent.writeUnsignedByte(libsumo::TYPE_STRING);
                        tempContent.writeString(i->approachedInternal);
                        ++cnt;
                        // priority
                        tempContent.writeUnsignedByte(libsumo::TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->hasPrio);
                        ++cnt;
                        // opened
                        tempContent.writeUnsignedByte(libsumo::TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->isOpen);
                        ++cnt;
                        // approaching foe
                        tempContent.writeUnsignedByte(libsumo::TYPE_UBYTE);
                        tempContent.writeUnsignedByte(i->hasFoe);
                        ++cnt;
                        // state (not implemented, yet)
                        tempContent.writeUnsignedByte(libsumo::TYPE_STRING);
                        tempContent.writeString(i->state);
                        ++cnt;
                        // direction
                        tempContent.writeUnsignedByte(libsumo::TYPE_STRING);
                        tempContent.writeString(i->direction);
                        ++cnt;
                        // length
                        tempContent.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                        tempContent.writeDouble(i->length);
                        ++cnt;
                    }
                    server.getWrapperStorage().writeInt(cnt);
                    server.getWrapperStorage().writeStorage(tempContent);
                    break;
                }
                case libsumo::VAR_FOES: {
                    const std::string toLane = StoHelp::readTypedString(inputStorage, "Foe retrieval requires a string.");
                    StoHelp::writeTypedStringList(server.getWrapperStorage(), toLane == "" ? libsumo::Lane::getInternalFoes(id) : libsumo::Lane::getFoes(id, toLane));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_LANE_VARIABLE, "Get Lane Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_LANE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_LANE_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Lane::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_MAXSPEED && variable != libsumo::VAR_LENGTH && variable != libsumo::LANE_ALLOWED && variable != libsumo::LANE_DISALLOWED
            && variable != libsumo::VAR_PARAMETER && variable != libsumo::LANE_CHANGES) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_LANE_VARIABLE, "Change Lane State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    MSLane* l = MSLane::dictionary(id);
    if (l == nullptr) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_LANE_VARIABLE, "Lane '" + id + "' is not known", outputStorage);
    }
    try {
        // process
        switch (variable) {
            case libsumo::VAR_MAXSPEED: {
                const double value = StoHelp::readTypedDouble(inputStorage, "The speed must be given as a double.");
                libsumo::Lane::setMaxSpeed(id, value);
                break;
            }
            case libsumo::VAR_FRICTION: {
                const double value = StoHelp::readTypedDouble(inputStorage, "The friction must be given as a double.");
                libsumo::Lane::setFriction(id, value);
                break;
            }
            case libsumo::VAR_LENGTH: {
                const double value = StoHelp::readTypedDouble(inputStorage, "The length must be given as a double.");
                libsumo::Lane::setLength(id, value);
                break;
            }
            case libsumo::LANE_ALLOWED: {
                const std::vector<std::string> classes = StoHelp::readTypedStringList(inputStorage, "Allowed vehicle classes must be given as a list of strings.");
                libsumo::Lane::setAllowed(id, classes);
                break;
            }
            case libsumo::LANE_DISALLOWED: {
                const std::vector<std::string> classes = StoHelp::readTypedStringList(inputStorage, "Not allowed vehicle classes must be given as a list of strings.");
                libsumo::Lane::setDisallowed(id, classes);
                break;
            }
            case libsumo::LANE_CHANGES: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting lane change permissions.");
                const std::vector<std::string> classes = StoHelp::readTypedStringList(inputStorage, "Vehicle classes allowed to change lane must be given as a list of strings.");
                const int direction = StoHelp::readTypedByte(inputStorage, "The lane change direction must be given as an integer.");
                libsumo::Lane::setChangePermissions(id, classes, direction);
                break;
            }
            case libsumo::VAR_PARAMETER: {
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Lane::setParameter(id, name, value);
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_LANE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_LANE_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
