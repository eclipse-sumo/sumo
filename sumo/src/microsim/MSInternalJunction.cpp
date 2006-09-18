/***************************************************************************
                          MSInternalJunction.cpp  -  Usual right-of-way
                          junction.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
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
// Revision 1.1  2006/09/18 10:06:29  dkrajzew
// patching junction-internal state simulation
//
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

#include "MSInternalJunction.h"
#include "MSLane.h"
#include "MSJunctionLogic.h"
#include "MSBitSetLogic.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * method definitions
 * ======================================================================= */
#ifdef HAVE_INTERNAL_LANES
MSInternalJunction::MSInternalJunction( string id,
                                            const Position2D &position,
                                            LaneCont incoming,
                                            LaneCont internal)
    : MSLogicJunction( id, position, incoming, internal )
{
}


bool
MSInternalJunction::clearRequests()
{
    return true;
}


MSInternalJunction::~MSInternalJunction()
{
}


bool
MSInternalJunction::setAllowed()
{
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime&&myID==debug_searchedJunction) {
        DEBUG_OUT << "Request: " << myRequest << endl;
        DEBUG_OUT << "InnerSt: " << myInnerState<< endl;
    }
#endif
    // Get myRespond from logic and check for deadlocks.
    bool allow = true;
    LaneCont::iterator i;

    // do nothing if there is no vehicle
    if(myIncomingLanes[0]->empty()) {
        myRespond.set(0, allow);
        return true;
    }


    // do not move if any other internal (foe) lane is set
    for(i=myInternalLanes.begin(); allow&&i!=myInternalLanes.end(); ++i) {
        if(!(*i)->empty()) {
            allow = false;
        }
    }
    // do not move if a vehicle is approaching on a link from foe lanes
    for(i=myIncomingLanes.begin()+1; allow&&i!=myIncomingLanes.end(); ++i) {
        MSLane *l = *i;
        const MSLinkCont &lc = l->getLinkCont();
        for(MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            if(find(myInternalLanes.begin(), myInternalLanes.end(), (*j)->getViaLane())!=myInternalLanes.end()) {
                if((*j)->isApproached()) {
                    allow = false;
                }
            }
        }
    }
    // do not move if the destination lane is full
        // get the next lane
    MSLane *l = myIncomingLanes[0];
    const MSLinkCont &lc1 = l->getLinkCont();
    MSLink *link = lc1[0];
    l = link->getLane();
        // get the destination lane
    const MSLinkCont &lc2 = l->getLinkCont();
    link = lc2[0];
    MSLane *dest = link->getLane();
    const MSVehicle * const lastOnDest = dest->getLastVehicle();
    if(lastOnDest!=0) {
        if(lastOnDest->getPositionOnLane()-lastOnDest->getLength()<5) { // !!! explcite vehicle length
            allow = false;
        }
    }



    //
    myRespond.set(0, allow);
    /*
    myLogic->respond( myRequest, myInnerState, myRespond );
    deadlockKiller();
    */
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime&&myID==debug_searchedJunction) {
        DEBUG_OUT << "Respond: " << myRespond << endl;
    }
#endif
    return true;
}

#endif

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
