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
/// @file    MSStageTrip.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 1 Jun 2022
///
// An intermodal routing request (to be transformed into a sequence of walks and rides)
/****************************************************************************/
#pragma once
#include <config.h>

#include <microsim/transportables/MSStage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTransportable;
class MSEdge;
class MSLane;
class MSNet;
class MSStoppingPlace;
class OutputDevice;

/**
* A "placeholder" stage storing routing info which will result in real stages when routed
*/
class MSStageTrip : public MSStage {
public:
    /// constructor
    MSStageTrip(const MSEdge* origin, MSStoppingPlace* fromStop,
                const MSEdge* destination, MSStoppingPlace* toStop,
                const SUMOTime duration, const SVCPermissions modeSet,
                const std::string& vTypes, const double speed, const double walkFactor,
                const std::string& group,
                const double departPosLat, const bool hasArrivalPos, const double arrivalPos);

    /// destructor
    virtual ~MSStageTrip();

    MSStage* clone() const;

    const MSEdge* getEdge() const;

    MSStoppingPlace* getOriginStop() const {
        return myOriginStop;
    }

    double getEdgePos(SUMOTime now) const;

    Position getPosition(SUMOTime now) const;

    double getAngle(SUMOTime now) const;

    double getDistance() const {
        // invalid
        return -1;
    }

    std::string getStageDescription(const bool isPerson) const {
        UNUSED_PARAMETER(isPerson);
        return "trip";
    }

    std::string getStageSummary(const bool isPerson) const;

    /// logs end of the step
    const std::string setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now, const bool vehicleArrived);

    /// change origin for parking area rerouting
    void setOrigin(const MSEdge* origin) {
        myOrigin = origin;
    }

    /// proceeds to the next step
    void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* previous);

    /** @brief Called on writing tripinfo output
    *
    * @param[in] os The stream to write the information into
    * @exception IOError not yet implemented
    */
    void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const {
        UNUSED_PARAMETER(os);
        UNUSED_PARAMETER(transportable);
    }

    /** @brief Called on writing vehroute output
    *
    * @param[in] os The stream to write the information into
    * @exception IOError not yet implemented
    */
    void routeOutput(const bool isPerson, OutputDevice& os, const bool withRouteLength, const MSStage* const previous) const;

private:
    /// the origin edge
    const MSEdge* myOrigin;

    /// the origin edge
    MSStoppingPlace* myOriginStop;

    /// the time the trip should take (applies to only walking)
    SUMOTime myDuration;

    /// @brief The allowed modes of transportation
    const SVCPermissions myModeSet;

    /// @brief The possible vehicles to use
    const std::string myVTypes;

    /// @brief The walking speed
    const double mySpeed;

    /// @brief The factor to apply to walking durations
    const double myWalkFactor;

    /// @brief The depart position
    double myDepartPos;

    /// @brief The lateral depart position
    const double myDepartPosLat;

    /// @brief whether an arrivalPos was in the input
    const bool myHaveArrivalPos;

private:
    /// @brief Invalidated copy constructor.
    MSStageTrip(const MSStageTrip&);

    /// @brief Invalidated assignment operator.
    MSStageTrip& operator=(const MSStageTrip&);

};
