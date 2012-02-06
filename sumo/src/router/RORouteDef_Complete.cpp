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
    : RORouteDef(id, color), myEdges(edges), myTryRepair(tryRepair) {
}


RORouteDef_Complete::~RORouteDef_Complete() {}


void
RORouteDef_Complete::preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                       SUMOTime begin, const ROVehicle& veh) const {
    if (myTryRepair) {
        const std::vector<const ROEdge*> &oldEdges = myEdges;
        if (oldEdges.size() == 0) {
            MsgHandler* m = OptionsCont::getOptions().getBool("ignore-errors") ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance();
            m->inform("Could not repair empty route of vehicle '" + veh.getID() + "'.");
            myPrecomputed =  new RORoute(myID, 0, 1, std::vector<const ROEdge*>(), copyColorIfGiven());
            return;
        }
        std::vector<const ROEdge*> newEdges;
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
        if (myEdges != newEdges) {
            WRITE_WARNING("Repaired route of vehicle '" + veh.getID() + "'.");
        }
        myEdges = newEdges;
    }
    SUMOReal costs = router.recomputeCosts(myEdges, &veh, begin);
    if (costs < 0) {
        throw ProcessError("Route '" + getID() + "' (vehicle '" + veh.getID() + "') is not valid.");
    }
    myPrecomputed = new RORoute(myID, 0, 1, myEdges, copyColorIfGiven());
}


void
RORouteDef_Complete::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle> &,
                                    const ROVehicle* const, RORoute* current, SUMOTime begin) {
    myStartTime = begin;
    myEdges = current->getEdgeVector();
    delete current;
}


RORouteDef*
RORouteDef_Complete::copy(const std::string& id) const {
    return new RORouteDef_Complete(id, copyColorIfGiven(), myEdges, myTryRepair);
}


OutputDevice&
RORouteDef_Complete::writeXMLDefinition(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                        OutputDevice& dev, const ROVehicle* const veh,
                                        bool asAlternatives, bool withExitTimes) const {
    // (optional) alternatives header
    if (asAlternatives) {
        dev.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, 0).closeOpener();
    }
    // the route
    dev.openTag(SUMO_TAG_ROUTE);
    if (asAlternatives) {
        dev.writeAttr(SUMO_ATTR_COST, router.recomputeCosts(myEdges, veh, veh->getDepartureTime()));
        dev.writeAttr(SUMO_ATTR_PROB, 1.);
    }
    if (myColor != 0) {
        dev.writeAttr(SUMO_ATTR_COLOR, *myColor);
    }
    dev.writeAttr(SUMO_ATTR_EDGES, myEdges);
    if (withExitTimes) {
        std::string exitTimes;
        SUMOReal time = STEPS2TIME(veh->getDepartureTime());
        for (std::vector<const ROEdge*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
            if (i != myEdges.begin()) {
                exitTimes += " ";
            }
            time += (*i)->getTravelTime(veh, time);
            exitTimes += toString(time);
        }
        dev.writeAttr("exitTimes", exitTimes);
    }
    dev.closeTag(true);
    // (optional) alternatives end
    if (asAlternatives) {
        dev.closeTag();
    }
    return dev;
}


const ROEdge*
RORouteDef_Complete::getDestination() const {
    return myEdges.back();
}


/****************************************************************************/

