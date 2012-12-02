/****************************************************************************/
/// @file    MSLink.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// A connnection between lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
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

#include <iostream>
#include "MSLink.h"
#include "MSLane.h"
#include "MSGlobals.h"
#include "MSVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
SUMOTime MSLink::myLookaheadTime = TIME2STEPS(1);


// ===========================================================================
// member method definitions
// ===========================================================================
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* succLane,
               LinkDirection dir, LinkState state,
               SUMOReal length)
    :
    myLane(succLane),
    myRequestIdx(0), myRespondIdx(0),
    myState(state), myDirection(dir),  myLength(length) {}
#else
MSLink::MSLink(MSLane* succLane, MSLane* via,
               LinkDirection dir, LinkState state, SUMOReal length)
    :
    myLane(succLane),
    myRequestIdx(0), myRespondIdx(0),
    myState(state), myDirection(dir), myLength(length),
    myJunctionInlane(via) {}
#endif


MSLink::~MSLink() {}


void
MSLink::setRequestInformation(unsigned int requestIdx, unsigned int respondIdx, bool isCrossing, bool isCont,
                              const std::vector<MSLink*>& foeLinks,
                              const std::vector<MSLane*>& foeLanes) {
    myRequestIdx = requestIdx;
    myRespondIdx = respondIdx;
    myIsCrossing = isCrossing;
    myAmCont = isCont;
    myFoeLinks = foeLinks;
    myFoeLanes = foeLanes;
}


void
MSLink::setApproaching(SUMOVehicle* approaching, SUMOTime arrivalTime, SUMOReal speed, bool setRequest) {
    LinkApproachingVehicles::iterator i = find_if(myApproachingVehicles.begin(), myApproachingVehicles.end(), vehicle_in_request_finder(approaching));
    if (i != myApproachingVehicles.end()) {
        myApproachingVehicles.erase(i);
    }
    const SUMOTime leaveTime = arrivalTime + TIME2STEPS((approaching->getVehicleType().getLengthWithGap() + getLength()) / speed);
    ApproachingVehicleInformation approachInfo(arrivalTime, leaveTime, approaching, setRequest);
    myApproachingVehicles.push_back(approachInfo);
}


void
MSLink::addBlockedLink(MSLink* link) {
    myBlockedFoeLinks.insert(link);
}



bool
MSLink::willHaveBlockedFoe() const {
    for (std::set<MSLink*>::const_iterator i = myBlockedFoeLinks.begin(); i != myBlockedFoeLinks.end(); ++i) {
        if ((*i)->isBlockingAnyone()) {
            return true;
        }
    }
    return false;
}


void
MSLink::removeApproaching(SUMOVehicle* veh) {
    LinkApproachingVehicles::iterator i = find_if(myApproachingVehicles.begin(), myApproachingVehicles.end(), vehicle_in_request_finder(veh));
    if (i != myApproachingVehicles.end()) {
        myApproachingVehicles.erase(i);
    }
}


bool
MSLink::opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength) const {
    if (myState == LINKSTATE_TL_RED) {
        return false;
    }
    if (myAmCont) {
        return true;
    }
    const SUMOTime leaveTime = arrivalTime + TIME2STEPS((getLength() + vehicleLength) / (0.5 * (arrivalSpeed + leaveSpeed)));
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            if ((*i)->getState() == LINKSTATE_TL_RED) {
                continue;
            }
        }
#endif
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, leaveSpeed)) {
            return false;
        }
    }
    for (std::vector<MSLane*>::const_iterator i = myFoeLanes.begin(); i != myFoeLanes.end(); ++i) {
        if ((*i)->getVehicleNumber() > 0 || (*i)->getPartialOccupator() != 0) {
            return false;
        }
    }
    return true;
}


bool
MSLink::blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal speed) const {
    for (LinkApproachingVehicles::const_iterator i = myApproachingVehicles.begin(); i != myApproachingVehicles.end(); ++i) {
        if (!(*i).willPass) {
            continue;
        }
        if ((*i).leavingTime < arrivalTime) {
            // ego wants to be follower
            if ((*i).leavingTime + safeHeadwayTime(i->vehicle->getSpeed(), speed) >= arrivalTime) {
                return true;
            }
        } else if ((*i).arrivalTime > leaveTime) {
            // ego wants to be leader
            if ((*i).arrivalTime - safeHeadwayTime(speed, i->vehicle->getSpeed()) <= leaveTime) {
                return true;
            }
        } else {
            // even without considering safeHeadwayTime there is already a conflict
            return true;
        }
    }
    return false;
}


SUMOTime
MSLink::safeHeadwayTime(SUMOReal leaderSpeed, SUMOReal followerSpeed) {
    // v: leader speed
    // u: follower speed
    // a: leader decel
    // b: follower decel
    // g: follower min gap
    // h: save headway time (result)
    const SUMOReal v = leaderSpeed;
    const SUMOReal u = followerSpeed;
    // XXX use cfmodel values of possible
    const SUMOReal a = DEFAULT_VEH_DECEL;
    const SUMOReal b = DEFAULT_VEH_DECEL;
    const SUMOReal g = DEFAULT_VEH_MINGAP;
    // breaking distance ~ (v^2 - a*v)/(2*a)
    if (v < a) {
        // leader may break in one timestep (need different formula)
        // u*h > g + (u^2 - b*u)/(2*b) + 0.5
        return TIME2STEPS((g + 0.5) / u + (u / b - 1.0) * 0.5);
    } else {
        // u*h + (v^2 - a*v)/(2*a) > g + (u^2 - b*u)/(2*b) + 0.5
        return TIME2STEPS((g + (1.0 + v - v * v / a) * 0.5) / u + (u / b - 1.0) * 0.5);
    }
}


bool
MSLink::hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal speed) const {
    for (std::vector<MSLink*>::const_iterator i = myFoeLinks.begin(); i != myFoeLinks.end(); ++i) {
        if ((*i)->blockedAtTime(arrivalTime, leaveTime, speed)) {
            return true;
        }
    }
    for (std::vector<MSLane*>::const_iterator i = myFoeLanes.begin(); i != myFoeLanes.end(); ++i) {
        if ((*i)->getVehicleNumber() > 0 || (*i)->getPartialOccupator() != 0) {
            return true;
        }
    }
    return false;
}


LinkDirection
MSLink::getDirection() const {
    return myDirection;
}


void
MSLink::setTLState(LinkState state, SUMOTime /*t*/) {
    myState = state;
}


MSLane*
MSLink::getLane() const {
    return myLane;
}


#ifdef HAVE_INTERNAL_LANES
MSLane*
MSLink::getViaLane() const {
    return myJunctionInlane;
}
#endif


MSLane*
MSLink::getViaLaneOrLane() const {
#ifdef HAVE_INTERNAL_LANES
    if (myJunctionInlane != 0) {
        return myJunctionInlane;
    } else {
        return myLane;
    }
#else
    return myLane;
#endif
}


unsigned int
MSLink::getRespondIndex() const {
    return myRespondIdx;
}



/****************************************************************************/

