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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include "MSVehicleType.h"
#include "MSBaseVehicle.h"
#include "MSLink.h"
#include "MSLane.h"


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOSAXAttributes;
class MSMoveReminder;
class MSLaneChanger;
class MSVehicleTransfer;
class MSAbstractLaneChangeModel;
class MSBusStop;
class MSPerson;
class MSDevice;
class MSEdgeWeightsStorage;
class OutputDevice;
class Position;
class MSDevice_Person;


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

    /** @class State
     * @brief Container that holds the vehicles driving state (position+speed).
     */
    class State {
        /// @brief vehicle sets states directly
        friend class MSVehicle;
        friend class MSLaneChanger;

    public:
        /// Constructor.
        State(SUMOReal pos, SUMOReal speed);

        /// Copy constructor.
        State(const State& state);

        /// Assignment operator.
        State& operator=(const State& state);

        /// Operator !=
        bool operator!=(const State& state);

        /// Position of this state.
        SUMOReal pos() const;

        /// Speed of this state
        SUMOReal speed() const {
            return mySpeed;
        };

    private:
        /// the stored position
        SUMOReal myPos;

        /// the stored speed
        SUMOReal mySpeed;

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
              const MSVehicleType* type, SUMOReal speedFactor);

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
    bool replaceRoute(const MSRoute* route, bool onInit = false, int offset=0);


    /** @brief Returns whether the vehicle wil pass the given edge
     * @param[in] The edge to find in the vehicle's current route
     * @return Whether the given edge will be passed by the vehicle
     * @todo Move to MSRoute?
     */
    bool willPass(const MSEdge* const edge) const;

    unsigned int getRoutePosition() const;
    void resetRoutePosition(unsigned int index);

    /** @brief Returns the vehicle's internal edge travel times/efforts container
     *
     * If the vehicle does not have such a container, it is built.
     * @return The vehicle's knowledge about edge weights
     */
    const MSEdgeWeightsStorage& getWeightsStorage() const;
    MSEdgeWeightsStorage& getWeightsStorage();
    //@}


    /** @brief Uses the given values to compute the brutto-gap
     *
     * @param[in] predPos Position of the leader
     * @param[in] predLength Length of the leader
     * @param[in] pos Position of the follower
     * @return The gap between the leader and the follower
     */
    static inline SUMOReal gap(SUMOReal predPos, SUMOReal predLength, SUMOReal pos) {
        return predPos - predLength - pos;
    }



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
     * @param[in] pred The leader (may be 0)
     * @param[in] lengthsInFront Sum of vehicle lengths in front of the vehicle
     */
    void planMove(const SUMOTime t, const MSVehicle* pred, const SUMOReal lengthsInFront);


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


    /// @name state setter/getter
    //@{

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    SUMOReal getPositionOnLane() const {
        return myState.myPos;
    }


    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    SUMOReal getSpeed() const {
        return myState.mySpeed;
    }


    /** @brief Returns the vehicle's acceleration in m/s
     * @return The acceleration
     */
    SUMOReal getAcceleration() const {
        return myAcceleration;
    }
    //@}



    /// @name Other getter methods
    //@{

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


    /** @brief Returns the information whether the vehicle is on a road (is simulated)
     * @return Whether the vehicle is simulated
     */
    inline bool isOnRoad() const {
        return myAmOnNet;
    }


    /** @brief Returns the starting point for reroutes (usually the current edge)
     * 
     * This differs from *myCurrEdge only if the vehicle is on an internal edge
     * @return The rerouting start point
     */
    const MSEdge* getRerouteOrigin() const {
#ifdef HAVE_INTERNAL_LANES
        if (myLane != 0) {
            return myLane->getInternalFollower();
        }
#endif
        return *myCurrEdge;
    }


    /** @brief Returns the SUMOTime waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * Intentional stopping does not count towards this time.
     * @return The time the vehicle is standing
     */
    SUMOTime getWaitingTime() const {
        return myWaitingTime;
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


    /** @brief Returns the vehicle's direction in degrees
     * @return The vehicle's current angle
     */
    SUMOReal getAngle() const;
    //@}


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
     * @param[in] notification The cause of insertion (i.e. departure, teleport, parking)
     */
    void enterLaneAtInsertion(MSLane* enteredLane, SUMOReal pos, SUMOReal speed,
                              MSMoveReminder::Notification notification);


    /** @brief Update when the vehicle enters a new lane in the laneChange step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     */
    void enterLaneAtLaneChange(MSLane* enteredLane);


    /** @brief Update of members if vehicle leaves a new lane in the lane change step or at arrival. */
    void leaveLane(const MSMoveReminder::Notification reason);


    MSAbstractLaneChangeModel& getLaneChangeModel();
    const MSAbstractLaneChangeModel& getLaneChangeModel() const;

    /// @name strategical/tactical lane choosing methods
    /// @{

    /** @struct LaneQ
     * @brief A structure representing the best lanes for continuing the route
     */
    struct LaneQ {
        /// @brief The described lane
        MSLane* lane;
        /// @brief The overall length which may be driven when using this lane without a lane change
        SUMOReal length;
        /// @brief The overall vehicle sum on consecutive lanes which can be passed without a lane change
        SUMOReal occupation;
        /// @brief As occupation, but without the first lane
        SUMOReal nextOccupation;
        /// @brief The (signed) number of lanes to be crossed to get to the lane which allows to continue the drive
        int bestLaneOffset;
        /// @brief Whether this lane allows to continue the drive
        bool allowsContinuation;
        /// @brief Consecutive lane that can be followed without a lane change (contribute to length and occupation)
        std::vector<MSLane*> bestContinuations;
    };

    /** @brief Returns the description of best lanes to use in order to continue the route
     *
     * The information is rebuilt if the vehicle is on a different edge than
     *  the one stored in "myLastBestLanesEdge" or "forceRebuild" is true.
     *
     * Otherwise, only the density changes on the stored lanes are adapted to
     *  the container only.
     *
     * A rebuild must be done if the vehicle leaves a stop; then, another lane may get
     *  the best one.
     *
     * If no starting lane ("startLane") is given, the vehicle's current lane ("myLane")
     *  is used as start of bect lanes building.
     *
     * @param[in] forceRebuild Whether the best lanes container shall be rebuilt even if the vehicle's edge has not changed
     * @param[in] startLane The lane the process shall start at ("myLane" will be used if ==0)
     * @return The best lanes structure holding matching the current vehicle position and state ahead
     */
    virtual const std::vector<LaneQ>& getBestLanes(bool forceRebuild = false, MSLane* startLane = 0) const;


    /** @brief Returns the subpart of best lanes that describes the vehicle's current lane and their successors
     * @return The best lane information for the vehicle's current lane
     * @todo Describe better
     */
    const std::vector<MSLane*>& getBestLanesContinuation() const;

    /** @brief Returns the subpart of best lanes that describes the given lane and their successors
     * @return The best lane information for the given lane
     * @todo Describe better
     */
    const std::vector<MSLane*>& getBestLanesContinuation(const MSLane* const l) const;

    /// @brief returns the current offset from the best lane
    int getBestLaneOffset() const;

    /// @}

    /// @brief repair errors in vehicle position after changing between internal edges
    bool fixPosition();


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
        MSBusStop* busstop;
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
        /// @brief whether the vehicle is removed from the net while stopping
        bool parking;
        /// @brief Information whether the stop has been reached
        bool reached;
        /// @brief IDs of persons the vehicle has to wait for until departing
        std::set<std::string> awaitedPersons;
    };


    /** @brief Adds a stop
     *
     * The stop is put into the sorted list.
     * @param[in] stop The stop to add
     * @return Whether the stop could be added
     */
    bool addStop(const SUMOVehicleParameter::Stop& stopPar, SUMOTime untilOffset = 0);


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


    /** @brief Returns whether the vehicle is on a triggered stop
     * @return whether the vehicle is on a triggered stop
     */
    bool isStoppedTriggered() const;
    /// @}

    bool knowsEdgeTest(MSEdge& edge) const;
    unsigned int getLaneIndex() const;

    /**
     * Compute distance that will be covered, if the vehicle moves to a given position on its route,
     * starting at its current position.
     * @param destPos:	position on the destination edge that shall be reached
     * @param destEdge: destination edge that shall be reached
     * @return			distance from the vehicles current position to the destination position,
     *					or a near infinite real value if the destination position is not contained
     *					within the vehicles route or the vehicle is not active
     */
    SUMOReal getDistanceToPosition(SUMOReal destPos, const MSEdge* destEdge);


    /** @brief Processes stops, returns the velocity needed to reach the stop
     * @return The velocity in dependance to the next/current stop
     * @todo Describe more detailed
     * @see Stop
     * @see MSBusStop
     */
    SUMOReal processNextStop(SUMOReal currentVelocity);

    /** @brief Returns the leader of the vehicle looking for a fixed distance.
     *
     * If the distance is not given it is calculated from the brake gap.
     * The gap returned does not include the minGap.
     * @param dist		up to which distance to look for a leader
     * @return The leading vehicle together with the gap; (0, -1) if no leader was found.
     */
    std::pair<const MSVehicle* const, SUMOReal> getLeader(SUMOReal dist=0) const;

    /** @brief Returns the time gap in seconds to the leader of the vehicle looking for a fixed distance.
     *
     * If the distance is too big -1 is returned.
     * The gap returned takes the minGap into account.
     * @return The time gap in seconds; -1 if no leader was found or speed is 0.
     */
    SUMOReal getTimeGap() const;


    /// @name Emission retrieval
    //@{

    /** @brief Returns CO2 emission of the current state
     * @return The current CO2 emission
     */
    SUMOReal getHBEFA_CO2Emissions() const;


    /** @brief Returns CO emission of the current state
     * @return The current CO emission
     */
    SUMOReal getHBEFA_COEmissions() const;


    /** @brief Returns HC emission of the current state
     * @return The current HC emission
     */
    SUMOReal getHBEFA_HCEmissions() const;


    /** @brief Returns NOx emission of the current state
     * @return The current NOx emission
     */
    SUMOReal getHBEFA_NOxEmissions() const;


    /** @brief Returns PMx emission of the current state
     * @return The current PMx emission
     */
    SUMOReal getHBEFA_PMxEmissions() const;


    /** @brief Returns fuel consumption of the current state
     * @return The current fuel consumption
     */
    SUMOReal getHBEFA_FuelConsumption() const;


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
    void addPerson(MSPerson* person);


    /** @brief Returns the number of persons
     * @return The number of passengers on-board
     */
    unsigned int getPersonNumber() const;

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
     * @param lane		lane on wich to stop
     * @param pos		position on the given lane at wich to stop
     * @param radius	the vehicle will stop if it is within the range [pos-radius, pos+radius]
     * @param duration	after waiting for the time period duration, the vehicle will
     * @param parking   a flag indicating whether the traci stop is used for parking or not
     * @param triggered a flag indicating whether the traci stop is triggered or not
     */
    bool addTraciStop(MSLane* lane, SUMOReal pos, SUMOReal radius, SUMOTime duration, bool parking, bool triggered);

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
        void setLaneTimeLine(const std::vector<std::pair<SUMOTime, unsigned int> >& laneTimeLine);


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
        int influenceChangeDecision(const SUMOTime currentTime, const MSEdge& currentEdge, const unsigned int currentLaneIndex, int state);


        /** @brief Return the remaining number of seconds of the current
         * laneTimeLine assuming one exists
         * @param[in] currentTime The current simulation time
         * @return The remaining seconds to change lanes
         */
        SUMOReal changeRequestRemainingSeconds(const SUMOTime currentTime) const;

        /** @brief Sets whether the safe velocity shall be regarded
         * @param[in] value Whether the safe velocity shall be regarded
         */
        void setConsiderSafeVelocity(bool value);


        /** @brief Sets whether the maximum acceleration shall be regarded
         * @param[in] value Whether the maximum acceleration shall be regarded
         */
        void setConsiderMaxAcceleration(bool value);


        /** @brief Sets whether the maximum deceleration shall be regarded
         * @param[in] value Whether the maximum deceleration shall be regarded
         */
        void setConsiderMaxDeceleration(bool value);


        /** @brief Sets whether junction priority rules shall be respected
         * @param[in] value Whether junction priority rules be respected
         */
        void setRespectJunctionPriority(bool value);


        /** @brief Returns whether junction priority rules shall be respected
         * @return Whether junction priority rules be respected
         */
        inline bool getRespectJunctionPriority() const {
            return myRespectJunctionPriority;
        }


        /** @brief Sets whether red lights shall be a reason to brake
         * @param[in] value Whether red lights shall be a reason to brake
         */
        void setEmergencyBrakeRedLight(bool value);


        /** @brief Returns whether red lights shall be a reason to brake
         * @return Whether red lights shall be a reason to brake
         */
        inline bool getEmergencyBrakeRedLight() const {
            return myEmergencyBrakeRedLight;
        }


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

        void setVTDControlled(bool c, MSLane* l, SUMOReal pos, int edgeOffset, const MSEdgeVector& route) {
            myAmVTDControlled = c;
            myVTDLane = l;
            myVTDPos = pos;
            myVTDEdgeOffset = edgeOffset;
            myVTDRoute = route;
        }

        void postProcessVTD(MSVehicle* v);

        inline bool isVTDControlled() const {
            return myAmVTDControlled;
        }

    private:
        /// @brief The velocity time line to apply
        std::vector<std::pair<SUMOTime, SUMOReal> > mySpeedTimeLine;

        /// @brief The lane usage time line to apply
        std::vector<std::pair<SUMOTime, unsigned int> > myLaneTimeLine;

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

        bool myAmVTDControlled;
        MSLane* myVTDLane;
        SUMOReal myVTDPos;
        int myVTDEdgeOffset;
        MSEdgeVector myVTDRoute;

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
        //@}
        ///* @brief flags for determining the priority of traci lane change requests
        TraciLaneChangePriority myTraciLaneChangePriority;

    };


    /** @brief Returns the velocity/lane influencer
     *
     * If no influencer was existing before, one is built, first
     * @return Reference to this vehicle's speed influencer
     */
    Influencer& getInfluencer();

    bool hasInfluencer() const {
        return myInfluencer != 0;
    }

    /// @brief allow TraCI to influence a lane change decision
    int influenceChangeDecision(int state);


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


    /// @brief The time the vehicle waits (is not faster than 0.1m/s) in seconds
    SUMOTime myWaitingTime;

    /// @brief This Vehicles driving state (pos and speed)
    State myState;

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel* myLaneChangeModel;

    mutable const MSEdge* myLastBestLanesEdge;
    mutable const MSLane* myLastBestLanesInternalLane;

    mutable std::vector<std::vector<LaneQ> > myBestLanes;
    mutable std::vector<LaneQ>::iterator myCurrentLaneInBestLanes;
    static std::vector<MSLane*> myEmptyLaneVector;

    /// @brief The vehicle's list of stops
    std::list<Stop> myStops;

    /// @brief The passengers this vehicle may have
    MSDevice_Person* myPersonDevice;

    /// @brief The current acceleration after dawdling in m/s
    SUMOReal myAcceleration;

    /// @brief The information into which lanes the vehicle laps into
    std::vector<MSLane*> myFurtherLanes;

    /// @brief State of things of the vehicle that can be on or off
    int mySignals;

    /// @brief Whether the vehicle is on the network (not parking, teleported, vaporized, or arrived)
    bool myAmOnNet;

    /// @brief Whether this vehicle is registered as waiting for a person (for deadlock-recognition)
    bool myAmRegisteredAsWaitingForPerson;

    bool myHaveToWaitOnNextLink;

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
            assert(vWait >= 0);
            assert(vPass >= 0);
        };

        /// @brief constructor if the link shall not be passed
        DriveProcessItem(SUMOReal vWait, SUMOReal distance) :
            myLink(0), myVLinkPass(vWait), myVLinkWait(vWait), mySetRequest(false),
            myArrivalTime(0), myArrivalSpeed(0),
            myArrivalTimeBraking(0), myArrivalSpeedBraking(0),
            myDistance(distance),
            accelV(-1), hadVehicle(false), availableSpace(-1.) {
            assert(vWait >= 0);
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

    typedef std::vector< DriveProcessItem > DriveItemVector;

    /// Container for used Links/visited Lanes during lookForward.
    DriveItemVector myLFLinkLanes;

    void planMoveInternal(const SUMOTime t, const MSVehicle* pred, DriveItemVector& lfLinks) const;
    void checkRewindLinkLanes(const SUMOReal lengthsInFront, DriveItemVector& lfLinks) const;

    /// @brief estimate leaving speed when accelerating across a link
    inline SUMOReal estimateLeaveSpeed(const MSLink* const link, const SUMOReal vLinkPass) const {
        // estimate leave speed for passing time computation
        // l=linkLength, a=accel, t=continuousTime, v=vLeave
        // l=v*t + 0.5*a*t^2, solve for t and multiply with a, then add v
        return MIN2(link->getViaLaneOrLane()->getVehicleMaxSpeed(this),
                    estimateSpeedAfterDistance(link->getLength(), vLinkPass, getVehicleType().getCarFollowModel().getMaxAccel()));
    }

    /* @brief estimate speed while accelerating for the given distance
     * @param[in] dist The distance during which accelerating takes place
     * @param[in] v The initial speed
     * @param[in] accel The acceleration
     */
    inline SUMOReal estimateSpeedAfterDistance(const SUMOReal dist, const SUMOReal v, const SUMOReal accel) const {
        // dist=v*t + 0.5*accel*t^2, solve for t and multiply with accel, then add v
        return MIN2(getVehicleType().getMaxSpeed(),
                    (SUMOReal)sqrt(2 * dist * accel + v * v));
    }


    /* @brief estimate speed while accelerating for the given distance
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
                       SUMOReal distToCrossing=-1) const;

#ifdef HAVE_INTERNAL_LANES
    /// @brief ids of vehicles being followed across a link (for resolving priority)
    mutable std::set<std::string> myLinkLeaders;
#endif

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

