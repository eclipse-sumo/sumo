/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCFModel_TCI.h
/// @author  Leonhard Luecken
/// @date    Tue, 5 Feb 2018
/// @version $Id$
///
// Task Capability Interface car-following model.
/****************************************************************************/
#ifndef MSCFModel_TCI_h
#define MSCFModel_TCI_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <memory>
#include <functional>
#include "MSCFModel_Krauss.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrafficItem;


// ===========================================================================
// class definitions
// ===========================================================================

/** @class MSCFModel_TCI
 * @brief Task Capability Interface car-following model.
 * @see MSCFModel
 */
class MSCFModel_TCI : public MSCFModel_Krauss {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] emergencyDecel The maximum emergency deceleration
     * @param[in] apparentDecel The deceleration as expected by others
     * @param[in] dawdle The driver imperfection
     * @param[in] headwayTime The driver's desired headway
     */
    MSCFModel_TCI(const MSVehicleType* vtype, double accel, double decel,
                     double emergencyDecel, double apparentDecel, double headwayTime);


    /// @brief Destructor
    ~MSCFModel_TCI();

    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * this uses the maximumSafeStopSpeed
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @todo generic Interface, models can call for the values they need
     */
    double stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * this uses the maximumSafeFollowSpeed
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     */
    double followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_TCI;
    }

    /// @}


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;

};


#endif /* MSCFModel_TCI_H */

