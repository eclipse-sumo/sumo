/****************************************************************************/
/// @file    RORouteDef_OrigDest.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A route where only the origin and the destination edges are known
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
#ifndef RORouteDef_OrigDest_h
#define RORouteDef_OrigDest_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "RORouteDef.h"
#include <utils/common/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RORoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDef_OrigDest
 * A route definition where only the begin and the end edge are given.
 */
class RORouteDef_OrigDest
            : public RORouteDef
{
public:
    /// Constructor
    RORouteDef_OrigDest(const std::string &id, const RGBColor &color,
                        const ROEdge *from, const ROEdge *to, bool removeFirst=false) throw();

    /// Destructor
    virtual ~RORouteDef_OrigDest() throw();

    /// Returns the begin of the trip
    const ROEdge * const getFrom() const;

    /// Returns the end of the trip
    const ROEdge * const getTo() const;

    /// Builds the current route from the given information (perform routing, here)
    RORoute *buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime begin,
                               const ROVehicle &veh) const;

    /** @brief Adds the build route to the container
    *
     * Here, the currently new route is added */
    void addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin);

    /** @brief Returns a copy of the route definition */
    RORouteDef *copy(const std::string &id) const;

    const std::vector<const ROEdge*> &getCurrentEdgeVector() const;

protected:
    /// The origin and the destination edge of the route
    const ROEdge *myFrom, *myTo;

    /// The complete route (after building)
    RORoute *myCurrent;

    /// The begin of the trip
    SUMOTime myStartTime;

    /** @brief Information whether the first edge shall be removed
        */
    bool myRemoveFirst;

};


#endif

/****************************************************************************/

