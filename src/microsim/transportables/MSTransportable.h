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
/// @file    MSTransportable.h
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
#include <utils/common/WrappingCommand.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/vehicle/SUMOTrafficObject.h>
#include <microsim/MSRouterDefs.h>
#include <microsim/MSVehicleType.h>
#include "MSStage.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class MSNet;
class MSStoppingPlace;
class OutputDevice;
class SUMOVehicleParameter;
class SUMOVehicle;
class MSTransportableDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSTransportable
  *
  * The class holds a simulated moveable object
  */
class MSTransportable : public SUMOTrafficObject {
public:
    /// @name inherited from SUMOTrafficObject
    /// @{
    inline bool isPerson() const {
        return myAmPerson;
    }

    inline bool isContainer() const {
        return !myAmPerson;
    }

    inline std::string getObjectType() {
        return myAmPerson ? "Person" : "Container";
    }

    inline NumericalID getNumericalID() const {
        return myNumericalID;
    }

    inline bool isStopped() const {
        return getCurrentStageType() == MSStageType::WAITING;
    }

    double getSlope() const;

    SUMOVehicleClass getVClass() const;

    /// @brief whether the transportable (persons) is jammed as defined by the current pedestrian model
    virtual bool isJammed() const {
        return false;
    }

    /** @brief Returns the maximum speed (the minimum of desired and physical maximum speed)
     * @return The objects's maximum speed
     */
    double getMaxSpeed() const;

    SUMOTime getWaitingTime(const bool accumulated = false) const;

    double getPreviousSpeed() const {
        return getSpeed();
    }

    double getAcceleration() const {
        return 0.0;
    }

    double getPositionOnLane() const {
        return getEdgePos();
    }

    double getBackPositionOnLane(const MSLane* lane) const;

    Position getPosition(const double /*offset*/) const {
        return getPosition();
    }
    /// @}

    /// the structure holding the plan of a transportable
    typedef std::vector<MSStage*> MSTransportablePlan;

    /// constructor
    MSTransportable(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportablePlan* plan, const bool isPerson);

    /// destructor
    virtual ~MSTransportable();

    /* @brief proceeds to the next step of the route,
     * @return Whether the transportables plan continues  */
    virtual bool proceed(MSNet* net, SUMOTime time, const bool vehicleArrived = false);

    virtual bool checkAccess(const MSStage* const prior, const bool waitAtStop = true) {
        UNUSED_PARAMETER(prior);
        UNUSED_PARAMETER(waitAtStop);
        return false;
    }

    /// @brief set the id (inherited from Named but forbidden for transportables)
    void setID(const std::string& newID);

    inline const SUMOVehicleParameter& getParameter() const {
        return *myParameter;
    }

    inline const MSVehicleType& getVehicleType() const {
        return *myVType;
    }

    /** @brief Returns the object's "vehicle" type parameter
     * @return The object's type parameter
     */
    inline const SUMOVTypeParameter& getVTypeParameter() const {
        return myVType->getParameter();
    }

    /// @brief returns the associated RNG
    SumoRNG* getRNG() const;

    /// @brief returns the index of the associated RNG
    int getRNGIndex() const;

    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const;

    /// logs depart time of the current stage
    void setDeparted(SUMOTime now);

    /// logs depart time of the current stage
    SUMOTime getDeparture() const;

    /// Returns the current destination.
    const MSEdge* getDestination() const {
        return (*myStep)->getDestination();
    }

    /// Returns the destination after the current destination.
    const MSEdge* getNextDestination() const {
        return (*(myStep + 1))->getDestination();
    }

    /// @brief Returns the current edge
    const MSEdge* getEdge() const {
        return (*myStep)->getEdge();
    }

    /// @brief Returns the current lane (may be nullptr)
    const MSLane* getLane() const {
        return (*myStep)->getLane();
    }

    const MSLane* getBackLane() const {
        return getLane();
    }

    /// @brief Returns the departure edge
    const MSEdge* getFromEdge() const {
        return (*myStep)->getFromEdge();
    }

    /// @brief Return the position on the edge
    virtual double getEdgePos() const;

    /// @brief Return the movement directon on the edge
    virtual int getDirection() const;

    /// @brief Return the Network coordinate of the transportable
    virtual Position getPosition() const;

    /// @brief return the current angle of the transportable
    virtual double getAngle() const;

    /// @brief the time this transportable spent waiting in seconds
    virtual double getWaitingSeconds() const;

    /// @brief the current speed of the transportable
    virtual double getSpeed() const;

    /// @brief the current speed factor of the transportable (where applicable)
    virtual double getChosenSpeedFactor() const {
        return 1;
    }

    /// @brief the current stage type of the transportable
    MSStageType getCurrentStageType() const {
        return (*myStep)->getStageType();
    }

    /// @brief the stage type for the nth next stage
    MSStageType getStageType(int next) const {
        assert(myStep + next < myPlan->end());
        assert(myStep + next >= myPlan->begin());
        return (*(myStep + next))->getStageType();
    }

    /// @brief return textual summary for the given stage
    std::string getStageSummary(int stageIndex) const;

    /// Returns the current stage description as a string
    std::string getCurrentStageDescription() const {
        return (*myStep)->getStageDescription(myAmPerson);
    }

    /// @brief Return the current stage
    MSStage* getCurrentStage() const {
        return *myStep;
    }

    /// @brief Return the next (or previous) stage denoted by the offset
    inline MSStage* getNextStage(int offset) const {
        assert(myStep + offset >= myPlan->begin());
        assert(myStep + offset < myPlan->end());
        return *(myStep + offset);
    }

    /// @brief returns the numerical IDs of edges to be used (possibly of future stages)
    const std::set<NumericalID> getUpcomingEdgeIDs() const;

    /// @brief Return the total number stages in this person's plan
    inline int getNumStages() const {
        return (int)myPlan->size();
    }

    /// @brief Return the number of remaining stages (including the current)
    inline int getNumRemainingStages() const {
        return (int)(myPlan->end() - myStep);
    }

    /// @brief Return the index of the current stage
    inline int getCurrentStageIndex() const {
        return (int)(myStep - myPlan->begin());
    }

    /// @brief return the index of the edge within the route
    inline int getRoutePosition() const {
        return (*myStep)->getRoutePosition();
    }

    /// @brief returns the next edge ptr (used by walking persons)
    virtual const MSEdge* getNextEdgePtr() const {
        return nullptr;
    }

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void tripInfoOutput(OutputDevice& os) const;

    /** @brief Called on writing vehroute output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void routeOutput(OutputDevice& os, const bool withRouteLength) const;

    /// Whether the transportable waits for the given vehicle in the current step
    bool isWaitingFor(const SUMOVehicle* vehicle) const {
        return (*myStep)->isWaitingFor(vehicle);
    }

    /// @brief Whether the transportable waits for a vehicle
    bool isWaiting4Vehicle() const {
        return (*myStep)->isWaiting4Vehicle();
    }

    void setAbortWaiting(const SUMOTime timeout);

    /// @brief Abort current stage (used for aborting waiting for a vehicle)
    SUMOTime abortStage(SUMOTime step);

    /// @brief The vehicle associated with this transportable
    SUMOVehicle* getVehicle() const {
        return (*myStep)->getVehicle();
    }

    /// @brief Appends the given stage to the current plan
    void appendStage(MSStage* stage, int next = -1);

    /// @brief removes the nth next stage
    void removeStage(int next, bool stayInSim = true);

    /// @brief set the speed for all present and future (walking) stages and modify the vType so that stages added later are also affected
    void setSpeed(double speed);

    /// @brief returns the final arrival pos
    double getArrivalPos() const {
        return myPlan->back()->getArrivalPos();
    }

    /// @brief returns the final arrival edge
    const MSEdge* getArrivalEdge() const {
        return myPlan->back()->getEdges().back();
    }

    /** @brief Returns the end point for reroutes (usually the last edge of the route)
     *
     * @return The rerouting end point
     */
    const MSEdge* getRerouteDestination() const  {
        return getArrivalEdge();
    }

    bool reroute(SUMOTime t, const std::string& info, MSTransportableRouter& router, const bool onInit = false, const bool withTaz = false, const bool silent = false, const MSEdge* sink = nullptr);

    /// Replaces the current route by the given one
    bool replaceRoute(ConstMSRoutePtr route, const std::string& info, bool onInit = false, int offset = 0, bool addStops = true, bool removeStops = true, std::string* msgReturn = nullptr);

    /** @brief Replaces the current vehicle type by the one given
    *
    * If the currently used vehicle type is marked as being used by this vehicle
    *  only, it is deleted, first. The new, given type is then assigned to
    *  "myVType".
    * @param[in] type The new vehicle type
    * @see MSTransportable::myVType
    */
    void replaceVehicleType(MSVehicleType* type);

    /** @brief Replaces the current vehicle type with a new one used by this vehicle only
    *
    * If the currently used vehicle type is already marked as being used by this vehicle
    *  only, no new type is created.
    * @return The new modifiable vehicle type
    * @see MSTransportable::myVType
    */
    MSVehicleType& getSingularType();

    /// @brief return the bounding box of the person
    PositionVector getBoundingBox() const;

    /// @brief return whether the person has reached the end of its plan
    bool hasArrived() const;

    /// @brief return whether the transportable has started its plan
    bool hasDeparted() const;

    /// @brief adapt plan when the vehicle reroutes and now stops at replacement instead of orig
    void rerouteParkingArea(MSStoppingPlace* orig, MSStoppingPlace* replacement);

    /// @brief Returns a device of the given type if it exists or nullptr if not
    MSDevice* getDevice(const std::type_info& type) const;

    /// @brief set individual junction model paramete (not type related)
    void setJunctionModelParameter(const std::string& key, const std::string& value);

    /** @brief Returns this vehicle's devices
     * @return This vehicle's devices
     */
    inline const std::vector<MSTransportableDevice*>& getDevices() const {
        return myDevices;
    }

    virtual bool hasInfluencer() const {
        return false;
    }

    /// @brief whether this transportable is selected in the GUI
    virtual bool isSelected() const {
        return false;
    }

    /// @brief return routing mode (configures router choice but also handling of transient permission changes)
    virtual int getRoutingMode() const;

    /** @brief Saves the current state into the given stream
     */
    void saveState(OutputDevice& out);

    /** @brief Reconstructs the current state
     */
    void loadState(const std::string& state);

protected:
    /// the plan of the transportable
    const SUMOVehicleParameter* myParameter;

    /// @brief This transportable's type. (mainly used for drawing related information
    /// Note sure if it is really necessary
    MSVehicleType* myVType;

    /// @brief Whether events shall be written
    bool myWriteEvents;

    /// the plan of the transportable
    MSTransportablePlan* myPlan;

    /// the iterator over the route
    MSTransportablePlan::iterator myStep;

    /// @brief The devices this transportable has
    std::vector<MSTransportableDevice*> myDevices;

private:
    const bool myAmPerson;

    const NumericalID myNumericalID;

    WrappingCommand<MSTransportable>* myAbortCommand;

    static NumericalID myCurrentNumericalIndex;

private:
    /// @brief Invalidated copy constructor.
    MSTransportable(const MSTransportable&);

    /// @brief Invalidated assignment operator.
    MSTransportable& operator=(const MSTransportable&);

};
