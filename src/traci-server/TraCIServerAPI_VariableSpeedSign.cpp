/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_VariableSpeedSign.cpp
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// APIs for getting/setting variable speed sign values via TraCI
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSStoppingPlace.h>
#include <libsumo/VariableSpeedSign.h>
#include <libsumo/TraCIConstants.h>
#include "TraCIServerAPI_VariableSpeedSign.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_VariableSpeedSign::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(libsumo::RESPONSE_GET_VARIABLESPEEDSIGN_VARIABLE, variable, id);
    try {
        if (!libsumo::VariableSpeedSign::handleVariable(id, variable, &server, &inputStorage)) {
            return server.writeErrorStatusCmd(libsumo::CMD_GET_VARIABLESPEEDSIGN_VARIABLE, "Get VariableSpeedSign Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_GET_VARIABLESPEEDSIGN_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_GET_VARIABLESPEEDSIGN_VARIABLE, libsumo::RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


bool
TraCIServerAPI_VariableSpeedSign::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != libsumo::VAR_PARAMETER) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE, "Change VariableSpeedSign State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();

    try {
        // process
        switch (variable) {
            case libsumo::VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != libsumo::TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
                }
                //read itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
                }
                libsumo::VariableSpeedSign::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE, libsumo::RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
