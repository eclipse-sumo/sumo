/****************************************************************************/
/// @file    RORunningVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle that has an initial speed (and possibly position)
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
#ifndef RORunningVehicle_h
#define RORunningVehicle_h


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
#include "ROVehicle.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROVehicleBuilder;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORunningVehicle
 * A vehicle which when being emitted, already has an initial speed and
 * which also has a fixed position it shall be emitted at.
 */
class RORunningVehicle : public ROVehicle
{
public:
    /// Constructor
    RORunningVehicle(ROVehicleBuilder &vb,
                     const std::string &id, RORouteDef *route,
                     SUMOTime time, ROVehicleType *type, const std::string &lane,
                     SUMOReal pos, SUMOReal speed,
					 const std::string &col, int period, int repNo);

    /// Destructor
    ~RORunningVehicle();

    /// Writes the vehicle's definition to the given stream
    void xmlOut(OutputDevice &dev) const;

    /// Returns a copy of the vehicle using a new id, departure time and route
    virtual ROVehicle *copy(ROVehicleBuilder &vb,
                            const std::string &id, unsigned int depTime,
                            RORouteDef *newRoute);

private:
    /// The lane the vehicle shall depart from
    std::string myLane;

    /// The position on the lane the vehicle shall depart from
    SUMOReal myPos;

    /// The initial speed of the vehicle
    SUMOReal mySpeed;

};


#endif

/****************************************************************************/

