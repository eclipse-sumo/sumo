/****************************************************************************/
/// @file    SUMOVehicle.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 17 Feb 2009
/// @version $Id$
///
// Abstract base class for vehicle representations
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
#ifndef SUMOVehicle_h
#define SUMOVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <typeinfo>
#include <utils/common/SUMOTime.h>
#include <utils/common/Named.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class MSRoute;
class MSEdge;
class MSLane;
class MSDevice;
class MSPerson;
class MSTransportable;
class MSParkingArea;
class SUMOSAXAttributes;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOVehicle
 * @brief Representation of a vehicle
 */
class SUMOVehicle {
public:

    // XXX: This definition was introduced to make the MSVehicle's previousSpeed
    //      available in the context of MSMoveReminder::notifyMove(). Another solution
    //      would be to modify notifyMove()'s interface to work with MSVehicle instead
    //      of SUMOVehicle (it is only called with MSVehicles!). Refs. #2579
    /** @brief Returns the vehicle's previous speed
     * @return The vehicle's speed
     */
    virtual SUMOReal getPreviousSpeed() const = 0;


    typedef Named::NamedLikeComparatorIdLess<SUMOVehicle> ComparatorIdLess;

    /// @brief Destructor
    virtual ~SUMOVehicle() {}

    /** @brief Get the vehicle's ID
     * @return The the ID of the vehicle
     */
    virtual const std::string& getID() const = 0;

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual SUMOReal getPositionOnLane() const = 0;

    /** @brief Get the vehicle's back position along the given lane
     * @return The position of the vehicle (in m from the given lane's begin)
     */
    virtual SUMOReal getBackPositionOnLane(const MSLane* lane) const = 0;

    /** @brief Get the vehicle's lateral position on the lane
     * @return The lateral position of the vehicle (in m relative to the
     * centerline of the lane)
     */
    virtual SUMOReal getLateralPositionOnLane() const = 0;

    /** @brief Get the vehicle's angle
     * @return The angle of the vehicle (in degree)
     */
    virtual SUMOReal getAngle() const = 0;

    /** @brief Return current position (x/y, cartesian)
     *
     * If the vehicle is not in the net, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    virtual Position getPosition(const SUMOReal offset = 0) const = 0;

    /** @brief Returns the vehicle's maximum speed
     * @return The vehicle's maximum speed
     */
    virtual SUMOReal getMaxSpeed() const = 0;

    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    virtual SUMOReal getSpeed() const = 0;

    /** @brief Returns the lane the vehicle is on
    * @return The vehicle's current lane
    */
    virtual MSLane* getLane() const = 0;

    /** @brief Returns the vehicle's type
     * @return The vehicle's type
     */
    virtual const MSVehicleType& getVehicleType() const = 0;

    /** @brief Returns the vehicle's access class
     * @return The vehicle's access class
     */
    virtual SUMOVehicleClass getVClass() const = 0;

    /// Returns the current route
    virtual const MSRoute& getRoute() const = 0;

    /** @brief Returns the nSuccs'th successor of edge the vehicle is currently at
     *
     * If the rest of the route (counted from the current edge) than nSuccs,
     *  0 is returned.
     * @param[in] nSuccs The number of edge to look forward
     * @return The nSuccs'th following edge in the vehicle's route
     */
    virtual const MSEdge* succEdge(int nSuccs) const = 0;

    /** @brief Replaces the current route by the given edges
     *
     * It is possible that the new route is not accepted, if a) it does not
     *  contain the vehicle's current edge, or b) something fails on insertion
     *  into the routes container (see in-line comments).
     *
     * @param[in] edges The new list of edges to pass
     * @param[in] onInit Whether the vehicle starts with this route
     * @param[in] check Whether the route should be checked for validity
     * @return Whether the new route was accepted
     */
    virtual bool replaceRouteEdges(ConstMSEdgeVector& edges, bool onInit = false, bool check = false, bool addStops = true) = 0;

    /// Replaces the current route by the given one
    virtual bool replaceRoute(const MSRoute* route, bool onInit = false, int offset = 0, bool addStops = true) = 0;

    /** @brief Performs a rerouting using the given router
     *
     * Tries to find a new route between the current edge and the destination edge, first.
     * Tries to replace the current route by the new one using replaceRoute.
     *
     * @param[in] t The time for which the route is computed
     * @param[in] router The router to use
     * @see replaceRoute
     */
    virtual void reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, const bool onInit = false, const bool withTaz = false) = 0;

    /** @brief Validates the current or given route
     * @param[out] msg Description why the route is not valid (if it is the case)
     * @param[in] route The route to check (or 0 if the current route shall be checked)
     * @return Whether the vehicle's current route is valid
     */
    virtual bool hasValidRoute(std::string& msg, const MSRoute* route = 0) const = 0;


    /** @brief Returns an iterator pointing to the current edge in this vehicles route
     * @return The current route pointer
     */
    virtual const ConstMSEdgeVector::const_iterator& getCurrentRouteEdge() const = 0;

    /** @brief Returns the vehicle's acceleration
     * @return The acceleration
     */
    virtual SUMOReal getAcceleration() const = 0;

    /** @brief Returns the slope of the road at vehicle's position
     * @return The slope
     */
    virtual SUMOReal getSlope() const = 0;

    /** @brief Returns the edge the vehicle is currently at
     *
     * @return The current edge in the vehicle's route
     */
    virtual const MSEdge* getEdge() const = 0;

    /** @brief Returns the vehicle's parameter (including departure definition)
     *
     * @return The vehicle's parameter
     */
    virtual const SUMOVehicleParameter& getParameter() const = 0;

    /** @brief Called when the vehicle is inserted into the network
     *
     * Sets optional information about departure time, informs the vehicle
     *  control about a further running vehicle.
     */
    virtual void onDepart() = 0;

    /** @brief Returns the information whether the vehicle is on a road (is simulated)
     * @return Whether the vehicle is simulated
     */
    virtual bool isOnRoad() const = 0;

    /** @brief Returns the information whether the front of the vehhicle is on the given lane
     * @return Whether the vehicle's front is on that lane
     */
    virtual bool isFrontOnLane(const MSLane*) const = 0;

    /** @brief Returns the information whether the vehicle is parked
     * @return Whether the vehicle is parked
     */
    virtual bool isParking() const = 0;

    /** @brief Returns the information whether the vehicle is fully controlled
     * via TraCI
     * @return Whether the vehicle is remote-controlled
     */
    virtual bool isRemoteControlled() const = 0;

    /** @brief Returns this vehicle's real departure time
     * @return This vehicle's real departure time
     */
    virtual SUMOTime getDeparture() const = 0;

    /** @brief Returns this vehicle's real departure position
     * @return This vehicle's real departure position
     */
    virtual SUMOReal getDepartPos() const = 0;

    /** @brief Returns this vehicle's desired arrivalPos for its current route
     * (may change on reroute)
     * @return This vehicle's real arrivalPos
     */
    virtual SUMOReal getArrivalPos() const = 0;

    /** @brief Sets this vehicle's desired arrivalPos for its current route
     */
    virtual void setArrivalPos(SUMOReal arrivalPos) = 0;

    /** @brief Returns whether this vehicle has departed
     */
    virtual bool hasDeparted() const = 0;

    /** @brief Returns whether this vehicle has arrived
     */
    virtual bool hasArrived() const = 0;

    /** @brief Returns the number of new routes this vehicle got
     * @return the number of new routes this vehicle got
     */
    virtual int getNumberReroutes() const = 0;

    /** @brief Returns this vehicle's devices
     * @return This vehicle's devices
     */
    virtual const std::vector<MSDevice*>& getDevices() const = 0;

    /** @brief Adds a person to this vehicle
     *
     * May do nothing since persons are not supported by default
     *
     * @param[in] person The person to add
     */
    virtual void addPerson(MSTransportable* person) = 0;

    /** @brief Adds a container to this vehicle
     *
     * May do nothing since containers are not supported by default
     *
     * @param[in] container The container to add
     */
    virtual void addContainer(MSTransportable* container) = 0;

    /** @brief Adds a stop
     *
     * The stop is put into the sorted list.
     * @param[in] stop The stop to add
     * @return Whether the stop could be added
     */
    virtual bool addStop(const SUMOVehicleParameter::Stop& stopPar, std::string& errorMsg, SUMOTime untilOffset = 0) = 0;


    /**
    * returns the next imminent stop in the stop queue
    * @return the upcoming stop
    */
    virtual MSParkingArea* getNextParkingArea() = 0;

    /** @brief Replaces a stop
      *
      * The stop replace the next stop into the sorted list.
      * @param[in] stop The stop to add
      * @return Whether the stop could be added
      */
    virtual bool replaceParkingArea(MSParkingArea* parkingArea, std::string& errorMsg) = 0;

    /** @brief Returns whether the vehicle is at a stop
     * @return Whether the has stopped
     */
    virtual bool isStopped() const = 0;


    /** @brief Returns whether the vehicle is at a stop and waiting for a person or container to continue
     */
    virtual bool isStoppedTriggered() const = 0;

    /// @brief Returns a device of the given type if it exists or 0
    virtual MSDevice* getDevice(const std::type_info& type) const = 0;


    virtual SUMOReal getChosenSpeedFactor() const = 0;

    virtual void setChosenSpeedFactor(const SUMOReal factor) = 0;

    virtual SUMOTime getWaitingTime() const = 0;

    virtual SUMOTime getDepartDelay() const = 0;

    /// @brief Returns this vehicles impatience
    virtual SUMOReal getImpatience() const = 0;

    /// @name state io
    //@{

    /// Saves the states of a vehicle
    virtual void saveState(OutputDevice& out) = 0;

    /** @brief Loads the state of this vehicle from the given description
     */
    virtual void loadState(const SUMOSAXAttributes& attrs, const SUMOTime offset) = 0;
    //@}
};


#endif

/****************************************************************************/
