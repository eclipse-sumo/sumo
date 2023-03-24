/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    Command_RouteReplacement.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    15 Feb 2004
///
// Writes the state of the tls to a file (in each second)
/****************************************************************************/
#include <config.h>

#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSRoute.h>
#include <microsim/MSGlobals.h>
#include "Command_RouteReplacement.h"


// ===========================================================================
// method definitions
// ===========================================================================
Command_RouteReplacement::Command_RouteReplacement(const std::string& vehID, ConstMSRoutePtr route) :
    myVehID(vehID),
    myRoute(route) {
}


Command_RouteReplacement::~Command_RouteReplacement() { }


SUMOTime
Command_RouteReplacement::execute(SUMOTime /*currentTime*/) {
    SUMOVehicle* veh = MSNet::getInstance()->getVehicleControl().getVehicle(myVehID);
    // if the vehicle is not available anymore, silently ignore replacement
    if (veh != nullptr) {
        std::string errorPrefix = ("Replayed route replacement failed for vehicle '"
                                   + veh->getID() + "' route=" + myRoute->getID() + " time=" + time2string(SIMSTEP));
        std::string msg;
        if (!veh->hasValidRoute(msg, myRoute)) {
            WRITE_WARNING("Invalid route replacement for vehicle '" + veh->getID() + "'. " + msg);
            if (MSGlobals::gCheckRoutes) {
                throw ProcessError(errorPrefix + ".");
            }
        }
        std::string errorMsg;
        if (!veh->replaceRoute(myRoute, "replayRerouting", veh->getLane() == nullptr,
                               veh->getRoute().getReplacedIndex(), true, true, &errorMsg)) {
            throw ProcessError(errorPrefix + " (" + errorMsg + ").");
        }
    }
    return 0;
}


/****************************************************************************/
