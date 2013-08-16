/****************************************************************************/
/// @file    NBNode.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Yun-Pang Wang
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The representation of a single node
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

    public:
        /** @brief Constructor
         * @param[in] approaching The list of the edges that approach the outgoing edge
         * @param[in] currentOutgoing The outgoing edge
         */
        ApproachingDivider(EdgeVector* approaching,
                           NBEdge* currentOutgoing);

        /// @brief Destructor
        ~ApproachingDivider();

        /** the bresenham-callback */
        void execute(const unsigned int src, const unsigned int dest);

        /** the method that spreads the wished number of lanes from the
            the lane given by the bresenham-call to both left and right */
        std::deque<int>* spread(const std::vector<int>& approachingLanes, int dest) const;

    };

public:
    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     */
    NBNode(const std::string& id, const Position& position);


    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     * @param[in] type The type of the node
     */
    NBNode(const std::string& id, const Position& position, SumoXMLNodeType type);


    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     * @param[in] district The district this district node represents
     */
    NBNode(const std::string& id, const Position& position, NBDistrict* district);


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
    void computeLanes2Lanes();

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

    void computeNodeShape(bool leftHand);


    const PositionVector& getShape() const;

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

    void buildInnerEdges();

    const NBConnectionProhibits& getProhibitions() {
        return myBlockedConnections;
    }

    /** @brief whether this is structurally similar to a geometry node
     */
    bool geometryLike() const;

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

    /// @brief The type of the junction
    SumoXMLNodeType myType;

    /** The container for connection block dependencies */
    NBConnectionProhibits myBlockedConnections;

    /// The district the node is the centre of
    NBDistrict* myDistrict;

    /// the (outer) shape of the junction
    PositionVector myPoly;

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

