/****************************************************************************/
/// @file    RORoute.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A build route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RORoute_h
#define RORoute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Named.h>
#include "ROEdgeVector.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORoute
 * A complete route
 */
class RORoute : public Named
{
public:
    /// Constructor
    RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
            const ROEdgeVector &route);

    /// Constructor
    RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
            const std::vector<const ROEdge*> &route);

    /// Destructor
    ~RORoute();

    /// Adds an edge to the end of the route
    void add(ROEdge *id);

    /// Saves the whole route (as a route)
    void xmlOut(std::ostream &os, bool isPeriodical) const;

    /// Saves the edges the route consists of
    void xmlOutEdges(std::ostream &os) const;

    /// Returns the first edge in the route
    const ROEdge *getFirst() const;

    /// Returns the last edge in the route
    const ROEdge *getLast() const;

    /// Returns the costs of the route
    SUMOReal getCosts() const;

    /// Returns the probability the driver will take this route with
    SUMOReal getProbability() const;

    /// Sets the costs of the route
    void setCosts(SUMOReal costs);

    /// Sets the probability of the route
    void setProbability(SUMOReal prop);

    /// Recomputes the costs of the route by summing up the costs for every edge
    SUMOReal recomputeCosts(const ROVehicle *const, SUMOTime begin) const;

    /// Returns the information whether the given route matches to this one
    bool equals(RORoute *ro) const;

    /// Returns the number of edges in this route
    size_t size() const;

    /** @brief Removes the first edge
        used if this edge is too short to emit vehicles on */
    void pruneFirst();

    const ROEdgeVector &getEdgeVector() const;

    void recheckForLoops();

private:
    /// The costs of the route
    SUMOReal _costs;

    /// The probability the driver will take this route with
    SUMOReal _probability;

    /// The edges the route consists of
    ROEdgeVector _route;

};


#endif

/****************************************************************************/

