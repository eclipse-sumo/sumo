#ifndef ROOrigDestRouteDef_h
#define ROOrigDestRouteDef_h
//---------------------------------------------------------------------------//
//                        ROOrigDestRouteDef.h -
//  A route where only the origin and the destination edges are known
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
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
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
#include "RORouteDef.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class RORoute;
class RORouter;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROOrigDestRouteDef
 * A route definition where only the begin and the end edge are given.
 */
class ROOrigDestRouteDef : public RORouteDef {
public:
    /// Constructor
    ROOrigDestRouteDef(const std::string &id, ROEdge *from, ROEdge *to);

    /// Destructor
	virtual ~ROOrigDestRouteDef();

    /// Returns the begin of the trip
    ROEdge *getFrom() const;

    /// Returns the end of the trip
    ROEdge *getTo() const;

protected:
    /// Builds the current route from the given information (perform routing, here)
    RORoute *buildCurrentRoute(RORouter &router, long begin);

    /** @brief Adds the build route to the container
        Here, the currently new route is added */
    void addAlternative(RORoute *current, long begin);

    /// Saves the current route
    void xmlOutCurrent(std::ostream &res) const;

    /// Saves the current route as a single alternative
    void xmlOutAlternatives(std::ostream &altres) const;

protected:
    /// The origin and the destination edge of the route
	ROEdge *_from, *_to;

    /// The complete route (after building)
    RORoute *_current;

    /// The begin of the trip
    long _startTime;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROOrigDestRouteDef.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

