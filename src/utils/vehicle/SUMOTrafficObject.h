/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOTrafficObject.h
/// @author  Jakob Erdmann
/// @date    Mon, 25 Mar 2019
/// @version $Id$
///
// Abstract base class for vehicle and person representations
/****************************************************************************/
#ifndef SUMOTrafficObject_h
#define SUMOTrafficObject_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <typeinfo>
#include <utils/common/SUMOTime.h>
#include <utils/common/Named.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class MSEdge;
class MSLane;
class Position;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOTrafficObject
 * @brief Representation of a vehicle or person
 */
class SUMOTrafficObject {
public:
    /// @brief Destructor
    virtual ~SUMOTrafficObject() {}

    /** @brief Get the vehicle's ID
     * @return The the ID of the vehicle
     */
    virtual bool isVehicle() const = 0;

    /** @brief Get the vehicle's ID
     * @return The the ID of the vehicle
     */
    virtual const std::string& getID() const = 0;

    /** @brief Returns the vehicle's type
     * @return The vehicle's type
     */
    virtual const MSVehicleType& getVehicleType() const = 0;

    /** @brief Returns whether the vehicle is at a stop
     * @return Whether the has stopped
     */
    virtual bool isStopped() const = 0;

    /** @brief Returns the edge the vehicle is currently at
     *
     * @return The current edge in the vehicle's route
     */
    virtual const MSEdge* getEdge() const = 0;

    /** @brief Returns the slope of the road at vehicle's position
     * @return The slope
     */
    virtual double getSlope() const = 0;

    virtual double getChosenSpeedFactor() const = 0;

    /** @brief Returns the vehicle's access class
     * @return The vehicle's access class
     */
    virtual SUMOVehicleClass getVClass() const = 0;

    /** @brief Returns the vehicle's maximum speed
     * @return The vehicle's maximum speed
     */
    virtual double getMaxSpeed() const = 0;

    virtual SUMOTime getWaitingTime() const = 0;

    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    virtual double getSpeed() const = 0;

    // This definition was introduced to make the MSVehicle's previousSpeed Refs. #2579
    /** @brief Returns the vehicle's previous speed
     * @return The vehicle's previous speed
     */
    virtual double getPreviousSpeed() const = 0;


    /** @brief Returns the vehicle's acceleration
     * @return The acceleration
     */
    virtual double getAcceleration() const = 0;

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual double getPositionOnLane() const = 0;

    /** @brief Get the vehicle's back position along the given lane
     * @return The position of the vehicle (in m from the given lane's begin)
     */
    virtual double getBackPositionOnLane(const MSLane* lane) const = 0;


    /** @brief Return current position (x/y, cartesian)
     *
     * If the vehicle is not in the net, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    virtual Position getPosition(const double offset = 0) const = 0;

    /** @brief Returns the objects angle in degrees
     */
    virtual double getAngle() const = 0;

    /** @brief Returns whether this vehicle has arrived
     */
    virtual bool hasArrived() const = 0;

};


#endif

/****************************************************************************/
