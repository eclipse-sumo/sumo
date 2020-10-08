/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
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
/// @file    Route.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/Route.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_ROUTE_VARIABLE, libsumo::CMD_SET_ROUTE_VARIABLE, libsumo::CMD_SUBSCRIBE_ROUTE_VARIABLE, libsumo::CMD_SUBSCRIBE_ROUTE_CONTEXT> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Route::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


std::vector<std::string>
Route::getEdges(const std::string& routeID) {
    return Dom::getStringVector(libsumo::VAR_EDGES, routeID);
}


int
Route::getIDCount() {
    return (int)getIDList().size();
}


std::string
Route::getParameter(const std::string& routeID, const std::string& param) {
    tcpip::Storage content;
    content.writeByte(libsumo::TYPE_STRING);
    content.writeString(param);
    return Dom::getString(libsumo::VAR_PARAMETER, routeID, &content);
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(Route)


void
Route::setParameter(const std::string& routeID, const std::string& key, const std::string& value) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(key);
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(value);
    Connection::getActive().createCommand(libsumo::CMD_SET_ROUTE_VARIABLE, libsumo::VAR_PARAMETER, routeID, &content);
    Connection::getActive().processSet(libsumo::CMD_SET_ROUTE_VARIABLE);
}


void
Route::add(const std::string& routeID, const std::vector<std::string>& edgeIDs) {
    Dom::setStringVector(libsumo::CMD_SET_ROUTE_VARIABLE, routeID, edgeIDs);
}
}


/****************************************************************************/
