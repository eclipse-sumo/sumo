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
#ifndef MSCFModel_h
#define MSCFModel_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <math.h>
#include <string>
#include <utils/common/StdDefs.h>
#include <utils/common/FileHelpers.h>

#define INVALID_SPEED 299792458 + 1 // nothing can go faster than the speed of light!

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
    public:
        virtual ~VehicleVariables();
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
     *  velocity at the end of the free range. If onInsertion is true, the vehicle may still brake
     *  before the next movement.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] seen The look ahead distance
     * @param[in] maxSpeed The maximum allowed speed
     * @param[in] onInsertion whether speed at insertion is asked for
     * @return EGO's safe speed
     */
    virtual SUMOReal freeSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal seen,
                               SUMOReal maxSpeed, const bool onInsertion = false) const;


    /** @brief Computes the vehicle's follow speed (no dawdling)
     *
     * Returns the velocity of the vehicle in dependence to the vehicle's and its leader's values and the distance between them.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     */
    virtual SUMOReal followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const = 0;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * This method is used during the insertion stage. Whereas the method
     * followSpeed returns the desired speed which may be lower than the safe
     * speed, this method only considers safety constraints
     *
     * Returns the velocity of the vehicle in dependence to the vehicle's and its leader's values and the distance between them.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     */
    virtual SUMOReal insertionFollowSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     *
     * Returns the velocity of the vehicle when approaching a static object (such as the end of a lane) assuming no reaction time is needed.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @todo generic Interface, models can call for the values they need
     */
    virtual SUMOReal stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const = 0;


    /** @brief Computes the vehicle's safe speed for approaching an obstacle at insertion without constraints
     *         due to acceleration capabilities and previous speeds.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle at insertion
     * @see stopSpeed() and insertionFollowSpeed()
     *
     */
    virtual SUMOReal insertionStopSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap) const;


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


    /** @brief Returns the minimum speed given the current speed
     * (depends on the numerical update scheme and its step width)
     * Note that it wouldn't have to depend on the numerical update
     * scheme if the semantics would rely on acceleration instead of velocity.
     *
     * @param[in] speed The vehicle's current speed
     * @param[in] speed The vehicle itself, for obtaining other values, if needed as e.g. road conditions.
     * @return The minimum possible speed for the next step
     */
    SUMOReal minNextSpeed(SUMOReal speed, const MSVehicle* const veh = 0) const;


    /** @brief Returns the distance the vehicle needs to halt including driver's reaction time,
     * assuming that during the reaction time, the speed remains constant
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     */
    inline SUMOReal brakeGap(const SUMOReal speed) const {
        return brakeGap(speed, myDecel, myHeadwayTime);
    }

    static SUMOReal brakeGap(const SUMOReal speed, const SUMOReal decel, const SUMOReal headwayTime);

    static SUMOReal freeSpeed(const SUMOReal currentSpeed, const SUMOReal decel, const SUMOReal dist, const SUMOReal maxSpeed, const bool onInsertion);

    /** @brief Returns the minimum gap to reserve if the leader is braking at maximum (>=0)
      * @param[in] speed EGO's speed
      * @param[in] leaderSpeed LEADER's speed
      * @param[in] leaderMaxDecel LEADER's max. deceleration rate
      */
    inline SUMOReal getSecureGap(const SUMOReal speed, const SUMOReal leaderSpeed, const SUMOReal leaderMaxDecel) const {
        // The solution approach leaderBrakeGap >= followerBrakeGap is not
        // secure when the follower can brake harder than the leader because the paths may still cross.
        // As a workaround we lower the value of followerDecel which errs on the side of caution
        //
        // xxx (Leo, refs #2548) This is somewhat different from the approach in maximumSafeFollowSpeed, where
        // the leaderMaxDecel is increased instead. This is no perfect estimate either,
        // but without taking into account the reaction time it is less conservative than decreasing followDecel.
        // Consider replacement by 'const leaderMaxDecel = MAX2(myDecel, leaderMaxDecel);' below and 'followDecel = myDecel;'
        // With maximumSafeSpeed = maximumSafeFollowSpeed(*secureGap*, speed, leaderSpeed, leaderMaxDecel) we should have:
        //        assert(maximumSafeSpeed <= speed + NUMERICAL_EPS && maximumSafeSpeed >= speed - NUMERICAL_EPS);

        // XXX: this should fix #2548 (postponed after merge of branch {ticket860}):
        //        const SUMOReal maxDecel = MAX2(myDecel, leaderMaxDecel);
        //        SUMOReal secureGap = MAX2((SUMOReal) 0, brakeGap(speed, myDecel, myHeadwayTime) - brakeGap(leaderSpeed, maxDecel, 0));

        const SUMOReal followDecel = MIN2(myDecel, leaderMaxDecel);
        // XXX: returning 0 can be wrong if the leader is slower than the follower! Why not return negative values? (Leo)
        SUMOReal secureGap = MAX2((SUMOReal) 0, brakeGap(speed, followDecel, myHeadwayTime) - brakeGap(leaderSpeed, leaderMaxDecel, 0));
        return secureGap;
    }

    /** @brief Returns the velocity after maximum deceleration
     * @param[in] v The velocity
     * @return The velocity after maximum deceleration
     */
    inline SUMOReal getSpeedAfterMaxDecel(SUMOReal v) const {
        return MAX2((SUMOReal) 0, v - (SUMOReal) ACCEL2SPEED(myDecel));
    }
    /// @}

    /** @brief Computes the minimal time needed to cover a distance given the desired speed at arrival.
     * @param[in] dist Distance to be covered
     * @param[in] currentSpeed Actual speed of vehicle
     * @param[in] arrivalSpeed Desired speed at arrival
     */
    SUMOTime getMinimalArrivalTime(SUMOReal dist, SUMOReal currentSpeed, SUMOReal arrivalSpeed) const;


    /** @brief Computes the minimal possible arrival speed after covering a given distance
     * @param[in] dist Distance to be covered
     * @param[in] currentSpeed Actual speed of vehicle
     */
    SUMOReal getMinimalArrivalSpeed(SUMOReal dist, SUMOReal currentSpeed) const;

    /** @brief Computes the minimal possible arrival speed after covering a given distance for Euler update
     * @param[in] dist Distance to be covered
     * @param[in] currentSpeed Actual speed of vehicle
     */
    SUMOReal getMinimalArrivalSpeedEuler(SUMOReal dist, SUMOReal currentSpeed) const;


    /** @brief return the resulting gap if, starting with gap currentGap, two vehicles
     * continue with constant accelerations (velocities bounded by 0 and maxSpeed) for
     * a given timespan of length 'duration'.
     * @param[in] currentGap (pos(veh1) - pos(veh2) at start)
     * @param[in] v1 initial speed of vehicle 1
     * @param[in] v2 initial speed of vehicle 2
     * @param[in] a1 acceleration of vehicle 1
     * @param[in] a2 acceleration of vehicle 2
     * @param[in] maxV1 maximal speed of vehicle 1
     * @param[in] maxV2 maximal speed of vehicle 2
     * @param[in] duration time span for the process
     * @return estimated gap after 'duration' seconds
     */
    static SUMOReal gapExtrapolation(const SUMOReal duration, const SUMOReal currentGap, SUMOReal v1,  SUMOReal v2, SUMOReal a1 = 0, SUMOReal a2 = 0, const SUMOReal maxV1 = std::numeric_limits<SUMOReal>::max(), const SUMOReal maxV2 = std::numeric_limits<SUMOReal>::max());

    /**
     * @brief Calculates the time at which the position passedPosition has been passed
     *         In case of a ballistic update, the possibility of a stop within a time step
     *         requires more information about the last time-step than in case of the euler update
     *         to determine the last position if the currentSpeed is zero.
     * @param[in] lastPos the position at time t=0 (must be < currentPos)
     * @param[in] passedPos the position for which the passing time is to be determined (has to lie within [lastPos, currentPos]!)
     * @param[in] currentPos the position at time t=TS (one time-step after lastPos) (must be > lastPos)
     * @param[in] lastSpeed the speed at moment t=0
     * @param[in] currentSpeed the speed at moment t=TS
     * @return  time t in [0,TS] at which passedPos in [lastPos, currentPos] was passed.
     */
    static SUMOReal passingTime(const SUMOReal lastPos, const SUMOReal passedPos, const SUMOReal currentPos, const SUMOReal lastSpeed, const SUMOReal currentSpeed);



    /**
     * @brief Calculates the speed after a time t \in [0,TS]
     *        given the initial speed and the distance traveled in an interval of step length TS.
     * @note  If the acceleration were known, this would be much nicer, but in this way
     *        we need to reconstruct it (for the ballistic update at least, where we assume that
     *        a stop may occur within the interval)
     * @param[in] t time in [0,TS] for which the speed shall be determined
     * @param[in] oldSpeed speed before the last time step (referred to as t == 0)
     * @param[in] distance covered
     * @return    speed at time t
     */
    static SUMOReal speedAfterTime(const SUMOReal t, const SUMOReal oldSpeed, const SUMOReal dist);





    /* @brief estimate speed while accelerating for the given distance
     * @param[in] dist The distance during which accelerating takes place
     * @param[in] v The initial speed
     * @param[in] accel The acceleration
     * XXX affected by ticket #860 (the formula is invalid for the Euler position update rule)
     * XXX (Leo) Migrated estimateSpeedAfterDistance() to MSCFModel from MSVehicle as Jakob suggested (removed inline property, because myType is fw-declared)
     */
    SUMOReal estimateSpeedAfterDistance(const SUMOReal dist, const SUMOReal v, const SUMOReal accel) const;

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

    /** @brief Returns the maximum safe velocity for following the given leader
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] egoSpeed The FOLLOWERS's speed
     * @param[in] predSpeed The LEADER's speed
     * @param[in] predMaxDecel The LEADER's maximum deceleration
     * @param[in] onInsertion Indicator whether the call is triggered during vehicle insertion
     * @return the safe velocity
     */
    SUMOReal maximumSafeFollowSpeed(SUMOReal gap,  SUMOReal egoSpeed, SUMOReal predSpeed, SUMOReal predMaxDecel, bool onInsertion = false) const;


    /** @brief Returns the maximum next velocity for stopping within gap
     * @param[in] gap The (netto) distance to the desired stopping point
     * @param[in] currentSpeed The current speed of the ego vehicle
     * @param[in] onInsertion Indicator whether the call is triggered during vehicle insertion
     * @param[in] headway The desired time headway to be included in the calculations (default argument -1 induces the use of myHeadway)
     */
    SUMOReal maximumSafeStopSpeed(SUMOReal gap, SUMOReal currentSpeed, bool onInsertion = false, SUMOReal headway = -1) const;


    /** @brief Returns the maximum next velocity for stopping within gap
     * when using the semi-implicit Euler update
     * @param[in] gap The (netto) distance to the LEADER
     */
    SUMOReal maximumSafeStopSpeedEuler(SUMOReal gap) const;


    /** @brief Returns the maximum next velocity for stopping within gap
     * when using the ballistic positional update.
     * @note This takes into account the driver's reaction time and the car's current speed.
     * (The latter is required to calculate the distance covered in the following timestep.)
     * @param[in] gap The (netto) distance to the desired stopping point
     * @param[in] currentSpeed The current speed of the ego vehicle
     * @param[in] onInsertion Indicator whether the call is triggered during vehicle insertion
     * @param[in] headway The desired time headway to be included in the calculations (default argument -1 induces the use of myHeadway)
     * @return the safe velocity (to be attained at the end of the following time step) that assures the possibility of stopping within gap.
     * If a negative value is returned, the required stop has to take place before the end of the time step.
     */
    SUMOReal maximumSafeStopSpeedBallistic(SUMOReal gap, SUMOReal currentSpeed, bool onInsertion = false, SUMOReal headway = -1) const;


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


#endif /* MSCFModel_h */

