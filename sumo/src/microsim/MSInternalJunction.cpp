/****************************************************************************/
/// @file    MSInternalJunction.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junction.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include "MSRightOfWayJunction.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSJunctionLogic.h"
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
                                       SumoXMLNodeType type,
                                       const Position& position,
                                       const PositionVector& shape,
                                       std::vector<MSLane*> incoming,
                                       std::vector<MSLane*> internal)
    : MSLogicJunction(id, type, position, shape, incoming, internal) {}



MSInternalJunction::~MSInternalJunction() {}


void
MSInternalJunction::postloadInit() {
    if (myIncomingLanes.size() == 0) {
        throw ProcessError("Internal junction " + getID() + " has no incoming lanes");
    }
    // the first lane in the list of incoming lanes is special. It defines the
    // link that needs to do all the checking for this internal junction
    const MSLane* specialLane = myIncomingLanes[0];
    assert(specialLane->getLinkCont().size() == 1);
    MSLink* thisLink = specialLane->getLinkCont()[0];
    const MSRightOfWayJunction* parent = dynamic_cast<const MSRightOfWayJunction*>(specialLane->getEdge().getToJunction());
    if (parent == 0) {
        // parent has type traffic_light_unregulated
        return;
    }
    const int ownLinkIndex = specialLane->getIncomingLanes()[0].viaLink->getIndex();
    const MSLogicJunction::LinkBits& response = parent->getLogic()->getResponseFor(ownLinkIndex);
    // inform links where they have to report approaching vehicles to
    int requestPos = 0;
    for (std::vector<MSLane*>::iterator i = myInternalLanes.begin(); i != myInternalLanes.end(); ++i) {
        const MSLinkCont& lc = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator q = lc.begin(); q != lc.end(); ++q) {
            if ((*q)->getViaLane() != 0) {
                const int foeIndex = (*i)->getIncomingLanes()[0].viaLink->getIndex();
                if (response.test(foeIndex)) {
                    // only respect vehicles before internal junctions if they
                    // have priority (see the analogous foeLinks.test() when
                    // initializing myLinkFoeInternalLanes in MSRightOfWayJunction
                    myInternalLaneFoes.push_back(*i);
                }
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
    // thisLinks is itself an exitLink of the preceding internal lane
    thisLink->setRequestInformation((int)requestPos, true, false, myInternalLinkFoes, myInternalLaneFoes, thisLink->getViaLane()->getLogicalPredecessorLane());
    assert(thisLink->getViaLane()->getLinkCont().size() == 1);
    MSLink* exitLink = thisLink->getViaLane()->getLinkCont()[0];
    exitLink->setRequestInformation((int)requestPos, false, false, std::vector<MSLink*>(),
                                    myInternalLaneFoes, thisLink->getViaLane());
    for (std::vector<MSLink*>::const_iterator k = myInternalLinkFoes.begin(); k != myInternalLinkFoes.end(); ++k) {
        thisLink->addBlockedLink(*k);
        (*k)->addBlockedLink(thisLink);
    }
}


#endif


/****************************************************************************/

