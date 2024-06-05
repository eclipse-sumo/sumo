/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSDriveWay.h
/// @author  Jakob Erdmann
/// @date    December 2021
///
// A sequende of rail tracks (lanes) that may be used as a "set route" (Fahrstraße)
/****************************************************************************/
#include <config.h>
#include <cassert>
#include <utility>

#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/common/StringUtils.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include "MSRailSignal.h"
#include "MSDriveWay.h"
#include "MSRailSignalControl.h"

//#define DEBUG_PASSED
//#define DEBUG_LANE

// ===========================================================================
// static value definitions
// ===========================================================================

// ===========================================================================
// MSDriveWay method definitions
// ===========================================================================


MSDriveWay::MSDriveWay(const std::vector<MSLane*> lanes) :
    MSMoveReminder(lanes.front()->getEdge().getFromJunction()->getID() + "_" + lanes.back()->getEdge().getToJunction()->getID()),
    myLanes(lanes)
{
    for (MSLane* lane : myLanes) {
        lane->addMoveReminder(this);
    }
}

bool
MSDriveWay::notifyEnter(SUMOTrafficObject& veh, Notification /*reason*/, const MSLane* /*enteredLane*/) {
    if (veh.isVehicle()) {
        myTrains.insert(&dynamic_cast<SUMOVehicle&>(veh));
        return true;
    } else {
        return false;
    }
}


bool
MSDriveWay::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/, Notification reason, const MSLane* /*enteredLane*/) {
    if (veh.isVehicle()) {
        if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
            myTrains.erase(&dynamic_cast<SUMOVehicle&>(veh));
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}


bool
MSDriveWay::notifyLeaveBack(SUMOTrafficObject& veh, Notification /*reason*/, const MSLane* leftLane) {
    if (veh.isVehicle()) {
        // leaving network with departure, teleport etc
        if (leftLane == myLanes.back()) {
            myTrains.erase(&dynamic_cast<SUMOVehicle&>(veh));
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}


/****************************************************************************/
