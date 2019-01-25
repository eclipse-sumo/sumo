/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSNoLogicJunction.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSNoLogicJunction.h"
#include "MSLane.h"
#include <algorithm>
#include <cassert>
#include <cmath>


// ===========================================================================
// static member definitions
// ===========================================================================

// ===========================================================================
// method definitions
// ===========================================================================
MSNoLogicJunction::MSNoLogicJunction(const std::string& id,
                                     SumoXMLNodeType type,
                                     const Position& position,
                                     const PositionVector& shape,
                                     std::vector<MSLane*> incoming, std::vector<MSLane*> internal):
    MSJunction(id, type, position, shape),
    myIncomingLanes(incoming),
    myInternalLanes(internal) {
}


MSNoLogicJunction::~MSNoLogicJunction() {}


void
MSNoLogicJunction::postloadInit() {
    std::vector<MSLane*>::iterator i;
    // inform links where they have to report approaching vehicles to
    for (i = myIncomingLanes.begin(); i != myIncomingLanes.end(); ++i) {
        const MSLinkCont& links = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j = links.begin(); j != links.end(); j++) {
            (*j)->setRequestInformation(-1, false, false, std::vector<MSLink*>(), std::vector<MSLane*>());
        }
    }
}


const std::vector<MSLane*>
MSNoLogicJunction::getInternalLanes() const {
    // Besides the lanes im myInternal lanes, which are only the last parts of the connections,
    // this collects all lanes on the junction
    std::vector<MSLane*> allInternalLanes;
    for (std::vector<MSLane*>::const_iterator i = myInternalLanes.begin(); i != myInternalLanes.end(); ++i) {
        MSLane* l = *i;
        while (l != nullptr) {
            allInternalLanes.push_back(l);
            const std::vector<MSLane::IncomingLaneInfo> incoming = l->getIncomingLanes();
            if (incoming.size() == 0) {
                break;
            }
            assert(l->getIncomingLanes().size() == 1);
            l = l->getIncomingLanes()[0].lane;
            if (!l->isInternal()) {
                break;
            }
        }
    }
    return allInternalLanes;
}



/****************************************************************************/

