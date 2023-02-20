/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSStop.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 13.12.2005
///
// A lane area vehicles can halt at
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MSRoute.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MESegment;
class MSStoppingPlace;
class MSParkingArea;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/** @class Stop
 * @brief Definition of vehicle stop (position and duration)
 */
class MSStop {
public:
    MSStop(const SUMOVehicleParameter::Stop& par) : pars(par) {}
    /// @brief The edge in the route to stop at
    MSRouteIterator edge;
    /// @brief The lane to stop at (microsim only)
    const MSLane* lane = nullptr;
    /// @brief The segment to stop at (mesosim only)
    const MESegment* segment = nullptr;
    /// @brief (Optional) bus stop if one is assigned to the stop
    MSStoppingPlace* busstop = nullptr;
    /// @brief (Optional) container stop if one is assigned to the stop
    MSStoppingPlace* containerstop = nullptr;
    /// @brief (Optional) parkingArea if one is assigned to the stop
    MSParkingArea* parkingarea = nullptr;
    /// @brief (Optional) charging station if one is assigned to the stop
    MSStoppingPlace* chargingStation = nullptr;
    /// @brief (Optional) overhead wire segment if one is assigned to the stop
    /// @todo Check that this should really be a stopping place instance
    MSStoppingPlace* overheadWireSegment = nullptr;
    /// @brief The stop parameter
    const SUMOVehicleParameter::Stop pars;
    /// @brief The stopping duration
    SUMOTime duration = -1;
    /// @brief whether an arriving person lets the vehicle continue
    bool triggered = false;
    /// @brief whether an arriving container lets the vehicle continue
    bool containerTriggered = false;
    /// @brief whether coupling another vehicle (train) the vehicle continue
    bool joinTriggered = false;
    /// @brief Information whether the stop has been reached
    bool reached = false;
    /// @brief The number of still expected persons
    int numExpectedPerson = 0;
    /// @brief The number of still expected containers
    int numExpectedContainer = 0;
    /// @brief The time at which the vehicle is able to board another person
    SUMOTime timeToBoardNextPerson = 0;
    /// @brief The time at which the vehicle is able to load another container
    SUMOTime timeToLoadNextContainer = 0;
    /// @brief the maximum time at which persons may board this vehicle
    SUMOTime endBoarding = SUMOTime_MAX;
    /// @brief whether this an opposite-direction stop
    bool isOpposite = false;
    /// @brief whether the decision to skip this stop has been made
    bool skipOnDemand = false;
    /// @brief whether the 'started' value was loaded from simulaton state
    bool startedFromState = false;

    /// @brief Write the current stop configuration (used for state saving)
    void write(OutputDevice& dev) const;

    /// @brief return halting position for upcoming stop;
    double getEndPos(const SUMOVehicle& veh) const;

    /// @brief return startPos taking into account opposite stopping
    double getReachedThreshold() const;

    /// @brief get a short description for showing in the gui
    std::string getDescription() const;

    /// @brief initialize attributes from the given stop parameters
    void initPars(const SUMOVehicleParameter::Stop& stopPar);

    const MSEdge* getEdge() const;

    /// @brief return flags as used by Vehicle::getStopState
    int getStateFlagsOld() const;

    /// @brief return minimum stop duration when starting stop at time
    SUMOTime getMinDuration(SUMOTime time) const;

    /// @brief return until / ended time
    SUMOTime getUntil() const;

    /// @brief return speed for passing waypoint / skipping on-demand stop
    double getSpeed() const;

    /// @brief whether the stop is in range of the given position
    bool isInRange(const double pos, const double tolerance) const;
};
