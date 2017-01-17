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

#include <microsim/MSVehicle.h>

class MSLane;

// ===========================================================================
// used enumeration
// ===========================================================================

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

    struct StateAndDist {
        // @brief LaneChangeAction flags
        int state;
        // @brief lateralDistance
        SUMOReal latDist;
        // @brief direction that was checked
        int dir;

        StateAndDist(int _state, SUMOReal _latDist, int _dir) :
            state(_state),
            latDist(_latDist),
            dir(_dir) {}
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
     * @param[in] model The type of lane change model
     */
    MSAbstractLaneChangeModel(MSVehicle& v, const LaneChangeModel model);

    /// @brief Destructor
    virtual ~MSAbstractLaneChangeModel();

    inline int getOwnState() const {
        return myOwnState;
    }

    virtual void setOwnState(int state) {
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
        MSVehicle** firstBlocked) {
        UNUSED_PARAMETER(laneOffset);
        UNUSED_PARAMETER(&msgPass);
        UNUSED_PARAMETER(blocked);
        UNUSED_PARAMETER(&leader);
        UNUSED_PARAMETER(&neighLead);
        UNUSED_PARAMETER(&neighFollow);
        UNUSED_PARAMETER(&neighLane);
        UNUSED_PARAMETER(&preb);
        UNUSED_PARAMETER(lastBlocked);
        UNUSED_PARAMETER(firstBlocked);
        throw ProcessError("Method not implemented by model " + toString(myModel));
    };

    virtual int wantsChangeSublane(
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
        SUMOReal& latDist, int& blocked) {
        UNUSED_PARAMETER(laneOffset);
        UNUSED_PARAMETER(&leaders);
        UNUSED_PARAMETER(&followers);
        UNUSED_PARAMETER(&blockers);
        UNUSED_PARAMETER(&neighLeaders);
        UNUSED_PARAMETER(&neighFollowers);
        UNUSED_PARAMETER(&neighBlockers);
        UNUSED_PARAMETER(&neighLane);
        UNUSED_PARAMETER(&preb);
        UNUSED_PARAMETER(lastBlocked);
        UNUSED_PARAMETER(firstBlocked);
        UNUSED_PARAMETER(latDist);
        UNUSED_PARAMETER(blocked);
        throw ProcessError("Method not implemented by model " + toString(myModel));
    }

    /// @brief update expected speeds for each sublane of the current edge
    virtual void updateExpectedSublaneSpeeds(const MSLeaderInfo& ahead, int sublaneOffset, int laneIndex) {
        UNUSED_PARAMETER(&ahead);
        UNUSED_PARAMETER(sublaneOffset);
        UNUSED_PARAMETER(laneIndex);
        throw ProcessError("Method not implemented by model " + toString(myModel));
    }

    /// @brief decide in which direction to move in case both directions are desirable
    virtual StateAndDist decideDirection(StateAndDist sd1, StateAndDist sd2) const {
        UNUSED_PARAMETER(sd1);
        UNUSED_PARAMETER(sd2);
        throw ProcessError("Method not implemented by model " + toString(myModel));
    }

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

    /* @brief called once when the primary lane of the vehicle changes (updates
     * the custom variables of each child implementation */
    virtual void changed() = 0;


    /// @brief whether the current vehicles shall be debugged
    virtual bool debugVehicle() const {
        return false;
    }

    /// @brief called when a vehicle changes between lanes in opposite directions
    void changedToOpposite();

    void unchanged() {
        if (myLastLaneChangeOffset > 0) {
            myLastLaneChangeOffset += DELTA_T;
        } else if (myLastLaneChangeOffset < 0) {
            myLastLaneChangeOffset -= DELTA_T;
        }
    }

    /** @brief Returns the lane the vehicles shadow is on during continuous/sublane lane change
     * @return The vehicle's shadow lane
     */
    MSLane* getShadowLane() const {
        return myShadowLane;
    }

    /// @brief return the shadow lane for the given lane
    MSLane* getShadowLane(const MSLane* lane) const;

    /// @brief set the shadow lane
    void setShadowLane(MSLane* lane) {
        myShadowLane = lane;
    }

    const std::vector<MSLane*>& getShadowFurtherLanes() const {
        return myShadowFurtherLanes;
    }

    const std::vector<SUMOReal>& getShadowFurtherLanesPosLat() const {
        return myShadowFurtherLanesPosLat;
    }

    inline SUMOTime getLastLaneChangeOffset() const {
        return myLastLaneChangeOffset;
    }


    /// @brief return whether the vehicle passed the midpoint of a continuous lane change maneuver
    inline bool pastMidpoint() const {
        return myLaneChangeCompletion >= 0.5;
    }

    /// @brief return whether the vehicle passed the midpoint of a continuous lane change maneuver
    SUMOTime remainingTime() const;

    /// @brief return true if the vehicle currently performs a lane change maneuver
    inline bool isChangingLanes() const {
        return myLaneChangeCompletion < (1 - NUMERICAL_EPS);
    }

    /// @brief return the direction of the current lane change maneuver
    inline int getLaneChangeDirection() const {
        return myLaneChangeDirection;
    }

    /// @brief return the direction in which the current shadow lane lies
    int getShadowDirection() const;

    /// @brief return the angle offset during a continuous change maneuver
    SUMOReal getAngleOffset() const;

    /// @brief return the lateral speed of the current lane change maneuver
    inline SUMOReal getLateralSpeed() const {
        return myLateralspeed;
    }

    /// @brief reset the flag whether a vehicle already moved to false
    inline bool alreadyChanged() const {
        return myAlreadyChanged;
    }

    /// @brief reset the flag whether a vehicle already moved to false
    void resetChanged() {
        myAlreadyChanged = false;
    }

    /// @brief start the lane change maneuver and return whether it continues
    bool startLaneChangeManeuver(MSLane* source, MSLane* target, int direction);

    /* @brief continue the lane change maneuver and return whether the midpoint
     * was passed in this step
     */
    bool updateCompletion();

    /* @brief update lane change shadow after the vehicle moved to a new lane */
    void updateShadowLane();

    /* @brief finish the lane change maneuver
     */
    void endLaneChangeManeuver(const MSMoveReminder::Notification reason = MSMoveReminder::NOTIFICATION_LANE_CHANGE);

    /* @brief clean up all references to the shadow vehicle
     */
    void cleanupShadowLane();

    /// @brief reserve space at the end of the lane to avoid dead locks
    virtual void saveBlockerLength(SUMOReal length) {
        UNUSED_PARAMETER(length);
    };

    void setShadowPartialOccupator(MSLane* lane) {
        myPartiallyOccupatedByShadow.push_back(lane);
    }

    void setNoShadowPartialOccupator(MSLane* lane) {
        myNoPartiallyOccupatedByShadow.push_back(lane);
    }

    /// @brief called once when the vehicles primary lane changes
    void primaryLaneChanged(MSLane* source, MSLane* target, int direction);

    /// @brief set approach information for the shadow vehicle
    void setShadowApproachingInformation(MSLink* link) const;
    void removeShadowApproachingInformation() const;

    bool isOpposite() const {
        return myAmOpposite;
    }

protected:
    virtual bool congested(const MSVehicle* const neighLeader);

    virtual bool predInteraction(const std::pair<MSVehicle*, SUMOReal>& leader);

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

    /// @brief The lateral offset during a continuous LaneChangeManeuver
    SUMOReal myLateralspeed;

    /// @brief whether the vehicle has already moved this step
    bool myAlreadyChanged;

    /// @brief A lane that is partially occupied by the front of the vehicle but that is not the primary lane
    MSLane* myShadowLane;

    /* @brief Lanes that are parially (laterally) occupied by the back of the
     * vehicle (analogue to MSVehicle::myFurtherLanes) */
    std::vector<MSLane*> myShadowFurtherLanes;
    std::vector<SUMOReal> myShadowFurtherLanesPosLat;

    /// @brief The vehicle's car following model
    const MSCFModel& myCarFollowModel;

    /// @brief the type of this model
    const LaneChangeModel myModel;

    /// @brief list of lanes where the shadow vehicle is partial occupator
    std::vector<MSLane*> myPartiallyOccupatedByShadow;

    /* @brief list of lanes where there is no shadow vehicle partial occupator
     * (when changing to a lane that has no predecessor) */
    std::vector<MSLane*> myNoPartiallyOccupatedByShadow;

    /* @brief to be called by derived classes in their changed() method.
     * If dir=0 is given, the current value remains unchanged */
    void initLastLaneChangeOffset(int dir);

    /// @brief whether overtaking on the right is permitted
    static bool myAllowOvertakingRight;

    /// @brief whether to record lane-changing
    static bool myLCOutput;

private:
    /* @brief information how long ago the vehicle has performed a lane-change,
     * sign indicates direction of the last change
     */
    SUMOTime myLastLaneChangeOffset;

    /// @brief links which are approached by the shadow vehicle
    mutable std::vector<MSLink*> myApproachedByShadow;

    /// @brief whether the vehicle is driving in the opposite direction
    bool myAmOpposite;


private:
    /// @brief Invalidated assignment operator
    MSAbstractLaneChangeModel& operator=(const MSAbstractLaneChangeModel& s);
};


#endif

/****************************************************************************/

