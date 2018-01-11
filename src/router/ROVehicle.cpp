/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <string>
#include <iostream>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORoute.h"
#include "ROHelper.h"
#include "RONet.h"


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicle::ROVehicle(const SUMOVehicleParameter& pars,
                     RORouteDef* route, const SUMOVTypeParameter* type,
                     const RONet* net, MsgHandler* errorHandler)
    : RORoutable(pars, type), myRoute(route) {
    getParameter().stops.clear();
    if (route != 0 && route->getFirstRoute() != 0) {
        for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = route->getFirstRoute()->getStops().begin(); s != route->getFirstRoute()->getStops().end(); ++s) {
            addStop(*s, net, errorHandler);
        }
    }
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin(); s != pars.stops.end(); ++s) {
        addStop(*s, net, errorHandler);
    }
    if (pars.via.size() != 0) {
        // via takes precedence over stop edges
        // XXX check for inconsistencies #2275
        myStopEdges.clear();
        for (std::vector<std::string>::const_iterator it = pars.via.begin(); it != pars.via.end(); ++it) {
            assert(net->getEdge(*it) != 0);
            myStopEdges.push_back(net->getEdge(*it));
        }
    }
}


void
ROVehicle::addStop(const SUMOVehicleParameter::Stop& stopPar, const RONet* net, MsgHandler* errorHandler) {
    const ROEdge* stopEdge = net->getEdgeForLaneID(stopPar.lane);
    assert(stopEdge != 0); // was checked when parsing the stop
    if (stopEdge->prohibits(this)) {
        if (errorHandler != 0) {
            errorHandler->inform("Stop edge '" + stopEdge->getID() + "' does not allow vehicle '" + getID() + "'.");
        }
        return;
    }
    // where to insert the stop
    std::vector<SUMOVehicleParameter::Stop>::iterator iter = getParameter().stops.begin();
    ConstROEdgeVector::iterator edgeIter = myStopEdges.begin();
    if (stopPar.index == STOP_INDEX_END || stopPar.index >= static_cast<int>(getParameter().stops.size())) {
        if (getParameter().stops.size() > 0) {
            iter = getParameter().stops.end();
            edgeIter = myStopEdges.end();
        }
    } else {
        if (stopPar.index == STOP_INDEX_FIT) {
            const ConstROEdgeVector edges = myRoute->getFirstRoute()->getEdgeVector();
            ConstROEdgeVector::const_iterator stopEdgeIt = std::find(edges.begin(), edges.end(), stopEdge);
            if (stopEdgeIt == edges.end()) {
                iter = getParameter().stops.end();
                edgeIter = myStopEdges.end();
            } else {
                while (iter != getParameter().stops.end()) {
                    if (edgeIter > stopEdgeIt || (edgeIter == stopEdgeIt && iter->endPos >= stopPar.endPos)) {
                        break;
                    }
                    ++iter;
                    ++edgeIter;
                }
            }
        } else {
            iter += stopPar.index;
            edgeIter += stopPar.index;
        }
    }
    getParameter().stops.insert(iter, stopPar);
    myStopEdges.insert(edgeIter, stopEdge);
}


ROVehicle::~ROVehicle() {}


const ROEdge*
ROVehicle:: getDepartEdge() const {
    return myRoute->getFirstRoute()->getFirst();
}


void
ROVehicle::computeRoute(const RORouterProvider& provider,
                        const bool removeLoops, MsgHandler* errorHandler) {
    SUMOAbstractRouter<ROEdge, ROVehicle>& router = provider.getVehicleRouter();
    std::string noRouteMsg = "The vehicle '" + getID() + "' has no valid route.";
    RORouteDef* const routeDef = getRouteDefinition();
    // check if the route definition is valid
    if (routeDef == 0) {
        errorHandler->inform(noRouteMsg);
        myRoutingSuccess = false;
        return;
    }
    RORoute* current = routeDef->buildCurrentRoute(router, getDepartureTime(), *this);
    if (current == 0 || current->size() == 0) {
        delete current;
        errorHandler->inform(noRouteMsg);
        myRoutingSuccess = false;
        return;
    }
    // check whether we have to evaluate the route for not containing loops
    if (removeLoops) {
        current->recheckForLoops();
        // check whether the route is still valid
        if (current->size() == 0) {
            delete current;
            errorHandler->inform(noRouteMsg + " (after removing loops)");
            myRoutingSuccess = false;
            return;
        }
    }
    // add built route
    routeDef->addAlternative(router, this, current, getDepartureTime());
    myRoutingSuccess = true;
}


void
ROVehicle::saveAsXML(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const {
    if (typeos != 0 && getType() != 0 && !getType()->saved) {
        getType()->write(*typeos);
        getType()->saved = true;
    }
    if (getType() != 0 && !getType()->saved) {
        getType()->write(os);
        getType()->saved = asAlternatives;
    }

    // write the vehicle (new style, with included routes)
    getParameter().write(os, options);

    // save the route
    myRoute->writeXMLDefinition(os, this, asAlternatives, options.getBool("exit-times"));
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator stop = getParameter().stops.begin(); stop != getParameter().stops.end(); ++stop) {
        stop->write(os);
    }
    getParameter().writeParams(os);
    os.closeTag();
}


/****************************************************************************/

