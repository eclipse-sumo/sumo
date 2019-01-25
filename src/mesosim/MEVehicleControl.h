/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MEVehicleControl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (meso-version)
/****************************************************************************/
#ifndef MEVehicleControl_h
#define MEVehicleControl_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
                                      const MSRoute* route, MSVehicleType* type,
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

