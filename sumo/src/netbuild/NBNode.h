/****************************************************************************/
/// @file    NBNode.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The representation of a single node
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "NBEdge.h"
#include "NBJunctionLogicCont.h"
#include "NBConnection.h"
#include "NBConnectionDefs.h"
#include "NBContHelper.h"
#include "NBMMLDirections.h"
#include <utils/geom/Bresenham.h>


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
class NBNode {
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
        std::vector<NBEdge*> *myApproaching;

        /// @brief The approached current edge
        NBEdge *myCurrentOutgoing;

    public:
        /** @brief Constructor
         * @param[in] approaching The list of the edges that approach the outgoing edge
         * @param[in] currentOutgoing The outgoing edge
         */
        ApproachingDivider(std::vector<NBEdge*> *approaching,
                           NBEdge *currentOutgoing) throw();

        /// @brief Destructor
        ~ApproachingDivider() throw();

        /** the bresenham-callback */
        void execute(const unsigned int src, const unsigned int dest) throw();

        /** the method that spreads the wished number of lanes from the
            the lane given by the bresenham-call to both left and right */
        std::deque<int> *spread(const std::vector<int> &approachingLanes,
                                int dest) const;

    };

public:
    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     */
    NBNode(const std::string &id, const Position2D &position) throw();


    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     * @param[in] type The type of the node
     */
    NBNode(const std::string &id, const Position2D &position, SumoXMLNodeType type) throw();


    /** @brief Constructor
     * @param[in] id The id of the node
     * @param[in] position The position of the node
     * @param[in] district The district this district node represents
     */
    NBNode(const std::string &id, const Position2D &position, NBDistrict *district) throw();


    /// @brief Destructor
    ~NBNode() throw();


    /** @brief Resets initial values
     * @param[in] position The position of the node
     * @param[in] type The type of the node
     */
    void reinit(const Position2D &position, SumoXMLNodeType type) throw();


    /// @name Atomar getter methods
    /// @{

    /** @brief Returns the id of the node
     * @return The id of this node
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns the position of this node
     * @return This node's position
     */
    const Position2D &getPosition() const throw() {
        return myPosition;
    }


    /** @brief Returns this node's incoming edges
     * @return The edges which yield in this node
     */
    const EdgeVector &getIncomingEdges() const throw() {
        return *myIncomingEdges;
    }


    /** @brief Returns this node's outgoing edges
     * @return The edges which start at this node
     */
    const EdgeVector &getOutgoingEdges() const throw() {
        return *myOutgoingEdges;
    }


    /** @brief Returns all edges which participate in this node
     * @return Edges that start or end at this node
     */
    const EdgeVector &getEdges() const throw() {
        return myAllEdges;
    }


    /** @brief Returns the type of this node
     * @return The type of this node
     * @see SumoXMLNodeType
     */
    SumoXMLNodeType getType() const throw() {
        return myType;
    }
    /// @}



    /// @name Methods for dealing with assigned traffic lights
    /// @{

    /** @brief Adds a traffic light to the list of traffic lights that control this node
     * @param[in] tld The traffic light that controls this node
     */
    void addTrafficLight(NBTrafficLightDefinition *tld) throw();


    /** @brief Removes all references to traffic lights that control this tls
     */
    void removeTrafficLights() throw();


    /** @brief Returns whether this node is controlled by any tls
     * @return Whether a traffic light was assigned to this node
     */
    bool isTLControlled() const throw() {
        return myTrafficLights.size()!=0;
    }


    /** @brief Returns whether this node is controlled by a tls that spans over more than one node
     * @return Whether a "joined" traffic light was assigned to this node
     */
    bool isJoinedTLSControlled() const throw();


    /** @brief Returns the traffic lights that were assigned to this node
     * @return The set of tls that control this node
     */
    const std::set<NBTrafficLightDefinition*> &getControllingTLS() const throw() {
        return myTrafficLights;
    }
    /// @}



    /// adds an incoming edge
    void addIncomingEdge(NBEdge *edge);

    /// adds an outgoing edge
    void addOutgoingEdge(NBEdge *edge);

    bool writeXMLInternalLinks(OutputDevice &into);
    bool writeXMLInternalSuccInfos(OutputDevice &into);
    bool writeXMLInternalNodes(OutputDevice &into);



    /// prints the junction
    void writeXML(OutputDevice &into);

    /// computes the connections of lanes to edges
    void computeLanes2Lanes();

    /// computes the node's type, logic and traffic light
    void computeLogic(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
                      OptionsCont &oc);

    /** initialises the list of all edges and sorts all edges */
    void sortNodesEdges(bool leftHand, const NBTypeCont &tc);

    /** @brief Returns something like the most unused direction
        Should only be used to add source or sink nodes */
    Position2D getEmptyDir() const;


    /** @brief Returns whether the given edge ends at this node
     * @param[in] e The edge
     * @return Whether the given edge is one of this node's incoming edges
     */
    bool hasIncoming(const NBEdge * const e) const throw();


    /** @brief Returns whether the given edge starts at this node
     * @param[in] e The edge
     * @return Whether the given edge is one of this node's outgoing edges
     */
    bool hasOutgoing(const NBEdge * const e) const throw();


    NBEdge *getOppositeIncoming(NBEdge *e) const;
    void invalidateIncomingConnections();
    void invalidateOutgoingConnections();

    void removeDoubleEdges();
    NBEdge *getConnectionTo(NBNode *n) const;


    void addSortedLinkFoes(const NBConnection &mayDrive,
                           const NBConnection &mustStop);

    NBEdge *getPossiblySplittedIncoming(const std::string &edgeid);
    NBEdge *getPossiblySplittedOutgoing(const std::string &edgeid);

    unsigned int eraseDummies(NBDistrictCont &dc, NBEdgeCont &ec,
                              NBTrafficLightLogicCont &tc);

    void removeOutgoing(NBEdge *edge);
    void removeIncoming(NBEdge *edge);

    /** @brief Computes whether the given connection is a left mover across the junction
     *
     * It is assumed, that it is a left-mover if the clockwise angle is lower
     *  than the counter-clockwise angle.
     *
     * @param[in] from The incoming edge (the begin of the connection)
     * @param[in] from The outgoing edge (the end of the connection)
     * @return Whether the described connection is a left-mover
     */
    bool isLeftMover(const NBEdge * const from, const NBEdge * const to) const throw();


    /** @brief Returns the information whether the described flow must let any other flow pass
     * @param[in] from The connection's start edge
     * @param[in] to The connection's end edge
     * @param[in] toLane The lane the connection ends at
     * @return Whether the described connection must brake (has higher priorised foes)
     */
    bool mustBrake(const NBEdge * const from, const NBEdge * const to, int toLane) const throw();


    /** @brief Returns the information whether "prohibited" flow must let "prohibitor" flow pass
     * @param[in] possProhibitedFrom The maybe prohibited connection's begin
     * @param[in] possProhibitedTo The maybe prohibited connection's end
     * @param[in] possProhibitorFrom The maybe prohibiting connection's begin
     * @param[in] possProhibitorTo The maybe prohibiting connection's end
     * @param[in] regardNonSignalisedLowerPriority Whether the right of way rules without traffic lights shall be regarded
     * @return Whether the second flow prohibits the first one
     */
    bool forbids(const NBEdge * const possProhibitorFrom, const NBEdge * const possProhibitorTo,
                 const NBEdge * const possProhibitedFrom, const NBEdge * const possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const throw();


    /** @brief Returns the information whether the given flows cross
     * @param[in] from1 The starting edge of the first stream
     * @param[in] to1 The ending edge of the first stream
     * @param[in] from2 The starting edge of the second stream
     * @param[in] to2 The ending edge of the second stream
     * @return Whether both stream are foes (cross)
     */
    bool foes(const NBEdge * const from1, const NBEdge * const to1,
              const NBEdge * const from2, const NBEdge * const to2) const throw();


    /** @brief Returns the MML-representation of the described stream's direction
     * @param[in] incoming The edge the stream starts at
     * @param[in] outgoing The edge the stream ends at
     * @return The direction of the stream
     */
    NBMMLDirection getMMLDirection(const NBEdge * const incoming, const NBEdge * const outgoing) const throw();

    char stateCode(NBEdge *incoming, NBEdge *outgoing, int fromLane, bool mayDefinitelyPass) const throw();

    void computeNodeShape(bool leftHand);

    friend class NBNodeCont;


    const Position2DVector &getShape() const;

    // @brief adds up all lanes of all incoming edges which have a continuation
    unsigned int countInternalLanes(bool includeSplits);

    std::string getInternalLaneID(NBEdge *from, unsigned int fromlane,
                                  NBEdge *to, unsigned int tolane) const;


    bool checkIsRemovable() const;


    std::vector<std::pair<NBEdge*, NBEdge*> > getEdgesToJoin() const;

    SUMOReal getMaxEdgeWidth() const;

    friend class NBNodeShapeComputer;

    bool isNearDistrict() const;
    bool isDistrict() const;


    std::pair<SUMOReal, std::vector<unsigned int> > getCrossingPosition(NBEdge *fromE, unsigned int fromL,
            NBEdge *toE, unsigned int toL);
    std::string getCrossingNames_dividedBySpace(NBEdge *fromE, unsigned int fromL,
            NBEdge *toE, unsigned int toL);
    std::string getCrossingSourcesNames_dividedBySpace(NBEdge *fromE, unsigned int fromL,
            NBEdge *toE, unsigned int toL);

    /** @brief Replaces occurences of the first edge within the list of incoming by the second
        Connections are remapped, too */
    void replaceIncoming(NBEdge *which, NBEdge *by, unsigned int laneOff);

    /** @brief Replaces occurences of every edge from the given list within the list of incoming by the second
        Connections are remapped, too */
    void replaceIncoming(const EdgeVector &which, NBEdge *by);

    /** @brief Replaces occurences of the first edge within the list of outgoing by the second
        Connections are remapped, too */
    void replaceOutgoing(NBEdge *which, NBEdge *by, unsigned int laneOff);

    /** @brief Replaces occurences of every edge from the given list within the list of outgoing by the second
        Connections are remapped, too */
    void replaceOutgoing(const EdgeVector &which, NBEdge *by);


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
        int operator()(NBNode *n1, NBNode *n2) const {
            return n1->getID()<n2->getID();
        }

    };

private:
    /// build the logic using the NBRequest
    void buildBitfieldLogic();

    /** sorts edges with same direction (other direction lanes) in a way that
        the outgoing lanes are "earlier" in the list */
    void sortSmall();

    // computes the junction type
    SumoXMLNodeType computeType(const NBTypeCont &tc) const;
    bool isSimpleContinuation() const;

    /// computes the priorities of participating edges within this junction
    void setPriorities();

    /// sets the priorites in case of a priority junction
    void setPriorityJunctionPriorities();

    /** used while fine sorting the incoming and outgoing edges, this method
        performs the swapping of two edges in the myAllEdges-list when the
        outgoing is in clockwise direction to the incoming */
    bool swapWhenReversed(bool leftHand,
                          const std::vector<NBEdge*>::iterator &i1,
                          const std::vector<NBEdge*>::iterator &i2);

    /** removes the first edge from the list, marks it as higher priorised and
        returns it */
    NBEdge* extractAndMarkFirst(std::vector<NBEdge*> &s);

    /** returns a list of edges which are connected to the given
        outgoing edge */
    std::vector<NBEdge*> *getEdgesThatApproach(NBEdge *currentOutgoing);

    /// resets the position by the given amount
    void reshiftPosition(SUMOReal xoff, SUMOReal yoff);



    void replaceInConnectionProhibitions(NBEdge *which, NBEdge *by,
                                         unsigned int whichLaneOff, unsigned int byLaneOff);


    void remapRemoved(NBTrafficLightLogicCont &tc,
                      NBEdge *removed, const EdgeVector &incoming, const EdgeVector &outgoing);

    Position2DVector computeInternalLaneShape(NBEdge *fromE,
            int fromL, NBEdge *toE, int toL);

    void writeinternal(EdgeVector *myIncomingEdges, OutputDevice &into, const std::string &id);

private:
    /// @brief The id of the node
    std::string myID;

    /// @brief The position the node lies at
    Position2D myPosition;

    /// @brief Vector of incoming edges
    std::vector<NBEdge*> *myIncomingEdges;

    /// @brief Vector of outgoing edges
    std::vector<NBEdge*> *myOutgoingEdges;

    /// @brief Vector of incoming and outgoing edges
    std::vector<NBEdge*> myAllEdges;

    /// @brief The type of the junction
    SumoXMLNodeType myType;

    /** The container for connection block dependencies */
    NBConnectionProhibits myBlockedConnections;

    /// The district the node is the centre of
    NBDistrict *myDistrict;

    /// the (outer) shape of the junction
    Position2DVector myPoly;

    NBRequest *myRequest;

    std::set<NBTrafficLightDefinition*> myTrafficLights;

private:
    /// @brief invalidated copy constructor
    NBNode(const NBNode &s);

    /// @brief invalidated assignment operator
    NBNode &operator=(const NBNode &s);


};


#endif

/****************************************************************************/

