/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2024 German Aerospace Center (DLR) and others.
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
/// @file    LogitCalculator.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// Calculators for route costs and probabilities
/****************************************************************************/
#pragma once
#include <config.h>

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
    LogitCalculator(const double beta, const double gamma,
                    const double theta) : myBeta(beta), myGamma(gamma), myTheta(theta) {}

    /// Destructor
    virtual ~LogitCalculator() {}

    void setCosts(R* route, const double costs, const bool /* isActive */) const {
        route->setCosts(costs);
    }

    /** @brief calculate the probabilities in the logit model */
    void calculateProbabilities(std::vector<R*> alternatives, const V* const veh, const SUMOTime time) {
        const double theta = myTheta >= 0 ? myTheta : getThetaForCLogit(alternatives);
        const double beta = myBeta >= 0 ? myBeta : getBetaForCLogit(alternatives);
        const double t = STEPS2TIME(time);
        if (beta > 0) {
            // calculate commonalities
            for (const R* const pR : alternatives) {
                double lengthR = 0;
                const std::vector<const E*>& edgesR = pR->getEdgeVector();
                for (const E* const edge : edgesR) {
                    //@todo we should use costs here
                    lengthR += edge->getTravelTime(veh, t);
                }
                double overlapSum = 0;
                for (const R* const pS : alternatives) {
                    double overlapLength = 0.;
                    double lengthS = 0;
                    for (const E* const edge : pS->getEdgeVector()) {
                        lengthS += edge->getTravelTime(veh, t);
                        if (std::find(edgesR.begin(), edgesR.end(), edge) != edgesR.end()) {
                            overlapLength += edge->getTravelTime(veh, t);
                        }
                    }
                    overlapSum += pow(overlapLength / sqrt(lengthR * lengthS), myGamma);
                }
                myCommonalities[pR] = beta * log(overlapSum);
            }
        }
        for (R* const pR : alternatives) {
            double weightedSum = 0;
            for (const R* const pS : alternatives) {
                weightedSum += exp(theta * (pR->getCosts() - pS->getCosts() + myCommonalities[pR] - myCommonalities[pS]));
            }
            pR->setProbability(1. / weightedSum);
        }
    }


private:
    /** @brief calculate the scaling factor in the logit model */
    double getBetaForCLogit(const std::vector<R*> alternatives) const {
        double min = std::numeric_limits<double>::max();
        for (const R* const pR : alternatives) {
            const double cost = pR->getCosts() / 3600.;
            if (cost < min) {
                min = cost;
            }
        }
        return min;
    }

    /** @brief calculate the scaling factor in the logit model */
    double getThetaForCLogit(const std::vector<R*> alternatives) const {
        // @todo this calculation works for travel times only
        double sum = 0.;
        double diff = 0.;
        double min = std::numeric_limits<double>::max();
        for (const R* const pR : alternatives) {
            const double cost = pR->getCosts() / 3600.;
            sum += cost;
            if (cost < min) {
                min = cost;
            }
        }
        const double meanCost = sum / double(alternatives.size());
        for (const R* const pR : alternatives) {
            diff += pow(pR->getCosts() / 3600. - meanCost, 2);
        }
        const double cvCost = sqrt(diff / double(alternatives.size())) / meanCost;
        // @todo re-evaluate function
        if (cvCost > 0) { // all Magic numbers from Lohse book, original says this should be cvCost > 0.04
            return 3.1415926535897932384626433832795 / (sqrt(6.) * cvCost * (min + 1.1)) / 3600.;
        }
        return 1. / 3600.;
    }


private:
    /// @brief logit beta - value
    const double myBeta;

    /// @brief logit gamma - value
    const double myGamma;

    /// @brief logit theta - value
    const double myTheta;

    /// @brief The route commonality factors for c-logit
    std::map<const R*, double> myCommonalities;

private:
    /** @brief invalidated assignment operator */
    LogitCalculator& operator=(const LogitCalculator& s);

};
