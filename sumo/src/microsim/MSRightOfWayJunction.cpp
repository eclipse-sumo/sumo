/****************************************************************************/
/// @file    MSRightOfWayJunction.cpp
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

#include "MSRightOfWayJunction.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSJunctionLogic.h"
#include "MSGlobals.h"
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
MSRightOfWayJunction::MSRightOfWayJunction(const std::string& id,
        SumoXMLNodeType type,
        const Position& position,
        const PositionVector& shape,
        std::vector<MSLane*> incoming,
#ifdef HAVE_INTERNAL_LANES
        std::vector<MSLane*> internal,
#endif
        MSJunctionLogic* logic)
    : MSLogicJunction(id, type, position, shape, incoming
#ifdef HAVE_INTERNAL_LANES
    , internal),
#else
                     ),
#endif
      myLogic(logic) {}


MSRightOfWayJunction::~MSRightOfWayJunction() {
    delete myLogic;
}


void
MSRightOfWayJunction::postloadInit() {
    // inform links where they have to report approaching vehicles to
    int requestPos = 0;
    std::vector<MSLane*>::iterator i;
    // going through the incoming lanes...
    int maxNo = 0;
    std::vector<std::pair<MSLane*, MSLink*> > sortedLinks;
    for (i = myIncomingLanes.begin(); i != myIncomingLanes.end(); ++i) {
        const MSLinkCont& links = (*i)->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j = links.begin(); j != links.end(); j++) {
            if ((*j)->getLane()->getEdge().isWalkingArea() ||
                    ((*i)->getEdge().isWalkingArea() && !(*j)->getLane()->getEdge().isCrossing())) {
                continue;
            }
            sortedLinks.push_back(std::make_pair(*i, *j));
            ++maxNo;
        }
    }

    const bool hasFoes = myLogic->hasFoes();
    for (i = myIncomingLanes.begin(); i != myIncomingLanes.end(); ++i) {
        const MSLinkCont& links = (*i)->getLinkCont();
        // ... set information for every link
        for (MSLinkCont::const_iterator j = links.begin(); j != links.end(); j++) {
            if ((*j)->getLane()->getEdge().isWalkingArea() ||
                    ((*i)->getEdge().isWalkingArea() && !(*j)->getLane()->getEdge().isCrossing())) {
                continue;
            }
            if (myLogic->getLogicSize() <= requestPos) {
                throw ProcessError("Found invalid logic position of a link for junction '" + getID() + "' (" + toString(requestPos) + ", max " + toString(myLogic->getLogicSize()) + ") -> (network error)");
            }
            const MSLogicJunction::LinkBits& linkResponse = myLogic->getResponseFor(requestPos); // SUMO_ATTR_RESPONSE
            const MSLogicJunction::LinkBits& linkFoes = myLogic->getFoesFor(requestPos); // SUMO_ATTR_FOES
            bool cont = myLogic->getIsCont(requestPos);
            myLinkFoeLinks[*j] = std::vector<MSLink*>();
            for (int c = 0; c < maxNo; ++c) {
                if (linkResponse.test(c)) {
                    MSLink* foe = sortedLinks[c].second;
                    myLinkFoeLinks[*j].push_back(foe);
#ifdef HAVE_INTERNAL_LANES
                    if (MSGlobals::gUsingInternalLanes && foe->getViaLane() != 0) {
                        assert(foe->getViaLane()->getLinkCont().size() == 1);
                        MSLink* foeExitLink = foe->getViaLane()->getLinkCont()[0];
                        // add foe links after an internal junction
                        if (foeExitLink->getViaLane() != 0) {
                            myLinkFoeLinks[*j].push_back(foeExitLink);
                        }
                    }
#endif
                }
            }
            std::vector<MSLink*> foes;
            for (int c = 0; c < maxNo; ++c) {
                if (linkFoes.test(c)) {
                    MSLink* foe = sortedLinks[c].second;
                    foes.push_back(foe);
#ifdef HAVE_INTERNAL_LANES
                    MSLane* l = foe->getViaLane();
                    if (l == 0) {
                        continue;
                    }
                    // add foe links after an internal junction
                    const MSLinkCont& lc = l->getLinkCont();
                    for (MSLinkCont::const_iterator q = lc.begin(); q != lc.end(); ++q) {
                        if ((*q)->getViaLane() != 0) {
                            foes.push_back(*q);
                        }
                    }
#endif
                }
            }

            myLinkFoeInternalLanes[*j] = std::vector<MSLane*>();
#ifdef HAVE_INTERNAL_LANES
            if (MSGlobals::gUsingInternalLanes && myInternalLanes.size() > 0) {
                int li = 0;
                for (int c = 0; c < (int)sortedLinks.size(); ++c) {
                    if (sortedLinks[c].second->getLane() == 0) { // dead end
                        continue;
                    }
                    if (linkFoes.test(c)) {
                        myLinkFoeInternalLanes[*j].push_back(myInternalLanes[li]);
                        if (linkResponse.test(c)) {
                            const std::vector<MSLane::IncomingLaneInfo>& l = myInternalLanes[li]->getIncomingLanes();
                            if (l.size() == 1 && l[0].lane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
                                myLinkFoeInternalLanes[*j].push_back(l[0].lane);
                            }
                        }
                    }
                    ++li;
                }
            }
#endif
            (*j)->setRequestInformation((int)requestPos, hasFoes, cont, myLinkFoeLinks[*j], myLinkFoeInternalLanes[*j]);
#ifdef HAVE_INTERNAL_LANES
            // the exit link for a link before an internal junction is handled in MSInternalJunction
            // so we need to skip if cont=true
            if (MSGlobals::gUsingInternalLanes && (*j)->getViaLane() != 0 && !cont) {
                assert((*j)->getViaLane()->getLinkCont().size() == 1);
                MSLink* exitLink = (*j)->getViaLane()->getLinkCont()[0];
                exitLink->setRequestInformation((int)requestPos, false, false, std::vector<MSLink*>(),
                                                myLinkFoeInternalLanes[*j], (*j)->getViaLane());
            }
#endif
            for (std::vector<MSLink*>::const_iterator k = foes.begin(); k != foes.end(); ++k) {
                (*j)->addBlockedLink(*k);
                (*k)->addBlockedLink(*j);
            }
            requestPos++;
        }
    }
}


/****************************************************************************/

