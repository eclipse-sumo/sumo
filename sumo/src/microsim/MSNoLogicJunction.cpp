/****************************************************************************/
/// @file    MSNoLogicJunction.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// -------------------
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
// static member definitions
// ===========================================================================
std::bitset<64> MSNoLogicJunction::myDump((unsigned long) 0xffffffff);



// ===========================================================================
// method definitions
// ===========================================================================
MSNoLogicJunction::MSNoLogicJunction(const std::string &id,
                                     const Position2D &position,
                                     const Position2DVector &shape,
                                     std::vector<MSLane*> incoming
#ifdef HAVE_INTERNAL_LANES
                                     , std::vector<MSLane*> internal
#endif
                                    ) throw()
        : MSJunction(id, position, shape),
        myIncomingLanes(incoming)
#ifdef HAVE_INTERNAL_LANES
        , myInternalLanes(internal)
#endif
{
}


bool
MSNoLogicJunction::clearRequests() {
    return true;
}

//-------------------------------------------------------------------------//

MSNoLogicJunction::~MSNoLogicJunction() {}

//-------------------------------------------------------------------------//

void
MSNoLogicJunction::postloadInit() throw(ProcessError) {
    std::vector<MSLane*>::iterator i;
    // inform links where they have to report approaching vehicles to
    for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(this, &myDump, &myNewRequest, 0, &myDump, 0, MSLogicJunction::LinkFoes(), false, false);
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
    myNewRequest = std::vector<LinkApproachingVehicles>(1);
}



/****************************************************************************/

