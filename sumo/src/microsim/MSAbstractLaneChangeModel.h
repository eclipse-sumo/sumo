/****************************************************************************/
/// @file    MSAbstractLaneChangeModel.h
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Interface for lane-change models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSAbstractLaneChangeModel_h
#define MSAbstractLaneChangeModel_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSVehicle.h"
class MSLane;

// ===========================================================================
// used enumeration
// ===========================================================================
/** @enum LaneChangeAction
 * @brief A try to store the state of a vehicle's lane-change wish in an int
 */
enum LaneChangeAction {
    /// @name currently wanted lane-change action
    /// @{

    /// @brief No action desired
    LCA_NONE = 0,
    /// @brief Needs to stay on the current lane
    LCA_STAY = 1 << 0,
    /// @brief Wants go to the left
    LCA_LEFT = 1 << 1,
    /// @brief Wants go to the right
    LCA_RIGHT = 1 << 2,

    /// @brief The action is needed to follow the route (navigational lc)
    LCA_STRATEGIC = 1 << 3,
    /// @brief The action is done to help someone else
    LCA_COOPERATIVE = 1 << 4,
    /// @brief The action is due to the wish to be faster (tactical lc)
    LCA_SPEEDGAIN = 1 << 5,
    /// @brief The action is due to the default of keeping right "Rechtsfahrgebot"
    LCA_KEEPRIGHT = 1 << 6,
    /// @brief The action is due to a TraCI request
    LCA_TRACI = 1 << 7,

    /// @brief The action is urgent (to be defined by lc-model)
    LCA_URGENT = 1 << 8,

    LCA_WANTS_LANECHANGE = LCA_LEFT | LCA_RIGHT,
    LCA_WANTS_LANECHANGE_OR_STAY = LCA_WANTS_LANECHANGE | LCA_STAY,
    /// @}

    /// @name External state
    /// @{

    /// @brief The vehicle is blocked by left leader
    LCA_BLOCKED_BY_LEFT_LEADER = 1 << 9,
    /// @brief The vehicle is blocked by left follower
    LCA_BLOCKED_BY_LEFT_FOLLOWER = 1 << 10,

    /// @brief The vehicle is blocked by right leader
    LCA_BLOCKED_BY_RIGHT_LEADER = 1 << 11,
    /// @brief The vehicle is blocked by right follower
    LCA_BLOCKED_BY_RIGHT_FOLLOWER = 1 << 12,

    // The vehicle is blocked being overlapping
    LCA_OVERLAPPING =  1 << 13,

    LCA_BLOCKED_LEFT = LCA_BLOCKED_BY_LEFT_LEADER | LCA_BLOCKED_BY_LEFT_FOLLOWER,
    LCA_BLOCKED_RIGHT = LCA_BLOCKED_BY_RIGHT_LEADER | LCA_BLOCKED_BY_RIGHT_FOLLOWER,
    LCA_BLOCKED_BY_LEADER = LCA_BLOCKED_BY_LEFT_LEADER | LCA_BLOCKED_BY_RIGHT_LEADER,
    LCA_BLOCKED_BY_FOLLOWER = LCA_BLOCKED_BY_LEFT_FOLLOWER | LCA_BLOCKED_BY_RIGHT_FOLLOWER,
    LCA_BLOCKED = LCA_BLOCKED_LEFT | LCA_BLOCKED_RIGHT

                  /// @}

};





// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSAbstractLaneChangeModel
 * @brief Interface for lane-change models
 */
class MSAbstractLaneChangeModel {
public:

    /** @class MSLCMessager
     * @brief A class responsible for exchanging messages between cars involved in lane-change interaction
     */
    class MSLCMessager {
    public:
        /** @brief Constructor
         * @param[in] leader The leader on the informed vehicle's lane
         * @param[in] neighLead The leader on the lane the vehicle want to change to
         * @param[in] neighFollow The follower on the lane the vehicle want to change to
         */
        MSLCMessager(MSVehicle* leader,  MSVehicle* neighLead, MSVehicle* neighFollow)
            : myLeader(leader), myNeighLeader(neighLead),
              myNeighFollower(neighFollow) { }


        /// @brief Destructor
        ~MSLCMessager() { }


        /** @brief Informs the leader on the same lane
         * @param[in] info The information to pass
         * @param[in] sender The sending vehicle (the lane changing vehicle)
         * @return Something!?
         */
        void* informLeader(void* info, MSVehicle* sender) {
            assert(myLeader != 0);
            return myLeader->getLaneChangeModel().inform(info, sender);
        }


        /** @brief Informs the leader on the desired lane
         * @param[in] info The information to pass
         * @param[in] sender The sending vehicle (the lane changing vehicle)
         * @return Something!?
         */
        void* informNeighLeader(void* info, MSVehicle* sender) {
            assert(myNeighLeader != 0);
            return myNeighLeader->getLaneChangeModel().inform(info, sender);
        }


        /** @brief Informs the follower on the desired lane
         * @param[in] info The information to pass
         * @param[in] sender The sending vehicle (the lane changing vehicle)
         * @return Something!?
         */
        void* informNeighFollower(void* info, MSVehicle* sender) {
            assert(myNeighFollower != 0);
            return myNeighFollower->getLaneChangeModel().inform(info, sender);
        }


    private:
        /// @brief The leader on the informed vehicle's lane
        MSVehicle* myLeader;
        /// @brief The leader on the lane the vehicle want to change to
        MSVehicle* myNeighLeader;
        /// @brief The follower on the lane the vehicle want to change to
        MSVehicle* myNeighFollower;

    };

    /// @brief init global model parameters
    void static initGlobalOptions(const OptionsCont& oc);

    /** @brief Factory method for instantiating new lane changing models
     * @param[in] lcm The type of model to build
     * @param[in] vehicle The vehicle for which this model shall be built
     */
    static MSAbstractLaneChangeModel* build(LaneChangeModel lcm, MSVehicle& vehicle);

    /** @brief Constructor
     * @param[in] v The vehicle this lane-changer belongs to
     */
    MSAbstractLaneChangeModel(MSVehicle& v);

    /// @brief Destructor
    virtual ~MSAbstractLaneChangeModel();

    inline int getOwnState() const {
        return myOwnState;
    }

    inline void setOwnState(int state) {
        myOwnState = state;
    }

    virtual void prepareStep() { }

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset.
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    virtual int wantsChange(
        int laneOffset,
        MSAbstractLaneChangeModel::MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked) = 0;

    virtual void* inform(void* info, MSVehicle* sender) = 0;

    /** @brief Called to adapt the speed in order to allow a lane change.
     *
     * It is guaranteed that min<=wanted<=max, but the implementation needs
     * to make sure that the return value is between min and max.
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    virtual SUMOReal patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                                const MSCFModel& cfModel) = 0;

    virtual void changed(int dir) = 0;

    void unchanged() {
        if (myLastLaneChangeOffset > 0) {
            myLastLaneChangeOffset += DELTA_T;
        } else if (myLastLaneChangeOffset < 0) {
            myLastLaneChangeOffset -= DELTA_T;
        }
    }

    /** @brief Returns the lane the vehicles shadow is on during continuouss lane change
     * @return The vehicle's shadow lane
     */
    MSLane* getShadowLane() const {
        return myShadowLane;
    }


    inline SUMOTime getLastLaneChangeOffset() const {
        return myLastLaneChangeOffset;
    }


    /// @brief return whether the vehicle passed the midpoint of a continuous lane change maneuver
    inline bool isLaneChangeMidpointPassed() const {
        return myLaneChangeMidpointPassed;
    }

    /// @brief return whether the vehicle passed the midpoint of a continuous lane change maneuver
    inline SUMOReal getLaneChangeCompletion() const {
        return myLaneChangeCompletion;
    }

    /// @brief return true if the vehicle currently performs a lane change maneuver
    inline bool isChangingLanes() const {
        return myLaneChangeCompletion < (1 - NUMERICAL_EPS);
    }

    /// @brief return the direction of the current lane change maneuver
    inline int getLaneChangeDirection() const {
        return myLaneChangeDirection;
    }

    /// @brief reset the flag whether a vehicle already moved to false
    inline bool alreadyMoved() const {
        return myAlreadyMoved;
    }

    /// @brief reset the flag whether a vehicle already moved to false
    void resetMoved() {
        myAlreadyMoved = false;
    }


    /// @brief start the lane change maneuver and return whether it continues
    bool startLaneChangeManeuver(MSLane* source, MSLane* target, int direction);


    /* @brief continue the lane change maneuver
     * @param[in] moved Whether the vehicle has moved to a new lane
     */
    void continueLaneChangeManeuver(bool moved);

    /* @brief finish the lane change maneuver
     */
    inline void endLaneChangeManeuver() {
        removeLaneChangeShadow();
        myLaneChangeCompletion = 1;
        myShadowLane = 0;
    }

    /// @brief remove the shadow copy of a lane change maneuver
    void removeLaneChangeShadow();

    /// @brief reserve space at the end of the lane to avoid dead locks
    virtual void saveBlockerLength(SUMOReal length) {
        UNUSED_PARAMETER(length);
    };

protected:
    virtual bool congested(const MSVehicle* const neighLeader);

    virtual bool predInteraction(const MSVehicle* const leader);

    /// @brief whether the influencer cancels the given request
    bool cancelRequest(int state);


protected:
    /// @brief The vehicle this lane-changer belongs to
    MSVehicle& myVehicle;

    /// @brief The current state of the vehicle
    int myOwnState;

    /// @brief progress of the lane change maneuver 0:started, 1:complete
    SUMOReal myLaneChangeCompletion;

    /// @brief direction of the lane change maneuver -1 means right, 1 means left
    int myLaneChangeDirection;

    /// @brief whether myLane has already been set to the target of the lane-change maneuver
    bool myLaneChangeMidpointPassed;

    /// @brief whether the vehicle has already moved this step
    bool myAlreadyMoved;

    /// @brief The lane the vehicle shadow is on during a continuous lane change
    MSLane* myShadowLane;

    /// Wether a vehicle shadow exists
    bool myHaveShadow;

    /// @brief The vehicle's car following model
    const MSCFModel& myCarFollowModel;

    /* @brief to be called by derived classes in their changed() method.
     * If dir=0 is given, the current value remains unchanged */
    void initLastLaneChangeOffset(int dir);

    /// @brief whether overtaking on the right is permitted
    static bool myAllowOvertakingRight;

private:
    /* @brief information how long ago the vehicle has performed a lane-change,
     * sign indicates direction of the last change
     */
    SUMOTime myLastLaneChangeOffset;



private:
    /// @brief Invalidated assignment operator
    MSAbstractLaneChangeModel& operator=(const MSAbstractLaneChangeModel& s);
};


#endif

/****************************************************************************/

