/****************************************************************************/
/// @file    ROCostCalculator.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Calculators for route costs and probabilities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#ifndef ROCostCalculator_h
#define ROCostCalculator_h


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
// class declarations
// ===========================================================================
class RORoute;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROCostCalculator
 * @brief Abstract base class providing static factory method.
 */
class ROCostCalculator {
public:
    static ROCostCalculator& getCalculator();

    static void cleanup();

    virtual void setCosts(RORoute* route, const SUMOReal costs, const bool isActive = false) const = 0;

    /** @brief calculate the probabilities in the logit model */
    virtual void calculateProbabilities(std::vector<RORoute*> alternatives, const ROVehicle* const veh, const SUMOTime time) = 0;

    unsigned int getMaxRouteNumber() const {
        return myMaxRouteNumber;
    }

    bool keepRoutes() const {
        return myKeepRoutes;
    }

    bool skipRouteCalculation() const {
        return mySkipRouteCalculation;
    }

protected:
    /// @brief Constructor
    ROCostCalculator();

    /// @brief Destructor
    virtual ~ROCostCalculator();

private:
    static ROCostCalculator* myInstance;

    /// @brief The maximum route alternatives number
    unsigned int myMaxRouteNumber;

    /// @brief Information whether all routes should be saved
    bool myKeepRoutes;

    /// @brief Information whether new routes should be calculated
    bool mySkipRouteCalculation;

};


/**
 * @class ROGawronCalculator
 * @brief Cost calculation with Gawron's method.
 */
class ROGawronCalculator : public ROCostCalculator {
public:
    /// Constructor
    ROGawronCalculator(const SUMOReal beta, const SUMOReal a);

    /// Destructor
    virtual ~ROGawronCalculator();

    void setCosts(RORoute* route, const SUMOReal costs, const bool isActive = false) const;

    /** @brief calculate the probabilities */
    void calculateProbabilities(std::vector<RORoute*> alternatives, const ROVehicle* const veh, const SUMOTime time);

private:
    /** @brief Performs the gawron - f() function
        From "Dynamic User Equilibria..." */
    SUMOReal gawronF(const SUMOReal pdr, const SUMOReal pds, const SUMOReal x) const;

    /** @brief Performs the gawron - g() function
        From "Dynamic User Equilibria..." */
    SUMOReal gawronG(const SUMOReal a, const SUMOReal x) const;

private:
    /// @brief gawron beta - value
    const SUMOReal myBeta;

    /// @brief gawron a - value
    const SUMOReal myA;

private:
    /** @brief invalidated assignment operator */
    ROGawronCalculator& operator=(const ROGawronCalculator& s);

};


/**
 * @class ROLogitCalculator
 * @brief Cost calculation with c-logit or logit method.
 */
class ROLogitCalculator : public ROCostCalculator {
public:
    /// Constructor
    ROLogitCalculator(const SUMOReal beta, const SUMOReal gamma,
                      const SUMOReal theta);

    /// Destructor
    virtual ~ROLogitCalculator();

    void setCosts(RORoute* route, const SUMOReal costs, const bool isActive = false) const;

    /** @brief calculate the probabilities in the logit model */
    void calculateProbabilities(std::vector<RORoute*> alternatives, const ROVehicle* const veh, const SUMOTime time);

private:
    /** @brief calculate the scaling factor in the logit model */
    SUMOReal getBetaForCLogit(const std::vector<RORoute*> alternatives) const;

    /** @brief calculate the scaling factor in the logit model */
    SUMOReal getThetaForCLogit(const std::vector<RORoute*> alternatives) const;

private:
    /// @brief logit beta - value
    const SUMOReal myBeta;

    /// @brief logit gamma - value
    const SUMOReal myGamma;

    /// @brief logit theta - value
    const SUMOReal myTheta;

    /// @brief The route commonality factors for c-logit
    std::map<const RORoute*, SUMOReal> myCommonalities;

private:
    /** @brief invalidated assignment operator */
    ROLogitCalculator& operator=(const ROLogitCalculator& s);

};


#endif

/****************************************************************************/

