/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    Junction.cpp
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
#include <libsumo/TraCIConstants.h>
#include <libsumo/Junction.h>
#include "Domain.h"
#include <libsumo/TraCIDefs.h>

namespace libtraci {

typedef Domain<libsumo::CMD_GET_JUNCTION_VARIABLE, libsumo::CMD_SET_JUNCTION_VARIABLE> Dom;


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
Junction::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
Junction::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


libsumo::TraCIPosition
Junction::getPosition(const std::string& junctionID, bool includeZ) {
    return includeZ ? Dom::getPos3D(libsumo::VAR_POSITION3D, junctionID) : Dom::getPos(libsumo::VAR_POSITION, junctionID);
}


libsumo::TraCIPositionVector
Junction::getShape(const std::string& junctionID) {
    return Dom::getPolygon(libsumo::VAR_SHAPE, junctionID);
}


LIBTRACI_PARAMETER_IMPLEMENTATION(Junction, JUNCTION)
LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Junction, JUNCTION)

}


/****************************************************************************/
