/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    Route.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRoute.h>
#include <libsumo/TraCIConstants.h>
#include "Helper.h"
#include "Route.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Route::mySubscriptionResults;
ContextSubscriptionResults Route::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Route::getIDList() {
    std::vector<std::string> ids;
    MSRoute::insertIDs(ids);
    return ids;
}

std::vector<std::string>
Route::getEdges(const std::string& routeID) {
    ConstMSRoutePtr r = getRoute(routeID);
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
    ConstMSRoutePtr r = getRoute(routeID);
    return r->getParameter(param, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(Route)


void
Route::setParameter(const std::string& routeID, const std::string& key, const std::string& value) {
    MSRoute* r = const_cast<MSRoute*>(getRoute(routeID).get());
    r->setParameter(key, value);
}


void
Route::add(const std::string& routeID, const std::vector<std::string>& edgeIDs) {
    ConstMSEdgeVector edges;
    if (edgeIDs.size() == 0) {
        throw TraCIException("Cannot add route '" + routeID + "' without edges.");
    }
    for (std::vector<std::string>::const_iterator ei = edgeIDs.begin(); ei != edgeIDs.end(); ++ei) {
        MSEdge* edge = MSEdge::dictionary(*ei);
        if (edge == nullptr) {
            throw TraCIException("Unknown edge '" + *ei + "' in route.");
        }
        edges.push_back(edge);
    }
    const std::vector<SUMOVehicleParameter::Stop> stops;
    ConstMSRoutePtr route = std::make_shared<MSRoute>(routeID, edges, true, nullptr, stops);
    if (!MSRoute::dictionary(routeID, route)) {
        throw TraCIException("Could not add route '" + routeID + "'.");
    }
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(Route, ROUTE)


ConstMSRoutePtr
Route::getRoute(const std::string& id) {
    ConstMSRoutePtr r = MSRoute::dictionary(id);
    if (r == nullptr) {
        throw TraCIException("Route '" + id + "' is not known");
    }
    return r;
}


std::shared_ptr<VariableWrapper>
Route::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Route::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_EDGES:
            return wrapper->wrapStringList(objID, variable, getEdges(objID));
        case libsumo::VAR_PARAMETER:
            paramData->readUnsignedByte();
            return wrapper->wrapString(objID, variable, getParameter(objID, paramData->readString()));
        case libsumo::VAR_PARAMETER_WITH_KEY:
            paramData->readUnsignedByte();
            return wrapper->wrapStringPair(objID, variable, getParameterWithKey(objID, paramData->readString()));
        default:
            return false;
    }
}
}


/****************************************************************************/
