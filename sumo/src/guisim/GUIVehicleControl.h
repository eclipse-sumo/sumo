/****************************************************************************/
/// @file    GUIVehicleControl.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2003-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIVehicleControl_h
#define GUIVehicleControl_h


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
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MFXMutex;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVehicleControl
 * @brief The class responsible for building and deletion of vehicles (gui-version)
 *
 * Builds GUIVehicle instances instead of MSVehicle.
 *
 * @see MSVehicleControl
 * @todo This is partially unsecure due to concurrent access...
 * @todo Recheck vehicle deletion
 */
class GUIVehicleControl : public MSVehicleControl {
public:
    /// @brief Constructor
    GUIVehicleControl();


    /// @brief Destructor
    ~GUIVehicleControl();


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
     * @return The built vehicle (GUIVehicle instance)
     * @see MSVehicleControl::buildVehicle
     */
    SUMOVehicle* buildVehicle(SUMOVehicleParameter* defs,
                              const MSRoute* route, const MSVehicleType* type,
                              const bool ignoreStopErrors, const bool fromRouteFile = true);
    /// @}


    /** @brief Tries to insert the vehicle into the internal vehicle container
     *
     * Identical to the MSVehicleControl implementation except for locking.
     *
     * @param[in] id The id of the vehicle
     * @param[in] v The vehicle
     * @return Whether the vehicle could be inserted (no other vehicle with the same id was inserted before)
     */
    bool addVehicle(const std::string& id, SUMOVehicle* v);


    /** @brief Deletes the vehicle
     *
     * Identical to the MSVehicleControl implementation except for locking.
     *
     * @param[in] v The vehicle to delete
     * @param[discard] Whether the vehicle is discard during loading (scale < 1)
     */
    void deleteVehicle(SUMOVehicle* v, bool discard = false);


    /** @brief Returns the list of all known vehicles by gl-id
     * @param[fill] into The list to fill with vehicle ids
     * @todo Well, what about concurrent modifications?
     */
    void insertVehicleIDs(std::vector<GUIGlID>& into);


    /// @brief lock access to vehicle removal/additions for thread synchronization
    void secureVehicles();

    /// @brief unlock access to vehicle removal/additions for thread synchronization
    void releaseVehicles();


private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;


private:
    /// @brief invalidated copy constructor
    GUIVehicleControl(const GUIVehicleControl& s);

    /// @brief invalidated assignment operator
    GUIVehicleControl& operator=(const GUIVehicleControl& s);


};


#endif

/****************************************************************************/

