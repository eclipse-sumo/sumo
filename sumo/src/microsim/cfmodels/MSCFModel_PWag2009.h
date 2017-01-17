/****************************************************************************/
/// @file    MSCFModel_PWag2009.h
/// @author  Peter Wagner
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    03.04.2010
/// @version $Id$
///
// Scalable model based on Krauss by Peter Wagner
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCFModel_PWag2009_h
#define MSCFModel_PWag2009_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_PWag2009
 * @brief Scalable model based on Krauss by Peter Wagner
 * @see MSCFModel
 */
class MSCFModel_PWag2009 : public MSCFModel {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] dawdle The driver imperfection
     * @param[in] tau The driver's reaction time
     */
    MSCFModel_PWag2009(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel, SUMOReal dawdle,
                       SUMOReal headwayTime, SUMOReal tauLast, SUMOReal apProb);


    /// @brief Destructor
    ~MSCFModel_PWag2009();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    SUMOReal moveHelper(MSVehicle* const veh, SUMOReal vPos) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     */
    SUMOReal followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    SUMOReal stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap2pred) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_PWAGNER2009;
    }


    /** @brief Get the driver's imperfection
     * @return The imperfection of drivers of this class
     */
    SUMOReal getImperfection() const {
        return myDawdle;
    }
    /// @}



    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


    virtual MSCFModel::VehicleVariables* createVehicleVariables() const {
        VehicleVariables* ret = new VehicleVariables();
        ret->aOld = 0.0;
        return ret;
    }


private:
    class VehicleVariables : public MSCFModel::VehicleVariables {
    public:
        SUMOReal aOld;
    };

    /** @brief Returns the next velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    SUMOReal _v(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const;


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     */
    SUMOReal dawdle(SUMOReal speed) const;

private:
    /// @name model parameter
    /// @{
    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;

    /// @brief The precomputed value for myDecel/myTau
    SUMOReal myDecelDivTau;

    /// @brief The precomputed value for (minimum headway time)*myDecel
    SUMOReal myTauLastDecel;

    /// @brief The probability for any action
    SUMOReal myActionPointProbability;
    /// @}

};

#endif /* MSCFModel_PWag2009_H */
