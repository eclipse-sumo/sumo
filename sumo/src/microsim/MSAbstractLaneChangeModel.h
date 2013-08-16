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

    /// @brief No action
    LCA_NONE = 0,
    /// @brief The action is due to the wish to follow the route (navigational lc)
    LCA_URGENT = 1,
    /// @brief The action is due to the wish to be faster (tactical lc)
    LCA_SPEEDGAIN = 2,
    /// @brief Wants go to the left
    LCA_LEFT = 4,
    /// @brief Wants go to the right
    LCA_RIGHT = 8,

    LCA_WANTS_LANECHANGE = LCA_URGENT | LCA_SPEEDGAIN | LCA_LEFT | LCA_RIGHT,
    /// @}


    /// @name External state
    /// @{

    /// @brief The vehicle is blocked by left leader
    LCA_BLOCKED_BY_LEFT_LEADER = 16,
    /// @brief The vehicle is blocked by left follower
    LCA_BLOCKED_BY_LEFT_FOLLOWER = 32,

    /// @brief The vehicle is blocked by right leader
    LCA_BLOCKED_BY_RIGHT_LEADER = 64,
    /// @brief The vehicle is blocked by right follower
    LCA_BLOCKED_BY_RIGHT_FOLLOWER = 128,

    LCA_BLOCKED_LEFT = LCA_BLOCKED_BY_LEFT_LEADER | LCA_BLOCKED_BY_LEFT_FOLLOWER,
    LCA_BLOCKED_RIGHT = LCA_BLOCKED_BY_RIGHT_LEADER | LCA_BLOCKED_BY_RIGHT_FOLLOWER,
    LCA_BLOCKED_BY_LEADER = LCA_BLOCKED_BY_LEFT_LEADER | LCA_BLOCKED_BY_RIGHT_LEADER,
    LCA_BLOCKED_BY_FOLLOWER = LCA_BLOCKED_BY_LEFT_FOLLOWER | LCA_BLOCKED_BY_RIGHT_FOLLOWER,
    LCA_BLOCKED = LCA_BLOCKED_LEFT | LCA_BLOCKED_RIGHT

                  // The vehicle is blocked being overlapping
                  // This is currently not used, but I'll keep it while working on this, as
                  //  overlapping may be interested, but surely divided by leader/follower
                  // LCA_OVERLAPPING = 64
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

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked) = 0;

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSLCMessager& msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal>& leader,
        const std::pair<MSVehicle*, SUMOReal>& neighLead,
        const std::pair<MSVehicle*, SUMOReal>& neighFollow,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked) = 0;

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

    virtual void changed() = 0;

    void unchanged() {
        myLastLaneChangeOffset += DELTA_T;
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

#ifndef NO_TRACI
    /**
     * The vehicle is requested to change the lane as soon as possible
     * without violating any directives defined by this lane change model
     *
     * @param request	indicates the requested change
     */
    virtual void requestLaneChange(MSVehicle::ChangeRequest request) {
        myChangeRequest = request;
    };

    /**
     * Inform the model that a certain lane change request has been fulfilled
     * by the lane changer, so the request won't be taken into account the next time.
     *
     * @param request	indicates the request that was fulfilled
     */
    virtual void fulfillChangeRequest(MSVehicle::ChangeRequest request) {
        if (request == myChangeRequest) {
            myChangeRequest = MSVehicle::REQUEST_NONE;
        }
    }
#endif

protected:
    virtual bool congested(const MSVehicle* const neighLeader);

    virtual bool predInteraction(const MSVehicle* const leader);


protected:
    /// @brief The vehicle this lane-changer belongs to
    MSVehicle& myVehicle;

    /// @brief The current state of the vehicle
    int myOwnState;

    /// @brief information how long ago the vehicle has performed a lane-change
    SUMOTime myLastLaneChangeOffset;

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

#ifndef NO_TRACI
    MSVehicle::ChangeRequest myChangeRequest;
#endif

    /// @brief The vehicle's car following model
    const MSCFModel& myCarFollowModel;

private:
    /// @brief Invalidated assignment operator
    MSAbstractLaneChangeModel& operator=(const MSAbstractLaneChangeModel& s);
};


#endif

/****************************************************************************/

