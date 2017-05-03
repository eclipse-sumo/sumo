/****************************************************************************/
/// @file    MSLCM_SL2015.h
/// @author  Jakob Erdmann
/// @date    Tue, 06.10.2015
/// @version $Id$
///
// A lane change model for heterogeneous traffic (based on sub-lanes)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLCM_SL2015_h
#define MSLCM_SL2015_h


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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_SL2015
 * @brief A lane change model developed by J. Erdmann
 */
class MSLCM_SL2015 : public MSAbstractLaneChangeModel {
public:

    enum MyLCAEnum {
        LCA_MNone = 0,
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


    MSLCM_SL2015(MSVehicle& v);

    virtual ~MSLCM_SL2015();

    /** @brief Called to examine whether the vehicle wants to change
     * with the given laneOffset (using the sublane model)
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    int wantsChangeSublane(int laneOffset,
                           const MSLeaderDistanceInfo& leaders,
                           const MSLeaderDistanceInfo& followers,
                           const MSLeaderDistanceInfo& blockers,
                           const MSLeaderDistanceInfo& neighLeaders,
                           const MSLeaderDistanceInfo& neighFollowers,
                           const MSLeaderDistanceInfo& neighBlockers,
                           const MSLane& neighLane,
                           const std::vector<MSVehicle::LaneQ>& preb,
                           MSVehicle** lastBlocked,
                           MSVehicle** firstBlocked,
                           double& latDist, int& blocked);

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset (this is a wrapper around wantsChangeSublane).
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

    /// @brief whether the current vehicles shall be debugged
    bool debugVehicle() const;

    void setOwnState(const int state);

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const;

    /// @brief try to set the given parameter for this laneChangeModel. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value);

protected:

    /// @brief helper function for doing the actual work
    int _wantsChangeSublane(
        int laneOffset,
        const MSLeaderDistanceInfo& leaders,
        const MSLeaderDistanceInfo& followers,
        const MSLeaderDistanceInfo& blockers,
        const MSLeaderDistanceInfo& neighLeaders,
        const MSLeaderDistanceInfo& neighFollowers,
        const MSLeaderDistanceInfo& neighBlockers,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked,
        double& latDist, int& blocked);


    /* @brief decide whether we will overtake or follow blocking leaders
     * and inform them accordingly (see informLeader)
     * If we decide to follow, myVSafes will be extended
     * returns the planned speed if following or -1 if overtaking */
    double informLeaders(int blocked, int dir,
                         const std::vector<CLeaderDist>& blockers,
                         double remainingSeconds);

    /// @brief call informFollower for multiple followers
    void informFollowers(int blocked, int dir,
                         const std::vector<CLeaderDist>& blockers,
                         double remainingSeconds,
                         double plannedSpeed);

    /* @brief decide whether we will overtake or follow a blocking leader
     * and inform it accordingly
     * If we decide to follow, myVSafes will be extended
     * returns the planned speed if following or -1 if overtaking */
    double informLeader(int blocked, int dir,
                        const CLeaderDist& neighLead,
                        double remainingSeconds);

    /// @brief decide whether we will try cut in before the follower or allow to be overtaken
    void informFollower(int blocked, int dir,
                        const CLeaderDist& neighFollow,
                        double remainingSeconds,
                        double plannedSpeed);


    /// @brief compute useful slowdowns for blocked vehicles
    int slowDownForBlocked(MSVehicle** blocked, int state);

    /// @brief save space for vehicles which need to counter-lane-change
    void saveBlockerLength(const MSVehicle* blocker, int lcaCounter);

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


    /// @brief update expected speeds for each sublane of the current edge
    void updateExpectedSublaneSpeeds(const MSLeaderInfo& ahead, int sublaneOffset, int laneIndex);

    /// @brief decide in which direction to move in case both directions are desirable
    StateAndDist decideDirection(StateAndDist sd1, StateAndDist sd2) const;

protected:

    /// @brief send a speed recommendation to the given vehicle
    void msg(const CLeaderDist& cld, double speed, int state);

    /// @brief compute shift so that prevSublane + shift = newSublane
    int computeSublaneShift(const MSEdge* prevEdge, const MSEdge* curEdge);

    /// @brief get the longest vehicle in the given info
    static CLeaderDist getLongest(const MSLeaderDistanceInfo& ldi);

    /// @brief get the slowest vehicle in the given info
    static CLeaderDist getSlowest(const MSLeaderDistanceInfo& ldi);

    /// @brief restrict latDist to permissible speed and determine blocking state depending on that distance
    int checkBlocking(const MSLane& neighLane, double& latDist, int laneOffset,
                      const MSLeaderDistanceInfo& leaders,
                      const MSLeaderDistanceInfo& followers,
                      const MSLeaderDistanceInfo& blockers,
                      const MSLeaderDistanceInfo& neighLeaders,
                      const MSLeaderDistanceInfo& neighFollowers,
                      const MSLeaderDistanceInfo& neighBlockers,
                      std::vector<CLeaderDist>* collectLeadBlockers = 0,
                      std::vector<CLeaderDist>* collectFollowBlockers = 0,
                      bool keepLatGapManeuver = false);

    /// @brief check whether any of the vehicles overlaps with ego
    int checkBlockingVehicles(const MSVehicle* ego, const MSLeaderDistanceInfo& vehicles,
                              double latDist, double foeOffset, bool leaders, LaneChangeAction blockType,
                              std::vector<CLeaderDist>* collectBlockers = 0) const;

    /// @brief return whether the given intervals overlap
    static bool overlap(double right, double left, double right2, double left2);

    /// @brief compute lane change action from desired lateral distance
    static LaneChangeAction getLCA(int state, double latDist);

    /// @brief compute strategic lane change actions
    int checkStrategicChange(int ret,
                             int laneOffset,
                             const std::vector<MSVehicle::LaneQ>& preb,
                             const MSLeaderDistanceInfo& leaders,
                             const MSLeaderDistanceInfo& neighLeaders,
                             int currIdx,
                             int bestLaneOffset,
                             bool changeToBest,
                             int lcaCounter,
                             double currentDist,
                             double neighDist,
                             double laDist,
                             int roundaboutEdgesAhead,
                             double latLaneDist,
                             double& latDist
                            );

    /// @brief check whether lateral gap requirements are met override the current maneuver if necessary
    int keepLatGap(int state,
                   const MSLeaderDistanceInfo& leaders,
                   const MSLeaderDistanceInfo& followers,
                   const MSLeaderDistanceInfo& blockers,
                   const MSLeaderDistanceInfo& neighLeaders,
                   const MSLeaderDistanceInfo& neighFollowers,
                   const MSLeaderDistanceInfo& neighBlockers,
                   const MSLane& neighLane,
                   int laneOffset,
                   double& latDist,
                   int& blocked);


    /// @brief check remaining lateral gaps for the given foe vehicles and optionally update minimum lateral gaps
    void updateGaps(const MSLeaderDistanceInfo& others, double foeOffset, double oldCenter, double gapFactor,
                    double& surplusGapRight, double& surplusGapLeft, bool saveMinGap = false, double netOverlap = 0);

protected:
    /// @brief a value for tracking the probability that a change to the right is beneficial
    double mySpeedGainProbabilityRight;
    /// @brief a value for tracking the probability that a change to the left is beneficial
    double mySpeedGainProbabilityLeft;

    /* @brief a value for tracking the probability of following the/"Rechtsfahrgebot"
     * A larger negative value indicates higher probability for moving to the
     * right (as in mySpeedGainProbability) */
    double myKeepRightProbability;

    double myLeadingBlockerLength;
    double myLeftSpace;

    /*@brief the speed to use when computing the look-ahead distance for
     * determining urgency of strategic lane changes */
    double myLookAheadSpeed;

    /// @brief speed adaptation requests by ego and surrounding vehicles
    std::vector<double> myVSafes;

    /// @brief expected travel speeds on all sublanes on the current edge(!)
    std::vector<double> myExpectedSublaneSpeeds;

    /// @brief expected travel speeds on all sublanes on the current edge(!)
    const MSEdge* myLastEdge;

    /// @brief flag to prevent speed adaptation by slowing down
    bool myDontBrake;

    /// @brief whether the current lane changing meneuver can be finished in a single step
    bool myCanChangeFully;

    /// @brief lane changing state from the previous simulation step
    int myPreviousState;

    /// @brief the complete lateral distance the vehicle wants to travel to finish its maneuver
    double myOrigLatDist;

    /// @name user configurable model parameters (can be changed via TraCI)
    //@{
    double myStrategicParam;
    double myCooperativeParam;
    double mySpeedGainParam;
    double myKeepRightParam;
    double mySublaneParam;
    // @brief willingness to encroach on other vehicles laterally (pushing them around)
    double myPushy;
    double myAssertive;
    //@}

    /// @name derived parameters
    //@{
    // @brief willingness to encroach on other vehicles laterally (pushing them around)
    const double myChangeProbThresholdRight;
    const double myChangeProbThresholdLeft;
    const double mySpeedLossProbThreshold;
    //@}

};


#endif

/****************************************************************************/

