/****************************************************************************/
/// @file    ROEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "ROVehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
bool ROEdge::myHaveTTWarned = false;
bool ROEdge::myHaveEWarned = false;


// ===========================================================================
// method definitions
// ===========================================================================
ROEdge::ROEdge(const std::string &id, RONode *from, RONode *to, unsigned int index, bool useBoundariesOnOverride) throw()
        : myID(id), mySpeed(-1),
        myIndex(index), myLength(-1),
        myUsingTTTimeLine(false), myUseBoundariesOnOverrideTT(useBoundariesOnOverride),
        myUsingETimeLine(false), myUseBoundariesOnOverrideE(useBoundariesOnOverride),
        myFromNode(from), myToNode(to)
{}


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
ROEdge::getEffort(const ROVehicle *const, SUMOTime time) const throw() {
    // ok, no absolute value was found, use the normal value (without)
    //  weight as default
    SUMOReal value = (SUMOReal)(myLength / mySpeed);
    if (myUsingETimeLine) {
        if (!myHaveEWarned && !myEfforts.describesTime(time)) {
            WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
            myHaveEWarned = true;
        }
        return myEfforts.getValue(time);
    }
    return value;
}


SUMOReal
ROEdge::getTravelTime(const ROVehicle *const, SUMOTime time) const throw() {
    // ok, no absolute value was found, use the normal value (without)
    //  weight as default
    SUMOReal value = (SUMOReal)(myLength / mySpeed);
    if (myUsingTTTimeLine) {
        if (!myHaveTTWarned && !myTravelTimes.describesTime(time)) {
            WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
            myHaveTTWarned = true;
        }
        return myTravelTimes.getValue(time);
    }
    return value;
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
    SUMOVehicleClass vclass = vehicle->getType()!=0 ? vehicle->getType()->getClass() : DEFAULT_VEH_CLASS;
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
ROEdge::buildTimeLines() throw()
{
    if (myUsingETimeLine) {
        SUMOReal value = (SUMOReal)(myLength / mySpeed);
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



/****************************************************************************/

