/****************************************************************************/
/// @file    ROEdge.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
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
#ifndef ROEdge_h
#define ROEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <utils/common/Named.h>
#include <utils/common/ValueTimeLine.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/SUMOVTypeParameter.h>
#include "RONode.h"
#include "ROVehicle.h"


// ===========================================================================
// class declarations
// ===========================================================================
class ROLane;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROEdge
 * @brief A basic edge for routing applications
 *
 * The edge contains two time lines, one for the travel time and one for a second
 *  measure which may be used for computing the costs of a route. After loading
 *  the weights, it is needed to call "buildTimeLines" in order to initialise
 *  these time lines.
 */
class ROEdge : public Named {
public:
    /**
     * @enum EdgeType
     * @brief Possible types of edges
     */
    enum EdgeType {
        /// @brief A normal edge
        ET_NORMAL,
        /// @brief An edge representing a whole district
        ET_DISTRICT,
        /// @brief An edge where vehicles are inserted at (no vehicle may come from back)
        ET_SOURCE,
        /// @brief An edge where vehicles disappear (no vehicle may leave this edge)
        ET_SINK,
        /// @brief An internal edge which models driving across a junction. This is currently not used for routing
        ET_INTERNAL
    };


    /** @brief Constructor
     *
     * @param[in] id The id of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] index The numeric id of the edge
     */
    ROEdge(const std::string& id, RONode* from, RONode* to, unsigned int index, const int priority);


    /// Destructor
    virtual ~ROEdge();


    /// @name Set-up methods
    //@{

    /** @brief Adds a lane to the edge while loading
     *
     * The lane's length is adapted. Additionally, the information about allowed/disallowed
     *  vehicle classes is patched using the information stored in the lane.
     *
     * @param[in] lane The lane to add
     * @todo What about vehicle-type aware connections?
     */
    virtual void addLane(ROLane* lane);


    /** @brief Adds information about a connected edge
     *
     * The edge is added to "myFollowingEdges".
     * @param[in] s The edge to add
     * @todo What about vehicle-type aware connections?
     * @note: if HAVE_INTERNAL is defined, the backward connections is added as well
     */
    virtual void addFollower(ROEdge* s, std::string dir = "");


    /** @brief Sets the type of te edge
     * @param[in] type The new type for the edge
     */
    void setType(EdgeType type);


    /** @brief Builds the internal representation of the travel time/effort
     *
     * Should be called after weights / travel times have been loaded.
     *
     * In the case "weight-attribute" is one of "CO", "CO2", "HC", "NOx", "PMx", or "fuel"
     *  the proper value (departs/s) is computed and multiplied with the travel time.
     *
     * @param[in] measure The name of the measure to use.
     */
    void buildTimeLines(const std::string& measure);
    //@}



    /// @name Getter methods
    //@{

    /** @brief Returns the type of the edge
     * @return This edge's type
     * @see EdgeType
     */
    EdgeType getType() const {
        return myType;
    }


    /** @brief Returns the length of the edge
     * @return This edge's length
     */
    SUMOReal getLength() const {
        return myLength;
    }

    /** @brief Returns the index (numeric id) of the edge
     * @return This edge's numerical id
     */
    unsigned int getNumericalID() const {
        return myIndex;
    }


    /** @brief Returns the speed allowed on this edge
     * @return The speed allowed on this edge
     */
    SUMOReal getSpeed() const {
        return mySpeed;
    }


    /** @brief Returns the number of lanes this edge has
     * @return This edge's number of lanes
     */
    unsigned int getLaneNo() const {
        return (unsigned int) myLanes.size();
    }


    /** @brief Returns the node this edge starts at
     * @return The node this edge starts at
     */
    RONode* getFromNode() const {
        return myFromNode;
    }


    /** @brief Returns the node this edge ends at
     * @return The node this edge ends at
     */
    RONode* getToNode() const {
        return myToNode;
    }


    /** @brief returns the information whether this edge is directly connected to the given
     *
     * @param[in] e The edge which may be connected
     * @return Whether the given edge is a direct successor to this one
     */
    bool isConnectedTo(const ROEdge* const e) const {
        return std::find(myFollowingEdges.begin(), myFollowingEdges.end(), e) != myFollowingEdges.end();
    }


    /** @brief Returns whether this edge prohibits the given vehicle to pass it
     * @param[in] vehicle The vehicle for which the information has to be returned
     * @return Whether the vehicle must not enter this edge
     */
    inline bool prohibits(const ROVehicle* const vehicle) const {
        const SUMOVehicleClass vclass = vehicle->getVClass();
        return (myCombinedPermissions & vclass) != vclass;
    }

    inline SVCPermissions getPermissions() const {
        return myCombinedPermissions;
    }


    /** @brief Returns whether this edge succeding edges prohibit the given vehicle to pass them
     * @param[in] vehicle The vehicle for which the information has to be returned
     * @return Whether the vehicle may continue its route on any of the following edges
     */
    bool allFollowersProhibit(const ROVehicle* const vehicle) const;
    //@}



    /// @name Methods for getting/setting travel time and cost information
    //@{

    /** @brief Adds a weight value
     *
     * @param[in] value The value to add
     * @param[in] timeBegin The begin time of the interval the given value is valid for [s]
     * @param[in] timeEnd The end time of the interval the given value is valid for [s]
     */
    void addEffort(SUMOReal value, SUMOReal timeBegin, SUMOReal timeEnd);


    /** @brief Adds a travel time value
     *
     * @param[in] value The value to add
     * @param[in] timeBegin The begin time of the interval the given value is valid for [s]
     * @param[in] timeEnd The end time of the interval the given value is valid for [s]
     */
    void addTravelTime(SUMOReal value, SUMOReal timeBegin, SUMOReal timeEnd);


    /** @brief Returns the number of edges this edge is connected to
     *
     * If this edge's type is set to "sink", 0 is returned, otherwise
     *  the number of edges stored in "myFollowingEdges".
     *
     * @return The number of edges following this edge
     */
    unsigned int getNoFollowing() const;


    /** @brief Returns the edge at the given position from the list of reachable edges
     * @param[in] pos The position of the list within the list of following
     * @return The following edge, stored at position pos
     */
    ROEdge* getFollower(unsigned int pos) const {
        return myFollowingEdges[pos];
    }


    /** @brief Returns the number of edges this edge is connected to
     *
     * If this edge's type is set to "source", 0 is returned, otherwise
     *  the number of edges stored in "myApproachingEdges".
     *
     * @return The number of edges following this edge
     */
    unsigned int getNumApproaching() const;


    /** @brief Returns the edge at the given position from the list of reachable edges
     * @param[in] pos The position of the list within the list of approached
     * @return The following edge, stored at position pos
     */
    ROEdge* getApproaching(unsigned int pos) const {
        return myApproachingEdges[pos];
    }


    /** @brief Returns the effort for this edge
     *
     * @param[in] veh The vehicle for which the effort on this edge shall be retrieved
     * @param[in] time The tim for which the effort shall be returned [s]
     * @return The effort needed by the given vehicle to pass the edge at the given time
     * @todo Recheck whether the vehicle's maximum speed is considered
     */
    SUMOReal getEffort(const ROVehicle* const veh, SUMOReal time) const;


    /** @brief Returns the travel time for this edge
     *
     * @param[in] veh The vehicle for which the effort on this edge shall be retrieved
     * @param[in] time The time for which the effort shall be returned [s]
     * @return The traveltime needed by the given vehicle to pass the edge at the given time
     */
    SUMOReal getTravelTime(const ROVehicle* const veh, SUMOReal time) const;


    /** @brief Returns the travel time for this edge
     *
     * @param[in] maxSpeed The maximum speed to assume if no travel times are stored
     * @param[in] time The time in seconds(!) for which the traveltime shall be returned
     * @return The traveltime needed to pass the edge at the given time
     */
    SUMOReal getTravelTime(const SUMOReal maxSpeed, SUMOReal time) const;


    /** @brief Returns the travel time for this edge without using any stored timeLine
     *
     * @param[in] veh The vehicle for which the effort on this edge shall be retrieved
     * @param[in] time The time for which the effort shall be returned [s]
     */
    SUMOReal getMinimumTravelTime(const ROVehicle* const veh) const;


    SUMOReal getCOEffort(const ROVehicle* const veh, SUMOReal time) const;
    SUMOReal getCO2Effort(const ROVehicle* const veh, SUMOReal time) const;
    SUMOReal getPMxEffort(const ROVehicle* const veh, SUMOReal time) const;
    SUMOReal getHCEffort(const ROVehicle* const veh, SUMOReal time) const;
    SUMOReal getNOxEffort(const ROVehicle* const veh, SUMOReal time) const;
    SUMOReal getFuelEffort(const ROVehicle* const veh, SUMOReal time) const;
    SUMOReal getNoiseEffort(const ROVehicle* const veh, SUMOReal time) const;
    //@}


    /// @brief optimistic distance heuristic for use in routing
    SUMOReal getDistanceTo(const ROEdge* other) const;


    /** @brief Returns the ROEdge at the index */
    static ROEdge* dictionary(size_t index);

    /// @brief Returns the number of edges
    static size_t dictSize() {
        return myEdges.size();
    };

    static void setTimeLineOptions(
        bool useBoundariesOnOverrideTT,
        bool useBoundariesOnOverrideE,
        bool interpolate) {
        myUseBoundariesOnOverrideTT = useBoundariesOnOverrideTT;
        myUseBoundariesOnOverrideE = useBoundariesOnOverrideE;
        myInterpolate = interpolate;
    }

    /// @brief get edge priority (road class)
    int getPriority() const {
        return myPriority;
    }

protected:
    /** @brief Retrieves the stored effort
     *
     * @param[in] veh The vehicle for which the effort on this edge shall be retrieved
     * @param[in] time The tim for which the effort shall be returned
     * @return Whether the effort is given
     */
    bool getStoredEffort(SUMOReal time, SUMOReal& ret) const;



protected:
    /// @brief The nodes this edge is connecting
    RONode* const myFromNode, * const myToNode;

    /// @brief The index (numeric id) of the edge
    const unsigned int myIndex;

    /// @brief The edge priority (road class)
    const int myPriority;

    /// @brief The maximum speed allowed on this edge
    SUMOReal mySpeed;

    /// @brief The length of the edge
    SUMOReal myLength;


    /// @brief Container storing passing time varying over time for the edge
    mutable ValueTimeLine<SUMOReal> myTravelTimes;
    /// @brief Information whether the time line shall be used instead of the length value
    bool myUsingTTTimeLine;
    /// @brief Whether overriding weight boundaries shall be reported
    static bool myUseBoundariesOnOverrideTT;

    /// @brief Container storing passing time varying over time for the edge
    mutable ValueTimeLine<SUMOReal> myEfforts;
    /// @brief Information whether the time line shall be used instead of the length value
    bool myUsingETimeLine;
    /// @brief Whether overriding weight boundaries shall be reported
    static bool myUseBoundariesOnOverrideE;

    /// @brief Information whether to interpolate at interval boundaries
    static bool myInterpolate;

    /// @brief Information whether the edge has reported missing weights
    static bool myHaveEWarned;
    /// @brief Information whether the edge has reported missing weights
    static bool myHaveTTWarned;

    /// @brief List of edges that may be approached from this edge
    std::vector<ROEdge*> myFollowingEdges;

    /// @brief List of edges that approached this edge
    std::vector<ROEdge*> myApproachingEdges;

    /// @brief The type of the edge
    EdgeType myType;

    /// @brief This edge's lanes
    std::vector<ROLane*> myLanes;

    /// @brief The list of allowed vehicle classes combined across lanes
    SVCPermissions myCombinedPermissions;

    static std::vector<ROEdge*> myEdges;


private:
    /** @brief Returns the minimum travel time for this edge
     * If there is a timeline-value for the given time it is returned, otherwise
     * the maximum speed of the edge is assumed
     * @param[in] veh The vehicle for which the traveltime on this edge shall be retrieved
     * @return The minimum traveltime needed to pass the edge at the given time
     */
    SUMOReal getTravelTime(SUMOReal time) const;

private:
    /// @brief Invalidated copy constructor
    ROEdge(const ROEdge& src);

    /// @brief Invalidated assignment operator
    ROEdge& operator=(const ROEdge& src);

};


#endif

/****************************************************************************/

