//---------------------------------------------------------------------------//
//                        MSLinkCont.cpp -
//  Helpers for link vector
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 15 Feb 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2005/05/04 08:28:15  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/02/16 14:24:48  dkrajzew
// some helper methods added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSLinkCont.h"
#include "MSLane.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
const MSEdge *
MSLinkContHelper::getInternalFollowingEdge(MSLane *fromLane,
                                           MSEdge *followerAfterInternal)
{
    // !!! too slow
    const MSLinkCont &lc = fromLane->getLinkCont();
    for(MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); j++) {
        MSLink *link = *j;
        if(&link->getLane()->edge()==followerAfterInternal) {
            return &(link->getViaLane()->edge());
        }
    }
    return 0;
}


MSLink *
MSLinkContHelper::getConnectingLink(const MSLane &from, const MSLane &to)
{
    const MSLinkCont &lc = from.getLinkCont();
    for(MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); j++) {
        MSLink *link = *j;
        if(link->getLane()==&to) {
            return link;
        }
    }
    return 0;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
