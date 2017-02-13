/****************************************************************************/
/// @file    MESegment.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// A single mesoscopic segment (cell)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MESegment_h
#define MESegment_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/Named.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLink;
class MSMoveReminder;
class MSVehicleControl;
class MEVehicle;
class BinaryInputDevice;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MESegment
 * @brief A single mesoscopic segment (cell)
 */
class MESegment : public Named {
public:
    /** @brief constructor
     * @param[in] id The id of this segment (currently: "<EDGEID>:<SEGMENTNO>")
     * @param[in] parent The edge this segment is located within
     * @param[in] next The following segment (belonging to the same edge)
     * @param[in] length The segment's length
     * @param[in] speed The speed allowed on this segment
     * @param[in] idx The running index of this segment within the segment's edge
     * @param[in] tauff The factor for free-free headway time
     * @param[in] taufj The factor for free-jam headway time
     * @param[in] taujf The factor for jam-free headway time
     * @param[in] taujj The factor for jam-jam headway time
     * @param[in] jamThresh percentage of occupied space before the segment is jammed
     * @param[in] multiQueue whether to install multiple queues on this segment
     * @param[in] junctionControl whether junction control is enabled on this segment
     * @param[in] the quotient of geometrical length / given length
     * @todo recheck the id; using a ':' as divider is not really nice
     */
    MESegment(const std::string& id,
              const MSEdge& parent, MESegment* next,
              SUMOReal length, SUMOReal speed,
              int idx,
              SUMOTime tauff, SUMOTime taufj,
              SUMOTime taujf, SUMOTime taujj,
              SUMOReal jamThresh,
              bool multiQueue, bool junctionControl);


    typedef std::vector<MEVehicle*> Queue;
    typedef std::vector<Queue> Queues;
    /// @name Measure collection
    /// @{

    /** @brief Adds a data collector for a detector to this segment
     *
     * @param[in] data The data collector to add
     */
    void addDetector(MSMoveReminder* data);

    /** @brief Removes a data collector for a detector from this segment
     *
     * @param[in] data The data collector to remove
     */
    void removeDetector(MSMoveReminder* data);

    /** @brief Updates data of a detector for all vehicle queues
     *
     * @param[in] data The detector data to update
     */
    void prepareDetectorForWriting(MSMoveReminder& data);
    /// @}

    /** @brief Returns whether the given vehicle would still fit into the segment
     *
     * @param[in] veh The vehicle to check space for
     * @param[in] entryTime The time at which the vehicle wants to enter
     * @param[in] init whether the check is done at insertion time
     * @return true if the vehicle may be added to this segment, false otherwise
     */
    bool hasSpaceFor(const MEVehicle* veh, SUMOTime entryTime, bool init = false) const;

    /** @brief Inserts (emits) vehicle into the segment
     *
     * @param[in] veh The vehicle to emit
     * @param[in] time The emission time
     * @return Whether the emission was successful
     */
    bool initialise(MEVehicle* veh, SUMOTime time);

    /** @brief Returns the total number of cars on the segment
     *
     * @return the total number of cars on the segment
     */
    int getCarNumber() const;

    /// @brief return the number of queues
    inline int numQueues() const {
        return (int)myCarQues.size();
    }
    /** @brief Returns the cars in the queue with the given index for visualization
     * @return the Queue (XXX not thread-safe!)
     */
    inline const Queue& getQueue(int index) const {
        assert(index < (int)myCarQues.size());
        return myCarQues[index];
    }

    /** @brief Returns the running index of the segment in the edge (0 is the most upstream).
     *
     * @return the running index of the segment in the edge
     */
    inline int getIndex() const {
        return myIndex;
    }

    /** @brief Returns the following segment on the same edge (0 if it is the last).
     *
     * @return the following segment on the same edge (0 if it is the last)
     */
    inline MESegment* getNextSegment() const {
        return myNextSegment;
    }

    /** @brief Returns the length of the segment in meters.
     *
     * @return the length of the segment
     */
    inline SUMOReal getLength() const {
        return myLength;
    }

    /** @brief Returns the occupany of the segment (the sum of the vehicle lengths + minGaps)
     *
     * @return the occupany of the segment in meters
     */
    inline SUMOReal getBruttoOccupancy() const {
        return myOccupancy;
    }


    /** @brief Returns the relative occupany of the segment (percentage of road used))
     * @return the occupany of the segment in percent
     */
    inline SUMOReal getRelativeOccupancy() const {
        return myOccupancy / myCapacity;
    }

    /** @brief Returns the relative occupany of the segment (percentage of road used))
     * at which the segment is considered jammed
     * @return the jam treshold of the segment in percent
     */
    inline SUMOReal getRelativeJamThreshold() const {
        return myJamThreshold / myCapacity;
    }

    /** @brief Returns the average speed of vehicles on the segment in meters per second.
     * If there is no vehicle on the segment it returns the maximum allowed speed
     * @param[in] useCache whether to use a cached value if available
     * @note this value is cached in myMeanSpeed. Since caching only takes place
     * once every simstep there is a potential for side-influences (i.e. GUI calls to
     * this method, ...) For that reason the simulation logic doesn't use the cache.
     * This shouldn't matter much for speed since it is only used during
     * initializsation of vehicles onto the segment.
     * @return the average speed on the segment
     */
    SUMOReal getMeanSpeed(bool useCache) const;

    /// @brief wrapper to satisfy the FunctionBinding signature
    inline SUMOReal getMeanSpeed() const {
        return getMeanSpeed(true);
    }


    void writeVehicles(OutputDevice& of) const;

    /** @brief Removes the given car from the edge's que
     *
     * @param[in] v The vehicle to remove
     * @param[in] leaveTime The time at which the vehicle is leaving the que
     * @param[in] next The next segment for this vehicle
     * @return The next first vehicle to add to the net's que
     */
    MEVehicle* removeCar(MEVehicle* v, SUMOTime leaveTime, MESegment* next);

    /** @brief Returns the link the given car will use when passing the next junction
     *
     * This returns non-zero values only for the last segment and only
     *  if junction control is enabled.
     *
     * @param[in] veh The vehicle in question
     * @param[in] tlsPenalty Whether the link should be returned for computing tlsPenalty
     * @return The link to use or 0 without junction control
     */
    MSLink* getLink(const MEVehicle* veh, bool tlsPenalty = false) const;

    /** @brief Returns whether the vehicle may use the next link
     *
     * In case of disabled junction control it returns always true.
     *
     * @param[in] veh The vehicle in question
     * @return Whether it may pass to the next segment
     */
    bool isOpen(const MEVehicle* veh) const;

    /** @brief Removes the vehicle from the segment, adapting its parameters
     *
     * @param[in] veh The vehicle in question
     * @param[in] next The subsequent segment for delay calculation
     * @param[in] time the leave time
     * @todo Isn't always time == veh->getEventTime?
     */
    void send(MEVehicle* veh, MESegment* next, SUMOTime time);

    /** @brief Adds the vehicle to the segment, adapting its parameters
     *
     * @param[in] veh The vehicle in question
     * @param[in] time the leave time
     * @param[in] isDepart whether the vehicle just departed
     * @todo Isn't always time == veh->getEventTime?
     */
    void receive(MEVehicle* veh, SUMOTime time, bool isDepart = false, bool afterTeleport = false);


    /** @brief tries to remove any car from this segment
     *
     * @param[in] currentTime the current time
     * @return Whether vaporization was successful
     * @note: cars removed via this method do NOT count as arrivals */
    bool vaporizeAnyCar(SUMOTime currentTime);

    /** @brief Returns the edge this segment belongs to
     * @return the edge this segment belongs to
     */
    inline const MSEdge& getEdge() const {
        return myEdge;
    }


    /** @brief reset mySpeed and patch the speed of
     * all vehicles in it. Also set/recompute myJamThreshold
     * @param[in] jamThresh follows the semantic of option meso-jam-threshold
     */
    void setSpeed(SUMOReal newSpeed, SUMOTime currentTime, SUMOReal jamThresh = DO_NOT_PATCH_JAM_THRESHOLD);

    /** @brief Returns the (planned) time at which the next vehicle leaves this segment
     * @return The time the vehicle thinks it leaves
     */
    SUMOTime getEventTime() const;

    /// @brief Like getEventTime but returns seconds (for visualization)
    inline SUMOReal getEventTimeSeconds() const {
        return STEPS2TIME(getEventTime());
    }

    /// @brief get the last headway time in seconds
    inline SUMOReal getLastHeadwaySeconds() const {
        return STEPS2TIME(myLastHeadway);
    }

    /// @name State saving/loading
    /// @{

    /** @brief Saves the state of this segment into the given stream
     *
     * Some internal values which must be restored are saved as well as ids of
     *  the vehicles stored in internal queues and the last departures of connected
     *  edges.
     *
     * @param[in, filled] out The (possibly binary) device to write the state into
     * @todo What about throwing an IOError?
     */
    void saveState(OutputDevice& out);

    /** @brief Loads the state of this segment with the given parameters
     *
     * This method is called for every internal que the segment has.
     *  Every vehicle is retrieved from the given MSVehicleControl and added to this
     *  segment. Then, the internal queues that store vehicles dependant to their next
     *  edge are filled the same way. Then, the departure of last vehicles onto the next
     *  edge are restored.
     *
     * @param[in] vehIDs The vehicle ids for the current que
     * @param[in] vc The vehicle control to retrieve references vehicles from
     * @param[in] blockTime The time the last vehicle left the que
     * @param[in] queIdx The index of the current que
     * @todo What about throwing an IOError?
     * @todo What about throwing an error if something else fails (a vehicle can not be referenced)?
     */
    void loadState(std::vector<std::string>& vehIDs, MSVehicleControl& vc, const SUMOTime blockTime, const int queIdx);
    /// @}


    /** @brief returns all vehicles (for debugging)
     */
    std::vector<const MEVehicle*> getVehicles() const;


    /** @brief returns flow based on headway
     * @note: returns magic number 10000 when headway cannot be computed
     */
    SUMOReal getFlow() const;


    /// @brief whether the given segment is 0 or encodes vaporization
    static inline bool isInvalid(const MESegment* segment) {
        return segment == 0 || segment == &myVaporizationTarget;
    }

    /// @brief return a time after earliestEntry at which a vehicle may be inserted at full speed
    SUMOTime getNextInsertionTime(SUMOTime earliestEntry) const;

    /** @brief return whether this segment is considered free as opposed to jammed
     */
    inline bool free() const {
        return myOccupancy <= myJamThreshold;
    }

    /// @brief return the remaining physical space on this segment
    inline int remainingVehicleCapacity(const SUMOReal vehLength) const {
        if (myOccupancy == 0. && myCapacity < vehLength) {
            // even small segments can hold at least one vehicle
            return 1;
        }
        return (int)((myCapacity - myOccupancy) / vehLength);
    }

    /// @brief return the next time at which a vehicle my enter this segment
    inline SUMOTime getEntryBlockTime() const {
        return myEntryBlockTime;
    }

    /// @brief set the next time at which a vehicle my enter this segment
    inline void setEntryBlockTime(SUMOTime entryBlockTime) {
        myEntryBlockTime = entryBlockTime;
    }

    /// @brief return the minimum headway-time with which vehicles may enter or leave this segment
    inline SUMOTime getMinimumHeadwayTime() const {
        return myTau_ff;
    }

    static const SUMOReal DO_NOT_PATCH_JAM_THRESHOLD;

    /// @brief add this lanes MoveReminders to the given vehicle
    void addReminders(MEVehicle* veh) const;

    /** @brief Returns the penalty time for passing a link (if using gMesoTLSPenalty > 0 or gMesoMinorPenalty > 0)
     * @param[in] veh The vehicle in question
     * @return The time penalty
     */
    SUMOTime getLinkPenalty(const MEVehicle* veh) const;

    /** @brief Returns the average green time as fraction of cycle time
     * @param[in] veh The vehicle in question for determining the link
     * @return The green fraction or 1 if the vehicle does not continue after this edge
     */
    SUMOReal getTLSCapacity(const MEVehicle* veh) const;

private:
    /** @brief Updates data of all detectors for a leaving vehicle
     *
     * @param[in] v The vehicle to update values for
     * @param[in] currentTime The leave time of the vehicle
     * @param[in] next The next segment on this vehicles route
     */
    void updateDetectorsOnLeave(MEVehicle* v, SUMOTime currentTime, MESegment* next);

    bool overtake();

    SUMOTime getTimeHeadway(const MESegment* pred, const MEVehicle* veh);

    void setSpeedForQueue(SUMOReal newSpeed, SUMOTime currentTime,
                          SUMOTime blockTime, const std::vector<MEVehicle*>& vehs);

    /** @brief compute the new arrival time when switching speed
     */
    SUMOTime newArrival(const MEVehicle* const v, SUMOReal newSpeed, SUMOTime currentTime);

    /// @brief whether a leader in any queue is blocked
    bool hasBlockedLeader() const;

    /** @brief compute a value for myJamThreshold
     * if jamThresh is negative, compute a value which allows free flow at mySpeed
     * interpret jamThresh as the relative occupation at which jam starts
     */
    void recomputeJamThreshold(SUMOReal jamThresh);

    /// @brief compute jam threshold for the given speed and jam-threshold option
    SUMOReal jamThresholdForSpeed(SUMOReal speed, SUMOReal jamThresh) const;

    /// @brief whether the given link may be passed because the option meso-junction-control.limited is set
    bool limitedControlOverride(const MSLink* link) const;

    /// @brief return the maximum tls penalty for all links from this edge
    SUMOReal getMaxPenaltySeconds() const;

    /// @brief whether the segment requires use of multiple queues
    static bool useMultiQueue(bool multiQueue, const MSEdge& parent);

    /// @brief convert net time gap (leader back to follower front) to gross time gap (leader front to follower front)
    inline SUMOTime tauWithVehLength(SUMOTime tau, SUMOReal lengthWithGap) const {
        return tau + (SUMOTime)(lengthWithGap / myTau_length);
    }

private:
    /// @brief The microsim edge this segment belongs to
    const MSEdge& myEdge;

    /// @brief The next segment of this edge, 0 if this is the last segment of this edge
    MESegment* myNextSegment;

    /// @brief The segment's length
    const SUMOReal myLength;

    /// @brief Running number of the segment in the edge
    const int myIndex;

    /// @brief The time headway parameters, see the Eissfeldt thesis
    const SUMOTime myTau_ff, myTau_fj, myTau_jf, myTau_jj;
    /// @brief Headway parameter for computing gross time headyway from net time headway, length and edge speed
    SUMOReal myTau_length;

    /// @brief slope and axis offset for the jam-jam headway function
    SUMOReal myA, myB;

    /// @brief The capacity of the segment in number of cars, used only in time headway calculation
    /// This parameter has only an effect if tau_jf != tau_jj, which is not(!) the case per default
    const SUMOReal myHeadwayCapacity;

    /// @brief The number of lanes * the length
    const SUMOReal myCapacity;

    /// @brief The occupied space (in m) on the segment
    SUMOReal myOccupancy;

    /// @brief Whether junction control is enabled
    const bool myJunctionControl;

    /// @brief Whether tls penalty is enabled
    const bool myTLSPenalty;

    /// @brief Whether minor penalty is enabled
    const bool myMinorPenalty;

    /// @brief The space (in m) which needs to be occupied before the segment is considered jammed
    SUMOReal myJamThreshold;

    /// @brief The data collection for all kinds of detectors
    std::vector<MSMoveReminder*> myDetectorData;

    /// @brief The car queues. Vehicles are inserted in the front and removed in the back
    Queues myCarQues;

    /// @brief The follower edge to que index mapping for multi queue segments
    std::map<const MSEdge*, std::vector<int> > myFollowerMap;

    /// @brief The block times
    std::vector<SUMOTime> myBlockTimes;

    /* @brief The block time for vehicles who wish to enter this segment.
     * @note since we do not know which queue will be used there is only one
     * value for all queues */
    SUMOTime myEntryBlockTime;

    /// @brief the last headway
    SUMOTime myLastHeadway;

    /* @brief segment for signifying vaporization. This segment has invalid
     * data and should only be used as a unique pointer */
    static MSEdge myDummyParent;
    static MESegment myVaporizationTarget;

    /// @brief the mean speed on this segment. Updated at event time or on demand
    mutable SUMOReal myMeanSpeed;

    /// @brief the time at which myMeanSpeed was last updated
    mutable SUMOTime myLastMeanSpeedUpdate;

private:
    /// @brief Invalidated copy constructor.
    MESegment(const MESegment&);

    /// @brief Invalidated assignment operator.
    MESegment& operator=(const MESegment&);

    /// @brief constructor for dummy segment
    MESegment(const std::string& id);
};


#endif

/****************************************************************************/
