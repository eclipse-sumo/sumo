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
    unsigned int maxNo = 0;
    std::vector<std::pair<MSLane*, MSLink*> > sortedLinks;
    for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            if (myLogic->getLogicSize()<=requestPos) {
                throw ProcessError("Found invalid logic position of a link (network error)");
            }
            sortedLinks.push_back(std::make_pair(*i, *j));
            ++maxNo;
        }
    }

    bool isCrossing = myLogic->isCrossing();
    for (i=myIncomingLanes.begin(); i!=myIncomingLanes.end(); ++i) {
        const MSLinkCont &links = (*i)->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            if (myLogic->getLogicSize()<=requestPos) {
                throw ProcessError("Found invalid logic position of a link (network error)");
            }
            const MSLogicJunction::LinkFoes &foeLinks = myLogic->getFoesFor(requestPos);
            const std::bitset<64> &internalFoes = myLogic->getInternalFoesFor(requestPos);
            bool cont = myLogic->getIsCont(requestPos);
            myLinkFoeLinks[*j] = std::vector<MSLink*>();
            for (unsigned int c=0; c<maxNo; ++c) {
                if (foeLinks.test(c)) {
                    myLinkFoeLinks[*j].push_back(sortedLinks[c].second);
                }
            }
            myLinkFoeInternalLanes[*j] = std::vector<MSLane*>();
            for (unsigned int c=0; c<myInternalLanes.size(); ++c) {
                if (internalFoes.test(c)) {
                    myLinkFoeInternalLanes[*j].push_back(myInternalLanes[c]);
                }
            }
            (*j)->setRequestInformation(&myRequest, requestPos, &myRespond, requestPos,
                                        foeLinks, isCrossing, cont, myLinkFoeLinks[*j], myLinkFoeInternalLanes[*j]);
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


/****************************************************************************/

