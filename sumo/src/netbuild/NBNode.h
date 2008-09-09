/****************************************************************************/
/// @file    NBNode.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The representation of a single node
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
 *  The class for a single node (junction)
 */
class NBNode
{
public:
    /**
     * ApproachingDivider
     * Being a bresenham-callback, this class computes which lanes
     * are approached by the current lane (first callback parameter)
     * The second callback parameter is the destination lane that is the
     * middle of the computed lanes.
     * The lanes are spreaded from this middle position both to left and right
     * but may also be transposed in full when there is not enough space.
     */
class ApproachingDivider : public Bresenham::BresenhamCallBack
    {
    private:
        /// the list of edges that approach the current edge
        std::vector<NBEdge*> *myApproaching;

        /// the approached current edge
        NBEdge *myCurrentOutgoing;

    public:
        /// constructor
        ApproachingDivider(std::vector<NBEdge*> *approaching,
                           NBEdge *currentOutgoing) throw();

        /// destructor
        ~ApproachingDivider() throw();

        /** the bresenham-callback */
        void execute(SUMOReal src, SUMOReal dest) throw();

        /** the method that spreads the wished number of lanes from the
            the lane given by the bresenham-call to both left and right */
        std::deque<int> *spread(const std::vector<int> &approachingLanes,
                                SUMOReal dest) const;

    };

    enum BasicNodeType {
        /// Unknown yet
        NODETYPE_UNKNOWN,
        /** internal type for a tls-controlled junction */
        NODETYPE_TRAFFIC_LIGHT,
        /** internal type for a priority-junction */
        NODETYPE_PRIORITY_JUNCTION,
        /** internal type for a right-before-left junction */
        NODETYPE_RIGHT_BEFORE_LEFT,
        /** internal type for a district junction */
        NODETYPE_DISTRICT,
        /** internal type for no-junction */
        NODETYPE_NOJUNCTION,
        /** internal type for a dead-end junction */
        NODETYPE_DEAD_END
    };

    /** a counter for the no-junctions build */
    static int myNoDistricts;

    /** a counter for the no-junctions build */
    static int myNoUnregulatedJunctions;

    /** a counter for priority-junctions build */
    static int myNoPriorityJunctions;

    /** a counter for right-before-left-junctions build */
    static int myNoRightBeforeLeftJunctions;

    friend class NBEdgeCont;

public:
    /** constructor */
    NBNode(const std::string &id, const Position2D &position);

    /** constructor */
    NBNode(const std::string &id, const Position2D &position, BasicNodeType type);

    /** constructor */
    NBNode(const std::string &id, const Position2D &position, NBDistrict *district);

    /** destructor */
    ~NBNode();

    Position2D getPosition() const;

    /** returns the id of the node */
    const std::string &getID() const;

    /// adds an incoming edge
    void addIncomingEdge(NBEdge *edge);

    /// adds an outgoing edge
    void addOutgoingEdge(NBEdge *edge);

    /// returns the list of the ids of the incoming edges
    const EdgeVector &getIncomingEdges() const;

    /// returns the list of the ids of the outgoing edges
    const EdgeVector &getOutgoingEdges() const;

    /// returns the list of all edgs
    const EdgeVector &getEdges() const;

    void writeXMLInternalLinks(OutputDevice &into);
    void writeXMLInternalSuccInfos(OutputDevice &into);
    void writeXMLInternalNodes(OutputDevice &into);



    /// prints the junction
    void writeXML(OutputDevice &into);

    /// computes the connections of lanes to edges
    void computeLanes2Lanes();

    /// computes the node's type, logic and traffic light
    void computeLogic(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
                      OptionsCont &oc);

    /** initialises the list of all edges and sorts all edges */
    void sortNodesEdges(const NBTypeCont &tc);

    /** reports about the build junctions */
    static void reportBuild();

    /** @brief Returns something like the most unused direction
        Should only be used to add source or sink nodes */
    Position2D getEmptyDir() const;

    bool hasOutgoing(NBEdge *e) const;
    bool hasIncoming(NBEdge *e) const;
    NBEdge *getOppositeIncoming(NBEdge *e) const;
    void invalidateIncomingConnections();
    void invalidateOutgoingConnections();

    void removeDoubleEdges();
    NBEdge *getConnectionTo(NBNode *n) const;


    void addSortedLinkFoes(const NBConnection &mayDrive,
                           const NBConnection &mustStop);

    NBEdge *getPossiblySplittedIncoming(const std::string &edgeid);
    NBEdge *getPossiblySplittedOutgoing(const std::string &edgeid);

    size_t eraseDummies(NBDistrictCont &dc, NBEdgeCont &ec,
                        NBTrafficLightLogicCont &tc);

    void removeOutgoing(NBEdge *edge);
    void removeIncoming(NBEdge *edge);

    /** sets the type of the junction */
    void setType(BasicNodeType type);

    BasicNodeType getType() const;


    bool isLeftMover(NBEdge *from, NBEdge *to) const;


    bool mustBrake(NBEdge *from, NBEdge *to, int toLane) const;

    bool forbids(NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
                 NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const;

    bool foes(NBEdge *from1, NBEdge *to1, NBEdge *from2, NBEdge *to2) const;

    void addTrafficLight(NBTrafficLightDefinition *tld);

    NBMMLDirection getMMLDirection(NBEdge *incoming, NBEdge *outgoing) const;

    char stateCode(NBEdge *incoming, NBEdge *outgoing, int fromLane);

    void computeNodeShape();

    friend class NBNodeCont;


    SUMOReal getOffset(Position2DVector on, Position2DVector cross) const;

    const Position2DVector &getShape() const;

    size_t countInternalLanes(bool includeSplits);

    std::string getInternalLaneID(NBEdge *from, size_t fromlane,
                                  NBEdge *to, size_t tolane) const;


    bool checkIsRemovable() const;

    bool isTLControlled() const;

    std::vector<std::pair<NBEdge*, NBEdge*> > getEdgesToJoin() const;

    SUMOReal getMaxEdgeWidth() const;

    friend class NBNodeShapeComputer;

    bool isNearDistrict() const;
    bool isDistrict() const;


    std::pair<SUMOReal, std::vector<size_t> > getCrossingPosition(NBEdge *fromE, size_t fromL,
            NBEdge *toE, size_t toL);
    std::string getCrossingNames_dividedBySpace(NBEdge *fromE, size_t fromL,
            NBEdge *toE, size_t toL);
    std::string getCrossingSourcesNames_dividedBySpace(NBEdge *fromE, size_t fromL,
            NBEdge *toE, size_t toL);

    const std::set<NBTrafficLightDefinition*> &getControllingTLS() const {
        return myTrafficLights;
    }

private:


    /// sets the computed non - permutating key
    void setKey(std::string key);

    /// build the logic using the NBRequest
    void buildMapLogic();

    /// build the logic using the NBRequest
    void buildBitfieldLogic();

    /** sorts edges with same direction (other direction lanes) in a way that
        the outgoing lanes are "earlier" in the list */
    void sortSmall();

    // computes the junction type
    BasicNodeType computeType(const NBTypeCont &tc) const;
    bool isSimpleContinuation() const;

    /// computes the priorities of participating edges within this junction
    void setPriorities();

    /// sets the priorites in case of a priority junction
    void setPriorityJunctionPriorities();

    /** used while fine sorting the incoming and outgoing edges, this method
        performs the swapping of two edges in the myAllEdges-list when the
        outgoing is in clockwise direction to the incoming */
    bool swapWhenReversed(const std::vector<NBEdge*>::iterator &i1,
                          const std::vector<NBEdge*>::iterator &i2);

    /** removes the first edge from the list, marks it as higher priorised and
        returns it */
    NBEdge* extractAndMarkFirst(std::vector<NBEdge*> &s);

    /** returns a list of edges which are connected to the given
        outgoing edge */
    std::vector<NBEdge*> *getEdgesThatApproach(NBEdge *currentOutgoing);

    /// resets the position by the given amount
    void resetby(SUMOReal xoffset, SUMOReal yoffset);

    void reshiftPosition(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    /** @brief Replaces occurences of the first edge within the list of incoming by the second
        Connections are remapped, too */
    void replaceIncoming(NBEdge *which, NBEdge *by, size_t laneOff);

    /** @brief Replaces occurences of every edge from the given list within the list of incoming by the second
        Connections are remapped, too */
    void replaceIncoming(const EdgeVector &which, NBEdge *by);

    /** @brief Replaces occurences of the first edge within the list of outgoing by the second
        Connections are remapped, too */
    void replaceOutgoing(NBEdge *which, NBEdge *by, size_t laneOff);

    /** @brief Replaces occurences of every edge from the given list within the list of outgoing by the second
        Connections are remapped, too */
    void replaceOutgoing(const EdgeVector &which, NBEdge *by);


    void replaceInConnectionProhibitions(NBEdge *which, NBEdge *by,
                                         size_t whichLaneOff, size_t byLaneOff);


    void remapRemoved(NBTrafficLightLogicCont &tc,
                      NBEdge *removed, const EdgeVector &incoming, const EdgeVector &outgoing);

    SUMOReal chooseLaneOffset(DoubleVector &chk);
    SUMOReal chooseLaneOffset2(DoubleVector &chk);

    Position2DVector computeInternalLaneShape(NBEdge *fromE,
            size_t fromL, NBEdge *toE, size_t toL);

    void writeinternal(EdgeVector *myIncomingEdges, OutputDevice &into, const std::string &id);

private:
    /** the name of the node */
    std::string  myID;

    /// The position the node lies at
    Position2D myPosition;

    /** the logic-key */
    //std::string myKey;

    /// vector of incoming edges
    std::vector<NBEdge*> *myIncomingEdges;

    /// vector of outgoing edges
    std::vector<NBEdge*> *myOutgoingEdges;

    /// a vector of incoming and outgoing edges
    std::vector<NBEdge*> myAllEdges;

    /// the type of the junction
    BasicNodeType   myType;

    /** The container for connection block dependencies */
    NBConnectionProhibits myBlockedConnections;

    /// The district the node is the centre of
    NBDistrict *myDistrict;

    /// the (outer) shape of the junction
    Position2DVector myPoly;

    NBRequest *myRequest;

    std::set<NBTrafficLightDefinition*> myTrafficLights;

private:
    /** invalid copy constructor */
    NBNode(const NBNode &s);

    /** invalid assignment operator */
    NBNode &operator=(const NBNode &s);

};


#endif

/****************************************************************************/

