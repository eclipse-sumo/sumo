/****************************************************************************/
/// @file    MSVehicle.h
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Bjoern Hendriks
/// @author  Daniel Krajzewicz
/// @author  Thimor Bohn
/// @author  Friedemann Wesner
/// @author  Clemens Honomichl
/// @author  Michael Behrisch
/// @author  Axel Wegener
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Representation of a vehicle in the micro simulation
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
#ifndef MSVehicle_h
#define MSVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "MSGlobals.h"
#include "MSVehicleType.h"
#include "MSBaseVehicle.h"
#include "MSLink.h"
#include "MSLane.h"

#define INVALID_SPEED 299792458 + 1 // nothing can go faster than the speed of light! Refs. #2577

// ===========================================================================
// class declarations
// ===========================================================================
class SUMOSAXAttributes;
class MSMoveReminder;
class MSLaneChanger;
class MSVehicleTransfer;
class MSAbstractLaneChangeModel;
class MSStoppingPlace;
class MSChargingStation;
class MSParkingArea;
class MSPerson;
class MSDevice;
class MSEdgeWeightsStorage;
class OutputDevice;
class Position;
class MSDevice_Transportable;
class MSContainer;
class MSJunction;
class MSLeaderInfo;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicle
 * @brief Representation of a vehicle in the micro simulation
 */
class MSVehicle : public MSBaseVehicle {
public:

    /// the lane changer sets myLastLaneChangeOffset
    friend class MSLaneChanger;
    friend class MSLaneChangerSublane;

    /** @class State
     * @brief Container that holds the vehicles driving state (position+speed).
     */
    class State {
        /// @brief vehicle sets states directly
        friend class MSVehicle;
        friend class MSLaneChanger;
        friend class MSLaneChangerSublane;

    public:
        /// Constructor.
        State(SUMOReal pos, SUMOReal speed, SUMOReal posLat, SUMOReal backPos);

        /// Copy constructor.
        State(const State& state);

        /// Assignment operator.
        State& operator=(const State& state);

        /// Operator !=
        bool operator!=(const State& state);

        /// Position of this state.
        SUMOReal pos() const {
            return myPos;
        }

        /// Speed of this state
        SUMOReal speed() const {
            return mySpeed;
        };

        /// Lateral Position of this state (m relative to the centerline of the lane).
        SUMOReal posLat() const {
            return myPosLat;
        }

        /// back Position of this state
        SUMOReal backPos() const {
            return myBackPos;
        }

        /// previous Speed of this state
        SUMOReal lastCoveredDist() const {
            return myLastCoveredDist;
        }


    private:
        /// the stored position
        SUMOReal myPos;

        /// the stored speed (should be >=0 at any time)
        SUMOReal mySpeed;

        /// the stored lateral position
        SUMOReal myPosLat;

        /// @brief the stored back position
        // if the vehicle occupies multiple lanes, this is the position relative
        // to the lane occupied by its back
        SUMOReal myBackPos;

        /// the speed at the begin of the previous time step
        SUMOReal myPreviousSpeed;

        /// the distance covered in the last timestep
        /// NOTE: In case of ballistic positional update, this is not necessarily given by
        ///       myPos - SPEED2DIST(mySpeed + myPreviousSpeed)/2,
        /// because a stop may have occured within the last step.
        SUMOReal myLastCoveredDist;

    };


    /** @class WaitingTimeCollector
     * @brief Stores the waiting intervals over the previous seconds (memory is to be specified in ms.).
     */
    class WaitingTimeCollector {
        friend class MSVehicle;

        typedef std::list<std::pair<SUMOTime, SUMOTime> > waitingIntervalList;

    public:
        /// Constructor.
        WaitingTimeCollector(SUMOTime memory = MSGlobals::gWaitingTimeMemory);

        /// Copy constructor.
        WaitingTimeCollector(const WaitingTimeCollector& wt);

        /// Assignment operator.
        WaitingTimeCollector& operator=(const WaitingTimeCollector& wt);

        /// Operator !=
        bool operator!=(const WaitingTimeCollector& wt) const;

        /// Assignment operator (in place!)
        WaitingTimeCollector& operator=(SUMOTime t);

        // return the waiting time within the last memory millisecs
        SUMOTime cumulatedWaitingTime(SUMOTime memory = -1) const;

        // process time passing for dt millisecs
        void passTime(SUMOTime dt, bool waiting);

        // maximal memory time stored
        SUMOTime getMemorySize() const {
            return myMemorySize;
        }

        // maximal memory time stored
        const waitingIntervalList& getWaitingIntervals() const {
            return myWaitingIntervals;
        }

    private:
        /// the maximal memory to store
        SUMOTime myMemorySize;

        /// the stored waiting intervals within the last memory milliseconds
        /// If the current (ongoing) waiting interval has begun at time t - dt (where t is the current time)
        /// then waitingIntervalList[0]->first = 0., waitingIntervalList[0]->second = dt
        waitingIntervalList myWaitingIntervals;

        /// append an amount of dt millisecs to the stored waiting times
        void appendWaitingTime(SUMOTime dt);
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

    /** @brief Constructor
     * @param[in] pars The vehicle description
     * @param[in] route The vehicle's route
     * @param[in] type The vehicle's type
     * @param[in] speedFactor The factor for driven lane's speed limits
     * @exception ProcessError If a value is wrong
     */
    MSVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
              const MSVehicleType* type, const SUMOReal speedFactor);

    /// @brief Destructor.
    virtual ~MSVehicle();


    /// @name insertion/removal
    //@{

    /** @brief Called when the vehicle is removed from the network.
     *
     * Moves along work reminders and
     *  informs all devices about quitting. Calls "leaveLane" then.
     *
     * @param[in] reason why the vehicle leaves (reached its destination, parking, teleport)
     */
    void onRemovalFromNet(const MSMoveReminder::Notification reason);
    //@}



    /// @name interaction with the route
    //@{

    /** @brief Returns whether this vehicle has already arived
     * (reached the arrivalPosition on its final edge)
     */
    bool hasArrived() const;

    /** @brief Replaces the current route by the given one
     *
     * It is possible that the new route is not accepted, if it does not
     *  contain the vehicle's current edge.
     *
     * @param[in] route The new route to pass
     * @return Whether the new route was accepted
     */
    bool replaceRoute(const MSRoute* route, bool onInit = false, int offset = 0, bool addStops = true);


    /** @brief Returns whether the vehicle wil pass the given edge
     * @param[in] The edge to find in the vehicle's current route
     * @return Whether the given edge will be passed by the vehicle
     * @todo Move to MSRoute?
     */
    bool willPass(const MSEdge* const edge) const;

    int getRoutePosition() const;
    void resetRoutePosition(int index);

    /** @brief Returns the vehicle's internal edge travel times/efforts container
     *
     * If the vehicle does not have such a container, it is built.
     * @return The vehicle's knowledge about edge weights
     */
    const MSEdgeWeightsStorage& getWeightsStorage() const;
    MSEdgeWeightsStorage& getWeightsStorage();
    //@}


    /// @name Interaction with move reminders
    //@{

    /** @brief Processes active move reminder
     *
     * This method goes through all active move reminder, both those for the current
     *  lane, stored in "myMoveReminders" and those of prior lanes stored in
     *  "myOldLaneMoveReminders" calling "MSMoveReminder::notifyMove".
     *
     * When processing move reminder from "myOldLaneMoveReminders",
     *  the offsets (prior lane lengths) are used, which are stored in
     *  "myOldLaneMoveReminderOffsets".
     *
     * Each move reminder which is no longer active is removed from the container.
     *
     * @param[in] oldPos The position the vehicle had before it has moved
     * @param[in] newPos The position the vehicle has after it has moved
     * @param[in] newSpeed The vehicle's speed within this move
     * @see MSMoveReminder
     */
    void workOnMoveReminders(SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);
    //@}


    /** @brief Compute safe velocities for the upcoming lanes based on positions and
     * speeds from the last time step. Also registers
     * ApproachingVehicleInformation for all links
     *
     * This method goes through the best continuation lanes of the current lane and
     * computes the safe velocities for passing/stopping at the next link as a DriveProcessItem
     *
     * Afterwards it checks if any DriveProcessItem should be discared to avoid
     * blocking a junction (checkRewindLinkLanes).
     *
     * Finally the ApproachingVehicleInformation is registered for all links that
     * shall be passed
     *
     * @param[in] t The current timeStep
     * @param[in] ahead The leaders (may be 0)
     * @param[in] lengthsInFront Sum of vehicle lengths in front of the vehicle
     */
    void planMove(const SUMOTime t, const MSLeaderInfo& ahead, const SUMOReal lengthsInFront);


    /** @brief Executes planned vehicle movements with regards to right-of-way
     *
     * This method goes through all DriveProcessItems in myLFLinkLanes in order
     * to find a speed that is safe for all upcoming links.
     *
     * Using this speed the position is updated and the vehicle is moved to the
     * next lane (myLane is updated) if the end of the current lane is reached (this may happen
     * multiple times in this method)
     *
     * The vehicle also sets the lanes it is in-lapping into and informs them about it.
     * @return Whether the vehicle has moved to the next edge
     */
    bool executeMove();


    /** @brief calculates the distance covered in the next integration step given
     *         an acceleration and assuming the current velocity. (gives different
     *         results for different integration methods, e.g., euler vs. ballistic)
     *  @param[in] accel the assumed acceleration
     *  @return distance covered in next integration step
     */
    SUMOReal getDeltaPos(SUMOReal accel);


    /// @name state setter/getter
    //@{

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    SUMOReal getPositionOnLane() const {
        return myState.myPos;
    }

    /** @brief Get the distance the vehicle covered in the previous timestep
     * @return The distance covered in the last timestep (in m)
     */
    SUMOReal getLastStepDist() const {
        return myState.lastCoveredDist();
    }

    /** @brief Get the vehicle's front position relative to the given lane
     * @return The front position of the vehicle (in m from the given lane's begin)
     */
    SUMOReal getPositionOnLane(const MSLane* lane) const;

    /** @brief Get the vehicle's position relative to the given lane
     * @return The back position of the vehicle (in m from the given lane's begin)
     */
    SUMOReal getBackPositionOnLane(const MSLane* lane) const;

    /** @brief Get the vehicle's position relative to its current lane
     * @return The back position of the vehicle (in m from the current lane's begin)
     */
    SUMOReal getBackPositionOnLane() const {
        return getBackPositionOnLane(myLane);
    }

    /** @brief Get the vehicle's lateral position on the lane
     * @return The lateral position of the vehicle (in m relative to the
     * centerline of the lane)
     */
    SUMOReal getLateralPositionOnLane() const {
        return myState.myPosLat;
    }

    /** @brief Get the vehicle's lateral position on the lane:
     * @return The lateral position of the vehicle (in m distance between right
     * side of vehicle and ride side of the lane it is on
     */
    SUMOReal getRightSideOnLane() const;

    /// @brief return the amount by which the vehicle extends laterally outside it's primary lane
    SUMOReal getLateralOverlap() const;

    /** @brief Get the vehicle's lateral position on the edge of the given lane
     * (or its current edge if lane == 0)
     * @return The lateral position of the vehicle (in m distance between right
     * side of vehicle and ride side of edge
     */
    SUMOReal getRightSideOnEdge(const MSLane* lane = 0) const;

    /** @brief Get the vehicle's lateral position on the edge of the given lane
     * (or its current edge if lane == 0)
     * @return The lateral position of the vehicle (in m distance between center
     * of vehicle and ride side of edge
     */
    SUMOReal getCenterOnEdge(const MSLane* lane = 0) const;

    /** @brief Get the offset that that must be added to interpret
     * myState.myPosLat for the given lane
     */
    SUMOReal getLatOffset(const MSLane* lane) const;

    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    SUMOReal getSpeed() const {
        return myState.mySpeed;
    }


    /** @brief Returns the vehicle's speed before the previous time step
     * @return The vehicle's speed before the previous time step
     */
    SUMOReal getPreviousSpeed() const {
        return myState.myPreviousSpeed;
    }


    /** @brief Returns the vehicle's acceleration in m/s
     *         (this is computed as the last step's mean acceleration in case that a stop occurs within the middle of the time-step)
     * @return The acceleration
     */
    SUMOReal getAcceleration() const {
        return myAcceleration;
    }
    //@}



    /// @name Other getter methods
    //@{

    /** @brief Returns the slope of the road at vehicle's position
     * @return The slope
     */
    SUMOReal getSlope() const;


    /** @brief Return current position (x/y, cartesian)
     *
     * If the vehicle's myLane is 0, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    Position getPosition(const SUMOReal offset = 0) const;


    /** @brief Returns the lane the vehicle is on
     * @return The vehicle's current lane
     */
    MSLane* getLane() const {
        return myLane;
    }


    /** @brief Returns the maximal speed for the vehicle on its current lane (including speed factor and deviation,
     *         i.e., not necessarily the allowed speed limit)
     * @return The vehicle's max speed
     */
    SUMOReal
    getMaxSpeedOnLane() const {
        if (myLane != 0) {
            return myLane->getVehicleMaxSpeed(this);
        } else {
            return myType->getMaxSpeed();
        }
    }


    /** @brief Returns the information whether the vehicle is on a road (is simulated)
     * @return Whether the vehicle is simulated
     */
    inline bool isOnRoad() const {
        return myAmOnNet;
    }


    /** @brief Returns the information whether the front of the vehicle is on the given lane
     * @return Whether the vehicle's front is on that lane
     */
    bool isFrontOnLane(const MSLane* lane) const;


    /** @brief Returns the starting point for reroutes (usually the current edge)
     *
     * This differs from *myCurrEdge only if the vehicle is on an internal edge or
     *  very close to the junction
     * @return The rerouting start point
     */
    const MSEdge* getRerouteOrigin() const;


    /** @brief Returns the SUMOTime waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * Intentional stopping does not count towards this time.
     * @return The time the vehicle is standing
     */
    SUMOTime getWaitingTime() const {
        return myWaitingTime;
    }

    /** @brief Returns the SUMOTime lost (speed was lesser maximum speed)
     *
     * @note Intentional stopping does not count towards this time.
    // @note speedFactor is included so time loss can never be negative.
    // The value is that of a driver who compares his travel time when
    // the road is clear (which includes speed factor) with the actual travel time.
    // @note includes time lost due to low departSpeed and decelerating/accelerating for planned stops
     * @return The time the vehicle lost due to various effects
     */
    SUMOTime getTimeLoss() const {
        return myTimeLoss;
    }


    /** @brief Returns the SUMOTime waited (speed was lesser than 0.1m/s) within the last t millisecs
     *
     * @param[in] t specifies the length of the interval over which the cumulated waiting time is to be summed up (defaults to and must not exceed MSGlobals::gWaitingTimeMemory)
     * @return The time the vehicle was standing within the last t millisecs
     */
    SUMOTime getAccumulatedWaitingTime(SUMOTime t = MSGlobals::gWaitingTimeMemory) const {
        return myWaitingTimeCollector.cumulatedWaitingTime(t);
    }


    /** @brief Returns the number of seconds waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * Intentional stopping does not count towards this time.
     * @return The time the vehicle is standing
     */
    SUMOReal getWaitingSeconds() const {
        return STEPS2TIME(myWaitingTime);
    }


    /** @brief Returns the number of seconds waited (speed was lesser than 0.1m/s) within the last millisecs
     *
     * @return The time the vehicle was standing within the last t millisecs
     */

    SUMOReal getAccumulatedWaitingSeconds() const {
        return STEPS2TIME(getAccumulatedWaitingTime());
    }

    /** @brief Returns the time loss in seconds
     */
    SUMOReal getTimeLossSeconds() const {
        return STEPS2TIME(myTimeLoss);
    }


    /** @brief Returns the vehicle's direction in radians
     * @return The vehicle's current angle
     */
    SUMOReal getAngle() const {
        return myAngle;
    }
    //@}

    /// @brief compute the current vehicle angle
    SUMOReal computeAngle() const;

    /// @brief Set a custom vehicle angle in rad
    void setAngle(SUMOReal angle);

    /** Returns true if the two vehicles overlap. */
    static bool overlap(const MSVehicle* veh1, const MSVehicle* veh2) {
        if (veh1->myState.myPos < veh2->myState.myPos) {
            return veh2->myState.myPos - veh2->getVehicleType().getLengthWithGap() < veh1->myState.myPos;
        }
        return veh1->myState.myPos - veh1->getVehicleType().getLengthWithGap() < veh2->myState.myPos;
    }


    /** Returns true if vehicle's speed is below 60km/h. This is only relevant
        on highways. Overtaking on the right is allowed then. */
    bool congested() const {
        return myState.mySpeed < SUMOReal(60) / SUMOReal(3.6);
    }


    /** @brief "Activates" all current move reminder
     *
     * For all move reminder stored in "myMoveReminders", their method
     *  "MSMoveReminder::notifyEnter" is called.
     *
     * @param[in] reason The reason for changing the reminders' states
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    void activateReminders(const MSMoveReminder::Notification reason);

    /** @brief Update when the vehicle enters a new lane in the move step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] onTeleporting Whether the lane was entered while being teleported
     * @return Whether the vehicle's route has ended (due to vaporization, or because the destination was reached)
     */
    bool enterLaneAtMove(MSLane* enteredLane, bool onTeleporting = false);



    /** @brief Update when the vehicle enters a new lane in the emit step
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] pos The position the vehicle was inserted into the lane
     * @param[in] speed The speed with which the vehicle was inserted into the lane
     * @param[in] posLat The lateral position the vehicle was inserted into the lane
     * @param[in] notification The cause of insertion (i.e. departure, teleport, parking)
     */
    void enterLaneAtInsertion(MSLane* enteredLane, SUMOReal pos, SUMOReal speed, SUMOReal posLat,
                              MSMoveReminder::Notification notification);

    /** @brief set tentative lane and position during insertion to ensure that
     * all cfmodels work (some of them require veh->getLane() to return a valid lane)
     * Once the vehicle is sucessfully inserted the lane is set again (see enterLaneAtInsertion)
     */
    void setTentativeLaneAndPosition(MSLane* lane, SUMOReal pos, SUMOReal posLat = 0);

    /** @brief Update when the vehicle enters a new lane in the laneChange step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     */
    void enterLaneAtLaneChange(MSLane* enteredLane);


    /** @brief Update of members if vehicle leaves a new lane in the lane change step or at arrival. */
    void leaveLane(const MSMoveReminder::Notification reason);


    MSAbstractLaneChangeModel& getLaneChangeModel();
    const MSAbstractLaneChangeModel& getLaneChangeModel() const;

    const std::vector<MSLane*>& getFurtherLanes() const {
        return myFurtherLanes;
    }

    const std::vector<SUMOReal>& getFurtherLanesPosLat() const {
        return myFurtherLanesPosLat;
    }

    /// @name strategical/tactical lane choosing methods
    /// @{

    //
    /** @struct LaneQ
     * @brief A structure representing the best lanes for continuing the current route starting at 'lane'
     */
    struct LaneQ {
        /// @brief The described lane
        MSLane* lane;
        /// @brief The overall length which may be driven when using this lane without a lane change
        SUMOReal length;
        /// @brief The length which may be driven on this lane
        SUMOReal currentLength;
        /// @brief The overall vehicle sum on consecutive lanes which can be passed without a lane change
        SUMOReal occupation;
        /// @brief As occupation, but without the first lane
        SUMOReal nextOccupation;
        /// @brief The (signed) number of lanes to be crossed to get to the lane which allows to continue the drive
        int bestLaneOffset;
        /// @brief Whether this lane allows to continue the drive
        bool allowsContinuation;
        /* @brief Longest sequence of (normal-edge) lanes that can be followed without a lane change
         * The 'length' attribute is the sum of these lane lengths
         * (There may be alternative sequences that have equal length)
         * It is the 'best' in the strategic sense of reducing required lane-changes
         */
        std::vector<MSLane*> bestContinuations;
    };

    /** @brief Returns the description of best lanes to use in order to continue the route
     * @return The LaneQ for all lanes of the current edge
     */
    const std::vector<LaneQ>& getBestLanes() const;

    /** @brief computes the best lanes to use in order to continue the route
     *
     * The information is rebuilt if the vehicle is on a different edge than
     *  the one stored in "myLastBestLanesEdge" or "forceRebuild" is true.
     *
     * Otherwise, only the density changes on the stored lanes are adapted to
     *  the container only.
     *
     * A rebuild must be done if the vehicle leaves a stop; then, another lane may become
     *  the best one.
     *
     * If no starting lane ("startLane") is given, the vehicle's current lane ("myLane")
     *  is used as start of bect lanes building.
     *
     * @param[in] forceRebuild Whether the best lanes container shall be rebuilt even if the vehicle's edge has not changed
     * @param[in] startLane The lane the process shall start at ("myLane" will be used if ==0)
     */
    void updateBestLanes(bool forceRebuild = false, const MSLane* startLane = 0);


    /** @brief Returns the best sequence of lanes to continue the route starting at myLane
     * @return The bestContinuations of the LaneQ for myLane (see LaneQ)
     */
    const std::vector<MSLane*>& getBestLanesContinuation() const;


    /** @brief Returns the best sequence of lanes to continue the route starting at the given lane
     * @return The bestContinuations of the LaneQ for the given lane (see LaneQ)
     */
    const std::vector<MSLane*>& getBestLanesContinuation(const MSLane* const l) const;

    /* @brief returns the current signed offset from the lane that is most
     * suited for continuing the current route (in the strategic sense of reducing lane-changes)
     * - 0 if the vehicle is one it's best lane
     * - negative if the vehicle should change to the right
     * - positive if the vehicle should change to the left
     */
    int getBestLaneOffset() const;

    /// @brief update occupation from MSLaneChanger
    void adaptBestLanesOccupation(int laneIndex, SUMOReal density);

    /// @}

    /// @brief repair errors in vehicle position after changing between internal edges
    void fixPosition();


    /** @brief Replaces the current vehicle type by the one given
     *
     * If the currently used vehicle type is marked as being used by this vehicle
     *  only, it is deleted, first. The new, given type is then assigned to
     *  "myType".
     * @param[in] type The new vehicle type
     * @see MSVehicle::myType
     */
    void replaceVehicleType(MSVehicleType* type);


    /** @brief Returns the vehicle's car following model definition
     *
     * This is simply a wrapper around the vehicle type's car-following
     *  model retrieval for a shorter access.
     *
     * @return The vehicle's car following model definition
     */
    inline const MSCFModel& getCarFollowModel() const {
        return myType->getCarFollowModel();
    }


    /** @brief Returns the vehicle's car following model variables
     *
     * @return The vehicle's car following model variables
     */
    inline MSCFModel::VehicleVariables* getCarFollowVariables() const {
        return myCFVariables;
    }


    /// @name vehicle stops definitions and i/o
    //@{

    /** @struct Stop
     * @brief Definition of vehicle stop (position and duration)
     */
    struct Stop {
        /// @brief The edge in the route to stop at
        MSRouteIterator edge;
        /// @brief The lane to stop at
        const MSLane* lane;
        /// @brief (Optional) bus stop if one is assigned to the stop
        MSStoppingPlace* busstop;
        /// @brief (Optional) container stop if one is assigned to the stop
        MSStoppingPlace* containerstop;
        /// @brief (Optional) parkingArea if one is assigned to the stop
        MSParkingArea* parkingarea;
        /// @brief (Optional) charging station if one is assigned to the stop
        MSChargingStation* chargingStation;
        /// @brief The stopping position start
        SUMOReal startPos;
        /// @brief The stopping position end
        SUMOReal endPos;
        /// @brief The stopping duration
        SUMOTime duration;
        /// @brief The time at which the vehicle may continue its journey
        SUMOTime until;
        /// @brief whether an arriving person lets the vehicle continue
        bool triggered;
        /// @brief whether an arriving container lets the vehicle continue
        bool containerTriggered;
        /// @brief whether the vehicle is removed from the net while stopping
        bool parking;
        /// @brief Information whether the stop has been reached
        bool reached;
        /// @brief IDs of persons the vehicle has to wait for until departing
        std::set<std::string> awaitedPersons;
        /// @brief IDs of containers the vehicle has to wait for until departing
        std::set<std::string> awaitedContainers;
        /// @brief The time at which the vehicle is able to board another person
        SUMOTime timeToBoardNextPerson;
        /// @brief The time at which the vehicle is able to load another container
        SUMOTime timeToLoadNextContainer;

        void write(OutputDevice& dev) const;

        /// @brief return halting position for upcoming stop;
        SUMOReal getEndPos(const SUMOVehicle& veh) const;
    };


    /** @brief Adds a stop
     *
     * The stop is put into the sorted list.
     * @param[in] stop The stop to add
     * @return Whether the stop could be added
     */
    bool addStop(const SUMOVehicleParameter::Stop& stopPar, std::string& errorMsg, SUMOTime untilOffset = 0);

    /** @brief replace the current parking area stop with a new stop with merge duration
     */
    bool replaceParkingArea(MSParkingArea* parkingArea, std::string& errorMsg);


    /** @brief get the current parking area stop
     */
    MSParkingArea* getNextParkingArea();

    /** @brief Returns whether the vehicle has to stop somewhere
     * @return Whether the vehicle has to stop somewhere
     */
    bool hasStops() const {
        return !myStops.empty();
    }

    /** @brief Returns whether the vehicle is at a stop
     * @return Whether the vehicle has stopped
     */
    bool isStopped() const;

    /** @brief Returns whether the vehicle is parking
     * @return whether the vehicle is parking
     */
    bool isParking() const;

    /** @brief Returns the information whether the vehicle is fully controlled via TraCI
     * @return Whether the vehicle is remote-controlled
     */
    bool isRemoteControlled() const;

    /// @brief return the distance to the next stop or SUMORealMax if there is none.
    SUMOReal nextStopDist() const {
        return myStopDist;
    }

    /** @brief Returns whether the vehicle is on a triggered stop
     * @return whether the vehicle is on a triggered stop
     */
    bool isStoppedTriggered() const;

    /** @brief return whether the given position is within range of the current stop
     */
    bool isStoppedInRange(SUMOReal pos) const;
    /// @}

    bool knowsEdgeTest(MSEdge& edge) const;
    int getLaneIndex() const;

    /**
     * Compute distance that will be covered, if the vehicle moves to a given position on its route,
     * starting at its current position.
     * @param destPos:  position on the destination edge that shall be reached
     * @param destEdge: destination edge that shall be reached
     * @return      distance from the vehicles current position to the destination position,
     *          or a near infinite real value if the destination position is not contained
     *          within the vehicles route or the vehicle is not active
     */
    SUMOReal getDistanceToPosition(SUMOReal destPos, const MSEdge* destEdge) const;


    /** @brief Processes stops, returns the velocity needed to reach the stop
     * @return The velocity in dependance to the next/current stop
     * @todo Describe more detailed
     * @see Stop
     * @see MSStoppingPlace
     * @see MSStoppingPlace
     */
    SUMOReal processNextStop(SUMOReal currentVelocity);

    /** @brief Returns the leader of the vehicle looking for a fixed distance.
     *
     * If the distance is not given it is calculated from the brake gap.
     * The gap returned does not include the minGap.
     * @param dist    up to which distance to look for a leader
     * @return The leading vehicle together with the gap; (0, -1) if no leader was found.
     */
    std::pair<const MSVehicle* const, SUMOReal> getLeader(SUMOReal dist = 0) const;

    /** @brief Returns the time gap in seconds to the leader of the vehicle on the same lane.
     *
     * If the distance is too big -1 is returned.
     * The gap returned takes the minGap into account.
     * @return The time gap in seconds; -1 if no leader was found or speed is 0.
     */
    SUMOReal getTimeGapOnLane() const;


    /// @name Emission retrieval
    //@{

    /** @brief Returns CO2 emission of the current state
     * @return The current CO2 emission
     */
    SUMOReal getCO2Emissions() const;


    /** @brief Returns CO emission of the current state
     * @return The current CO emission
     */
    SUMOReal getCOEmissions() const;


    /** @brief Returns HC emission of the current state
     * @return The current HC emission
     */
    SUMOReal getHCEmissions() const;


    /** @brief Returns NOx emission of the current state
     * @return The current NOx emission
     */
    SUMOReal getNOxEmissions() const;


    /** @brief Returns PMx emission of the current state
     * @return The current PMx emission
     */
    SUMOReal getPMxEmissions() const;


    /** @brief Returns fuel consumption of the current state
    * @return The current fuel consumption
    */
    SUMOReal getFuelConsumption() const;


    /** @brief Returns electricity consumption of the current state
    * @return The current electricity consumption
    */
    SUMOReal getElectricityConsumption() const;


    /** @brief Returns noise emissions of the current state
     * @return The noise produced
     */
    SUMOReal getHarmonoise_NoiseEmissions() const;
    //@}



    /// @name Interaction with persons
    //@{

    /** @brief Adds a passenger
     * @param[in] person The person to add
     */
    void addPerson(MSTransportable* person);

    /// @name Interaction with containers
    //@{

    /** @brief Adds a container
     * @param[in] container The container to add
     */
    void addContainer(MSTransportable* container);

    /// @brief removes a person or container
    void removeTransportable(MSTransportable* t);

    /// @brief retrieve riding persons
    const std::vector<MSTransportable*>& getPersons() const;

    /// @brief retrieve riding containers
    const std::vector<MSTransportable*>& getContainers() const;

    /** @brief Returns the number of persons
     * @return The number of passengers on-board
     */
    int getPersonNumber() const;

    /** @brief Returns the number of containers
     * @return The number of contaiers on-board
     */
    int getContainerNumber() const;

    /// @name Access to bool signals
    /// @{

    /** @enum Signalling
     * @brief Some boolean values which describe the state of some vehicle parts
     */
    enum Signalling {
        /// @brief Everything is switched off
        VEH_SIGNAL_NONE = 0,
        /// @brief Right blinker lights are switched on
        VEH_SIGNAL_BLINKER_RIGHT = 1,
        /// @brief Left blinker lights are switched on
        VEH_SIGNAL_BLINKER_LEFT = 2,
        /// @brief Blinker lights on both sides are switched on
        VEH_SIGNAL_BLINKER_EMERGENCY = 4,
        /// @brief The brake lights are on
        VEH_SIGNAL_BRAKELIGHT = 8,
        /// @brief The front lights are on (no visualisation)
        VEH_SIGNAL_FRONTLIGHT = 16,
        /// @brief The fog lights are on (no visualisation)
        VEH_SIGNAL_FOGLIGHT = 32,
        /// @brief The high beam lights are on (no visualisation)
        VEH_SIGNAL_HIGHBEAM = 64,
        /// @brief The backwards driving lights are on (no visualisation)
        VEH_SIGNAL_BACKDRIVE = 128,
        /// @brief The wipers are on
        VEH_SIGNAL_WIPER = 256,
        /// @brief One of the left doors is opened
        VEH_SIGNAL_DOOR_OPEN_LEFT = 512,
        /// @brief One of the right doors is opened
        VEH_SIGNAL_DOOR_OPEN_RIGHT = 1024,
        /// @brief A blue emergency light is on
        VEH_SIGNAL_EMERGENCY_BLUE = 2048,
        /// @brief A red emergency light is on
        VEH_SIGNAL_EMERGENCY_RED = 4096,
        /// @brief A yellow emergency light is on
        VEH_SIGNAL_EMERGENCY_YELLOW = 8192
    };


    /** @brief modes for resolving conflicts between external control (traci)
     * and vehicle control over lane changing. Each level of the lane-changing
     * hierarchy (strategic, cooperative, speedGain, keepRight) can be controlled
     * separately */
    enum LaneChangeMode {
        LC_NEVER      = 0,  // lcModel shall never trigger changes at this level
        LC_NOCONFLICT = 1,  // lcModel may trigger changes if not in conflict with TraCI request
        LC_ALWAYS     = 2   // lcModel may always trigger changes of this level regardless of requests
    };


    /// @brief modes for prioritizing traci lane change requests
    enum TraciLaneChangePriority {
        LCP_ALWAYS        = 0,  // change regardless of blockers, adapt own speed and speed of blockers
        LCP_NOOVERLAP     = 1,  // change unless overlapping with blockers, adapt own speed and speed of blockers
        LCP_URGENT        = 2,  // change if not blocked, adapt own speed and speed of blockers
        LCP_OPPORTUNISTIC = 3   // change if not blocked
    };


    /** @brief Switches the given signal on
     * @param[in] signal The signal to mark as being switched on
     */
    void switchOnSignal(int signal) {
        mySignals |= signal;
    }


    /** @brief Switches the given signal off
     * @param[in] signal The signal to mark as being switched off
     */
    void switchOffSignal(int signal) {
        mySignals &= ~signal;
    }


    /** @brief Returns the signals
     * @return The signals' states
     */
    int getSignals() const {
        return mySignals;
    }


    /** @brief Returns whether the given signal is on
     * @param[in] signal The signal to return the value of
     * @return Whether the given signal is on
     */
    bool signalSet(int which) const {
        return (mySignals & which) != 0;
    }
    /// @}


    /// @brief whether the vehicle may safely move to the given lane with regard to upcoming links
    bool unsafeLinkAhead(const MSLane* lane) const;


#ifndef NO_TRACI
    /** @brief Returns the uninfluenced velocity
     *
     * If no influencer exists (myInfluencer==0) the vehicle's current speed is
     *  returned. Otherwise the speed returned from myInfluencer->getOriginalSpeed().
     * @return The vehicle's velocity as it would without an influence
     * @see Influencer::getOriginalSpeed
     */
    SUMOReal getSpeedWithoutTraciInfluence() const;

    /**
     * schedule a new stop for the vehicle; each time a stop is reached, the vehicle
     * will wait for the given duration before continuing on its route
     * @param lane     lane on wich to stop
     * @param startPos start position on the given lane at wich to stop
     * @param endPos   end position on the given lane at wich to stop
     * @param duration waiting time duration
     * @param until    time step at which the stop shall end
     * @param parking  a flag indicating whether the traci stop is used for parking or not
     * @param triggered a flag indicating whether the traci stop is triggered or not
     * @param containerTriggered a flag indicating whether the traci stop is triggered by a container or not
     */
    bool addTraciStop(MSLane* const lane, const SUMOReal startPos, const SUMOReal endPos, const SUMOTime duration, const SUMOTime until,
                      const bool parking, const bool triggered, const bool containerTriggered, std::string& errorMsg);

    /**
     * schedule a new stop for the vehicle; each time a stop is reached, the vehicle
     * will wait for the given duration before continuing on its route
     * @param stopId    bus or container stop id
     * @param duration waiting time duration
     * @param until    time step at which the stop shall end
     * @param parking   a flag indicating whether the traci stop is used for parking or not
     * @param triggered a flag indicating whether the traci stop is triggered or not
     * @param containerTriggered a flag indicating whether the traci stop is triggered by a container or not
     * @param isContainerStop a flag indicating whether the stop is a container stop
     */
    bool addTraciBusOrContainerStop(const std::string& stopId, const SUMOTime duration, const SUMOTime until, const bool parking,
                                    const bool triggered, const bool containerTriggered, const bool isContainerStop, std::string& errorMsg);

    /**
    * returns the next imminent stop in the stop queue
    * @return the upcoming stop
    */
    Stop& getNextStop();

    /**
    * resumes a vehicle from stopping
    * @return true on success, the resuming fails if the vehicle wasn't parking in the first place
    */
    bool resumeFromStopping();


    /// @brief update a vector of further lanes and return the new backPos
    SUMOReal updateFurtherLanes(std::vector<MSLane*>& furtherLanes,
                                std::vector<SUMOReal>& furtherLanesPosLat,
                                const std::vector<MSLane*>& passedLanes);

    /// @brief get bounding rectangle
    PositionVector getBoundingBox() const;

    /// @brief get bounding polygon
    PositionVector getBoundingPoly() const;

    /** @class Influencer
     * @brief Changes the wished vehicle speed / lanes
     *
     * The class is used for passing velocities or velocity profiles obtained via TraCI to the vehicle.
     * The speed adaptation is controlled by the stored speedTimeLine
     * Additionally, the variables myConsiderSafeVelocity, myConsiderMaxAcceleration, and myConsiderMaxDeceleration
     * control whether the safe velocity, the maximum acceleration, and the maximum deceleration
     * have to be regarded.
     *
     * Furthermore this class is used to affect lane changing decisions according to
     * LaneChangeMode and any given laneTimeLine
     */
    class Influencer {
    public:
        /// @brief Constructor
        Influencer();


        /// @brief Destructor
        ~Influencer();


        /** @brief Sets a new velocity timeline
         * @param[in] speedTimeLine The time line of speeds to use
         */
        void setSpeedTimeLine(const std::vector<std::pair<SUMOTime, SUMOReal> >& speedTimeLine);


        /** @brief Sets a new lane timeline
         * @param[in] laneTimeLine The time line of lanes to use
         */
        void setLaneTimeLine(const std::vector<std::pair<SUMOTime, int> >& laneTimeLine);

        /// @brief return the current speed mode
        int getSpeedMode() const;

        /** @brief Applies stored velocity information on the speed to use
         *
         * The given speed is assumed to be the non-influenced speed from longitudinal control.
         *  It is stored for further usage in "myOriginalSpeed".
         * @param[in] currentTime The current simulation time
         * @param[in] speed The undisturbed speed
         * @param[in] vSafe The safe velocity
         * @param[in] vMin The minimum velocity
         * @param[in] vMax The maximum simulation time
         * @return The speed to use
         */
        SUMOReal influenceSpeed(SUMOTime currentTime, SUMOReal speed, SUMOReal vSafe, SUMOReal vMin, SUMOReal vMax);

        /** @brief Applies stored LaneChangeMode information and laneTimeLine
         * @param[in] currentTime The current simulation time
         * @param[in] currentEdge The current edge the vehicle is on
         * @param[in] currentLaneIndex The index of the lane the vehicle is currently on
         * @param[in] state The LaneChangeAction flags as computed by the laneChangeModel
         * @return The new LaneChangeAction flags to use
         */
        int influenceChangeDecision(const SUMOTime currentTime, const MSEdge& currentEdge, const int currentLaneIndex, int state);


        /** @brief Return the remaining number of seconds of the current
         * laneTimeLine assuming one exists
         * @param[in] currentTime The current simulation time
         * @return The remaining seconds to change lanes
         */
        SUMOReal changeRequestRemainingSeconds(const SUMOTime currentTime) const;

        /** @brief Returns whether junction priority rules shall be respected
         * @return Whether junction priority rules be respected
         */
        inline bool getRespectJunctionPriority() const {
            return myRespectJunctionPriority;
        }


        /** @brief Returns whether red lights shall be a reason to brake
         * @return Whether red lights shall be a reason to brake
         */
        inline bool getEmergencyBrakeRedLight() const {
            return myEmergencyBrakeRedLight;
        }

        /** @brief Sets speed-constraining behaviors
         * @param[in] value a bitset controlling the different modes
         */
        void setSpeedMode(int speedMode);

        /** @brief Sets lane changing behavior
         * @param[in] value a bitset controlling the different modes
         */
        void setLaneChangeMode(int value);


        /** @brief Returns the originally longitudinal speed to use
         * @return The speed given before influence
         */
        inline SUMOReal getOriginalSpeed() const {
            return myOriginalSpeed;
        }

        void setVTDControlled(Position xyPos, MSLane* l, SUMOReal pos, SUMOReal posLat, SUMOReal angle, int edgeOffset, const ConstMSEdgeVector& route, SUMOTime t);

        SUMOTime getLastAccessTimeStep() const {
            return myLastVTDAccess;
        }

        void postProcessVTD(MSVehicle* v);

        /// @brief return the speed that is implicit in the new VTD position
        SUMOReal implicitSpeedVTD(const MSVehicle* veh, SUMOReal oldSpeed);

        /// @brief return the change in longitudinal position that is implicit in the new VTD position
        SUMOReal implicitDeltaPosVTD(const MSVehicle* veh);

        bool isVTDControlled() const;

        bool isVTDAffected(SUMOTime t) const;

        void setSignals(int signals) {
            myTraCISignals = signals;
        }

        int getSignals() const {
            return myTraCISignals;
        }

    private:
        /// @brief The velocity time line to apply
        std::vector<std::pair<SUMOTime, SUMOReal> > mySpeedTimeLine;

        /// @brief The lane usage time line to apply
        std::vector<std::pair<SUMOTime, int> > myLaneTimeLine;

        /// @brief The velocity before influence
        SUMOReal myOriginalSpeed;

        /// @brief Whether influencing the speed has already started
        bool mySpeedAdaptationStarted;

        /// @brief Whether the safe velocity shall be regarded
        bool myConsiderSafeVelocity;

        /// @brief Whether the maximum acceleration shall be regarded
        bool myConsiderMaxAcceleration;

        /// @brief Whether the maximum deceleration shall be regarded
        bool myConsiderMaxDeceleration;

        /// @brief Whether the junction priority rules are respected
        bool myRespectJunctionPriority;

        /// @brief Whether red lights are a reason to brake
        bool myEmergencyBrakeRedLight;

        Position myVTDXYPos;
        MSLane* myVTDLane;
        SUMOReal myVTDPos;
        SUMOReal myVTDPosLat;
        SUMOReal myVTDAngle;
        int myVTDEdgeOffset;
        ConstMSEdgeVector myVTDRoute;
        SUMOTime myLastVTDAccess;

        /// @name Flags for managing conflicts between the laneChangeModel and TraCI laneTimeLine
        //@{
        /// @brief lane changing which is necessary to follow the current route
        LaneChangeMode myStrategicLC;
        /// @brief lane changing with the intent to help other vehicles
        LaneChangeMode myCooperativeLC;
        /// @brief lane changing to travel with higher speed
        LaneChangeMode mySpeedGainLC;
        /// @brief changing to the rightmost lane
        LaneChangeMode myRightDriveLC;
        /// @brief changing to the prefered lateral alignment
        LaneChangeMode mySublaneLC;
        //@}
        ///* @brief flags for determining the priority of traci lane change requests
        TraciLaneChangePriority myTraciLaneChangePriority;

        // @brief the signals set via TraCI
        int myTraCISignals;

    };


    /** @brief Returns the velocity/lane influencer
     *
     * If no influencer was existing before, one is built, first
     * @return Reference to this vehicle's speed influencer
     */
    Influencer& getInfluencer();

    const Influencer* getInfluencer() const;

    bool hasInfluencer() const {
        return myInfluencer != 0;
    }

    /// @brief allow TraCI to influence a lane change decision
    int influenceChangeDecision(int state);

    /// @brief sets position outside the road network
    void setVTDState(Position xyPos);

    /// @brief compute safe speed for following the given leader
    SUMOReal getSafeFollowSpeed(const std::pair<const MSVehicle*, SUMOReal> leaderInfo,
                                const SUMOReal seen, const MSLane* const lane, SUMOReal distToCrossing) const;

    /// @brief get a numerical value for the priority of the  upcoming link
    static int nextLinkPriority(const std::vector<MSLane*>& conts);

#endif

    /// @name state io
    //@{

    /// Saves the states of a vehicle
    void saveState(OutputDevice& out);

    /** @brief Loads the state of this vehicle from the given description
     */
    void loadState(const SUMOSAXAttributes& attrs, const SUMOTime offset);
    //@}

protected:

    SUMOReal getSpaceTillLastStanding(const MSLane* l, bool& foundStopped) const;

    /// @name Interaction with move reminders
    ///@{

    /** @brief Adapts the vehicle's entering of a new lane
     *
     * All offsets already stored in "myOldLaneMoveReminderOffsets" are increased by the
     *  length that has been left. All still active move reminders from "myMoveReminders"
     *  are put into "myOldLaneMoveReminders" and the offset to the last lane is added to
     *  "myOldLaneMoveReminderOffsets" for each of these.
     *
     * Move reminder from the given lane are set into "myMoveReminders".
     *
     * "myLane" must still be the left lane!
     *
     * @param[in] enteredLane
     * @see MSMoveReminder
     * @see MSLane::getMoveReminder
     */
    void adaptLaneEntering2MoveReminder(const MSLane& enteredLane);
    ///@}



    void setBlinkerInformation();

    /** @brief sets the blue flashing light for emergency vehicles
     */
    void setEmergencyBlueLight(SUMOTime currentTime);

    /// updates LaneQ::nextOccupation and myCurrentLaneInBestLanes
    void updateOccupancyAndCurrentBestLane(const MSLane* startLane);

    /** @brief Returns the list of still pending stop edges
     */
    const ConstMSEdgeVector getStopEdges() const;

    /// @brief register vehicle for drawing while outside the network
    virtual void drawOutsideNetwork(bool /*add*/) {};

    /// @brief The time the vehicle waits (is not faster than 0.1m/s) in seconds
    SUMOTime myWaitingTime;
    WaitingTimeCollector myWaitingTimeCollector;

    /// @brief the time loss due to writing with less than maximum speed
    SUMOTime myTimeLoss;

    /// @brief This Vehicles driving state (pos and speed)
    State myState;

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel* myLaneChangeModel;

    const MSEdge* myLastBestLanesEdge;
    const MSLane* myLastBestLanesInternalLane;

    /* @brief Complex data structure for keeping and updating LaneQ:
     * Each element of the outer vector corresponds to an upcoming edge on the vehicles route
     * The first element corresponds to the current edge and is returned in getBestLanes()
     * The other elements are only used as a temporary structure in updateBestLanes();
     */
    std::vector<std::vector<LaneQ> > myBestLanes;

    /* @brief iterator to speed up retrival of the current lane's LaneQ in getBestLaneOffset() and getBestLanesContinuation()
     * This is updated in updateOccupancyAndCurrentBestLane()
     */
    std::vector<LaneQ>::iterator myCurrentLaneInBestLanes;

    static std::vector<MSLane*> myEmptyLaneVector;
    static std::vector<MSTransportable*> myEmptyTransportableVector;

    /// @brief The vehicle's list of stops
    std::list<Stop> myStops;

    /// @brief The passengers this vehicle may have
    MSDevice_Transportable* myPersonDevice;

    /// @brief The containers this vehicle may have
    MSDevice_Transportable* myContainerDevice;

    /// @brief The current acceleration after dawdling in m/s
    SUMOReal myAcceleration;

    /// @brief The information into which lanes the vehicle laps into
    std::vector<MSLane*> myFurtherLanes;
    std::vector<SUMOReal> myFurtherLanesPosLat;

    /// @brief State of things of the vehicle that can be on or off
    int mySignals;

    /// @brief Whether the vehicle is on the network (not parking, teleported, vaporized, or arrived)
    bool myAmOnNet;

    /// @brief Whether this vehicle is registered as waiting for a person (for deadlock-recognition)
    bool myAmRegisteredAsWaitingForPerson;

    /// @brief Whether this vehicle is registered as waiting for a container (for deadlock-recognition)
    bool myAmRegisteredAsWaitingForContainer;

    bool myHaveToWaitOnNextLink;

    /// @brief the angle in radians (@todo consider moving this into myState)
    SUMOReal myAngle;

    /// @brief distance to the next stop or -1 if there is none
    SUMOReal myStopDist;

    mutable Position myCachedPosition;

protected:
    struct DriveProcessItem {
        MSLink* myLink;
        SUMOReal myVLinkPass;
        SUMOReal myVLinkWait;
        bool mySetRequest;
        SUMOTime myArrivalTime;
        SUMOReal myArrivalSpeed;
        SUMOTime myArrivalTimeBraking;
        SUMOReal myArrivalSpeedBraking;
        SUMOReal myDistance;
        SUMOReal accelV;
        bool hadVehicle;
        SUMOReal availableSpace;

        DriveProcessItem(MSLink* link, SUMOReal vPass, SUMOReal vWait, bool setRequest,
                         SUMOTime arrivalTime, SUMOReal arrivalSpeed,
                         SUMOTime arrivalTimeBraking, SUMOReal arrivalSpeedBraking,
                         SUMOReal distance,
                         SUMOReal leaveSpeed = -1.) :
            myLink(link), myVLinkPass(vPass), myVLinkWait(vWait), mySetRequest(setRequest),
            myArrivalTime(arrivalTime), myArrivalSpeed(arrivalSpeed),
            myArrivalTimeBraking(arrivalTimeBraking), myArrivalSpeedBraking(arrivalSpeedBraking),
            myDistance(distance),
            accelV(leaveSpeed), hadVehicle(false), availableSpace(-1.) {
            assert(vWait >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
            assert(vPass >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
        };


        /// @brief constructor if the link shall not be passed
        DriveProcessItem(SUMOReal vWait, SUMOReal distance) :
            myLink(0), myVLinkPass(vWait), myVLinkWait(vWait), mySetRequest(false),
            myArrivalTime(0), myArrivalSpeed(0),
            myArrivalTimeBraking(0), myArrivalSpeedBraking(0),
            myDistance(distance),
            accelV(-1), hadVehicle(false), availableSpace(-1.) {
            assert(vWait >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
        };


        inline void adaptLeaveSpeed(const SUMOReal v) {
            if (accelV < 0) {
                accelV = v;
            } else {
                accelV = MIN2(accelV, v);
            }
        }
        inline SUMOReal getLeaveSpeed() const {
            return accelV < 0 ? myVLinkPass : accelV;
        }
    };

    /// Container for used Links/visited Lanes during lookForward.
    typedef std::vector< DriveProcessItem > DriveItemVector;
    DriveItemVector myLFLinkLanes;

    /// @todo: documentation
    void planMoveInternal(const SUMOTime t, MSLeaderInfo ahead, DriveItemVector& lfLinks, SUMOReal& myStopDist) const;

    /// @todo: documentation
    void checkRewindLinkLanes(const SUMOReal lengthsInFront, DriveItemVector& lfLinks) const;

    /// @brief unregister approach from all upcoming links
    void removeApproachingInformation(DriveItemVector& lfLinks) const;


    /// @brief estimate leaving speed when accelerating across a link
    inline SUMOReal estimateLeaveSpeed(const MSLink* const link, const SUMOReal vLinkPass) const {
        // estimate leave speed for passing time computation
        // l=linkLength, a=accel, t=continuousTime, v=vLeave
        // l=v*t + 0.5*a*t^2, solve for t and multiply with a, then add v
        return MIN2(link->getViaLaneOrLane()->getVehicleMaxSpeed(this),
                    getCarFollowModel().estimateSpeedAfterDistance(link->getLength(), vLinkPass, getVehicleType().getCarFollowModel().getMaxAccel()));
    }


    /* @brief adapt safe velocity in accordance to a moving obstacle:
     * - a leader vehicle
     * - a vehicle or pedestrian that crosses this vehicles path on an upcoming intersection
     * @param[in] leaderInfo The leading vehicle and the (virtual) distance to it
     * @param[in] seen the distance to the end of the current lane
     * @param[in] lastLink the lastLink index
     * @param[in] lane The current Lane the vehicle is on
     * @param[in,out] the safe velocity for driving
     * @param[in,out] the safe velocity for arriving at the next link
     * @param[in] distToCrossing The distance to the crossing point with the current leader where relevant or -1
     */
    void adaptToLeader(const std::pair<const MSVehicle*, SUMOReal> leaderInfo,
                       const SUMOReal seen, DriveProcessItem* const lastLink,
                       const MSLane* const lane, SUMOReal& v, SUMOReal& vLinkPass,
                       SUMOReal distToCrossing = -1) const;

    /* @brief adapt safe velocity in accordance to multiple vehicles ahead:
     * @param[in] ahead The leader information according to the current lateral-resolution
     * @param[in] latOffset the lateral offset for locating the ego vehicle on the given lane
     * @param[in] seen the distance to the end of the current lane
     * @param[in] lastLink the lastLink index
     * @param[in] lane The current Lane the vehicle is on
     * @param[in,out] the safe velocity for driving
     * @param[in,out] the safe velocity for arriving at the next link
     */
    void adaptToLeaders(const MSLeaderInfo& ahead,
                        SUMOReal latOffset,
                        const SUMOReal seen, DriveProcessItem* const lastLink,
                        const MSLane* const lane, SUMOReal& v, SUMOReal& vLinkPass) const;


    // @brief return the lane on which the back of this vehicle resides
    const MSLane* getBackLane() const;

    // @brief get the position of the back bumper;
    const Position getBackPosition() const;

    /** @brief updates the vehicles state, given a next value for its speed.
     *         This value can be negative in case of the ballistic update to indicate
     *         a stop within the next timestep. (You can call this a 'hack' to
     *         emulate reasoning based on accelerations: The assumed constant
     *         acceleration a within the next time step is then a = (vNext - vCurrent)/TS )
     *  @param[in] vNext speed in the next time step
     */
    void updateState(SUMOReal vNext);

private:
    /* @brief The vehicle's knowledge about edge efforts/travel times; @see MSEdgeWeightsStorage
     * @note member is initialized on first access */
    mutable MSEdgeWeightsStorage* myEdgeWeights;

    /// @brief The per vehicle variables of the car following model
    MSCFModel::VehicleVariables* myCFVariables;

#ifndef NO_TRACI
    /// @brief An instance of a velocity/lane influencing instance; built in "getInfluencer"
    Influencer* myInfluencer;
#endif

private:
    /// @brief invalidated default constructor
    MSVehicle();

    /// @brief invalidated copy constructor
    MSVehicle(const MSVehicle&);

    /// @brief invalidated assignment operator
    MSVehicle& operator=(const MSVehicle&);

    MSEdgeWeightsStorage& _getWeightsStorage() const;

};


#endif

/****************************************************************************/

