/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSMoveReminder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2008-10-27
///
// Something on a lane to be noticed about vehicle movement
/****************************************************************************/
#include <config.h>

#include <string>
#include "MSLane.h"
#include "MSMoveReminder.h"

StringBijection<MSMoveReminder::Notification>::Entry MSMoveReminder::NotificationValues[] = {
    {"departed",               NOTIFICATION_DEPARTED},
    {"junction",               NOTIFICATION_JUNCTION},
    {"segment",                NOTIFICATION_SEGMENT},
    {"laneChange",             NOTIFICATION_LANE_CHANGE},
    {"loadState",              NOTIFICATION_LOAD_STATE},
    {"teleport",               NOTIFICATION_TELEPORT},
    {"teleportContinuation",   NOTIFICATION_TELEPORT_CONTINUATION},
    {"parking",                NOTIFICATION_PARKING},
    {"reroute",                NOTIFICATION_REROUTE},
    {"parkingReroute",         NOTIFICATION_PARKING_REROUTE},
    {"arrived",                NOTIFICATION_ARRIVED},
    {"teleportArrived",        NOTIFICATION_TELEPORT_ARRIVED},
    {"vaporizedCalibrator",    NOTIFICATION_VAPORIZED_CALIBRATOR},
    {"vaporizedCollision",     NOTIFICATION_VAPORIZED_COLLISION},
    {"vaporizedTraCI",         NOTIFICATION_VAPORIZED_TRACI},
    {"vaporizedGUI",           NOTIFICATION_VAPORIZED_GUI},
    {"vaporizer",              NOTIFICATION_VAPORIZED_VAPORIZER},
    {"vaporizedBreakdown",     NOTIFICATION_VAPORIZED_BREAKDOWN},
    {"none",                   NOTIFICATION_NONE}
};

StringBijection<MSMoveReminder::Notification> MSMoveReminder::Notifications(
    MSMoveReminder::NotificationValues, MSMoveReminder::NOTIFICATION_NONE, false);

// ===========================================================================
// method definitions
// ===========================================================================
MSMoveReminder::MSMoveReminder(const std::string& description, MSLane* const lane, const bool doAdd) :
    myLane(lane),
    myDescription(description)
#ifdef HAVE_FOX
    , myNotificationMutex(true)
#endif
{
    if (myLane != nullptr && doAdd) {
        // add reminder to lane
        myLane->addMoveReminder(this);
    }
}


void
MSMoveReminder::updateDetector(SUMOTrafficObject& veh, double entryPos, double leavePos,
                               SUMOTime entryTime, SUMOTime currentTime, SUMOTime leaveTime,
                               bool cleanUp) {
    // each vehicle is tracked linearly across its segment. For each vehicle,
    // the time and position of the previous call are maintained and only
    // the increments are sent to notifyMoveInternal
    if (entryTime > currentTime) {
        return; // calibrator may insert vehicles a tiny bit into the future; ignore those
    }
    auto j = myLastVehicleUpdateValues.find(veh.getNumericalID());
    if (j != myLastVehicleUpdateValues.end()) {
        // the vehicle already has reported its values before; use these
        // however, if this was called from prepareDetectorForWriting the time
        // only has a resolution of DELTA_T and might be invalid
        const SUMOTime previousUpdateTime = j->second.first;
        if (previousUpdateTime <= currentTime) {
            entryTime = previousUpdateTime;
            entryPos = j->second.second;
        }
    }
    assert(entryTime <= currentTime);
    if ((entryTime < leaveTime) && (entryPos <= leavePos)) {
        const double timeOnLane = STEPS2TIME(currentTime - entryTime);
        const double speed = (leavePos - entryPos) / STEPS2TIME(leaveTime - entryTime);
        myLastVehicleUpdateValues[veh.getNumericalID()] = std::pair<SUMOTime, double>(currentTime, entryPos + speed * timeOnLane);
        assert(timeOnLane >= 0);
        notifyMoveInternal(veh, timeOnLane, timeOnLane, speed, speed, speed * timeOnLane, speed * timeOnLane, 0.);
    } else {
        // it would be natural to
        // assert(entryTime == leaveTime);
        // assert(entryPos == leavePos);
        // However, in the presence of calibrators, vehicles may jump a bit
        myLastVehicleUpdateValues[veh.getNumericalID()] = std::pair<SUMOTime, double>(leaveTime, leavePos);
    }
    if (cleanUp) {
        // clean up after the vehicle has left the area of this reminder
        removeFromVehicleUpdateValues(veh);
    }
}

void
MSMoveReminder::saveReminderState(OutputDevice& out, const SUMOTrafficObject& veh) {
    auto j = myLastVehicleUpdateValues.find(veh.getNumericalID());
    if (j != myLastVehicleUpdateValues.end()) {
        out.openTag(SUMO_TAG_REMINDER);
        out.writeAttr(SUMO_ATTR_ID, getDescription());
        out.writeAttr(SUMO_ATTR_TIME, (*j).second.first);
        out.writeAttr(SUMO_ATTR_POSITION, (*j).second.second);
        out.closeTag();
    }
}


void
MSMoveReminder::loadReminderState(long long int numID, SUMOTime time, double pos) {
    myLastVehicleUpdateValues[numID] = std::make_pair(time, pos);
}


void
MSMoveReminder::removeFromVehicleUpdateValues(SUMOTrafficObject& veh) {
    myLastVehicleUpdateValues.erase(veh.getNumericalID());
}


/****************************************************************************/
