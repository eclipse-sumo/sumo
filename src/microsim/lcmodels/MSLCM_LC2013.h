/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSLCM_LC2013.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 08.10.2013
///
// A lane change model developed by D. Krajzewicz, J. Erdmann et al. between 2004 and 2013
/****************************************************************************/
#pragma once
#include <config.h>

#include "MSAbstractLaneChangeModel.h"
#include <vector>

// INVALID_SPEED should be used when the construction of upper bound for the speed
// leads to no restrictions, e.g. during LC-messaging to followers or leaders.
// Currently either std::numeric_limits<...>.max() or -1 is used for this purpose in many places.
// TODO: implement this everywhere and remove workarounds for ballistic update in cases of possible '-1'-returns. Refs. #2577
#define INVALID_SPEED 299792458 + 1 // nothing can go faster than the speed of light!


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_LC2013
 * @brief A lane change model developed by D. Krajzewicz, J. Erdmann
 * et al. between 2004 and 2013
 */
class MSLCM_LC2013 : public MSAbstractLaneChangeModel {
public:

    MSLCM_LC2013(MSVehicle& v);

    virtual ~MSLCM_LC2013();

    /// @brief Returns the model's id
    LaneChangeModel getModelID() const override {
        return LaneChangeModel::LC2013;
    }

    /// @brief init cached parameters derived directly from model parameters
    void initDerivedParameters();

    bool debugVehicle() const override;

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset.
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable
     *
     * TODO: better documentation, refs #2
     *
     * */
    int wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, double>& leader,
        const std::pair<MSVehicle*, double>& follower,
        const std::pair<MSVehicle*, double>& neighLead,
        const std::pair<MSVehicle*, double>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked) override;

    void* inform(void* info, MSVehicle* sender) override;

    /** @brief Called to adapt the speed in order to allow a lane change.
     *         It uses information on LC-related desired speed-changes from
     *         the call to wantsChange() at the end of the previous simulation step
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    double patchSpeed(const double min, const double wanted, const double max,
                      const MSCFModel& cfModel) override;

    void changed() override;

    void resetState() override;

    double getSafetyFactor() const override;

    double getOppositeSafetyFactor() const override;

    void prepareStep() override;

    double getExtraReservation(int bestLaneOffset) const override;

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const override;

    /// @brief try to set the given parameter for this laneChangeModel. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value) override;

    /// @brief decides the next lateral speed (for continuous lane changing)
    double computeSpeedLat(double latDist, double& maneuverDist, bool urgent) const override;

protected:

    /** helper function which contains the actual logic */
    double _patchSpeed(double min, const double wanted, double max,
                       const MSCFModel& cfModel);

    /// @brief helper function for doing the actual work
    int _wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, double>& leader,
        const std::pair<MSVehicle*, double>& follower,
        const std::pair<MSVehicle*, double>& neighLead,
        const std::pair<MSVehicle*, double>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked);

    /* @brief decide whether we will overtake or follow a blocking leader
     * and inform it accordingly
     * If we decide to follow, myVSafes will be extended
     * returns the planned speed if following or -1 if overtaking */
    double informLeader(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                        int blocked, int dir,
                        const std::pair<MSVehicle*, double>& neighLead,
                        double remainingSeconds);

    /// @brief decide whether we will try cut in before the follower or allow to be overtaken
    void informFollower(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                        int blocked, int dir,
                        const std::pair<MSVehicle*, double>& neighFollow,
                        double remainingSeconds,
                        double plannedSpeed);


    /* @brief compute the distance to cover until a safe gap to the vehicle v in front is reached
     *        assuming constant velocities
         * @param[in] follower the vehicle which overtakes
         * @param[in] leader the vehicle to be overtaken
         * @param[in] gap initial distance between front of follower and back of leader
         * @param[in] leaderSpeed an assumed speed for the leader (default uses the current speed)
         * @param[in] followerSpeed an assumed speed for the follower (default uses the current speed)
         * @return the distance that the relative positions would have to change.
         */
    static double overtakeDistance(const MSVehicle* follower, const MSVehicle* leader, const double gap, double followerSpeed = INVALID_SPEED, double leaderSpeed = INVALID_SPEED);

    /// @brief compute useful slowdowns for blocked vehicles
    int slowDownForBlocked(MSVehicle** blocked, int state);

    /// @brief anticipate future follow speed for the given leader
    double anticipateFollowSpeed(const std::pair<MSVehicle*, double>& leaderDist, double dist, double vMax, bool acceleratingLeader);

    /// @brief react to pedestrians on the given lane
    void adaptSpeedToPedestrians(const MSLane* lane, double& v);

    /// @brief reserve space at the end of the lane to avoid dead locks
    bool saveBlockerLength(double length, double foeLeftSpace) override;

    inline bool amBlockingLeader() {
        return (myOwnState & LCA_AMBLOCKINGLEADER) != 0;
    }
    inline bool amBlockingFollower() {
        return (myOwnState & LCA_AMBLOCKINGFOLLOWER) != 0;
    }
    inline bool amBlockingFollowerNB() {
        return (myOwnState & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0;
    }
    inline bool amBlockingFollowerPlusNB() {
        return (myOwnState & (LCA_AMBLOCKINGFOLLOWER | LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)) != 0;
    }
    inline bool currentDistDisallows(double dist, int laneOffset, double lookForwardDist) {
        return dist / (abs(laneOffset)) < lookForwardDist;
    }
    inline bool currentDistAllows(double dist, int laneOffset, double lookForwardDist) {
        return dist / abs(laneOffset) > lookForwardDist;
    }

protected:

    /// @brief information regarding save velocity (unused) and state flags of the ego vehicle
    typedef std::pair<double, int> Info;

    /// @brief a value for tracking the probability that a change to the offset with the same sign is beneficial
    double mySpeedGainProbability;
    /* @brief a value for tracking the probability of following the/"Rechtsfahrgebot"
     * A larger negative value indicates higher probability for moving to the
     * right (as in mySpeedGainProbability) */
    double myKeepRightProbability;

    double myLeadingBlockerLength;
    double myLeftSpace;

    /*@brief the speed to use when computing the look-ahead distance for
     * determining urgency of strategic lane changes */
    double myLookAheadSpeed;

    bool myDontBrake; // XXX: myDontBrake is initialized as false and seems not to be changed anywhere... What's its purpose???

    /// @name user configurable model parameters (can be changed via TraCI)
    //@{
    double myStrategicParam;
    double myCooperativeParam; // in [0,1]
    double mySpeedGainParam;
    double myKeepRightParam;
    double myOppositeParam;

    // @brief the factor by which the lookahead distance to the left differs from the lookahead to the right
    double myLookaheadLeft;
    // @brief the factor by which the speedGain-threshold for the leftdiffers from the threshold for the right
    double mySpeedGainRight;

    // @brief willingness to undercut longitudinal safe gaps
    double myAssertive;
    // @brief lookahead for speedGain in seconds
    double mySpeedGainLookahead;
    // @brief bounus factor staying on the inside of multi-lane roundabout
    double myRoundaboutBonus;
    // @brief factor for cooperative speed adjustment
    double myCooperativeSpeed;

    // time for unrestricted driving on the right to accept keepRight change
    double myKeepRightAcceptanceTime;

    // @brief speed difference factor for overtaking the leader on the neighbor lane before changing to that lane
    double myOvertakeDeltaSpeedFactor;

    // for feature testing
    const double myExperimentalParam1;

    //@}

    /// @name derived parameters
    //@{
    // @brief willingness to encroach on other vehicles laterally (pushing them around)
    double myChangeProbThresholdRight;
    double myChangeProbThresholdLeft;
    //@}
};
