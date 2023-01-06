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
/// @file    MSStageWaiting.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 1 Jun 2022
///
// An stage for planned waiting (stopping)
/****************************************************************************/
#pragma once
#include <config.h>

#include <microsim/transportables/MSStage.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class MSNet;
class MSStoppingPlace;
class MSVehicleType;
class OutputDevice;
class MSTransportable;


/**
* A "real" stage performing a waiting over the specified time
*/
class MSStageWaiting : public MSStage {
public:
    /// constructor
    MSStageWaiting(const MSEdge* destination, MSStoppingPlace* toStop, SUMOTime duration, SUMOTime until,
                   double pos, const std::string& actType, const bool initial);

    /// destructor
    virtual ~MSStageWaiting();

    MSStage* clone() const;

    /// abort this stage (TraCI)
    void abort(MSTransportable*);

    SUMOTime getUntil() const;

    SUMOTime getDuration() const;

    SUMOTime getStopEnd() const {
        return myStopEndTime;
    }
    ///
    Position getPosition(SUMOTime now) const;

    double getAngle(SUMOTime now) const;

    /// @brief get travel distance in this stage
    double getDistance() const {
        return 0;
    }

    std::string getStageDescription(const bool isPerson) const;

    std::string getStageSummary(const bool isPerson) const;

    /// proceeds to the next step
    void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* previous);

    /** @brief Called on writing tripinfo output
    *
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

    void saveState(std::ostringstream& out);

    void loadState(MSTransportable* transportable, std::istringstream& state);

private:
    /// the time the person is waiting
    SUMOTime myWaitingDuration;

    /// the time until the person is waiting
    SUMOTime myWaitingUntil;

    /// @brief waiting position at stopping place
    Position myStopWaitPos;

    /// @brief The type of activity
    std::string myActType;

    /// @brief stores the actual end time of the stop (combination of duration and until)
    SUMOTime myStopEndTime;

private:
    /// @brief Invalidated copy constructor.
    MSStageWaiting(const MSStageWaiting&);

    /// @brief Invalidated assignment operator.
    MSStageWaiting& operator=(const MSStageWaiting&) = delete;

};
