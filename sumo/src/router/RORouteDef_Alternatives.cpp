/****************************************************************************/
/// @file    RORouteDef_Alternatives.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A route with alternative routes
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

#include <utils/common/RandHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include "ROHelper.h"
#include "ROEdge.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "ROCostCalculator.h"
#include "RORouteDef_Alternatives.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef_Alternatives::RORouteDef_Alternatives(const std::string& id,
        unsigned int lastUsed, 
        const int maxRoutes, const bool keepRoutes, const bool skipRouteCalculation)
    : RORouteDef(id, 0), myLastUsed((int) lastUsed), myNewRoute(false),
      myMaxRouteNumber(maxRoutes), myKeepRoutes(keepRoutes),
      mySkipRouteCalculation(skipRouteCalculation) {
}


RORouteDef_Alternatives::~RORouteDef_Alternatives() {
    for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        delete *i;
    }
}


void
RORouteDef_Alternatives::addLoadedAlternative(RORoute* alt) {
    myAlternatives.push_back(alt);
}


void
RORouteDef_Alternatives::preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
        SUMOTime begin, const ROVehicle& veh) const {
    myNewRoute = false;
    if (mySkipRouteCalculation) {
        myPrecomputed = myAlternatives[myLastUsed];
    } else {
        // build a new route to test whether it is better
        std::vector<const ROEdge*> edges;
        router.compute(myAlternatives[0]->getFirst(), myAlternatives[0]->getLast(), &veh, begin, edges);
        // check whether the same route was already used
        int cheapest = findRoute(edges);
        if (cheapest >= 0) {
            myPrecomputed = myAlternatives[cheapest];
        } else {
            myPrecomputed = new RORoute(myID, 0, 1, edges, copyColorIfGiven());
            myNewRoute = true;
        }
    }
}


int
RORouteDef_Alternatives::findRoute(const std::vector<const ROEdge*>& edges) const {
    for (unsigned int i = 0; i < myAlternatives.size(); i++) {
        if (edges == myAlternatives[i]->getEdgeVector()) {
            return (int) i;
        }
    }
    return -1;
}


void
RORouteDef_Alternatives::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                        const ROVehicle* const veh, RORoute* current, SUMOTime begin) {
    // add the route when it's new
    if (myNewRoute) {
        myAlternatives.push_back(current);
    }
    // recompute the costs and (when a new route was added) scale the probabilities
    const SUMOReal scale = SUMOReal(myAlternatives.size() - 1) / SUMOReal(myAlternatives.size());
    for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
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
        ROCostCalculator::getCalculator().setCosts(alt, newCosts, *i == myAlternatives[myLastUsed]);
    }
    assert(myAlternatives.size() != 0);
    ROCostCalculator::getCalculator().calculateProbabilities(veh, myAlternatives);
    if (!myKeepRoutes) {
        // remove with probability of 0 (not mentioned in Gawron)
        for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end();) {
            if ((*i)->getProbability() == 0) {
                delete *i;
                i = myAlternatives.erase(i);
            } else {
                i++;
            }
        }
    }
    if (myAlternatives.size() > (unsigned)myMaxRouteNumber) {
        // only keep the routes with highest probability
        sort(myAlternatives.begin(), myAlternatives.end(), ComparatorProbability());
        for (AlternativesVector::iterator i = myAlternatives.begin() + myMaxRouteNumber; i != myAlternatives.end(); i++) {
            delete *i;
        }
        myAlternatives.erase(myAlternatives.begin() + myMaxRouteNumber, myAlternatives.end());
        // rescale probabilities
        SUMOReal newSum = 0;
        for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
            newSum += (*i)->getProbability();
        }
        assert(newSum > 0);
        // @note newSum may be larger than 1 for numerical reasons
        for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
            (*i)->setProbability((*i)->getProbability() / newSum);
        }
    }

    // find the route to use
    SUMOReal chosen = RandHelper::rand();
    int pos = 0;
    for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end() - 1; i++, pos++) {
        chosen -= (*i)->getProbability();
        if (chosen <= 0) {
            myLastUsed = pos;
            return;
        }
    }
    myLastUsed = pos;
}


OutputDevice&
RORouteDef_Alternatives::writeXMLDefinition(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
        OutputDevice& dev, const ROVehicle* const veh,
        bool asAlternatives, bool withExitTimes) const {
    // (optional) alternatives header
    if (asAlternatives) {
        dev.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, myLastUsed).closeOpener();
        for (size_t i = 0; i != myAlternatives.size(); i++) {
            const RORoute& alt = *(myAlternatives[i]);
            dev.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_COST, alt.getCosts());
            dev.setPrecision(8);
            dev.writeAttr(SUMO_ATTR_PROB, alt.getProbability());
            dev.setPrecision();
            if (alt.getColor() != 0) {
                dev.writeAttr(SUMO_ATTR_COLOR, *alt.getColor());
            } else if (myColor != 0) {
                dev.writeAttr(SUMO_ATTR_COLOR, *myColor);
            }
            dev.writeAttr(SUMO_ATTR_EDGES, alt.getEdgeVector());
            if (withExitTimes) {
                std::string exitTimes;
                SUMOReal time = STEPS2TIME(veh->getDepartureTime());
                for (std::vector<const ROEdge*>::const_iterator i = alt.getEdgeVector().begin(); i != alt.getEdgeVector().end(); ++i) {
                    if (i != alt.getEdgeVector().begin()) {
                        exitTimes += " ";
                    }
                    time += (*i)->getTravelTime(veh, time);
                    exitTimes += toString(time);
                }
                dev.writeAttr("exitTimes", exitTimes);
            }
            dev.closeTag(true);
        }
        dev.closeTag();
        return dev;
    } else {
        return myAlternatives[myLastUsed]->writeXMLDefinition(router, dev, veh, asAlternatives, withExitTimes);
    }
}


/****************************************************************************/
