#ifndef RORouteDef_OrigDest_h
#define RORouteDef_OrigDest_h
//---------------------------------------------------------------------------//
//                        RORouteDef_OrigDest.h -
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
// Revision 1.4  2005/05/04 08:53:07  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.2  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.8  2003/11/11 08:04:46  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.7  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.6  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.5  2003/04/10 15:47:01  dkrajzew
// random routes are now being prunned to avoid some stress with turning
//  vehicles
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
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
class ROAbstractRouter;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDef_OrigDest
 * A route definition where only the begin and the end edge are given.
 */
class RORouteDef_OrigDest
    : public RORouteDef {
public:
    /// Constructor
    RORouteDef_OrigDest(const std::string &id, const RGBColor &color,
        ROEdge *from, ROEdge *to, bool removeFirst=false);

    /// Destructor
    virtual ~RORouteDef_OrigDest();

    /// Returns the begin of the trip
    ROEdge *getFrom() const;

    /// Returns the end of the trip
    ROEdge *getTo() const;

    /// Builds the current route from the given information (perform routing, here)
    RORoute *buildCurrentRoute(ROAbstractRouter &router, SUMOTime begin,
        bool continueOnUnbuild, ROVehicle &veh,
		ROAbstractRouter::ROAbstractEdgeEffortRetriever * const retriever);

    /** @brief Adds the build route to the container
        Here, the currently new route is added */
    void addAlternative(RORoute *current, SUMOTime begin);

    /// Saves the current route
    void xmlOutCurrent(std::ostream &res, bool isPeriodical) const;

    /// Saves the current route as a single alternative
    void xmlOutAlternatives(std::ostream &altres) const;

    /** @brief Returns a copy of the route definition */
    RORouteDef *copy(const std::string &id) const;

    const ROEdgeVector &getCurrentEdgeVector() const;

protected:
    /// The origin and the destination edge of the route
    ROEdge *_from, *_to;

    /// The complete route (after building)
    RORoute *_current;

    /// The begin of the trip
    SUMOTime _startTime;

    /** @brief Information whether the first edge shall be removed
        */
    bool myRemoveFirst;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

