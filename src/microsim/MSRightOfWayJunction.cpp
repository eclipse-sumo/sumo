/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSRightOfWayJunction.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSJunctionLogic.h"
#include "MSGlobals.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSRightOfWayJunction::MSRightOfWayJunction(const std::string& id,
        SumoXMLNodeType type,
        const Position& position,
        const PositionVector& shape,
        std::vector<MSLane*> incoming,
        std::vector<MSLane*> internal,
        MSJunctionLogic* logic) : MSLogicJunction(id, type, position, shape, incoming, internal),
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
        for (auto link : links) {
            if (link->getLane()->getEdge().isWalkingArea() ||
                    ((*i)->getEdge().isWalkingArea() && !link->getLane()->getEdge().isCrossing())) {
                continue;
            }
            sortedLinks.push_back(std::make_pair(*i, link));
            ++maxNo;
        }
    }

    const bool hasFoes = myLogic->hasFoes();
    for (i = myIncomingLanes.begin(); i != myIncomingLanes.end(); ++i) {
        const MSLinkCont& links = (*i)->getLinkCont();
        // ... set information for every link
        const MSLane* walkingAreaFoe = nullptr;
        for (auto link : links) {
            if (link->getLane()->getEdge().isWalkingArea()) {
                if ((*i)->getPermissions() != SVC_PEDESTRIAN) {
                    // vehicular lane connects to a walkingarea
                    walkingAreaFoe = link->getLane();
                }
                continue;
            } else if (((*i)->getEdge().isWalkingArea() && !link->getLane()->getEdge().isCrossing())) {
                continue;
            }
            if (myLogic->getLogicSize() <= requestPos) {
                throw ProcessError("Found invalid logic position of a link for junction '" + getID() + "' (" + toString(requestPos) + ", max " + toString(myLogic->getLogicSize()) + ") -> (network error)");
            }
            const MSLogicJunction::LinkBits& linkResponse = myLogic->getResponseFor(requestPos); // SUMO_ATTR_RESPONSE
            const MSLogicJunction::LinkBits& linkFoes = myLogic->getFoesFor(requestPos); // SUMO_ATTR_FOES
            bool cont = myLogic->getIsCont(requestPos);
            myLinkFoeLinks[link] = std::vector<MSLink*>();
            for (int c = 0; c < maxNo; ++c) {
                if (linkResponse.test(c)) {
                    MSLink* foe = sortedLinks[c].second;
                    myLinkFoeLinks[link].push_back(foe);
                    if (MSGlobals::gUsingInternalLanes && foe->getViaLane() != nullptr) {
                        assert(foe->getViaLane()->getLinkCont().size() == 1);
                        MSLink* foeExitLink = foe->getViaLane()->getLinkCont()[0];
                        // add foe links after an internal junction
                        if (foeExitLink->getViaLane() != nullptr) {
                            myLinkFoeLinks[link].push_back(foeExitLink);
                        }
                    }
                }
            }
            std::vector<MSLink*> foes;
            for (int c = 0; c < maxNo; ++c) {
                if (linkFoes.test(c)) {
                    MSLink* foe = sortedLinks[c].second;
                    foes.push_back(foe);
                    MSLane* l = foe->getViaLane();
                    if (l == nullptr) {
                        continue;
                    }
                    // add foe links after an internal junction
                    const MSLinkCont& lc = l->getLinkCont();
                    for (auto q : lc) {
                        if (q->getViaLane() != nullptr) {
                            foes.push_back(q);
                        }
                    }
                }
            }

            myLinkFoeInternalLanes[link] = std::vector<MSLane*>();
            if (MSGlobals::gUsingInternalLanes && myInternalLanes.size() > 0) {
                int li = 0;
                for (int c = 0; c < (int)sortedLinks.size(); ++c) {
                    if (sortedLinks[c].second->getLane() == nullptr) { // dead end
                        continue;
                    }
                    if (linkFoes.test(c)) {
                        myLinkFoeInternalLanes[link].push_back(myInternalLanes[li]);
                        if (linkResponse.test(c)) {
                            const std::vector<MSLane::IncomingLaneInfo>& l = myInternalLanes[li]->getIncomingLanes();
                            if (l.size() == 1 && l[0].lane->getEdge().isInternal()) {
                                myLinkFoeInternalLanes[link].push_back(l[0].lane);
                            }
                        }
                    }
                    ++li;
                }
            }
            link->setRequestInformation((int)requestPos, hasFoes, cont, myLinkFoeLinks[link], myLinkFoeInternalLanes[link]);
            // the exit link for a link before an internal junction is handled in MSInternalJunction
            // so we need to skip if cont=true
            if (MSGlobals::gUsingInternalLanes && link->getViaLane() != nullptr && !cont) {
                assert((*j)->getViaLane()->getLinkCont().size() == 1);
                MSLink* exitLink = link->getViaLane()->getLinkCont()[0];
                exitLink->setRequestInformation((int)requestPos, false, false, std::vector<MSLink*>(),
                                                myLinkFoeInternalLanes[link], link->getViaLane());
            }
            // the exit link for a crossing is needed for the pedestrian model
            if (MSGlobals::gUsingInternalLanes && link->getLane()->getEdge().isCrossing()) {
                MSLink* exitLink = link->getLane()->getLinkCont()[0];
                exitLink->setRequestInformation((int)requestPos, false, false, std::vector<MSLink*>(),
                                                myLinkFoeInternalLanes[link], link->getLane());
            }
            for (std::vector<MSLink*>::const_iterator k = foes.begin(); k != foes.end(); ++k) {
                link->addBlockedLink(*k);
                (*k)->addBlockedLink(link);
            }
            requestPos++;
        }
        if (walkingAreaFoe != nullptr && links.size() > 1) {
            for (auto link : links) {
                if (!link->getLane()->getEdge().isWalkingArea()) {
                    MSLink* exitLink = link->getViaLane()->getLinkCont()[0];
                    exitLink->addWalkingAreaFoe(walkingAreaFoe);
                }
            }
        }
    }
}


/****************************************************************************/

