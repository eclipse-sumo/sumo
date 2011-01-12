/****************************************************************************/
/// @file    RORouteDef_Alternatives.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A route with alternative routes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include <cassert>
#include <limits>
#include <iostream>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "ROVehicle.h"
#include "ROHelper.h"
#include <utils/common/SUMOAbstractRouter.h>
#include "RORouteDef_Alternatives.h"
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/RandHelper.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef_Alternatives::RORouteDef_Alternatives(const std::string &id,
        unsigned int lastUsed,
        const SUMOReal beta, const SUMOReal gawronA, const SUMOReal logitGamma,
        const int maxRoutes, const bool keepRoutes) throw()
        : RORouteDef(id, 0), myLastUsed((int) lastUsed),
        myBeta(beta), myGawronA(gawronA), myLogitGamma(logitGamma),
        myMaxRouteNumber(maxRoutes), myKeepRoutes(keepRoutes) {
}


RORouteDef_Alternatives::~RORouteDef_Alternatives() throw() {
    for (AlternativesVector::iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
        delete *i;
    }
}


void
RORouteDef_Alternatives::addLoadedAlternative(RORoute *alt) {
    myAlternatives.push_back(alt);
}



RORoute *
RORouteDef_Alternatives::buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
        SUMOTime begin, const ROVehicle &veh) const {
    // recompute duration of the last route used
    // build a new route to test whether it is better
    std::vector<const ROEdge*> edges;
    router.compute(myAlternatives[0]->getFirst(), myAlternatives[0]->getLast(), &veh, begin, edges);
    RORoute *opt = new RORoute(myID, 0, 1, edges, copyColorIfGiven());
    SUMOReal costs = router.recomputeCosts(opt->getEdgeVector(), &veh, begin);
    // check whether the same route was already used
    myLastUsed = findRoute(opt);
    myNewRoute = true;
    // delete the route when it already existed
    if (myLastUsed>=0) {
        delete opt;
        myNewRoute = false;
        myAlternatives[myLastUsed]->setCosts(costs);
        return myAlternatives[myLastUsed];
    }
    // return the built route
    opt->setCosts(costs);
    return opt;
}


int
RORouteDef_Alternatives::findRoute(RORoute *opt) const {
    for (unsigned int i=0; i<myAlternatives.size(); i++) {
        if (opt->getEdgeVector() == myAlternatives[i]->getEdgeVector()) {
            return (int) i;
        }
    }
    return -1;
}


void
RORouteDef_Alternatives::addAlternative(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                        const ROVehicle *const veh, RORoute *current, SUMOTime begin) {
    // add the route when it's new
    if (myLastUsed<0) {
        myAlternatives.push_back(current);
        myLastUsed = (int) myAlternatives.size() - 1;
    }
    // recompute the costs and (when a new route was added) the probabilities
    for (AlternativesVector::iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
        RORoute *alt = *i;
        // apply changes for old routes only
        //  (the costs for the current were computed already)
        if ((*i)!=current||!myNewRoute) {
            // recompute the costs for old routes
            const SUMOReal oldCosts = alt->getCosts();
            const SUMOReal newCosts = router.recomputeCosts(alt->getEdgeVector(), veh, begin);
            if (newCosts<0) {
                throw ProcessError("Route '" + current->getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
            }
            if (myLogitGamma >= 0) {
                alt->setCosts(newCosts/3600.); //@todo need configurable scaling factor esp. for non travel time weights
            } else {
                alt->setCosts(myBeta * newCosts + ((SUMOReal) 1.0 - myBeta) * oldCosts);
            }
        }
        assert(myAlternatives.size()!=0);
        if (myNewRoute) {
            if ((*i)!=current) {
                alt->setProbability(
                    alt->getProbability()
                    * SUMOReal(myAlternatives.size()-1)
                    / SUMOReal(myAlternatives.size()));
            } else {
                alt->setProbability((SUMOReal)(1.0 / (SUMOReal) myAlternatives.size()));
            }
        }
    }
    assert(myAlternatives.size()!=0);
    // compute the probabilities
    if (myLogitGamma >= 0) {
        calculateLogitProbabilities();
    } else {
        for (AlternativesVector::iterator i=myAlternatives.begin(); i!=myAlternatives.end()-1; i++) {
            RORoute *pR = *i;
            for (AlternativesVector::iterator j=i+1; j!=myAlternatives.end(); j++) {
                RORoute *pS = *j;
                // see [Gawron, 1998] (4.2)
                SUMOReal delta =
                    (pS->getCosts() - pR->getCosts()) /
                    (pS->getCosts() + pR->getCosts());
                // see [Gawron, 1998] (4.3a, 4.3b)
                SUMOReal newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
                SUMOReal newPS = pR->getProbability() + pS->getProbability() - newPR;
                if (ISNAN(newPR)||ISNAN(newPS)) {
                    newPR = pS->getCosts() > pR->getCosts()
                            ? (SUMOReal) 1. : 0;
                    newPS = pS->getCosts() > pR->getCosts()
                            ? 0 : (SUMOReal) 1.;
                }
                newPR = MIN2((SUMOReal) MAX2(newPR, (SUMOReal) 0), (SUMOReal) 1);
                newPS = MIN2((SUMOReal) MAX2(newPS, (SUMOReal) 0), (SUMOReal) 1);
                pR->setProbability(newPR);
                pS->setProbability(newPS);
            }
        }
    }
    if (!myKeepRoutes) {
        // remove with probability of 0 (not mentioned in Gawron)
        for (AlternativesVector::iterator i=myAlternatives.begin(); i!=myAlternatives.end();) {
            if ((*i)->getProbability()==0) {
                i = myAlternatives.erase(i);
            } else {
                i++;
            }
        }
    }
    // find the route to use
    SUMOReal chosen = RandHelper::rand();
    int pos = 0;
    for (AlternativesVector::iterator i=myAlternatives.begin(); i!=myAlternatives.end()-1; i++, pos++) {
        chosen = chosen - (*i)->getProbability();
        if (chosen<=0) {
            myLastUsed = pos;
            return;
        }
    }
    myLastUsed = pos;
}


SUMOReal
RORouteDef_Alternatives::getThetaForCLogit() const {
    SUMOReal sum = 0.;
    SUMOReal diff = 0.;
    SUMOReal min = std::numeric_limits<SUMOReal>::max();
    for (AlternativesVector::const_iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
        const SUMOReal cost = (*i)->getCosts();
        sum += cost;
        if (cost < min) {
            min = cost;
        }
    }
    const SUMOReal meanCost = sum / SUMOReal(myAlternatives.size());
    for (AlternativesVector::const_iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
        diff += pow((*i)->getCosts() - meanCost, 2);
    }
    const SUMOReal sdCost = sqrt(diff/SUMOReal(myAlternatives.size()));
    if (sdCost > 0.04) { // Magic numbers from Lohse book
        return PI / (sqrt(6.) * sdCost * min);
    }
    return 1.;
}


void
RORouteDef_Alternatives::calculateLogitProbabilities() {
    const SUMOReal theta = getThetaForCLogit();
    if (myBeta > 0) {
        // calculate commonalities
        for (AlternativesVector::const_iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
            const RORoute *pR = *i;
            SUMOReal lengthR = 0;
            const std::vector<const ROEdge*> &edgesR = pR->getEdgeVector();
            for (std::vector<const ROEdge*>::const_iterator edge = edgesR.begin(); edge != edgesR.end(); ++edge) {
                lengthR += (*edge)->getTravelTime(0, 0)/3600.;
            }
            SUMOReal overlapSum = 0;
            for (AlternativesVector::const_iterator j=myAlternatives.begin(); j!=myAlternatives.end(); j++) {
                const RORoute *pS = *j;
                SUMOReal overlapLength = 0.;
                SUMOReal lengthS = 0;
                const std::vector<const ROEdge*> &edgesS = pS->getEdgeVector();
                for (std::vector<const ROEdge*>::const_iterator edge = edgesS.begin(); edge != edgesS.end(); ++edge) {
                    lengthS += (*edge)->getTravelTime(0, 0)/3600.;
                    if (std::find(edgesR.begin(), edgesR.end(), *edge) != edgesR.end()) {
                        overlapLength += (*edge)->getTravelTime(0, 0)/3600.;
                    }
                }
                overlapSum += pow(overlapLength/sqrt(lengthR * lengthS), myLogitGamma);
            }
            myCommonalities[pR] = myBeta * log(overlapSum);
        }
    }
    for (AlternativesVector::iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
        RORoute *pR = *i;
        SUMOReal weightedSum = 0;
        for (AlternativesVector::iterator j=myAlternatives.begin(); j!=myAlternatives.end(); j++) {
            RORoute *pS = *j;
            weightedSum += exp(theta * (pR->getCosts() - pS->getCosts() + myCommonalities[pR] - myCommonalities[pS]));
        }
        pR->setProbability(1./weightedSum);
    }
}


SUMOReal
RORouteDef_Alternatives::gawronF(SUMOReal pdr, SUMOReal pds, SUMOReal x) {
    if (((pdr*gawronG(myGawronA, x)+pds)==0)) {
        return std::numeric_limits<SUMOReal>::max();
    }
    return (pdr*(pdr+pds)*gawronG(myGawronA, x)) /
           (pdr*gawronG(myGawronA, x)+pds);
}


SUMOReal
RORouteDef_Alternatives::gawronG(SUMOReal a, SUMOReal x) {
    if (((1.0-(x*x))==0)) {
        return std::numeric_limits<SUMOReal>::max();
    }
    return (SUMOReal) exp((a*x)/(1.0-(x*x)));
}


RORouteDef *
RORouteDef_Alternatives::copy(const std::string &id) const {
    RORouteDef_Alternatives *ret = new RORouteDef_Alternatives(id,
            myLastUsed, myBeta, myGawronA, myLogitGamma, myMaxRouteNumber, myKeepRoutes);
    for (std::vector<RORoute*>::const_iterator i=myAlternatives.begin(); i!=myAlternatives.end(); i++) {
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
    assert(myAlternatives.size()>=2);
    myAlternatives.erase(myAlternatives.end()-1);
    myLastUsed = (int) myAlternatives.size() - 1;
    // !!! recompute probabilities
}


OutputDevice &
RORouteDef_Alternatives::writeXMLDefinition(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
        OutputDevice &dev, const ROVehicle * const veh,
        bool asAlternatives, bool withExitTimes) const {
    // (optional) alternatives header
    if (asAlternatives) {
        dev.openTag("routeDistribution") << " last=\"" << myLastUsed << "\">\n";
        for (size_t i=0; i!=myAlternatives.size(); i++) {
            const RORoute &alt = *(myAlternatives[i]);
            dev.openTag("route") << " cost=\"" << alt.getCosts();
            dev.setPrecision(8);
            dev << "\" probability=\"" << alt.getProbability();
            dev.setPrecision();
            if (alt.getColor()!=0) {
                dev << "\" color=\"" << *alt.getColor();
            } else if (myColor!=0) {
                dev << "\" color=\"" << *myColor;
            }
            dev << "\" edges=\"" << alt.getEdgeVector();
            if (withExitTimes) {
                SUMOReal time = STEPS2TIME(veh->getDepartureTime());
                dev << "\" exitTimes=\"";
                std::vector<const ROEdge*>::const_iterator i = alt.getEdgeVector().begin();
                for (; i!=alt.getEdgeVector().end(); ++i) {
                    if (i != alt.getEdgeVector().begin()) {
                        dev << " ";
                    }
                    time += (*i)->getTravelTime(veh, (SUMOTime) time);
                    dev << time;
                }
            }
            (dev << "\"").closeTag(true);
        }
        dev.closeTag();
        return dev;
    } else {
        return myAlternatives[myLastUsed]->writeXMLDefinition(router, dev, veh, asAlternatives, withExitTimes);
    }
}



/****************************************************************************/

