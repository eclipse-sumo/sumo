/****************************************************************************/
/// @file    MSCFModel_PWag2009.h
/// @author  Daniel Krajzewicz
/// @date    03.04.2010
/// @version $Id: MSCFModel_PWag2009.h 8561 2010-04-03 13:10:33Z dkrajzew $
///
// Scalable model based on Krauﬂ by Peter Wagner
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
#ifndef MSCFModel_PWag2009_h
#define	MSCFModel_PWag2009_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSCFModel.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_PWag2009
 * @brief Scalable model based on Krauﬂ by Peter Wagner
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
    MSCFModel_PWag2009(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel, SUMOReal dawdle, SUMOReal tau) throw();


    /// @brief Destructor
    ~MSCFModel_PWag2009() throw();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] lane The lane ego is at
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    SUMOReal moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw();


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


    /** @brief Get the vehicle's maximum acceleration [m/s^2]
	 *
	 * As some models describe that a vehicle is accelerating slower the higher its
	 *  speed is, the velocity is given.
	 *
	 * @param[in] v The vehicle's velocity
	 * @return The maximum acceleration
	 */
    SUMOReal getMaxAccel(SUMOReal v) const throw() {
		return myAccel;
    }


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const throw() {
        return SUMO_TAG_CF_PWAGNER2009;
    }


    /** @brief Get the driver's reaction time [s]
     * @return The reaction time of this class' drivers in s
     */
    SUMOReal getTau() const throw() {
        return myTau;
    }
    /// @}


private:
    /** @brief Returns the next velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    SUMOReal _v(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal vmax) const throw();


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     */
    SUMOReal dawdle(SUMOReal speed) const throw();

private:
    /// @name model parameter
    /// @{

    /// @brief The vehicle's maximum acceleration [m/s^2]
    SUMOReal myAccel;

    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The driver's reaction time [s]
    SUMOReal myTau;

    /// @brief The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;

	SUMOReal myDecelDivTau;
    /// @}

};

#endif	/* MSCFModel_PWag2009_H */

