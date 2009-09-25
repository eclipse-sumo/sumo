/****************************************************************************/
/// @file    MSCFModel_Krauss.h
/// @author  Tobias Mayer
/// @date    Tue, 28 Jul 2009
/// @version $Id$
///
// The Krauss car-following model and parameter
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
     *  @param[in] dawdle The driver imperfection
     *  @param[in] tau The driver's reaction time
     */
    MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal dawdle, SUMOReal tau) throw();


    /// @brief Destructor
    ~MSCFModel_Krauss() throw();



    /// @name Implementations of the MSCFModel interface
    /// @{

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


    /** @brief Returns the distance the vehicle needs to halt excluding driver's reaction time
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     * @see MSCFModel::approachingBrakeGap
     */
    SUMOReal approachingBrakeGap(SUMOReal speed) const throw();


    /** @brief Returns the maximum gap at which an interaction between both vehicles occures
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @see MSCFModel::interactionGap
     * @todo evaluate signature
     */
    SUMOReal interactionGap(MSVehicle *veh, SUMOReal vL) const throw();


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


    /** @brief Returns the gap needed to allow a safe emission
     * @param[in] speed The assumed speed
     * @return The gap needed for allowing an emission
     * @see MSCFModel::safeEmitGap
     */
    SUMOReal safeEmitGap(SUMOReal speed) const throw();


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     * @see MSCFModel::dawdle
     * @todo must exist until MSVehicle::move() is updated
     */
    SUMOReal dawdle(SUMOReal speed) const throw();


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
        return SUMO_TAG_CF_KRAUSS;
    }
    /// @}


private:
    /** @brief Returns the "safe" velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    SUMOReal _vsafe(SUMOReal gap2pred, SUMOReal predSpeed) const throw();


private:
    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The driver's reaction time [s]
    SUMOReal myTau;

    /// The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;

    /// The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;
    /// @}

};

#endif	/* MSCFMODEL_KRAUSS_H */

