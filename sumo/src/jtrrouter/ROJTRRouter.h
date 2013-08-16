/****************************************************************************/
/// @file    ROJTRRouter.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Computes routes using junction turning percentages
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
#ifndef ROJTRRouter_h
#define ROJTRRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOAbstractRouter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class ROEdge;
class ROJTREdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTRRouter
 * @brief Computes routes using junction turning percentages
 */
class ROJTRRouter : public SUMOAbstractRouter<ROEdge, ROVehicle> {
public:
    /** @brief Constructor
     * @param[in] net The net used for routing
     * @param[in] unbuildIsWarningOnly Whether not closed routes shall not yield in an error
     * @param[in] acceptAllDestinations If false, only sinks will be used as final edges
     * @param[in] maxEdges The maximum number of edges a route may have
     * @param[in] ignoreClasses Whether routing shall be done without regarding vehicle classes
     * @param[in] allowLoops Whether a vehicle may reuse a road
     */
    ROJTRRouter(RONet& net, bool unbuildIsWarningOnly,
                bool acceptAllDestinations, int maxEdges, bool ignoreClasses,
                bool allowLoops);


    /// @brief Destructor
    ~ROJTRRouter();



    /// @name Implementatios of SUMOAbstractRouter
    /// @{

    /** @brief Computes a route
     *
     * The description how routes are computed is given in the user documentation
     * @param[in] from The edge the vehicle starts at
     * @param[in] to The destination edge - invalid here
     * @param[in] vehicle The vehicle to compute the route for
     * @param[in] time The departure time of the vehicle
     * @param[filled] into The list of edges to store the route into
     */
    void compute(const ROEdge* from, const ROEdge* to, const ROVehicle* const vehicle,
                 SUMOTime time, std::vector<const ROEdge*>& into);


    /** @brief Recomputes the costs of a route
     * @param[in] edges The route
     * @param[in] v The vehicle that belongs to the route
     * @param[in] time The departure time of the vehicle
     * @return The route costs
     */
    SUMOReal recomputeCosts(const std::vector<const ROEdge*>& edges, const ROVehicle* const v, SUMOTime time) const;
    /// @}


private:
    /// @brief The network to use
    RONet& myNet;

    /// @brief Whether unbuildable routes shall be reported as warniings, not errors
    const bool myUnbuildIsWarningOnly;

    /// @brief Whether all edges may be used as route end
    const bool myAcceptAllDestination;

    /// @brief The maximum number of edges a route may have
    const int myMaxEdges;

    /// @brief Whether vehicle class information shall be ignored
    const bool myIgnoreClasses;

    /// @brief Whether a vehicle may reuse a road
    const bool myAllowLoops;

};


#endif

/****************************************************************************/

