/****************************************************************************/
/// @file    RORouteDef.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Base class for a vehicle's route definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RORouteDef_h
#define RORouteDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include "ReferencedItem.h"
#include <utils/common/SUMOAbstractRouter.h>
#include "RORoute.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class OptionsCont;
class ROVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDef
 * @brief Base class for a vehicle's route definition
 *
 * This class resembles what a vehicle knows about his route when being loaded
 *  into a router. Whether it is just the origin and the destination, the whole
 *  route through the network or even a route with alternatives depends on
 *  the derived class.
 */
class RORouteDef : public ReferencedItem, public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the route
     * @param[in] color The color of the route
     */
    RORouteDef(const std::string& id, const unsigned int lastUsed,
               const bool tryRepair);


    /// @brief Destructor
    virtual ~RORouteDef();


    /** @brief Adds a single alternative loaded from the file
        An alternative may also be generated during DUA */
    void addLoadedAlternative(RORoute* alternative);

    /** @brief Adds an alternative loaded from the file */
    void addAlternativeDef(const RORouteDef* alternative);

    /** @brief Triggers building of the complete route (via
     * preComputeCurrentRoute) or returns precomputed route */
    RORoute* buildCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router, SUMOTime begin,
                               const ROVehicle& veh) const;

    /** @brief Builds the complete route
     * (or chooses her from the list of alternatives, when existing) */
    void preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router, SUMOTime begin,
                                const ROVehicle& veh) const;

    /** @brief Builds the complete route
     * (or chooses her from the list of alternatives, when existing) */
    void repairCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router, SUMOTime begin,
                            const ROVehicle& veh) const;

    /** @brief Adds an alternative to the list of routes
    *
     * (This may be the new route) */
    void addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                        const ROVehicle* const, RORoute* current, SUMOTime begin);

    const ROEdge* getDestination() const;

    const RORoute* getFirstRoute() const {
        return myAlternatives.front();
    }

    /** @brief Saves the built route / route alternatives
     *
     * Writes the route into the given stream.
     *
     * @param[in] dev The device to write the route into
     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
     * @return The same device for further usage
     */
    OutputDevice& writeXMLDefinition(OutputDevice& dev, const ROVehicle* const veh,
                                     bool asAlternatives, bool withExitTimes) const;

    /** @brief Returns a origin-destination copy of the route definition.
     *
     * The resulting route definition contains only a single route with
     * origin and destination edge copied from this one
     *
     * @param[in] id The id for the new route definition
     * @return the new route definition
     */
    RORouteDef* copyOrigDest(const std::string& id) const;

    /** @brief Returns a deep copy of the route definition.
     *
     * The resulting route definition contains copies of all
     * routes contained in this one
     *
     * @param[in] id The id for the new route definition
     * @return the new route definition
     */
    RORouteDef* copy(const std::string& id) const;

    /** @brief Returns the sum of the probablities of the contained routes */
    SUMOReal getOverallProb() const;

protected:
    /// @brief precomputed route for out-of-order computation
    mutable RORoute* myPrecomputed;

    /// @brief Index of the route used within the last step
    mutable unsigned int myLastUsed;

    /// @brief The alternatives
    std::vector<RORoute*> myAlternatives;

    /// @brief Information whether a new route was generated
    mutable bool myNewRoute;

    const bool myTryRepair;

private:
    /** Function-object for sorting routes from highest to lowest probability. */
    struct ComparatorProbability {
        bool operator()(const RORoute* const a, const RORoute* const b) {
            return a->getProbability() > b->getProbability();
        }
    };

private:
    /// @brief Invalidated copy constructor
    RORouteDef(const RORouteDef& src);

    /// @brief Invalidated assignment operator
    RORouteDef& operator=(const RORouteDef& src);

};


#endif

/****************************************************************************/

