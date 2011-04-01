/****************************************************************************/
/// @file    MSAbstractLaneChangeModel.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Interface for lane-change models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MSLane.h"
#include "MSVehicle.h"


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


/** @enum ChangeRequest
 * @brief Requests set via TraCI
 */
enum ChangeRequest {
    /// @brief vehicle doesn't want to change
    REQUEST_NONE,
    /// @brief vehicle want's to change to left lane
    REQUEST_LEFT,
    /// @brief vehicle want's to change to right lane
    REQUEST_RIGHT,
    /// @brief vehicle want's to keep the current lane
    REQUEST_HOLD
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
        MSLCMessager(MSVehicle *leader,  MSVehicle *neighLead, MSVehicle *neighFollow) throw()
                : myLeader(leader), myNeighLeader(neighLead),
                myNeighFollower(neighFollow) { }


        /// @brief Destructor
        ~MSLCMessager() { }


        /** @brief Informs the leader on the same lane
         * @param[in] info The information to pass
         * @param[in] sender The sending vehicle (the lane changing vehicle)
         * @return Something!?
         */
        void *informLeader(void *info, MSVehicle *sender) throw() {
            assert(myLeader!=0);
            return myLeader->getLaneChangeModel().inform(info, sender);
        }


        /** @brief Informs the leader on the desired lane
         * @param[in] info The information to pass
         * @param[in] sender The sending vehicle (the lane changing vehicle)
         * @return Something!?
         */
        void *informNeighLeader(void *info, MSVehicle *sender) throw() {
            assert(myNeighLeader!=0);
            return myNeighLeader->getLaneChangeModel().inform(info, sender);
        }


        /** @brief Informs the follower on the desired lane
         * @param[in] info The information to pass
         * @param[in] sender The sending vehicle (the lane changing vehicle)
         * @return Something!?
         */
        void *informNeighFollower(void *info, MSVehicle *sender) throw() {
            assert(myNeighFollower!=0);
            return myNeighFollower->getLaneChangeModel().inform(info, sender);
        }


    private:
        /// @brief The leader on the informed vehicle's lane
        MSVehicle *myLeader;
        /// @brief The leader on the lane the vehicle want to change to
        MSVehicle *myNeighLeader;
        /// @brief The follower on the lane the vehicle want to change to
        MSVehicle *myNeighFollower;

    };



    /** @brief Constructor
     * @param[in] v The vehicle this lane-changer belongs to
     */
    MSAbstractLaneChangeModel(MSVehicle &v) throw()
            : myVehicle(v), myOwnState(0),
#ifndef NO_TRACI
            myChangeRequest(REQUEST_NONE),
#endif
            myCarFollowModel(v.getCarFollowModel()) {
    }


    /// @brief Destructor
    virtual ~MSAbstractLaneChangeModel() { }



    int getOwnState() const {
        return myOwnState;
    }

    void setOwnState(int state) {
        myOwnState = state;
    }

    virtual void prepareStep() { }

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &neighLead,
        const std::pair<MSVehicle*, SUMOReal> &neighFollow,
        const MSLane &neighLane,
        const std::vector<MSVehicle::LaneQ> &preb,
        MSVehicle **lastBlocked) = 0;

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::pair<MSVehicle*, SUMOReal> &neighLead,
        const std::pair<MSVehicle*, SUMOReal> &neighFollow,
        const MSLane &neighLane,
        const std::vector<MSVehicle::LaneQ> &preb,
        MSVehicle **lastBlocked) = 0;

    virtual void *inform(void *info, MSVehicle *sender) = 0;

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
                                const MSCFModel &cfModel) = 0;

    virtual void changed() = 0;

#ifndef NO_TRACI
    /**
     * The vehicle is requested to change the lane as soon as possible
     * without violating any directives defined by this lane change model
     *
     * @param request	indicates the requested change
     */
    virtual void requestLaneChange(ChangeRequest request) {
        myChangeRequest = request;
    };

    /**
     * Inform the model that a certain lane change request has been fulfilled
     * by the lane changer, so the request won't be taken into account the next time.
     *
     * @param request	indicates the request that was fulfilled
     */
    virtual void fulfillChangeRequest(ChangeRequest request) {
        if (request == myChangeRequest) {
            myChangeRequest = REQUEST_NONE;
        }
    }
#endif

protected:
    virtual bool congested(const MSVehicle * const neighLeader) {
        if (neighLeader==0) {
            return false;
        }
        // Congested situation are relevant only on highways (maxSpeed > 70km/h)
        // and congested on German Highways means that the vehicles have speeds
        // below 60km/h. Overtaking on the right is allowed then.
        if ((myVehicle.getLane()->getMaxSpeed() <= 70.0 / 3.6) || (neighLeader->getLane()->getMaxSpeed() <= 70.0 / 3.6)) {

            return false;
        }
        if (myVehicle.congested() && neighLeader->congested()) {
            return true;
        }
        return false;
    }

    virtual bool predInteraction(const MSVehicle * const leader) {
        if (leader==0) {
            return false;
        }
        // let's check it on highways only
        if (leader->getSpeed()<(80.0/3.6)) {
            return false;
        }
        SUMOReal gap = leader->getPositionOnLane() - leader->getVehicleType().getLength() - myVehicle.getPositionOnLane();
        return gap < myCarFollowModel.interactionGap(&myVehicle, leader->getSpeed());
    }



protected:
    /// @brief The vehicle this lane-changer belongs to
    MSVehicle &myVehicle;

    /// @brief The current state of the vehicle
    int myOwnState;


#ifndef NO_TRACI
    ChangeRequest myChangeRequest;
#endif

    /// @brief The vehicle's car following model
    const MSCFModel &myCarFollowModel;

};


#endif

/****************************************************************************/

