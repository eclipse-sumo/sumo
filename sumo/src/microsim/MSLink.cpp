//---------------------------------------------------------------------------//
//                        MSLink.cpp -
//  The link between two lanes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.16  2006/01/11 11:54:35  dkrajzew
// reworked possible link states; new link coloring
//
// Revision 1.15  2005/11/09 06:39:38  dkrajzew
// usage of internal lanes is now optional at building
//
// Revision 1.14  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.13  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.12  2004/11/25 11:53:48  dkrajzew
// patched the bug on false intervals stamps if begin!=0
//
// Revision 1.11  2004/08/02 12:14:54  dkrajzew
// added some security checks for buggy nets (aehhh)
//
// Revision 1.10  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.9  2003/11/12 13:50:30  dkrajzew
// MSLink-members are now secured from the outer world
//
// Revision 1.8  2003/09/05 15:11:43  dkrajzew
// first steps for reading of internal lanes
//
// Revision 1.7  2003/08/04 11:42:35  dkrajzew
// missing deletion of traffic light logics on closing a network added
//
// Revision 1.6  2003/07/30 09:09:55  dkrajzew
// added end-of-link definition (direction, type) for visualisation
//
// Revision 1.5  2003/06/05 16:05:40  dkrajzew
// removal of links request added; needed by new traffic lights
//
// Revision 1.4  2003/05/21 15:15:41  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.3  2003/04/14 08:33:00  dkrajzew
// some further bugs removed
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLink.h"
#include <iostream>
#include <cassert>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
#ifndef HAVE_INTERNAL_LANES
MSLink::MSLink(MSLane* succLane, bool yield,
               LinkDirection dir, LinkState state, bool internalEnd ) // !!! subclass
    : myLane(succLane),
    myPrio(!yield), myApproaching(0),
    myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
    myState(state), myAmYellow(false), myDirection(dir),
    myIsInternalEnd(internalEnd)
{
}
#else
MSLink::MSLink(MSLane* succLane, MSLane *via, bool yield,
               LinkDirection dir, LinkState state, bool internalEnd ) // !!! subclass
    : myLane(succLane), myJunctionInlane(via),
    myPrio(!yield), myApproaching(0),
    myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
    myState(state), myAmYellow(false), myDirection(dir),
    myIsInternalEnd(internalEnd)
{
}
#endif


MSLink::~MSLink()
{
}


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
    if(myRequest==0) {
        return;
    }
    myApproaching = approaching;
    myRequest->set(myRequestIdx);
}


/////////////////////////////////////////////////////////////////////////////

void
MSLink::setPriority( bool prio, bool yellow )
{
    myPrio = prio;
    myAmYellow = yellow;
}


bool
MSLink::opened() const
{
    if(myIsInternalEnd) {
        return true;
    }
    if(myRespond==0) {
        std::cout << "Buggy" << std::endl;
        return false; // !!! should never happen, was sometimes the case in possibly buggy networks
    }
    return myRespond->test(myRespondIdx);
}


void
MSLink::deleteRequest()
{
    if(myRequest==0) {
        std::cout << "Buggy" << std::endl;
        return ; // !!! should never happen, was sometimes the case in possibly buggy networks
    }
    myRequest->reset(myRequestIdx);
    if(myRespond==0) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


