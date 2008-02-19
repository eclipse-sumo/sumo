/****************************************************************************/
/// @file    NBEdge.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The representation of a single edge
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
#ifndef NBEdge_h
#define NBEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <vector>
#include <string>
#include <set>
#include "NBCont.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/Bresenham.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class NBNodeCont;
class NBEdgeCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
class NBEdge
{
public:
    /** lane (index) -> list of reachable edges */
    typedef std::vector<EdgeLaneVector> ReachableFromLaneVector;

    /** lane (index) -> list of link priorities */
    typedef std::vector<BoolVector> ReachablePrioritiesFromLaneVector;

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

    /**
     * @enum LaneSpreadFunction
     * In dependence to this value, lanes will be spread to the right side or
     * to both sides from the given edge gemetry. (Also used when node
     * positions are used as edge geometry)
     */
    enum LaneSpreadFunction {
        /// The lanes will be spread to right
        LANESPREAD_RIGHT,
        /// The lanes will be spread to both sides
        LANESPREAD_CENTER
    };

    /**
     * As the network is build in a cascaded way, considering loaded
     * information, a counter holding the current step is needed. This is done
     * by using this enumeration
     */
    enum EdgeBuildingStep {
        /// the edge has been loaded and connections shall not be added
        INIT_REJECT_CONNECTIONS = -1,
        /// the edge has been loaded, nothing is computed yet
        INIT = 0,
        /// the relationships between edges are computed/loaded
        EDGE2EDGES = 1,
        /// lanes to edges - relationships are computed/loaded
        LANES2EDGES = 2,
        /// lanes to lanes - relationships are loaded; no recheck is necessary/wished
        LANES2LANES = 3,
        /// lanes to lanes - relationships are computed; should be recheked
        LANES2LANES_RECHECK = 4
    };


public:
    /// constructor
    NBEdge(const std::string &id, const std::string &name,
           NBNode *from, NBNode *to, std::string type,
           SUMOReal speed, size_t nolanes, int priority,
           LaneSpreadFunction spread=LANESPREAD_RIGHT,
           EdgeBasicFunction basic=EDGEFUNCTION_NORMAL) throw(ProcessError);

    NBEdge(const std::string &id, const std::string &name,
           NBNode *from, NBNode *to, std::string type,
           SUMOReal speed, size_t nolanes, int priority,
           Position2DVector geom, LaneSpreadFunction spread=LANESPREAD_RIGHT,
           EdgeBasicFunction basic=EDGEFUNCTION_NORMAL) throw(ProcessError);

    /// destructor
    ~NBEdge();

    /// computes which edge shall be the turn-around one, if any
    void computeTurningDirections();

    /// returns the angle of the edge
    SUMOReal getAngle();

    /// returns the priority of the edge
    int getPriority() const;

    /// sets the junction priority of the edge
    void setJunctionPriority(NBNode *node, int prio);

    /// returns the junction priority (normalised for the node currently build)
    int getJunctionPriority(NBNode *node);

    /// returns the number of lanes
    size_t getNoLanes() const;

    /// returns the edges approached by the given lane
    const EdgeLaneVector & getEdgeLanesFromLane(size_t lane) const;

    /// returns the id of the edge
    const std::string &getID() const;

    /// returns the name of the edge
    std::string getName() const;

    /// returns the origin node of the edge
    NBNode *getFromNode();

    /// returns the destination node of the edge
    NBNode *getToNode();

    /// returns the type of the edge
    std::string getType();

    /// returns the length of the edge
    SUMOReal getLength();

    /// Returns the geometry of the edge
    const Position2DVector &getGeometry() const;

    /// Sets an edges geometry (!!! not really great)
    void setGeometry(const Position2DVector &s);

    /** writes the edge definition with lanes and connected edges
        into the given stream */
    void writeXMLStep1(OutputDevice &into);

    /** writes the succeeding lane information */
    void writeXMLStep2(OutputDevice &into, bool includeInternal);

    bool hasRestrictions() const;
    void writeLanesPlain(OutputDevice &into);
    void setLoadedLength(SUMOReal val);
    void dismissVehicleClassInformation();


    /** adds a connection to another edge;
        instead of adding connections one by one, they may be computed
        using "computeEdge2Edges"
        returns false when an connection already existed; otherwise true */
    bool addEdge2EdgeConnection(NBEdge *dest);

    /** adds a connection between the specified this edge's lane and an approached one */
    bool addLane2LaneConnection(size_t fromLane, NBEdge *dest,
                                size_t toLane, bool markAs2Recheck,
                                bool mayUseSameDestination=false);

    /** builds no connections starting at the given lanes */
    bool addLane2LaneConnections(size_t fromLane,
                                 NBEdge *dest, size_t toLane, size_t no,
                                 bool markAs2Recheck, bool invalidatePrevious=false);

    /// computes the edge (step1: computation of approached edges)
    bool computeEdge2Edges();

    /// computes the edge, step2: computation of which lanes approach the edges)
    bool computeLanes2Edges();

    /// sorts the connections of outgoing lanes (!!! Kaskade beschreiben)
    void sortOutgoingLanesConnections();

    /** recheck whether all lanes within the edge are all right and
        optimises the connections once again */
    bool recheckLanes();

    /// appends turnarounds
    void appendTurnaround();

    /// returns the list of lanes that may be used to reach the given edge
    std::vector<size_t> getConnectionLanes(NBEdge *currentOutgoing);

    /// adds a connection to a certain lane of a certain edge
    void setConnection(size_t lane, NBEdge *destEdge,
                       size_t destLane, bool markAs2Recheck,
                       bool mayUseSameDestination=false);

    /** returns the information whether the given edge is the opposite
        direction to this edge */
    bool isTurningDirectionAt(const NBNode *n, NBEdge *edge) const;

    bool isAlmostSameDirectionAt(const NBNode *n, NBEdge *edge) const;


    /** @brief Returns the node at the given edges length (using an epsilon)
        When no node is existing at the given position, 0 is returned
        The epsilon is a static member of NBEdge, should be setable via program options */
    NBNode *tryGetNodeAtPosition(SUMOReal pos, SUMOReal tolerance=5.0) const;

    EdgeBasicFunction getBasicType() const;

    SUMOReal getSpeed() const;

    void replaceInConnections(NBEdge *which, NBEdge *by, size_t laneOff);

    SUMOReal getMaxLaneOffset();

    Position2D getMinLaneOffsetPositionAt(NBNode *node, SUMOReal width);
    Position2D getMaxLaneOffsetPositionAt(NBNode *node, SUMOReal width);

    /// Returns the information whethe a connection to the given edge has been added (or computed)
    bool isConnectedTo(NBEdge *e);

    /** returns the list of outgoing edges without the turnaround
        sorted in clockwise direction */
    const std::vector<NBEdge*> *getConnectedSorted();

    /** returns the list of outgoing edges unsorted */
    const EdgeVector &getConnected() const;

    /** @brief Remaps the connection in a way tha allows the removal of it
        This edges (which is a "dummy" edge, in fact) connections are spread over the incoming non-dummy edges */
    void remapConnections(const EdgeVector &incoming);

    EdgeVector getEdgesFromLane(size_t lane) const;

    void removeFromConnections(NBEdge *which, int lane=-1);

    void invalidateConnections(bool reallowSetting=false);

    bool lanesWereAssigned() const;

    /// Returns if the link could be set as to be controlled
    bool setControllingTLInformation(int fromLane, NBEdge *toEdge, int toLane,
                                     const std::string &tlID, size_t tlPos);

    void normalisePosition();

    void reshiftPosition(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    void addCrossingPointsAsIncomingWithGivenOutgoing(NBEdge *o,
            Position2DVector &into);

    SUMOReal width() const;

    Position2DVector getCWBoundaryLine(const NBNode &n, SUMOReal offset) const;
    Position2DVector getCCWBoundaryLine(const NBNode &n, SUMOReal offset) const;

    bool expandableBy(NBEdge *possContinuation) const;
    void append(NBEdge *continuation);
    SUMOReal getNormedAngle() const;

    void computeEdgeShape();

    const Position2DVector &getLaneShape(size_t i) const;

    bool hasSignalisedConnectionTo(NBEdge *e) const;


    /** friend class used for the computation of connections to
        following edges */
    friend class NBEdgeSuccessorBuilder;

    friend class NBEdgeCont;

    EdgeBuildingStep getStep() const {
        return myStep;
    }

    void moveOutgoingConnectionsFrom(NBEdge *e, size_t laneOff,
                                     bool markAs2Recheck);

    NBEdge *getTurnDestination() const;

    typedef std::map<std::string, std::vector<std::string> > StringContMap;

    std::string getLaneID(size_t lane);

    void setLaneSpeed(int lane, SUMOReal speed);

    SUMOReal getLaneSpeed(int lane) const;

    bool isNearEnough2BeJoined2(NBEdge *e);

    SUMOReal getAngle(const NBNode &atNode) const;

    SUMOReal getNormedAngle(const NBNode &atNode) const;

    void addGeometryPoint(int index, const Position2D &p);

    void incLaneNo(int by);

    void decLaneNo(int by, int dir=0);

    void copyConnectionsFrom(NBEdge *src);

    void setLaneSpreadFunction(LaneSpreadFunction spread);

    LaneSpreadFunction getLaneSpreadFunction() const;

    void markAsInLane2LaneState();

    void allowVehicleClass(int lane, SUMOVehicleClass vclass);
    void disallowVehicleClass(int lane, SUMOVehicleClass vclass);
    std::vector<SUMOVehicleClass> getAllowedVehicleClasses() const;
    std::vector<SUMOVehicleClass> getNotAllowedVehicleClasses() const;

    void disableConnection4TLS(int fromLane, NBEdge *toEdge, int toLane);

    void recheckEdgeGeomForDoublePositions();

    void addAdditionalConnections();

    int getMinConnectedLane(NBEdge *of) const;
    int getMaxConnectedLane(NBEdge *of) const;

    void setTurningDestination(NBEdge *e);

    void setIsInnerEdge() {
        myAmInnerEdge = true;
    }

    bool isInnerEdge() const {
        return myAmInnerEdge;
    }

private:
    /**
     * ToEdgeConnectionsAdder
     * A class that being a bresenham-callback assigns the incoming
     * lanes to the edges
     */
class ToEdgeConnectionsAdder : public Bresenham::BresenhamCallBack
    {
    private:
        /// map of edges to this edge's lanes that reach them
        std::map<NBEdge*, std::vector<size_t> > *myConnections;

        /// the transition from the virtual lane to the edge it belongs to
        const std::vector<NBEdge*> &myTransitions;

    public:
        /// constructor
        ToEdgeConnectionsAdder(std::map<NBEdge*,
                               std::vector<size_t> > *connections,
                               const std::vector<NBEdge*> &transitions) throw()
                : myConnections(connections), myTransitions(transitions) { }

        /// destructor
        ~ToEdgeConnectionsAdder() throw() { }

        /// executes a bresenham - step
        void execute(SUMOReal lane, SUMOReal virtEdge) throw();

    private:
        /// @brief Invalidated copy constructor.
        ToEdgeConnectionsAdder(const ToEdgeConnectionsAdder&);

        /// @brief Invalidated assignment operator.
        ToEdgeConnectionsAdder& operator=(const ToEdgeConnectionsAdder&);

    };


    /**
     * MainDirections
     * Holds (- relative to the edge it is build from -!!!) the list of
     * main directions a vehicle that drives on this street may take on
     * the junction the edge ends in
     * The back direction is not regarded
     */
    class MainDirections
    {
    public:
        /// enum of possible directions
        enum Direction { DIR_RIGHTMOST, DIR_LEFTMOST, DIR_FORWARD };

        /** list of the main direction within the following junction relative
            to the edge */
        std::vector<Direction> myDirs;

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

    private:
        /// @brief Invalidated copy constructor.
        MainDirections(const MainDirections&);

        /// @brief Invalidated assignment operator.
        MainDirections& operator=(const MainDirections&);

    };

    /// Computes the shape for the given lane
    Position2DVector computeLaneShape(size_t lane) throw(InvalidArgument);

    /// Computes the offset from the edge shape on the current segment
    std::pair<SUMOReal, SUMOReal> laneOffset(const Position2D &from,
            const Position2D &to, SUMOReal lanewidth, size_t lane) throw(InvalidArgument);

    void computeLaneShapes() throw(InvalidArgument);

    bool splitGeometry(NBEdgeCont &ec, NBNodeCont &nc);
protected:
    bool acceptBeingTurning(NBEdge *e);


private:
    /// the building step
    EdgeBuildingStep myStep;

    /// the id of the edge
    std::string myID;

    /// the type of the edge
    std::string myType;

    /// the number of lanes
    size_t     myNolanes;

    /// the source and the destination node
    NBNode  *myFrom, *myTo;

    /// the length of the edge
    SUMOReal  myLength;

    /// the angle of the edge
    SUMOReal  myAngle;

    /** the converted priority of the edge (needed while the ocmputation of
        the node the edge is incoming in / outgoing of) */
    int     myPriority;

    /// the maximal speed
    SUMOReal  mySpeed;

    /// the name of the edge
    std::string  myName;

    /// information about connected edges
    EdgeVector myConnectedEdges;

    /// information about which edge is approached by which lanes
    std::map<NBEdge*, std::vector<size_t> > myToEdges;

    /// the edge to turn into
    NBEdge *myTurnDestination;

    /** contains the information which lanes of which edges may follow a
        lane (index) */
    ReachableFromLaneVector myReachable;

    /** contains the information which lanes(value) may be used to reach the
        edge (key) */
    LanesThatSucceedEdgeCont mySucceedinglanes;

    /// the priority normalised for the node the edge is outgoing of
    int     myFromJunctionPriority;

    /// the priority normalised for the node the edge is incoming in
    int     myToJunctionPriority;

    /// the information what purpose the edge has within a simulation
    EdgeBasicFunction myBasicType;

    /// An optional geometry for the edge
    Position2DVector myGeom;

    /// The information about how to spread the lanes
    LaneSpreadFunction myLaneSpreadFunction;

    std::vector<Position2DVector> myLaneGeoms;
    std::vector<SUMOReal> myLaneSpeeds;
    std::vector<std::vector<SUMOVehicleClass> > myAllowedOnLanes;
    std::vector<std::vector<SUMOVehicleClass> > myNotAllowedOnLanes;

    SUMOReal myLoadedLength;

    SUMOReal myAmTurningWithAngle;
    NBEdge *myAmTurningOf;
    bool myAmInnerEdge;

    struct TLSDisabledConnection {
        int fromLane;
        NBEdge *to;
        int toLane;
    };

    std::vector<TLSDisabledConnection> myTLSDisabledConnections;

    /**
     * @class tls_disable_finder
     */
    class tls_disable_finder
    {
    public:
        /// constructor
        tls_disable_finder(const TLSDisabledConnection &tpl) : myDefinition(tpl) { }

        bool operator()(const TLSDisabledConnection &e) const {
            if (e.to!=myDefinition.to) {
                return false;
            }
            if (e.fromLane!=myDefinition.fromLane) {
                return false;
            }
            if (e.toLane!=myDefinition.toLane) {
                return false;
            }
            return true;
        }

    private:
        TLSDisabledConnection myDefinition;

    };


private:
    /** initialization routines common to all constructors */
    void init() throw(ProcessError);

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
    void writeLane(OutputDevice &into, size_t lane);

    // !!! describe
    void writeConnected(OutputDevice &into, NBEdge *edge, LaneVector &lanes);

    // !!! describe
    void writeSucceeding(OutputDevice &into, size_t lane,
                         bool includeInternal);

    // !!! describe
    void writeSingleSucceeding(OutputDevice &into,
                               size_t fromlane, size_t destidx, bool includeInternal);

private:
    /** invalid copy constructor */
    NBEdge(const NBEdge &s);

    /** invalid assignment operator */
    NBEdge &operator=(const NBEdge &s);

};


#endif

/****************************************************************************/

