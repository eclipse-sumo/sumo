/****************************************************************************/
/// @file    GUIVehicleControl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles (gui-version)
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>



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
    GUIVehicleControl() throw();


    /// @brief Destructor
    ~GUIVehicleControl() throw();


    /// @name Vehicle creation
    /// @{

    /** @brief Builds a vehicle, increases the number of built vehicles
     *
     * Instead of a MSVehicle, a GUIVehicle is built
     *
     * @param[in] id The id of the vehicle to build
     * @param[in] route The route of this vehicle
     * @param[in] departTime The departure time of this vehicle
     * @param[in] type The type of this vehicle
     * @param[in] repNo The number of repetitions
     * @param[in] repOffset The repetition offset
     * @return The built vehicle (GUIVehicle instance)
     * @see MSVehicleControl::buildVehicle
     */
    MSVehicle *buildVehicle(SUMOVehicleParameter* defs,
                            const MSRoute* route, const MSVehicleType* type) throw();
    /// @}



    /// @name Insertion, deletion and retrieal of vehicles
    /// @{

    /** @brief Deletes the vehicle
     *
     * We are destroying this vehicle only if it is not in use
     *  with the visualization.
     *
     * @param[in] v The vehicle to delete
     * @todo Isn't this quite insecure?
     * @see MSVehicleControl::deleteVehicle
     */
    virtual void deleteVehicle(MSVehicle *v) throw();
    /// @}


    /** @brief Returns the list of all known vehicles by gl-id
     * @param[fill] into The list to fill with vehicle ids
     * @todo Well, what about concurrent modifications?
     */
    void insertVehicleIDs(std::vector<GLuint> &into) throw();


private:
    /// @brief invalidated copy constructor
    GUIVehicleControl(const GUIVehicleControl &s);

    /// @brief invalidated assignment operator
    GUIVehicleControl &operator=(const GUIVehicleControl &s);


};


#endif

/****************************************************************************/

