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
// Revision 1.3  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
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
    RORoute(const std::string &id, double costs, double prop,
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
    double getCosts() const;

    /// Returns the propability the driver will take this route with
    double getPropability() const;

    /// Sets the costs of the route
    void setCosts(double costs);

    /// Sets the propability of the route
    void setPropability(double prop);

    /// Recomputes the costs of the route by summing up the costs for every edge
    double recomputeCosts(long begin) const;

    /// Returns the information whether the given route matches to this one
    bool equals(RORoute *ro) const;

    /// Returns the number of edges in this route
    size_t size() const;

private:
    /// The costs of the route
    double _costs;

    /// The propability the driver will take this route with
    double _propability;

    /// The edges the route consists of
	ROEdgeVector _route;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RORoute.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

