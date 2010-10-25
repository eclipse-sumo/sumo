/****************************************************************************/
/// @file    MSVehicle.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Representation of a vehicle in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MSEdge.h"
#include "MSRoute.h"
#include "MSCORN.h"
#include "MSGlobals.h"
#include <list>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/SUMOVehicle.h>
#include <utils/common/SUMOVehicleClass.h>
#include "MSVehicleType.h"
#include <utils/common/SUMOAbstractRouter.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MEVehicle.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSLink;
class MSMoveReminder;
class MSLaneChanger;
class MSVehicleTransfer;
class MSAbstractLaneChangeModel;
class MSBusStop;
class MSPerson;
class MSDevice;
class MSEdgeWeightsStorage;
class OutputDevice;
class Position2D;
#ifdef _MESSAGES
class MSMessageEmitter;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicle
 * @brief Representation of a vehicle in the micro simulation
 */
class MSVehicle :
#ifdef HAVE_MESOSIM
        public MEVehicle
#else
        public SUMOVehicle
#endif
{
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



    /// @brief Destructor.
    virtual ~MSVehicle() throw();



    /// @name emission handling
    //@{

    /** @brief Returns the vehicle's parameter (including departure definition)
     *
     * @return The vehicle's parameter
     */
    const SUMOVehicleParameter &getParameter() const throw() {
        return *myParameter;
    }


    /** @brief Returns the edge the vehicle starts from
     * @return The vehicle's departure edge
     */
    const MSEdge &getDepartEdge() const {
        return **myCurrEdge;
    }


    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const throw() {
        return myParameter->depart;
    }
    //@}



    /// @name insertion/removal
    //@{

    /** @brief Called when the vehicle tries to get into the network
     *
     * Calls the appropriate device function, needed for rerouting.
     */
    void onTryEmit() throw();


    /** @brief Called when the vehicle is inserted into the network
     *
     * Sets optional information about departure time, informs the vehicle
     *  control about a further running vehicle.
     */
    void onDepart() throw();


    /** @brief Called when the vehicle is removed the network.
     *
     * Sets the optional information about arival if forTeleporting is false
     *  (the vehicle has arrived). Moves along work reminders and
     *  informs all devices about quitting. Calls "leaveLane" then.
     *
     * @param[in] forTeleporting true if the vehicle shall be teleported (otherwise it has reached its destination)
     */
    void onRemovalFromNet(bool forTeleporting) throw();
    //@}


    /// @name interaction with the route
    //@{

    /** @brief Returns whether the vehicle has still to pass nSuccs edges
     * @param[in] nSuccs The number of edge to look forward
     * @return Whether the rest of the route is at least as long as nSuccs
     */
    inline bool hasSuccEdge(unsigned int nSuccs) const throw() {
        return myCurrEdge + nSuccs < myRoute->end();
    }

    /** @brief Returns the nSuccs'th successor of edge the vehicle is currently at
     *
     * If the rest of the route (counted from the current edge) than nSuccs,
     *  0 is returned.
     * @param[in] nSuccs The number of edge to look forward
     * @return The nSuccs'th following edge in the vehicle's route
     */
    const MSEdge* succEdge(unsigned int nSuccs) const throw();


    /** @brief Returns the information whether the vehicle should end now
     * @return Whether the route ends
     */
    bool ends() const throw();


    /** @brief Returns the current route
     * @return The route the vehicle uses
     */
    const MSRoute &getRoute() const throw() {
        return *myRoute;
    }


    /** @brief Returns the current or a previously used route
     * @param[in] index The route to return (0 is the current route; other have an index>0)
     * @return The current (index=0) or a prior route
     * @todo Should an exception be thrown if a not existing route is requested?
     */
    const MSRoute &getRoute(int index) const throw();


    /** @brief Replaces the current route by the given edges
     *
     * It is possible that the new route is not accepted, if a) it does not
     *  contain the vehicle's current edge, or b) something fails on insertion
     *  into the routes container (see in-line comments).
     *
     * @param[in] edges The new list of edges to pass
     * @param[in] simTime The time at which the route was replaced
     * @return Whether the new route was accepted
     */
    bool replaceRoute(const MSEdgeVector &edges, SUMOTime simTime, bool onInit=false) throw();


    /** @brief Replaces the current route by the given one
     *
     * It is possible that the new route is not accepted, if it does not
     *  contain the vehicle's current edge.
     *
     * @param[in] route The new route to pass
     * @param[in] simTime The time at which the route was replaced
     * @return Whether the new route was accepted
     */
    bool replaceRoute(const MSRoute* route, SUMOTime simTime, bool onInit=false) throw();


    /** @brief Returns whether the vehicle wil pass the given edge
     * @param[in] The edge to find in the vehicle's current route
     * @return Whether the given edge will be passed by the vehicle
     * @todo Move to MSRoute?
     */
    bool willPass(const MSEdge * const edge) const throw();


    /** @brief Performs a rerouting using the given router
     *
     * Tries to find a new route between the current edge and the destination edge, first.
     * Tries to replace the current route by the new one using replaceRoute.
     *
     * @param[in] t The time for which the route is computed
     * @param[in] router The router to use
     * @see replaceRoute
     */
    void reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle> &router, bool withTaz=false) throw();


    /** @brief Returns the vehicle's internal edge travel times/efforts container
     *
     * If the vehicle does not have such a container, it is built.
     * @return The vehicle's knowledge about edge weights
     */
    MSEdgeWeightsStorage &getWeightsStorage() throw();


    /** @brief Validates the current route
     * @param[out] msg Description why the route is not valid (if it is the case)
     * @return Whether the vehicle's current route is valid
     */
    bool hasValidRoute(std::string &msg) const throw();
    //@}






    /// moves the vehicles after their responds (right-of-way rules) are known
    bool moveFirstChecked();


    /** @brief Returns the gap between pred and this vehicle.
     *
     * Assumes both vehicles are on the same or on are on parallel lanes.
     *
     * @param[in] pred The leader
     * @return The gap between this vehicle and the leader (may be <0)
     */
    SUMOReal gap2pred(const MSVehicle& pred) const throw() {
        SUMOReal gap = pred.getPositionOnLane() - pred.getVehicleType().getLength() - getPositionOnLane();
        if (gap<0&&gap>-1.0e-12) {
            gap = 0;
        }
        return gap;
    }


    /** @brief Uses the given values to compute the brutto-gap
     *
     * @param[in] predPos Position of the leader
     * @param[in] predLength Length of the leader
     * @param[in] pos Position of the follower
     * @return The gap between the leader and the follower
     */
    static inline SUMOReal gap(SUMOReal predPos, SUMOReal predLength, SUMOReal pos) throw() {
        return predPos - predLength - pos;
    }


    /// @name Retrieval of CORN values
    //@{

    /** @brief Returns the named CORN integer value
     *
     * The behaviour is undefined for not stored values.
     *
     * @return The named CORN integer value
     * @todo Recheck whether throwing an exception for unset values is more appropriate
     */
    int getCORNIntValue(MSCORN::Function f) const throw();


    /** @brief Returns the named CORN pointer value
     *
     * The behaviour is undefined for not stored values.
     *
     * @return The named CORN pointer value
     * @todo Recheck whether throwing an exception for unset values is more appropriate
     */
    void *getCORNPointerValue(MSCORN::Pointer p) const throw();


    /** Returns whether this vehicle has the named integer value stored
     * @return Whether the named integer value is stored in CORN
     */
    bool hasCORNIntValue(MSCORN::Function f) const throw();


    /** Returns whether this vehicle has the named pointer value stored
     * @return Whether the named pointer value is stored in CORN
     */
    bool hasCORNPointerValue(MSCORN::Pointer p) const throw();
    //@}



    /// @name Interaction with move reminders
    //@{

    /** @brief Processes active move reminder
     *
     * This method goes through all active move reminder, both those for the current
     *  lane, stored in "myMoveReminders" and those of prior lanes stored in
     *  "myOldLaneMoveReminders" calling "MSMoveReminder::isStillActive".
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
    void workOnMoveReminders(SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();


    /** @brief Returns the vehicle's position in relation to a passed move reminder lane
     *
     * @param[in] searchedLane The lane to search for within move reminder
     * @return The distance to this lane (vehicle position + lane offset); -1 if n move reminder is placed on this lane
     * @todo Maybe this should be rechecked - the name says it: much to complicated and the parameter should be a move reminder?
     */
    SUMOReal getPositionOnActiveMoveReminderLane(const MSLane * const searchedLane) const throw();
    //@}



    /** @brief Moves vehicles which may run out of the lane
     *
     * @param[in] lane The lane the vehicle is on
     * @param[in] pred The leader (may be 0)
     * @param[in] neigh The neighbor vehicle (may be 0)
     * @param[in] lengthsInFront Sum of vehicle lengths in front of the vehicle
     * @return Whether a collision occured (gap2pred(leader)<=0)
     */
    bool moveRegardingCritical(SUMOTime t, const MSLane* const lane, const MSVehicle * const pred,
                               const MSVehicle * const neigh, SUMOReal lengthsInFront) throw();


    /// @name state setter/getter
    //@{

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    SUMOReal getPositionOnLane() const throw() {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            return MEVehicle::getPositionOnLane();
        }
#endif
        return myState.myPos;
    }


    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    SUMOReal getSpeed() const throw() {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            return MEVehicle::getSpeed();
        }
#endif
        return myState.mySpeed;
    }

    /** @brief Returns the vehicle's acceleration before dawdling
     * @return The acceleration before dawdling
     */
    SUMOReal getPreDawdleAcceleration() const throw() {
        return myPreDawdleAcceleration;
    }
    //@}



    SUMOReal getMaxSpeed() const {
        if (myHasIndividualMaxSpeed)
            return myIndividualMaxSpeed;
        return myType->getMaxSpeed();
    }

    SUMOReal adaptMaxSpeed(SUMOReal referenceSpeed) {
        if (myType->hasSpeedDeviation() && referenceSpeed != myReferenceSpeed) {
            myHasIndividualMaxSpeed = true;
            myIndividualMaxSpeed = myType->getMaxSpeedWithDeviation(referenceSpeed);
            myReferenceSpeed = referenceSpeed;
        }
        if (myHasIndividualMaxSpeed)
            return myIndividualMaxSpeed;
        return MIN2(myType->getMaxSpeed(), referenceSpeed);
    }

    void setIndividualMaxSpeed(SUMOReal individualMaxSpeed) {
        myHasIndividualMaxSpeed = true;
        myIndividualMaxSpeed = individualMaxSpeed;
    }

    void unsetIndividualMaxSpeed(void) {
        myHasIndividualMaxSpeed = false;
    }

    void setPreDawdleAcceleration(SUMOReal accel) {
        myPreDawdleAcceleration = accel;
    }


    /// Returns the name of the vehicle
    const std::string &getID() const throw();

    /** Returns true if the two vehicles overlap. */
    static bool overlap(const MSVehicle* veh1, const MSVehicle* veh2) {
        if (veh1->myState.myPos < veh2->myState.myPos) {
            return veh2->myState.myPos - veh2->getVehicleType().getLength() < veh1->myState.myPos;
        }
        return veh1->myState.myPos - veh1->getVehicleType().getLength() < veh2->myState.myPos;
    }


    /** Returns true if vehicle's speed is below 60km/h. This is only relevant
        on highways. Overtaking on the right is allowed then. */
    bool congested() const {
        return myState.mySpeed < SUMOReal(60) / SUMOReal(3.6);
    }

    /// Return current Position
    Position2D getPosition() const;


    /// @name state io
    //@{

    /// Saves the states of a vehicle
    void saveState(std::ostream &os);
    //@}


    const MSEdge * const getEdge() const;


    /** @brief Update when the vehicle enters a new lane in the move step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] driven The distance driven by the vehicle within this time step
     * @param[in] onTeleporting Whether the lane was entered while being teleported
     * @return Whether the vehicle's route has ended (due to vaporization, or because the destination was reached)
     */
    bool enterLaneAtMove(MSLane* enteredLane, SUMOReal driven, bool onTeleporting=false);



    /** @brief Update when the vehicle enters a new lane in the emit step
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] pos The position the vehicle was inserted into the lane
     * @param[in] speed The speed with which the vehicle was inserted into the lane
     */
    void enterLaneAtEmit(MSLane* enteredLane, SUMOReal pos, SUMOReal speed);


    /** @brief Update when the vehicle enters a new lane in the laneChange step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     */
    void enterLaneAtLaneChange(MSLane* enteredLane);


    /** @brief Update when the vehicle leaves a new lane in the move step.
     *
     * @param[in] driven The distance driven by the vehicle within this time step
     */
    void leaveLaneAtMove(SUMOReal driven);


    /** @brief Update of members if vehicle leaves a new lane in the lane change step or at arrival. */
    void leaveLane(bool isArrival);


    friend class MSVehicleControl;

    void vsafeCriticalCont(SUMOTime t, SUMOReal minVSafe, SUMOReal lengthsInFront);

    MSLane *getTargetLane() const;

    /// Returns the lane the vehicle is on
    MSLane &getLane() const throw() {
        return *myLane;
    }

    /** @brief Returns the information whether the vehicle is on a road (is simulated)
     * @return Whether the vehicle is simulated
     */
    inline bool isOnRoad() const throw() {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            return MEVehicle::getSegment() != 0;
        }
#endif
        return myLane!=0;
    }



    /** @brief Returns the SUMOTime waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * @return The time the vehicle is standing
     */
    SUMOTime getWaitingTime() const throw() {
        return myWaitingTime;
    }


    /** @brief Returns the number of seconds waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * @return The time the vehicle is standing
     */
    SUMOReal getWaitingSeconds() const throw() {
        return STEPS2TIME(myWaitingTime);
    }



    void quitRemindedEntered(MSVehicleQuitReminded *r);
    void quitRemindedLeft(MSVehicleQuitReminded *r);


    MSAbstractLaneChangeModel &getLaneChangeModel();
    const MSAbstractLaneChangeModel &getLaneChangeModel() const;

    /// @name strategical/tactical lane choosing methods
    /// @{

    /** @struct LaneQ
     * @brief A structure representing the best lanes for continuing the route
     */
    struct LaneQ {
        /// @brief The described lane
        MSLane *lane;
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
    virtual const std::vector<LaneQ> &getBestLanes(bool forceRebuild=false, MSLane *startLane=0) const throw();


    /** @brief Returns the subpart of best lanes that describes the vehicle's current lane and their successors
     * @return The best lane information for the vehicle's current lane
     * @todo Describe better
     */
    const std::vector<MSLane*> &getBestLanesContinuation() const throw();

    /** @brief Returns the subpart of best lanes that describes the given lane and their successors
     * @return The best lane information for the given lane
     * @todo Describe better
     */
    const std::vector<MSLane*> &getBestLanesContinuation(const MSLane * const l) const throw();
    /// @}


    /** @brief Returns the vehicle's type definition
     * @return The vehicle's type definition
     */
    inline const MSVehicleType &getVehicleType() const throw() {
        return *myType;
    }


    /** @brief Replaces the current vehicle type by the one given
     *
     * If the currently used vehicle type is marked as being used by this vehicle
     *  only, it is deleted, first. The new, given type is then assigned to
     *  "myType".
     * @param[in] type The new vehicle type
     * @see MSVehicle::myType
     */
    void replaceVehicleType(MSVehicleType *type) throw();


    /** @brief Returns the vehicle's car following model definition
     *
     * This is simply a wrapper around the vehicle type's car-following
     *  model retrieval for a shorter access.
     *
     * @return The vehicle's car following model definition
     */
    inline const MSCFModel &getCarFollowModel() const throw() {
        return myType->getCarFollowModel();
    }


    void writeXMLRoute(OutputDevice &os, int index=-1) const;


    /// @name vehicle stops definitions and i/o
    //@{

    /** @struct Stop
     * @brief Definition of vehicle stop (position and duration)
     */
    struct Stop {
        /// @brief The edge in the route to stop at
        MSRouteIterator edge;
        /// @brief The lane to stop at
        const MSLane *lane;
        /// @brief (Optional) bus stop if one is assigned to the stop
        MSBusStop *busstop;
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
    };


    /** @brief Adds a stop
     *
     * The stop is put into the sorted list.
     * @param[in] stop The stop to add
     * @return Whether the stop could be added
     */
    bool addStop(const SUMOVehicleParameter::Stop &stopPar, SUMOTime untilOffset=0) throw();


    /** @brief Returns whether the vehicle has to stop somewhere
     * @return Whether the vehicle has to stop somewhere
     */
    bool hasStops() const {
        return !myStops.empty();
    }

    /** @brief Returns whether the vehicle is at a stop
     * @return Whether the has stopped
     */
    bool isStopped() const;

    /** @brief Returns whether the vehicle is parking
     * @return whether the vehicle is parking
     */
    bool isParking() const;
    /// @}

    bool knowsEdgeTest(MSEdge &edge) const;


    /** @brief Returns this vehicle's devices
     * @return This vehicle's devices
     */
    const std::vector<MSDevice*> &getDevices() const {
        return myDevices;
    }

    void setWasVaporized(bool onDepart);

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
    SUMOReal processNextStop(SUMOReal currentVelocity) throw();


    /// @name Emission retrieval
    //@{

    /** @brief Returns CO2 emission of the current state
     * @return The current CO2 emission
     */
    SUMOReal getHBEFA_CO2Emissions() const throw();


    /** @brief Returns CO emission of the current state
     * @return The current CO emission
     */
    SUMOReal getHBEFA_COEmissions() const throw();


    /** @brief Returns HC emission of the current state
     * @return The current HC emission
     */
    SUMOReal getHBEFA_HCEmissions() const throw();


    /** @brief Returns NOx emission of the current state
     * @return The current NOx emission
     */
    SUMOReal getHBEFA_NOxEmissions() const throw();


    /** @brief Returns PMx emission of the current state
     * @return The current PMx emission
     */
    SUMOReal getHBEFA_PMxEmissions() const throw();


    /** @brief Returns fuel consumption of the current state
     * @return The current fuel consumption
     */
    SUMOReal getHBEFA_FuelConsumption() const throw();


    /** @brief Returns noise emissions of the current state
     * @return The noise produced
     */
    SUMOReal getHarmonoise_NoiseEmissions() const throw();
    //@}

    void addPerson(MSPerson* person) throw();



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


    /** @brief Switches the given signal on
     * @param[in] signal The signal to mark as being switched on
     */
    void switchOnSignal(int signal) throw() {
        mySignals |= signal;
    }


    /** @brief Switches the given signal off
     * @param[in] signal The signal to mark as being switched off
     */
    void switchOffSignal(int signal) throw() {
        mySignals &= ~signal;
    }


    /** @brief Returns the signals
     * @return The signals' states
     */
    int getSignals() const throw() {
        return mySignals;
    }


    /** @brief Returns whether the given signal is on
     * @param[in] signal The signal to return the value of
     * @return Whether the given signal is on
     */
    bool signalSet(int which) const throw() {
        return (mySignals&which)!=0;
    }
    /// @}




#ifndef NO_TRACI
    /**
     * Get speed before influence of TraCI settings takes place.
     */
    SUMOReal getSpeedWithoutTraciInfluence() const throw() {
        return speedWithoutTraciInfluence;
    }

    /**
     * schedule a new stop for the vehicle; each time a stop is reached, the vehicle
     * will wait for the given duration before continuing on its route
     * @param lane		lane on wich to stop
     * @param pos		position on the given lane at wich to stop
     * @param radius	the vehicle will stop if it is within the range [pos-radius, pos+radius]
     * @duration		after waiting for the time period duration, the vehicle will
     *					continue until the stop is reached again
     */
    bool addTraciStop(MSLane* lane, SUMOReal pos, SUMOReal radius, SUMOTime duration);


    /**
     * The vehicle will slow down to newSpeed within the time interval duration.
     * This is done by limiting the maximum speed every time a simulation step
     * is performed by TraCI. Speed reduction is linear.
     * @param newSpeed speed to reduce to
     * @param duration time intervall for the speed adaption
     * @param currentTime current simulation time
     */
    bool startSpeedAdaption(float newSpeed, SUMOTime duration, SUMOTime currentTime);

    /**
     * called each simulation step to slow down if necessary (see above)
     */
    void adaptSpeed();

    /**
     * Checks if the sticky time for a "changelane" command has passed already
     * @param time current simulation time
     */
    void checkLaneChangeConstraint(SUMOTime time);

    /**
     * After each changed lane, check if the lane requested by TraCI command "changeLane"
     * is reached and request for more changes if necessary.
     */
    void checkForLaneChanges();

    /**
     * Initiate a lane change requested by TraCI command "changeLane".
     * @param lane	the lane index within the current edge, that is the destination of the change
     * @param stickyTime	duration for wich the constraint takes effect
     */
    void startLaneChange(unsigned lane, SUMOTime stickyTime);

    /**
     * takes all action necessary during a simulation step to process any active command sent by TraCI
     * @param time the current simulation time
     */
    void processTraCICommands(SUMOTime time);

    void setTraCISpeed(SUMOReal speed) throw();
#endif

    /** @brief Adds a MoveReminder dynamically
     *
     * @see MSMoveReminder
     */
    void addReminder(MSMoveReminder* rem) throw();

protected:




    void checkRewindLinkLanes(SUMOReal lengthsInFront) throw();

    /// @name Interaction with move reminders
    ///@{

    /** @brief "Activates" all current move reminder
     *
     * For all move reminder stored in "myMoveReminders", their method
     *  "MSMoveReminder::notifyEnter" is called.
     *
     * @param[in] isEmit true means the vehicle was emitted
     * @param[in] isLaneChange true means the vehicle entered via lane change
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyEnter
     */
    void activateReminders(bool isEmit, bool isLaneChange) throw();


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
    void adaptLaneEntering2MoveReminder(const MSLane &enteredLane) throw();
    ///@}



    void setBlinkerInformation() throw();
#ifndef HAVE_MESOSIM
    SUMOTime getLastEntryTime() const throw() {
        return 0;
    }
    SUMOReal getSegmentLength() const throw() {
        return 0;
    }
#endif


    /// Use this constructor only.
    MSVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
              const MSVehicleType* type, int vehicleIndex) throw(ProcessError);

    /// @brief the lane, the vehicle will be within the next time step (0 if the vehicle stays on the same it was before)
    MSLane *myTarget;

    /// @brief information how long ago the vehicle has performed a lane-change
    SUMOReal myLastLaneChangeOffset;

    /// @brief The time the vehicle waits (is not faster than 0.1m/s) in seconds
    SUMOTime myWaitingTime;

#ifdef _MESSAGES
    /// The message emitters
    MSMessageEmitter *myLCMsgEmitter;
    MSMessageEmitter *myBMsgEmitter;
    MSMessageEmitter *myHBMsgEmitter;
#endif

    /// @brief This Vehicle's parameter.
    const SUMOVehicleParameter* myParameter;

    /// @brief This Vehicle's route.
    const MSRoute* myRoute;

    /// @brief This Vehicles driving state (pos and speed)
    State myState;

    /// An individual speed for an vehicle that is used (iff set) instead of
    /// the maximal speed of the vehicle class.
    /// NOTE: This is just a little workaround for allowing an external
    ///       influence on the actual speed
    SUMOReal myIndividualMaxSpeed;

    /// is true if there has an individual speed been set
    bool myHasIndividualMaxSpeed;

    /// the speed which served as reference when calculating the individual maxspeed
    SUMOReal myReferenceSpeed;

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel *myLaneChangeModel;

    const MSVehicleType *myType;

    mutable const MSEdge *myLastBestLanesEdge;
    mutable std::vector<std::vector<LaneQ> > myBestLanes;
    mutable std::vector<LaneQ>::iterator myCurrentLaneInBestLanes;
    static std::vector<MSLane*> myEmptyLaneVector;

    std::map<MSCORN::Pointer, void*> myPointerCORNMap;
    std::map<MSCORN::Function, int> myIntCORNMap;

    /// @brief The vehicle's list of stops
    std::list<Stop> myStops;

    /// the position on the destination lane where the vehicle stops
    SUMOReal myArrivalPos;

    /// @brief The current acceleration before dawdling
    SUMOReal myPreDawdleAcceleration;

    /// @brief The information into which lanes the vehicle laps into
    std::vector<MSLane*> myFurtherLanes;

    /// @brief State of things of the vehicle that can be on or off
    int mySignals;


private:
    /// @brief The devices this vehicle has
    std::vector<MSDevice*> myDevices;

    /// @brief Iterator to current route-edge
    MSRouteIterator myCurrEdge;


    struct DriveProcessItem {
        MSLink *myLink;
        SUMOReal myVLinkPass;
        SUMOReal myVLinkWait;
        SUMOTime myArrivalTime;
        SUMOReal myArrivalSpeed;
        SUMOReal myDistance;
        bool mySetRequest;
        DriveProcessItem(MSLink *link, SUMOReal vPass, SUMOReal vWait, bool setRequest,
                         SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal distance) :
                myLink(link), myVLinkPass(vPass), myVLinkWait(vWait), mySetRequest(setRequest),
                myArrivalTime(arrivalTime), myArrivalSpeed(arrivalSpeed), myDistance(distance) { };
    };

    typedef std::vector< DriveProcessItem > DriveItemVector;

    /// Container for used Links/visited Lanes during lookForward.
    DriveItemVector myLFLinkLanes;


    /// @name Move reminder structures
    /// @{

    /// @brief Definition of a move reminder container
    typedef std::vector< MSMoveReminder* > MoveReminderCont;

    /// @brief Current lane's move reminder
    MoveReminderCont myMoveReminders;

    /// @brief Prior lanes' move reminder
    MoveReminderCont myOldLaneMoveReminders;

    /// @brief Definition of a vector of offset to prior move reminder
    typedef std::vector<SUMOReal> OffsetVector;

    /// @brief Offsets for prior move reminder
    OffsetVector myOldLaneMoveReminderOffsets;
    /// @}

    typedef std::vector<MSVehicleQuitReminded*> QuitRemindedVector;
    QuitRemindedVector myQuitReminded;



    /**
     * @class RouteReplaceInfo
     * @brief Information about a replaced route
     *
     * Generated optionally and stored in a vector within the Pointer-CORN-map
     *  this structure contains information about a replaced route: the edge
     *  the route was replaced at by a new one, the time this was done, and
     *  the previous route.
     */
    class RouteReplaceInfo {
    public:
        /// Constructor
        RouteReplaceInfo(const MSEdge * const edge_, const SUMOTime time_, const MSRoute * const route_)
                : edge(edge_), time(time_), route(route_) {}

        /// Destructor
        ~RouteReplaceInfo() { }

        /// The edge the vehicle was on when the route was replaced
        const MSEdge * edge;

        /// The time the route was replaced
        SUMOTime time;

        /// The prior route
        const MSRoute * route;

    };

    /// Definition of the vector which stores information about replaced routes
    typedef std::vector<RouteReplaceInfo> ReplacedRoutesVector;


    struct DepartArrivalInformation {
        SUMOTime time;
        MSLane *lane;
        SUMOReal pos;
        SUMOReal speed;
    };


    /// @brief The vehicle's knowledge about edge efforts/travel times; @see MSEdgeWeightsStorage
    MSEdgeWeightsStorage *myEdgeWeights;



#ifndef NO_TRACI
    /* indicates whether the vehicle is adapting its speed caused by the TraCI command slowDown*/
    bool adaptingSpeed;
    bool isLastAdaption;

    /* speed of the vehicle before any speed adaption began */
    SUMOReal speedBeforeAdaption;

    /* speed of the vehicle before influence of TraCI settings takes place */
    SUMOReal speedWithoutTraciInfluence;

    /* the amount by wich the speed shall be reduced */
    SUMOReal speedReduction;

    /* simulation time, when the last speed adaption started */
    SUMOTime timeBeforeAdaption;

    /* duration of the last speed adaption */
    SUMOTime adaptDuration;

    /* simulation time when the last lane change was forced */
    SUMOTime timeBeforeLaneChange;

    /* duration for which the last lane change will be in effect */
    SUMOTime laneChangeStickyTime;

    /* lane index of the destination road map position for an active lane change*/
    unsigned myDestinationLane;

    /* true if any forced lane change is in effect*/
    bool laneChangeConstraintActive;

    SUMOReal myTraCISpeed;

#endif

private:
    /// @brief invalidated default constructor
    MSVehicle();

    /// @brief invalidated copy constructor
    MSVehicle(const MSVehicle&);

    /// @brief invalidated assignment operator
    MSVehicle& operator=(const MSVehicle&);


};


#endif

/****************************************************************************/

