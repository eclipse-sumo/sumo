#ifndef NBEdge_h
#define NBEdge_h
/***************************************************************************
                          NBEdge.h
			  The representation of a single edge
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2003/02/13 15:50:59  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.3  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:32:01  dkrajzew
// possibility to add connections between lanes added; adding of connectionsbetween edges revalidated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/06/11 16:00:39  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:23:09  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/15 09:14:26  traffic
// Changed the number of lanes field into unsigned
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include "NBCont.h"
#include <utils/common/IntVector.h>
#include <utils/geom/Bresenham.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBEdge
{
public:
    static const int LINKTYPE_INVALID;
    static const int LINKTYPE_UNPRIORISED;
    static const int LINKTYPE_PRIORISED;
public:
    /** lane (index) -> list of reachable edges */
    typedef std::vector<EdgeLaneVector> ReachableFromLaneVector;

    /** lane (index) -> list of link priorities */
    typedef std::vector<IntVector> ReachablePrioritiesFromLaneVector;

    /** edge -> list of this edge reaching lanes */
    typedef std::map<NBEdge*, LaneVector> LanesThatSucceedEdgeCont;

    /**
     * EdgeBasicFunction
     * Edges may have certain functions listed here
     */
    enum EdgeBasicFunction {
        /// the edge is normal street within the network
        EDGEFUNCTION_UNKNOWN,
        /// the edge is normal street within the network
        EDGEFUNCTION_NORMAL,
        /// the edge is a source
        EDGEFUNCTION_SOURCE,
        /// the edge is a sink
        EDGEFUNCTION_SINK
    };

public:
    /// constructor
    NBEdge(std::string id, std::string name,
        NBNode *from, NBNode *to, std::string type,
        double speed, size_t nolanes, double length, int priority,
        EdgeBasicFunction basic=EDGEFUNCTION_NORMAL);

    /// destructor
    ~NBEdge();

    /// computes which edge shall be the turn-around one, if any
    void computeTurningDirections();

    /// returns the angle of the edge
    double getAngle();

    /// returns the priority of the edge
    int getPriority();

    /// sets the junction priority of the edge
    void setJunctionPriority(NBNode *node, int prio);

    /// returns the junction priority (normalised for the node currently build)
    int getJunctionPriority(NBNode *node);

    /// sets the junction angle
    void setJunctionAngle(NBNode *node, double angle);

    /// returns the junction angle
    double getJunctionAngle(NBNode *node);

    /// returns the number of lanes
    size_t getNoLanes();

    /// returns the edges approached by the given lane
    const EdgeLaneVector *getEdgeLanesFromLane(size_t lane);

    /// returns the id of the edge
    std::string getID();

    /// returns the name of the edge
    std::string getName();

    /// returns the origin node of the edge
    NBNode *getFromNode();

    /// returns the destination node of the edge
    NBNode *getToNode();

    /// returns the type of the edge
    std::string getType();

    /// returns the length of the edge
    double getLength();

    /** writes the edge definition with lanes and connected edges
        into the given stream */
    void writeXMLStep1(std::ostream &into);

    /** writes the succeeding lane information */
    void writeXMLStep2(std::ostream &into);

    /** writes the positions of edges */
    void writeXMLStep3(std::ostream &into);

    /** adds a connection to another edge;
        instead of adding connections one by one, they may be computed
        using "computeEdge2Edges"
        returns false when an connection already existed; otherwise true */
    bool addEdge2EdgeConnection(NBEdge *dest);

    /** adds connections between this edge's lanes and the approached ones */
    bool addLane2LaneConnection(size_t from, NBEdge *dest, size_t toLane);

    /// computes the edge (step1: computation of approached edges)
    bool computeEdge2Edges();

    /// sorts the connections of outgoing lanes (!!! Kaskade beschreiben)
    void sortOutgoingLanesConnections();

    /// returns the information whether this edge is a source only
    // !!! regarded?
    bool isSource() const;

    /// returns the information whether this edge is a sink only
    // !!! regarded?    bool isSink() const;

    /** recheck whether all lanes within the edge are all right and
        optimises the connections once again */
    bool recheckLanes(bool verbose);

    /// computes the node-internal priorities of links
    void computeLinkPriorities();

    /// appends turnarounds
    void appendTurnaround();

    /// returns the list of lanes that may be used to reach the given edge
    std::vector<size_t> getConnectionLanes(NBEdge *currentOutgoing);

    /// adds a connection to a certain lane of a certain edge
    void setConnection(size_t lane, NBEdge *destEdge, size_t destLane);

    /** returns the information whether the given edge is the opposite
        direction to this edge */
    bool isTurningDirection(NBEdge *edge) const;


    /** @brief Returns the node at the given edges length (using an epsilon)
        When no node is existing at the given position, 0 is returned
        The epsilon is a static member of NBEdge, should be setable via program options */
    NBNode *tryGetNodeAtPosition(double pos) const;

    NBEdge *checkCorrectNode(NBEdge *opposite);

    EdgeBasicFunction getBasicType() const;

    double getSpeed() const;

    void replaceInConnections(NBEdge *which, NBEdge *by);

    /** returns the list of outgoing edges without the turnaround
        sorted in clockwise direction */
    const std::vector<NBEdge*> *getConnectedSorted();

    /** friend class used for the computation of connections to
        following edges */
    friend class NBEdgeSuccessorBuilder;

    friend class NBEdgeCont;

private:
    /**
     * ToEdgeConnectionsAdder
     * A class that being a bresenham-callback assigns the incoming
     * lanes to the edges
     */
    class ToEdgeConnectionsAdder : public Bresenham::BresenhamCallBack {
    private:
        /// map of edges to this edge's lanes that reach them
        std::map<NBEdge*, std::vector<size_t> > *_connections;

        /// the transition from the virtual lane to the edge it belongs to
        const std::vector<NBEdge*> &_transitions;

    public:
        /// constructor
        ToEdgeConnectionsAdder(std::map<NBEdge*,
            std::vector<size_t> > *connections,
            const std::vector<NBEdge*> &transitions)
            : _connections(connections), _transitions(transitions) { }

        /// destructor
        ~ToEdgeConnectionsAdder() { }

        /// executes a bresenham - step
        void execute(double lane, double virtEdge);
    };

    /**
     * MainDirections
     * Holds (- relative to the edge it is build from -!!!) the list of
     * main directions a vehicle that drives on this street may take on
     * the junction the edge ends in
     * The back direction is not regarded
     */
    class MainDirections {
    public:
        /// enum of possible directions
        enum Direction { DIR_RIGHTMOST, DIR_LEFTMOST, DIR_FORWARD };

        /** list of the main direction within the following junction relative
            to the edge */
        std::vector<Direction> _dirs;

    public:
        /// constructor
        MainDirections(const std::vector<NBEdge*> &outgoing,
            NBEdge *parent, NBNode *to);

        /// destructor
        ~MainDirections();

        /** returns the information whether no following street has a higher
            priority */
        bool empty() const;

        /** returns the information whether the street in the given direction
            has a higher priority */
        bool includes(Direction d) const;
    };

    /**
     * As the network is build in a cascaded way, considering loaded
     * information, a counter holding the current step is needed. This is done
     * by using this enumeration
     */
    enum EdgeBuildingStep {
        /// the edge has been loaded, nothing is computed yet
        INIT = 0,
        /// the relationships between edges are computed/loaded
        EDGE2EDGES = 1,
        /** the relationships between edges and approached lanes are
            loaded/computed (!!!) */
        EDGE2LANES = 2
    };

private:
    /// the building step
    EdgeBuildingStep _step;

    /// the id of the edge
    std::string _id;

    /// the type of the edge
    std::string _type;

    // the number of lanes
    size_t     _nolanes;

    /// the source and the destination node
    NBNode  *_from, *_to;

    /// the length of the node
    double  _length;

    /// the angle of the current edge
    double  _angle;

    /** the converted priority of the edge (needed while the ocmputation of
        the node the edge is incoming in / outgoing of) */
    int     _priority;

    /// the maximal speed
    double  _speed;

    /// the name of the edge
    std::string  _name;

    /// information about connected edges
    std::vector<NBEdge*> *_connectedEdges;

    /// information about which edge is approached by which lanes
    std::map<NBEdge*, std::vector<size_t> > *_ToEdges;

    /// the edge to turn into
    NBEdge *_turnDestination;

    /** contains the information which lanes of which edges may follow a
        lane (index) */
    ReachableFromLaneVector *_reachable;

    /** contains the information which links have which priority
        the priorities are sorted in the same way as edges in
        _reachableedges are */
    ReachablePrioritiesFromLaneVector *_reachablePriorities;

    /** contains the information which lanes(value) may be used to reach the
        edge (key) */
    LanesThatSucceedEdgeCont *_succeedinglanes;

    /// the priority normalised for the node the edge is outgoing of
    char     _fromJunctionPriority;

    /// the priority normalised for the node the edge is incoming in
    char     _toJunctionPriority;

    /// the angle of the edge in the node it is outgoing from
    double _fromJunctionAngle;

    /// the angle of the edge in the node it is incoming into
    double _toJunctionAngle;

    // the information what purpose the edge has within a simulation
    EdgeBasicFunction _basicType;

private:

    /** divides the lanes on the outgoing edges */
    void divideOnEdges(const std::vector<NBEdge*> *outgoing);

    /** recomputes the priorities and manipulates them for a distribution
        of lanes on edges which is more like in real-life */
    std::vector<size_t> *preparePriorities(
        const std::vector<NBEdge*> *outgoing);

    /** computes teh sum of the given list's entries (sic!) */
    size_t computePrioritySum(std::vector<size_t> *priorities);

    /** moves a connection one place to the left;
        Attention! no checking for field validity */
    void moveConnectionToLeft(size_t lane);

    /** moves a connection one place to the right;
        Attention! no checking for field validity */
    void moveConnectionToRight(size_t lane);

    /** returns the information about the connection to move and
        removes it from the containers */
    std::pair<NBEdge*, size_t> getConnectionRemoving(size_t srcLane, size_t pos);

    /** writes information about the described lane into the given stream */
    void writeLane(std::ostream &into, size_t lane);

    // !!! describe
    void writeConnected(std::ostream &into, NBEdge *edge, LaneVector &lanes);

    // !!! describe
    void writeSucceeding(std::ostream &into, size_t lane);

    // !!! describe
    void writeSingleSucceeding(std::ostream &into, size_t from, size_t dest);

private:
    /** invalid copy constructor */
    NBEdge(const NBEdge &s);

    /** invalid assignment operator */
    NBEdge &operator=(const NBEdge &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
