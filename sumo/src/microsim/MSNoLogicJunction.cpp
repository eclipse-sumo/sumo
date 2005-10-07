/***************************************************************************
                          MSNoLogicJunction.cpp
                             -------------------
    begin                : Thu, 06 Jun 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.10  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/05/04 08:32:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/08/02 12:09:39  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.5  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.4  2003/04/14 08:33:01  dkrajzew
// some further bugs removed
//
// Revision 1.3  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:29  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include; junction extended by position information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/08/06 15:50:16  roessel
// SetFirstVehiclesRequests implemented.
//
// Revision 1.2  2002/06/18 10:59:53  croessel
// Removed some ^M.
//
// Revision 1.1  2002/06/07 14:45:17  dkrajzew
// Added MSNoLogicJunction ,,pro forma,,. The code may compile but the
// class has no real functionality...
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

#include "MSNoLogicJunction.h"
#include "MSLane.h"
#include "MSInternalLane.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
std::bitset<64> MSNoLogicJunction::myDump((unsigned long) 0xffffffff);



/* =========================================================================
 * method definitions
 * ======================================================================= */
MSNoLogicJunction::MSNoLogicJunction(string id, const Position2D &position,
                                     LaneCont incoming,
                                     LaneCont internal)
    : MSJunction( id, position ),
    myIncomingLanes( incoming ), myInternalLanes( internal )
{
}


bool
MSNoLogicJunction::clearRequests()
{
    return true;
}

//-------------------------------------------------------------------------//

MSNoLogicJunction::~MSNoLogicJunction()
{
}

//-------------------------------------------------------------------------//

void
MSNoLogicJunction::postloadInit()
{
    LaneCont::iterator i;
    // inform links where they have to report approaching vehicles to
    for(i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); i++) {
        const MSLinkCont &links = (*i)->getLinkCont();
        for(MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myDump, 0,
                &myDump, 0);
        }
    }
    // set information for the internal lanes
    for(i=myInternalLanes.begin(); i!=myInternalLanes.end(); i++) {
        // ... set information about participation
        static_cast<MSInternalLane*>(*i)->setParentJunctionInformation(
            &myDump, 0);
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
