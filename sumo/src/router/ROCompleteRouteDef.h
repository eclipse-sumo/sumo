#ifndef ROCompleteRouteDef_h
#define ROCompleteRouteDef_h
//---------------------------------------------------------------------------//
//                        ROCompleteRouteDef.h -
//  A complete route definition (with all passed edges being known)
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
// Revision 1.4  2003/03/20 17:40:59  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
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
#include "ROEdgeVector.h"
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
 * @class ROCompleteRouteDef
 * A complete route description containing all edges the driver will pass.
 */
class ROCompleteRouteDef :
            public RORouteDef {
public:
    /// Constructor
    ROCompleteRouteDef(const std::string &id, const ROEdgeVector &edges);

    /// Destructor
	virtual ~ROCompleteRouteDef();

    /// returns the begin of the trip
    ROEdge *getFrom() const;

    /// Returns the end of the trip
    ROEdge *getTo() const;

protected:
    /// Builds the route
    RORoute *buildCurrentRoute(RORouter &router, long begin);

    /** @brief Adds an route alternative (see further comments)
        Here, as in most cases, the alternative is the route that was build
        as last and will stay the only route known */
    void addAlternative(RORoute *current, long begin);

    /// Outputs the new (current) route
    void xmlOutCurrent(std::ostream &res, bool isPeriodical) const;

    /** @brief Outputs the alternatives
        (being the only one route here) */
    void xmlOutAlternatives(std::ostream &altres) const;

protected:
    /// The list of edges the driver passes
    ROEdgeVector _edges;

    /// The begin of the route
    long _startTime;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROCompleteRouteDef.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

