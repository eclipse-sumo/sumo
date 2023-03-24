/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSRightOfWayJunction.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 12 Dez 2001
///
// junction.
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utils/common/RandHelper.h>
#include "MSEdge.h"
#include "MSJunctionLogic.h"
#include "MSGlobals.h"
#include "MSLane.h"
#include "MSLink.h"
#include "MSRightOfWayJunction.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSRightOfWayJunction::MSRightOfWayJunction(const std::string& id,
        SumoXMLNodeType type,
        const Position& position,
        const PositionVector& shape,
        const std::string& name,
        std::vector<MSLane*> incoming,
        std::vector<MSLane*> internal,
        MSJunctionLogic* logic) : MSLogicJunction(id, type, position, shape, name, incoming, internal),
    myLogic(logic) {}


MSRightOfWayJunction::~MSRightOfWayJunction() {
    delete myLogic;
}


void
MSRightOfWayJunction::postloadInit() {
    // inform links where they have to report approaching vehicles to
    int requestPos = 0;
    // going through the incoming lanes...
    int maxNo = 0;
    std::vector<std::pair<const MSLane*, MSLink*> > sortedLinks;
    for (MSLane* const lane : myIncomingLanes) {
        // ... set information for every link
        for (MSLink* const link : lane->getLinkCont()) {
            if (link->getLane()->getEdge().isWalkingArea() ||
                    (lane->getEdge().isWalkingArea() && !link->getLane()->getEdge().isCrossing())) {
                continue;
            }
            sortedLinks.emplace_back(lane, link);
            ++maxNo;
        }
    }

    const bool hasFoes = myLogic->hasFoes();
    for (const MSLane* const lane : myIncomingLanes) {
        // ... set information for every link
        const MSLane* walkingAreaFoe = nullptr;
        for (MSLink* const link : lane->getLinkCont()) {
            if (link->getLane()->getEdge().isWalkingArea()) {
                if (lane->getPermissions() != SVC_PEDESTRIAN) {
                    // vehicular lane connects to a walkingarea
                    walkingAreaFoe = link->getLane();
                }
                continue;
            } else if ((lane->getEdge().isWalkingArea() && !link->getLane()->getEdge().isCrossing())) {
                continue;
            }
            if (myLogic->getLogicSize() <= requestPos) {
                throw ProcessError("Found invalid logic position of a link for junction '" + getID() + "' (" + toString(requestPos) + ", max " + toString(myLogic->getLogicSize()) + ") -> (network error)");
            }
            const MSLogicJunction::LinkBits& linkResponse = myLogic->getResponseFor(requestPos); // SUMO_ATTR_RESPONSE
            const MSLogicJunction::LinkBits& linkFoes = myLogic->getFoesFor(requestPos); // SUMO_ATTR_FOES
            bool cont = myLogic->getIsCont(requestPos);
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
                    for (MSLink* const vLink : l->getLinkCont()) {
                        if (vLink->getViaLane() != nullptr) {
                            foes.push_back(vLink);
                        }
                    }
                }
            }

            if (MSGlobals::gUsingInternalLanes && myInternalLanes.size() > 0) {
                int li = 0;
                for (int c = 0; c < (int)sortedLinks.size(); ++c) {
                    if (sortedLinks[c].second->getLane() == nullptr) { // dead end
                        continue;
                    }
                    if (linkFoes.test(c)) {
                        myLinkFoeInternalLanes[link].push_back(myInternalLanes[li]);
                        if (linkResponse.test(c) || sortedLinks[c].second->isIndirect()) {
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
                assert(link->getViaLane()->getLinkCont().size() == 1);
                MSLink* exitLink = link->getViaLane()->getLinkCont()[0];
                exitLink->setRequestInformation((int)requestPos, false, false, std::vector<MSLink*>(),
                                                myLinkFoeInternalLanes[link], link->getViaLane());
                for (const auto& ili : exitLink->getLane()->getIncomingLanes()) {
                    if (ili.lane->getEdge().isWalkingArea()) {
                        exitLink->addWalkingAreaFoeExit(ili.lane);
                        break;
                    }
                }
            }
            // the exit link for a crossing is needed for the pedestrian model
            if (MSGlobals::gUsingInternalLanes && link->getLane()->getEdge().isCrossing()) {
                MSLink* exitLink = link->getLane()->getLinkCont()[0];
                exitLink->setRequestInformation((int)requestPos, false, false, std::vector<MSLink*>(),
                                                myLinkFoeInternalLanes[link], link->getLane());
            }
            for (MSLink* const foe : foes) {
                link->addBlockedLink(foe);
                foe->addBlockedLink(link);
            }
            requestPos++;
        }
        if (walkingAreaFoe != nullptr && lane->getLinkCont().size() > 1) {
            for (const MSLink* const link : lane->getLinkCont()) {
                if (!link->getLane()->getEdge().isWalkingArea()) {
                    MSLink* exitLink = link->getViaLane()->getLinkCont()[0];
                    exitLink->addWalkingAreaFoe(walkingAreaFoe);
                }
            }
        }
    }
}


/****************************************************************************/
