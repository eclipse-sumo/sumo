#ifndef NBEdgeCont_h
#define NBEdgeCont_h
/***************************************************************************
                          NBEdgeCont.h
			  A container for all of the net's edges
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
// Revision 1.13  2003/08/14 13:51:51  dkrajzew
// reshifting of networks added
//
// Revision 1.12  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.11  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.10  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.9  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.8  2003/03/19 08:03:40  dkrajzew
// splitting of edges made a little bit more stable
//
// Revision 1.7  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.6  2003/03/12 16:47:53  dkrajzew
// extension for artemis-import
//
// Revision 1.5  2003/03/06 17:18:41  dkrajzew
// debugging during vissim implementation
//
// Revision 1.4  2003/03/03 14:59:04  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.3  2003/02/13 15:51:05  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.6  2002/07/25 08:31:43  dkrajzew
// Report methods transfered from loader to the containers
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
// Revision 1.2  2002/03/22 10:51:26  dkrajzew
// Clearing of static members added
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:38:00  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include "NBCont.h"
#include "NBNode.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBSection;
class NBNodeCont;
class NBEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBEdgeCont
 *
 */
class NBEdgeCont {
public:
    /** adds an edge to the dictionary;
        returns false if the edge already was in the dictionary */
    static bool insert(NBEdge *edge);

    /// returns the edge that has the given id
    static NBEdge *retrieve(const std::string &id);

    /** @brief Tries to retrieve an edge, even if it is splitted
        To describe which part of the edge shall be returned, the
        id of a second edge, participating at the node and the information
        whether to return the outgoing or the incoming is needed */
    static NBEdge *retrievePossiblySplitted(
        const std::string &id, const std::string &hint, bool incoming);

    /** @brief Tries to retrieve an edge, even if it is splitted
        To describe which part of the edge shall be returned, a
        position hint is supplied */
    static NBEdge *retrievePossiblySplitted(
        const std::string &id, double pos);

    /** computes edges, step1: computation of approached edges */
    static bool computeEdge2Edges();

	/// computes edges, step2: computation of which lanes approach the edges)
	static bool computeLanes2Edges();

    /** sorts all lanes of all edges within the container by their direction */
    static bool sortOutgoingLanesConnections();

    /** computes the turn-around directions of all edges within the
        container */
    static bool computeTurningDirections();

    /** rechecks whether all lanes have a successor */
    static bool recheckLanes();

    /** computes the node-internal priorities of links */
//    static bool computeLinkPriorities(bool verbose);

    /** appends turnarounds */
    static bool appendTurnarounds();

    /** @brief Splits the edge at the position nearest to the given node */
    static bool splitAt(NBEdge *edge, NBNode *node);

    /** @brief Splits the edge at the position nearest to the given node using the given modifications */
    static bool splitAt(NBEdge *edge, NBNode *node,
        const std::string &firstEdgeName, const std::string &secondEdgeName,
        size_t noLanesFirstEdge, size_t noLanesSecondEdge);

    /** @brief Splits the edge at the position nearest to the given node using the given modifications */
    static bool splitAt(NBEdge *edge, double edgepos, NBNode *node,
        const std::string &firstEdgeName, const std::string &secondEdgeName,
        size_t noLanesFirstEdge, size_t noLanesSecondEdge);

    /** Removes the given edge from the container (deleting it) */
    static void erase(NBEdge *edge);

    /** writes the list of edge names into the given stream */
    static void writeXMLEdgeList(std::ostream &into);

    /** writes the edge definitions with lanes and connected edges
        into the given stream */
    static void writeXMLStep1(std::ostream &into);

    /** writes the successor definitions of edges */
    static void writeXMLStep2(std::ostream &into);

    /** writes the positions of edges */
    static void writeXMLStep3(std::ostream &into);

    /** returns the size of the edges */
    static int size();

    /** returns the number of known edges */
    static int getNo();

    /** deletes all edges */
    static void clear();

    /// reports how many edges were loaded
    static void report();

    /// joins the given edges as they connect the same nodes
    static void joinSameNodeConnectingEdges(EdgeVector edges);

    /// !!! debug only
    static void search(NBEdge *e);

    /// moves the geometry of the edges by the network offset
    static bool normaliseEdgePositions();
    static bool reshiftEdgePositions(double xoff, double yoff, double rot);

private:
    static std::vector<std::string> buildPossibilities(
        const std::vector<std::string> &s);


    static EdgeVector getGeneratedFrom(const std::string &id);

private:
    /// the type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBEdge*> EdgeCont;

    /// the instance of the dictionary
    static EdgeCont _edges;

	/// the number of splits of edges during the building
	static size_t EdgesSplit;

private:
    /** invalid copy constructor */
    NBEdgeCont(const NBEdgeCont &s);

    /** invalid assignment operator */
    NBEdgeCont &operator=(const NBEdgeCont &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBEdgeCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
