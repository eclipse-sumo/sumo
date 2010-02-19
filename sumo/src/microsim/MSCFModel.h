/****************************************************************************/
/// @file    MSCFModel.h
/// @author  Tobias Mayer
/// @date    Mon, 27 Jul 2009
/// @version $Id $
///
// The car-following model abstraction
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
#ifndef MSCFModel_h
#define	MSCFModel_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <string>
#include <utils/common/FileHelpers.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class MSVehicle;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCFModel
 * @brief The car-following model abstraction
 *
 * MSCFModel is an interface for different car following Models to implement.
 *
 * It provides methods to compute a vehicles velocity for a simulation step.
 */
class MSCFModel {
public:
    /** @brief Constructor
     *  @param[in] rvtype a reference to the corresponding vtype
     */
    MSCFModel(const MSVehicleType* vtype) throw();


    /// @brief Destructor
    virtual ~MSCFModel() throw();


    /// @name Methods to override by model implementation
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] lane The lane ego is at
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    virtual SUMOReal moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() = 0;


    /** @brief Incorporates the influence of the vehicle on the left lane
     * @param[in] ego The ego vehicle
     * @param[in] neigh The neighbor vehicle on the left lane
     * @param[in, out] vSafe Current vSafe; may be adapted due to the left neighbor
     */
    virtual void leftVehicleVsafe(const MSVehicle * const ego, const MSVehicle * const neigh, SUMOReal &vSafe) const throw() = 0;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @todo used by MSLane, can hopefully be removed eventually
     */
    virtual SUMOReal ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const throw() = 0;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @todo used by MSLCM_DK2004, allows hypothetic values of gap2pred and predSpeed
     */
    virtual SUMOReal ffeV(const MSVehicle * const veh, SUMOReal gap2pred, SUMOReal predSpeed) const throw() = 0;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] pred The LEADER
     * @return EGO's safe speed
     * @todo generic Interface, models can call for the values they need
     */
    virtual SUMOReal ffeV(const MSVehicle * const veh, const MSVehicle * const pred) const throw() = 0;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @todo generic Interface, models can call for the values they need
     */
    virtual SUMOReal ffeS(const MSVehicle * const veh, SUMOReal gap2pred) const throw() = 0;


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
     */
    virtual SUMOReal maxNextSpeed(SUMOReal speed) const throw() = 0;


    /** @brief Returns the distance the vehicle needs to halt including driver's reaction time
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     */
    virtual SUMOReal brakeGap(SUMOReal speed) const throw() = 0;


    /** @brief Returns the distance the vehicle needs to halt excluding driver's reaction time
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     */
    virtual SUMOReal approachingBrakeGap(SUMOReal speed) const throw() = 0;


    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @todo evaluate signature
     */
    virtual SUMOReal interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() = 0;


    /** @brief Returns whether the given gap is safe
     *
     * "safe" means that no collision occur when using the gap, given other values.
     * @param[in] speed EGO's speed
     * @param[in] gap The (netto) gap between LEADER and EGO
     * @param[in] predSpeed LEADER's speed
     * @param[in] laneMaxSpeed The maximum velocity allowed on the lane
     * @return Whether the given gap is safe
     * @todo evaluate signature
     */
    virtual bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const throw() = 0;


    /** @brief Returns the gap needed to allow a safe emission
     * @param[in] speed The assumed speed
     * @return The gap needed for allowing an emission
     */
    virtual SUMOReal safeEmitGap(SUMOReal speed) const throw() = 0;


    /** @brief Returns the vehicle's maximum deceleration ability
     * @return The vehicle's maximum deceleration ability
     */
    virtual SUMOReal decelAbility() const throw() = 0;


    /** @brief Saves the model's definition into the state
     * @param[in] os The output to write the definition into
     */
    virtual void saveState(std::ostream &os);


    /** @brief Returns the model's ID
     * @return The model's ID
     */
    virtual int getModelID() const throw() = 0;
    /// @}


    /** @brief Returns the distance the vehicle drives at the given speed
     * @param[in] speed The vehicle's speed
     * @return The distance the vehicle drives at the given speed
     * @todo needed?
     */
    SUMOReal timeHeadWayGap(SUMOReal speed) const throw() {
        assert(speed >= 0);
        return SPEED2DIST(speed);
    }


protected:
    /// @brief The type to which this model definition belongs to
    const MSVehicleType* myType;

};


#endif	/* MSCFModel_h */

