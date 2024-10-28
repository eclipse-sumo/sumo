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
/// @file    MSStageDriving.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
///
// A stage performing the travelling by a transport system (cars, public transport)
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <cassert>
#include <utils/common/Command.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/vehicle/SUMOTrafficObject.h>
#include "MSStage.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class MSNet;
class MSStop;
class MSStoppingPlace;
class MSVehicleType;
class OutputDevice;
class SUMOVehicleParameter;
class SUMOVehicle;
class MSTransportableDevice;
class MSTransportable;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* A "real" stage performing the travelling by a transport system
* The given route will be chosen. The travel time is computed by the simulation
*/
class MSStageDriving : public MSStage {
public:
    /// constructor
    MSStageDriving(const MSEdge* origin, const MSEdge* destination, MSStoppingPlace* toStop,
                   const double arrivalPos, const double arrivalPosLat, const std::vector<std::string>& lines,
                   const std::string& group = "",
                   const std::string& intendedVeh = "", SUMOTime intendedDepart = -1);

    /// destructor
    virtual ~MSStageDriving();

    MSStage* clone() const;

    /// @brief return default value for undefined arrivalPos
    double getArrivalPos() const;

    bool unspecifiedArrivalPos() const;

    /// abort this stage (TraCI)
    void abort(MSTransportable* t);

    /// initialization, e.g. for param-related events
    void init(MSTransportable* transportable);

    /// Returns the current edge
    const MSEdge* getEdge() const;
    const MSEdge* getFromEdge() const;
    double getEdgePos(SUMOTime now) const;

    /// @brief Return the movement directon on the edge
    int getDirection() const;

    const MSLane* getLane() const;

    MSStoppingPlace* getOriginStop() const {
        return myOriginStop;
    }

    ///
    Position getPosition(SUMOTime now) const;

    double getAngle(SUMOTime now) const;

    /// @brief get travel distance in this stage
    double getDistance() const;

    /// @brief return (brief) string representation of the current stage
    std::string getStageDescription(const bool isPerson) const;

    /// @brief return string summary of the current stage
    std::string getStageSummary(const bool isPerson) const;

    /// proceeds to this stage
    void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* previous);

    /** @brief Called on writing tripinfo output
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

    /** @brief Called on writing vehroute output
     * @param[in] isPerson Whether we are writing person or container info
     * @param[in] os The stream to write the information into
     * @param[in] withRouteLength whether route length shall be written
     * @param[in] previous The previous stage for additional info such as from edge
     * @exception IOError not yet implemented
     */
    void routeOutput(const bool isPerson, OutputDevice& os, const bool withRouteLength, const MSStage* const previous) const;

    /// Whether the person waits for the given vehicle
    bool isWaitingFor(const SUMOVehicle* vehicle) const;

    /// @brief Whether the person waits for a vehicle
    bool isWaiting4Vehicle() const;

    /// @brief Return where the person waits and for what
    std::string getWaitingDescription() const;

    SUMOVehicle* getVehicle() const {
        return myVehicle;
    }

    /// @brief time spent waiting for a ride
    SUMOTime getWaitingTime(SUMOTime now) const;

    double getSpeed() const;

    ConstMSEdgeVector getEdges() const;

    void setVehicle(SUMOVehicle* v);

    /// @brief marks arrival time and records driven distance
    const std::string setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now, const bool vehicleArrived);

    const std::set<std::string>& getLines() const {
        return myLines;
    }

    std::string getIntendedVehicleID() const {
        return myIntendedVehicleID;
    }

    SUMOTime getIntendedDepart() const {
        return myIntendedDepart;
    }

    std::string getVehicleType() const {
        return myVehicleType;
    }

    /// change origin for parking area rerouting
    void setOrigin(const MSEdge* origin) {
        myOrigin = origin;
    }

    /// @brief checks whether the person may exit at the current vehicle position
    bool canLeaveVehicle(const MSTransportable* t, const SUMOVehicle& veh, const MSStop& stop);

    SUMOTime getTimeLoss(const MSTransportable* transportable) const;
    SUMOTime getDuration() const;
    SUMOTime getTravelTime() const;
    SUMOTime getWaitingTime() const;

    /** @brief Saves the current state into the given stream
     */
    void saveState(std::ostringstream& out);

    /** @brief Reconstructs the current state
     */
    void loadState(MSTransportable* transportable, std::istringstream& state);

    bool equals(const MSStage& s) const {
        if (!MSStage::equals(s)) {
            return false;
        }
        // this is safe because MSStage already checked that the type fits
        const MSStageDriving& sd = static_cast<const MSStageDriving&>(s);
        return myOrigin == sd.myOrigin &&
               myLines == sd.myLines &&
               myIntendedVehicleID == sd.myIntendedVehicleID;
    }

protected:
    /// the origin edge
    const MSEdge* myOrigin;

    /// the lines  to choose from
    const std::set<std::string> myLines;

    /// @brief The taken vehicle
    SUMOVehicle* myVehicle;
    /// @brief cached vehicle data for output after the vehicle has been removed
    std::string myVehicleID;
    std::string myVehicleLine;
    std::string myVehicleType;

    SUMOVehicleClass myVehicleVClass;
    double myVehicleDistance;
    /// @brief While driving, this is the timeLoss of the vehicle when the ride started, after arrival this is the timeLoss which the vehicle accumulated during the ride
    SUMOTime myTimeLoss;

    double myWaitingPos;
    /// @brief The time since which this person is waiting for a ride
    SUMOTime myWaitingSince;
    const MSEdge* myWaitingEdge;
    Position myStopWaitPos;
    /// @brief the stop at which this ride starts (or nullptr)
    MSStoppingPlace* myOriginStop;

    std::string myIntendedVehicleID;
    SUMOTime myIntendedDepart;


private:
    /// brief register waiting person (on proceed or loadState)
    void registerWaiting(MSTransportable* transportable, SUMOTime now);

private:
    /// @brief Invalidated copy constructor.
    MSStageDriving(const MSStageDriving&);

    /// @brief Invalidated assignment operator.
    MSStageDriving& operator=(const MSStageDriving&) = delete;

private:
    class BookReservation : public Command {
    public:
        BookReservation(MSTransportable* transportable, SUMOTime earliestPickupTime, MSStageDriving* stage) :
            myTransportable(transportable), myEarliestPickupTime(earliestPickupTime), myStage(stage), myWaitingPos(stage->myWaitingPos) {}
        SUMOTime execute(SUMOTime currentTime);

    public:
        MSTransportable* myTransportable;
        SUMOTime myEarliestPickupTime;
        MSStageDriving* myStage;
        double myWaitingPos;
    };

protected:
    BookReservation* myReservationCommand;

};
