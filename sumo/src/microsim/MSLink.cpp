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


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSLink::MSLink(MSLane* succLane, bool yield)
    : myLane(succLane),
    myPrio(!yield), myApproaching(0),
    myRequest(0), myRequestIdx(0), myRespond(0), myRespondIdx(0)
{
}


void
MSLink::setRequestInformation(MSLogicJunction::Request *request,
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
MSLink::setPriority( bool prio )
{
    myPrio = prio;
}


bool
MSLink::opened() const
{
    return myRespond->test(myRespondIdx);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSLink.icc"
//#endif

// Local Variables:
// mode:C++
// End:


