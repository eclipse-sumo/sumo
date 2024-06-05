/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2021 German Aerospace Center (DLR) and others.
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
#pragma once
#include <config.h>

#include <microsim/MSMoveReminder.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSRailSignal;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDriveWay
 */
class MSDriveWay : public MSMoveReminder {
public:
    /** @brief Constructor
     */
    MSDriveWay(const std::vector<MSLane*> lanes);

    /// @brief Destructor
    virtual ~MSDriveWay() {};

    bool notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* enteredLane); 
    bool notifyLeave(SUMOTrafficObject& veh, double lastPos, Notification reason, const MSLane* enteredLane = 0); 
    bool notifyLeaveBack(SUMOTrafficObject& veh, Notification reason, const MSLane* leftLane); 

private:

    std::vector<MSLane*> myLanes;
    std::set<SUMOVehicle*> myTrains;
};


