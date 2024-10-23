/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    SUMOTrafficObject.h
/// @author  Jakob Erdmann
/// @date    Mon, 25 Mar 2019
///
// Abstract base class for vehicle, person, and container representations
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <typeinfo>
#include <memory>
#include <utils/common/SUMOTime.h>
#include <utils/common/Named.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class SUMOVehicleParameter;
class SUMOVTypeParameter;
class MSEdge;
class MSLane;
class Position;
class MSDevice;
class MSRoute;

typedef std::shared_ptr<const MSRoute> ConstMSRoutePtr;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOTrafficObject
 * @brief Representation of a vehicle, person, or container
 */
class SUMOTrafficObject : public Named {
public:
    typedef long long int NumericalID;

    /// @brief Constructor
    SUMOTrafficObject(const std::string& id) : Named(id) {}

    /// @brief Destructor
    virtual ~SUMOTrafficObject() {}

    /** @brief Whether it is a vehicle
     * @return true for vehicles, false otherwise
     */
    virtual bool isVehicle() const {
        return false;
    }

    /** @brief Whether it is a person
     * @return true for persons, false otherwise
     */
    virtual bool isPerson() const {
        return false;
    }

    /** @brief Whether it is a container
     * @return true for containers, false otherwise
     */
    virtual bool isContainer() const {
        return false;
    }

    /// @brief return the numerical ID which is only for internal usage
    //  (especially fast comparison in maps which need vehicles as keys)
    virtual NumericalID getNumericalID() const = 0;


    /** @brief Returns the object's "vehicle" type
     * @return The vehicle's type
     */
    virtual const MSVehicleType& getVehicleType() const = 0;

    /** @brief Returns the object's "vehicle" type parameter
     * @return The type parameter
     */
    virtual const SUMOVTypeParameter& getVTypeParameter() const = 0;

    /** @brief Replaces the current vehicle type by the one given
    *
    * If the currently used vehicle type is marked as being used by this object
    *  only, it is deleted, first. The new, given type is then assigned to
    *  "myVType".
    * @param[in] type The new vehicle type
    * @see MSTransportable::myVType
    */
    virtual void replaceVehicleType(MSVehicleType* type) = 0;


    /** @brief Returns the vehicle's parameter (including departure definition)
     *
     * @return The vehicle's parameter
     */
    virtual const SUMOVehicleParameter& getParameter() const = 0;

    /** @brief Returns the associated RNG for this object
    * @return The vehicle's associated RNG
    */
    virtual SumoRNG* getRNG() const = 0;

    /// @brief @return The index of the associated RNG
    virtual int getRNGIndex() const = 0;

    /** @brief Returns whether the object is at a stop
     * @return Whether it has stopped
     */
    virtual bool isStopped() const = 0;

    /** @brief Returns the edge the object is currently at
     *
     * @return The current edge in the object's route
     */
    virtual const MSEdge* getEdge() const = 0;

    /// @brief returns the next edge (possibly an internal edge)
    virtual const MSEdge* getNextEdgePtr() const = 0;

    /// @brief returns the numerical IDs of edges to be used (possibly of future stages)
    virtual const std::set<NumericalID> getUpcomingEdgeIDs() const = 0;

    /** @brief Returns the lane the object is currently at
     *
     * @return The current lane or nullptr if the object is not on a lane
     */
    virtual const MSLane* getLane() const = 0;

    /** @brief Returns the lane the where the rear of the object is currently at
     *
     * @return The current back lane or nullptr if the object is not on a lane
     */
    virtual const MSLane* getBackLane() const = 0;


    /// @brief return index of edge within route
    virtual int getRoutePosition() const = 0;

    /** @brief Returns the end point for reroutes (usually the last edge of the route)
     *
     * @return The rerouting end point
     */
    virtual const MSEdge* getRerouteDestination() const = 0;

    /// Replaces the current route by the given one
    virtual bool replaceRoute(ConstMSRoutePtr route, const std::string& info, bool onInit = false, int offset = 0, bool addStops = true, bool removeStops = true, std::string* msgReturn = nullptr) = 0;

    /** @brief Returns the slope of the road at object's position in degrees
     * @return The slope
     */
    virtual double getSlope() const = 0;

    virtual double getChosenSpeedFactor() const = 0;

    /** @brief Returns the object's access class
     * @return The object's access class
     */
    virtual SUMOVehicleClass getVClass() const = 0;

    /** @brief Returns whether this object is ignoring transient permission
     * changes (during routing)
     */
    virtual bool ignoreTransientPermissions() const {
        return false;
    };

    virtual int getRoutingMode() const = 0;

    /** @brief Returns the object's maximum speed (minimum of technical and desired maximum speed)
     * @return The object's maximum speed
     */
    virtual double getMaxSpeed() const = 0;

    virtual SUMOTime getWaitingTime(const bool accumulated = false) const = 0;

    /** @brief Returns the object's current speed
     * @return The object's speed
     */
    virtual double getSpeed() const = 0;

    // This definition was introduced to make the MSVehicle's previousSpeed Refs. #2579
    /** @brief Returns the object's previous speed
     * @return The object's previous speed
     */
    virtual double getPreviousSpeed() const = 0;


    /** @brief Returns the object's acceleration
     * @return The acceleration
     */
    virtual double getAcceleration() const = 0;

    /** @brief Get the object's position along the lane
     * @return The position of the object (in m from the lane's begin)
     */
    virtual double getPositionOnLane() const = 0;

    /** @brief Get the object's back position along the given lane
     * @return The position of the object (in m from the given lane's begin)
     */
    virtual double getBackPositionOnLane(const MSLane* lane) const = 0;


    /** @brief Return current position (x/y, cartesian)
     *
     * If the object is not in the net, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    virtual Position getPosition(const double offset = 0) const = 0;

    /** @brief Returns the object's angle in degrees
     */
    virtual double getAngle() const = 0;

    /** @brief Returns whether this object has arrived
     */
    virtual bool hasArrived() const = 0;

    /// @brief whether the vehicle is individually influenced (via TraCI or special parameters)
    virtual bool hasInfluencer() const = 0;

    /// @brief whether this object is selected in the GUI
    virtual bool isSelected() const = 0;

    /// @brief Returns a device of the given type if it exists or nullptr if not
    virtual MSDevice* getDevice(const std::type_info& type) const = 0;

    /// @name Helper methods for parsing parameters from the object itself, it's type or the global OptionsCont
    /// @{
    /** @brief Retrieve a string parameter for the traffic object.
     * @param paramName the parameter name
     * @param required whether it is an error if the parameter is not set
     * @param deflt the default value to take if the parameter is not set (the default in the OptionsCont takes precedence)
     * @return the string value
     */
    std::string getStringParam(const std::string& paramName, const bool required = false, const std::string& deflt = "") const;

    /** @brief Retrieve a floating point parameter for the traffic object.
     * @param paramName the parameter name
     * @param required whether it is an error if the parameter is not set
     * @param deflt the default value to take if the parameter is not set (the default in the OptionsCont takes precedence)
     * @return the float value
     */
    double getFloatParam(const std::string& paramName, const bool required = false, const double deflt = INVALID_DOUBLE) const;

    /** @brief Retrieve a boolean parameter for the traffic object.
     * @param paramName the parameter name
     * @param required whether it is an error if the parameter is not set
     * @param deflt the default value to take if the parameter is not set (the default in the OptionsCont takes precedence)
     * @return the bool value
     */
    bool getBoolParam(const std::string& paramName, const bool required = false, const bool deflt = false) const;

    /** @brief Retrieve a time parameter for the traffic object.
     * @param paramName the parameter name
     * @param required whether it is an error if the parameter is not set
     * @param deflt the default value to take if the parameter is not set (the default in the OptionsCont takes precedence)
     * @return the time value
     */
    SUMOTime getTimeParam(const std::string& paramName, const bool required = false, const SUMOTime deflt = SUMOTime_MIN) const;
    /// @}
};
