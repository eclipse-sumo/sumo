/****************************************************************************/
/// @file    MSCFModel_KraussX.h
/// @author  Jakob Erdmann
/// @date    27 Feb 2017
/// @version $Id$
///
// Experimental extensions to the Krauss car-following model
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
#ifndef MSCFModel_KraussX_h
#define MSCFModel_KraussX_h

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
/** @class MSCFModel_KraussX
 * @brief Krauss car-following model, changing accel and speed by slope
 * @see MSCFModel
 * @see MSCFModel_Krauss
 */
class MSCFModel_KraussX : public MSCFModel_Krauss {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] dawdle The driver imperfection
     * @param[in] emergencyDecel The maximum emergency deceleration
     * @param[in] apparentDecel The deceleration as expected by others
     * @param[in] headwayTime The driver's reaction time
     */
    MSCFModel_KraussX(const MSVehicleType* vtype, double accel, double decel,
                      double emergencyDecel, double apparentDecel,
                      double dawdle, double headwayTime,
                      double tmp1, double tmp2);


    /// @brief Destructor
    ~MSCFModel_KraussX();


    /// @name Implementations of the MSCFModel interface
    /// @{
    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     *
     * @note: this is the exact duplicate of MSCFModel_KraussOrig1::moveHelper (also used by Krauss) except that dawdle is called with a second paramter
     */
    double moveHelper(MSVehicle* const veh, double vPos) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_KRAUSSX;
    }
    /// @}


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;

private:


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] vOld The previous speed
     * @param[in] vMin The minimum speed (due to braking constraints)
     * @param[in] vMax The maximum speed that may be driven (all constraints)
     * @return The speed after dawdling
     *
     */
    double dawdleX(double vOld, double vMin, double vMax) const;

    /// @brief extension parameter nr1
    double myTmp1;
    double myTmp2;

};

#endif /* MSCFModel_KraussX_H */

