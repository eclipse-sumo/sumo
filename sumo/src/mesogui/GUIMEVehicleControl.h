/****************************************************************************/
/// @file    GUIMEVehicleControl.h
/// @author  Jakob Erdmann
/// @date    Okt 2012
/// @version $Id$
///
// The class responsible for building and deletion of meso vehicles (gui-version)
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
#ifndef GUIMEVehicleControl_h
#define GUIMEVehicleControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/foxtools/MFXMutex.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <mesosim/MEVehicleControl.h>


// ===========================================================================
// class declarations
// ===========================================================================

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIMEVehicleControl
 * @brief The class responsible for building and deletion of vehicles (gui-version)
 *
 * @see MEVehicleControl
 */
class GUIMEVehicleControl : public MEVehicleControl {
public:
    /// @brief Constructor
    GUIMEVehicleControl() ;


    /// @brief Destructor
    ~GUIMEVehicleControl() ;

    /// @name Vehicle creation
    /// @{

    /** @brief Builds a vehicle, increases the number of built vehicles
     *
     * Instead of a MEVehicle, a GUIMEVehicle is built
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
    GUIMEVehicleControl(const GUIMEVehicleControl& s);

    /// @brief invalidated assignment operator
    GUIMEVehicleControl& operator=(const GUIMEVehicleControl& s);


};


#endif

/****************************************************************************/

