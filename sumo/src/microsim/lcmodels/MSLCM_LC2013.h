/****************************************************************************/
/// @file    MSLCM_LC2013.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by D. Krajzewicz, J. Erdmann et al. between 2004 and 2013
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
#ifndef MSLCM_LC2013_h
#define MSLCM_LC2013_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

    enum MyLCAEnum {
        LCA_AMBLOCKINGLEADER = 1 << 16,
        LCA_AMBLOCKINGFOLLOWER = 1 << 17,
        LCA_MRIGHT = 1 << 18,
        LCA_MLEFT = 1 << 19,
        // !!! never set LCA_UNBLOCK = 1 << 20,
        LCA_AMBLOCKINGFOLLOWER_DONTBRAKE = 1 << 21,
        // !!! never used LCA_AMBLOCKINGSECONDFOLLOWER = 1 << 22,
        LCA_CHANGE_TO_HELP = 1 << 23,
        // !!! never read LCA_KEEP1 = 1 << 24,
        // !!! never used LCA_KEEP2 = 1 << 25,
        LCA_AMBACKBLOCKER = 1 << 26,
        LCA_AMBACKBLOCKER_STANDING = 1 << 27
    };


    MSLCM_LC2013(MSVehicle& v);

    virtual ~MSLCM_LC2013();

    bool debugVehicle() const;

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset.
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    int wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, double>& leader,
        const std::pair<MSVehicle*, double>& neighLead,
        const std::pair<MSVehicle*, double>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked);

    void* inform(void* info, MSVehicle* sender);

    /** @brief Called to adapt the speed in order to allow a lane change.
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    double patchSpeed(const double min, const double wanted, const double max,
                      const MSCFModel& cfModel);
    /** helper function which contains the actual logic */
    double _patchSpeed(const double min, const double wanted, const double max,
                       const MSCFModel& cfModel);

    void changed();

    void prepareStep();

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const;

    /// @brief try to set the given parameter for this laneChangeModel. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value);

protected:

    /// @brief helper function for doing the actual work
    int _wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, double>& leader,
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


    // XXX: consider relocation of the roundabout functions (perhaps to MSVehicle or the abstract LC Model...) (Leo)
    /// @brief computes the distance and number of edges in the next upcoming
    ///        roundabout along the lane continuations given in curr and neigh
    /// @param[in] veh The considered ego Vehicle
    /// @param[in] curr continuation info along veh's current lane
    /// @param[in] neigh continuation info along a neighboring lane (in MSLCM_2013::_wantsChange() the considered lane for a lanechange)
    /// @param[out] roundaboutDistanceAhead Accumulated length of lanes in the next oncoming roundabout in curr
    /// @param[out] roundaboutDistanceAheadNeigh Accumulated length of lanes in the next oncoming roundabout in neigh
    /// @param[out] roundaboutEdgesAhead  Number of lanes in the next oncoming roundabout in curr
    /// @param[out] roundaboutEdgesAheadNeigh Number of lanes in the next oncoming roundabout in neigh
    static void
    getRoundaboutAheadInfo(const MSLCM_LC2013* lcm, const MSVehicle::LaneQ& curr, const MSVehicle::LaneQ& neigh,
                           double& roundaboutDistanceAhead, double& roundaboutDistanceAheadNeigh, int& roundaboutEdgesAhead, int& roundaboutEdgesAheadNeigh);

    /// @brief Computes the artificial bonus distance for roundabout lanes
    ///        this additional distance reduces the sense of urgency within
    ///        roundabouts and thereby promotes the use of the inner roundabout
    ///        lane in multi-lane roundabouts.
    /// @param[in] roundaboutDistAhead Distance on roundabout
    /// @param[in] roundaboutEdgesAhead number of edges on roundabout
    double
    roundaboutDistBonus(double roundaboutDistAhead, int roundaboutEdgesAhead) const;

    /// @brief compute the distance on the next upcoming roundabout along a given sequence of lanes.
    /// @param[in] position position of the vehicle on the initial lane
    /// @param[in] initialLane starting lane for the computation (may be internal)
    /// @param[in] continuationLanes sequence of lanes along which the roundabout distance is to be computed (only containing non-internal lanes)
    /// @return distance along next upcoming roundabout on the given sequence of lanes continuationLanes
    static double
    distanceAlongNextRoundabout(double position, const MSLane* initialLane, const std::vector<MSLane*>& continuationLanes);

    /// @brief save space for vehicles which need to counter-lane-change
    void saveBlockerLength(MSVehicle* blocker, int lcaCounter);

    /// @brief reserve space at the end of the lane to avoid dead locks
    inline void saveBlockerLength(double length) {
        myLeadingBlockerLength = MAX2(length, myLeadingBlockerLength);
    };

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

    /// @brief information regarding save velocity (unused) and state flags of the ego vehicle
    typedef std::pair<double, int> Info;



protected:
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

    std::vector<double> myVSafes;
    bool myDontBrake; // XXX: myDontBrake is initialized as false and seems not to be changed anywhere... What's its purpose???

    /// @name user configurable model parameters (can be changed via TraCI)
    //@{
    double myStrategicParam;
    double myCooperativeParam; // in [0,1]
    double mySpeedGainParam;
    double myKeepRightParam;

    const double myExperimentalParam1; // for feature testing
    //@}

    /// @name derived parameters
    //@{
    // @brief willingness to encroach on other vehicles laterally (pushing them around)
    const double myChangeProbThresholdRight;
    const double myChangeProbThresholdLeft;
    //@}
};


#endif

/****************************************************************************/

