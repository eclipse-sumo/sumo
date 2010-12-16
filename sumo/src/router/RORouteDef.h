/****************************************************************************/
/// @file    RORouteDef.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for a vehicle's route definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RORoute;
class OptionsCont;
class ROVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteDef
 * @brief Base class for a vehicle's route definition
 *
 * This class resembles what a vehicle knows about his route when being loaded
 *  into a router. Whether it is just the origin and the destination, the whole
 *  route through the network or even a route with alternatives depends on
 *  the derived class.
 */
class RORouteDef : public ReferencedItem, public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the route
     * @param[in] color The color of the route
     */
    RORouteDef(const std::string &id, const RGBColor * const color) throw();


    /// @brief Destructor
    virtual ~RORouteDef() throw();


    /** @brief Builds the complete route
     *
     * (or chooses her from the list of alternatives, when existing) */
    virtual RORoute *buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime begin,
                                       const ROVehicle &veh) const = 0;

    /** @brief Adds an alternative to the list of routes
    *
     * (This may be the new route) */
    virtual void addAlternative(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                const ROVehicle *const, RORoute *current, SUMOTime begin) = 0;

    /** @brief Returns a copy of the route definition */
    virtual RORouteDef *copy(const std::string &id) const = 0;

    /// Returns the color of the route
    const RGBColor* getColor() const {
        return myColor;
    }


    /** @brief Saves the built route / route alternatives
     *
     * Writes the route into the given stream.
     *
     * @param[in] dev The device to write the route into
     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
     * @return The same device for further usage
     */
    virtual OutputDevice &writeXMLDefinition(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
            OutputDevice &dev, const ROVehicle * const veh,
            bool asAlternatives, bool withExitTimes) const = 0;

protected:
    const RGBColor* copyColorIfGiven() const throw();

protected:
    /// The color the route shall have
    const RGBColor * const myColor;


private:
    /// @brief Invalidated copy constructor
    RORouteDef(const RORouteDef &src);

    /// @brief Invalidated assignment operator
    RORouteDef &operator=(const RORouteDef &src);

};


#endif

/****************************************************************************/

