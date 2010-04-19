/****************************************************************************/
/// @file    ROEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
bool ROEdge::myHaveTTWarned = false;
bool ROEdge::myHaveEWarned = false;
std::vector<ROEdge*> ROEdge::myEdges;


// ===========================================================================
// method definitions
// ===========================================================================
ROEdge::ROEdge(const std::string &id, RONode *from, RONode *to, unsigned int index,
               bool useBoundariesOnOverride, bool interpolate) throw()
        : myID(id), mySpeed(-1),
        myIndex(index), myLength(-1),
        myUsingTTTimeLine(false), myUseBoundariesOnOverrideTT(useBoundariesOnOverride),
        myUsingETimeLine(false), myUseBoundariesOnOverrideE(useBoundariesOnOverride),
        myFromNode(from), myToNode(to), myInterpolate(interpolate) {
    while (myEdges.size()<=index) {
        myEdges.push_back(0);
    }
    myEdges[index] = this;
}


ROEdge::~ROEdge() throw() {
    for (std::vector<ROLane*>::iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        delete(*i);
    }
}


void
ROEdge::addLane(ROLane *lane) throw() {
    SUMOReal length = lane->getLength();
    assert(myLength==-1||length==myLength);
    myLength = length;
    SUMOReal speed = lane->getSpeed();
    mySpeed = speed > mySpeed ? speed : mySpeed;
    myLanes.push_back(lane);

    std::vector<SUMOVehicleClass>::const_iterator i;
    const std::vector<SUMOVehicleClass> &allowed = lane->getAllowedClasses();
    // for allowed classes
    for (i=allowed.begin(); i!=allowed.end(); ++i) {
        SUMOVehicleClass allowedC = *i;
        std::vector<SUMOVehicleClass>::iterator t;
        // add to allowed if not already in there
        t = find(myAllowedClasses.begin(), myAllowedClasses.end(), allowedC);
        if (t==myAllowedClasses.end()) {
            myAllowedClasses.push_back(allowedC);
        }
        // remove from disallowed if allowed on the lane
        t = find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), allowedC);
        if (t!=myNotAllowedClasses.end()) {
            myNotAllowedClasses.erase(t);
        }
    }
    // for disallowed classes
    const std::vector<SUMOVehicleClass> &disallowed = lane->getNotAllowedClasses();
    for (i=disallowed.begin(); i!=disallowed.end(); ++i) {
        SUMOVehicleClass disallowedC = *i;
        std::vector<SUMOVehicleClass>::iterator t;
        // add to disallowed if not already in there
        //  and not within allowed
        t = find(myAllowedClasses.begin(), myAllowedClasses.end(), disallowedC);
        if (t==myAllowedClasses.end()) {
            t = find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), disallowedC);
            if (t==myNotAllowedClasses.end()) {
                myNotAllowedClasses.push_back(disallowedC);
            }
        }
    }
}


void
ROEdge::addFollower(ROEdge *s) throw() {
    if (find(myFollowingEdges.begin(), myFollowingEdges.end(), s)==myFollowingEdges.end()) {
        myFollowingEdges.push_back(s);
    }
}


void
ROEdge::addEffort(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd) throw() {
    myEfforts.add(timeBegin, timeEnd, value);
    myUsingETimeLine = true;
}


void
ROEdge::addTravelTime(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd) throw() {
    myTravelTimes.add(timeBegin, timeEnd, value);
    myUsingTTTimeLine = true;
}


SUMOReal
ROEdge::getEffort(const ROVehicle *const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        return (SUMOReal)(myLength / mySpeed);
    }
    return ret;
}


SUMOReal
ROEdge::getTravelTime(const ROVehicle *const, SUMOTime time) const throw() {
    if (myUsingTTTimeLine) {
        if (!myHaveTTWarned && !myTravelTimes.describesTime(time)) {
            WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
            myHaveTTWarned = true;
        }
        if (myInterpolate) {
            SUMOReal inTT = myTravelTimes.getValue(time);
            SUMOReal split = (SUMOReal)(myTravelTimes.getSplitTime(time, time + (SUMOTime)inTT) - time);
            if (split >= 0) {
                return myTravelTimes.getValue(time + (SUMOTime)inTT) *((SUMOReal)1. - split / inTT) + split;
            }
        }
        return myTravelTimes.getValue(time);
    }
    // ok, no absolute value was found, use the normal value (without) as default
    return (SUMOReal)(myLength / mySpeed);
}


SUMOReal
ROEdge::getCOEffort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHBEFA::computeCO(c, v, 0) * getTravelTime(veh, time);
    }
    return ret;
}


SUMOReal
ROEdge::getCO2Effort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHBEFA::computeCO2(c, v, 0) * getTravelTime(veh, time);
    }
    return ret;
}


SUMOReal
ROEdge::getPMxEffort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHBEFA::computePMx(c, v, 0) * getTravelTime(veh, time);
    }
    return ret;
}


SUMOReal
ROEdge::getHCEffort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHBEFA::computeHC(c, v, 0) * getTravelTime(veh, time);
    }
    return ret;
}


SUMOReal
ROEdge::getNOxEffort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHBEFA::computeNOx(c, v, 0) * getTravelTime(veh, time);
    }
    return ret;
}


SUMOReal
ROEdge::getFuelEffort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHBEFA::computeFuel(c, v, 0) * getTravelTime(veh, time);
    }
    return ret;
}


SUMOReal
ROEdge::getNoiseEffort(const ROVehicle * const veh, SUMOTime time) const throw() {
    SUMOReal ret = 0;
    if (!getStoredEffort(time, ret)) {
        SUMOReal v = mySpeed;
        SUMOEmissionClass c = SVE_UNKNOWN;
        if (veh->getType()!=0) {
            v = MIN2(veh->getType()->maxSpeed, mySpeed);
            c = veh->getType()->emissionClass;
        }
        ret = HelpersHarmonoise::computeNoise(veh->getType()->emissionClass, v, 0);
    }
    return ret;
}


bool
ROEdge::getStoredEffort(SUMOTime time, SUMOReal &ret) const throw() {
    if (myUsingETimeLine) {
        if (!myEfforts.describesTime(time)) {
            if (!myHaveEWarned) {
                WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
                myHaveEWarned = true;
            }
            return false;
        }
        if (myInterpolate) {
            SUMOReal inTT = myTravelTimes.getValue(time);
            SUMOReal ratio = (SUMOReal)(myEfforts.getSplitTime(time, time + (SUMOTime)inTT) - time) / inTT;
            if (ratio >= 0) {
                ret = ratio * myEfforts.getValue(time) + (1-ratio)*myEfforts.getValue(time + (SUMOTime)inTT);
				return true;
            }
        }
        ret = myEfforts.getValue(time);
        return true;
    }
    return false;
}


unsigned int
ROEdge::getNoFollowing() const throw() {
    if (getType()==ET_SINK) {
        return 0;
    }
    return (unsigned int) myFollowingEdges.size();
}


void
ROEdge::setType(ROEdge::EdgeType type) throw() {
    myType = type;
}


bool
ROEdge::prohibits(const ROVehicle * const vehicle) const throw() {
    if (myAllowedClasses.size()==0&&myNotAllowedClasses.size()==0) {
        return false;
    }
    // ok, vehicles with an unknown class may be only prohibited
    //  if the edge is limited to a set of classes
    SUMOVehicleClass vclass = vehicle->getType()!=0 ? vehicle->getType()->vehicleClass : DEFAULT_VEH_CLASS;
    if (vclass==SVC_UNKNOWN) {
        return false;
    }
    // check whether it is explicitely disallowed
    if (find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), vclass)!=myNotAllowedClasses.end()) {
        return true;
    }
    // check whether it is within the allowed classes
    if (myAllowedClasses.size()==0||find(myAllowedClasses.begin(), myAllowedClasses.end(), vclass)!=myAllowedClasses.end()) {
        return false;
    }
    // ok, we have a set of allowed vehicle classes, but this vehicle's class
    //  is not among them
    return true;
}


void
ROEdge::buildTimeLines(const std::string &measure) throw() {
    if (myUsingETimeLine) {
        SUMOReal value = (SUMOReal)(myLength / mySpeed);
        if (measure=="CO") {
            value = HelpersHBEFA::computeCO(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure=="CO2") {
            value = HelpersHBEFA::computeCO2(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure=="HC") {
            value = HelpersHBEFA::computeHC(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure=="PMx") {
            value = HelpersHBEFA::computePMx(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure=="NOx") {
            value = HelpersHBEFA::computeNOx(SVE_UNKNOWN, mySpeed, 0) * value;
        }
        if (measure=="fuel") {
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
ROEdge::allFollowersProhibit(const ROVehicle * const vehicle) const throw() {
    for (std::vector<ROEdge*>::const_iterator i=myFollowingEdges.begin(); i!=myFollowingEdges.end(); ++i) {
        if (!(*i)->prohibits(vehicle)) {
            return false;
        }
    }
    return true;
}


ROEdge*
ROEdge::dictionary(size_t id) throw() {
    assert(myEdges.size()>id);
    return myEdges[id];
}



/****************************************************************************/

