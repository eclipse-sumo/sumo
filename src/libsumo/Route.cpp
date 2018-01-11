/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Route.cpp
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
#include "Route.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string>
Route::getIDList() {
    std::vector<std::string> ids;
    MSRoute::insertIDs(ids);
    return ids;
}

std::vector<std::string>
Route::getEdges(const std::string& routeID) {
    const MSRoute* r = getRoute(routeID);
    std::vector<std::string> ids;
    for (ConstMSEdgeVector::const_iterator i = r->getEdges().begin(); i != r->getEdges().end(); ++i) {
        ids.push_back((*i)->getID());
    }
    return ids;
}


int
Route::getIDCount() {
    return (int)getIDList().size();
}


std::string
Route::getParameter(const std::string& routeID, const std::string& param) {
    const MSRoute* r = getRoute(routeID);
    return r->getParameter(param, "");
}

void
Route::setParameter(const std::string& routeID, const std::string& key, const std::string& value) {
    MSRoute* r = const_cast<MSRoute*>(getRoute(routeID));
    r->setParameter(key, value);
}


void
Route::add(const std::string& routeID, const std::vector<std::string>& edgeIDs) {
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
Route::getRoute(const std::string& id) {
    const MSRoute* r = MSRoute::dictionary(id);
    if (r == 0) {
        throw TraCIException("Route '" + id + "' is not known");
    }
    return r;
}
}


/****************************************************************************/
