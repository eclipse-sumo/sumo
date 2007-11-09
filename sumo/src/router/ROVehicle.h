/****************************************************************************/
/// @file    ROVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single vehicle
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
#ifndef ROVehicle_h
#define ROVehicle_h


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
#include <utils/common/RGBColor.h>
#include "ROVehicleBuilder.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROVehicleType;
class RORouteDef;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicle
 * @brief A single vehicle holding information about his type,
 *  departure time and the route he takes.
 */
class ROVehicle
{
public:
    /// Constructor
    ROVehicle(ROVehicleBuilder &vb,
              const std::string &id, RORouteDef *route, unsigned int depart,
              ROVehicleType *type, const RGBColor &color, int period, int repNo);

    /// Destructor
    virtual ~ROVehicle();

    /// Returns the route the vehicle takes
    RORouteDef * const getRoute() const;

    /// Returns the type of the vehicle
    const ROVehicleType * const getType() const;

    /// Returns the id of the vehicle
    const std::string &getID() const;

    /// Returns the time the vehicle starts his route
    SUMOTime getDepartureTime() const;

    /** @brief Saves the complete vehicle description.
    *
    * Saves the vehicle type if it was not saved before.
    * Saves the vehicle route if it was not saved before.
    * Saves the vehicle itself.
    * Use this method polymorph if no route alternatives shall be generated */
    void saveAllAsXML(OutputDevice &os, OutputDevice * const altos,
                      const RORouteDef * const route) const;

    /// Returns a copy of the vehicle using a new id, departure time and route
    virtual ROVehicle *copy(ROVehicleBuilder &vb,
                            const std::string &id, unsigned int depTime, RORouteDef *newRoute);


protected:
    /// Saves the vehicle definition only into the given stream
    void saveXMLVehicle(OutputDevice &dev) const;

protected:
    /// The name of the vehicle
    std::string myID;

    /// The color of the vehicle
    RGBColor myColor;

    /// The type of the vehicle
    ROVehicleType *myType;

    /// The route the vehicle takes
    RORouteDef *myRoute;

    /// The time the vehicle shall be emitted at
    unsigned int myDepartTime;

    /// The repetition period (-1 if only one vehicle shall be emitted)
    int myRepetitionPeriod;

    /// The number of times such vehicles shall be emitted
    int myRepetitionNumber;

};


#endif

/****************************************************************************/

