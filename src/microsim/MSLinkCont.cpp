/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSLinkCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    15 Feb 2004
/// @version $Id$
///
// Helpers for link vector
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSLinkCont.h"
#include "MSLane.h"


// ===========================================================================
// method definitions
// ===========================================================================
const MSEdge*
MSLinkContHelper::getInternalFollowingEdge(const MSLane* fromLane,
        const MSEdge* followerAfterInternal) {
    //@todo to be optimized
    const MSLinkCont& lc = fromLane->getLinkCont();
    for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); j++) {
        MSLink* link = *j;
        if (&link->getLane()->getEdge() == followerAfterInternal) {
            if (link->getViaLane() != nullptr) {
                return &link->getViaLane()->getEdge();
            } else {
                return nullptr; // network without internal links
            }
        }
    }
    return nullptr;
}


const MSLane*
MSLinkContHelper::getInternalFollowingLane(const MSLane* fromLane,
        const MSLane* followerAfterInternal) {
    //@todo to be optimized
    const MSLinkCont& lc = fromLane->getLinkCont();
    for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); j++) {
        MSLink* link = *j;
        if (link->getLane() == followerAfterInternal) {
            if (link->getViaLane() != nullptr) {
                return link->getViaLane();
            } else {
                return nullptr; // network without internal links
            }
        }
    }
    return nullptr;
}


MSLink*
MSLinkContHelper::getConnectingLink(const MSLane& from, const MSLane& to) {
    const MSLinkCont& lc = from.getLinkCont();
    for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); j++) {
        MSLink* link = *j;
        if (link->getLane() == &to) {
            return link;
        } else if (link->getViaLaneOrLane() == &to) {
            return link;
        }
    }
    return nullptr;
}



/****************************************************************************/

