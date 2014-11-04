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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <router/RONet.h>
#include "ROJTRRouter.h"
#include "ROJTREdge.h"
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROJTRRouter::ROJTRRouter(bool unbuildIsWarningOnly, bool acceptAllDestinations,
                         int maxEdges, bool ignoreClasses, bool allowLoops) :
    SUMOAbstractRouter<ROEdge, ROVehicle>(0, "JTRRouter"),
    myUnbuildIsWarningOnly(unbuildIsWarningOnly),
    myAcceptAllDestination(acceptAllDestinations), myMaxEdges(maxEdges),
    myIgnoreClasses(ignoreClasses), myAllowLoops(allowLoops)
{ }


ROJTRRouter::~ROJTRRouter() {}


void
ROJTRRouter::compute(const ROEdge* from, const ROEdge* to,
                     const ROVehicle* const vehicle,
                     SUMOTime time, std::vector<const ROEdge*>& into) {
    const ROJTREdge* current = static_cast<const ROJTREdge*>(from);
    SUMOReal timeS = STEPS2TIME(time);
    std::set<const ROEdge*> avoidEdges;
    // route until a sinks has been found
    while (current != 0 && current != to &&
            current->getType() != ROEdge::ET_SINK &&
            (int)into.size() < myMaxEdges) {
        into.push_back(current);
        if (!myAllowLoops) {
            avoidEdges.insert(current);
        }
        timeS += current->getTravelTime(vehicle, timeS);
        current = current->chooseNext(myIgnoreClasses ? 0 : vehicle, timeS, avoidEdges);
        assert(myIgnoreClasses || current == 0 || !current->prohibits(vehicle));
    }
    // check whether no valid ending edge was found
    if (current == 0 || (int) into.size() >= myMaxEdges) {
        if (myAcceptAllDestination) {
            return;
        } else {
            MsgHandler* mh = myUnbuildIsWarningOnly ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance();
            mh->inform("The route starting at edge '" + from->getID() + "' could not be closed.");
        }
    }
    // append the sink
    if (current != 0) {
        into.push_back(current);
    }
}


SUMOReal
ROJTRRouter::recomputeCosts(const std::vector<const ROEdge*>& edges, const ROVehicle* const v, SUMOTime time) const {
    SUMOReal costs = 0;
    for (std::vector<const ROEdge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        costs += (*i)->getTravelTime(v, time);
    }
    return costs;
}



/****************************************************************************/

