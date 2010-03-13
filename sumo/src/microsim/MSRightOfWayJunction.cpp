/****************************************************************************/
/// @file    MSRightOfWayJunction.cpp
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

#include "MSRightOfWayJunction.h"
#include "MSLane.h"
#include "MSJunctionLogic.h"
#include "MSBitSetLogic.h"
#include "MSGlobals.h"
#include "MSInternalLane.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSRightOfWayJunction::MSRightOfWayJunction(const std::string &id,
        const Position2D &position,
        const Position2DVector &shape,
        std::vector<MSLane*> incoming,
#ifdef HAVE_INTERNAL_LANES
        std::vector<MSLane*> internal,
#endif
        MSJunctionLogic* logic) throw()
        : MSLogicJunction(id, position, shape, incoming
#ifdef HAVE_INTERNAL_LANES
                          , internal),
#else
                         ),
#endif
        myLogic(logic) {}


bool
MSRightOfWayJunction::clearRequests() {
    myRequest.reset();
    myInnerState.reset();
    return true;
}


MSRightOfWayJunction::~MSRightOfWayJunction() {
    delete myLogic;
}


void
MSRightOfWayJunction::postloadInit() throw(ProcessError) {
    // inform links where they have to report approaching vehicles to
    unsigned int requestPos = 0;
    std::vector<MSLane*>::iterator i;
    // going through the incoming lanes...
    bool isCrossing = myLogic->isCrossing();
    for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            if (myLogic->getLogicSize()<=requestPos) {
                throw ProcessError("Found invalid logic position of a link (network error)");
            }
            (*j)->setRequestInformation(&myRequest, requestPos, &myRespond, requestPos, myLogic->getFoesFor(requestPos), isCrossing);
            requestPos++;
        }
    }
#ifdef HAVE_INTERNAL_LANES
    // set information for the internal lanes
    requestPos = 0;
    for (i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
        // ... set information about participation
        static_cast<MSInternalLane*>(*i)->setParentJunctionInformation(&myInnerState, requestPos++);
    }
#endif
}


bool
MSRightOfWayJunction::setAllowed() {
#ifdef HAVE_INTERNAL_LANES
    // lets reset the yield information on internal, split
    //  left-moving links
    /*
    if (MSGlobals::gUsingInternalLanes) {
        std::vector<MSLane*>::iterator i;
        size_t requestPos = 0;
        // going through the incoming lanes...
        for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
            const MSLinkCont &links = (*i)->getLinkCont();
            // check whether the next lane is free
            for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
                if(myRequest.test(requestPos)) {
                    MSLane *dest = (*j)->getLane();
                    if (dest!=0) {
                        SUMOReal approachingLength = 0;
                        MSLane *via = 0;
                        if(via!=0) {
                            approachingLength = via->getVehLenSum();
                        }
                        const MSVehicle * const lastOnDest = dest->getLastVehicle();
                        if (lastOnDest!=0) {
                            if (lastOnDest->getPositionOnLane()-lastOnDest->getLength()-approachingLength<0) {
                                myRequest.set(requestPos, false);
                            }
                        }
                    }
                }
                requestPos++;
            }
        }
    }
    */
#endif
    // Get myRespond from logic and check for deadlocks.
    myLogic->respond(myRequest, myInnerState, myRespond);
    deadlockKiller();

#ifdef HAVE_INTERNAL_LANES
    // reset the yield information on internal, split left-moving links
    if (MSGlobals::gUsingInternalLanes) {
        for (std::vector<MSLane*>::iterator i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
            const MSLinkCont &lc = (*i)->getLinkCont();
            if (lc.size()==1) {
                MSLink *link = lc[0];
                if (link->getViaLane()!=0) {
                    // this is a split left-mover
                    link->resetInternalPriority();
                }
            }
        }
    }
#endif
    myInnerState.reset();
    return true;
}


void
MSRightOfWayJunction::deadlockKiller() {
    if (myRequest.none()) {
        return;
    }

    // let's assume temporary, that deadlocks only occure on right-before-left
    //  junctions
    if (myRespond.none() && myInnerState.none()) {
        // Handle deadlock: Create randomly a deadlock-free request out of
        // myRequest, i.e. a "single bit" request. Then again, send it
        // through myLogic (this is necessary because we don't have a
        // mapping between requests and lanes.) !!! (we do now!!)
        vector< unsigned > trueRequests;
        trueRequests.reserve(myRespond.size());
        for (unsigned i = 0; i < myRequest.size(); ++i) {
            if (myRequest.test(i)) {
                trueRequests.push_back(i);
                assert(trueRequests.size() <= myRespond.size());
            }
        }
        // Choose randomly an index out of [0,trueRequests.size()];
        // !!! random choosing may choose one of less priorised lanes
        unsigned int noLockIndex = (unsigned int) RandHelper::rand(trueRequests.size());

        // Create deadlock-free request.
        std::bitset<64> noLockRequest(false);
        assert(trueRequests.size()>noLockIndex);
        noLockRequest.set(trueRequests[ noLockIndex ]);
        // Calculate respond with deadlock-free request.
        myLogic->respond(noLockRequest, myInnerState,  myRespond);
    }
    return;
}


/****************************************************************************/

