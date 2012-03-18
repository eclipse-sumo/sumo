/****************************************************************************/
/// @file    RORouteDef_OrigDest.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A route where only the origin and the destination edges are known
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <iostream>
#include <cassert>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "RORouteDef_OrigDest.h"
#include <utils/common/SUMOAbstractRouter.h>
#include "ROVehicle.h"
#include "ROHelper.h"
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
RORouteDef_OrigDest::RORouteDef_OrigDest(const std::string& id,
        const RGBColor* const color,
        const ROEdge* from,
        const ROEdge* to,
        bool removeFirst)
    : RORouteDef(id, color) {
    std::vector<const ROEdge*> edges;
    edges.push_back(from);
    edges.push_back(to);
    myAlternatives.push_back(new RORoute(id, 0, 1, edges, copyColorIfGiven()));
}


RORouteDef_OrigDest::~RORouteDef_OrigDest() {
}


void
RORouteDef_OrigDest::preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                       SUMOTime begin, const ROVehicle& veh) const {
    std::vector<const ROEdge*> edges;
    router.compute(myAlternatives[0]->getFirst(), myAlternatives[0]->getLast(), &veh, begin, edges);
    myPrecomputed = new RORoute(myID, 0, 1, edges, copyColorIfGiven());
}


void
RORouteDef_OrigDest::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                    const ROVehicle* const veh, RORoute* current, SUMOTime begin) {
    delete myAlternatives[0];
    myAlternatives.pop_back();
    current->setCosts(router.recomputeCosts(current->getEdgeVector(), veh, begin));
    myAlternatives.push_back(current);
}


RORouteDef*
RORouteDef_OrigDest::copy(const std::string& id) const {
    return new RORouteDef_OrigDest(id, copyColorIfGiven(),
        myAlternatives[0]->getFirst(), myAlternatives[0]->getLast(), false);
}


/****************************************************************************/
