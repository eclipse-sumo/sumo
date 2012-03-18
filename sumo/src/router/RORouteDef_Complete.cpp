/****************************************************************************/
/// @file    RORouteDef_Complete.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A complete route definition (with all passed edges being known)
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
#include <deque>
#include <iterator>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include "RORouteDef_Complete.h"
#include "ROHelper.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef_Complete::RORouteDef_Complete(const std::string& id,
        const RGBColor* const color,
        const std::vector<const ROEdge*> &edges,
        bool tryRepair)
    : RORouteDef(id, color), myTryRepair(tryRepair) {
    myAlternatives.push_back(new RORoute(id, 0, 1, edges, copyColorIfGiven()));
}


RORouteDef_Complete::~RORouteDef_Complete() {
}


void
RORouteDef_Complete::preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                       SUMOTime begin, const ROVehicle& veh) const {
    std::vector<const ROEdge*> newEdges;
    if (myTryRepair) {
        const std::vector<const ROEdge*> &oldEdges = myAlternatives[0]->getEdgeVector();
        if (oldEdges.size() == 0) {
            MsgHandler* m = OptionsCont::getOptions().getBool("ignore-errors") ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance();
            m->inform("Could not repair empty route of vehicle '" + veh.getID() + "'.");
            myPrecomputed =  new RORoute(myID, 0, 1, std::vector<const ROEdge*>(), copyColorIfGiven());
            return;
        }
        newEdges.push_back(*(oldEdges.begin()));
        for (std::vector<const ROEdge*>::const_iterator i = oldEdges.begin() + 1; i != oldEdges.end(); ++i) {
            if ((*(i - 1))->isConnectedTo(*i)) {
                newEdges.push_back(*i);
            } else {
                std::vector<const ROEdge*> edges;
                router.compute(*(i - 1), *i, &veh, begin, edges);
                if (edges.size() == 0) {
                    return;
                }
                std::copy(edges.begin() + 1, edges.end(), back_inserter(newEdges));
            }
        }
        if (myAlternatives[0]->getEdgeVector() != newEdges) {
            WRITE_WARNING("Repaired route of vehicle '" + veh.getID() + "'.");
        }
    } else {
        newEdges = myAlternatives[0]->getEdgeVector();
    }
    SUMOReal costs = router.recomputeCosts(newEdges, &veh, begin);
    if (costs < 0) {
        throw ProcessError("Route '" + getID() + "' (vehicle '" + veh.getID() + "') is not valid.");
    }
    myPrecomputed = new RORoute(myID, costs, 1, newEdges, copyColorIfGiven());
}


void
RORouteDef_Complete::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                    const ROVehicle* const veh, RORoute* current, SUMOTime begin) {
    delete myAlternatives[0];
    myAlternatives.pop_back();
    current->setCosts(router.recomputeCosts(current->getEdgeVector(), veh, begin));
    myAlternatives.push_back(current);
}


/****************************************************************************/
