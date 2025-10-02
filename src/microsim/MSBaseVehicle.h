/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSBaseVehicle.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 8 Nov 2010
///
// A base class for vehicle implementations
/****************************************************************************/
#pragma once
#include <config.h>

#include <iostream>
#include <vector>
#include <set>
#include <utils/common/StdDefs.h>
#include <utils/emissions/EnergyParams.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/vehicle/SUMOVehicle.h>
#include "MSRoute.h"
#include "MSMoveReminder.h"
#include "MSVehicleType.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSStop;
class MSDevice_Transportable;
class MSDevice_Emissions;
class MSVehicleDevice;
class MSEdgeWeightsStorage;
class MSChargingStation;
class StoppingPlaceMemory;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBaseVehicle
 * @brief The base class for microscopic and mesoscopic vehicles
 */
class MSBaseVehicle : public SUMOVehicle {
public:
    // XXX: This definition was introduced to make the MSVehicle's previousSpeed
    //      available in the context of MSMoveReminder::notifyMove(). Another solution
    //      would be to modify notifyMove()'s interface to work with MSVehicle instead
    //      of SUMOVehicle (it is only called with MSVehicles!). Refs. #2579
    /** @brief Returns the vehicle's previous speed
     * @return The vehicle's speed
     */
    double getPreviousSpeed() const;

    friend class GUIBaseVehicle;

    /** @enum RouteValidity
     */
    enum RouteValidity {
        ROUTE_VALID = 0,
        ROUTE_UNCHECKED = 1 << 0,
        /// route was checked and is valid
        ROUTE_INVALID = 1 << 1,
        // starting edge permissions invalid (could change)
        ROUTE_START_INVALID_PERMISSIONS = 1 << 2,
        // insertion lane does not exist
        ROUTE_START_INVALID_LANE = 1 << 3
    };

    /** @brief Constructor
     * @param[in] pars The vehicle description
     * @param[in] route The vehicle's route
     * @param[in] type The vehicle's type
     * @param[in] speedFactor The factor for driven lane's speed limits
     * @exception ProcessError If a value is wrong
     */
    MSBaseVehicle(SUMOVehicleParameter* pars, ConstMSRoutePtr route,
                  MSVehicleType* type, const double speedFactor);


    /// @brief Destructor
    virtual ~MSBaseVehicle();

    virtual void initDevices();

    bool isVehicle() const {
        return true;
    }

    /// @brief set the id (inherited from Named but forbidden for vehicles)
    void setID(const std::string& newID);

    /** @brief Returns the vehicle's parameter (including departure definition)
     *
     * @return The vehicle's parameter
     */
    const SUMOVehicleParameter& getParameter() const;

    /// @brief retrieve parameters of devices, models and the vehicle itself
    std::string getPrefixedParameter(const std::string& key, std::string& error) const;

    /// @brief replace the vehicle parameter (deleting the old one)
    void replaceParameter(const SUMOVehicleParameter* newParameter);

    /// @brief check whether the vehicle is equiped with a device of the given name
    bool hasDevice(const std::string& deviceName) const;

    /// @brief create device of the given type
    void createDevice(const std::string& deviceName);

    /// @brief try to retrieve the given parameter from any of the vehicles devices, raise InvalidArgument if no device parameter by that name exists
    std::string getDeviceParameter(const std::string& deviceName, const std::string& key) const;

    /// @brief try to set the given parameter from any of the vehicles devices, raise InvalidArgument if no device parameter by that name exists
    void setDeviceParameter(const std::string& deviceName, const std::string& key, const std::string& value);

    /// @brief set individual junction model paramete (not type related)
    void setJunctionModelParameter(const std::string& key, const std::string& value);

    /// @brief set individual carFollow model parameters (not type related)
    void setCarFollowModelParameter(const std::string& key, const std::string& value);

    /** @brief Returns the current route
     * @return The route the vehicle uses
     */
    inline const MSRoute& getRoute() const {
        return *myRoute;
    }

    /** @brief Returns the current route
     * @return The route the vehicle uses
     */
    inline ConstMSRoutePtr getRoutePtr() const {
        return myRoute;
    }

    /** @brief Returns the vehicle's type definition
     * @return The vehicle's type definition
     */
    inline const MSVehicleType& getVehicleType() const {
        return *myType;
    }

    /** @brief Returns the vehicle's type parameter
     * @return The vehicle's type parameter
     */
    inline const SUMOVTypeParameter& getVTypeParameter() const {
        return myType->getParameter();
    }

    /** @brief Returns the vehicle's access class
     * @return The vehicle's access class
     */
    inline SUMOVehicleClass getVClass() const {
        return myType->getParameter().vehicleClass;
    }

    /** @brief Returns whether this object is ignoring transient permission
     * changes (during routing)
     */
    bool ignoreTransientPermissions() const;

    /** @brief Returns the maximum speed (the minimum of desired and technical maximum speed)
     * @return The vehicle's maximum speed
     */
    double getMaxSpeed() const;

    /** @brief Returns the nSuccs'th successor of edge the vehicle is currently at
     *
     * If the rest of the route (counted from the current edge) has less than nSuccs edges,
     *  0 is returned.
     * @param[in] nSuccs The number of edge to look forward
     * @return The nSuccs'th following edge in the vehicle's route
     */
    const MSEdge* succEdge(int nSuccs) const;

    /** @brief Returns the edge the vehicle is currently at
     *
     * @return The current edge in the vehicle's route
     */
    const MSEdge* getEdge() const;

    /** @brief Returns the edge the vehicle is currently at (possibly an
     * internal edge)
     */
    virtual const MSEdge* getCurrentEdge() const {
        return getEdge();
    }

    /// @brief returns the numerical ids of edges to travel
    const std::set<SUMOTrafficObject::NumericalID> getUpcomingEdgeIDs() const;

    /** @brief Returns whether the vehicle stops at the given stopping place */
    bool stopsAt(MSStoppingPlace* stop) const;

    /** @brief Returns whether the vehicle stops at the given edge */
    bool stopsAtEdge(const MSEdge* edge) const;

    /// @brief returns the next edge (possibly an internal edge)
    virtual const MSEdge* getNextEdgePtr() const {
        return nullptr;
    }

    /** @brief Returns the information whether the vehicle is on a road (is simulated)
     * @return Whether the vehicle is simulated
     */
    virtual bool isOnRoad() const {
        return true;
    }

    /** @brief Returns the information whether the vehicle is fully controlled
     * via TraCI
     * @return Whether the vehicle is remote-controlled
     */
    virtual bool isRemoteControlled() const {
        return false;
    }

    virtual bool wasRemoteControlled(SUMOTime lookBack = DELTA_T) const {
        UNUSED_PARAMETER(lookBack);
        return false;
    }

    /** @brief Returns the information whether the front of the vehhicle is on the given lane
     * @return Whether the vehicle's front is on that lane
     */
    virtual bool isFrontOnLane(const MSLane*) const {
        return true;
    }

    /** @brief Get the vehicle's lateral position on the lane
     * @return The lateral position of the vehicle (in m relative to the
     * centerline of the lane)
     */
    virtual double getLateralPositionOnLane() const {
        return 0;
    }

    /** @brief Get the vehicle's lateral position on the edge of the given lane
     * (or its current edge if lane == 0)
     * @return The lateral position of the vehicle (in m distance between right
     * side of vehicle and ride side of edge
     */
    virtual double getRightSideOnEdge(const MSLane* lane = 0) const {
        UNUSED_PARAMETER(lane);
        return 0;
    }

    /** @brief Returns the starting point for reroutes (usually the current edge)
     *
     * This differs from *myCurrEdge only if the vehicle is on an internal edge
     * @return The rerouting start point
     */
    virtual ConstMSEdgeVector::const_iterator getRerouteOrigin() const {
        return myCurrEdge;
    }

    /** @brief Returns the end point for reroutes (usually the last edge of the route)
     *
     * @return The rerouting end point
     */
    virtual const MSEdge* getRerouteDestination() const {
        return myRoute->getLastEdge();
    }

    /** @brief Returns the time loss in seconds
     */
    virtual double getTimeLossSeconds() const {
        // better timeLoss for meso?
        return 0;
    }

    /** @brief Returns the number of seconds waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * Intentional stopping does not count towards this time.
     * @return The time the vehicle is standing
     */
    double getWaitingSeconds() const {
        return STEPS2TIME(getWaitingTime());
    }



    /** @brief Returns an iterator pointing to the current edge in this vehicles route
     * @return The current route pointer
     */
    const MSRouteIterator& getCurrentRouteEdge() const {
        return myCurrEdge;
    }


    /** @brief Performs a rerouting using the given router
     *
     * Tries to find a new route between the current edge and the destination edge, first.
     * Tries to replace the current route by the new one using replaceRoute.
     *
     * @param[in] t The time for which the route is computed
     * @param[in] router The router to use
     * @param[in] sink (optionally) a new destination edge
     * @see replaceRoute
     */
    bool reroute(SUMOTime t, const std::string& info, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, const bool onInit = false, const bool withTaz = false, const bool silent = false, const MSEdge* sink = nullptr);


    /** @brief Replaces the current route by the given edges
     *
     * It is possible that the new route is not accepted, if a) it does not
     *  contain the vehicle's current edge, or b) something fails on insertion
     *  into the routes container (see in-line comments).
     *
     * @param[in] edges The new list of edges to pass
     * @param[in] onInit Whether the vehicle starts with this route
     * @param[in] check Whether the route should be checked for validity
     * @param[in] removeStops Whether stops should be removed if they do not fit onto the new route
     * @return Whether the new route was accepted
     */
    bool replaceRouteEdges(ConstMSEdgeVector& edges, double cost, double savings, const std::string& info, bool onInit = false, bool check = false, bool removeStops = true,
                           std::string* msgReturn = nullptr);

    /** @brief Replaces the current route by the given one
     *
     * It is possible that the new route is not accepted, if it does not
     *  contain the vehicle's current edge.
     *
     * @param[in] route The new route to pass
     * @param[in] info Information regarding the replacement
     * @param[in] addRouteStops Whether stops from the replacement route should be added
     * @param[in] removeStops Whether stops should be removed if they do not fit onto the new route
     * @return Whether the new route was accepted
     */
    virtual bool replaceRoute(ConstMSRoutePtr route, const std::string& info, bool onInit = false, int offset = 0, bool addRouteStops = true, bool removeStops = true,
                              std::string* msgReturn = nullptr);

    /** @brief Returns the vehicle's acceleration
     *
     * This default implementation returns always 0.
     * @return The acceleration
     */
    virtual double getAcceleration() const;

    /** @brief Called when the vehicle is inserted into the network
     *
     * Sets optional information about departure time, informs the vehicle
     *  control about a further running vehicle.
     */
    void onDepart();

    /** @brief Returns this vehicle's real departure time
     * @return This vehicle's real departure time
     */
    inline SUMOTime getDeparture() const {
        return myDeparture;
    }

    /** @brief Returns the depart delay */
    SUMOTime getDepartDelay() const;

    /** @brief Returns the estimated public transport stop (departure) delay in seconds
     */
    virtual double getStopDelay() const {
        /// @todo implement for meso
        return -1;
    }

    /** @brief Returns the estimated public transport stop arrival delay in seconds
     */
    virtual double getStopArrivalDelay() const {
        /// @todo implement for meso
        return INVALID_DOUBLE;
    }

    /// @brief return time (s) and distance to the next stop
    virtual std::pair<double, double> estimateTimeToNextStop() const {
        return std::make_pair(-1, -1);
    }

    /** @brief Returns this vehicle's real departure position
     * @return This vehicle's real departure position
     */
    inline double getDepartPos() const {
        return myDepartPos;
    }

    /** @brief Returns this vehicle's desired arrivalPos for its current route
     * (may change on reroute)
     * @return This vehicle's real arrivalPos
     */
    virtual double getArrivalPos() const {
        return myArrivalPos;
    }

    virtual int getArrivalLane() const {
        return myArrivalLane;
    }

    /** @brief Sets this vehicle's desired arrivalPos for its current route
     */
    virtual void setArrivalPos(double arrivalPos) {
        myArrivalPos = arrivalPos;
    }

    /** @brief Called when the vehicle is removed from the network.
     *
     * Moves along work reminders and
     *  informs all devices about quitting. Calls "leaveLane" then.
     *
     * @param[in] reason why the vehicle leaves (reached its destination, parking, teleport)
     */
    virtual void onRemovalFromNet(const MSMoveReminder::Notification /*reason*/) {}

    /** @brief Returns whether this vehicle has already departed
     */
    inline bool hasDeparted() const {
        return myDeparture != NOT_YET_DEPARTED;
    }

    /** @brief Returns whether this vehicle has already arived
     * (by default this is true if the vehicle has reached its final edge)
     */
    virtual bool hasArrived() const;

    /// @brief return index of edge within route
    int getRoutePosition() const;

    /// @brief return the number of edges remaining in the route (include the current)
    int getNumRemainingEdges() const;

    int getArrivalIndex() const {
        return myParameter->arrivalEdge;
    }

    /// @brief reset index of edge within route
    void resetRoutePosition(int index, DepartLaneDefinition departLaneProcedure);

    /** @brief Returns the distance that was already driven by this vehicle
     * @return the distance driven [m]
     */
    double getOdometer() const;

    /// @brief Manipulate the odometer
    void addToOdometer(double value) {
        myOdometer += value;
    }

    /** @brief Returns the number of new routes this vehicle got
     * @return the number of new routes this vehicle got
     */
    inline int getNumberReroutes() const {
        return myNumberReroutes;
    }

    /// @brief Returns this vehicles impatience
    double getImpatience() const;

    /** @brief Returns the number of persons
     * @return The number of passengers on-board
     */
    int getPersonNumber() const;

    /** @brief Returns the number of leaving persons
     * @return The number of leaving passengers
     */
    int getLeavingPersonNumber() const;

    /** @brief Returns the list of persons
     * @return The list of passengers on-board
     */
    std::vector<std::string> getPersonIDList() const;

    /** @brief Returns the number of containers
     * @return The number of contaiers on-board
     */
    int getContainerNumber() const;


    /** @brief Returns this vehicle's devices
     * @return This vehicle's devices
     */
    inline const std::vector<MSVehicleDevice*>& getDevices() const {
        return myDevices;
    }

    /// @brief whether the given transportable is allowed to board this vehicle
    bool allowsBoarding(const MSTransportable* t) const;

    /** @brief Adds a person or container to this vehicle
     *
     * @param[in] transportable The person/container to add
     */
    virtual void addTransportable(MSTransportable* transportable);

    /// @brief removes a person or container
    void removeTransportable(MSTransportable* t);

    /// @brief removes a person or containers mass
    void removeTransportableMass(MSTransportable* t);

    /// @brief retrieve riding persons
    const std::vector<MSTransportable*>& getPersons() const;

    /// @brief retrieve riding containers
    const std::vector<MSTransportable*>& getContainers() const;

    /// @brief returns whether the vehicle serves a public transport line that serves the given stop
    bool isLineStop(double position) const;

    /// @brief check wether the vehicle has jump at the given part of its route
    bool hasJump(const MSRouteIterator& it) const;

    /** @brief Validates the current or given route
     * @param[out] msg Description why the route is not valid (if it is the case)
     * @param[in] route The route to check (or 0 if the current route shall be checked)
     * @return Whether the vehicle's current route is valid
     */
    bool hasValidRoute(std::string& msg, ConstMSRoutePtr route = 0) const;

    bool hasValidRoute(std::string& msg, MSRouteIterator start, MSRouteIterator last, bool checkJumps) const;

    /// @brief checks wether the vehicle can depart on the first edge
    virtual bool hasValidRouteStart(std::string& msg);

    /// @brief check for route validity at first insertion attempt
    int getRouteValidity(bool update = true, bool silent = false, std::string* msgReturn = nullptr);

    /// @brief Checks whether the vehilce has the given MoveReminder
    bool hasReminder(MSMoveReminder* rem) const;

    /** @brief Adds a MoveReminder dynamically
     *
     * @param[in] rem the reminder to add
     * @see MSMoveReminder
     */
    void addReminder(MSMoveReminder* rem, double pos = 0);

    /** @brief Removes a MoveReminder dynamically
     *
     * @param[in] rem the reminder to remove
     * @see MSMoveReminder
     */
    void removeReminder(MSMoveReminder* rem);

    /** @brief "Activates" all current move reminder
     *
     * For all move reminder stored in "myMoveReminders", their method
     *  "MSMoveReminder::notifyEnter" is called.
     *
     * @param[in] reason The reason for changing the reminders' states
     * @param[in] enteredLane The lane, which is entered (if applicable)
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    virtual void activateReminders(const MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);


    /** @brief Returns the vehicle's length
     * @return vehicle's length
     */
    inline double getLength() const {
        return myType->getLength();
    }

    /* @brief Return whether this vehicle must be treated like a railway vehicle
     * either due to its vClass or the vClass of it's edge */
    bool isRail() const;

    /** @brief Returns the vehicle's width
     * @return vehicle's width
     */
    inline double getWidth() const {
        return myType->getWidth();
    }


    /** @brief Returns the precomputed factor by which the driver wants to be faster than the speed limit
     * @return Speed limit factor
     */
    inline double getChosenSpeedFactor() const {
        return myChosenSpeedFactor;
    }

    inline double getDesiredMaxSpeed() const {
        return myType->getDesiredMaxSpeed() * myChosenSpeedFactor;
    }

    /** @brief Returns the precomputed factor by which the driver wants to be faster than the speed limit
     * @return Speed limit factor
     */
    inline void setChosenSpeedFactor(const double factor) {
        myChosenSpeedFactor = factor;
    }

    /// @brief Returns a device of the given type if it exists, nullptr otherwise
    MSDevice* getDevice(const std::type_info& type) const;


    /** @brief Replaces the current vehicle type by the one given
     *
     * If the currently used vehicle type is marked as being used by this vehicle
     *  only, it is deleted, first. The new, given type is then assigned to
     *  "myType".
     * @param[in] type The new vehicle type
     * @see MSBaseVehicle::myType
     */
    virtual void replaceVehicleType(const MSVehicleType* type);


    /** @brief Replaces the current vehicle type with a new one used by this vehicle only
     *
     * If the currently used vehicle type is already marked as being used by this vehicle
     *  only, no new type is created.
     * @return The new modifiable vehicle type
     * @see MSBaseVehicle::myType
     */
    MSVehicleType& getSingularType();

    /// @name state io
    //@{

    /// Saves the (common) state of a vehicle
    virtual void saveState(OutputDevice& out);

    //@}

    virtual bool handleCollisionStop(MSStop& stop, const double distToStop);

    /** @brief Returns whether the vehicle is at a stop
     * @return Whether the vehicle has stopped
     */
    bool isStopped() const;

    /** @brief Returns whether the vehicle is parking
     * @return whether the vehicle is parking
     */
    bool isParking() const;

    /** @brief Returns whether the vehicle is perform a jump
     * @return whether the vehicle is starting to jump
     */
    bool isJumping() const;

    /** @brief Returns whether the logical state of the vehicle is reversed - for drawing
    * @return whether the logical state of the vehicle is reversed
    */
    inline bool isReversed() const {
        return myAmReversed;
    }

    /** @brief Returns whether the vehicle is on a triggered stop
     * @return whether the vehicle is on a triggered stop
     */
    bool isStoppedTriggered() const;

    /** @brief Returns whether the vehicle is on a parking stop
     * @return whether the vehicle is on a parking stop
     */
    bool isStoppedParking() const;

    /** @brief return whether the given position is within range of the current stop
     */
    bool isStoppedInRange(const double pos, const double tolerance, bool checkFuture = false) const;

    /** @brief Returns whether the vehicle has to stop somewhere
     * @return Whether the vehicle has to stop somewhere
     */
    bool hasStops() const {
        return !myStops.empty();
    }

    /** @brief replace the current parking area stop with a new stop with merge duration
     */
    bool replaceParkingArea(MSParkingArea* parkingArea, std::string& errorMsg);

    /** @brief get the upcoming parking area stop or nullptr
     */
    MSParkingArea* getNextParkingArea();

    /** @brief get the current  parking area stop or nullptr */
    MSParkingArea* getCurrentParkingArea();

    /// @brief get the valid parking access rights (vehicle settings override vehicle type settings)
    const std::vector<std::string>& getParkingBadges() const;

    /// @brief departure position where the vehicle fits fully onto the edge (if possible)
    double basePos(const MSEdge* edge) const;

    /** @brief Adds a stop
     *
     * The stop is put into the sorted list.
     * @param[in] stop The stop to add
     * @return Whether the stop could be added
     */
    bool addStop(const SUMOVehicleParameter::Stop& stopPar, std::string& errorMsg, SUMOTime untilOffset = 0,
                 MSRouteIterator* searchStart = nullptr);

    /** @brief Adds stops to the built vehicle
     *
     * This code needs to be separated from the MSBaseVehicle constructor
     *  since it is not allowed to call virtual functions from a constructor
     *
     * @param[in] ignoreStopErrors whether invalid stops trigger a warning only
     */
    void addStops(const bool ignoreStopErrors, MSRouteIterator* searchStart = nullptr, bool addRouteStops = true);

    /// @brief check whether all stop.edge MSRouteIterators are valid and in order
    bool haveValidStopEdges(bool silent = false) const;

    /// @brief return list of route indices for the remaining stops
    std::vector<std::pair<int, double> > getStopIndices() const;

    /**
    * returns the list of stops not yet reached in the stop queue
    * @return the list of upcoming stops
    */
    inline const std::list<MSStop>& getStops() const {
        return myStops;
    }

    inline const StopParVector& getPastStops() const {
        return myPastStops;
    }

    /**
    * returns the next imminent stop in the stop queue
    * @return the upcoming stop
    */
    const MSStop& getNextStop() const;

    /**
    * returns the next imminent stop in the stop queue
    * @return the upcoming stop
    */
    MSStop& getNextStopMutable();

    /// @brief get remaining stop duration or 0 if the vehicle isn't stopped
    SUMOTime getStopDuration() const;

    /**
    * returns the upcoming stop with the given index in the stop queue
    * @return an upcoming stop
    */
    MSStop& getStop(int nextStopIndex);

    /// @brief return parameters for the next stop (SUMOVehicle Interface)
    const SUMOVehicleParameter::Stop* getNextStopParameter() const;

    /**
     * schedule a new stop for the vehicle; each time a stop is reached, the vehicle
     * will wait for the given duration before continuing on its route
     * @param[in] stop Stop parameters
     * @param[out] errorMsg returned error message
     */
    virtual bool addTraciStop(SUMOVehicleParameter::Stop stop, std::string& errorMsg);

    /**
    * resumes a vehicle from stopping
    * @return true on success, the resuming fails if the vehicle wasn't parking in the first place
    */
    virtual bool resumeFromStopping() = 0;

    /// @brief mark vehicle as active
    void unregisterWaiting();

    /// @brief deletes the next stop at the given index if it exists
    bool abortNextStop(int nextStopIndex = 0);

    /**
     * replace the next stop at the given index with the given stop parameters
     * will wait for the given duration before continuing on its route
     * The route between start other stops and destination will be kept unchanged and
     * only the part around the replacement index will be adapted according to the new stop location
     * @param[in] nextStopIndex The replacement index
     * @param[in] stop Stop parameters
     * @param[in] info The rerouting info
     * @param[in] teleport Whether to cover the route to the replacement stop via teleporting
     * @param[out] errorMsg returned error message
     */
    bool replaceStop(int nextStopIndex, SUMOVehicleParameter::Stop stop, const std::string& info, bool teleport, std::string& errorMsg);

    /**
     * reroute between stops nextStopIndex - 1 and nextStopIndex (defaults to current position / final edge) if the respective stops do not exist
     * @param[in] nextStopIndex The replacement index
     * @param[in] info The rerouting info
     * @param[in] teleport Whether to cover the route between stops via teleporting
     * @param[out] errorMsg returned error message
     */
    bool rerouteBetweenStops(int nextStopIndex, const std::string& info, bool teleport, std::string& errorMsg);

    /**
     * insert stop at the given index with the given stop parameters
     * will wait for the given duration before continuing on its route
     * The route will be adapted to pass the new stop edge but only from the previous stop (or start) to the new stop and only up to the next stop (or end).
     * @param[in] nextStopIndex The replacement index
     * @param[in] stop Stop parameters
     * @param[in] info The rerouting info
     * @param[in] teleport Whether to cover the route to the new stop via teleporting
     * @param[out] errorMsg returned error message
     */
    bool insertStop(int nextStopIndex, SUMOVehicleParameter::Stop stop, const std::string& info, bool teleport, std::string& errorMsg);


    /// @brief whether this vehicle is selected in the GUI
    virtual bool isSelected() const {
        return false;
    }

    /// @brief @return The index of the vehicle's associated RNG
    int getRNGIndex() const;

    /// @brief @return The vehicle's associated RNG
    SumoRNG* getRNG() const;

    inline NumericalID getNumericalID() const {
        return myNumericalID;
    }

    const MSDevice_Transportable* getPersonDevice() const {
        return myPersonDevice;
    }

    const MSDevice_Transportable* getContainerDevice() const {
        return myContainerDevice;
    }

    /// @brief retrieve parameters for the energy consumption model
    inline EnergyParams* getEmissionParameters() const {
        if (myEnergyParams == nullptr) {
            myEnergyParams = new EnergyParams(getVehicleType().getEmissionParameters());
        }
        return myEnergyParams;
    }

    /// @name Emission retrieval
    //@{

    /** @brief Returns emissions of the current state
     * The value is always per 1s, so multiply by step length if necessary.
     * @return The current emission
     */
    template<PollutantsInterface::EmissionType ET>
    double getEmissions() const {
        if (isOnRoad() || isIdling()) {
            return PollutantsInterface::compute(myType->getEmissionClass(), ET, getSpeed(), getAcceleration(), getSlope(), getEmissionParameters());
        }
        return 0.;
    }

    /** @brief Returns actual state of charge of battery (Wh)
    * RICE_CHECK: This may be a misnomer, SOC is typically percentage of the maximum battery capacity.
    * @return The actual battery state of charge
    */
    double getStateOfCharge() const;

    /** @brief Returns actual relative state of charge of battery (-)
    * @return The actual relative battery state of charge, normalised to the maximum battery capacity.
    */
    double getRelativeStateOfCharge() const;

    /** @brief Returns the energy charged to the battery in the current time step (Wh)
    * @return The energy charged to the battery in the current time step.
    */
    double getChargedEnergy() const;

    /** @brief Returns the maximum charge rate allowed by the battery in the current time step (W)
    * @return The maximum charge rate in the current time step.
    */
    double getMaxChargeRate() const;

    /** @brief Returns actual current (A) of ElecHybrid device
    * RICE_CHECK: Is this the current consumed from the overhead wire or the current driving the powertrain of the vehicle?
    * RICE_REV_JS: It is the current drawn from the overhead wire (value if the vehicle is not connected to overhead wire?)
    * @return The current of ElecHybrid device
    */
    double getElecHybridCurrent() const;

    /** @brief Returns noise emissions of the current state
     * @return The noise produced
     */
    double getHarmonoise_NoiseEmissions() const;
    //@}

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
    class BaseInfluencer {
    public:
        /// @brief Constructor
        BaseInfluencer();

        /// @brief Destructor
        virtual ~BaseInfluencer() {}

        /// @brief Static initalization
        static void init();
        /// @brief Static cleanup
        static void cleanup();


        /// @brief return the current routing mode
        double getExtraImpatience() const {
            return myExtraImpatience;
        }

        /** @brief Sets routing behavior
         * @param[in] value an enum value controlling the different modes
         */
        void setExtraImpatience(double value) {
            myExtraImpatience = value;
        }

    protected:
        /// @brief dynamic impatience offset
        double myExtraImpatience = 0;

    };



    /** @brief Returns the velocity/lane influencer
     *
     * If no influencer was existing before, one is built, first
     * @return Reference to this vehicle's speed influencer
     */
    virtual BaseInfluencer& getBaseInfluencer() = 0;

    virtual const BaseInfluencer* getBaseInfluencer() const = 0;

    virtual bool hasInfluencer() const  = 0;

    /// @brief return routing mode (configures router choice but also handling of transient permission changes)
    int getRoutingMode() const {
        return myRoutingMode;
    }

    /** @brief Sets routing behavior
     * @param[in] value an enum value controlling the different modes
     */
    void setRoutingMode(int value) {
        myRoutingMode = value;
    }


    SUMOAbstractRouter<MSEdge, SUMOVehicle>& getRouterTT() const;

    /** @brief Returns the vehicle's internal edge travel times/efforts container
     *
     * If the vehicle does not have such a container, it is built.
     * @return The vehicle's knowledge about edge weights
     */
    const MSEdgeWeightsStorage& getWeightsStorage() const;
    MSEdgeWeightsStorage& getWeightsStorage();

    /** @brief Returns the leader of the vehicle looking for a fixed distance.
     *
     * If the distance is not given it is calculated from the brake gap.
     * The gap returned does not include the minGap.
     * @param dist    up to which distance to look at least for a leader
     * @param considerCrossingFoes Whether vehicles on crossing foe links should be considered
     * @return The leading vehicle together with the gap; (0, -1) if no leader was found.
     */
    virtual std::pair<const MSVehicle* const, double> getLeader(double dist = 0, bool considerCrossingFoes = true) const {
        UNUSED_PARAMETER(dist);
        UNUSED_PARAMETER(considerCrossingFoes);
        WRITE_WARNING(TL("getLeader not yet implemented for meso"));
        return std::make_pair(nullptr, -1);
    }

    /** @brief Returns the follower of the vehicle looking for a fixed distance.
     *
     * If the distance is not given it is set to the value of MSCFModel::brakeGap(2*roadSpeed, 4.5, 0)
     * The gap returned does not include the minGap.
     * If there are multiple followers, the one that maximizes the term (getSecureGap - gap) is returned.
     * @param dist    up to which distance to look at least for a leader
     * @return The leading vehicle together with the gap; (0, -1) if no leader was found.
     */
    virtual std::pair<const MSVehicle* const, double> getFollower(double dist = 0) const {
        UNUSED_PARAMETER(dist);
        WRITE_WARNING(TL("getFollower not yet implemented for meso"));
        return std::make_pair(nullptr, -1);
    }

    /** @brief (Re-)Calculates the arrival position and lane from the vehicle parameters
     */
    void calculateArrivalParams(bool onInit);

    /// @brief apply departEdge and arrivalEdge attributes
    void setDepartAndArrivalEdge();

    int getDepartEdge() const;

    int getInsertionChecks() const;

    /// @brief interpret stop lane on opposite side of the road
    static MSLane* interpretOppositeStop(SUMOVehicleParameter::Stop& stop);

    /// @name state io
    //@{
    void rememberBlockedParkingArea(const MSStoppingPlace* pa, bool local);
    SUMOTime sawBlockedParkingArea(const MSStoppingPlace* pa, bool local) const;
    void rememberBlockedChargingStation(const MSStoppingPlace* cs, bool local);
    SUMOTime sawBlockedChargingStation(const MSStoppingPlace* cs, bool local) const;

    /// @brief score only needed when running with gui
    void rememberParkingAreaScore(const MSStoppingPlace* pa, const std::string& score);
    void resetParkingAreaScores();
    void rememberChargingStationScore(const MSStoppingPlace* cs, const std::string& score);
    void resetChargingStationScores();

    int getNumberParkingReroutes() const {
        return myNumberParkingReroutes;
    }
    void setNumberParkingReroutes(int value) {
        myNumberParkingReroutes = value;
    }

    const StoppingPlaceMemory* getParkingMemory() const {
        return myParkingMemory;
    }

    const StoppingPlaceMemory* getChargingMemory() const {
        return myChargingMemory;
    }
    //@}

protected:
    /// @brief reset rail signal approach information
    virtual void resetApproachOnReroute() {};

    struct StopEdgeInfo {

        StopEdgeInfo(const MSEdge* _edge, double _priority, SUMOTime _arrival, double _pos):
            edge(_edge), pos(_pos),
            priority(_priority), arrival(_arrival) {};
        const MSEdge* edge;
        double pos;
        double priority;
        SUMOTime arrival;
        /// @brief values set during routing and used during optimization
        int routeIndex = -1;
        bool skipped = false;
        bool backtracked = false;
        SUMOTime delay = 0;

        bool operator==(const StopEdgeInfo& o) const {
            return edge == o.edge;
        }
        bool operator!=(const StopEdgeInfo& o) const {
            return !(*this == o);
        }
    };

    /** @brief Returns the list of still pending stop edges
     * also returns the first and last stop position
     */
    std::vector<StopEdgeInfo> getStopEdges(double& firstPos, double& lastPos, std::set<int>& jumps) const;

    static double addStopPriority(double p1, double p2);


    ConstMSEdgeVector optimizeSkipped(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                                      const MSEdge* source, double sourcePos, std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges, SUMOTime maxDelay) const;

    ConstMSEdgeVector routeAlongStops(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                                      std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges,
                                      int originStop, SUMOTime maxDelay, double& skippedPrio2) const;


protected:
    /// @brief This vehicle's parameter.
    const SUMOVehicleParameter* myParameter;

    /// @brief This vehicle's route.
    ConstMSRoutePtr myRoute;

    /// @brief This vehicle's type.
    const MSVehicleType* myType;

    /// @brief Iterator to current route-edge
    MSRouteIterator myCurrEdge;

    /// @brief A precomputed factor by which the driver wants to be faster than the speed limit
    double myChosenSpeedFactor;

    /// @brief The vehicle's list of stops
    std::list<MSStop> myStops;

    /// @brief The list of stops that the vehicle has already reached
    StopParVector myPastStops;

    /// @name Move reminder structures
    /// @{

    /// @brief Definition of a move reminder container
    //         The double value holds the relative position offset, i.e.,
    //         offset + vehicle-position - moveReminder-position = distance,
    //         i.e. the offset is counted up when the vehicle continues to a
    //         succeeding lane.
    typedef std::vector< std::pair<MSMoveReminder*, double> > MoveReminderCont;

    /// @brief Currently relevant move reminders
    MoveReminderCont myMoveReminders;
    /// @}

    /// @brief The devices this vehicle has
    std::vector<MSVehicleDevice*> myDevices;

    /// @brief The passengers this vehicle may have
    MSDevice_Transportable* myPersonDevice;

    /// @brief The containers this vehicle may have
    MSDevice_Transportable* myContainerDevice;

    /// @brief The emission parameters this vehicle may have
    mutable EnergyParams* myEnergyParams;

    /// @brief The real departure time
    SUMOTime myDeparture;

    /// @brief The real depart position
    double myDepartPos;

    /// @brief The position on the destination lane where the vehicle stops
    double myArrivalPos;

    /// @brief The destination lane where the vehicle stops
    int myArrivalLane;

    /// @brief The number of reroutings
    int myNumberReroutes;

    /// @brief The offset when adding route stops with 'until' on route replacement
    SUMOTime myStopUntilOffset;

    /// @brief A simple odometer to keep track of the length of the route already driven
    double myOdometer;

    /// @brief status of the current vehicle route
    int myRouteValidity;

    /// memory for parking search
    StoppingPlaceMemory* myParkingMemory = nullptr;
    StoppingPlaceMemory* myChargingMemory = nullptr;
    int myNumberParkingReroutes = 0;

    /// @brief Whether this vehicle is registered as waiting for a person or container (for deadlock-recognition)
    bool myAmRegisteredAsWaiting = false;

    /* @brief magic value for undeparted vehicles
     * @note: in previous versions this was -1
     */
    static const SUMOTime NOT_YET_DEPARTED;

    static std::vector<MSTransportable*> myEmptyTransportableVector;

    /* @brief The logical 'reversed' state of the vehicle - intended to be used by drawing functions
     * @note:   only set by vClass rail reversing at the moment
     */
    bool myAmReversed = false;

    ///@brief routing mode (see TraCIConstants.h)
    int myRoutingMode;

private:
    const NumericalID myNumericalID;

    /* @brief The vehicle's knowledge about edge efforts/travel times; @see MSEdgeWeightsStorage
     * @note member is initialized on first access */
    mutable MSEdgeWeightsStorage* myEdgeWeights;

    MSEdgeWeightsStorage& _getWeightsStorage() const;

    static NumericalID myCurrentNumericalIndex;

    /// @brief init model parameters from generic params
    void initTransientModelParams();

    /// @brief reconstruct flow id from vehicle id
    std::string getFlowID() const;

    /// @brief remove route at the end of the simulation
    void checkRouteRemoval();

    /// @brief helper function
    bool insertJump(int nextStopIndex, MSRouteIterator itStart, std::string& errorMsg);

    /// @brief patch stop.pars.index to record the number of skipped candidate edges before stop.edge (in a looped route)
    void setSkips(MSStop& stop, int prevActiveStops);

    /// @brief remove outdated driveways on reroute
    SUMOTime activateRemindersOnReroute(SUMOTime currentTime);

private:
    /// invalidated assignment operator
    MSBaseVehicle& operator=(const MSBaseVehicle& s) = delete;

#ifdef _DEBUG
public:
    static void initMoveReminderOutput(const OptionsCont& oc);

protected:
    /// @brief optionally generate movereminder-output for this vehicle
    void traceMoveReminder(const std::string& type, MSMoveReminder* rem, double pos, bool keep) const;

    /// @brief whether this vehicle shall trace its moveReminders
    const bool myTraceMoveReminders;
private:
    /// @brief vehicles which shall trace their move reminders
    static std::set<std::string> myShallTraceMoveReminders;
#endif


};
