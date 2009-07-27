/****************************************************************************/
/// @file    RORouteDef_Alternatives.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A route with alternative routes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RORouteDef_Alternatives_h
#define RORouteDef_Alternatives_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include "RORouteDef.h"
#include <utils/common/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDef_Alternatives
 * @brief A route definition which has some alternatives, already.
 */
class RORouteDef_Alternatives : public RORouteDef {
public:
    /// Constructor
    RORouteDef_Alternatives(const std::string &id,
                            unsigned int lastUsed, SUMOReal gawronBeta, SUMOReal gawronA, int maxRoutes) throw();

    /// Destructor
    virtual ~RORouteDef_Alternatives() throw();

    /** @brief Adds an alternative loaded from the file
        An alternative may also be generated whicle DUA */
    virtual void addLoadedAlternative(RORoute *alternative);

    /// Build the next route
    RORoute *buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime begin,
                               const ROVehicle &veh) const;

    /// Adds a build alternative
    void addAlternative(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                        const ROVehicle *const, RORoute *current, SUMOTime begin);

    /** @brief Returns a copy of the route definition */
    RORouteDef *copy(const std::string &id) const;

    void invalidateLast();

    void removeLast();

    virtual OutputDevice &writeXMLDefinition(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
            OutputDevice &dev, const ROVehicle * const veh,
            bool asAlternatives, bool withExitTimes) const;

private:
    /// Searches for the route within the list of alternatives
    int findRoute(RORoute *opt) const;

    /** @brief Performs the gawron - f() function
        From "Dynamic User Equilibria..." */
    SUMOReal gawronF(SUMOReal pdr, SUMOReal pds, SUMOReal x);

    /** @brief Performs the gawron - g() function
        From "Dynamic User Equilibria..." */
    SUMOReal gawronG(SUMOReal a, SUMOReal x);

private:
    /// Information whether a new route was generated
    mutable bool myNewRoute;

    /// Index of the route used within the last step
    mutable int myLastUsed;

    /// Definition of the storage for alternatives
    typedef std::vector<RORoute*> AlternativesVector;

    /// The alternatives
    AlternativesVector myAlternatives;

    /// gawron beta - value
    SUMOReal myGawronBeta;

    /// gawron a - value
    SUMOReal myGawronA;

    /// The maximum route number
    int myMaxRouteNumber;


private:
    /// @brief Invalidated copy constructor
    RORouteDef_Alternatives(const RORouteDef_Alternatives &src);

    /// @brief Invalidated assignment operator
    RORouteDef_Alternatives &operator=(const RORouteDef_Alternatives &src);

};


#endif

/****************************************************************************/

