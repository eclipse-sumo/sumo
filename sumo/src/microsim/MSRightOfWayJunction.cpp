/****************************************************************************/
/// @file    MSRightOfWayJunction.cpp
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junction.
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

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// method definitions
// ===========================================================================
MSRightOfWayJunction::MSRightOfWayJunction(string id,
        const Position2D &position,
        LaneCont incoming,
#ifdef HAVE_INTERNAL_LANES
        LaneCont internal,
#endif
        MSJunctionLogic* logic)
        : MSLogicJunction(id, position, incoming
#ifdef HAVE_INTERNAL_LANES
                          , internal),
#else
                         ),
#endif
        myLogic(logic)
{}


bool
MSRightOfWayJunction::clearRequests()
{
    myRequest.reset();
    myInnerState.reset();
    return true;
}


MSRightOfWayJunction::~MSRightOfWayJunction()
{
    delete myLogic;
}


void
MSRightOfWayJunction::postloadInit()
{
    // inform links where they have to report approaching vehicles to
    size_t requestPos = 0;
    LaneCont::iterator i;
    // going through the incoming lanes...
    for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myRequest, requestPos,
                                        &myRespond, requestPos/*, clearInfo*/);
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
MSRightOfWayJunction::setAllowed()
{
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime&&myID==debug_searchedJunction) {
        DEBUG_OUT << "Request: " << myRequest << endl;
        DEBUG_OUT << "InnerSt: " << myInnerState<< endl;
    }
#endif
    // Get myRespond from logic and check for deadlocks.
    myLogic->respond(myRequest, myInnerState, myRespond);
    deadlockKiller();

#ifdef HAVE_INTERNAL_LANES
    // lets reset the yield information on internal, split
    //  left-moving links
    if (MSGlobals::gUsingInternalLanes) {
        for (LaneCont::iterator i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
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

#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime&&myID==debug_searchedJunction) {
        DEBUG_OUT << "Respond: " << myRespond << endl;
    }
#endif
    return true;
}


void
MSRightOfWayJunction::deadlockKiller()
{
    if (myRequest.none()) {
        return;
    }

    // let's assume temporary, that deadlocks only occure on right-before-left
    //  junctions
    if (myRespond.none() && myInnerState.none()) {
#ifdef ABS_DEBUG
        if (debug_globaltime>debug_searchedtime&&myID==debug_searchedJunction) {
            DEBUG_OUT << "Killing deadlock" << endl;
        }
#endif

        // Handle deadlock: Create randomly a deadlock-free request out of
        // myRequest, i.e. a "single bit" request. Then again, send it
        // through myLogic (this is neccessary because we don't have a
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
        unsigned noLockIndex = RandHelper::rand(trueRequests.size());

        // Create deadlock-free request.
        std::bitset<64> noLockRequest(false);
        assert(trueRequests.size()>noLockIndex);
        noLockRequest.set(trueRequests[ noLockIndex ]);
        // Calculate respond with deadlock-free request.
        myLogic->respond(noLockRequest, myInnerState,  myRespond);
    }
    return;
}


bool areRealFoes(MSLink *l1, MSLink *l2)
{
    if (l1->getLane()->getEdge()!=l2->getLane()->getEdge()) {
        return true;
    }
    return false;
}


/****************************************************************************/

