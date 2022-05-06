/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2022 German Aerospace Center (DLR) and others.
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
/// @file    FrictionCoefficientSign.cpp
/// @author  Jakob Erdmann
/// @author  Tomas Weber
/// @date    10.03.2022
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/FrictionCoefficientSign.h>
#include "Connection.h"
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_FRICTIONCOEFFICIENT_VARIABLE, libsumo::CMD_SET_FRICTIONCOEFFICIENT_VARIABLE> Dom;

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
FrictionCoefficientSign::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}

int
FrictionCoefficientSign::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}

std::vector<std::string>
FrictionCoefficientSign::getLanes(const std::string& cofID) {
    return Dom::getStringVector(libsumo::VAR_LANES, cofID);
}

LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(FrictionCoefficientSign, FRICTIONCOEFFICIENT)
LIBTRACI_PARAMETER_IMPLEMENTATION(FrictionCoefficientSign, FRICTIONCOEFFICIENT)


}


/****************************************************************************/
