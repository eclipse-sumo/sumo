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
/// @file    Rerouter.cpp
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/TraCIConstants.h>
#include <libsumo/Rerouter.h>
#include "Domain.h"

namespace libtraci {

typedef Domain<libsumo::CMD_GET_REROUTER_VARIABLE, libsumo::CMD_SET_REROUTER_VARIABLE> Dom;

// ===========================================================================
// static member initializations
// ===========================================================================
//SubscriptionResults Rerouter::mySubscriptionResults;
//ContextSubscriptionResults Rerouter::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Rerouter::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}

int
Rerouter::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


LIBTRACI_PARAMETER_IMPLEMENTATION(Rerouter, REROUTER)


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Rerouter, REROUTER)


}


/****************************************************************************/
