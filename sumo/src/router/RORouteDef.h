#ifndef RORouteDef_h
#define RORouteDef_h
//---------------------------------------------------------------------------//
//                        RORouteDef.h -
//  Basic class for route definitions (not the computed routes)
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
#include <iostream>
#include <utils/common/Named.h>
#include "ReferencedItem.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class RORouter;
class RORoute;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDef
 * A RORouteDef is the upper class for all route definitions.
 * Whether it is just the origin and the destination, the whole route through
 * the network or even a route with alternatives depends on the derived class.
 */
class RORouteDef : public ReferencedItem,
                   public Named {
public:
    /// Constructor
    RORouteDef(const std::string &id);

    /// Destructor
	virtual ~RORouteDef();

    /// Returns the route's origin edge
    virtual ROEdge *getFrom() const = 0;

    /// Returns the route's destination edge
    virtual ROEdge *getTo() const = 0;

    /** @brief builds the route and saves it into the given files
        The route herself will be written into the first, the alternatives
        (also including the current route) will be written to the second file */
    bool computeAndSave(RORouter &router, long begin,
        std::ostream &res, std::ostream &altres);

protected:
    /** @brief Builds the complete route
        (or chooses her from the list of alternatives, when existing) */
    virtual RORoute *buildCurrentRoute(RORouter &router, long begin) = 0;

    /** @brief Adds an alternative to the list of routes
        (This may be the new route) */
    virtual void addAlternative(RORoute *current, long begin) = 0;

    /** @brief Writes the current route */
    virtual void xmlOutCurrent(std::ostream &res) const = 0;

    /** @brief Writes the list of known alternatives */
    virtual void xmlOutAlternatives(std::ostream &altres) const = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RORouteDef.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

