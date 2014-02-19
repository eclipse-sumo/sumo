/****************************************************************************/
/// @file    MSInternalJunction.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junction.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
MSInternalJunction::MSInternalJunction(const std::string& id,
                                       const Position& position,
                                       const PositionVector& shape,
                                       std::vector<MSLane*> incoming,
                                       std::vector<MSLane*> internal)
    : MSLogicJunction(id, position, shape, incoming, internal) {}



MSInternalJunction::~MSInternalJunction() {}


void
MSInternalJunction::postloadInit() {
    if (myIncomingLanes.size() == 0) {
        throw ProcessError("Internal junction " + getID() + " has no incoming lanes");
    }
    // the first lane in the list of incoming lanes is special. It defines the
    // link that needs to do all the checking for this internal junction
    assert(myIncomingLanes[0]->getLinkCont().size() == 1);
    MSLink* thisLink = myIncomingLanes[0]->getLinkCont()[0];
    // inform links where they have to report approaching vehicles to
    unsigned int requestPos = 0;
    for (std::vector<MSLane*>::iterator i = myInternalLanes.begin(); i != myInternalLanes.end(); ++i) {
        const MSLinkCont& lc = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator q = lc.begin(); q != lc.end(); ++q) {
            if ((*q)->getViaLane() != 0) {
                myInternalLaneFoes.push_back((*q)->getViaLane());
            } else {
                myInternalLaneFoes.push_back(*i);
            }
        }

    }
    for (std::vector<MSLane*>::const_iterator i = myIncomingLanes.begin() + 1; i != myIncomingLanes.end(); ++i) {
        MSLane* l = *i;
        const MSLinkCont& lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); ++j) {
            MSLane* via = (*j)->getViaLane();
            if (std::find(myInternalLanes.begin(), myInternalLanes.end(), via) == myInternalLanes.end()) {
                continue;
            }
            myInternalLinkFoes.push_back(*j);
        }
    }
    thisLink->setRequestInformation(requestPos, true, false, myInternalLinkFoes, myInternalLaneFoes);
    assert(thisLink->getViaLane()->getLinkCont().size() == 1);
    MSLink* exitLink = thisLink->getViaLane()->getLinkCont()[0];
    exitLink->setRequestInformation(requestPos, false, false, std::vector<MSLink*>(),
                                    myInternalLaneFoes, thisLink->getViaLane());
    for (std::vector<MSLink*>::const_iterator k = myInternalLinkFoes.begin(); k != myInternalLinkFoes.end(); ++k) {
        thisLink->addBlockedLink(*k);
        (*k)->addBlockedLink(thisLink);
    }
}


#endif


/****************************************************************************/

