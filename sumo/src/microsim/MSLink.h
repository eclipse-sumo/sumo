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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class SUMOVehicle;


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
#ifndef HAVE_INTERNAL_LANES
    /** @brief Constructor for simulation not using internal lanes
     *
     * @param[in] succLane The lane approached by this link
     * @param[in] dir The direction of this link
     * @param[in] state The state of this link
     * @param[in] length The length of this link
     */
    MSLink(MSLane* succLane,
           LinkDirection dir, LinkState state, SUMOReal length) ;
#else
    /** @brief Constructor for simulation which uses internal lanes
     *
     * @param[in] succLane The lane approached by this link
     * @param[in] via The lane to use within the junction
     * @param[in] dir The direction of this link
     * @param[in] state The state of this link
     * @param[in] length The length of this link
     */
    MSLink(MSLane* succLane, MSLane* via,
           LinkDirection dir, LinkState state,
           SUMOReal length) ;
#endif

    /// @brief Destructor
    ~MSLink() ;


    /** @brief Sets the request information
     *
     * Because traffic lights and junction logics are loaded after links,
     *  we have to assign the information about the right-of-way
     *  requests and responses after the initialisation.
     *
     * @param[in] requestIdx This link's index within this request
     * @param[in] respondIdx This link's index within this respond
     * @param[in] foes This link's foes
     * @todo Unsecure!
     */
    void setRequestInformation(unsigned int requestIdx, unsigned int respondIdx, bool isCrossing, bool isCont,
                               const std::vector<MSLink*> &foeLinks, const std::vector<MSLane*> &foeLanes) ;


    /** @brief Sets the information about an approaching vehicle
     *
     * The information is stored in myApproachingVehicles.
     *
     * @param[in] approaching The approaching vehicle
     */
    void setApproaching(SUMOVehicle* approaching, SUMOTime arrivalTime, SUMOReal speed, bool setRequest) ;

    void addBlockedLink(MSLink* link) ;



    void removeApproaching(SUMOVehicle* veh);



    /** @brief Returns the information whether the link may be passed
     *
     * Valid after the junctions have set their reponds
     *
     * @return Whether this link may be passed.
     */
    bool opened(SUMOTime arrivalTime, SUMOReal arrivalSpeed, SUMOReal vehicleLength) const ;

    bool blockedAtTime(SUMOTime arrivalTime, SUMOTime leaveTime) const ;
    bool isBlockingAnyone() const {
        return myApproachingVehicles.size() != 0;
    }

    bool willHaveBlockedFoe() const ;



    /** @brief Returns the information whether a vehicle is approaching on one of the link's foe streams
     *
     * Valid after the vehicles have set their requests
     *
     * @return Whether a foe of this link is approaching
     */
    bool hasApproachingFoe(SUMOTime arrivalTime, SUMOTime leaveTime) const ;


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
    LinkDirection getDirection() const ;


    /** @brief Sets the current tl-state
     *
     * @param[in] state The current state of the link
     */
    void setTLState(LinkState state, SUMOTime t) ;


    /** @brief Returns the connected lane
     *
     * @return The lane approached by this link
     */
    MSLane* getLane() const ;


    /** @brief Returns the respond index (for visualization)
     *
     * @return The respond index for this link
     */
    unsigned int getRespondIndex() const ;


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

#ifdef HAVE_INTERNAL_LANES
    /** @brief Returns the following inner lane
     *
     * @return The inner lane to use to cross the junction
     */
    MSLane* getViaLane() const ;
#endif

private:
    struct ApproachingVehicleInformation {
        ApproachingVehicleInformation(const SUMOTime _arrivalTime, const SUMOTime _leavingTime, SUMOVehicle* _vehicle, const bool _willPass)
            : arrivalTime(_arrivalTime), leavingTime(_leavingTime), vehicle(_vehicle), willPass(_willPass) {}
        SUMOTime arrivalTime;
        SUMOTime leavingTime;
        SUMOVehicle* vehicle;
        bool willPass;
    };

    typedef std::vector<ApproachingVehicleInformation> LinkApproachingVehicles;

    class vehicle_in_request_finder {
    public:
        explicit vehicle_in_request_finder(const SUMOVehicle* const v) : myVehicle(v) { }
        bool operator()(const ApproachingVehicleInformation& vo) {
            return vo.vehicle == myVehicle;
        }
    private:
        vehicle_in_request_finder& operator=(const vehicle_in_request_finder&); // just to avoid a compiler warning
    private:
        const SUMOVehicle* const myVehicle;

    };


private:
    /// @brief The lane approached by this link
    MSLane* myLane;

    LinkApproachingVehicles myApproachingVehicles;
    std::set<MSLink*> myBlockedFoeLinks;

    /// @brief The position of the link within this request
    unsigned int myRequestIdx;

    /// @brief The position within this respond
    unsigned int myRespondIdx;

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

