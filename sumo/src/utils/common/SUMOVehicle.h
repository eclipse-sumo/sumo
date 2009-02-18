/****************************************************************************/
/// @file    SUMOVehicle.h
/// @author  Michael Behrisch
/// @date    Tue, 17 Feb 2009
/// @version $Id$
///
// Abstract base class for vehicle representations
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
#ifndef SUMOVehicle_h
#define SUMOVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOVehicle
 * @brief Representation of a vehicle
 */
class SUMOVehicle
{
public:
    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual const std::string& getID() const throw() = 0;


    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual SUMOReal getPositionOnLane() const throw() = 0;


    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    virtual SUMOReal getSpeed() const throw() = 0;
};


#endif

/****************************************************************************/

