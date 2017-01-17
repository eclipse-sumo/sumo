/****************************************************************************/
/// @file    MEVehicleControl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (meso-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MEVehicleControl_h
#define MEVehicleControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <microsim/MSVehicleControl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MEVehicleControl
 * @brief The class responsible for building and deletion of vehicles (gui-version)
 *
 * Builds GUIVehicle instances instead of MSVehicle.
 *
 * @see MSVehicleControl
 * @todo This is partially unsecure due to concurrent access...
 * @todo Recheck vehicle deletion
 */
class MEVehicleControl : public MSVehicleControl {
public:
    /// @brief Constructor
    MEVehicleControl();


    /// @brief Destructor
    ~MEVehicleControl();


    /// @name Vehicle creation
    /// @{

    /** @brief Builds a vehicle, increases the number of built vehicles
     *
     * Instead of a MSVehicle, a GUIVehicle is built
     *
     * @param[in] defs The parameter defining the vehicle
     * @param[in] route The route of this vehicle
     * @param[in] type The type of this vehicle
     * @param[in] ignoreStopErrors whether invalid stops trigger a warning only
     * @param[in] fromRouteFile whether we are just reading the route file or creating via trigger, traci, ...
     * @return The built vehicle (MEVehicle instance)
     * @see MSVehicleControl::buildVehicle
     */
    virtual SUMOVehicle* buildVehicle(SUMOVehicleParameter* defs,
                                      const MSRoute* route, const MSVehicleType* type,
                                      const bool ignoreStopErrors, const bool fromRouteFile = true);
    /// @}


private:
    /// @brief invalidated copy constructor
    MEVehicleControl(const MEVehicleControl& s);

    /// @brief invalidated assignment operator
    MEVehicleControl& operator=(const MEVehicleControl& s);


};


#endif

/****************************************************************************/

