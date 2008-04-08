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


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class OutputDevice;


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
            const std::vector<const ROEdge*> &route) throw();

    /// Destructor
    ~RORoute() throw();

    /// Adds an edge to the end of the route
    void add(ROEdge *id);

    /// Saves the whole route (as a route)
    void xmlOut(OutputDevice &dev, bool isPeriodical) const;

    /// Saves the edges the route consists of
    void xmlOutEdges(OutputDevice &dev) const;

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

    /// Returns the number of edges in this route
    size_t size() const;

    /** @brief Removes the first edge
        used if this edge is too short to emit vehicles on */
    void pruneFirst();

    const std::vector<const ROEdge*> &getEdgeVector() const;

    void recheckForLoops();

private:
    /// The costs of the route
    SUMOReal myCosts;

    /// The probability the driver will take this route with
    SUMOReal myProbability;

    /// The edges the route consists of
    std::vector<const ROEdge*> myRoute;

};


#endif

/****************************************************************************/

