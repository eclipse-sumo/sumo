/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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
