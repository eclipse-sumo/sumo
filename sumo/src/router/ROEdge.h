/****************************************************************************/
/// @file    ROEdge.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/ValueTimeLine.h>
#include <utils/common/SUMOVehicleClass.h>
#include "RONode.h"


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
 */
class ROEdge {
public:
    /**
     * @enum EdgeType
     * @brief Possible types of edges
     */
    enum EdgeType {
        /// @brief A normal edge
        ET_NORMAL,
        /// @brief An edge where vehicles are emitted at (no vehicle may come from back)
        ET_SOURCE,
        /// @brief An edge where vehicles disappear (no vehicle may leave this edge)
        ET_SINK
    };


    /** @brief Constructor
     *
     * @param[in] id The id of the edge
     * @param[in] index The numeric id of the edge
     * @param[in] useBoundariesOnOverride Whether the edge shall use a boundary value if the requested is beyond known time scale
     * @todo useBoundariesOnOverride should not be a member of the edge
     */
    ROEdge(const std::string &id, unsigned int index, bool useBoundariesOnOverride) throw();


    /// Destructor
    virtual ~ROEdge() throw();


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
    virtual void addLane(ROLane *lane) throw();


    /** @brief Adds information about a connected edge
     *
     * The edge is added to "myFollowingEdges".
     *
     * @param[in] lane The lane to add
     * @todo What about vehicle-type aware connections?
     */
    virtual void addFollower(ROEdge *s) throw();


    /** @brief Sets the type of te edge
     * @param[in] type The new type for the edge
     */
    void setType(EdgeType type) throw();


    /** @brief Sets the information which nodes this edge connects
     * @param[in] from The node this edge starts at
     * @param[in] to The node this edge ends at
     */
    void setNodes(RONode *from, RONode *to) throw();
    //@}



    /// @name Getter methods
    //@{

    /** @brief Returns the id of the edge
     * @return This edge's id
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns the type of the edge
     * @return This edge's type
     * @see EdgeType
     */
    EdgeType getType() const throw() {
        return myType;
    }


    /** @brief Returns the length of the edge
     * @return This edge's length
     */
    SUMOReal getLength() const throw() {
        return myLength;
    }

    /** @brief Returns the index (numeric id) of the edge
     * @return This edge's numerical id
     */
    unsigned int getNumericalID() const throw() {
        return myIndex;
    }


    /** @brief Returns the speed allowed on this edge
     * @return The speed allowed on this edge
     */
    SUMOReal getSpeed() const throw() {
        return mySpeed;
    }


    /** @brief Returns the number of lanes this edge has
     * @return This edge's number of lanes
     */
    unsigned int getLaneNo() const throw() {
        return (unsigned int) myLanes.size();
    }


    /** @brief Returns the node this edge starts at
     * @return The node this edge starts at
     */
    RONode *getFromNode() const throw() {
        return myFromNode;
    }


    /** @brief Returns the node this edge ends at
     * @return The node this edge ends at
     */
    RONode *getToNode() const throw() {
        return myToNode;
    }


    /** @brief returns the information whether this edge is directly connected to the given
     *
     * @param[in] e The edge which may be connected
     * @return Whether the given edge is a direct successor to this one
     */
    bool isConnectedTo(const ROEdge * const e) const throw() {
        return std::find(myFollowingEdges.begin(), myFollowingEdges.end(), e)!=myFollowingEdges.end();
    }


    /** @brief Returns whether this edge prohibits the given vehicle to pass it
     * @param[in] vehicle The vehicle for which the information has to be returned
     * @return Whether the vehicle must not enter this edge
     */
    bool prohibits(const ROVehicle * const vehicle) const throw();


    /** @brief Returns whether this edge succeding edges prohibit the given vehicle to pass them
     * @param[in] vehicle The vehicle for which the information has to be returned
     * @return Whether the vehicle may continue its route on any of the following edges
     */
    bool allFollowersProhibit(const ROVehicle * const vehicle) const throw();
    //@}



    /// @name Methods for getting/setting travel cost information
    //@{

    /** @brief Adds a weight definition
     *
     * @param[in] value The value to add
     * @param[in] timeBegin The begin time of the interval the given value is valid for
     * @param[in] timeEnd The end time of the interval the given value is valid for
     * @todo Refactor weights usage
     */
    void addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd) throw();


    /** @brief Returns the number of edges this edge is connected to
     *
     * If this edge's type is set to "sink", 0 is returned, otherwise
     *  the number of edges stored in "myFollowingEdges".
     *
     * @return The number of edges following this edge
     */
    unsigned int getNoFollowing() const throw();


    /** @brief Returns the edge at the given position from the list of reachable edges
     * @param[in] pos The position of the list within the list of following
     * @return The following edge, stored at position pos
     */
    ROEdge *getFollower(unsigned int pos) const throw() {
        return myFollowingEdges[pos];
    }


    /** @brief Returns the effort (normally the travel time) for this edge
     *
     * @param[in] veh The vehicle for which the effort on this edge shall be retrieved
     * @param[in] time The tim for which the effort shall be returned
     * @return The effort needed by the given vehicle to pass the edge at the given time
     * @todo Refactor weights usage
     * @todo Recheck whether the vehicle's maximum speed is considered
     */
    SUMOReal getEffort(const ROVehicle * const veh, SUMOTime time) const throw();
    //@}



protected:
    /// @brief The id of the edge
    std::string myID;

    /// @brief The maximum speed allowed on this edge
    SUMOReal mySpeed;


    /// @brief Container storing passing time varying over time for the edge
    mutable ValueTimeLine<SUMOReal> myOwnValueLine;
    /// @brief Information whether the time line shall be used instead of the length value
    bool myUsingTimeLine;
    /// @brief Whether overriding weight boundaries shall be reported
    bool myUseBoundariesOnOverride;
    /// @brief Whether gaps in the timeline have been filled
    mutable bool myHaveGapsFilled;

    /// @brief Information whether the edge has reported missing weights
    static bool myHaveWarned;


    /// @brief List of edges that may be approached from this edge
    std::vector<ROEdge*> myFollowingEdges;

    /// @brief The type of the edge
    EdgeType myType;

    /// @brief The index (numeric id) of the edge
    unsigned int myIndex;

    /// @brief The length of the edge
    SUMOReal myLength;

    /// @brief This edge's lanes
    std::vector<ROLane*> myLanes;

    /// @brief The list of allowed vehicle classes
    std::vector<SUMOVehicleClass> myAllowedClasses;

    /// @brief The list of disallowed vehicle classes
    std::vector<SUMOVehicleClass> myNotAllowedClasses;

    /// @brief The nodes this edge is connecting
    RONode *myFromNode, *myToNode;

private:
    /// @brief Invalidated copy constructor
    ROEdge(const ROEdge &src);

    /// @brief Invalidated assignment operator
    ROEdge &operator=(const ROEdge &src);

};


#endif

/****************************************************************************/

