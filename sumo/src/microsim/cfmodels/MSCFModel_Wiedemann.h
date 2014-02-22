/****************************************************************************/
/// @file    MSCFModel_Wiedemann.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    June 2011
/// @version $Id$
///
// The psycho-physical model of Wiedemann
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCFModel_Wiedemann_H
#define	MSCFModel_Wiedemann_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_Wiedemann
 * @brief The Intellignet Driver Model (IDM) car-following model
 * @see MSCFModel
 */
class MSCFModel_Wiedemann : public MSCFModel {
public:

    /** @brief Constructor
     *
     * @param[in] security The security parameter in [0,1] (dimensionless)
     * @param[in] estimation The estimation capability parameter in [0,1] (dimensionless)
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     *
     * @note other parameters of the wiedemann model:
     * - speed: included in MSVehicleType
     */
    MSCFModel_Wiedemann(const MSVehicleType* vtype,
                        SUMOReal accel, SUMOReal decel,
                        SUMOReal security, SUMOReal estimation);


    /// @brief Destructor
    ~MSCFModel_Wiedemann();


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
     * @param[in] gap The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    SUMOReal stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const;


    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @todo evaluate signature
     * @see MSCFModel::interactionGap
     */
    SUMOReal interactionGap(const MSVehicle* const , SUMOReal vL) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_WIEDEMANN;
    }


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


    VehicleVariables* createVehicleVariables() const {
        return new VehicleVariables();
    }
    /// @}


private:
    class VehicleVariables : public MSCFModel::VehicleVariables {
    public:
        VehicleVariables() : accelSign(1) {}
        /// @brief state variable for remembering the drift direction
        SUMOReal accelSign;
    };


private:
    /* @brief the main enty point for the speed computation
     * @param[in] gap The netto gap (front bumper of ego to back bumper of leader)
     */
    SUMOReal _v(const MSVehicle* veh, SUMOReal predSpeed, SUMOReal gap) const;

    /// @name acceleration based on the 'driving regime'
    /// @{
    SUMOReal fullspeed(SUMOReal v, SUMOReal vpref, SUMOReal dx, SUMOReal bx) const; // also 'WUNSCH'
    SUMOReal following(SUMOReal sign) const; // also 'FOLGEN'
    SUMOReal approaching(SUMOReal dv, SUMOReal dx, SUMOReal bx) const;  // also 'BREMSBX'
    SUMOReal emergency(SUMOReal dv, SUMOReal dx) const; // also 'BREMSAX'
    /// @}

private:
    /// @name model parameter
    /// @{

    /// @brief The driver's security parameter // also 'ZF1'
    const SUMOReal mySecurity;

    /// @brief The driver's estimation parameter // also 'ZF2'
    const SUMOReal myEstimation;

    /// @brief front-bumper to front-bumper distance
    const SUMOReal myAX;

    /// @brief perception threshold modifier
    const SUMOReal myCX;

    /// @brief The vehicle's minimum acceleration [m/s^2]
    const SUMOReal myMinAccel;

    /// @brief free-flow distance in m
    static const SUMOReal D_MAX;
    /// @}

    /// @brief vsafe from krauss since Wiedemann is deficient at approaching
    // standing obstacles (see MSCFModel_Krauss::_vsafe)
    SUMOReal krauss_vsafe(SUMOReal gap, SUMOReal predSpeed) const;

private:
    /// @brief Invalidated assignment operator
    MSCFModel_Wiedemann& operator=(const MSCFModel_Wiedemann& s);
};

#endif	/* MSCFModel_Wiedemann_H */
