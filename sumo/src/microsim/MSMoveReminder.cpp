/****************************************************************************/
/// @file    MSMoveReminder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2008-10-27
/// @version $Id$
///
// Something on a lane to be noticed about vehicle movement
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "MSLane.h"
#include "MSMoveReminder.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSMoveReminder::MSMoveReminder(const std::string& description, MSLane* const lane, const bool doAdd) :
    myLane(lane),
    myDescription(description) {
    if (myLane != 0 && doAdd) {
        // add reminder to lane
        myLane->addMoveReminder(this);
    }
}


#ifdef HAVE_INTERNAL
void
MSMoveReminder::updateDetector(SUMOVehicle& veh, SUMOReal entryPos, SUMOReal leavePos,
                               SUMOTime entryTime, SUMOTime currentTime, SUMOTime leaveTime) {
    // each vehicle is tracked linearly across its segment. For each vehicle,
    // the time and position of the previous call are maintained and only
    // the increments are sent to notifyMoveInternal
    if (entryTime > currentTime) {
        return; // calibrator may insert vehicles a tiny bit into the future; ignore those
    }
    std::map<SUMOVehicle*, std::pair<SUMOTime, SUMOReal> >::iterator j = myLastVehicleUpdateValues.find(&veh);
    if (j != myLastVehicleUpdateValues.end()) {
        // the vehicle already has reported its values before; use these
        // however, if this was called from prepareDetectorForWriting the time
        // only has a resolution of DELTA_T and might be invalid
        const SUMOTime previousEntryTime = j->second.first;
        if (previousEntryTime <= currentTime) {
            entryTime = previousEntryTime;
            entryPos = j->second.second;
        }
        myLastVehicleUpdateValues.erase(j);
    }
    assert(entryTime <= currentTime);
    if ((entryTime < leaveTime) && (entryPos < leavePos)) {
        const SUMOReal timeOnLane = STEPS2TIME(currentTime - entryTime);
        const SUMOReal speed = (leavePos - entryPos) / STEPS2TIME(leaveTime - entryTime);
        myLastVehicleUpdateValues[&veh] = std::pair<SUMOTime, SUMOReal>(currentTime, entryPos + speed * timeOnLane);
        assert(timeOnLane >= 0);
        assert(speed >= 0);
        notifyMoveInternal(veh, timeOnLane, speed);
    } else {
        // it would be natrual to
        // assert(entryTime == leaveTime);
        // assert(entryPos == leavePos);
        // However, in the presence of calibrators, vehicles may jump a bit
        myLastVehicleUpdateValues[&veh] = std::pair<SUMOTime, SUMOReal>(leaveTime, leavePos);
    }

}
#endif
/****************************************************************************/

