/****************************************************************************/
/// @file    ROCostCalculator.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Calculators for route costs and probabilities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <limits>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsCont.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "ROVehicle.h"
#include "ROCostCalculator.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
ROCostCalculator* ROCostCalculator::myInstance = 0;


// ===========================================================================
// method definitions
// ===========================================================================
ROCostCalculator::ROCostCalculator() {
    OptionsCont& oc = OptionsCont::getOptions();
    myMaxRouteNumber = oc.getInt("max-alternatives");
    myKeepRoutes = oc.getBool("keep-all-routes");
    mySkipRouteCalculation = oc.getBool("skip-new-routes");
}


ROCostCalculator::~ROCostCalculator() {}


ROCostCalculator&
ROCostCalculator::getCalculator() {
    if (myInstance == 0) {
        OptionsCont& oc = OptionsCont::getOptions();
        if (oc.getBool("logit")) {
            myInstance = new ROLogitCalculator(oc.getFloat("logit.beta"), oc.getFloat("logit.gamma"), oc.getFloat("logit.theta"));
        } else {
            myInstance = new ROGawronCalculator(oc.getFloat("gawron.beta"), oc.getFloat("gawron.a"));
        }
    }
    return *myInstance;
}


void
ROCostCalculator::cleanup() {
    delete myInstance;
    myInstance = 0;
}


ROGawronCalculator::ROGawronCalculator(const SUMOReal beta, const SUMOReal a)
    : myBeta(beta), myA(a) {}


ROGawronCalculator::~ROGawronCalculator() {}


void
ROGawronCalculator::setCosts(RORoute* route, const SUMOReal costs, const bool isActive) const {
    if (isActive) {
        route->setCosts(costs);
    } else {
        route->setCosts(myBeta * costs + ((SUMOReal) 1.0 - myBeta) * route->getCosts());
    }
}


void
ROGawronCalculator::calculateProbabilities(const ROVehicle* const /* veh */, std::vector<RORoute*> alternatives) {
    for (std::vector<RORoute*>::iterator i = alternatives.begin(); i != alternatives.end() - 1; i++) {
        RORoute* pR = *i;
        for (std::vector<RORoute*>::iterator j = i + 1; j != alternatives.end(); j++) {
            RORoute* pS = *j;
            // see [Gawron, 1998] (4.2)
            const SUMOReal delta =
                (pS->getCosts() - pR->getCosts()) /
                (pS->getCosts() + pR->getCosts());
            // see [Gawron, 1998] (4.3a, 4.3b)
            SUMOReal newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
            SUMOReal newPS = pR->getProbability() + pS->getProbability() - newPR;
            if (ISNAN(newPR) || ISNAN(newPS)) {
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


SUMOReal
ROGawronCalculator::gawronF(const SUMOReal pdr, const SUMOReal pds, const SUMOReal x) const {
    if (pdr * gawronG(myA, x) + pds == 0) {
        return std::numeric_limits<SUMOReal>::max();
    }
    return (pdr * (pdr + pds) * gawronG(myA, x)) /
           (pdr * gawronG(myA, x) + pds);
}


SUMOReal
ROGawronCalculator::gawronG(const SUMOReal a, const SUMOReal x) const {
    if (((1.0 - (x * x)) == 0)) {
        return std::numeric_limits<SUMOReal>::max();
    }
    return (SUMOReal) exp((a * x) / (1.0 - (x * x)));
}




ROLogitCalculator::ROLogitCalculator(const SUMOReal beta, const SUMOReal gamma,
                                     const SUMOReal theta)
    : myBeta(beta), myGamma(gamma), myTheta(theta) {}


ROLogitCalculator::~ROLogitCalculator() {}


void
ROLogitCalculator::setCosts(RORoute* route, const SUMOReal costs, const bool /* isActive */) const {
    route->setCosts(costs);
}


void
ROLogitCalculator::calculateProbabilities(const ROVehicle* const veh, std::vector<RORoute*> alternatives) {
    const SUMOReal theta = myTheta >= 0 ? myTheta : getThetaForCLogit(alternatives);
    const SUMOReal beta = myBeta >= 0 ? myBeta : getBetaForCLogit(alternatives);
    if (beta > 0) {
        // calculate commonalities
        for (std::vector<RORoute*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
            const RORoute* pR = *i;
            SUMOReal lengthR = 0;
            const std::vector<const ROEdge*>& edgesR = pR->getEdgeVector();
            for (std::vector<const ROEdge*>::const_iterator edge = edgesR.begin(); edge != edgesR.end(); ++edge) {
                //@todo we should use costs here
                lengthR += (*edge)->getTravelTime(veh, STEPS2TIME(veh->getDepartureTime()));
            }
            SUMOReal overlapSum = 0;
            for (std::vector<RORoute*>::const_iterator j = alternatives.begin(); j != alternatives.end(); j++) {
                const RORoute* pS = *j;
                SUMOReal overlapLength = 0.;
                SUMOReal lengthS = 0;
                const std::vector<const ROEdge*>& edgesS = pS->getEdgeVector();
                for (std::vector<const ROEdge*>::const_iterator edge = edgesS.begin(); edge != edgesS.end(); ++edge) {
                    lengthS += (*edge)->getTravelTime(veh, STEPS2TIME(veh->getDepartureTime()));
                    if (std::find(edgesR.begin(), edgesR.end(), *edge) != edgesR.end()) {
                        overlapLength += (*edge)->getTravelTime(veh, STEPS2TIME(veh->getDepartureTime()));
                    }
                }
                overlapSum += pow(overlapLength / sqrt(lengthR * lengthS), myGamma);
            }
            myCommonalities[pR] = beta * log(overlapSum);
        }
    }
    for (std::vector<RORoute*>::iterator i = alternatives.begin(); i != alternatives.end(); i++) {
        RORoute* pR = *i;
        SUMOReal weightedSum = 0;
        for (std::vector<RORoute*>::iterator j = alternatives.begin(); j != alternatives.end(); j++) {
            RORoute* pS = *j;
            weightedSum += exp(theta * (pR->getCosts() - pS->getCosts() + myCommonalities[pR] - myCommonalities[pS]));
        }
        pR->setProbability(1. / weightedSum);
    }
}


SUMOReal
ROLogitCalculator::getBetaForCLogit(const std::vector<RORoute*> alternatives) const {
    SUMOReal min = std::numeric_limits<SUMOReal>::max();
    for (std::vector<RORoute*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
        const SUMOReal cost = (*i)->getCosts() / 3600.;
        if (cost < min) {
            min = cost;
        }
    }
    return min;
}


SUMOReal
ROLogitCalculator::getThetaForCLogit(const std::vector<RORoute*> alternatives) const {
    // @todo this calculation works for travel times only
    SUMOReal sum = 0.;
    SUMOReal diff = 0.;
    SUMOReal min = std::numeric_limits<SUMOReal>::max();
    for (std::vector<RORoute*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
        const SUMOReal cost = (*i)->getCosts() / 3600.;
        sum += cost;
        if (cost < min) {
            min = cost;
        }
    }
    const SUMOReal meanCost = sum / SUMOReal(alternatives.size());
    for (std::vector<RORoute*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
        diff += pow((*i)->getCosts() / 3600. - meanCost, 2);
    }
    const SUMOReal cvCost = sqrt(diff / SUMOReal(alternatives.size())) / meanCost;
    // @todo re-evaluate function
//    if (cvCost > 0.04) { // Magic numbers from Lohse book
    return M_PI / (sqrt(6.) * cvCost * (min + 1.1)) / 3600.;
//    }
//    return 1./3600.;
}


/****************************************************************************/
