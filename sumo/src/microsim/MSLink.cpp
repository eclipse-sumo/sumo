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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLink.h"
#include <cassert>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSLink::MSLink(MSLane* succLane, MSLane *via, bool yield,
               LinkDirection dir, LinkState state, bool internalEnd ) // !!! subclass
    : myLane(succLane), myJunctionInlane(via),
    myPrio(!yield), myApproaching(0),
    myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0),
	myState(state), myAmYellow(false), myDirection(dir),
    myIsInternalEnd(internalEnd)
{
//    assert(internalEnd==false);
}


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
    return myRespond->test(myRespondIdx);
}


void
MSLink::deleteRequest()
{
    myRequest->reset(myRequestIdx);
    myRespond->reset(myRespondIdx);
}


MSLink::LinkState
MSLink::getState() const
{
    if(myState!=LINKSTATE_ABSTRACT_TL) {
        return myState;
    }
    if(myAmYellow) {
        return LINKSTATE_TL_YELLOW;
    }
    if(opened()) {
        return LINKSTATE_TL_GREEN;
    } else {
        return LINKSTATE_TL_RED;
    }
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSLink.icc"
//#endif

// Local Variables:
// mode:C++
// End:


