/****************************************************************************/
/// @file    MSLogicJunction.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// with one ore more logics.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "MSLinkCont.h"
#include "MSLogicJunction.h"
#include "MSLane.h"
#include "MSInternalLane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods from MSLogicJunction
 * ----------------------------------------------------------------------- */
MSLogicJunction::MSLogicJunction(string id, const Position2D &position,
                                 LaneCont incoming
#ifdef HAVE_INTERNAL_LANES
                                 , LaneCont internal
#endif
                                )
        : MSJunction(id, position),
        myIncomingLanes(incoming),
#ifdef HAVE_INTERNAL_LANES
        myInternalLanes(internal),
#endif
        myRequest(false), myInnerState(false), myRespond(false)
{}

//-------------------------------------------------------------------------//

MSLogicJunction::~MSLogicJunction()
{}

//-------------------------------------------------------------------------//

void
MSLogicJunction::postloadInit() throw(ProcessError)
{
    /*
    if(getID()=="1565") {
        int bla = 0;
    }
    // inform links where they have to report approaching vehicles to
    size_t requestPos = 0;
    LaneCont::iterator i;
    // going through the incoming lanes...
    for(i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        // ... set information for every link
        for(MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myRequest, requestPos,
                &myRespond, requestPos/, clearInfo/);
            requestPos++;
        }
    }
    #ifdef HAVE_INTERNAL_LANES
    // set information for the internal lanes
    requestPos = 0;
    for(i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
        // ... set information about participation
        static_cast<MSInternalLane*>(*i)->setParentJunctionInformation(
            &myInnerState, requestPos++);
    }
    #endif
    */
}



/****************************************************************************/

