/****************************************************************************/
/// @file    MSCFModel_IDM.h
/// @author  Tobias Mayer
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The IDM car-following model and parameter
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
#ifndef MSCFMODEL_IDM_H
#define	MSCFMODEL_IDM_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_IDM
 * @brief The IDM car-following model and parameter
 * @see MSCFModel
 */
class MSCFModel_IDM : public MSCFModel {
public:
    /** @brief Constructor
     *  @param[in] dawdle
     *  @param[in] timeHeadWay
     *  @param[in] mingap
     */
    MSCFModel_IDM(const MSVehicleType* vtype, SUMOReal dawdle,
                  SUMOReal timeHeadWay, SUMOReal mingap) throw();


    /// @brief Destructor
    ~MSCFModel_IDM() throw();


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


    /** @brief Returns the maximum speed given the current speed
     *
     * The implementation of this method must take into account the time step
     *  duration.
     *
     * Justification: Due to air brake or other influences, the vehicle's next maximum
     *  speed depends on his current speed (given).
     *
     * @param[in] speed The vehicle's current speed
     * @return The maximum possible speed for the next step
     * @see MSCFModel::maxNextSpeed
     */
    SUMOReal maxNextSpeed(SUMOReal speed) const throw();


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
     * @todo evaluate signature
     * @see MSCFModel::interactionGap
     */
    SUMOReal interactionGap(const MSVehicle * const , SUMOReal vL) const throw();


    /** @brief Returns whether the given gap is safe
     *
     * "safe" means that no collision occur when using the gap, given other values.
     * @param[in] speed EGO's speed
     * @param[in] gap The (netto) gap between LEADER and EGO
     * @param[in] predSpeed LEADER's speed
     * @param[in] laneMaxSpeed The maximum velocity allowed on the lane
     * @return Whether the given gap is safe
     * @todo evaluate signature
     * @see MSCFModel::hasSafeGap
     */
    bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const throw();


    /** @brief Returns the vehicle's maximum deceleration ability
     * @return The vehicle's maximum deceleration ability
     * @see MSCFModel::decelAbility
     */
    SUMOReal decelAbility() const throw();


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const throw() {
        return SUMO_TAG_CF_IDM;
    }
    /// @}


private:
    SUMOReal _updateSpeed(SUMOReal gap2pred, SUMOReal mySpeed, SUMOReal predSpeed, SUMOReal desSpeed) const throw();

	SUMOReal desiredSpeed(const MSVehicle * const veh) const throw() {
		return MIN2(myType->getMaxSpeed(), veh->getLane().getMaxSpeed());
	}


    /// @brief The driver's desired time headway [s]
    SUMOReal myTimeHeadWay;

    /// @brief The desired minimum Gap to the leading vehicle (no matter the speed)
    SUMOReal myMinSpace;

    /// The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;

    /// @}
};

#endif	/* MSCFMODEL_IDM_H */
