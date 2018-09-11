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
#include <config.h>

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
    const int variable = inputStorage.readUnsignedByte();
    const std::string id = inputStorage.readString();
    server.initWrapper(RESPONSE_GET_JUNCTION_VARIABLE, variable, id);
    try {
        if (!libsumo::Junction::handleVariable(id, variable, &server)) {
            switch (variable) {
                case VAR_SHAPE: {
                    server.getWrapperStorage().writeUnsignedByte(TYPE_POLYGON);
                    const libsumo::TraCIPositionVector shp = libsumo::Junction::getShape(id);
                    server.getWrapperStorage().writeUnsignedByte(MIN2(255, (int)shp.size()));
                    for (int iPoint = 0; iPoint < MIN2(255, (int)shp.size()); ++iPoint) {
                        server.getWrapperStorage().writeDouble(shp[iPoint].x);
                        server.getWrapperStorage().writeDouble(shp[iPoint].y);
                    }
                    break;
                }
                default:
                    return server.writeErrorStatusCmd(CMD_GET_JUNCTION_VARIABLE, "Get Junction Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
            }
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_JUNCTION_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_JUNCTION_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, server.getWrapperStorage());
    return true;
}


/****************************************************************************/
