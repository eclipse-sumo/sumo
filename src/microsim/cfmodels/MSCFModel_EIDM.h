/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSCFModel_EIDM.h
/// @author  Dominik Salles
/// @date    Fri, 06 Jul 2018
/// @version $Id$

/// Originalfile MSCFModel_IDM.cpp from
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The Extended Intelligent Driver Model (EIDM) car-following model
//
// Publication: Salles, Dominik, S. Kaufmann and H. Reuss. “Extending the Intelligent Driver
// Model in SUMO and Verifying the Drive Off Trajectories with Aerial
// Measurements.” (2020).
/****************************************************************************/
#ifndef MSCFMODEL_EIDM_H
#define MSCFMODEL_EIDM_H

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSCFModel.h"
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_EIDM
 * @brief The Extended Intelligent Driver Model (EIDM) car-following model
 * @see MSCFModel
 */
class MSCFModel_EIDM : public MSCFModel {
public:
    /** @brief Constructor
     *  @param[in] vtype the type for which this model is built and also the parameter object to configure this model
     */
    MSCFModel_EIDM(const MSVehicleType* vtype);

    /// @brief Destructor
    ~MSCFModel_EIDM();

    /// @name Implementations of the MSCFModel interface
    /// @{
    
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
    double insertionFollowSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel, const MSVehicle* const pred = 0) const;


    /** @brief Computes the vehicle's safe speed for approaching an obstacle at insertion without constraints
     *         due to acceleration capabilities and previous speeds.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle at insertion
     * @see stopSpeed() and insertionFollowSpeed()
     *
     */
    double insertionStopSpeed(const MSVehicle* const veh, double speed, double gap) const;


    /** @brief Returns the maximum safe velocity for following the given leader
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] egoSpeed The FOLLOWERS's speed
     * @param[in] predSpeed The LEADER's speed
     * @param[in] predMaxDecel The LEADER's maximum deceleration
     * @param[in] onInsertion Indicator whether the call is triggered during vehicle insertion
     * @return the safe velocity
     */
    double maximumSafeFollowSpeed(double gap, double egoSpeed, double predSpeed, double predMaxDecel, bool onInsertion = false) const;


    /** @brief Returns the maximum next velocity for stopping within gap
     * @param[in] gap The (netto) distance to the desired stopping point
     * @param[in] currentSpeed The current speed of the ego vehicle
     * @param[in] onInsertion Indicator whether the call is triggered during vehicle insertion
     * @param[in] headway The desired time headway to be included in the calculations (default argument -1 induces the use of myHeadway)
     */
    double maximumSafeStopSpeed(double gap, double decel, double currentSpeed, bool onInsertion = false, double headway = -1) const;


    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    double finalizeSpeed(MSVehicle* const veh, double vPos) const;


    /** @brief Computes the vehicle's safe speed
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     */
    double followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel, const MSVehicle* const pred = 0) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    double stopSpeed(const MSVehicle* const veh, const double speed, double gap, double decel) const;
    
    
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
    double freeSpeed(const MSVehicle* const veh, double speed, double seen,
                             double maxSpeed, const bool onInsertion = false) const;

    static double freeSpeed(const double currentSpeed, const double decel, const double dist, const double maxSpeed, const bool onInsertion);

    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @todo evaluate signature
     * @see MSCFModel::interactionGap
     */
    double interactionGap(const MSVehicle* const , double vL) const;

    /** @brief Returns the minimum gap to reserve if the leader is braking at maximum (>=0)
      * @param[in] speed EGO's speed
      * @param[in] leaderSpeed LEADER's speed
      * @param[in] leaderMaxDecel LEADER's max. deceleration rate
      */
    double getSecureGap(const MSVehicle* const veh, const MSVehicle* const /*pred*/, const double speed, const double leaderSpeed, const double leaderMaxDecel) const;

    /** @brief Returns the distance the vehicle needs to halt including driver's reaction time tau (i.e. desired headway),
     * assuming that during the reaction time, the speed remains constant
     * @param[in] speed The vehicle's current speed
     * @return The distance needed to halt
     */
    double brakeGap(const double speed, const double decel, const double headwayTime) const {
        if (MSGlobals::gComputeLC) {
            return MSCFModel::brakeGap(speed, decel, headwayTime);
        } else {
            // myDecel + 1.0 does not work correctly with the junction model and impatience > 0.
            // Vehicles may first be disregarded because their own arrivalTimeBraking is high (TIME2STEPS(30)).
            // This is amplified by this brakeGap-Term, because they "think" they could still brake in time (low brakeGap),
            // but actually 1.0 was only added for braking at traffic lights. But then when seen < brakeGap(v) happens (see MSVehicle: arrivalTimeBraking),
            // they realize, that they will soon arrive at the junction and other vehicles are notified to maybe then brake hard!
            return MSCFModel::brakeGap(speed, MAX2(decel, myDecel + 1.0), headwayTime);
        }
    }

    /** @brief Returns the maximum velocity the CF-model wants to achieve in the next step
     * @param[in] maxSpeed The maximum achievable speed in the next step
     * @param[in] maxSpeedLane The maximum speed the vehicle wants to drive on this lane (Speedlimit*SpeedFactor)
     */
    double maximumLaneSpeedCF(double maxSpeed, double /*maxSpeedLane*/) const {
        return maxSpeed;
    }

    /** @brief Returns the model's name
    * @return The model's name
    * @see MSCFModel::getModelName
    */
    int getModelID() const {
        return SUMO_TAG_CF_EIDM;
    }

    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;

    // @brief Variables that are stored throughout a call to the car-following functions
    VehicleVariables* createVehicleVariables() const {
        VehicleVariables* ret = new VehicleVariables();
        ret->minaccel = 100;
        ret->wouldacc = 100;
        ret->lastacc = 0;
        ret->realacc = 100;
        ret->lastrealacc = 0;
        ret->realleaderacc = 100;
        ret->lastleaderacc = 0;
        ret->v0_int = 0;
        ret->v0_old = 0;
        ret->t_off = -10.;
        ret->myw_gap = 0.;
        ret->myw_speed = 0.;
        ret->myw_error = 0.;
        ret->myv_est_l = 0.;
        ret->myv_est = 0.;
        ret->mys_est = 0.;
        ret->myap_update = 0;
        return ret;
    }


private:
    class VehicleVariables : public MSCFModel::VehicleVariables {
    public:
        double minaccel; // @brief saves the intended accel-value between multiple stopSpeed/followSpeed calls to then check which call actually updated the vehicles acceleration (accel-value is without coolness and drive-off)
        double wouldacc; // @brief saves the intended accel-value the CF-model would output, if there is no reaction time (accel-value is without coolness and drive-off)
        double lastacc; // @brief saves the intended accel-value when the driver was last updated (reaction time) (accel-value is without coolness and drive-off)
        double realacc; // @brief saves the resulting accel-value between multiple stopSpeed/followSpeed calls that the CF-model will eventually output
        double lastrealacc; // @brief saves the resulting accel-value the CF-model eventually outputed when the driver was last updated (reaction time)
        double realleaderacc; // @brief saves the leader accel-value from the call that resulted in the new speed
        double lastleaderacc; // @brief saves the leader accel-value from the call that resulted in the new speed when the driver was last updated (reaction time)
        double v0_int; // @brief is the internal desired speed of the vehicle
        double v0_old; // @brief is the previous desired speed of the vehicle needed for calculation purpose
        double t_off; // @brief is the time when the vehicle starts driving off
        double myw_gap; // @brief is the Wiener Process for the gap error calculation
        double myw_speed; // @brief is the Wiener Process for the speed error calculation
        double myw_error; // @brief is the Wiener Process for the driving error calculation
        double myv_est_l; // @brief saves the speed of the leading vehicle / 0 for a stop at the last driver update (reaction time)
        double myv_est; // @brief saves the speed of the vehicle at the last driver update (reaction time)
        double mys_est; // @brief saves the gap to leading vehicle / next stop at the last driver update (reaction time)
        int myap_update; // @brief is a number counting the simulation steps since the last driver/vehicle update (reaction time)
        std::vector<std::pair<double, double>> stop; // @brief saves the intended accelerations and distances from all stopSpeed-calculations of the current time step
    };

private:
    // @brief contains the main CF-model calculations
    double _v(const MSVehicle* const veh, const double gap2pred, const double mySpeed,
              const double predSpeed, const double desSpeed, const bool respectMinGap, const int update) const;

    // @brief calculates the internal desired speed for the vehicle depending on myTpreview and upcoming turns, intersections and speed limit changes
    void internalspeedlimit(MSVehicle* const veh, const double oldV) const;

    // @brief calculates the SecureGap similar to the MSCFModel-SecureGap-function, yet adding a targetDecel to decelerate with
    double internalsecuregap(const MSVehicle* const veh, const double speed, const double leaderSpeed, const double targetDecel) const;

private:
    // @brief The IDM delta exponent
    const double myDelta;

    // @brief A computational shortcut
    const double myTwoSqrtAccelDecel;

    // @brief The number of iterations in speed calculations
    const int myIterations;

    // @brief Correlation time of the Wiener Process for the driving error
    const double myTPersDrive;

    // @brief The maximal reaction time
    const double myTreaction;

    // @brief The preview distance time for the desired speed
    const double myTpreview;

    // @brief Correlation time of the Wiener Process for the estimation errors
    const double myTPersEstimate;

    // @brief Coolness Parameter of the Enhanced Intelligent Driver Model
    const double myCcoolness;

    // @brief Estimation error magnitude of the leading vehicle's speed
    const double mySigmaleader;

    // @brief Estimation error magnitude of the distance to the leading vehicle / next stop
    const double mySigmagap;

    // @brief Driving error magnitude
    const double mySigmaerror;

    // @brief Maximal jerk value
    const double myJerkmax;

    // @brief Maximal negative acceleration value before reacting instantaneously to the change in intended acceleration
    const double myEpsilonacc;

    // @brief Time until the maximal acceleration when driving off
    const double myTaccmax;

    // @brief Flatness of the drive off acceleration term
    const double myMflatness;

    // @brief Shift of the drive off acceleration term
    const double myMbegin;

    // @brief 1=simulate with the acceleration depending on the vehicle dynamics, 0=don't
    const bool myUseVehDynamics;

    // @brief number of vehicles in front, that are used to update the acceleration
    //const int myMaxVehPreview;

private:
    /// @brief Invalidated assignment operator
    MSCFModel_EIDM& operator=(const MSCFModel_EIDM& s);
};

#endif /* MSCFMODEL_EIDM_H */
