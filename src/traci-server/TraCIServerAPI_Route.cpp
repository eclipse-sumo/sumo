/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Route.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting route values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include "TraCIConstants.h"
#include <libsumo/Route.h>
#include "TraCIServerAPI_Route.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Route::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                 tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_EDGES && variable != ID_COUNT && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_GET_ROUTE_VARIABLE, "Get Route Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_ROUTE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::Route::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Route::getIDCount());
                break;
            case VAR_EDGES:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::Route::getEdges(id));
                break;
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_ROUTE_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::Route::getParameter(id, paramName));
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_ROUTE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_ROUTE_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Route::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                 tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != ADD && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "Change Route State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();

    try {
        // process
        switch (variable) {
            case ADD: {
                std::vector<std::string> edgeIDs;
                if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                    return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "A string list is needed for adding a new route.", outputStorage);
                }
                libsumo::Route::add(id, edgeIDs);
            }
            break;
            case VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
                }
                //read itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
                }
                libsumo::Route::setParameter(id, name, value);
            }
            break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_ROUTE_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_ROUTE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/
