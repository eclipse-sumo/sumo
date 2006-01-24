#ifndef RORouteDef_Complete_h
#define RORouteDef_Complete_h
//---------------------------------------------------------------------------//
//                        RORouteDef_Complete.h -
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
// Revision 1.8  2006/01/24 13:43:53  dkrajzew
// added vehicle classes to the routing modules
//
// Revision 1.7  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.6  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
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
// Revision 1.7  2003/11/11 08:04:45  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.6  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.5  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.4  2003/03/20 17:40:59  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include "ROEdgeVector.h"
#include "RORouteDef.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class RORoute;
class ROAbstractRouter;
class ROVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDef_Complete
 * @brief A complete route description containing all edges the driver will pass.
 */
class RORouteDef_Complete :
            public RORouteDef {
public:
    /// Constructor
    RORouteDef_Complete(const std::string &id, const RGBColor &color,
        const ROEdgeVector &edges);

    /// Destructor
    virtual ~RORouteDef_Complete();

    /// returns the begin of the trip
    ROEdge *getFrom() const;

    /// Returns the end of the trip
    ROEdge *getTo() const;

    /// Builds the route
    RORoute *buildCurrentRoute(ROAbstractRouter &router, SUMOTime begin,
        bool continueOnUnbuild, ROVehicle &veh,
		ROAbstractRouter::ROAbstractEdgeEffortRetriever * const retriever) const;

    /** @brief Adds an route alternative (see further comments)
        Here, as in most cases, the alternative is the route that was build
        as last and will stay the only route known */
    void addAlternative(RORoute *current, SUMOTime begin);

    /// Outputs the new (current) route
    void xmlOutCurrent(std::ostream &res, bool isPeriodical) const;

    /** @brief Returns a copy of the route definition */
    RORouteDef *copy(const std::string &id) const;

    const ROEdgeVector &getCurrentEdgeVector() const;

protected:
    /// The list of edges the driver passes
    ROEdgeVector _edges;

    /// The begin of the route
    SUMOTime _startTime;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

