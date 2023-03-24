/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_Edge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Jerome Haerri
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Mario Krumnow
/// @author  Gregor Laemmel
/// @date    Sept 2002
///
// APIs for getting/setting edge values via TraCI
/****************************************************************************/
#include <config.h>

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/transportables/MSPerson.h>
#include <libsumo/TraCIConstants.h>
#include "TraCIServerAPI_Edge.h"
#include <microsim/MSEdgeWeightsStorage.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <libsumo/StorageHelper.h>
#include <libsumo/Edge.h>


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Edge::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_EDGE_VARIABLE, variable, id);
    try {
        if (!libsumo::Edge::handleVariable(id, variable, &server, &inputStorage)) {
            switch (variable) {
                case libsumo::VAR_EDGE_TRAVELTIME: {
                    const double time = StoHelp::readTypedDouble(inputStorage, "The message must contain the time definition.");
                    StoHelp::writeTypedDouble(server.getWrapperStorage(), libsumo::Edge::getAdaptedTraveltime(id, time));
                    break;
                }
                case libsumo::VAR_EDGE_EFFORT: {
                    const double time = StoHelp::readTypedDouble(inputStorage, "The message must contain the time definition.");
                    StoHelp::writeTypedDouble(server.getWrapperStorage(), libsumo::Edge::getEffort(id, time));
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(libsumo::CMD_GET_EDGE_VARIABLE,
                                                      "Get Edge Variable: unsupported variable " + toHex(variable, 2)
                                                      + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_EDGE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_EDGE_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_Edge::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                tcpip::Storage& outputStorage) {
    std::string warning; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_EDGE_TRAVELTIME
            && variable != libsumo::VAR_EDGE_EFFORT
            && variable != libsumo::VAR_MAXSPEED
            && variable != libsumo::LANE_ALLOWED
            && variable != libsumo::LANE_DISALLOWED
            && variable != libsumo::VAR_FRICTION
            && variable != libsumo::VAR_PARAMETER) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_EDGE_VARIABLE,
                                          "Change Edge State: unsupported variable " + toHex(variable, 2)
                                          + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    try {
        // process
        switch (variable) {
            case libsumo::LANE_ALLOWED: {
                // read and set allowed vehicle classes
                const std::vector<std::string> classes = StoHelp::readTypedStringList(inputStorage, "Allowed vehicle classes must be given as a list of strings.");
                libsumo::Edge::setAllowed(id, classes);
                break;
            }
            case libsumo::LANE_DISALLOWED: {
                // read and set disallowed vehicle classes
                const std::vector<std::string> classes = StoHelp::readTypedStringList(inputStorage, "Not allowed vehicle classes must be given as a list of strings.");
                libsumo::Edge::setDisallowed(id, classes);
                break;
            }
            case libsumo::VAR_EDGE_TRAVELTIME: {
                // read and set travel time
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Setting travel time requires a compound object.");
                if (parameterCount == 3) {
                    // bound by time
                    const double begTime = StoHelp::readTypedDouble(inputStorage, "The first variable must be the begin time given as double.");
                    const double endTime = StoHelp::readTypedDouble(inputStorage, "The second variable must be the end time given as double.");
                    const double value = StoHelp::readTypedDouble(inputStorage, "The third variable must be the value given as double.");
                    libsumo::Edge::adaptTraveltime(id, value, begTime, endTime);
                } else if (parameterCount == 1) {
                    // unbound
                    const double value = StoHelp::readTypedDouble(inputStorage, "The variable must be the value given as double.");
                    libsumo::Edge::adaptTraveltime(id, value, 0., std::numeric_limits<double>::max());
                } else {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_EDGE_VARIABLE,
                                                      "Setting travel time requires either begin time, end time, and value, or only value as parameter.",
                                                      outputStorage);
                }
                break;
            }
            case libsumo::VAR_EDGE_EFFORT: {
                // read and set effort
                const int parameterCount = StoHelp::readCompound(inputStorage, -1, "Setting effort requires a compound object.");
                if (parameterCount == 3) {
                    // bound by time
                    const double begTime = StoHelp::readTypedDouble(inputStorage, "The first variable must be the begin time given as double.");
                    const double endTime = StoHelp::readTypedDouble(inputStorage, "The second variable must be the end time given as double.");
                    const double value = StoHelp::readTypedDouble(inputStorage, "The third variable must be the value given as double.");
                    libsumo::Edge::setEffort(id, value, begTime, endTime);
                } else if (parameterCount == 1) {
                    // unbound
                    const double value = StoHelp::readTypedDouble(inputStorage, "The variable must be the value given as double.");
                    libsumo::Edge::setEffort(id, value, 0., std::numeric_limits<double>::max());
                } else {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_EDGE_VARIABLE,
                                                      "Setting effort requires either begin time, end time, and value, or only value as parameter.",
                                                      outputStorage);
                }
                break;
            }
            case libsumo::VAR_MAXSPEED: {
                // read and set max. speed
                const double value = StoHelp::readTypedDouble(inputStorage, "The speed must be given as a double.");
                libsumo::Edge::setMaxSpeed(id, value);
                break;
            }
            case libsumo::VAR_FRICTION: {
                // read and set friction for entire edge
                const double value = StoHelp::readTypedDouble(inputStorage, "The friction must be given as a double.");
                libsumo::Edge::setFriction(id, value);
                break;
            }
            case libsumo::VAR_PARAMETER: {
                // read and check item number
                StoHelp::readCompound(inputStorage, 2, "A compound object of size 2 is needed for setting a parameter.");
                const std::string name = StoHelp::readTypedString(inputStorage, "The name of the parameter must be given as a string.");
                const std::string value = StoHelp::readTypedString(inputStorage, "The value of the parameter must be given as a string.");
                libsumo::Edge::setParameter(id, name, value);
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_EDGE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_EDGE_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
