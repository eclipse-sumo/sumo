/****************************************************************************/
/// @file    MSLink.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A connnection between lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLink_h
#define MSLink_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class SUMOVehicle;
class MSVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLinks
 * @brief A connnection between lanes
 *
 * A link is basically a connection between two lanes, stored within the
 *  originating (the one that is being left) lane and pointing to the
 *  approached lane. When using inner-junction simulation, additionally
 *  a "via-lane" is stored, the one that is used to cross the junction
 *  and which represents the links shape.
 *
 * Because right-of-way rules are controlled by the junctions, the link
 *  stores the information about where to write information about approaching
 *  vehicles (the "request") and where to get the information whether the
 *  vehicle really may drive (the "respond").
 *
 * Because a link is a connection over a junction, it basically also has a
 *  length. This length is needed to assure that vehicles have the correct halting
 *  distance before approaching the link. In the case of using internal lanes,
 *  the link's length is 0.
 */
class MSLink {
public:

    typedef std::vector<std::pair<std::pair<MSVehicle*, SUMOReal>, SUMOReal> > LinkLeaders;

    /** @struct ApproachingVehicleInformation
     * @brief A structure holding the information about vehicles approaching a link
     */
    struct ApproachingVehicleInformation {
        /** @brief Constructor
         * @param[in] waitingTime The time during which the vehicle is waiting at this link
         *   this needs to be placed here because MSVehicle::myWaitingTime is updated in between
         *   calls to opened() causing order dependencies
         **/
        ApproachingVehicleInformation(const SUMOTime _arrivalTime, const SUMOTime _leavingTime,
                                      const SUMOReal _arrivalSpeed, const SUMOReal _leaveSpeed,
                                      const bool _willPass,
                                      const SUMOTime _arrivalTimeBraking,
                                      const SUMOReal _arrivalSpeedBraking,
                                      const SUMOTime _waitingTime
                                     ) :
            arrivalTime(_arrivalTime), leavingTime(_leavingTime),
            arrivalSpeed(_arrivalSpeed), leaveSpeed(_leaveSpeed),
            willPass(_willPass),
            arrivalTimeBraking(_arrivalTimeBraking),
            arrivalSpeedBraking(_arrivalSpeedBraking),
            waitingTime(_waitingTime) {}

        /// @brief The time the vehicle's front arrives at the link
        const SUMOTime arrivalTime;
        /// @brief The estimated time at which the vehicle leaves the link
        const SUMOTime leavingTime;
        /// @brief The estimated speed with which the vehicle arrives at the link (for headway computation)
        const SUMOReal arrivalSpeed;
        /// @brief The estimated speed with which the vehicle leaves the link (for headway computation)
        const SUMOReal leaveSpeed;
        /// @brief Whether the vehicle wants to pass the link (@todo: check semantics)
        const bool willPass;
        /// @brief The time the vehicle's front arrives at the link if it starts braking
        const SUMOTime arrivalTimeBraking;
        /// @brief The estimated speed with which the vehicle arrives at the link if it starts braking(for headway computation)
        const SUMOReal arrivalSpeedBraking;
        /// @brief The waiting duration at the current link
        const SUMOTime waitingTime;

    private:
        /// invalidated assignment operator
        ApproachingVehicleInformation& operator=(const ApproachingVehicleInformation& s);

    };


#ifndef HAVE_INTERNAL_LANES
    /** @brief Constructor for simulation not using internal lanes
     *
     * @param[in] succLane The lane approached by this link
     * @param[in] dir The direction of this link
     * @param[in] state The state of this link
     * @param[in] length The length of this link
     */
    MSLink(MSLane* succLane, LinkDirection dir, LinkState state, SUMOReal length);
#else
    /** @brief Constructor for simulation which uses internal lanes
     *
     * @param[in] succLane The lane approached by this link
     * @param[in] via The lane to use within the junction
     * @param[in] dir The direction of this link
     * @param[in] state The state of this link
     * @param[in] length The length of this link
     */
    MSLink(MSLane* succLane, MSLane* via, LinkDirection dir, LinkState state, SUMOReal length);
#endif


    /// @brief Destructor
    ~MSLink();


    /** @brief Sets the request information
     *
     * Because traffic lights and junction logics are loaded after links,
     *  we have to assign the information about the right-of-way
     *  requests and responses after the initialisation.
     * @todo Unsecure!
     */
    void setRequestInformation(unsigned int index, bool isCrossing, bool isCont,
                               const std::vector<MSLink*>& foeLinks, const std::vector<MSLane*>& foeLanes,
                               MSLane* internalLaneBefore = 0);


    /** @brief Sets the information about an approaching vehicle
     *
     * The information is stored in myApproachingVehicles.
     */
    void setApproaching(const SUMOVehicle* approaching, const SUMOTime arrivalTime,
                        const SUMOReal arrivalSpeed, const SUMOReal leaveSpeed, const bool setRequest,
                        const SUMOTime arrivalTimeBraking, const SUMOReal arrivalSpeedBraking,
                        const SUMOTime waitingTime);

    /// @brief removes the vehicle from myApproachingVehicles
    void removeApproaching(const SUMOVehicle* veh);

    void addBlockedLink(MSLink* link);

    /* @brief return information about this vehicle if it is registered as
     * approaching (dummy values otherwise)
     * @note used for visualisation of link items */
    ApproachingVehicleInformation getApproaching(const SUMOVehicle* veh) const;

    /** @brief Returns the information whether the link may be passed
     *
     * Valid after the junctions have set their reponds
     *
     * @param[in] collectFoes If a vector is passed, all blocking foes are collected and inserted into this vector
     * @return Whether this link may be passed.
     */
    bool opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength,
                SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime,
                std::vector<const SUMOVehicle*>* collectFoes = 0) const;

    /** @brief Returns the information whether this link is blocked
     * Valid after the vehicles have set their requests
     * @param[in] arrivalTime The arrivalTime of the vehicle who checks for an approaching foe
     * @param[in] leaveTime The leaveTime of the vehicle who checks for an approaching foe
     * @param[in] arrivalSpeed The speed with which the checking vehicle plans to arrive at the link
     * @param[in] leaveSpeed The speed with which the checking vehicle plans to leave the link
     * @param[in] sameTargetLane Whether the link that calls this method has the same target lane as this link
     * @param[in] impatience The impatience of the checking vehicle
     * @param[in] decel The maximum deceleration of the checking vehicle
     * @param[in] waitingTime The waiting time of the checking vehicle
     * @param[in] collectFoes If a vector is passed the return value is always False, instead all blocking foes are collected and inserted into this vector
     * @return Whether this link is blocked
     * @note Since this needs to be called without a SUMOVehicle (TraCI), we cannot simply pass the checking vehicle itself
     **/
    bool blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                       bool sameTargetLane, SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime,
                       std::vector<const SUMOVehicle*>* collectFoes = 0) const;


    bool isBlockingAnyone() const {
        return myApproachingVehicles.size() != 0;
    }

    bool willHaveBlockedFoe() const;



    /** @brief Returns the information whether a vehicle is approaching on one of the link's foe streams
     *
     * Valid after the vehicles have set their requests
     * @param[in] arrivalTime The arrivalTime of the vehicle who checks for an approaching foe
     * @param[in] leaveTime The leaveTime of the vehicle who checks for an approaching foe
     * @param[in] speed The speed with which the checking vehicle plans to leave the link
     * @param[in] decel The maximum deceleration of the checking vehicle
     * @return Whether a foe of this link is approaching
     */
    bool hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal speed,
                           SUMOReal decel = DEFAULT_VEH_DECEL) const;


    /** @brief Returns the current state of the link
     *
     * @return The current state of this link
     */
    LinkState getState() const {
        return myState;
    }


    /** @brief Returns the direction the vehicle passing this link take
     *
     * @return The direction of this link
     */
    LinkDirection getDirection() const;


    /** @brief Sets the current tl-state
     *
     * @param[in] state The current state of the link
     */
    void setTLState(LinkState state, SUMOTime t);


    /** @brief Returns the connected lane
     *
     * @return The lane approached by this link
     */
    MSLane* getLane() const;


    /** @brief Returns the respond index (for visualization)
     *
     * @return The respond index for this link
     */
    inline unsigned int getIndex() const {
        return myIndex;
    }


    /** @brief Returns whether this link is a major link
     * @return Whether the link has a large priority
     */
    bool havePriority() const {
        return myState >= 'A' && myState <= 'Z';
    }


    /** @brief Returns the length of this link
     *
     * @return The length of this link
     */
    SUMOReal getLength() const {
        return myLength;
    }

    /** @brief Returns whether this link belongs to a junction where more than one edge is incoming
     *
     * @return Whether any foe links exist
     */
    bool isCrossing() const {
        return myIsCrossing;
    }


    bool isCont() const {
        return myAmCont;
    }


    /// @brief whether this is a link past an internal junction which currently has priority
    bool lastWasContMajor() const;


#ifdef HAVE_INTERNAL_LANES
    /** @brief Returns the following inner lane
     *
     * @return The inner lane to use to cross the junction
     */
    MSLane* getViaLane() const;


    /** @brief Returns all potential link leaders (vehicles on foeLanes)
     * Valid during the planMove() phase
     * @param[in] dist The distance of the vehicle who is asking about the leader to this link
     * @param[in] dist The minGap of the vehicle who is asking about the leader to this link
     * @return The all vehicles on foeLanes and their (virtual) distances to the asking vehicle
     */
    LinkLeaders getLeaderInfo(SUMOReal dist, SUMOReal minGap) const;
#endif

    /// @brief return the via lane if it exists and the lane otherwise
    MSLane* getViaLaneOrLane() const;


    /// @brief return the expected time at which the given vehicle will clear the link
    SUMOTime getLeaveTime(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength) const;

    /// @brief write information about all approaching vehicles to the given output device
    void writeApproaching(OutputDevice& od, const std::string fromLaneID) const;


private:
    /// @brief return whether the given vehicles may NOT merge safely
    static inline bool unsafeMergeSpeeds(SUMOReal leaderSpeed, SUMOReal followerSpeed, SUMOReal leaderDecel, SUMOReal followerDecel) {
        // XXX mismatch between continuous an discrete deceleration
        return (leaderSpeed * leaderSpeed / leaderDecel) <= (followerSpeed * followerSpeed / followerDecel);
    }

    /// @brief returns whether the given lane may still be occupied by a vehicle currently on it
    static bool maybeOccupied(MSLane* lane);

private:
    /// @brief The lane approached by this link
    MSLane* myLane;

    std::map<const SUMOVehicle*, ApproachingVehicleInformation> myApproachingVehicles;
    std::set<MSLink*> myBlockedFoeLinks;

    /// @brief The position within this respond
    unsigned int myIndex;

    /// @brief The state of the link
    LinkState myState;

    /// @brief An abstract (hopefully human readable) definition of the link's direction
    LinkDirection myDirection;

    /// @brief The length of the link
    SUMOReal myLength;

    /// @brief Whether any foe links exist
    bool myIsCrossing;

    bool myAmCont;

#ifdef HAVE_INTERNAL_LANES
    /// @brief The following junction-internal lane if used
    MSLane* const myJunctionInlane;

    /* @brief lengths after the crossing point with foeLane
     * (lengthOnThis, lengthOnFoe)
     * (index corresponds to myFoeLanes)
     * empty vector for entry links
     * */
    std::vector<std::pair<SUMOReal, SUMOReal> > myLengthsBehindCrossing;
#endif

    std::vector<MSLink*> myFoeLinks;
    std::vector<MSLane*> myFoeLanes;
    static SUMOTime myLookaheadTime;


private:
    /// invalidated copy constructor
    MSLink(const MSLink& s);

    /// invalidated assignment operator
    MSLink& operator=(const MSLink& s);

};


#endif

/****************************************************************************/

