/****************************************************************************/
/// @file    RORouteDef.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Basic class for route definitions (not the computed routes)
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
#ifndef RORouteDef_h
#define RORouteDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include "ReferencedItem.h"
#include <utils/common/SUMOAbstractRouter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RORoute;
class OptionsCont;
class ROVehicle;
class ROEdgeVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDef
 * @brief A RORouteDef is the upper class for all route definitions.
 *
 * Whether it is just the origin and the destination, the whole route through
 * the network or even a route with alternatives depends on the derived class.
 */
class RORouteDef : public ReferencedItem,
            public Named
{
public:
    /// Constructor
    RORouteDef(const std::string &id, const std::string &color) throw();

    /// Destructor
    virtual ~RORouteDef() throw();

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
    virtual RORoute *buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime begin,
                                       const ROVehicle &veh) const = 0;

    /** @brief Adds an alternative to the list of routes
    *
     * (This may be the new route) */
    virtual void addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin) = 0;

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
    const std::string &getColor() const;

protected:
    /// The color the route shall have
    std::string myColor;

};


#endif

/****************************************************************************/

