/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSStage.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <cassert>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/vehicle/SUMOTrafficObject.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class MSNet;
class MSStoppingPlace;
class MSVehicleType;
class OutputDevice;
class SUMOVehicleParameter;
class SUMOVehicle;
class MSTransportableDevice;
class MSTransportable;
class MSTransportableStateAdapter;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
enum class MSStageType {
    WAITING_FOR_DEPART = 0,
    WAITING = 1,
    WALKING = 2, // only for persons
    DRIVING = 3,
    ACCESS = 4,
    TRIP = 5,
    TRANSHIP = 6
};

/**
* The "abstract" class for a single stage of a movement
* Contains the destination of the current movement step
*/
class MSStage {
public:
    /// constructor
    MSStage(const MSEdge* destination, MSStoppingPlace* toStop, const double arrivalPos, MSStageType type, const std::string& group = "");

    /// destructor
    virtual ~MSStage();

    /// returns the destination edge
    const MSEdge* getDestination() const;

    /// returns the destination stop (if any)
    MSStoppingPlace* getDestinationStop() const {
        return myDestinationStop;
    }

    /// returns the origin stop (if any). only needed for MSStageTrip
    virtual MSStoppingPlace* getOriginStop() const {
        return nullptr;
    }

    virtual double getArrivalPos() const {
        return myArrivalPos;
    }

    void setArrivalPos(double arrivalPos) {
        myArrivalPos = arrivalPos;
    }

    /// Returns the current edge
    virtual const MSEdge* getEdge() const;
    virtual const MSEdge* getFromEdge() const;
    virtual double getEdgePos(SUMOTime now) const;

    /// @brief Return the movement directon on the edge
    virtual int getDirection() const;

    /// returns the position of the transportable
    virtual Position getPosition(SUMOTime now) const = 0;

    /// returns the angle of the transportable
    virtual double getAngle(SUMOTime now) const = 0;

    /// Returns the current lane (if applicable)
    virtual const MSLane* getLane() const {
        return nullptr;
    }

    ///
    MSStageType getStageType() const {
        return myType;
    }

    /// @brief return the id of the group of transportables traveling together
    const std::string& getGroup() const {
        return myGroup;
    }

    /// @brief return (brief) string representation of the current stage
    virtual std::string getStageDescription(const bool isPerson) const = 0;

    /// @brief return string summary of the current stage
    virtual std::string getStageSummary(const bool isPerson) const = 0;

    /// proceeds to this stage
    virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* previous) = 0;

    /// abort this stage (TraCI)
    virtual void abort(MSTransportable*) {};

    /// sets the walking speed (ignored in other stages)
    virtual void setSpeed(double) {};

    /// get departure time of stage
    SUMOTime getDeparted() const;

    /// get arrival time of stage
    SUMOTime getArrived() const;

    /// logs end of the step
    void setDeparted(SUMOTime now);

    /// logs end of the step
    virtual const std::string setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now, const bool vehicleArrived);

    /// Whether the transportable waits for the given vehicle
    virtual bool isWaitingFor(const SUMOVehicle* vehicle) const;

    /// @brief Whether the transportable waits for a vehicle
    virtual bool isWaiting4Vehicle() const {
        return false;
    }

    /// @brief Current vehicle in which the transportable is driving (or nullptr)
    virtual SUMOVehicle* getVehicle() const {
        return nullptr;
    }

    /// @brief the time this transportable spent waiting
    virtual SUMOTime getWaitingTime(SUMOTime now) const;

    /// @brief the speed of the transportable
    virtual double getSpeed() const;

    /// @brief the edges of the current stage
    virtual ConstMSEdgeVector getEdges() const;

    /// @brief return index of current edge within route
    virtual int getRoutePosition() const {
        return 0;
    }

    /// @brief get position on edge e at length at with orthogonal offset
    Position getEdgePosition(const MSEdge* e, double at, double offset) const;

    /// @brief get position on lane at length at with orthogonal offset
    Position getLanePosition(const MSLane* lane, double at, double offset) const;

    /// @brief get angle of the edge at a certain position
    double getEdgeAngle(const MSEdge* e, double at) const;

    void setDestination(const MSEdge* newDestination, MSStoppingPlace* newDestStop);

    /// @brief get travel distance in this stage
    virtual double getDistance() const = 0;

    /** @brief Called on writing tripinfo output
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const = 0;

    /** @brief Called on writing vehroute output
     * @param[in] isPerson Whether we are writing person or container info
     * @param[in] os The stream to write the information into
     * @param[in] withRouteLength whether route length shall be written
     * @param[in] previous The previous stage for additional info such as from edge
     * @exception IOError not yet implemented
     */
    virtual void routeOutput(const bool isPerson, OutputDevice& os, const bool withRouteLength, const MSStage* const previous) const = 0;

    virtual MSStage* clone() const = 0;

    /** @brief Saves the current state into the given stream, standard implementation does nothing
     */
    virtual void saveState(std::ostringstream& out) {
        UNUSED_PARAMETER(out);
    }

    /** @brief Reconstructs the current state, standard implementation does nothing
     */
    virtual void loadState(MSTransportable* transportable, std::istringstream& state) {
        UNUSED_PARAMETER(transportable);
        UNUSED_PARAMETER(state);
    }

    bool wasSet(int what) const {
        return (myParametersSet & what) != 0;
    }

    void markSet(int what) {
        myParametersSet |= what;
    }


protected:
    /// the next edge to reach by getting transported
    const MSEdge* myDestination;

    /// the stop to reach by getting transported (if any)
    MSStoppingPlace* myDestinationStop;

    /// the position at which we want to arrive
    double myArrivalPos;

    /// the time at which this stage started
    SUMOTime myDeparted;

    /// the time at which this stage ended
    SUMOTime myArrived;

    /// The type of this stage
    MSStageType myType;

    /// The id of the group of transportables traveling together
    const std::string myGroup;

    /// @brief Information on which parameter were set (mainly for vehroute output)
    int myParametersSet;

    /// @brief the offset for computing positions when standing at an edge
    static const double ROADSIDE_OFFSET;

private:
    /// @brief Invalidated copy constructor.
    MSStage(const MSStage&);

    /// @brief Invalidated assignment operator.
    MSStage& operator=(const MSStage&) = delete;

};
