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
    if (myIncomingLanes.size()!=0) {
        // for the first incoming lane
        const MSLinkCont &links = myIncomingLanes[0]->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myRequest, requestPos,
                                        &myRespond, requestPos,
                                        MSLogicJunction::LinkFoes(), true);
            requestPos++;
        }
    }
}


bool
MSInternalJunction::setAllowed() {
    // Get myRespond from logic and check for deadlocks.
    myRespond.set(0, true);
    std::vector<MSLane*>::iterator i;
    if (myIncomingLanes.size()==0) {
        return true;
    }

    // do nothing if there is no vehicle
    if (!myRequest.test(0)) {
        return true;
    }

    // do not move if any other internal (foe) lane is set
    for (i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
        if (!(*i)->empty()) {
            myRespond.set(0, false);
            return true;
        }
    }
    // do not move if a vehicle is approaching on a link from foe lanes
    //  the first entry is our lane itself, the following should be those that feed
    //  the internal lanes
    for (i=myIncomingLanes.begin()+1; i!=myIncomingLanes.end(); ++i) {
        MSLane *l = *i;
        const MSVehicle * const foe = l->getFirstVehicle();
        const MSLinkCont &lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            if (find(myInternalLanes.begin(), myInternalLanes.end(), (*j)->getViaLane())!=myInternalLanes.end()) {
                bool approached = foe!=0&&(*j)->getApproaching()==foe;
                approached |= ((*j)->getViaLane()!=0&&!(*j)->getViaLane()->empty());
                if (approached&&((*j)->opened()||(*j)->havePriority())) {
                    myRespond.set(0, false);
                    return true;
                }
            }
        }
    }
    /*
    // do not move if the destination lane is full
    // get the next lane
    // - recheck whether this is really needed !!!
    MSLane *l = myIncomingLanes[0];
    const MSLinkCont &lc1 = l->getLinkCont();
    MSLink *link = lc1[0];
    MSLane *dest = link->getLane();
    if (dest==0) {
        return true;
    }
    const MSVehicle * const lastOnDest = dest->getLastVehicle();
    if (lastOnDest!=0) {
        if (lastOnDest->getPositionOnLane()-lastOnDest->getLength()<5) { // !!! explcite vehicle length
            myRespond.set(0, false);
            return true;
        }
    }
    */
    return true;
}


#endif


/****************************************************************************/

