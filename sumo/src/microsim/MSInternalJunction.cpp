/****************************************************************************/
/// @file    MSInternalJunction.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junction.
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

#include "MSInternalJunction.h"
#include "MSLane.h"
#include "MSJunctionLogic.h"
#include "MSBitSetLogic.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
#ifdef HAVE_INTERNAL_LANES
MSInternalJunction::MSInternalJunction(const std::string &id,
                                       const Position2D &position,
                                       const Position2DVector &shape,
                                       std::vector<MSLane*> incoming,
                                       std::vector<MSLane*> internal) throw()
        : MSLogicJunction(id, position, shape, incoming, internal) {}


bool
MSInternalJunction::clearRequests() {
    return true;
}


MSInternalJunction::~MSInternalJunction() {}


void
MSInternalJunction::postloadInit() throw(ProcessError) {
    // inform links where they have to report approaching vehicles to
    unsigned int requestPos = 0;
    myInternalLaneFoes = myInternalLanes;
    for (std::vector<MSLane*>::const_iterator i=myIncomingLanes.begin()+1; i!=myIncomingLanes.end(); ++i) {
        MSLane *l = *i;
        const MSLinkCont &lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            myInternalLinkFoes.push_back(*j);
        }
    }
    if (myIncomingLanes.size()!=0) {
        // for the first incoming lane
        const MSLinkCont &links = myIncomingLanes[0]->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myRequest, requestPos,
                                        &myRespond, requestPos,
                                        MSLogicJunction::LinkFoes(), true, false,
                                        myInternalLinkFoes, myInternalLaneFoes);
            requestPos++;
            for (std::vector<MSLink*>::const_iterator k=myInternalLinkFoes.begin(); k!=myInternalLinkFoes.end(); ++k) {
                (*j)->addBlockedLink(*k);
                (*k)->addBlockedLink(*j);
            }
        }
    }
}


#endif


/****************************************************************************/

