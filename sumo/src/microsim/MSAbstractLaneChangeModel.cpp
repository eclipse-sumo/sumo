/****************************************************************************/
/// @file    MSAbstractLaneChangeModel.h
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Interface for lane-change models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

#include "MSAbstractLaneChangeModel.h"
#include "MSLCM_DK2008.h"
#include "MSLCM_LC2013.h"
#include "MSLCM_JE2013.h"
#include "MSNet.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSGlobals.h"

/* -------------------------------------------------------------------------
 * MSAbstractLaneChangeModel-methods
 * ----------------------------------------------------------------------- */

MSAbstractLaneChangeModel*
MSAbstractLaneChangeModel::build(LaneChangeModel lcm, MSVehicle& v) {
    switch (lcm) {
        case LCM_DK2008:
            return new MSLCM_DK2008(v);
        case LCM_LC2013:
            return new MSLCM_LC2013(v);
        case LCM_JE2013:
            return new MSLCM_JE2013(v);
        default:
            throw ProcessError("Lane change model '" + toString(lcm) + "' not implemented");
    }
}


MSAbstractLaneChangeModel::MSAbstractLaneChangeModel(MSVehicle& v) :
    myVehicle(v),
    myOwnState(0),
    myLastLaneChangeOffset(0),
    myLaneChangeCompletion(1.0),
    myLaneChangeDirection(0),
    myLaneChangeMidpointPassed(false),
    myAlreadyMoved(false),
    myShadowLane(0),
    myHaveShadow(false),
    myCarFollowModel(v.getCarFollowModel()) {
}


MSAbstractLaneChangeModel::~MSAbstractLaneChangeModel() {
    if (myShadowLane != 0 && myHaveShadow) {
        myShadowLane->removeVehicle(&myVehicle, MSMoveReminder::NOTIFICATION_VAPORIZED, false);
    }
}


bool
MSAbstractLaneChangeModel::congested(const MSVehicle* const neighLeader) {
    if (neighLeader == 0) {
        return false;
    }
    // Congested situation are relevant only on highways (maxSpeed > 70km/h)
    // and congested on German Highways means that the vehicles have speeds
    // below 60km/h. Overtaking on the right is allowed then.
    if ((myVehicle.getLane()->getSpeedLimit() <= 70.0 / 3.6) || (neighLeader->getLane()->getSpeedLimit() <= 70.0 / 3.6)) {

        return false;
    }
    if (myVehicle.congested() && neighLeader->congested()) {
        return true;
    }
    return false;
}


bool
MSAbstractLaneChangeModel::predInteraction(const MSVehicle* const leader) {
    if (leader == 0) {
        return false;
    }
    // let's check it on highways only
    if (leader->getSpeed() < (80.0 / 3.6)) {
        return false;
    }
    SUMOReal gap = leader->getPositionOnLane() - leader->getVehicleType().getLength() - myVehicle.getVehicleType().getMinGap() - myVehicle.getPositionOnLane();
    return gap < myCarFollowModel.interactionGap(&myVehicle, leader->getSpeed());
}


bool
MSAbstractLaneChangeModel::startLaneChangeManeuver(MSLane* source, MSLane* target, int direction) {
    target->enteredByLaneChange(&myVehicle);
    if (MSGlobals::gLaneChangeDuration > DELTA_T) {
        myLaneChangeCompletion = 0;
        myShadowLane = target;
        myHaveShadow = true;
        myLaneChangeMidpointPassed = false;
        myLaneChangeDirection = direction;
        continueLaneChangeManeuver(false);
        return true;
    } else {
        myVehicle.leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
        source->leftByLaneChange(&myVehicle);
        myVehicle.enterLaneAtLaneChange(target);
        myLastLaneChangeOffset = 0;
        changed();
        return false;
    }
}


void
MSAbstractLaneChangeModel::continueLaneChangeManeuver(bool moved) {
    if (moved && myHaveShadow) {
        // move shadow to next lane
        removeLaneChangeShadow();
        const int shadowDirection = myLaneChangeMidpointPassed ? -myLaneChangeDirection : myLaneChangeDirection;
        myShadowLane = myVehicle.getLane()->getParallelLane(shadowDirection);
        if (myShadowLane == 0) {
            // abort lane change
            WRITE_WARNING("Vehicle '" + myVehicle.getID() + "' could not finish continuous lane change (lane disappeared) time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            endLaneChangeManeuver();
            return;
        }
        myHaveShadow = true;
    }
    myLaneChangeCompletion += (SUMOReal)DELTA_T / (SUMOReal)MSGlobals::gLaneChangeDuration;
    if (!myLaneChangeMidpointPassed && myLaneChangeCompletion >=
            myVehicle.getLane()->getWidth() / (myVehicle.getLane()->getWidth() + myShadowLane->getWidth())) {
        // maneuver midpoint reached, swap myLane and myShadowLane
        myLaneChangeMidpointPassed = true;
        MSLane* tmp = myVehicle.getLane();
        myVehicle.leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
        myVehicle.enterLaneAtLaneChange(myShadowLane);
        myShadowLane = tmp;
        if (myVehicle.fixPosition()) {
            WRITE_WARNING("vehicle '" + myVehicle.getID() + "' set back by " + toString(myVehicle.getPositionOnLane() - myVehicle.getLane()->getLength()) +
                          "m when changing lanes on lane '" + myVehicle.getLane()->getID() + " time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
        }
        myLastLaneChangeOffset = 0;
        changed();
        myAlreadyMoved = true;
    }
    // remove shadow as soon as the vehicle leaves the original lane geometrically
    if (myLaneChangeMidpointPassed && myHaveShadow) {
        const SUMOReal sourceHalfWidth = myShadowLane->getWidth() / 2.0;
        const SUMOReal targetHalfWidth = myVehicle.getLane()->getWidth() / 2.0;
        if (myLaneChangeCompletion * (sourceHalfWidth + targetHalfWidth) - myVehicle.getVehicleType().getWidth() / 2.0 > sourceHalfWidth) {
            removeLaneChangeShadow();
        }
    }
    // finish maneuver
    if (!isChangingLanes()) {
        assert(myLaneChangeMidpointPassed);
        endLaneChangeManeuver();
    }
}


void
MSAbstractLaneChangeModel::removeLaneChangeShadow() {
    if (myShadowLane != 0 && myHaveShadow) {
        myShadowLane->removeVehicle(&myVehicle, MSMoveReminder::NOTIFICATION_LANE_CHANGE);
        myHaveShadow = false;
    }
}
