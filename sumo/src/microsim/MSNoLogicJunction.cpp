/****************************************************************************/
/// @file    MSNoLogicJunction.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// -------------------
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSNoLogicJunction.h"
#include "MSLane.h"
#include "MSInternalLane.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
std::bitset<64> MSNoLogicJunction::myDump((unsigned long) 0xffffffff);



// ===========================================================================
// method definitions
// ===========================================================================
MSNoLogicJunction::MSNoLogicJunction(string id, const Position2D &position,
                                     LaneCont incoming
#ifdef HAVE_INTERNAL_LANES
                                     , LaneCont internal
#endif
                                    )
        : MSJunction(id, position),
        myIncomingLanes(incoming)
#ifdef HAVE_INTERNAL_LANES
        , myInternalLanes(internal)
#endif
{}


bool
MSNoLogicJunction::clearRequests()
{
    return true;
}

//-------------------------------------------------------------------------//

MSNoLogicJunction::~MSNoLogicJunction()
{}

//-------------------------------------------------------------------------//

void
MSNoLogicJunction::postloadInit()
{
    LaneCont::iterator i;
    // inform links where they have to report approaching vehicles to
    for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myDump, 0, &myDump, 0, MSLogicJunction::LinkFoes());
        }
    }
#ifdef HAVE_INTERNAL_LANES
    // set information for the internal lanes
    for (i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
        // ... set information about participation
        static_cast<MSInternalLane*>(*i)->setParentJunctionInformation(
            &myDump, 0);
    }
#endif
}



/****************************************************************************/

