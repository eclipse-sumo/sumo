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
// Revision 1.6  2006/10/06 07:13:40  dkrajzew
// debugging internal lanes
//
// Revision 1.5  2006/10/05 11:24:34  dkrajzew
// debugging options parsing
//
// Revision 1.4  2006/10/04 13:18:17  dkrajzew
// debugging internal lanes, multiple vehicle emission and net building
//
// Revision 1.3  2006/09/21 09:45:50  dkrajzew
// code beautifying
//
// Revision 1.2  2006/09/19 11:48:23  dkrajzew
// debugging junction-internal lanes
//
// Revision 1.1  2006/09/18 10:06:29  dkrajzew
// patching junction-internal state simulation
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


void
MSInternalJunction::postloadInit()
{
    // inform links where they have to report approaching vehicles to
    size_t requestPos = 0;
    LaneCont::iterator i;
    // going through the incoming lanes...
    const MSLinkCont &links = myIncomingLanes[0]->getLinkCont();
    // ... set information for every link
    for(MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
        (*j)->setRequestInformation(&myRequest, requestPos,
            &myRespond, requestPos/*, clearInfo*/);
        requestPos++;
    }
    /*
#ifdef HAVE_INTERNAL_LANES
    // set information for the internal lanes
    requestPos = 0;
    for(i=myInternalLanes.begin(); i!=myInternalLanes.end(); i++) {
        // ... set information about participation
        static_cast<MSInternalLane*>(*i)->setParentJunctionInformation(
            &myInnerState, requestPos++);
    }
#endif
    */
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
    myRespond.set(0, true);
    LaneCont::iterator i;

    // do nothing if there is no vehicle
    if(myIncomingLanes[0]->empty()) {
        return true;
    }

    // do not move if any other internal (foe) lane is set
    for(i=myInternalLanes.begin(); i!=myInternalLanes.end(); ++i) {
        if(!(*i)->empty()) {
            myRespond.set(0, false);
            return true;
        }
    }
    // do not move if a vehicle is approaching on a link from foe lanes
    //  the first entry is our lane itself, the following should be those that feed
    //  the internal lanes
    for(i=myIncomingLanes.begin()+1; i!=myIncomingLanes.end(); ++i) {
        MSLane *l = *i;
        const MSLinkCont &lc = l->getLinkCont();
        for(MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            if(find(myInternalLanes.begin(), myInternalLanes.end(), (*j)->getViaLane())!=myInternalLanes.end()) {
                bool approached = (*j)->getViaLane()!=0&&(*j)->getViaLane()->myApproaching!=0;
                approached |= (*j)->getLane()->myApproaching!=0;
                if(approached&&((*j)->opened()||(*j)->havePriority())) {
                    myRespond.set(0, false);
                    return true;
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
            myRespond.set(0, false);
            return true;
        }
    }
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
