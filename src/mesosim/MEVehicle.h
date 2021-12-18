/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    MEVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
///
// A vehicle from the mesoscopic point of view
/****************************************************************************/
#pragma once
#include <config.h>

#include <iostream>
#include <cassert>
#include <map>
#include <vector>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSEdge.h>
#include <utils/common/StdDefs.h>
#include "MESegment.h"

class MSLane;
class MSLink;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MEVehicle
 * @brief A vehicle from the mesoscopic point of view
 */
class MEVehicle : public MSBaseVehicle {
public:
    /** @brief Constructor
     * @param[in] pars The vehicle description
     * @param[in] route The vehicle's route
     * @param[in] type The vehicle's type
     * @param[in] speedFactor The factor for driven lane's speed limits
     * @exception ProcessError If a value is wrong
     */
    MEVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
              MSVehicleType* type, const double speedFactor);


    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    double getPositionOnLane() const;


    /** @brief Get the vehicle's position relative to the given lane
     *  @return The back position of the vehicle (in m from the given lane's begin)
     */
    double getBackPositionOnLane(const MSLane* lane) const;


    /** @brief Returns the vehicle's direction in degrees
     * @return The vehicle's current angle
     */
    double getAngle() const;


    /** @brief Returns the slope of the road at vehicle's position in degrees
     * @return The slope
     */
    double getSlope() const;

    /** @brief Returns the lane the vehicle is on
    * @return The vehicle's current lane
    */
    const MSLane* getLane() const {
        return nullptr;
    }

    /** @brief Return current position (x/y, cartesian)
     *
     * If the vehicle's myLane is 0, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    Position getPosition(const double offset = 0) const;


    /** @brief Returns the vehicle's estimated speed assuming no delays
     * @return The vehicle's estimated speed
     * @note This is only an upper bound. The speed will be lower if the preceeding vehicle is delayed
     */
    double getSpeed() const;

    /** @brief Returns the vehicle's estimated average speed on the segment assuming no further delays
     * @return The vehicle's estimated average speed
     * @note This is only an upper bound. The speed will be lower if the preceeding vehicle is delayed
     */
    double getAverageSpeed() const;

    /// @brief Returns the vehicle's estimated speed after driving accross the link
    double estimateLeaveSpeed(const MSLink* link) const;


    /** @brief Returns the vehicle's estimated speed taking into account delays
     * @return The vehicle's estimated speed
     * @param[in, out] earliestArrival A lower bound on leaveTime, modified to contain new lower bound on leave Time
     * @note This is only an upper bound. The speed may be even lower if there are further delays downstream
     */
    double getConservativeSpeed(SUMOTime& earliestArrival) const;

    /// @name insertion/removal
    //@{

    /** @brief Called when the vehicle is removed from the network.
     *
     * Moves along work reminders and
     *  informs all devices about quitting. Calls "leaveLane" then.
     *
     * @param[in] reason why the vehicle leaves (reached its destination, parking, teleport)
     */
    void onRemovalFromNet(const MSMoveReminder::Notification reason);
    //@}


    /** @brief Update when the vehicle enters a new edge in the move step.
     * @return Whether the vehicle's route has ended (due to vaporization, or because the destination was reached)
     */
    bool moveRoutePointer();

    /** @brief Returns whether this vehicle has already arived
     * (reached the arrivalPosition on its final edge)
     */
    bool hasArrived() const;

    /** @brief Returns the information whether the vehicle is on a road (is simulated)
     * @return Whether the vehicle is simulated
     */
    bool isOnRoad() const;

    /** @brief Returns whether the vehicle is trying to re-enter the net
     * @return true if the vehicle is trying to enter the net (eg after parking)
     */
    virtual bool isIdling() const;


    /** @brief registers vehicle with the given link
     *
     * @param[in] link the link on which the car shall register its approach
     */
    void setApproaching(MSLink* link);

    /// @brief Returns the remaining stop duration for a stopped vehicle or 0
    SUMOTime remainingStopDuration() const {
        return 0;
    }

    ///@brief ends the current stop and performs loading/unloading
    void processStop();

    /** @brief Returns whether the vehicle stops at the given stopping place */
    bool stopsAt(MSStoppingPlace* /*stop*/) const {
        return false;
    }

    bool stopsAtEdge(const MSEdge* /*edge*/) const {
        return false;
    }

    /** @brief Returns until when to stop at the current segment and sets the information that the stop has been reached
     * @param[in] time the current time
     * @return stop time for the segment
     */
    SUMOTime checkStop(SUMOTime time);

    /**
    * resumes a vehicle from stopping
    * @return true on success, the resuming fails if the vehicle wasn't parking in the first place
    */
    bool resumeFromStopping();

    /// @brief get distance for coming to a stop (used for rerouting checks)
    double getBrakeGap() const {
        return 0;
    }

    /** @brief replace the current parking area stop with a new stop with merge duration
     */
    bool replaceParkingArea(MSParkingArea* /* parkingArea = 0 */, std::string& /*errorMsg*/) {
        throw ProcessError("parkingZoneReroute not implemented for meso");
    }

    /** @brief get the current parking area stop
     */
    MSParkingArea* getNextParkingArea() {
        throw ProcessError("parkingZoneReroute not implemented for meso");
    }

    /** @brief Sets the (planned) time at which the vehicle leaves his current cell
     * @param[in] t The leaving time
     */
    inline void setEventTime(SUMOTime t, bool hasDelay = true) {
        assert(t > myLastEntryTime);
        if (hasDelay && mySegment != 0) {
            mySegment->getEdge().markDelayed();
        }
        myEventTime = t;
    }


    /** @brief Returns the (planned) time at which the vehicle leaves his current cell
     * @return The time the vehicle thinks he leaves his cell at
     */
    inline SUMOTime getEventTime() const {
        return myEventTime;
    }


    /** @brief Sets the current segment the vehicle is at together with its que
     * @param[in] s The current segment
     * @param[in] q The current que
     */
    inline virtual void setSegment(MESegment* s, int idx = 0) {
        mySegment = s;
        myQueIndex = idx;
    }


    /** @brief Returns the current segment the vehicle is on
     * @return The segment the vehicle is on
     */
    inline MESegment* getSegment() const {
        return mySegment;
    }


    /** @brief Returns the index of the que the vehicle is in
     * @return The que index
     */
    inline int getQueIndex() const {
        return myQueIndex;
    }


    /** @brief Sets the entry time for the current segment
     * @param[in] t The entry time
     */
    inline void setLastEntryTime(SUMOTime t) {
        myLastEntryTime = t;
    }


    /** @brief Returns the time the vehicle entered the current segment
     * @return The entry time
     */
    SUMOTime getLastEntryTime() const {
        return myLastEntryTime;
    }


    /** @brief Sets the time at which the vehicle was blocked
     * @param[in] t The blocking time
     */
    inline void setBlockTime(const SUMOTime t) {
        assert(t > myLastEntryTime);
        myBlockTime = t;
    }


    /** @brief Returns the time at which the vehicle was blocked
     * @return The blocking time
     */
    inline SUMOTime getBlockTime() const {
        return myBlockTime;
    }


    /// @brief Returns the duration for which the vehicle was blocked
    inline SUMOTime getWaitingTime() const {
        return MAX2(SUMOTime(0), myEventTime - myBlockTime);
    }

    inline SUMOTime getTimeLoss() const {
        // slow-downs while driving are not modelled
        return getWaitingTime();
    }

    /// @brief Returns the duration for which the vehicle was blocked
    inline SUMOTime getAccumulatedWaitingTime() const {
        return getWaitingTime();
    }


    /// @brief Returns the earliest leave time for the current segment
    double getEventTimeSeconds() const {
        return STEPS2TIME(getEventTime());
    }

    /// @brief Returns the entry time for the current segment
    double getLastEntryTimeSeconds() const {
        return STEPS2TIME(getLastEntryTime());
    }

    /// @brief Returns the time at which the vehicle was blocked on the current segment
    double getBlockTimeSeconds() const {
        return STEPS2TIME(getBlockTime());
    }

    /// @brief Returns the delay that is accrued due to option --meso-tls-penalty or --meso-minor-penalty
    double getCurrentLinkPenaltySeconds() const;

    /// @brief Returns the delay that is accrued due to option --meso-tls-penalty or --meso-minor-penalty
    double getCurrentStoppingTimeSeconds() const;

    /// Replaces the current route by the given one
    bool replaceRoute(const MSRoute* route,  const std::string& info, bool onInit = false, int offset = 0, bool addRouteStops = true, bool removeStops = true, std::string* msgReturn = nullptr);

    /** @brief Returns whether the vehicle is allowed to pass the next junction, checks also for triggered stops
     * @return true iff the vehicle may drive over the next junction
     */
    bool mayProceed();

    /** @brief Updates a single vehicle detector if present
     */
    void updateDetectorForWriting(MSMoveReminder* rem, SUMOTime currentTime, SUMOTime exitTime);

    /** @brief Updates all vehicle detectors
     */
    void updateDetectors(SUMOTime currentTime, const bool isLeave,
                         const MSMoveReminder::Notification reason = MSMoveReminder::NOTIFICATION_JUNCTION);

    /** @brief Returns the velocity/lane influencer
     *
     * If no influencer was existing before, one is built, first
     * @return Reference to this vehicle's speed influencer
     */
    BaseInfluencer& getBaseInfluencer();

    const BaseInfluencer* getBaseInfluencer() const;

    bool hasInfluencer() const {
        return myInfluencer != nullptr;
    }

    /// @name state io
    //@{

    /// Saves the states of a vehicle
    void saveState(OutputDevice& out);

    /** @brief Loads the state of this vehicle from the given description
     */
    void loadState(const SUMOSAXAttributes& attrs, const SUMOTime offset);
    //@}


protected:
    /// @brief The segment the vehicle is at
    MESegment* mySegment;

    /// @brief Index of the que the vehicle is in (important for multiqueue extension)
    int myQueIndex;

    /// @brief The (planned) time of leaving the segment (cell)
    SUMOTime myEventTime;

    /// @brief The time the vehicle entered its current segment
    SUMOTime myLastEntryTime;

    /// @brief The time at which the vehicle was blocked on its current segment
    SUMOTime myBlockTime;

    /// @brief An instance of a velocity/lane influencing instance; built in "getInfluencer"
    BaseInfluencer* myInfluencer;

};
