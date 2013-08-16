/****************************************************************************/
/// @file    ROEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "ROLane.h"
#include "ROEdge.h"
#include "ROVehicle.h"
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/common/HelpersHBEFA.h>
#include <utils/common/HelpersHarmonoise.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
bool ROEdge::myUseBoundariesOnOverrideTT = false;
bool ROEdge::myUseBoundariesOnOverrideE = false;
bool ROEdge::myInterpolate = false;
bool ROEdge::myHaveTTWarned = false;
bool ROEdge::myHaveEWarned = false;
std::vector<ROEdge*> ROEdge::myEdges;


// ===========================================================================
// method definitions
// ===========================================================================
ROEdge::ROEdge(const std::string& id, RONode* from, RONode* to, unsigned int index, const int priority)
    : Named(id), myFromNode(from), myToNode(to), myIndex(index), myPriority(priority),
      mySpeed(-1), myLength(-1),
      myUsingTTTimeLine(false),
      myUsingETimeLine(false),
      myCombinedPermissions(0) {
    while (myEdges.size() <= index) {
        myEdges.push_back(0);
    }
    myEdges[index] = this;
}


ROEdge::~ROEdge() {
    for (std::vector<ROLane*>::iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        delete(*i);
    }
}


void
ROEdge::addLane(ROLane* lane) {
    SUMOReal length = lane->getLength();
    assert(myLength == -1 || length == myLength);
    myLength = length;
    SUMOReal speed = lane->getSpeed();
    mySpeed = speed > mySpeed ? speed : mySpeed;
    myLanes.push_back(lane);

    // integrate new allowed classes
    myCombinedPermissions |= lane->getPermissions();
}


void
ROEdge::addFollower(ROEdge* s, std::string) {
    if (find(myFollowingEdges.begin(), myFollowingEdges.end(), s) == myFollowingEdges.end()) {
        myFollowingEdges.push_back(s);
#ifdef HAVE_INTERNAL // catchall for internal stuff
        s->myApproachingEdges.push_back(this);
#endif
    }
}


void
ROEdge::addEffort(SUMOReal value, SUMOReal timeBegin, SUMOReal timeEnd) {
    myEfforts.add(timeBegin, timeEnd, value);
    myUsingETimeLine = true;
}


void
ROEdge::addTravelTime(SUMOReal value, SUMOReal timeBegin, SUMOReal timeEnd) {
    myTravelTimes.add(timeBegin, timeEnd, value);
    myUsingTTTimeLine = true;
}


SUMOReal
ROEdge::getEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        return (SUMOReal)(myLength / MIN2(veh->getType()->maxSpeed, mySpeed));
    }
    return ret;
}


SUMOReal
ROEdge::getDistanceTo(const ROEdge* other) const {
    if (getToNode() != 0 && other->getFromNode() != 0) {
        return getToNode()->getPosition().distanceTo2D(other->getFromNode()->getPosition());
    } else {
        return 0; // optimism is just right for astar
    }

}


SUMOReal
ROEdge::getTravelTime(const ROVehicle* const veh, SUMOReal time) const {
    return getTravelTime(veh->getType()->maxSpeed, time);
}


SUMOReal
ROEdge::getTravelTime(const SUMOReal maxSpeed, SUMOReal time) const {
    return MAX2(myLength / maxSpeed, getTravelTime(time));
}


SUMOReal
ROEdge::getTravelTime(SUMOReal time) const {
    if (myUsingTTTimeLine) {
        if (!myHaveTTWarned && !myTravelTimes.describesTime(time)) {
            WRITE_WARNING("No interval matches passed time " + toString(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
            myHaveTTWarned = true;
        }
        if (myInterpolate) {
            SUMOReal inTT = myTravelTimes.getValue(time);
            SUMOReal split = (SUMOReal)(myTravelTimes.getSplitTime(time, time + (SUMOTime)inTT) - time);
            if (split >= 0) {
                return myTravelTimes.getValue(time + (SUMOTime)inTT) * ((SUMOReal)1. - split / inTT) + split;
            }
        }
        return myTravelTimes.getValue(time);
    }
    return myLength / mySpeed;
}


SUMOReal
ROEdge::getMinimumTravelTime(const ROVehicle* const veh) const {
    return (SUMOReal)(myLength / MIN2(veh->getType()->maxSpeed, mySpeed));
}


SUMOReal
ROEdge::getCOEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal vMax = MIN2(veh->getType()->maxSpeed, mySpeed);
        const SUMOReal accel = veh->getType()->get(SUMO_ATTR_ACCEL, DEFAULT_VEH_ACCEL) * veh->getType()->get(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA) / 2.;
        ret = HelpersHBEFA::computeDefaultCO(veh->getType()->emissionClass, vMax, accel, getTravelTime(veh, time));
    }
    return ret;
}


SUMOReal
ROEdge::getCO2Effort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal vMax = MIN2(veh->getType()->maxSpeed, mySpeed);
        const SUMOReal accel = veh->getType()->get(SUMO_ATTR_ACCEL, DEFAULT_VEH_ACCEL) * veh->getType()->get(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA) / 2.;
        ret = HelpersHBEFA::computeDefaultCO2(veh->getType()->emissionClass, vMax, accel, getTravelTime(veh, time));
    }
    return ret;
}


SUMOReal
ROEdge::getPMxEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal vMax = MIN2(veh->getType()->maxSpeed, mySpeed);
        const SUMOReal accel = veh->getType()->get(SUMO_ATTR_ACCEL, DEFAULT_VEH_ACCEL) * veh->getType()->get(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA) / 2.;
        ret = HelpersHBEFA::computeDefaultPMx(veh->getType()->emissionClass, vMax, accel, getTravelTime(veh, time));
    }
    return ret;
}


SUMOReal
ROEdge::getHCEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal vMax = MIN2(veh->getType()->maxSpeed, mySpeed);
        const SUMOReal accel = veh->getType()->get(SUMO_ATTR_ACCEL, DEFAULT_VEH_ACCEL) * veh->getType()->get(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA) / 2.;
        ret = HelpersHBEFA::computeDefaultHC(veh->getType()->emissionClass, vMax, accel, getTravelTime(veh, time));
    }
    return ret;
}


SUMOReal
ROEdge::getNOxEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal vMax = MIN2(veh->getType()->maxSpeed, mySpeed);
        const SUMOReal accel = veh->getType()->get(SUMO_ATTR_ACCEL, DEFAULT_VEH_ACCEL) * veh->getType()->get(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA) / 2.;
        ret = HelpersHBEFA::computeDefaultNOx(veh->getType()->emissionClass, vMax, accel, getTravelTime(veh, time));
    }
    return ret;
}


SUMOReal
ROEdge::getFuelEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal vMax = MIN2(veh->getType()->maxSpeed, mySpeed);
        const SUMOReal accel = veh->getType()->get(SUMO_ATTR_ACCEL, DEFAULT_VEH_ACCEL) * veh->getType()->get(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA) / 2.;
        ret = HelpersHBEFA::computeDefaultFuel(veh->getType()->emissionClass, vMax, accel, getTravelTime(veh, time));
    }
    return ret;
}


SUMOReal
ROEdge::getNoiseEffort(const ROVehicle* const veh, SUMOReal time) const {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        const SUMOReal v = MIN2(veh->getType()->maxSpeed, mySpeed);
        ret = HelpersHarmonoise::computeNoise(veh->getType()->emissionClass, v, 0);
    }
    return ret;
}


bool
ROEdge::getStoredEffort(SUMOReal time, SUMOReal& ret) const {
    if (myUsingETimeLine) {
        if (!myEfforts.describesTime(time)) {
            if (!myHaveEWarned) {
                WRITE_WARNING("No interval matches passed time " + toString(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
                myHaveEWarned = true;
            }
            return false;
        }
        if (myInterpolate) {
            SUMOReal inTT = myTravelTimes.getValue(time);
            SUMOReal ratio = (SUMOReal)(myEfforts.getSplitTime(time, time + (SUMOTime)inTT) - time) / inTT;
            if (ratio >= 0) {
                ret = ratio * myEfforts.getValue(time) + (1 - ratio) * myEfforts.getValue(time + (SUMOTime)inTT);
                return true;
            }
        }
        ret = myEfforts.getValue(time);
        return true;
    }
    return false;
}


unsigned int
ROEdge::getNoFollowing() const {
    if (getType() == ET_SINK) {
        return 0;
    }
    return (unsigned int) myFollowingEdges.size();
}


#ifdef HAVE_INTERNAL // catchall for internal stuff
unsigned int
ROEdge::getNumApproaching() const {
    if (getType() == ET_SOURCE) {
        return 0;
    }
    return (unsigned int) myApproachingEdges.size();
}
#endif


void
ROEdge::setType(ROEdge::EdgeType type) {
    myType = type;
}


void
ROEdge::buildTimeLines(const std::string& measure) {
    if (myUsingETimeLine) {
        SUMOReal value = (SUMOReal)(myLength / mySpeed);
        if (measure == "CO") {
            value = HelpersHBEFA::computeCO(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure == "CO2") {
            value = HelpersHBEFA::computeCO2(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure == "HC") {
            value = HelpersHBEFA::computeHC(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure == "PMx") {
            value = HelpersHBEFA::computePMx(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure == "NOx") {
            value = HelpersHBEFA::computeNOx(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure == "fuel") {
            value = HelpersHBEFA::computeFuel(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        myEfforts.fillGaps(value, myUseBoundariesOnOverrideE);
    }
    if (myUsingTTTimeLine) {
        SUMOReal value = (SUMOReal)(myLength / mySpeed);
        myTravelTimes.fillGaps(value, myUseBoundariesOnOverrideTT);
    }
}


bool
ROEdge::allFollowersProhibit(const ROVehicle* const vehicle) const {
    for (std::vector<ROEdge*>::const_iterator i = myFollowingEdges.begin(); i != myFollowingEdges.end(); ++i) {
        if (!(*i)->prohibits(vehicle)) {
            return false;
        }
    }
    return true;
}


ROEdge*
ROEdge::dictionary(size_t id) {
    assert(myEdges.size() > id);
    return myEdges[id];
}



/****************************************************************************/

