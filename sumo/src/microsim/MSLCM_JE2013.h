/****************************************************************************/
/// @file    MSLCM_JE2013.h
/// @author  Jakob Erdmann
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by J. Erdmann
// based on the model of D. Krajzewicz developed between 2004 and 2011 (MSLCM_DK2004)
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
#ifndef MSLCM_JE2013_h
#define MSLCM_JE2013_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSAbstractLaneChangeModel.h>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_JE2013
 * @brief A lane change model developed by J. Erdmann
 */
class MSLCM_JE2013 : public MSAbstractLaneChangeModel {
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


    MSLCM_JE2013(MSVehicle& v);

    virtual ~MSLCM_JE2013();

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset.
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    int wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
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
    SUMOReal patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                        const MSCFModel& cfModel);
    /** helper function which contains the actual logic */
    SUMOReal _patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                         const MSCFModel& cfModel);

    void changed();

    void prepareStep();


protected:

    /// @brief helper function for doing the actual work
    int _wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked);


    /* @brief decide whether we will overtake or follow a blocking leader
     * and inform it accordingly
     * If we decide to follow, myVSafes will be extended
     * returns the planned speed if following or -1 if overtaking */
    SUMOReal informLeader(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                          int blocked, int dir,
                          const std::pair<MSVehicle*, SUMOReal>& neighLead,
                          SUMOReal remainingSeconds);

    /// @brief decide whether we will try cut in before the follower or allow to be overtaken
    void informFollower(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                        int blocked, int dir,
                        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
                        SUMOReal remainingSeconds,
                        SUMOReal plannedSpeed);


    /// @brief compute useful slowdowns for blocked vehicles
    int slowDownForBlocked(MSVehicle** blocked, int state);

    /// @brief save space for vehicles which need to counter-lane-change
    void saveBlockerLength(MSVehicle* blocker, int lcaCounter);

    /// @brief reserve space at the end of the lane to avoid dead locks
    inline void saveBlockerLength(SUMOReal length) {
        myLeadingBlockerLength = MAX2(length, myLeadingBlockerLength);
    };

    /// @brief updated myKeepRightProbability and mySpeedGainProbability if the right neighbours are faster
    void keepRight(MSVehicle* neigh);

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
    inline bool currentDistDisallows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist / (abs(laneOffset)) < lookForwardDist;
    }
    inline bool currentDistAllows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist / abs(laneOffset) > lookForwardDist;
    }

    /// @brief information regarding save velocity (unused) and state flags of the ego vehicle
    typedef std::pair<SUMOReal, int> Info;



protected:
    /// @brief a value for tracking the probability that a change to the offset with the same sign is beneficial
    SUMOReal mySpeedGainProbability;
    /// @brief a value for tracking the probability of following the/"Rechtsfahrgebot" (never a positive value)
    SUMOReal myKeepRightProbability;

    SUMOReal myLeadingBlockerLength;
    SUMOReal myLeftSpace;

    /*@brief the speed to use when computing the look-ahead distance for
     * determining urgency of strategic lane changes */
    SUMOReal myLookAheadSpeed;

    std::vector<SUMOReal> myVSafes;
    bool myDontBrake;

};


#endif

/****************************************************************************/

