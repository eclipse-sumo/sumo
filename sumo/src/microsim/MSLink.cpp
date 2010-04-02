/****************************************************************************/
/// @file    MSLink.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A connnection between lanes
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

#include "MSLink.h"
#include "MSLane.h"
#include <iostream>
#include <cassert>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* succLane, bool yield,
               LinkDirection dir, LinkState state,
               SUMOReal length) throw()
        :
        myLane(succLane),
        myPrio(!yield), myApproaching(0),
        myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
        myState(state), myDirection(dir),  myLength(length) {}
#else
MSLink::MSLink(MSLane* succLane, MSLane *via, bool yield,
               LinkDirection dir, LinkState state, bool internalEnd,
               SUMOReal length) throw()
        :
        myLane(succLane),
        myPrio(!yield), myApproaching(0),
        myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
        myState(state), myDirection(dir), myLength(length),
        myJunctionInlane(via),myIsInternalEnd(internalEnd) {}
#endif


MSLink::~MSLink() throw() {}


void
MSLink::setRequestInformation(MSLogicJunction::Request *request, unsigned int requestIdx,
                              MSLogicJunction::Respond *respond, unsigned int respondIdx,
                              const MSLogicJunction::LinkFoes &foes, bool isCrossing, bool isCont,
							  const std::vector<MSLink*> &foeLinks,
							  const std::vector<MSLane*> &foeLanes) throw() {
    assert(myRequest==0);
    assert(myRespond==0);
    myRequest = request;
    myRequestIdx = requestIdx;
    myRespond = respond;
    myRespondIdx = respondIdx;
    myFoes = foes;
    myIsCrossing = isCrossing;
    myAmCont = isCont;
	myFoeLinks = foeLinks;
	myFoeLanes = foeLanes;
}


void
MSLink::setApproaching(MSVehicle *approaching, SUMOTime arrivalTime, SUMOReal speed) throw() {
    if (myRequest==0) {
        return;
    }
    myApproaching = approaching;
    myRequest->set(myRequestIdx);
    std::vector<MSJunction::ApproachingVehicleInformation>::iterator i = find_if(myApproachingVehicles.begin(), myApproachingVehicles.end(), MSJunction::vehicle_in_request_finder(approaching));
    if(i!=myApproachingVehicles.end()) {
        myApproachingVehicles.erase(i);
    }
    MSJunction::ApproachingVehicleInformation approachInfo(arrivalTime, arrivalTime + getLength() / speed, approaching);
    myApproachingVehicles.push_back(approachInfo);
}


void 
MSLink::removeApproaching(MSVehicle *veh)
{
    if (myRequest==0) {
        return;
    }
    std::vector<MSJunction::ApproachingVehicleInformation>::iterator i = find_if(myApproachingVehicles.begin(), myApproachingVehicles.end(), MSJunction::vehicle_in_request_finder(veh));
    if(i!=myApproachingVehicles.end()) {
        myApproachingVehicles.erase(i);
    }
}


void
MSLink::setPriority(bool prio) throw() {
    myPrio = prio;
}


bool
MSLink::opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed) const throw() {
    if (myRespond==0) {
        // this is the case for internal lanes ending at a junction's end
        // (let the vehicle always leave the junction)
        return true;
    }
    if(myAmCont) {
        return true;
    }
	if(myState==LINKSTATE_TL_RED) {
		return false;
	}
#ifdef HAVE_INTERNAL_LANES
    SUMOTime leaveTime = myJunctionInlane==0 ? arrivalTime + getLength() * arrivalSpeed : arrivalTime + this->myJunctionInlane->getLength() * arrivalSpeed;
#else
    SUMOTime leaveTime = arrivalTime + getLength() * arrivalSpeed;
#endif
    for(std::vector<MSLink*>::const_iterator i=myFoeLinks.begin(); i!=myFoeLinks.end(); ++i) {
        if((*i)->blockedAtTime(arrivalTime, leaveTime)) {
            return false;
        }
    }
    for(std::vector<MSLane*>::const_iterator i=myFoeLanes.begin(); i!=myFoeLanes.end(); ++i) {
        if((*i)->getVehicleNumber()>0||(*i)->getPartialOccupator()!=0) {
            return false;
        }
    }
    return true;
}


bool 
MSLink::blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime) const throw()
{
    for(std::vector<MSJunction::ApproachingVehicleInformation>::const_iterator i=myApproachingVehicles.begin(); i!=myApproachingVehicles.end(); ++i) {
        if((*i).arrivalTime-1.5<=arrivalTime&&(*i).leavingTime+1.5>=arrivalTime) {
            return true;
        }
        if((*i).arrivalTime-1.5<=leaveTime&&(*i).leavingTime+1.5>=leaveTime) {
            return true;
        }
    }
    return false;
}


bool
MSLink::hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime) const throw() {
    if (myRequest==0) {
        return false;
    }
    for(std::vector<MSLink*>::const_iterator i=myFoeLinks.begin(); i!=myFoeLinks.end(); ++i) {
        if((*i)->blockedAtTime(arrivalTime, leaveTime)) {
            return true;
        }
    }
    for(std::vector<MSLane*>::const_iterator i=myFoeLanes.begin(); i!=myFoeLanes.end(); ++i) {
        if((*i)->getVehicleNumber()>0||(*i)->getPartialOccupator()!=0) {
            return true;
        }
    }
    return false;
//    return (*myRequest&myFoes).any();
}


void
MSLink::deleteRequest() throw() {
    if (myRequest!=0) {
        myRequest->reset(myRequestIdx);
    }
    if (myRespond!=0) {
        myRespond->reset(myRespondIdx);
    }
    myApproaching = 0;
}


MSLink::LinkDirection
MSLink::getDirection() const throw() {
    return myDirection;
}


void
MSLink::setTLState(LinkState state) throw() {
    myState = state;
}


MSLane *
MSLink::getLane() const throw() {
    return myLane;
}


#ifdef HAVE_INTERNAL_LANES
MSLane * const
MSLink::getViaLane() const throw() {
    return myJunctionInlane;
}
#endif


#ifdef HAVE_INTERNAL_LANES
void
MSLink::resetInternalPriority() throw() {
    myPrio = opened(MSNet::getInstance()->getCurrentTimeStep(), MSNet::getInstance()->getCurrentTimeStep());
    if (myJunctionInlane!=0&&myLane!=0) {
        if (myState==MSLink::LINKSTATE_TL_GREEN_MAJOR||myState==MSLink::LINKSTATE_TL_GREEN_MINOR) {
            if (myIsInternalEnd&&myJunctionInlane->getID()[0]==':') {
                if (myRequest->test(myRequestIdx)) {
                    myRespond->set(myRespondIdx, true);
                }
            }
        }
    }
}
#endif


unsigned int
MSLink::getRespondIndex() const throw() {
    return myRespondIdx;
}



/****************************************************************************/

