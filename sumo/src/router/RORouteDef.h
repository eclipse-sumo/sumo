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
// Revision 1.16  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.15  2006/01/24 13:43:53  dkrajzew
// added vehicle classes to the routing modules
//
// Revision 1.14  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.13  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.11  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.10  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.9  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT;
//  preparation of classes to be derived for an online-routing
//
// Revision 1.8  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.7  2003/11/11 08:04:47  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.6  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.5  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
// debugging
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include <utils/gfx/RGBColor.h>
#include "ReferencedItem.h"
#include "ROAbstractRouter.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class RORoute;
class OptionsCont;
class ROVehicle;
class ROEdgeVector;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDef
 * @brief A RORouteDef is the upper class for all route definitions.
 *
 * Whether it is just the origin and the destination, the whole route through
 * the network or even a route with alternatives depends on the derived class.
 */
class RORouteDef : public ReferencedItem,
                   public Named {
public:
    /// Constructor
    RORouteDef(const std::string &id,
        const RGBColor &color);

    /// Destructor
    virtual ~RORouteDef();

    /// Returns the route's origin edge
    virtual const ROEdge * const getFrom() const = 0;

    /// Returns the route's destination edge
    virtual const ROEdge * const getTo() const = 0;

    /** @brief Changes the id to a next, hopefully valid
	 *
     * This is done if the vehicle(s) using this route are emitted periodically */
    void patchID();

    /** @brief Builds the complete route
     *
     * (or chooses her from the list of alternatives, when existing) */
    virtual RORoute *buildCurrentRoute(ROAbstractRouter &router, SUMOTime begin,
        ROVehicle &veh) const = 0;

    /** @brief Adds an alternative to the list of routes
	 *
     * (This may be the new route) */
    virtual void addAlternative(RORoute *current, SUMOTime begin) = 0;

    /** @brief Returns a copy of the route definition */
    virtual RORouteDef *copy(const std::string &id) const = 0;

    virtual const ROEdgeVector &getCurrentEdgeVector() const = 0;

    /** @brief returns the index of the route that was used as last
	 *
     * Returns 0 for most of the implementations, as currently only alternatives may
     * have more than just a single route */
    virtual int getLastUsedIndex() const;

    /** @brief returns the number of alternatives
	 *
     * Returns 1 for most of the implementations, as currently only alternatives may
     * have more than just a single route */
    virtual size_t getAlternativesSize() const;

    /// Returns the color of the route
    const RGBColor &getColor() const;

protected:
    /// The color the route shall have
    RGBColor myColor;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

