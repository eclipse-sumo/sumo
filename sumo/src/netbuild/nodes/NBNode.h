#ifndef NBNode_h
#define NBNode_h
/***************************************************************************
                          NBNode.h
              The representation of a single node
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
// Revision 1.9  2005/11/09 06:40:49  dkrajzew
// complete geometry building rework (unfinished)
//
// Revision 1.8  2005/10/07 11:38:19  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:01:06  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:02:26  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 12:32:49  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
// Revision 1.4  2005/04/27 11:48:27  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.3  2004/11/23 10:21:42  dkrajzew
// debugging
//
// Revision 1.2  2004/08/02 13:11:40  dkrajzew
// made some deprovements or so
//
// Revision 1.1  2004/01/12 15:26:11  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.26  2003/11/18 14:20:02  dkrajzew
// computation of junction-inlanes geometry added
//
// Revision 1.25  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two SUMOReals added
//
// Revision 1.24  2003/10/15 11:48:13  dkrajzew
// geometry computation corrigued partially
//
// Revision 1.23  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality
//  cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.21  2003/09/22 12:40:12  dkrajzew
// further work on vissim-import
//
// Revision 1.20  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.19  2003/08/20 11:53:18  dkrajzew
// further work on node geometry
//
// Revision 1.18  2003/08/14 13:50:15  dkrajzew
// new junction shape computation implemented
//
// Revision 1.17  2003/07/30 09:21:11  dkrajzew
// added the generation about link directions and priority
//
// Revision 1.16  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.15  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler); this handler
//  puts it further to output instances. changes: no verbose-parameter needed;
//  messages are exported to singleton
//
// Revision 1.14  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.13  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on
//  tl-import; further work on vissim-import
//
// Revision 1.12  2003/04/16 10:03:48  dkrajzew
// further work on Vissim-import
//
// Revision 1.11  2003/04/10 15:45:19  dkrajzew
// some lost changes reapplied
//
// Revision 1.10  2003/04/07 12:15:41  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic
//  lights have yellow times given explicitely, now
//
// Revision 1.9  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge
//  sorting (false lane connections) debugged
//
// Revision 1.8  2003/04/01 15:15:53  dkrajzew
// further work on vissim-import
//
// Revision 1.7  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.6  2003/03/12 16:47:54  dkrajzew
// extension for artemis-import
//
// Revision 1.5  2003/03/03 14:59:06  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.4  2003/02/13 15:51:54  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.3  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:40:11  dkrajzew
// typing of nodes during loading allwoed
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.6  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the
//  EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.5  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed;
//  Comments improved
//
// Revision 1.4  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.3  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.2  2002/04/25 14:15:07  dkrajzew
// The assignement of priorities of incoming edges improved;
//  now, if having equal priorities, the opposite edges are chosen as higher
//  priorised
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
// Revision 1.2  2002/03/22 10:53:29  dkrajzew
// getIncomingEdges and getOutgoingEdges do now not return pointers to const
//  vectors; this should be reconcepted
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <deque>
#include <utility>
#include <string>
#include <set>
#include <iostream>
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/Position2DVector.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBJunctionLogicCont.h>
#include <netbuild/NBConnection.h>
#include <netbuild/NBConnectionDefs.h>
#include <netbuild/NBContHelper.h>
#include <netbuild/NBMMLDirections.h>
#include <utils/geom/Bresenham.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBRequest;
class NBDistrict;
class OptionsCont;
class NBTrafficLightDefinition;
class NBTypeCont;
class NBTrafficLightLogicCont;
class NBDistrictCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBNode
 *  The class for a single node (junction)
 */
class NBNode {
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
    class ApproachingDivider : public Bresenham::BresenhamCallBack {
    private:
        /// the list of edges that approach the current edge
        std::vector<NBEdge*> *_approaching;

        /// the approached current edge
        NBEdge *_currentOutgoing;

    public:
        /// constructor
        ApproachingDivider(std::vector<NBEdge*> *approaching,
            NBEdge *currentOutgoing);

        /// destructor
        ~ApproachingDivider();

        /** the bresenham-callback */
        void execute(SUMOReal src, SUMOReal dest);

        /** the method that spreads the wished number of lanes from the
            the lane given by the bresenham-call to both left and right */
        std::deque<size_t> *spread(const std::vector<size_t> &approachingLanes,
            SUMOReal dest) const;

    };

    enum BasicNodeType {
        /// Unknown yet
        NODETYPE_UNKNOWN,
        /** internal type for no-junction */
        NODETYPE_NOJUNCTION,
        /** internal type for a priority-junction */
        NODETYPE_PRIORITY_JUNCTION,
        /** internal type for a right-before-left junction */
        NODETYPE_RIGHT_BEFORE_LEFT,
        /** internal type for a district junction */
        NODETYPE_DISTRICT,
        /** internal type for a dead-end junction */
        NODETYPE_DEAD_END
    };

    /** a counter for the no-junctions build */
    static int _noDistricts;

    /** a counter for the no-junctions build */
    static int _noNoJunctions;

    /** a counter for priority-junctions build */
    static int _noPriorityJunctions;

    /** a counter for right-before-left-junctions build */
    static int _noRightBeforeLeftJunctions;

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
    std::string getID() const;

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

    std::vector<std::string> getInternalNamesList();

    void writeXMLInternalLinks(std::ostream &into);
    void writeXMLInternalSuccInfos(std::ostream &into);



    /// prints the junction
    void writeXML(std::ostream &into);

    /// computes the connections of lanes to edges
    void computeLanes2Lanes();

    /// computes the node's type, logic and traffic light
    void computeLogic(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
        OptionsCont &oc);

    /** initialises the list of all edges and sorts all edges */
    void sortNodesEdges(const NBTypeCont &tc);

    /** reports about the build junctions */
    static void reportBuild();

    /// sets the connection between two nodes via this node
    void setTurningDefinition(NBNode *from, NBNode *to);

    /** @brief Returns something like the most unused direction
        Should only be used to add source or sink nodes */
    Position2D getEmptyDir() const;

    bool hasOutgoing(NBEdge *e) const;
    bool hasIncoming(NBEdge *e) const;
    NBEdge *getOppositeIncoming(NBEdge *e) const;
    NBEdge *getOppositeOutgoing(NBEdge *e) const;
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


    bool isLeftMover(NBEdge *from, NBEdge *to) const;


    bool mustBrake(NBEdge *from, NBEdge *to, int toLane) const;

    bool forbids(NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
        NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
        bool regardNonSignalisedLowerPriority) const;

    bool foes(NBEdge *from1, NBEdge *to1, NBEdge *from2, NBEdge *to2) const;

    void addTrafficLight(NBTrafficLightDefinition *tld);

    NBMMLDirection getMMLDirection(NBEdge *incoming, NBEdge *outgoing) const;

    char stateCode(NBEdge *incoming, NBEdge *outgoing, int fromLane);

    void computeNodeShape(std::ofstream *out);

    friend class NBNodeCont;


    SUMOReal getOffset(Position2DVector on, Position2DVector cross) const;

    const Position2DVector &getShape() const;

    size_t countInternalLanes();

    std::string getInternalLaneID(NBEdge *from, size_t fromlane, NBEdge *to) const;


    bool checkIsRemovable() const;

    bool isTLControlled() const;

    std::vector<std::pair<NBEdge*, NBEdge*> > getEdgesToJoin() const;

    SUMOReal getMaxEdgeWidth() const;

    friend class NBNodeShapeComputer;

    bool isNearDistrict() const;
    bool isDistrict() const;

private:

    /// rotates the junction so that the key fits
    void rotateIncomingEdges(int norot);

    /// sets the computed non - permutating key
    void setKey(std::string key);

    /// build the logic using the NBRequest
    void buildMapLogic();

    /// build the logic using the NBRequest
    void buildBitfieldLogic();

    /// builds the list of incoming and outgoing lanes/edges
    void buildList();

    /** sorts edges with same direction (other direction lanes) in a way that
        the outgoing lanes are "earlier" in the list */
    void sortSmall();

    // computes the junction type
    BasicNodeType computeType(const NBTypeCont &tc) const;

    /** returns the information whether this node is the center of a district
        for this, all incoming edges must be sinks while all outgoing
        edges must be sources */
    bool isDistrictCenter() const;

    /// computes the priorities of participating edges within this junction
    void setPriorities();

    /// sets the priorites in case of a priority junction
    void setPriorityJunctionPriorities();

    /** used while fine sorting the incoming and outgoing edges, this method
        performs the swapping of two edges in the _allEdges-list when the
        outgoing is in clockwise direction to the incoming */
    bool swapWhenReversed(const std::vector<NBEdge*>::iterator &i1,
        const std::vector<NBEdge*>::iterator &i2);

    /// returns the highest priority of the edges in the list
    int getHighestPriority(const std::vector<NBEdge*> &s);

    /** removes the first edge from the list, marks it as higher priorised and
        returns it */
    NBEdge* extractAndMarkFirst(std::vector<NBEdge*> &s);

    /** returns a list that contains only edges of the most highest priority
        encountered in the given list */
    std::vector<NBEdge*> getMostPriorised(std::vector<NBEdge*> &s);

    /** returns a list of edges which are connected to the given
        outgoing edge */
    std::vector<NBEdge*> *getApproaching(NBEdge *currentOutgoing);

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

    SUMOReal getCCWAngleDiff(SUMOReal angle1, SUMOReal angle2) const; // !!! not in NBNode!


    SUMOReal getCWAngleDiff(SUMOReal angle1, SUMOReal angle2) const; // !!! not in NBNode!


    Position2DVector computeInternalLaneShape(NBEdge *fromE,
        size_t fromL, NBEdge *toE, size_t toL);


private:
    /** the name of the node */
    std::string  _id;

    /// The position the node lies at
    Position2D myPosition;

    /** the logic-key */
    std::string _key;

    /// vector of incoming edges
    std::vector<NBEdge*> *_incomingEdges;

    /// vector of outgoing edges
    std::vector<NBEdge*> *_outgoingEdges;

    /// a vector of incoming and outgoing edges
    std::vector<NBEdge*> _allEdges;

    /// the type of the junction
    BasicNodeType   _type;

    /** The container for connection block dependencies */
    NBConnectionProhibits _blockedConnections;

    /// The district the node is the centre of
    NBDistrict *myDistrict;

    /// the (outer) shape of the junction
    Position2DVector myPoly;

    NBRequest *_request;

    std::set<NBTrafficLightDefinition*> myTrafficLights;

private:
    /** invalid copy constructor */
    NBNode(const NBNode &s);

    /** invalid assignment operator */
    NBNode &operator=(const NBNode &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
