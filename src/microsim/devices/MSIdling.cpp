/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2020 German Aerospace Center (DLR) and others.
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
/// @file    MSIdling.cpp
/// @author  Jakob Erdmann
/// @date    17.08.2020
///
// An algorithm that performs Idling for the taxi device
/****************************************************************************/
#include <config.h>

#include <limits>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/transportables/MSTransportable.h>
#include "MSRoutingEngine.h"
#include "MSIdling.h"

//#define DEBUG_RESERVATION
//#define DEBUG_Idling
//#define DEBUG_SERVABLE
//#define DEBUG_TRAVELTIME
//#define DEBUG_DETOUR
//#define DEBUG_COND2(obj) (obj->getID() == "p0")
#define DEBUG_COND2(obj) (true)


// ===========================================================================
// MSIdling_stop methods
// ===========================================================================

void
MSIdling_Stop::idle(MSDevice_Taxi* taxi) {
    MSVehicle& veh = dynamic_cast<MSVehicle&>(taxi->getHolder());
    if (!veh.hasStops()) {
        //std::cout << SIMTIME << " MSIdling_Stop add stop\n";
        // add stop
        std::string errorOut;
        const double brakeGap = veh.getCarFollowModel().brakeGap(veh.getSpeed());
        std::pair<const MSLane*, double> stopPos = veh.getLanePosAfterDist(brakeGap);
        if (stopPos.first != nullptr) {
            SUMOVehicleParameter::Stop stop;
            stop.lane = stopPos.first->getID();
            stop.startPos = stopPos.second;
            stop.endPos = stopPos.second + POSITION_EPS;
            stop.triggered = true;
            stop.actType = "idling";
            veh.addTraciStop(stop, errorOut);
            if (errorOut != "") {
                WRITE_WARNING(errorOut);
            }
        } else {
            WRITE_WARNING("Idle taxi '" + veh.getID() + "' could not stop within " + toString(brakeGap) + "m");
        }
    } else {
        //std::cout << SIMTIME << " MSIdling_Stop reuse stop\n";
        MSVehicle::Stop& stop = veh.getNextStop();
        stop.triggered = true;
    }
}


/****************************************************************************/
