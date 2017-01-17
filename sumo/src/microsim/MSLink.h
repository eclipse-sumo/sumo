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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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
class MSJunction;
class SUMOVehicle;
class MSVehicle;
class MSPerson;
class OutputDevice;
class MSTrafficLightLogic;


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

    // distance to link in m below which adaptation for zipper-merging should take place
    static const SUMOReal ZIPPER_ADAPT_DIST;

    struct LinkLeader {
        LinkLeader(MSVehicle* _veh, SUMOReal _gap, SUMOReal _distToCrossing) :
            vehAndGap(std::make_pair(_veh, _gap)),
            distToCrossing(_distToCrossing) {}

        std::pair<MSVehicle*, SUMOReal> vehAndGap;
        SUMOReal distToCrossing;
    };

    typedef std::vector<LinkLeader> LinkLeaders;

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
                                      const SUMOTime _waitingTime,
                                      const SUMOReal _dist
                                     ) :
            arrivalTime(_arrivalTime), leavingTime(_leavingTime),
            arrivalSpeed(_arrivalSpeed), leaveSpeed(_leaveSpeed),
            willPass(_willPass),
            arrivalTimeBraking(_arrivalTimeBraking),
            arrivalSpeedBraking(_arrivalSpeedBraking),
            waitingTime(_waitingTime),
            dist(_dist) {
        }

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
        /// @brief The distance up to the current link
        const SUMOReal dist;

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
     * @param[in] keepClear Whether the junction after this link must be kept clear
     */
    MSLink(MSLane* predLane, MSLane* succLane, LinkDirection dir, LinkState state, SUMOReal length, SUMOReal foeVisibilityDistance, bool keepClear, MSTrafficLightLogic* logic, int tlLinkIdx);
#else
    /** @brief Constructor for simulation which uses internal lanes
     *
     * @param[in] succLane The lane approached by this link
     * @param[in] via The lane to use within the junction
     * @param[in] dir The direction of this link
     * @param[in] state The state of this link
     * @param[in] length The length of this link
     */
    MSLink(MSLane* predLane, MSLane* succLane, MSLane* via, LinkDirection dir, LinkState state, SUMOReal length, SUMOReal foeVisibilityDistance, bool keepClear, MSTrafficLightLogic* logic, int tlLinkIdx);
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
    void setRequestInformation(int index, bool hasFoes, bool isCont,
                               const std::vector<MSLink*>& foeLinks, const std::vector<MSLane*>& foeLanes,
                               MSLane* internalLaneBefore = 0);


    /** @brief Sets the information about an approaching vehicle
     *
     * The information is stored in myApproachingVehicles.
     */
    void setApproaching(const SUMOVehicle* approaching, const SUMOTime arrivalTime,
                        const SUMOReal arrivalSpeed, const SUMOReal leaveSpeed, const bool setRequest,
                        const SUMOTime arrivalTimeBraking, const SUMOReal arrivalSpeedBraking,
                        const SUMOTime waitingTime, SUMOReal dist);

    /// @brief removes the vehicle from myApproachingVehicles
    void removeApproaching(const SUMOVehicle* veh);

    void addBlockedLink(MSLink* link);

    /* @brief return information about this vehicle if it is registered as
     * approaching (dummy values otherwise)
     * @note used for visualisation of link items */
    ApproachingVehicleInformation getApproaching(const SUMOVehicle* veh) const;

    /// @brief return all approaching vehicles
    const std::map<const SUMOVehicle*, ApproachingVehicleInformation>& getApproaching() const {
        return myApproachingVehicles;
    }

    /** @brief Returns the information whether the link may be passed
     *
     * Valid after the junctions have set their reponds
     *
     * @param[in] collectFoes If a vector is passed, all blocking foes are collected and inserted into this vector
     * @return Whether this link may be passed.
     */
    bool opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed, SUMOReal vehicleLength,
                SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime,
                SUMOReal posLat = 0,
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
    bool hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal speed, SUMOReal decel) const;

    MSJunction* getJunction() const {
        return myJunction;
    }


    /** @brief Returns the current state of the link
     *
     * @return The current state of this link
     */
    LinkState getState() const {
        return myState;
    }


    //@brief Returns the time of the last state change
    inline SUMOTime getLastStateChange() const {
        return myLastStateChange;
    }


    /** @brief Returns the direction the vehicle passing this link take
     *
     * @return The direction of this link
     */
    LinkDirection getDirection() const;


    /** @brief Sets the current tl-state
     *
     * @param[in] state The current state of the link
     * @param[in] t The time of the state change
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
    inline int getIndex() const {
        return myIndex;
    }

    /** @brief Returns the TLS index */
    inline int getTLIndex() const {
        return myTLIndex;
    }

    /** @brief Returns the TLS index */
    inline const MSTrafficLightLogic* getTLLogic() const {
        return myLogic;
    }

    /** @brief Returns whether this link is a major link
     * @return Whether the link has a large priority
     */
    inline bool havePriority() const {
        return myState >= 'A' && myState <= 'Z';
    }

    /** @brief Returns whether this link is blocked by a red (or redyellow) traffic light
     * @return Whether the link has a red light
     */
    inline bool haveRed() const {
        return myState == LINKSTATE_TL_RED || myState == LINKSTATE_TL_REDYELLOW;
    }

    inline bool isTLSControlled() const {
        return myLogic != 0;
    }

    /** @brief Returns the length of this link
     *
     * @return The length of this link
     */
    SUMOReal getLength() const {
        return myLength;
    }


    /** @brief Returns the distance on the approaching lane from which an
     *         approaching vehicle is able to see all relevant foes and
     *         may accelerate if the link is minor and no foe is approaching.
     *
     * @return The foe-visibility-distance
     */
    SUMOReal getFoeVisibilityDistance() const {
        return myFoeVisibilityDistance;
    }


    /** @brief Returns whether this link belongs to a junction where more than one edge is incoming
     *
     * @return Whether any foe links exist
     */
    bool hasFoes() const {
        return myHasFoes;
    }

    // @todo documentation
    bool isCont() const {
        return myAmCont;
    }


    /// @brief whether the junction after this link must be kept clear
    bool keepClear() const {
        return myKeepClear;
    }

    /// @brief whether this is a link past an internal junction which currently has priority
    bool lastWasContMajor() const;

    /** @brief Returns the cumulative length of all internal lanes after this link
     *  @return sum of the lengths of all internal lanes following this link
     */
    SUMOReal getInternalLengthsAfter() const;

#ifdef HAVE_INTERNAL_LANES
    /** @brief Returns the following inner lane
     *
     * @return The inner lane to use to cross the junction
     */
    MSLane* getViaLane() const;

    /** @brief Returns all potential link leaders (vehicles on foeLanes)
     * Valid during the planMove() phase
     * @param[in] ego The ego vehicle that is looking for leaders
     * @param[in] dist The distance of the vehicle who is asking about the leader to this link
     * @param[out] blocking Return blocking pedestrians if a vector is given
     * @return The all vehicles on foeLanes and their (virtual) distances to the asking vehicle
     */
    LinkLeaders getLeaderInfo(const MSVehicle* ego, SUMOReal dist, std::vector<const MSPerson*>* collectBlockers = 0) const;
#endif

    /// @brief return the speed at which ego vehicle must approach the zipper link
    SUMOReal getZipperSpeed(const MSVehicle* ego, const SUMOReal dist, SUMOReal vSafe,
                            SUMOTime arrivalTime,
                            std::vector<const SUMOVehicle*>* collectFoes) const;

    /// @brief return the via lane if it exists and the lane otherwise
    MSLane* getViaLaneOrLane() const;

    /// @brief return the internalLaneBefore if it exists and the laneBefore otherwise
    const MSLane* getLaneBefore() const;

    /// @brief return myInternalLaneBefore (always 0 when compiled without internal lanes)
    const MSLane* getInternalLaneBefore() const;

    /// @brief return the expected time at which the given vehicle will clear the link
    SUMOTime getLeaveTime(const SUMOTime arrivalTime, const SUMOReal arrivalSpeed, const SUMOReal leaveSpeed, const SUMOReal vehicleLength) const;

    /// @brief write information about all approaching vehicles to the given output device
    void writeApproaching(OutputDevice& od, const std::string fromLaneID) const;

    /// @brief erase vehicle from myLinkLeaders of this links junction
    void passedJunction(const MSVehicle* vehicle);

    /// @brief return the link that is parallel to this lane or 0
    MSLink* getParallelLink(int direction) const;

    //// @brief @return whether the foe vehicle is a leader for ego
    bool isLeader(const MSVehicle* ego, const MSVehicle* foe);

    /// @brief return whether the fromLane of this link is an internal lane
    bool fromInternalLane() const;

    /// @brief return whether the fromLane of this link is an internal lane and toLane is a normal lane
    bool isExitLink() const;

    /// @brief return whether the fromLane and the toLane of this link are internal lanes
    bool isInternalJunctionLink() const;

    /** @brief Returns the time penalty for passing a tls-controlled link (meso) */
    SUMOTime getMesoTLSPenalty() const {
        return myMesoTLSPenalty;
    }

    /** @brief Returns the average proportion of green time to cycle time */
    SUMOReal getGreenFraction() const {
        return myGreenFraction;
    }

    /** @brief Sets the time penalty for passing a tls-controlled link (meso) */
    void setMesoTLSPenalty(const SUMOTime penalty) {
        myMesoTLSPenalty = penalty;
    }

    /** @brief Sets the green fraction for passing a tls-controlled link (meso) */
    void setGreenFraction(const SUMOReal fraction) {
        myGreenFraction = fraction;
    }

    const std::vector<const MSLane*>& getFoeLanes() {
        return myFoeLanes;
    }

    /// @brief initialize parallel links (to be called after all links are loaded)
    void initParallelLinks();

private:
    /// @brief return whether the given vehicles may NOT merge safely
    static inline bool unsafeMergeSpeeds(SUMOReal leaderSpeed, SUMOReal followerSpeed, SUMOReal leaderDecel, SUMOReal followerDecel) {
        // XXX mismatch between continuous an discrete deceleration
        return (leaderSpeed * leaderSpeed / leaderDecel) <= (followerSpeed * followerSpeed / followerDecel);
    }

    /// @brief returns whether the given lane may still be occupied by a vehicle currently on it
    static bool maybeOccupied(MSLane* lane);

    /// @brief whether fllower could stay behind leader (possibly by braking)
    static bool couldBrakeForLeader(SUMOReal followDist, SUMOReal leaderDist, const MSVehicle* follow, const MSVehicle* leader);

    MSLink* computeParallelLink(int direction);

    bool blockedByFoe(const SUMOVehicle* veh, const ApproachingVehicleInformation& avi, SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal arrivalSpeed, SUMOReal leaveSpeed,
                      bool sameTargetLane, SUMOReal impatience, SUMOReal decel, SUMOTime waitingTime) const;

private:
    /// @brief The lane behind the junction approached by this link
    MSLane* myLane;

    /// @brief The lane approaching this link
    MSLane* myLaneBefore;

    std::map<const SUMOVehicle*, ApproachingVehicleInformation> myApproachingVehicles;
    std::set<MSLink*> myBlockedFoeLinks;

    /// @brief The position within this respond
    int myIndex;

    /// @brief the traffic light index
    const int myTLIndex;

    /// @brief the controlling logic or 0
    const MSTrafficLightLogic* myLogic;

    /// @brief The state of the link
    LinkState myState;

    /// @brief The time of the last state change
    SUMOTime myLastStateChange;

    /// @brief An abstract (hopefully human readable) definition of the link's direction
    LinkDirection myDirection;

    /// @brief The length of the link
    SUMOReal myLength;

    /// @brief distance from which an approaching vehicle is able to
    ///        see all relevant foes and may accelerate if the link is minor
    ///        and no foe is approaching. Defaults to 4.5m.
    SUMOReal myFoeVisibilityDistance;

    /// @brief Whether any foe links exist
    bool myHasFoes;

    // @todo documentation
    bool myAmCont;

    bool myKeepClear;

    /// @brief penalty time for mesoscopic simulation
    SUMOTime myMesoTLSPenalty;
    SUMOReal myGreenFraction;

#ifdef HAVE_INTERNAL_LANES
    /// @brief The following junction-internal lane if used
    MSLane* const myInternalLane;

    /* @brief The preceding junction-internal lane, only used at
     * - exit links (from internal lane to normal lane)
     * - internal junction links (from internal lane to internal lane)
     */
    const MSLane* myInternalLaneBefore;

    /* @brief lengths after the crossing point with foeLane
     * (lengthOnThis, lengthOnFoe)
     * (index corresponds to myFoeLanes)
     * empty vector for entry links
     * */
    std::vector<std::pair<SUMOReal, SUMOReal> > myLengthsBehindCrossing;
#endif

    std::vector<MSLink*> myFoeLinks;
    std::vector<const MSLane*> myFoeLanes;

    /* @brief with the same origin lane and the same destination edge that may
       be in conflict for sublane simulation */
    std::vector<MSLink*> mySublaneFoeLinks;

    static const SUMOTime myLookaheadTime;
    static const SUMOTime myLookaheadTimeZipper;

    MSLink* myParallelRight;
    MSLink* myParallelLeft;

    /// @brief the junction to which this link belongs
    MSJunction* myJunction;

    /// invalidated copy constructor
    MSLink(const MSLink& s);

    /// invalidated assignment operator
    MSLink& operator=(const MSLink& s);

};


#endif

/****************************************************************************/

