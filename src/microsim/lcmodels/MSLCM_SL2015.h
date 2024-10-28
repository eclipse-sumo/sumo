/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSLCM_SL2015.h
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Tue, 06.10.2015
///
// A lane change model for heterogeneous traffic (based on sub-lanes)
/****************************************************************************/
#pragma once
#include <config.h>

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

    MSLCM_SL2015(MSVehicle& v);

    virtual ~MSLCM_SL2015();

    /// @brief Returns the model's id
    LaneChangeModel getModelID() const override {
        return LaneChangeModel::SL2015;
    }

    /// @brief init cached parameters derived directly from model parameters
    void initDerivedParameters();

    /** @brief Called to examine whether the vehicle wants to change
     * with the given laneOffset (using the sublane model)
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable
     *
     * TODO better documentation. Refs #2
     * A.o.: When is this called (as a wantsChange() exists as well!? What's the difference?)
     * */
    int wantsChangeSublane(int laneOffset,
                           LaneChangeAction alternatives,
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
                           double& latDist, double& maneuverDist, int& blocked) override;

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset (this is a wrapper around wantsChangeSublane). XXX: no, it wraps _wantsChangeSublane
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable
     *
     * TODO: better documentation. Refs #2
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

    /// @brief whether the current vehicles shall be debugged
    bool debugVehicle() const override;

    void setOwnState(const int state) override;

    /// @brief Updates the value of safe lateral distances (mySafeLatDistLeft and mySafeLatDistRight)
    ///        during maneuver continuation in non-action steps.
    virtual void updateSafeLatDist(const double travelledLatDist) override;

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const override;

    /// @brief try to set the given parameter for this laneChangeModel. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value) override;

    /// @brief decides the next lateral speed depending on the remaining lane change distance to be covered
    ///        and updates maneuverDist according to lateral safety constraints.
    double computeSpeedLat(double latDist, double& maneuverDist, bool urgent) const override;

    LatAlignmentDefinition getDesiredAlignment() const override;

protected:
    /** helper function which contains the actual logic */
    double _patchSpeed(double min, const double wanted, double max,
                       const MSCFModel& cfModel);

    /// @brief helper function for doing the actual work
    int _wantsChangeSublane(
        int laneOffset,
        LaneChangeAction alternatives,
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
        double& latDist, double& maneuverDist, int& blocked);


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

    /// @brief reserve space at the end of the lane to avoid dead locks
    bool saveBlockerLength(double length, double foeLeftSpace) override;

    /// @brief whether the ego vehicle is driving outside edgebounds
    bool outsideEdge() const;

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
    void updateExpectedSublaneSpeeds(const MSLeaderDistanceInfo& ahead, int sublaneOffset, int laneIndex) override;

    /// @brief decide in which direction to move in case both directions are desirable
    StateAndDist decideDirection(StateAndDist sd1, StateAndDist sd2) const override;

    /// @brief return the most important change reason
    static int lowest_bit(int changeReason);

protected:

    /// @brief send a speed recommendation to the given vehicle
    void msg(const CLeaderDist& cld, double speed, int state);

    /// @brief compute shift so that prevSublane + shift = newSublane
    int computeSublaneShift(const MSEdge* prevEdge, const MSEdge* curEdge);

    /// @brief get the longest vehicle in the given info
    CLeaderDist getLongest(const MSLeaderDistanceInfo& ldi) const;

    bool tieBrakeLeader(const MSVehicle* veh) const;

    /// @brief get the slowest vehicle in the given info
    static CLeaderDist getSlowest(const MSLeaderDistanceInfo& ldi);

    /// @brief restrict latDist to permissible speed and determine blocking state depending on that distance
    int checkBlocking(const MSLane& neighLane, double& latDist, double maneuverDist, int laneOffset,
                      const MSLeaderDistanceInfo& leaders,
                      const MSLeaderDistanceInfo& followers,
                      const MSLeaderDistanceInfo& blockers,
                      const MSLeaderDistanceInfo& neighLeaders,
                      const MSLeaderDistanceInfo& neighFollowers,
                      const MSLeaderDistanceInfo& neighBlockers,
                      std::vector<CLeaderDist>* collectLeadBlockers = 0,
                      std::vector<CLeaderDist>* collectFollowBlockers = 0,
                      bool keepLatGapManeuver = false,
                      double gapFactor = 0,
                      int* retBlockedFully = 0);

    /// @brief check whether any of the vehicles overlaps with ego
    int checkBlockingVehicles(const MSVehicle* ego, const MSLeaderDistanceInfo& vehicles,
                              int laneOffset, double latDist, double foeOffset, bool leaders,
                              double& safeLatGapRight, double& safeLatGapLeft,
                              std::vector<CLeaderDist>* collectBlockers = 0) const;

    /// @brief return whether the given intervals overlap
    static bool overlap(double right, double left, double right2, double left2);

    /// @brief compute lane change action from desired lateral distance
    static LaneChangeAction getLCA(int state, double latDist);

    /// @brief compute strategic lane change actions
    /// TODO: Better documentation, refs #2
    int checkStrategicChange(int ret,
                             const MSLane& neighLane,
                             int laneOffset,
                             const MSLeaderDistanceInfo& leaders,
                             const MSLeaderDistanceInfo& neighLeaders,
                             const MSVehicle::LaneQ& curr,
                             const MSVehicle::LaneQ& neigh,
                             const MSVehicle::LaneQ& best,
                             int bestLaneOffset,
                             bool changeToBest,
                             double currentDist,
                             double neighDist,
                             double laDist,
                             double roundaboutBonus,
                             double latLaneDist,
                             bool checkOpposite,
                             double& latDist
                            );


    bool mustOvertakeStopped(const MSLane& neighLane, const MSLeaderDistanceInfo& leaders, const MSLeaderDistanceInfo& neighLead,
                             double posOnLane, double neighDist, bool right, double latLaneDist, double& currentDist, double& latDist);

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
                   double& maneuverDist,
                   int& blocked);


    /// @brief check remaining lateral gaps for the given foe vehicles and optionally update minimum lateral gaps
    void updateGaps(const MSLeaderDistanceInfo& others, double foeOffset, double oldCenter, double gapFactor,
                    double& surplusGapRight, double& surplusGapLeft, bool saveMinGap = false, double netOverlap = 0,
                    double latDist = 0,
                    std::vector<CLeaderDist>* collectBlockers = 0);

    /// @brief compute the gap factor for the given state
    double computeGapFactor(int state) const;

    /// @brief return the width of this vehicle (padded for numerical stability)
    double getWidth() const;

    /// @brief find leaders/followers that are already in a car-following relationship with ego
    void updateCFRelated(const MSLeaderDistanceInfo& vehicles, double foeOffset, bool leaders);

    /// @brief return the current sublane width (and return a sensible value when running without sublanes)
    double getSublaneWidth() {
        return MSGlobals::gLateralResolution > 0 ? MSGlobals::gLateralResolution : myVehicle.getLane()->getWidth();
    }

    /// @brief commit to lane change maneuver potentially overriding safe speed
    void commitManoeuvre(int blocked, int blockedFully,
                         const MSLeaderDistanceInfo& leaders,
                         const MSLeaderDistanceInfo& neighLeaders,
                         const MSLane& neighLane,
                         double maneuverDist);

    /// @brief compute speed when committing to an urgent change that is safe in regard to leading vehicles
    double commitFollowSpeed(double speed, double latDist, double secondsToLeaveLane, const MSLeaderDistanceInfo& leaders, double foeOffset) const;

    /// @brief estimate average speed over mySpeedGainLookahead time
    double forecastAverageSpeed(double vSafe, double vMax, double gap, double vLeader) const;

    /// @brief compute speedGain when moving by the given amount
    double computeSpeedGain(double latDistSublane, double defaultNextSpeed) const;

    /// @brief get lateral position of this vehicle
    double getPosLat();

    /// @brief get lateral drift for the current step
    double getLateralDrift();

    /// @brief return current edge width optionally extended by opposite direction lane width
    double getLeftBorder(bool checkOpposite = true) const;

    /// @brief return vehicle position relative to the current edge (extend by another virtual lane for opposite-direction driving)
    double getVehicleCenter() const;

    /// @brief return the right offset of the neighboring lane relative to the current edge
    double getNeighRight(const MSLane& neighLane) const;

    /* @brief check whether vehicle speed is appropriate for the intended maneuver distance
     * (rather than doing an orthogonal slide) */
    bool preventSliding(double maneuverDist) const;

    /// @brief check against thresholds
    inline bool wantsKeepRight(double keepRightProb) const;

    /// @brief check whether lane is an upcoming bidi lane
    bool isBidi(const MSLane* lane) const;

    /// @brief avoid unsafe lateral speed (overruling lcAccelLat)
    double emergencySpeedLat(double speedLat) const;

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

    /// @brief expected travel speeds on all sublanes on the current edge(!)
    std::vector<double> myExpectedSublaneSpeeds;

    /// @brief expected travel speeds on all sublanes on the current edge(!)
    const MSEdge* myLastEdge;

    /// @brief flag to prevent speed adaptation by slowing down
    bool myDontBrake;

    /// @brief whether the current lane changing maneuver can be finished in a single step
    bool myCanChangeFully;

    /// @brief the lateral distance the vehicle can safely move in the currently considered direction
    double mySafeLatDistRight;
    double mySafeLatDistLeft;

    /// @brief set of vehicles that are in a car-following relationship with ego (leader of followers)
    std::set<const MSVehicle*> myCFRelated;
    bool myCFRelatedReady;

    /// @name user configurable model parameters (can be changed via TraCI)
    //@{
    double myStrategicParam;
    double myCooperativeParam;
    double mySpeedGainParam;
    double myKeepRightParam;
    double myOppositeParam;
    double mySublaneParam;
    // @brief minimum lateral gap
    double myMinGapLat;
    // @brief willingness to encroach on other vehicles laterally (pushing them around)
    double myPushy;
    // @brief willingness to undercut longitudinal safe gaps
    double myAssertive;
    // @brief dynamic component of willingness for longitudinal gap reduction
    double myImpatience;
    double myMinImpatience;
    // @brief time to reach maximum impatience in seconds
    double myTimeToImpatience;
    // @brief lateral acceleration
    double myAccelLat;
    // @brief distance to turn at which alignment should be adjusted to the turn direction
    double myTurnAlignmentDist;
    // @brief the factor by which the lookahead distance to the left differs from the lookahead to the right
    double myLookaheadLeft;
    // @brief the factor by which the speedGain-threshold for the leftdiffers from the threshold for the right
    double mySpeedGainRight;
    // @brief lane discipline factor
    double myLaneDiscipline;
    // @brief lookahead for speedGain in seconds
    double mySpeedGainLookahead;
    // @brief bonus factor staying on the inside of multi-lane roundabout
    double myRoundaboutBonus;
    // @brief factor for cooperative speed adjustment
    double myCooperativeSpeed;
    // time for unrestricted driving on the right to accept keepRight change
    double myKeepRightAcceptanceTime;
    // @brief speed difference factor for overtaking the leader on the neighbor lane before changing to that lane
    double myOvertakeDeltaSpeedFactor;
    //@}

    /// @name derived parameters
    //@{
    // @brief threshold value for changing to the right
    double myChangeProbThresholdRight;
    // @brief threshold value for changing to the left
    double myChangeProbThresholdLeft;
    // @brief threshold value for accepting speed loss to achieve desired sublane alignment
    double mySpeedLossProbThreshold;

    // @brief state of lane keeping imperfection
    double mySigmaState;
    //@}

};
