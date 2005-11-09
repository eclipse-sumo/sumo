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
// Revision 1.5  2005/11/09 06:39:38  dkrajzew
// usage of internal lanes is now optional at building
//
// Revision 1.4  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
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
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLinkCont.h"
#include "MSLane.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
#ifdef HAVE_INTERNAL_LANES
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
#endif


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
