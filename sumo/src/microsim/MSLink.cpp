/****************************************************************************/
/// @file    MSLink.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The link between two lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLink.h"
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
               LinkDirection dir, LinkState state) // !!! subclass
        :
        myLane(succLane),
        myPrio(!yield), myApproaching(0),
        myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
        myState(state), myAmYellow(false), myDirection(dir)
{}
#else
MSLink::MSLink(MSLane* succLane, MSLane *via, bool yield,
               LinkDirection dir, LinkState state, bool internalEnd)  // !!! subclass
        :
        myLane(succLane), myJunctionInlane(via),
        myPrio(!yield), myApproaching(0),
        myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
        myState(state), myAmYellow(false), myDirection(dir),
        myIsInternalEnd(internalEnd)
{}
#endif


MSLink::~MSLink()
{}


void
MSLink::setRequestInformation(
    MSLogicJunction::Request *request,
    size_t requestIdx, MSLogicJunction::Respond *respond,
    size_t respondIdx)
{
    myRequest = request;
    myRequestIdx = requestIdx;
    myRespond = respond;
    myRespondIdx = respondIdx;
}


void
MSLink::setApproaching(MSVehicle *approaching)
{
    if (myRequest==0) {
        return;
    }
    myApproaching = approaching;
    myRequest->set(myRequestIdx);
}


/////////////////////////////////////////////////////////////////////////////

void
MSLink::setPriority(bool prio, bool yellow)
{
    myPrio = prio;
    myAmYellow = yellow;
}


bool
MSLink::opened() const
{
    if (myRespond==0) {
        // this is the case for internal lanes ending at a junction's end
        // (let the vehicle always leave the junction)
        return true;
    }
    return myRespond->test(myRespondIdx);
}


void
MSLink::deleteRequest()
{
    if (myRequest==0) {
        std::cout << "Buggy" << std::endl;
        return ; // !!! should never happen, was sometimes the case in possibly buggy networks
    }
    myRequest->reset(myRequestIdx);
    if (myRespond==0) {
        std::cout << "Buggy" << std::endl;
        return ; // !!! should never happen, was sometimes the case in possibly buggy networks
    }
    myRespond->reset(myRespondIdx);
}


MSLink::LinkState
MSLink::getState() const
{
//    if(myState!=LINKSTATE_ABSTRACT_TL) {
    return myState;
    /*
    }
    if(myAmYellow) {
    return LINKSTATE_TL_YELLOW;
    }
    if(opened()) {
    return LINKSTATE_TL_GREEN;
    } else {
    return LINKSTATE_TL_RED;
    }
    */
}


MSLink::LinkDirection
MSLink::getDirection() const
{
    return myDirection;
}


void
MSLink::setTLState(LinkState state)
{
    myState = state;
}


bool
MSLink::amYellow() const
{
    return myAmYellow;
}


MSLane *
MSLink::getLane() const
{
    return myLane;
}


bool
MSLink::havePriority() const
{
    return myPrio;
}


#ifdef HAVE_INTERNAL_LANES
MSLane * const
MSLink::getViaLane() const
{
    return myJunctionInlane;
}
#endif


bool
MSLink::isApproached() const
{
    return myApproaching!=0;
}


#ifdef HAVE_INTERNAL_LANES
void
MSLink::resetInternalPriority()
{
    myPrio = opened();
}
#endif


size_t
MSLink::getRespondIndex() const
{
    return myRespondIdx;
}



/****************************************************************************/

