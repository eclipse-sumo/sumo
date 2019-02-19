/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROJTRRouter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Computes routes using junction turning percentages
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <router/RONet.h>
#include "ROJTRRouter.h"
#include "ROJTREdge.h"
#include <utils/common/MsgHandler.h>


// ===========================================================================
// method definitions
// ===========================================================================
ROJTRRouter::ROJTRRouter(bool unbuildIsWarningOnly, bool acceptAllDestinations,
                         int maxEdges, bool ignoreClasses, bool allowLoops) :
    SUMOAbstractRouter<ROEdge, ROVehicle>("JTRRouter", unbuildIsWarningOnly, &ROEdge::getTravelTimeStatic),
    myUnbuildIsWarningOnly(unbuildIsWarningOnly),
    myAcceptAllDestination(acceptAllDestinations), myMaxEdges(maxEdges),
    myIgnoreClasses(ignoreClasses), myAllowLoops(allowLoops) {
}


ROJTRRouter::~ROJTRRouter() {}


bool
ROJTRRouter::compute(const ROEdge* from, const ROEdge* to,
                     const ROVehicle* const vehicle,
                     SUMOTime time, ConstROEdgeVector& into, bool silent) {
    const ROJTREdge* current = static_cast<const ROJTREdge*>(from);
    double timeS = STEPS2TIME(time);
    std::set<const ROEdge*> avoidEdges;
    // route until a sinks has been found
    while (current != nullptr && current != to &&
            !current->isSink() &&
            (int)into.size() < myMaxEdges) {
        into.push_back(current);
        if (!myAllowLoops) {
            avoidEdges.insert(current);
        }
        timeS += current->getTravelTime(vehicle, timeS);
        current = current->chooseNext(myIgnoreClasses ? nullptr : vehicle, timeS, avoidEdges);
        assert(myIgnoreClasses || current == 0 || !current->prohibits(vehicle));
    }
    // check whether no valid ending edge was found
    if (current == nullptr || (int) into.size() >= myMaxEdges) {
        if (myAcceptAllDestination) {
            return true;
        } else {
            if (!silent) {
                MsgHandler* mh = myUnbuildIsWarningOnly ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance();
                mh->inform("The route starting at edge '" + from->getID() + "' could not be closed.");
            }
            return false;
        }
    }
    // append the sink
    if (current != nullptr) {
        into.push_back(current);
    }
    return true;
}


/****************************************************************************/

