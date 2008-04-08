/****************************************************************************/
/// @file    RORouteDef_Alternatives.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A route with alternative routes
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
class RORouteDef_Alternatives : public RORouteDef
{
public:
    /// Constructor
    RORouteDef_Alternatives(const std::string &id, const RGBColor &color,
                            size_t lastUsed, SUMOReal gawronBeta, SUMOReal gawronA, int maxRoutes) throw();

    /// Destructor
    virtual ~RORouteDef_Alternatives() throw();

    /** @brief Adds an alternative loaded from the file
        An alternative may also be generated whicle DUA */
    virtual void addLoadedAlternative(RORoute *alternative);

    /// Returns the trip's origin edge
    const ROEdge * const getFrom() const;

    /// returns the trip's destination edge
    const ROEdge * const getTo() const;

    /// Build the next route
    RORoute *buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime begin,
                               const ROVehicle &veh) const;

    /// Adds a build alternative
    void addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin);

    /** @brief Returns a copy of the route definition */
    RORouteDef *copy(const std::string &id) const;

    const std::vector<const ROEdge*> &getCurrentEdgeVector() const;

    void invalidateLast();

    void addExplicite(const ROVehicle *const veh, RORoute *current, SUMOTime begin);

    void removeLast();

    /** @brief returns the index of the route that was used as last */
    virtual int getLastUsedIndex() const;

    /** @brief returns the number of alternatives */
    virtual size_t getAlternativesSize() const;

    /// Returns the alternative at the given index
    virtual const RORoute &getAlternative(size_t i) const;

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

};


#endif

/****************************************************************************/

