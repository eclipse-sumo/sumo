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
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.5  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.4  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.3  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.2  2002/04/25 14:15:07  dkrajzew
// The assignement of priorities of incoming edges improved; now, if having equal priorities, the opposite edges are chosen as higher priorised
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
// getIncomingEdges and getOutgoingEdges do now not return pointers to const vectors; this should be reconcepted
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
#include <vector>
#include <deque>
#include <utility>
#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
#include "NBEdge.h"
#include "NBJunctionLogicCont.h"
#include "NBConnectionDefs.h"
#include "NBContHelper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBRequest;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBNode
 *  The class for a single node
 */
class NBNode {
public:
    static int debug;
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
        void execute(double src, double dest);

        /** the method that spreads the wished number of lanes from the
            the lane given by the bresenham-call to both left and right */
        std::deque<size_t> *spread(const std::vector<size_t> &approachingLanes,
            double dest) const;

    };

    enum TLColor {
        TLCOLOR_UNKNOWN,
        TLCOLOR_RED,
        TLCOLOR_YELLOW,
        TLCOLOR_REDYELLOW,
        TLCOLOR_GREEN,
        TLCOLOR_BLINK,
        TLCOLOR_BLUE // :-)
    };


    class SignalGroup 
        : public Named {
    public:
        SignalGroup(const std::string &id);
        ~SignalGroup();
        void addConnection(const Connection &c);
        void addPhaseBegin(double time, TLColor color);
        void setYellowTimes(double tRedYellowe, double tYellow);
        DoubleVector getTimes() const;
        void sortPhases();
        size_t getLinkNo() const;
        bool mayDrive(double time) const;
        bool mustBrake(double time) const;
        bool containsConnection(NBEdge *from, NBEdge *to) const;

        friend class phase_by_time_sorter;

    private:
        class PhaseDef {
        public:
            PhaseDef(double time, TLColor color) 
                : myTime(time), myColor(color) { }
            PhaseDef(const PhaseDef &p) 
                : myTime(p.myTime), myColor(p.myColor) { }
            double myTime;
            TLColor myColor;
        };

        class phase_by_time_sorter {
        public:
            /// constructor
            explicit phase_by_time_sorter() { }

            int operator() (const PhaseDef &p1, const PhaseDef &p2) {
                return p1.myTime<p2.myTime;
            }
        };

        ConnectionVector myConnections;
        typedef std::vector<PhaseDef> GroupsPhases;
        GroupsPhases myPhases;
        double myTRedYellow, myTYellow;
        size_t myNoLinks;
    };

    class Phase 
        : public Named {
    public:
        Phase(const std::string &id, size_t begin, size_t end);
        ~Phase();
/*        void addSignalGroupColor(const std::string &signalgroup, 
            TLColor color);*/
    private:
        std::string mySignalGroup;
        int myBegin, myEnd;
        typedef std::map<std::string, TLColor> SignalGroupColorMap;
        SignalGroupColorMap _groupColors;
    };

    /// Definition of the container for signal groups
    typedef std::map<std::string, SignalGroup*> SignalGroupCont;

    class NodesPhases {
    public:
        NodesPhases();
        ~NodesPhases();

    };

    /** internal type for no-junction */
    static const int TYPE_NOJUNCTION;
    /** internal type for a traffic-light-junction */
    static const int TYPE_TRAFFIC_LIGHT;
    /** internal type for a priority-junction */
    static const int TYPE_PRIORITY_JUNCTION;
    /** internal type for a right-before-left junction */
    static const int TYPE_RIGHT_BEFORE_LEFT;
    /** internal type for a dead-end junction */
    static const int TYPE_DEAD_END;

    /** a counter for the no-junctions build */
    static int _noNoJunctions;

    /** a counter for priority-junctions build */
    static int _noPriorityJunctions;

    /** a counter for traffic-light-junctions build */
    static int _noTrafficLightJunctions;

    /** a counter for right-before-left-junctions build */
    static int _noRightBeforeLeftJunctions;

    friend class NBEdgeCont;

public:
    /** constructor */
    NBNode(const std::string &id, double x, double y);

    /** constructor */
    NBNode(const std::string &id, double x, double y,
        const std::string &type);

    /** sumo-node constructor */
    NBNode(const std::string &id, double x, double y,
        int type, const std::string &key);

    /** destructor */
    ~NBNode();

    /** return the x-coordinate of the node */
    double getXCoordinate();

    /** return the y-coordinate of the node */
    double getYCoordinate();

    /** returns the id of the node */
    std::string getID();

    /// adds an incoming edge
    void addIncomingEdge(NBEdge *edge);

    /// adds an outgoing edge
    void addOutgoingEdge(NBEdge *edge);

    /// returns the list of the ids of the incoming edges
    EdgeVector *getIncomingEdges();

    /// returns the list of the ids of the outgoing edges
    EdgeVector *getOutgoingEdges();

    /// returns the list of all edgs
    const EdgeVector *getEdges();

    /// prints the junction
    void writeXML(std::ostream &into);

    /// computes the connections of lanes to edges
    void computeEdges2Lanes();

    /// computes the node's type, logic and traffic light
    void computeLogic(long maxSize);

    /** initialises the list of all edges and sorts all edges */
    void sortNodesEdges();

    /** reports about the build junctions */
    static void reportBuild();

    /// sets the connection between two nodes via this node
    std::string setTurningDefinition(NBNode *from, NBNode *to);

    /** @brief Returns something like the most unused direction
        Should only be used to add source or sink nodes */
    Position2D getEmptyDir() const;

    bool hasOutgoing(NBEdge *e) const;
    bool hasIncoming(NBEdge *e) const;
    NBEdge *getOppositeIncoming(NBEdge *e) const;
    NBEdge *getOppositeOutgoing(NBEdge *e) const;

    void removeDoubleEdges();


    void addSortedLinkFoes(
        const std::pair<NBEdge*, NBEdge*> &mayDrive,
        const std::pair<NBEdge*, NBEdge*> &mustStop);

    NBEdge *getPossiblySplittedIncoming(const std::string &edgeid);
    NBEdge *getPossiblySplittedOutgoing(const std::string &edgeid);

    void eraseDummies();

    void removeOutgoing(NBEdge *edge);
    void removeIncoming(NBEdge *edge);

    void setCycleDuration(size_t cycleDur);
    void addSignalGroup(const std::string &id);
    void addToSignalGroup(const std::string &groupid, 
        const Connection &connection);
    void addToSignalGroup(const std::string &groupid, 
        const ConnectionVector &connections);
    void addSignalGroupPhaseBegin(const std::string &groupid,
        double time, TLColor color);
    void setSignalYellowTimes(const std::string &groupid, 
        double tRedYellowe, double tYellow);


    static SignalGroup *findGroup(const SignalGroupCont &defs, NBEdge *from, NBEdge *to);

    friend class NBNodeCont;


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
    int computeType() const;

    /** returns the information whether this node is the center of a district
        for this, all incoming edges must be sinks while all outgoing
        edges must be sources */
    bool isDistrictCenter() const;

    /// computes the priorities of participating edges within this junction
    void setPriorities();

    /// sets the priorites in case of a priority junction
    void setPriorityJunctionPriorities();

    /// computes the logic
    void computeLogic(NBRequest *request, long maxSize);

    /** sets the type of the junction
        some version-dependend conversions are done */
    void setType(int type);

    /** used while fine sorting the incoming and outgoing edges, this method
        performs the swapping of two edges in the _allEdges-list when the
        outgoing is in clockwise direction to the incoming */
    void swapWhenReversed(const std::vector<NBEdge*>::iterator &i1,
        const std::vector<NBEdge*>::iterator &i2);

    /// returns the highest priority of the edges in the list
    int getHighestPriority(const std::vector<NBEdge*> &s);

    /** removes the first edge from the list, marks it as higher priorised and
        returns it */
    NBEdge* extractAndMarkFirst(std::vector<NBEdge*> &s);

    /** returns a list that contains only edges of the most highest priority
        encountered in the given list */
    std::vector<NBEdge*> NBNode::getMostPriorised(std::vector<NBEdge*> &s);

    /** returns a list of edges which are connected to the given
        outgoing edge */
    std::vector<NBEdge*> *getApproaching(NBEdge *currentOutgoing);

    /// resets the position by the given amount
    void resetby(double xoffset, double yoffset);

    void replaceOutgoing(NBEdge *which, NBEdge *by);

    void replaceIncoming(NBEdge *which, NBEdge *by);


    void replaceInConnectionProhibitions(NBEdge *which, NBEdge *by);



private:
    /** the name of the node */
    std::string  _id;

    /** the x-coordinate of the node */
    double  _x;

    /** the y-coordinate of the node */
    double  _y;

    /** the logic-key */
    std::string _key;

    /// vector of incoming edges
    std::vector<NBEdge*> *_incomingEdges;

    /// vector of outgoing edges
    std::vector<NBEdge*> *_outgoingEdges;

    /// a vector of incoming and outgoing edges
    std::vector<NBEdge*> _allEdges;

    /// the type of the junction
    int   _type;

    /** The container for connection block dependencies */
    ConnectionProhibits _blockedConnections;


    SignalGroupCont mySignalGroups;

    size_t myCycleDuration;

private:
    /** invalid copy constructor */
    NBNode(const NBNode &s);

    /** invalid assignment operator */
    NBNode &operator=(const NBNode &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBNode.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
