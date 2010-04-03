/****************************************************************************/
/// @file    MSCFModel_Krauss.h
/// @author  Tobias Mayer
/// @date    Tue, 28 Jul 2009
/// @version $Id$
///
// The Krauss car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCFModel_Krauss_h
#define	MSCFModel_Krauss_h

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
/** @class MSCFModel_Krauss
 * @brief The Krauss car-following model and parameter
 * @see MSCFModel
 */
class MSCFModel_Krauss : public MSCFModel {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] dawdle The driver imperfection
     * @param[in] tau The driver's reaction time
     */
    MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel, SUMOReal dawdle, SUMOReal tau) throw();


    /// @brief Destructor
    ~MSCFModel_Krauss() throw();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] lane The lane ego is at
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    SUMOReal moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw();


    /** @brief Incorporates the influence of the vehicle on the left lane
     * @param[in] ego The ego vehicle
     * @param[in] neigh The neighbor vehicle on the left lane
     * @param[in, out] vSafe Current vSafe; may be adapted due to the left neighbor
     * @see MSCFModel::leftVehicleVsafe
     */
    void leftVehicleVsafe(const MSVehicle * const ego, const MSVehicle * const neigh, SUMOReal &vSafe) const throw();


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     */
    SUMOReal ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const throw();


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     * @todo used by MSLCM_DK2004, allows hypothetic values of gap2pred and predSpeed
     */
    SUMOReal ffeV(const MSVehicle * const veh, SUMOReal gap2pred, SUMOReal predSpeed) const throw();


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] pred The LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     * @todo generic Interface, models can call for the values they need
     */
    SUMOReal ffeV(const MSVehicle * const veh, const MSVehicle * const pred) const throw();


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    SUMOReal ffeS(const MSVehicle * const veh, SUMOReal gap2pred) const throw();


    /** @brief Returns the distance the vehicle needs to halt including driver's reaction time
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     * @see MSCFModel::brakeGap
     */
    SUMOReal brakeGap(SUMOReal speed) const throw();


    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @see MSCFModel::interactionGap
     * @todo evaluate signature
     */
    SUMOReal interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw();


    /** @brief Returns whether the given gap is safe
     *
     * "safe" means that no collision occur when using the gap, given other values.
     * @param[in] speed EGO's speed
     * @param[in] gap The (netto) gap between LEADER and EGO
     * @param[in] predSpeed LEADER's speed
     * @param[in] laneMaxSpeed The maximum velocity allowed on the lane
     * @return Whether the given gap is safe
     * @see MSCFModel::hasSafeGap
     * @todo evaluate signature
     */
    bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const throw();


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const throw() {
        return SUMO_TAG_CF_KRAUSS;
    }
    /// @}


    /// Get the vehicle's maximum acceleration [m/s^2]
    SUMOReal getMaxAccel(SUMOReal v) const throw() {
		return (SUMOReal)(myAccel *(1.0 - (v/myType->getMaxSpeed())));
    }

    /** @brief Get the driver's reaction time [s]
     * @return The reaction time of this class' drivers in s
     */
    SUMOReal getTau() const throw() {
        return myTau;
    }


private:
    /** @brief Returns the "safe" velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    SUMOReal _vsafe(SUMOReal gap, SUMOReal predSpeed) const throw();


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     */
    SUMOReal dawdle(SUMOReal speed) const throw();

private:
    /// @brief The vehicle's maximum acceleration [m/s^2]
    SUMOReal myAccel;

    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The driver's reaction time [s]
    SUMOReal myTau;

    /// @brief The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;

    /// @brief The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;
    /// @}

};

#endif	/* MSCFMODEL_KRAUSS_H */

