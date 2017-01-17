/****************************************************************************/
/// @file    LogitCalculator.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Calculators for route costs and probabilities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef LogitCalculator_h
#define LogitCalculator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LogitCalculator
 * @brief Cost calculation with c-logit or logit method.
 */
template<class R, class E, class V>
class LogitCalculator : public RouteCostCalculator<R, E, V> {
public:
    /// Constructor
    LogitCalculator(const SUMOReal beta, const SUMOReal gamma,
                    const SUMOReal theta) : myBeta(beta), myGamma(gamma), myTheta(theta) {}

    /// Destructor
    virtual ~LogitCalculator() {}

    void setCosts(R* route, const SUMOReal costs, const bool /* isActive */) const {
        route->setCosts(costs);
    }

    /** @brief calculate the probabilities in the logit model */
    void calculateProbabilities(std::vector<R*> alternatives, const V* const veh, const SUMOTime time) {
        const SUMOReal theta = myTheta >= 0 ? myTheta : getThetaForCLogit(alternatives);
        const SUMOReal beta = myBeta >= 0 ? myBeta : getBetaForCLogit(alternatives);
        if (beta > 0) {
            // calculate commonalities
            for (typename std::vector<R*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
                const R* pR = *i;
                SUMOReal lengthR = 0;
                const std::vector<const E*>& edgesR = pR->getEdgeVector();
                for (typename std::vector<const E*>::const_iterator edge = edgesR.begin(); edge != edgesR.end(); ++edge) {
                    //@todo we should use costs here
                    lengthR += (*edge)->getTravelTime(veh, STEPS2TIME(time));
                }
                SUMOReal overlapSum = 0;
                for (typename std::vector<R*>::const_iterator j = alternatives.begin(); j != alternatives.end(); j++) {
                    const R* pS = *j;
                    SUMOReal overlapLength = 0.;
                    SUMOReal lengthS = 0;
                    const std::vector<const E*>& edgesS = pS->getEdgeVector();
                    for (typename std::vector<const E*>::const_iterator edge = edgesS.begin(); edge != edgesS.end(); ++edge) {
                        lengthS += (*edge)->getTravelTime(veh, STEPS2TIME(time));
                        if (std::find(edgesR.begin(), edgesR.end(), *edge) != edgesR.end()) {
                            overlapLength += (*edge)->getTravelTime(veh, STEPS2TIME(time));
                        }
                    }
                    overlapSum += pow(overlapLength / sqrt(lengthR * lengthS), myGamma);
                }
                myCommonalities[pR] = beta * log(overlapSum);
            }
        }
        for (typename std::vector<R*>::iterator i = alternatives.begin(); i != alternatives.end(); i++) {
            R* pR = *i;
            SUMOReal weightedSum = 0;
            for (typename std::vector<R*>::iterator j = alternatives.begin(); j != alternatives.end(); j++) {
                R* pS = *j;
                weightedSum += exp(theta * (pR->getCosts() - pS->getCosts() + myCommonalities[pR] - myCommonalities[pS]));
            }
            pR->setProbability(1. / weightedSum);
        }
    }


private:
    /** @brief calculate the scaling factor in the logit model */
    SUMOReal getBetaForCLogit(const std::vector<R*> alternatives) const {
        SUMOReal min = std::numeric_limits<SUMOReal>::max();
        for (typename std::vector<R*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
            const SUMOReal cost = (*i)->getCosts() / 3600.;
            if (cost < min) {
                min = cost;
            }
        }
        return min;
    }

    /** @brief calculate the scaling factor in the logit model */
    SUMOReal getThetaForCLogit(const std::vector<R*> alternatives) const {
        // @todo this calculation works for travel times only
        SUMOReal sum = 0.;
        SUMOReal diff = 0.;
        SUMOReal min = std::numeric_limits<SUMOReal>::max();
        for (typename std::vector<R*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
            const SUMOReal cost = (*i)->getCosts() / 3600.;
            sum += cost;
            if (cost < min) {
                min = cost;
            }
        }
        const SUMOReal meanCost = sum / SUMOReal(alternatives.size());
        for (typename std::vector<R*>::const_iterator i = alternatives.begin(); i != alternatives.end(); i++) {
            diff += pow((*i)->getCosts() / 3600. - meanCost, 2);
        }
        const SUMOReal cvCost = sqrt(diff / SUMOReal(alternatives.size())) / meanCost;
        // @todo re-evaluate function
        //    if (cvCost > 0.04) { // Magic numbers from Lohse book
        return 3.1415926535897932384626433832795 / (sqrt(6.) * cvCost * (min + 1.1)) / 3600.;
        //    }
        //    return 1./3600.;
    }


private:
    /// @brief logit beta - value
    const SUMOReal myBeta;

    /// @brief logit gamma - value
    const SUMOReal myGamma;

    /// @brief logit theta - value
    const SUMOReal myTheta;

    /// @brief The route commonality factors for c-logit
    std::map<const R*, SUMOReal> myCommonalities;

private:
    /** @brief invalidated assignment operator */
    LogitCalculator& operator=(const LogitCalculator& s);

};


#endif

/****************************************************************************/

