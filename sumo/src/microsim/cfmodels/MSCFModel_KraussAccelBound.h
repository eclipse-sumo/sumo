/****************************************************************************/
/// @file    MSCFModel_KraussAccelBound.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 07 Mar 2014
/// @version $Id$
///
// Krauss car-following model, with PHEMlight-based acceleration limits
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCFModel_KraussAccelBound_h
#define	MSCFModel_KraussAccelBound_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel_Krauss.h"
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_KraussAccelBound
 * @brief Krauss car-following model, with PHEMlight-based acceleration limits
 * @see MSCFModel
 */
class MSCFModel_KraussAccelBound : public MSCFModel_Krauss {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] dawdle The driver imperfection
     * @param[in] headwayTime The driver's reaction time
     */
    MSCFModel_KraussAccelBound(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel, SUMOReal dawdle, SUMOReal headwayTime);


    /// @brief Destructor
    ~MSCFModel_KraussAccelBound();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Returns the maximum speed given the current speed
     *
     * The implementation of this method must take into account the time step
     *  duration.
     *
     * Justification: Due to air brake or other influences, the vehicle's next maximum
     *  speed may depend on the vehicle's current speed (given).
     *
     * @param[in] speed The vehicle's current speed
     * @param[in] speed The vehicle itself, for obtaining other values
     * @return The maximum possible speed for the next step
     */
    SUMOReal maxNextSpeed(SUMOReal speed, const MSVehicle* const veh) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_KRAUSS_ACCEL_BOUND;
    }
    /// @}


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


};

#endif	/* MSCFModel_KraussAccelBound_H */

