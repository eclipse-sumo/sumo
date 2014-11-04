/****************************************************************************/
/// @file    RORouteDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Base class for a vehicle's route definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
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
#include <iterator>
#include <algorithm>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include "ROEdge.h"
#include "RORoute.h"
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/vehicle/RouteCostCalculator.h>
#include "RORouteDef.h"
#include "ROVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
bool RORouteDef::myUsingJTRR(false);

// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef::RORouteDef(const std::string& id, const unsigned int lastUsed,
                       const bool tryRepair, const bool mayBeDisconnected) :
    Named(StringUtils::convertUmlaute(id)),
    myPrecomputed(0), myLastUsed(lastUsed), myTryRepair(tryRepair), myMayBeDisconnected(mayBeDisconnected)
{}


RORouteDef::~RORouteDef() {
    for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        if (myRouteRefs.count(*i) == 0) {
            delete *i;
        }
    }
}


void
RORouteDef::addLoadedAlternative(RORoute* alt) {
    myAlternatives.push_back(alt);
}


void
RORouteDef::addAlternativeDef(const RORouteDef* alt) {
    std::copy(alt->myAlternatives.begin(), alt->myAlternatives.end(),
              back_inserter(myAlternatives));
    std::copy(alt->myAlternatives.begin(), alt->myAlternatives.end(),
              std::inserter(myRouteRefs, myRouteRefs.end()));
}


RORoute*
RORouteDef::buildCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                              SUMOTime begin, const ROVehicle& veh) const {
    if (myPrecomputed == 0) {
        preComputeCurrentRoute(router, begin, veh);
    }
    return myPrecomputed;
}


void
RORouteDef::preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                                   SUMOTime begin, const ROVehicle& veh) const {
    myNewRoute = false;
    assert(myAlternatives[0]->getEdgeVector().size() > 0);
    MsgHandler* mh = (OptionsCont::getOptions().getBool("ignore-errors") ?
                      MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance());
    if (myAlternatives[0]->getFirst()->prohibits(&veh)) {
        /// XXX check for specified arrivalLane / departLane
        mh->inform("Vehicle '" + veh.getID() + "' is not allowed to depart on edge '" +
                   myAlternatives[0]->getFirst()->getID() + "'.");
        return;
    } else if (myAlternatives[0]->getLast()->prohibits(&veh)) {
        // this check is not strictly necessary unless myTryRepair is set.
        // However, the error message is more helpful than "no connection found"
        mh->inform("Vehicle '" + veh.getID() + "' is not allowed to arrive on edge '" +
                   myAlternatives[0]->getLast()->getID() + "'.");
        return;
    }
    if (myTryRepair) {
        repairCurrentRoute(router, begin, veh);
        return;
    }
    if (RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().skipRouteCalculation()
            || OptionsCont::getOptions().getBool("remove-loops")) {
        myPrecomputed = myAlternatives[myLastUsed];
    } else {
        // build a new route to test whether it is better
        std::vector<const ROEdge*> edges;
        router.compute(myAlternatives[0]->getFirst(), myAlternatives[0]->getLast(), &veh, begin, edges);
        // check whether the same route was already used
        int cheapest = -1;
        for (unsigned int i = 0; i < myAlternatives.size(); i++) {
            if (edges == myAlternatives[i]->getEdgeVector()) {
                cheapest = i;
                break;
            }
        }
        if (cheapest >= 0) {
            myPrecomputed = myAlternatives[cheapest];
        } else {
            RGBColor* col = myAlternatives[0]->getColor() != 0 ? new RGBColor(*myAlternatives[0]->getColor()) : 0;
            myPrecomputed = new RORoute(myID, 0, 1, edges, col, myAlternatives[0]->getStops());
            myNewRoute = true;
        }
    }
}


void
RORouteDef::repairCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                               SUMOTime begin, const ROVehicle& veh) const {
    MsgHandler* mh = (OptionsCont::getOptions().getBool("ignore-errors") ?
                      MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance());
    std::vector<const ROEdge*> oldEdges = myAlternatives[0]->getEdgeVector();
    std::vector<const ROEdge*> newEdges;
    std::vector<const ROEdge*> mandatory;
    if (oldEdges.size() == 1) {
        if (myUsingJTRR) {
            /// only ROJTRRouter is supposed to handle this type of input
            router.compute(oldEdges.front(), 0, &veh, begin, newEdges);
        } else {
            newEdges = oldEdges;
        }
    } else {
        // prepare mandatory edges
        mandatory.push_back(oldEdges.front());
        std::vector<const ROEdge*> stops = veh.getStopEdges();
        for (std::vector<const ROEdge*>::const_iterator i = stops.begin(); i != stops.end(); ++i) {
            if (*i != mandatory.back()) {
                mandatory.push_back(*i);
            }
        }
        if (mandatory.size() < 2 || oldEdges.back() != mandatory.back()) {
            mandatory.push_back(oldEdges.back());
        }
        assert(mandatory.size() >= 2);
        // removed prohibited
        for (std::vector<const ROEdge*>::iterator i = oldEdges.begin(); i != oldEdges.end();) {
            if ((*i)->prohibits(&veh)) {
                if (std::find(mandatory.begin(), mandatory.end(), *i) != mandatory.end()) {
                    mh->inform("Mandatory edge '" + (*i)->getID() + "' does not allow vehicle '" + veh.getID() + "'.");
                    return;
                }
                i = oldEdges.erase(i);
            } else {
                ++i;
            }
        }
        // reconnect remaining edges
        newEdges.push_back(*(oldEdges.begin()));
        std::vector<const ROEdge*>::iterator nextMandatory = mandatory.begin() + 1;
        size_t lastMandatory = 0;
        for (std::vector<const ROEdge*>::iterator i = oldEdges.begin() + 1;
                i != oldEdges.end() && nextMandatory != mandatory.end(); ++i) {
            if ((*(i - 1))->isConnectedTo(*i)) {
                /// XXX could be connected from a prohibited lane only
                newEdges.push_back(*i);
            } else {
                std::vector<const ROEdge*> edges;
                router.compute(newEdges.back(), *i, &veh, begin, edges);
                if (edges.size() == 0) {
                    // backtrack: try to route from last mandatory edge to next mandatory edge
                    // XXX add option for backtracking in smaller increments
                    // (i.e. previous edge to edge after *i)
                    // we would then need to decide whether we have found a good
                    // tradeoff between faithfulness to the input data and detour-length
                    router.compute(newEdges[lastMandatory], *nextMandatory, &veh, begin, edges);
                    if (edges.size() == 0) {
                        mh->inform("Mandatory edge '" + (*i)->getID() + "' not reachable by vehicle '" + veh.getID() + "'.");
                        return;
                    } else {
                        while (*i != *nextMandatory) {
                            ++i;
                        }
                        newEdges.erase(newEdges.begin() + lastMandatory + 1, newEdges.end());
                    }
                }
                std::copy(edges.begin() + 1, edges.end(), back_inserter(newEdges));
            }
            if (*i == *nextMandatory) {
                nextMandatory++;
                lastMandatory = newEdges.size() - 1;
            }
        }
    }
    if (myAlternatives[0]->getEdgeVector() != newEdges) {
        if (!myMayBeDisconnected) {
            WRITE_WARNING("Repaired route of vehicle '" + veh.getID() + "'.");
        }
        myNewRoute = true;
        RGBColor* col = myAlternatives[0]->getColor() != 0 ? new RGBColor(*myAlternatives[0]->getColor()) : 0;
        myPrecomputed = new RORoute(myID, 0, myAlternatives[0]->getProbability(), newEdges, col, myAlternatives[0]->getStops());
    } else {
        myPrecomputed = myAlternatives[0];
    }
}


void
RORouteDef::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                           const ROVehicle* const veh, RORoute* current, SUMOTime begin) {
    if (myTryRepair) {
        if (myNewRoute) {
            delete myAlternatives[0];
            myAlternatives.pop_back();
            myAlternatives.push_back(current);
        }
        const SUMOReal costs = router.recomputeCosts(current->getEdgeVector(), veh, begin);
        if (costs < 0) {
            throw ProcessError("Route '" + getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
        }
        current->setCosts(costs);
        return;
    }
    // add the route when it's new
    if (myNewRoute) {
        myAlternatives.push_back(current);
    }
    // recompute the costs and (when a new route was added) scale the probabilities
    const SUMOReal scale = SUMOReal(myAlternatives.size() - 1) / SUMOReal(myAlternatives.size());
    for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        RORoute* alt = *i;
        // recompute the costs for all routes
        const SUMOReal newCosts = router.recomputeCosts(alt->getEdgeVector(), veh, begin);
        if (newCosts < 0.) {
            throw ProcessError("Route '" + current->getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
        }
        assert(myAlternatives.size() != 0);
        if (myNewRoute) {
            if (*i == current) {
                // set initial probability and costs
                alt->setProbability((SUMOReal)(1.0 / (SUMOReal) myAlternatives.size()));
                alt->setCosts(newCosts);
            } else {
                // rescale probs for all others
                alt->setProbability(alt->getProbability() * scale);
            }
        }
        RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().setCosts(alt, newCosts, *i == myAlternatives[myLastUsed]);
    }
    assert(myAlternatives.size() != 0);
    RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().calculateProbabilities(myAlternatives, veh, veh->getDepartureTime());
    if (!RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().keepRoutes()) {
        // remove with probability of 0 (not mentioned in Gawron)
        for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end();) {
            if ((*i)->getProbability() == 0) {
                delete *i;
                i = myAlternatives.erase(i);
            } else {
                i++;
            }
        }
    }
    if (myAlternatives.size() > RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().getMaxRouteNumber()) {
        // only keep the routes with highest probability
        sort(myAlternatives.begin(), myAlternatives.end(), ComparatorProbability());
        for (std::vector<RORoute*>::iterator i = myAlternatives.begin() + RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().getMaxRouteNumber(); i != myAlternatives.end(); i++) {
            delete *i;
        }
        myAlternatives.erase(myAlternatives.begin() + RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().getMaxRouteNumber(), myAlternatives.end());
        // rescale probabilities
        SUMOReal newSum = 0;
        for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
            newSum += (*i)->getProbability();
        }
        assert(newSum > 0);
        // @note newSum may be larger than 1 for numerical reasons
        for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
            (*i)->setProbability((*i)->getProbability() / newSum);
        }
    }

    // find the route to use
    SUMOReal chosen = RandHelper::rand();
    int pos = 0;
    for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end() - 1; i++, pos++) {
        chosen -= (*i)->getProbability();
        if (chosen <= 0) {
            myLastUsed = pos;
            return;
        }
    }
    myLastUsed = pos;
}


const ROEdge*
RORouteDef::getDestination() const {
    return myAlternatives[0]->getLast();
}


OutputDevice&
RORouteDef::writeXMLDefinition(OutputDevice& dev, const ROVehicle* const veh,
                               bool asAlternatives, bool withExitTimes) const {
    if (asAlternatives) {
        dev.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, myLastUsed);
        for (size_t i = 0; i != myAlternatives.size(); i++) {
            myAlternatives[i]->writeXMLDefinition(dev, veh, true, withExitTimes);
        }
        dev.closeTag();
        return dev;
    } else {
        return myAlternatives[myLastUsed]->writeXMLDefinition(dev, veh, false, withExitTimes);
    }
}


RORouteDef*
RORouteDef::copyOrigDest(const std::string& id) const {
    RORouteDef* result = new RORouteDef(id, 0, true, true);
    RORoute* route = myAlternatives[0];
    RGBColor* col = route->getColor() != 0 ? new RGBColor(*route->getColor()) : 0;
    std::vector<const ROEdge*> edges;
    edges.push_back(route->getFirst());
    edges.push_back(route->getLast());
    result->addLoadedAlternative(new RORoute(id, 0, 1, edges, col, route->getStops()));
    return result;
}


RORouteDef*
RORouteDef::copy(const std::string& id) const {
    RORouteDef* result = new RORouteDef(id, 0, myTryRepair, myMayBeDisconnected);
    for (std::vector<RORoute*>::const_iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        RORoute* route = *i;
        RGBColor* col = route->getColor() != 0 ? new RGBColor(*route->getColor()) : 0;
        result->addLoadedAlternative(new RORoute(id, 0, 1, route->getEdgeVector(), col, route->getStops()));
    }
    return result;
}


SUMOReal
RORouteDef::getOverallProb() const {
    SUMOReal sum = 0.;
    for (std::vector<RORoute*>::const_iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        sum += (*i)->getProbability();
    }
    return sum;
}


/****************************************************************************/
