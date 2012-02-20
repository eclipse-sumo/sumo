/****************************************************************************/
/// @file    RORouteDef_Alternatives.h
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
#ifndef RORouteDef_Alternatives_h
#define RORouteDef_Alternatives_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include "RORoute.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDef_Alternatives
 * @brief A route definition which has some alternatives, already.
 */
class RORouteDef_Alternatives : public RORouteDef {
public:
    /// Constructor
    RORouteDef_Alternatives(const std::string& id, unsigned int lastUsed, const int maxRoutes,
                            const bool keepRoutes, const bool skipRouteCalculation) ;

    /// Destructor
    virtual ~RORouteDef_Alternatives() ;

    /** @brief Adds an alternative loaded from the file
        An alternative may also be generated whicle DUA */
    virtual void addLoadedAlternative(RORoute* alternative);

    /// Build the next route
    void preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle> &router, SUMOTime begin,
                               const ROVehicle& veh) const;

    /// Adds a build alternative
    void addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                        const ROVehicle* const, RORoute* current, SUMOTime begin);

    /** @brief Returns a copy of the route definition */
    RORouteDef* copy(const std::string& id) const;

    void invalidateLast();

    void removeLast();

    virtual OutputDevice& writeXMLDefinition(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
            OutputDevice& dev, const ROVehicle* const veh,
            bool asAlternatives, bool withExitTimes) const;

    /* @brief Returns destination of this route definition */
    const ROEdge* getDestination() const;

private:
    /// Searches for the route within the list of alternatives
    int findRoute(RORoute* opt) const;

    /** Function-object for sorting routes from highest to lowest probability. */
    struct ComparatorProbability {
        bool operator()(const RORoute* const a, const RORoute* const b) {
            return a->getProbability() > b->getProbability();
        }
    };

private:
    /// @brief Information whether a new route was generated
    mutable bool myNewRoute;

    /// @brief Index of the route used within the last step
    mutable int myLastUsed;

    /// @brief Definition of the storage for alternatives
    typedef std::vector<RORoute*> AlternativesVector;

    /// @brief The alternatives
    AlternativesVector myAlternatives;

    /// @brief The maximum route number
    const int myMaxRouteNumber;

    /// @brief Information whether all routes should be saved
    const bool myKeepRoutes;

    /// @brief Information whether new routes should be calculated
    const bool mySkipRouteCalculation;


private:
    /// @brief Invalidated copy constructor
    RORouteDef_Alternatives(const RORouteDef_Alternatives& src);

    /// @brief Invalidated assignment operator
    RORouteDef_Alternatives& operator=(const RORouteDef_Alternatives& src);

};


#endif

/****************************************************************************/

