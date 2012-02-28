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
    : RORouteDef(id, 0), myLastUsed((int) lastUsed),
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
    if (mySkipRouteCalculation) {
        myLastUsed = 0;
        myNewRoute = false;
        myPrecomputed = myAlternatives[myLastUsed];
        return;
    }
    // recompute duration of the last route used
    // build a new route to test whether it is better
    std::vector<const ROEdge*> edges;
    router.compute(myAlternatives[0]->getFirst(), myAlternatives[0]->getLast(), &veh, begin, edges);
    RORoute* opt = new RORoute(myID, 0, 1, edges, copyColorIfGiven());
    const SUMOReal costs = router.recomputeCosts(edges, &veh, begin);
    // check whether the same route was already used
    myLastUsed = findRoute(opt);
    myNewRoute = true;
    // delete the route when it already existed
    if (myLastUsed >= 0) {
        delete opt;
        myNewRoute = false;
        myAlternatives[myLastUsed]->setCosts(costs);
        myPrecomputed = myAlternatives[myLastUsed];
        return;
    }
    // return the built route
    ROCostCalculator::getCalculator().setCosts(opt, costs, true);
    myPrecomputed = opt;
}


int
RORouteDef_Alternatives::findRoute(RORoute* opt) const {
    for (unsigned int i = 0; i < myAlternatives.size(); i++) {
        if (opt->getEdgeVector() == myAlternatives[i]->getEdgeVector()) {
            return (int) i;
        }
    }
    return -1;
}


void
RORouteDef_Alternatives::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                                        const ROVehicle* const veh, RORoute* current, SUMOTime begin) {
    // add the route when it's new
    if (myLastUsed < 0) {
        myAlternatives.push_back(current);
        myLastUsed = (int) myAlternatives.size() - 1;
    }
    // recompute the costs and (when a new route was added) the probabilities
    for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        RORoute* alt = *i;
        // apply changes for old routes only
        //  (the costs for the current were computed already)
        if ((*i) != current || !myNewRoute) {
            // recompute the costs for old routes
            const SUMOReal newCosts = router.recomputeCosts(alt->getEdgeVector(), veh, begin);
            if (newCosts < 0.) {
                throw ProcessError("Route '" + current->getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
            }
            ROCostCalculator::getCalculator().setCosts(alt, newCosts);
        }
        assert(myAlternatives.size() != 0);
        if (myNewRoute) {
            if ((*i) != current) {
                alt->setProbability(
                    alt->getProbability()
                    * SUMOReal(myAlternatives.size() - 1)
                    / SUMOReal(myAlternatives.size()));
            } else {
                alt->setProbability((SUMOReal)(1.0 / (SUMOReal) myAlternatives.size()));
            }
        }
    }
    assert(myAlternatives.size() != 0);
    ROCostCalculator::getCalculator().calculateProbabilities(veh, myAlternatives);
    if (!myKeepRoutes) {
        // remove with probability of 0 (not mentioned in Gawron)
        for (AlternativesVector::iterator i = myAlternatives.begin(); i != myAlternatives.end();) {
            if ((*i)->getProbability() == 0) {
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
        chosen = chosen - (*i)->getProbability();
        if (chosen <= 0) {
            myLastUsed = pos;
            return;
        }
    }
    myLastUsed = pos;
}


RORouteDef*
RORouteDef_Alternatives::copy(const std::string& id) const {
    RORouteDef_Alternatives* ret = new RORouteDef_Alternatives(id,
            myLastUsed, myMaxRouteNumber, myKeepRoutes, mySkipRouteCalculation);
    for (std::vector<RORoute*>::const_iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        ret->addLoadedAlternative(new RORoute(*(*i)));
    }
    return ret;
}


void
RORouteDef_Alternatives::invalidateLast() {
    myLastUsed = -1;
}


void
RORouteDef_Alternatives::removeLast() {
    assert(myAlternatives.size() >= 2);
    myAlternatives.erase(myAlternatives.end() - 1);
    myLastUsed = (int) myAlternatives.size() - 1;
    // !!! recompute probabilities
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


const ROEdge*
RORouteDef_Alternatives::getDestination() const {
    return myAlternatives[0]->getLast();
}



/****************************************************************************/
