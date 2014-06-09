/****************************************************************************/
/// @file    NBNode.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The representation of a single node
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
#ifndef NBNode_h
#define NBNode_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <deque>
#include <utility>
#include <string>
#include <set>
#include <utils/common/StdDefs.h>
#include <utils/common/Named.h>
#include <utils/geom/Bresenham.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/Line.h>
#include <utils/geom/PositionVector.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "NBEdge.h"
#include "NBConnection.h"
#include "NBConnectionDefs.h"
#include "NBContHelper.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBRequest;
class NBDistrict;
class OptionsCont;
class NBTrafficLightDefinition;
class NBTypeCont;
class NBTrafficLightLogicCont;
class NBDistrictCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNode
 * @brief Represents a single node (junction) during network building
 */
class NBNode : public Named {
    friend class NBNodeCont;
    friend class GNEJunction; // < used for visualization (NETEDIT)
    friend class NBNodesEdgesSorter; // < sorts the edges
    friend class NBNodeTypeComputer; // < computes type
    friend class NBEdgePriorityComputer; // < computes priorities of edges per intersection

public:
    /**
     * @class ApproachingDivider
     * @brief Computes lane-2-lane connections
     *
     * Being a bresenham-callback, this class computes which lanes
     *  are approached by the current lane (first callback parameter).
     * The second callback parameter is the destination lane that is the
     *  middle of the computed lanes.
     * The lanes are spreaded from this middle position both to left and right
     *  but may also be transposed in full when there is not enough space.
     */
    class ApproachingDivider : public Bresenham::BresenhamCallBack {
    private:
        /// @brief The list of edges that approach the current edge
        EdgeVector* myApproaching;

        /// @brief The approached current edge
        NBEdge* myCurrentOutgoing;

        /// @brief The available lanes to which connections shall be built
        std::vector<unsigned int> myAvailableLanes;

    public:
        /** @brief Constructor
         * @param[in] approaching The list of the edges that approach the outgoing edge
         * @param[in] currentOutgoing The outgoing edge
         */
        ApproachingDivider(EdgeVector* approaching,
                           NBEdge* currentOutgoing, const bool buildCrossingsAndWalkingAreas);

        /// @brief Destructor
        ~ApproachingDivider();

        unsigned int numAvailableLanes() const {
            return (unsigned int)myAvailableLanes.size();
        }

        /** the bresenham-callback */
        void execute(const unsigned int src, const unsigned int dest);

        /** the method that spreads the wished number of lanes from the
            the lane given by the bresenham-call to both left and right */
        std::deque<int>* spread(const std::vector<int>& approachingLanes, int dest) const;

    };

    /** @struct Crossing
     * @brief A definition of a pedestrian crossing
     */
    struct Crossing {
        Crossing(const NBNode* _node, const EdgeVector& _edges, SUMOReal _width, bool _priority) :
            node(_node), edges(_edges), width(_width), priority(_priority), tlLinkNo(-1)
        {}
        /// @brief The parent node of this crossing
        const NBNode* node;
        /// @brief The edges being crossed
        EdgeVector edges;
        /// @brief The lane's shape
        PositionVector shape;
        /// @brief This lane's width
        SUMOReal width;
        /// @brief the (edge)-id of this crossing
        std::string id;
        /// @brief the lane-id of the previous walkingArea
        std::string prevWalkingArea;
        /// @brief the lane-id of the next walkingArea
        std::string nextWalkingArea;
        /// @brief whether the pedestrians have priority
        bool priority;
        /// @brief the traffic light index of this crossing (if controlled)
        int tlLinkNo;
    };


    /** @struct WalkingArea
     * @brief A definition of a pedestrian walking area
     */
    struct WalkingArea {
        WalkingArea(const std::string& _id, SUMOReal _width) :
            id(_id),
            width(_width),
            nextCrossing(""),
            tlID("")
        {}
        /// @brief the (edge)-id of this walkingArea
        std::string id;
        /// @brief This lane's width
        SUMOReal width;
        /// @brief This lane's width
        SUMOReal length;
        /// @brief The polygonal shape
        PositionVector shape;
        /// @brief the lane-id of the next crossing
        std::string nextCrossing;
        /// @brief the traffic light id of the next crossing or ""
        std::string tlID;
        /// @brief the lane-id of the next sidewalk lane or ""
        std::vector<std::string> nextSidewalks;
        /// @brief the lane-id of the previous sidewalk lane or ""
        std::vector<std::string> prevSidewalks;
    };

    /// @brief edge directions (for pedestrian related stuff)
    static const int FORWARD;
    static const int BACKWARD;
    /// @brief default width of pedetrian crossings
    static const SUMOReal DEFAULT_CROSSING_WIDTH;


public:
    /// @brief maximum number of connections allowed
    static const int MAX_CONNECTIONS;

    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     * @param[in] type The type of the node
     */
    NBNode(const std::string& id, const Position& position, SumoXMLNodeType type);


    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     * @param[in] district The district this district node represents, 0 means no district node
     */
    NBNode(const std::string& id, const Position& position, NBDistrict* district = 0);


    /// @brief Destructor
    ~NBNode();


    /** @brief Resets initial values
     * @param[in] position The position of the node
     * @param[in] type The type of the node
     * @param[in] updateEdgeGeometries Whether the geometires of all
     *    connected edges shall be updated
     */
    void reinit(const Position& position, SumoXMLNodeType type,
                bool updateEdgeGeometries = false);


    /// @name Atomar getter methods
    /// @{

    /** @brief Returns the position of this node
     * @return This node's position
     */
    const Position& getPosition() const {
        return myPosition;
    }


    /** @brief Returns a position that is guaranteed to lie within the node shape */
    Position getCenter() const;


    /** @brief Returns this node's incoming edges
     * @return The edges which yield in this node
     */
    const EdgeVector& getIncomingEdges() const {
        return myIncomingEdges;
    }


    /** @brief Returns this node's outgoing edges
     * @return The edges which start at this node
     */
    const EdgeVector& getOutgoingEdges() const {
        return myOutgoingEdges;
    }


    /** @brief Returns all edges which participate in this node
     * @return Edges that start or end at this node
     */
    const EdgeVector& getEdges() const {
        return myAllEdges;
    }


    /** @brief Returns the type of this node
     * @return The type of this node
     * @see SumoXMLNodeType
     */
    SumoXMLNodeType getType() const {
        return myType;
    }
    /// @}



    /// @name Methods for dealing with assigned traffic lights
    /// @{

    /** @brief Adds a traffic light to the list of traffic lights that control this node
     * @param[in] tld The traffic light that controls this node
     */
    void addTrafficLight(NBTrafficLightDefinition* tlDef);

    /** @brief Removes the given traffic light from this node */
    void removeTrafficLight(NBTrafficLightDefinition* tlDef);

    /** @brief Removes all references to traffic lights that control this tls
     */
    void removeTrafficLights();


    /** @brief Returns whether this node is controlled by any tls
     * @return Whether a traffic light was assigned to this node
     */
    bool isTLControlled() const {
        return myTrafficLights.size() != 0;
    }


    /** @brief Returns whether this node is controlled by a tls that spans over more than one node
     * @return Whether a "joined" traffic light was assigned to this node
     */
    bool isJoinedTLSControlled() const;


    /** @brief Returns the traffic lights that were assigned to this node
     * @return The set of tls that control this node
     */
    const std::set<NBTrafficLightDefinition*>& getControllingTLS() const {
        return myTrafficLights;
    }


    /// @brief causes the traffic light to be computed anew
    void invalidateTLS(NBTrafficLightLogicCont& tlCont);

    /// @brief patches loaded signal plans by modifying lane indices
    void shiftTLConnectionLaneIndex(NBEdge* edge, int offset);
    /// @}



    /// @name Prunning the input
    /// @{

    /** @brief Removes edges which are both incoming and outgoing into this node
     *
     * If given, the connections to other edges participating in this node are updated
     *
     * @param[in, opt. changed] dc The districts container to update
     * @param[in, opt. changed] ec The edge container to remove the edges from
     * @param[in, opt. changed] tc The traffic lights container to update
     * @return The number of removed edges
     */
    unsigned int removeSelfLoops(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tc);
    /// @}



    /// @name Applying offset
    /// @{

    /** @brief Applies an offset to the node
     * @param[in] xoff The x-offset to apply
     * @param[in] yoff The y-offset to apply
     */
    void reshiftPosition(SUMOReal xoff, SUMOReal yoff);
    /// @}



    /// adds an incoming edge
    void addIncomingEdge(NBEdge* edge);

    /// adds an outgoing edge
    void addOutgoingEdge(NBEdge* edge);




    /// computes the connections of lanes to edges
    void computeLanes2Lanes(const bool buildCrossingsAndWalkingAreas);

    /// computes the node's type, logic and traffic light
    void computeLogic(const NBEdgeCont& ec, OptionsCont& oc);

    /** writes the XML-representation of the logic as a bitset-logic
        XML representation */
    bool writeLogic(OutputDevice& into, const bool checkLaneFoes) const;

    /** @brief Returns something like the most unused direction
        Should only be used to add source or sink nodes */
    Position getEmptyDir() const;


    /** @brief Returns whether the given edge ends at this node
     * @param[in] e The edge
     * @return Whether the given edge is one of this node's incoming edges
     */
    bool hasIncoming(const NBEdge* const e) const;


    /** @brief Returns whether the given edge starts at this node
     * @param[in] e The edge
     * @return Whether the given edge is one of this node's outgoing edges
     */
    bool hasOutgoing(const NBEdge* const e) const;


    NBEdge* getOppositeIncoming(NBEdge* e) const;
    void invalidateIncomingConnections();
    void invalidateOutgoingConnections();

    void removeDoubleEdges();
    NBEdge* getConnectionTo(NBNode* n) const;


    void addSortedLinkFoes(const NBConnection& mayDrive,
                           const NBConnection& mustStop);

    NBEdge* getPossiblySplittedIncoming(const std::string& edgeid);
    NBEdge* getPossiblySplittedOutgoing(const std::string& edgeid);

    /** @brief Removes edge from this node and optionally removes connections as well
     */
    void removeEdge(NBEdge* edge, bool removeFromConnections = true);

    /** @brief Computes whether the given connection is a left mover across the junction
     *
     * It is assumed, that it is a left-mover if the clockwise angle is lower
     *  than the counter-clockwise angle.
     *
     * @param[in] from The incoming edge (the begin of the connection)
     * @param[in] from The outgoing edge (the end of the connection)
     * @return Whether the described connection is a left-mover
     */
    bool isLeftMover(const NBEdge* const from, const NBEdge* const to) const;


    /** @brief Returns the information whether the described flow must let any other flow pass
     * @param[in] from The connection's start edge
     * @param[in] to The connection's end edge
     * @param[in] toLane The lane the connection ends at
     * @return Whether the described connection must brake (has higher priorised foes)
     */
    bool mustBrake(const NBEdge* const from, const NBEdge* const to, int toLane) const;

    /** @brief Returns the information whether the described flow must brake for the given crossing
     * @param[in] from The connection's start edge
     * @param[in] to The connection's end edge
     * @param[in] crossing The pedestrian crossing to check
     * @return Whether the described connection must brake (has higher priorised foes)
     */
    bool mustBrakeForCrossing(const NBEdge* const from, const NBEdge* const to, const Crossing& crossing) const;

    /** @brief Returns the information whether "prohibited" flow must let "prohibitor" flow pass
     * @param[in] possProhibitedFrom The maybe prohibited connection's begin
     * @param[in] possProhibitedTo The maybe prohibited connection's end
     * @param[in] possProhibitorFrom The maybe prohibiting connection's begin
     * @param[in] possProhibitorTo The maybe prohibiting connection's end
     * @param[in] regardNonSignalisedLowerPriority Whether the right of way rules without traffic lights shall be regarded
     * @return Whether the second flow prohibits the first one
     */
    bool forbids(const NBEdge* const possProhibitorFrom, const NBEdge* const possProhibitorTo,
                 const NBEdge* const possProhibitedFrom, const NBEdge* const possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const;


    /** @brief Returns the information whether the given flows cross
     * @param[in] from1 The starting edge of the first stream
     * @param[in] to1 The ending edge of the first stream
     * @param[in] from2 The starting edge of the second stream
     * @param[in] to2 The ending edge of the second stream
     * @return Whether both stream are foes (cross)
     */
    bool foes(const NBEdge* const from1, const NBEdge* const to1,
              const NBEdge* const from2, const NBEdge* const to2) const;


    /** @brief Returns the representation of the described stream's direction
     * @param[in] incoming The edge the stream starts at
     * @param[in] outgoing The edge the stream ends at
     * @return The direction of the stream
     */
    LinkDirection getDirection(const NBEdge* const incoming, const NBEdge* const outgoing) const;

    LinkState getLinkState(const NBEdge* incoming, NBEdge* outgoing,
                           int fromLane, bool mayDefinitelyPass, const std::string& tlID) const;

    /** @brief Compute the junction shape for this node
     * @param[in] lefhand Whether the network uses left-hand traffic
     * @param[in] mismatchThreshold The threshold for warning about shapes which are away from myPosition
     */
    void computeNodeShape(bool leftHand, SUMOReal mismatchThreshold);

    /// @brief retrieve the junction shape
    const PositionVector& getShape() const;

    /// @brief set the junction shape
    void setCustomShape(const PositionVector& shape);

    /// @brief return whether the shape was set by the user
    bool hasCustomShape() {
        return myHaveCustomPoly;
    }

    bool checkIsRemovable() const;


    std::vector<std::pair<NBEdge*, NBEdge*> > getEdgesToJoin() const;


    friend class NBNodeShapeComputer;

    bool isNearDistrict() const;
    bool isDistrict() const;

    bool needsCont(NBEdge* fromE, NBEdge* toE, NBEdge* otherFromE, NBEdge* otherToE, const NBEdge::Connection& c) const;

    /** @brief Compute the shape for an internal lane
     * @param[in] fromE The starting edge
     * @param[in] fromL The index of the starting lane
     * @param[in] toE The destination edge
     * @param[in] toL The index of the destination lane
     * @param[in] numPoints The number of geometry points for the internal lane
     * @return The shape of the internal lane
     */
    PositionVector computeInternalLaneShape(
        NBEdge* fromE, int fromL, NBEdge* toE, int toL, int numPoints = 5) const;


    /** @brief Replaces occurences of the first edge within the list of incoming by the second
        Connections are remapped, too */
    void replaceIncoming(NBEdge* which, NBEdge* by, unsigned int laneOff);

    /** @brief Replaces occurences of every edge from the given list within the list of incoming by the second
        Connections are remapped, too */
    void replaceIncoming(const EdgeVector& which, NBEdge* by);

    /** @brief Replaces occurences of the first edge within the list of outgoing by the second
        Connections are remapped, too */
    void replaceOutgoing(NBEdge* which, NBEdge* by, unsigned int laneOff);

    /** @brief Replaces occurences of every edge from the given list within the list of outgoing by the second
        Connections are remapped, too */
    void replaceOutgoing(const EdgeVector& which, NBEdge* by);

    /// @brief guess pedestrian crossings and return how many were guessed
    int guessCrossings();

    /* @brief check whether a crossing should be build for the candiate edges and build 0 to n crossings
     * @param[in] candidates The candidate vector of edges to be crossed
     * @return The number of crossings built
     * */
    int checkCrossing(EdgeVector candidates);

    /// @brief build internal lanes, pedestrian crossings and walking areas
    void buildInnerEdges(bool buildCrossingsAndWalkingAreas);

    /* @brief build pedestrian crossings
     * @return The next index for creating internal lanes
     * */
    unsigned int buildCrossings();

    /* @brief build pedestrian walking areas and set connections from/to walkingAreas
     * @param[in] index The starting index for naming the created internal lanes
     * @param[in] tlIndex The starting traffic light index to assign to connections to controlled crossings
     * */
    void buildWalkingAreas();

    /// @brief return all edges that lie clockwise between the given edges
    EdgeVector edgesBetween(const NBEdge* e1, const NBEdge* e2) const;

    /// @brief return true if the given edges are connected by a crossing
    bool crossingBetween(const NBEdge* e1, const NBEdge* e2) const;

    const NBConnectionProhibits& getProhibitions() {
        return myBlockedConnections;
    }

    /** @brief whether this is structurally similar to a geometry node
     */
    bool geometryLike() const;

    /// @brief update the type of this node as a roundabout
    void setRoundabout();

    /// @brief add a pedestrian crossing to this node
    void addCrossing(EdgeVector edges, SUMOReal width, bool priority);

    /// @brief return this junctions pedestrian crossings
    inline const std::vector<Crossing>& getCrossings() const {
        return myCrossings;
    }

    /// @brief return this junctions pedestrian walking areas
    inline const std::vector<WalkingArea>& getWalkingAreas() const {
        return myWalkingAreas;
    }

    /// @brief return the crossing with the given id
    const Crossing& getCrossing(const std::string& id) const;

    /// @brief set tl indices of this nodes crossing starting at the given index
    void setCrossingTLIndices(unsigned int startIndex);

    /// @brief return the number of lane-to-lane connections at this junction (excluding crossings)
    int numNormalConnections() const;

    /**
     * @class nodes_by_id_sorter
     * @brief Used for sorting the cells by the begin time they describe
     */
    class nodes_by_id_sorter {
    public:
        /// @brief Constructor
        explicit nodes_by_id_sorter() { }

        /** @brief Comparing operator
         */
        int operator()(NBNode* n1, NBNode* n2) const {
            return n1->getID() < n2->getID();
        }

    };


    /** @class edge_by_direction_sorter
     * @brief Sorts outgoing before incoming edges
     */
    class edge_by_direction_sorter {
    public:
        explicit edge_by_direction_sorter(NBNode* n) : myNode(n) {}
        int operator()(NBEdge* e1, NBEdge* e2) const {
            UNUSED_PARAMETER(e2);
            return e1->getFromNode() == myNode;
        }

    private:
        /// @brief The node to compute the relative angle of
        NBNode* myNode;

    };

private:
    bool isSimpleContinuation() const;

    /// sets the priorites in case of a priority junction
    void setPriorityJunctionPriorities();

    /** returns a list of edges which are connected to the given
        outgoing edge */
    EdgeVector* getEdgesThatApproach(NBEdge* currentOutgoing);




    void replaceInConnectionProhibitions(NBEdge* which, NBEdge* by,
                                         unsigned int whichLaneOff, unsigned int byLaneOff);


    void remapRemoved(NBTrafficLightLogicCont& tc,
                      NBEdge* removed, const EdgeVector& incoming, const EdgeVector& outgoing);


private:
    /// @brief The position the node lies at
    Position myPosition;

    /// @brief Vector of incoming edges
    EdgeVector myIncomingEdges;

    /// @brief Vector of outgoing edges
    EdgeVector myOutgoingEdges;

    /// @brief Vector of incoming and outgoing edges
    EdgeVector myAllEdges;

    /// @brief Vector of crossings
    std::vector<Crossing> myCrossings;

    /// @brief Vector of walking areas
    std::vector<WalkingArea> myWalkingAreas;

    /// @brief The type of the junction
    SumoXMLNodeType myType;

    /** The container for connection block dependencies */
    NBConnectionProhibits myBlockedConnections;

    /// The district the node is the centre of
    NBDistrict* myDistrict;

    /// the (outer) shape of the junction
    PositionVector myPoly;

    /// @brief whether this nodes shape was set by the user
    bool myHaveCustomPoly;

    NBRequest* myRequest;

    std::set<NBTrafficLightDefinition*> myTrafficLights;

private:
    /// @brief invalidated copy constructor
    NBNode(const NBNode& s);

    /// @brief invalidated assignment operator
    NBNode& operator=(const NBNode& s);


};


#endif

/****************************************************************************/

