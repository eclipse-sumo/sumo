#ifndef RORoute_h
#define RORoute_h
//---------------------------------------------------------------------------//
//                        RORoute.h -
//  A build route
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.10  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.6  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.5  2004/04/02 11:25:34  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added
//
// Revision 1.4  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.3  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/Named.h>
#include "ROEdgeVector.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORoute
 * A complete route
 */
class RORoute : public Named {
public:
    /// Constructor
    RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
        const ROEdgeVector &route);

    /// Destructor
    ~RORoute();

    /// Adds an edge to the end of the route
    void add(ROEdge *id);

    /// Saves the whole route (as a route)
    void xmlOut(std::ostream &os, bool isPeriodical) const;

    /// Saves the edges the route consists of
    void xmlOutEdges(std::ostream &os) const;

    /// Returns the first edge in the route
    ROEdge *getFirst() const;

    /// Returns the last edge in the route
    ROEdge *getLast() const;

    /// Returns the costs of the route
    SUMOReal getCosts() const;

    /// Returns the probability the driver will take this route with
    SUMOReal getProbability() const;

    /// Sets the costs of the route
    void setCosts(SUMOReal costs);

    /// Sets the probability of the route
    void setProbability(SUMOReal prop);

    /// Recomputes the costs of the route by summing up the costs for every edge
    SUMOReal recomputeCosts(SUMOTime begin) const;

    /// Returns the information whether the given route matches to this one
    bool equals(RORoute *ro) const;

    /// Returns the number of edges in this route
    size_t size() const;

    /** @brief Removes the first edge
        used if this edge is too short to emit vehicles on */
    void pruneFirst();

    const ROEdgeVector &getEdgeVector() const;


private:
    /// The costs of the route
    SUMOReal _costs;

    /// The probability the driver will take this route with
    SUMOReal _probability;

    /// The edges the route consists of
    ROEdgeVector _route;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

