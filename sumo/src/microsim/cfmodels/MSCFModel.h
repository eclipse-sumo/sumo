/****************************************************************************/
/// @file    MSCFModel.h
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include <utils/common/StdDefs.h>
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
 * It provides methods to compute a vehicles velocity for a simulation step.
 */
class MSCFModel {
public:

    class VehicleVariables {
    };

    /** @brief Constructor
     *  @param[in] rvtype a reference to the corresponding vtype
     */
    MSCFModel(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel, SUMOReal headwayTime);


    /// @brief Destructor
    virtual ~MSCFModel();


    /// @name Methods to override by model implementation
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    virtual SUMOReal moveHelper(MSVehicle* const veh, SUMOReal vPos) const;


    /** @brief Computes the vehicle's safe speed without a leader
     *
     * Returns the velocity of the vehicle in dependence to the length of the free street and the target
     *  velocity at the end of the free range.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] seen The look ahead distance
     * @param[in] maxSpeed The maximum allowed speed
     * @return EGO's safe speed
     */
    virtual SUMOReal freeSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal seen, SUMOReal maxSpeed) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     *
     * Returns the velocity of the vehicle in dependence to the vehicle's and its leader's values and the distance between them.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     */
    virtual SUMOReal followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const = 0;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     *
     * Returns the velocity of the vehicle when approaching a static object (such as the end of a lane) assuming no reaction time is needed.
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @todo generic Interface, models can call for the values they need
     */
    virtual SUMOReal stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap2pred) const = 0;


    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @todo evaluate signature
     */
    virtual SUMOReal interactionGap(const MSVehicle* const veh, SUMOReal vL) const;


    /** @brief Returns the model's ID; the XML-Tag number is used
     * @return The model's ID
     */
    virtual int getModelID() const = 0;


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    virtual MSCFModel* duplicate(const MSVehicleType* vtype) const = 0;


    /** @brief Returns model specific values which are stored inside a vehicle
     * and must be used with casting
     */
    virtual VehicleVariables* createVehicleVariables() const {
        return 0;
    }
    /// @}


    /** @brief Get the vehicle type's maximum acceleration [m/s^2]
     * @return The maximum acceleration (in m/s^2) of vehicles of this class
     */
    inline SUMOReal getMaxAccel() const {
        return myAccel;
    }


    /** @brief Get the vehicle type's maximum deceleration [m/s^2]
     * @return The maximum deceleration (in m/s^2) of vehicles of this class
     */
    inline SUMOReal getMaxDecel() const {
        return myDecel;
    }


    /// @name Virtual methods with default implementation
    /// @{

    /** @brief Get the driver's imperfection
     * @return The imperfection of drivers of this class
     */
    virtual SUMOReal getImperfection() const {
        return -1;
    }


    /** @brief Get the driver's reaction time [s]
     * @return The reaction time of this class' drivers in s
     */
    virtual SUMOReal getHeadwayTime() const {
        return myHeadwayTime;
    }
    /// @}



    /// @name Currently fixed methods
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
    virtual SUMOReal maxNextSpeed(SUMOReal speed, const MSVehicle* const veh) const;


    /** @brief Returns the distance the vehicle needs to halt including driver's reaction time
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     */
    inline SUMOReal brakeGap(const SUMOReal speed) const {
        return brakeGap(speed, myDecel, myHeadwayTime);
    }


    inline static SUMOReal brakeGap(const SUMOReal speed, const SUMOReal decel, const SUMOReal headwayTime) {
        /* one possiblity to speed this up is to precalculate speedReduction * steps * (steps+1) / 2
        for small values of steps (up to 10 maybe) and store them in an array */
        const SUMOReal speedReduction = ACCEL2SPEED(decel);
        const int steps = int(speed / speedReduction);
        return SPEED2DIST(steps * speed - speedReduction * steps * (steps + 1) / 2) + speed * headwayTime;
    }


    /** @brief Returns the minimum gap to reserve if the leader is braking at maximum
      * @param[in] speed EGO's speed
      * @param[in] leaderSpeed LEADER's speed
      * @param[in] leaderMaxDecel LEADER's max. deceleration rate
      */
    inline SUMOReal getSecureGap(const SUMOReal speed, const SUMOReal leaderSpeed, const SUMOReal leaderMaxDecel) const {
        // The solution approach leaderBrakeGap >= followerBrakeGap is not
        // secure when the follower can brake harder than the leader because the paths may still cross.
        // As a workaround we lower the value of followerDecel which errs on the side of caution
        const SUMOReal followDecel = MIN2(myDecel, leaderMaxDecel);
        return MAX2((SUMOReal) 0, brakeGap(speed, followDecel, myHeadwayTime) - brakeGap(leaderSpeed, leaderMaxDecel, 0));
    }


    /** @brief Returns the velocity after maximum deceleration
     * @param[in] v The velocity
     * @return The velocity after maximum deceleration
     */
    inline SUMOReal getSpeedAfterMaxDecel(SUMOReal v) const {
        return MAX2((SUMOReal) 0, v - (SUMOReal) ACCEL2SPEED(myDecel));
    }
    /// @}


    /// @name Setter methods
    /// @{

    /** @brief Sets a new value for maximum acceleration [m/s^2]
     * @param[in] accel The new acceleration in m/s^2
     */
    virtual void setMaxAccel(SUMOReal accel) {
        myAccel = accel;
    }


    /** @brief Sets a new value for maximum deceleration [m/s^2]
     * @param[in] accel The new deceleration in m/s^2
     */
    virtual void setMaxDecel(SUMOReal decel) {
        myDecel = decel;
    }


    /** @brief Sets a new value for driver imperfection
     * @param[in] accel The new driver imperfection
     */
    virtual void setImperfection(SUMOReal imperfection) {
        UNUSED_PARAMETER(imperfection);
    }


    /** @brief Sets a new value for driver reaction time [s]
     * @param[in] headwayTime The new driver reaction time (in s)
     */
    virtual void setHeadwayTime(SUMOReal headwayTime) {
        myHeadwayTime = headwayTime;
    }
    /// @}


protected:
    /// @brief The type to which this model definition belongs to
    const MSVehicleType* myType;

    /// @brief The vehicle's maximum acceleration [m/s^2]
    SUMOReal myAccel;

    /// @brief The vehicle's maximum deceleration [m/s^2]
    SUMOReal myDecel;

    /// @brief The driver's desired time headway (aka reaction time tau) [s]
    SUMOReal myHeadwayTime;
};


#endif	/* MSCFModel_h */

