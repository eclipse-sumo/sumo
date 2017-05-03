/****************************************************************************/
/// @file    TraCI_Route.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRoute.h>
#include "TraCI_Route.h"


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_Route::getIDList() {
    std::vector<std::string> ids;
    MSRoute::insertIDs(ids);
    return ids;
}

std::vector<std::string>
TraCI_Route::getEdges(const std::string& routeID) {
    const MSRoute* r = getRoute(routeID);
    std::vector<std::string> ids;
    for (ConstMSEdgeVector::const_iterator i = r->getEdges().begin(); i != r->getEdges().end(); ++i) {
        ids.push_back((*i)->getID());
    }
    return ids;
}


int
TraCI_Route::getIDCount() {
    return (int)getIDList().size();
}


std::string
TraCI_Route::getParameter(const std::string& routeID, const std::string& param) {
    const MSRoute* r = getRoute(routeID);
    return r->getParameter(param, "");
}

void
TraCI_Route::setParameter(const std::string& routeID, const std::string& key, const std::string& value) {
    MSRoute* r = const_cast<MSRoute*>(getRoute(routeID));
    r->addParameter(key, value);
}


void
TraCI_Route::add(const std::string& routeID, const std::vector<std::string>& edgeIDs) {
    ConstMSEdgeVector edges;
    for (std::vector<std::string>::const_iterator ei = edgeIDs.begin(); ei != edgeIDs.end(); ++ei) {
        MSEdge* edge = MSEdge::dictionary(*ei);
        if (edge == 0) {
            throw TraCIException("Unknown edge '" + *ei + "' in route.");
        }
        edges.push_back(edge);
    }
    const std::vector<SUMOVehicleParameter::Stop> stops;
    if (!MSRoute::dictionary(routeID, new MSRoute(routeID, edges, true, 0, stops))) {
        throw TraCIException("Could not add route.");
    }
}


const MSRoute*
TraCI_Route::getRoute(const std::string& id) {
    const MSRoute* r = MSRoute::dictionary(id);
    if (r == 0) {
        throw TraCIException("Route '" + id + "' is not known");
    }
    return r;
}


/****************************************************************************/
