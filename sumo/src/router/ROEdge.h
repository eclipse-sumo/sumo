#ifndef ROEdge_h
#define ROEdge_h
//---------------------------------------------------------------------------//
//                        ROEdge.h -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2003/11/11 08:04:46  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.6  2003/09/17 10:14:27  dkrajzew
// handling of unset values patched
//
// Revision 1.5  2003/04/15 09:09:18  dkrajzew
// documentation added
//
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <utils/router/ValueTimeLine.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROEdge
 * An edge.
 */
class ROEdge {
public:
    /**
     * @enum EdgeType
     * Possible types of edges
     */
    enum EdgeType {
        /// A normal edge
        ET_NORMAL,
        /// An edge where vehicles are emitted at (no vehicle may come from back)
        ET_SOURCE,
        /// An edge where vehicles disappear (no vehicle may leave this edge)
        ET_SINK
    };


    /// Constructor
	ROEdge(const std::string &id);

    /// Desturctor
	~ROEdge();

    /** @brief Initialises te edge after loading
        The weights over time are being set, here */
    void postloadInit();

    /// Adds a lane to the edge while loading
    void addLane(ROLane *lane);

    /// Sets the effort of a lane while loading
    void setLane(long timeBegin, long timeEnd,
        const std::string &id, float value);

    /// Adds information about a connected edge
    void addSucceeder(ROEdge *s);

    /// returns the information whether this edge is directly connected to the given
    bool isConnectedTo(ROEdge *e);

    /** @brief Returns the number of edges this edge is connected to
        (size of the list of reachable edges) */
    size_t getNoFollowing();

    /// Retunrns the edge at the given position from the list of reachable edges
    ROEdge *getFollower(size_t pos);

    /// retrieves the cost of this edge at the given time
    double getCost(long time) const;

    /// Retrieves the time a vehicle needs to pass this edge starting at the given time
    double getDuration(long time) const;

    // dijkstra
    /// Sets the current effort (from dijstra-router)
    void setEffort(double effort);

    /// Resets the edge for next route computation
    void init();

    /// Initialises the edge if it is the starting edge
    void initRootDistance();

    /// Returns the effort needed to pass this edge
    float getEffort() const;

    /// Returns the effort when starting at the gievn time
    float getNextEffort(long time) const;

    /// Sets the effort
    void setEffort(float dist);

    /// Returns the information whether the edge was already visited
    bool isInFrontList() const;

    /// Adds a connection, marking the effort to pas the connection (!!!)
    bool addConnection(ROEdge *to, float effort);

    /// Returns the information whether the edge was laready visited
    bool isExplored() const;

    /// Marks the edge as visted
    void setExplored(bool value);

    /// returns the edge this edge was approached from for the current route
    ROEdge *getPrevKnot() const;

    /// Sets the information from which edge this one was approached when computing the current route
    void setPrevKnot(ROEdge *prev);

    /// Returns the id of the edge
    std::string getID() const;

    /// Sets the type of te edge
    void setType(EdgeType type);

    /// Returns the type of the edge
    EdgeType getType() const;

    /// Returns the length of the edge
    double getLength() const;

protected:
    /// reurns the effort for this edge only
    float getMyEffort(long time) const;

private:
    /// The id of the edge
    std::string _id;

    /// Information whether this edge was already seen during the computation of the current route
    bool _explored;

    /// The maximum distance of this edge (including all lanes)
    double _dist;

    double _speed;

    /// The edge this one was approached from within the current route
    ROEdge *_prevKnot;

    /// Information whether this edge was already visted within the computation of the current route
    bool _inFrontList;

    /// The effort needed to come to the end of this route (including previous edges)
	double _effort;

    /// Definition of a container for storing passing time varying over time for an edge's lanes
    typedef std::map<ROLane*, ValueTimeLine*> LaneUsageCont;

    /// Container for storing passing time varying over time for an edge's lanes
    LaneUsageCont _laneCont;

    /// Container storing passing time varying over time for the edge
    ValueTimeLine _ownValueLine;

    /// List of edges that may be approached from this edge
    std::vector<ROEdge*> _succeeding;

    /// infomration whether the time line shall be used instead of the length value
    bool _usingTimeLine;

    /// The type of the edge
    EdgeType myType;

private:
    /// we made the copy constructor invalid
    ROEdge(const ROEdge &src);

    /// we made the assignment operator invalid
    ROEdge &operator=(const ROEdge &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

