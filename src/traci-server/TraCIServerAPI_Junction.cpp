/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Junction.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting junction values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSJunctionControl.h>
#include "TraCIServer.h"
#include <libsumo/Junction.h>
#include "TraCIServerAPI_Junction.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Junction::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                    tcpip::Storage& outputStorage) {
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_POSITION && variable != ID_COUNT && variable != VAR_SHAPE) {
        return server.writeErrorStatusCmd(CMD_GET_JUNCTION_VARIABLE, "Get Junction Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_JUNCTION_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);

    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::Junction::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::Junction::getIDCount());
                break;
            case VAR_POSITION: {
                tempMsg.writeUnsignedByte(POSITION_2D);
                libsumo::TraCIPosition p = libsumo::Junction::getPosition(id);
                tempMsg.writeDouble(p.x);
                tempMsg.writeDouble(p.y);
                break;
            }
            case VAR_SHAPE: {
                tempMsg.writeUnsignedByte(TYPE_POLYGON);
                const libsumo::TraCIPositionVector shp = libsumo::Junction::getShape(id);
                tempMsg.writeUnsignedByte(MIN2(255, (int) shp.size()));
                for (int iPoint = 0; iPoint < MIN2(255, (int) shp.size()); ++iPoint) {
                    tempMsg.writeDouble(shp[iPoint].x);
                    tempMsg.writeDouble(shp[iPoint].y);
                }
                break;
            }
            default:
                break;

        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_JUNCTION_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);

    return true;
}


bool
TraCIServerAPI_Junction::getPosition(const std::string& id, Position& p) {
    MSJunction* j = libsumo::Junction::getJunction(id);
    if (j == 0) {
        return false;
    }
    p = j->getPosition();
    return true;
}


/****************************************************************************/

