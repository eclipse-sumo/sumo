/****************************************************************************/
/// @file    ROEdge.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <utils/common/FloatValueTimeLine.h>
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
class ROEdge
{
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


    /// Constructor
    ROEdge(const std::string &id, int index, bool useBoundariesOnOverride) throw();

    /// Destructor
    virtual ~ROEdge() throw();

    /// Adds a lane to the edge while loading
    virtual void addLane(ROLane *lane);

    void addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd);

    /// Adds information about a connected edge
    virtual void addFollower(ROEdge *s);

    /// returns the information whether this edge is directly connected to the given
    bool isConnectedTo(const ROEdge * const e) const;

    /** @brief Returns the number of edges this edge is connected to
        (size of the list of reachable edges) */
    size_t getNoFollowing() const;

    /// Returns the edge at the given position from the list of reachable edges
    ROEdge *getFollower(size_t pos) const;

    /// retrieves the cost of this edge at the given time
    SUMOReal getCost(const ROVehicle *const, SUMOTime time) const;

    /// Retrieves the time a vehicle needs to pass this edge starting at the given time
    SUMOReal getDuration(const ROVehicle *const, SUMOTime time) const;

    /// Adds a connection, marking the effort to pas the connection (!!!)
    bool addConnection(ROEdge *to, SUMOReal effort);

    /// Returns the id of the edge
    const std::string &getID() const;

    /// Sets the type of te edge
    void setType(EdgeType type);

    /// Returns the type of the edge
    EdgeType getType() const;

    /// Returns the length of the edge
    SUMOReal getLength() const;

    /// Returns the index (numeric id) of the edge
    size_t getNumericalID() const;

    /// returns the effort for this edge only
    virtual SUMOReal getEffort(const ROVehicle *const, SUMOReal time) const;

    /**
     * Takes pointers to FloatValueTimeLines and assigns them to the classes
     *  supplementary weights. You must provide all three FloatValueTimeLines
     *  and they must be valid objects. These objects will be deleted on deletion
     *  of this ROEdge. The flag hasSupplementaryWeights will be set to true and
     *  getMyEffort() will use this supplementary weights in subsequent calls.
     * @param absolut Pointer to the absolut-FloatValueTimeLine.
     *
     * @param add Pointer to the add-FloatValueTimeLine.
     * @param mult Pointer to the mult-FloatValueTimeLine.
     */
    void setSupplementaryWeights(FloatValueTimeLine* absolut,
                                 FloatValueTimeLine* add,
                                 FloatValueTimeLine* mult);

    SUMOReal getSpeed() const;

    int getLaneNo() const;

    bool prohibits(const ROVehicle * const vehicle) const;
    bool allFollowersProhibit(const ROVehicle * const vehicle) const;

    void setNodes(RONode *from, RONode *to);

    RONode *getFromNode() const;
    RONode *getToNode() const;

protected:
    /// The id of the edge
    std::string myID;

    /// The maximum distance of this edge (including all lanes)
    SUMOReal myDist;

    /// The maximum speed allowed on this edge
    SUMOReal mySpeed;

    /// Container storing passing time varying over time for the edge
    FloatValueTimeLine myOwnValueLine;

    /// "Absolut" supplementary weights.
    FloatValueTimeLine* mySupplementaryWeightAbsolut;
    /// "Add" supplementary weights.
    FloatValueTimeLine* mySupplementaryWeightAdd;
    /// "Multiplication" supplementary weights.
    FloatValueTimeLine* mySupplementaryWeightMult;

    /// List of edges that may be approached from this edge
    std::vector<ROEdge*> myFollowingEdges;

    /// information whether the time line shall be used instead of the length value
    bool myUsingTimeLine;


    /// The type of the edge
    EdgeType myType;

    /// The index (numeric id) of the edge
    size_t myIndex;

    /// The length of the edge
    SUMOReal myLength;

    /// Flag that indicates, if the supplementary weights have been
    /// set. Initially false.
    bool myHasSupplementaryWeights;

    /// Information whether the edge has reported missing weights
    static bool myHaveWarned;

    std::vector<ROLane*> myLanes;

    /// The list of allowed vehicle classes
    std::vector<SUMOVehicleClass> myAllowedClasses;

    /// The list of disallowed vehicle classes
    std::vector<SUMOVehicleClass> myNotAllowedClasses;

    /// The nodes this edge is connecting
    RONode *myFromNode, *myToNode;

    bool myUseBoundariesOnOverride;
    mutable bool myHaveBuildShortCut;
    mutable SUMOReal *myPackedValueLine;
    mutable SUMOTime myShortCutBegin, myShortCutEnd, myShortCutInterval;
    mutable size_t myLastPackedIndex;


private:
    /// we made the copy constructor invalid
    ROEdge(const ROEdge &src);

    /// we made the assignment operator invalid
    ROEdge &operator=(const ROEdge &src);

};


#endif

/****************************************************************************/

