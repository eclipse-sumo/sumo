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
/// @file    MSStageWalking.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
///
// A stage performing walking on a sequence of edges.
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSStageMoving.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;
class MSLane;
class OutputDevice;
class SUMOVehicleParameter;
class MSStoppingPlace;
class SUMOVehicle;
class MSVehicleType;
class MSPModel;
class MSMoveReminder;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * A stage performing walking on a sequence of edges. The real movements are part of the pedestrian model.
 */
class MSStageWalking : public MSStageMoving {
public:
    /// constructor
    MSStageWalking(const std::string& personID, const ConstMSEdgeVector& route, MSStoppingPlace* toStop, SUMOTime walkingTime,
                   double speed, double departPos, double arrivalPos, double departPosLat, int departLane = -1, const std::string& routeID = "");

    /// destructor
    ~MSStageWalking();

    MSStage* clone() const;

    /// proceeds to the next step
    virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, MSStage* previous);

    /// abort this stage (TraCI)
    void abort(MSTransportable*);

    /// sets the walking speed (ignored in other stages)
    void setSpeed(double speed);

    /// @brief get travel distance in this stage
    double getDistance() const {
        return walkDistance();
    }

    /// @brief return index of current edge within route
    int getRoutePosition() const;

    std::string getStageDescription(const bool isPerson) const {
        UNUSED_PARAMETER(isPerson);
        return "walking";
    }

    std::string getStageSummary(const bool isPerson) const;

    /** @brief Saves the current state into the given stream
     */
    void saveState(std::ostringstream& out);

    /** @brief Reconstructs the current state
     */
    void loadState(MSTransportable* transportable, std::istringstream& state);

    /** @brief Called on writing tripinfo output
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

    /** @brief Called on writing vehroute output
     * @param[in] os The stream to write the information into
     * @param[in] withRouteLength whether route length shall be written
     * @exception IOError not yet implemented
     */
    virtual void routeOutput(const bool isPerson, OutputDevice& os, const bool withRouteLength, const MSStage* const previous) const;

    /// @brief move forward and return whether the person arrived
    bool moveToNextEdge(MSTransportable* person, SUMOTime currentTime, int prevDir, MSEdge* nextInternal = nullptr, const bool isReplay = false);

    void activateEntryReminders(MSTransportable* person, const bool isDepart = false);

    void activateLeaveReminders(MSTransportable* person, const MSLane* lane, double lastPos, SUMOTime t, bool arrived);

    /// @brief accessors to be used by MSPModel
    //@{
    double getMaxSpeed(const MSTransportable* const person) const;

    inline double getArrivalPos() const {
        return myArrivalPos;
    }

    inline const MSEdge* getNextRouteEdge() const {
        return myRouteStep == myRoute.end() - 1 ? nullptr : *(myRouteStep + 1);
    }
    //@}

    /// @brief Whether the transportable is walking
    bool isWalk() const {
        return true;
    }

    SUMOTime getTimeLoss(const MSTransportable* transportable) const;

private:
    /// @brief compute total walking distance
    double walkDistance(bool partial = false) const;

    /* @brief compute average speed if the total walking duration is given
        * @note Must be called when the previous stage changes myDepartPos from the default*/
    double computeAverageSpeed() const;

private:
    /// the time the person is walking
    SUMOTime myWalkingTime;

    /// the time the person entered the edge
    SUMOTime myLastEdgeEntryTime;

    /// @brief the MoveReminders encountered while walking
    std::vector<MSMoveReminder*> myMoveReminders;

    /// @brief optional exit time tracking for vehroute output
    std::vector<SUMOTime>* myExitTimes;

    /// @brief distance walked on non-normal edges (i.e. walkingareas)
    double myInternalDistance;

    static bool myWarnedInvalidTripinfo;

private:
    /// @brief Invalidated copy constructor.
    MSStageWalking(const MSStageWalking&);

    /// @brief Invalidated assignment operator.
    MSStageWalking& operator=(const MSStageWalking&);

};
