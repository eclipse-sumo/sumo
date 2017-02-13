/****************************************************************************/
/// @file    MEVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// A vehicle from the mesoscopic point of view
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MEVehicle_h
#define MEVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
              const MSVehicleType* type, const SUMOReal speedFactor);


    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    SUMOReal getPositionOnLane() const;


    /** @brief Get the vehicle's position relative to the given lane
     *  @return The back position of the vehicle (in m from the given lane's begin)
     */
    SUMOReal getBackPositionOnLane(const MSLane* lane) const;


    /** @brief Returns the vehicle's direction in degrees
     * @return The vehicle's current angle
     */
    SUMOReal getAngle() const;


    /** @brief Returns the slope of the road at vehicle's position
     * @return The slope
     */
    SUMOReal getSlope() const;

    /** @brief Returns the lane the vehicle is on
    * @return The vehicle's current lane
    */
    MSLane* getLane() const {
        return 0;
    }

    /** @brief Return current position (x/y, cartesian)
     *
     * If the vehicle's myLane is 0, Position::INVALID.
     * @param[in] offset optional offset in longitudinal direction
     * @return The current position (in cartesian coordinates)
     * @see myLane
     */
    Position getPosition(const SUMOReal offset = 0) const;


    /** @brief Returns the vehicle's estimated speed assuming no delays
     * @return The vehicle's estimated speed
     * @note This is only an upper bound. The speed will be lower if the preceeding vehicle is delayed
     */
    SUMOReal getSpeed() const;

    /** @brief Returns the vehicle's estimated average speed on the segment assuming no further delays
     * @return The vehicle's estimated average speed
     * @note This is only an upper bound. The speed will be lower if the preceeding vehicle is delayed
     */
    SUMOReal getAverageSpeed() const;

    /// @brief Returns the vehicle's estimated speed after driving accross the link
    SUMOReal estimateLeaveSpeed(const MSLink* link) const;


    /** @brief Returns the vehicle's estimated speed taking into account delays
     * @return The vehicle's estimated speed
     * @param[in, out] earliestArrival A lower bound on leaveTime, modified to contain new lower bound on leave Time
     * @note This is only an upper bound. The speed may be even lower if there are further delays downstream
     */
    SUMOReal getConservativeSpeed(SUMOTime& earliestArrival) const;


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

    /** @brief Returns whether the vehicle is parking
     * @return whether the vehicle is parking
     */
    bool isParking() const;

    /** @brief Adds a stop
     *
     * The stop is put into the sorted list.
     * @param[in] stop The stop to add
     * @return Whether the stop could be added
     */
    bool addStop(const SUMOVehicleParameter::Stop& stopPar, std::string& errorMsg, SUMOTime untilOffset = 0);


    /** @brief Returns whether the vehicle is at a stop
     * @return Whether it has stopped
     */
    bool isStopped() const;


    /** @brief Returns whether the vehicle is on a triggered stop
     * @return whether the vehicle is on a triggered stop
     */
    bool isStoppedTriggered() const;

    /** @brief Returns how long to stop at the given segment
     * @param[in] seg The segment in question
     * @return stop time for the segment
     */
    SUMOTime getStoptime(const MESegment* const seg) const;


    /** @brief Returns the list of still pending stop edges
     */
    const ConstMSEdgeVector getStopEdges() const;

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


    /** @brief Returns the number of seconds waited (speed was lesser than 0.1m/s)
     *
     * The value is reset if the vehicle moves faster than 0.1m/s
     * Intentional stopping does not count towards this time.
     * @return The time the vehicle is standing
     */
    SUMOReal getWaitingSeconds() const {
        return STEPS2TIME(getWaitingTime());
    }


    /// @brief Returns the earliest leave time for the current segment
    SUMOReal getEventTimeSeconds() const {
        return STEPS2TIME(getEventTime());
    }

    /// @brief Returns the entry time for the current segment
    SUMOReal getLastEntryTimeSeconds() const {
        return STEPS2TIME(getLastEntryTime());
    }

    /// @brief Returns the time at which the vehicle was blocked on the current segment
    SUMOReal getBlockTimeSeconds() const {
        return STEPS2TIME(getBlockTime());
    }

    /// @brief Returns the delay that is accrued due to option --meso-tls-penalty or --meso-minor-penalty
    SUMOReal getCurrentLinkPenaltySeconds() const;


    /// Replaces the current route by the given one
    bool replaceRoute(const MSRoute* route, bool onInit = false, int offset = 0, bool addStops = true);

    /** @brief Returns whether the vehicle is allowed to pass the next junction
     * @return true iff the vehicle may drive over the next junction
     */
    bool mayProceed() const;

    /** @brief Updates a single vehicle detector if present
     */
    void updateDetectorForWriting(MSMoveReminder* rem, SUMOTime currentTime, SUMOTime exitTime);

    /** @brief Updates all vehicle detectors
     */
    void updateDetectors(SUMOTime currentTime, const bool isLeave,
                         const MSMoveReminder::Notification reason = MSMoveReminder::NOTIFICATION_JUNCTION);

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

    /// @brief where to stop
    std::map<const MESegment* const, SUMOTime> myStops;

};

#endif

/****************************************************************************/
