#ifndef NBNodeCont_h
#define NBNodeCont_h
/***************************************************************************
                          NBNodeCont.h
			  A container for all of the nets nodes
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
// Revision 1.1  2004/01/12 15:26:11  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.18  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two doubles added
//
// Revision 1.17  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.16  2003/08/18 12:49:59  dkrajzew
// possibility to print node positions added
//
// Revision 1.15  2003/08/14 13:51:51  dkrajzew
// reshifting of networks added
//
// Revision 1.14  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.13  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.12  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.11  2003/04/10 15:45:19  dkrajzew
// some lost changes reapplied
//
// Revision 1.10  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.9  2003/04/01 15:15:54  dkrajzew
// further work on vissim-import
//
// Revision 1.8  2003/03/20 16:23:09  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.7  2003/03/18 13:07:23  dkrajzew
// usage of node position within xml-edge descriptions allowed
//
// Revision 1.6  2003/03/06 17:18:43  dkrajzew
// debugging during vissim implementation
//
// Revision 1.5  2003/03/03 14:59:14  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.4  2003/02/13 15:51:55  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.3  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:33:53  dkrajzew
// adding of typed nodes added
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:31:42  dkrajzew
// Report methods transfered from loader to the containers
//
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
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
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <iostream>
#include <utils/geom/Position2D.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBJunctionLogicCont.h>
#include "NBNode.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBDistrict;
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBNodeCont
{
public:
    /** definition of the map of names to nodes */
    typedef std::map<std::string, NBNode*> NodeCont;

public:
    /** inserts a node into the map */
    static bool insert(const std::string &id, double x, double y,
        NBDistrict *district);

    /** inserts a node into the map */
    static bool insert(const std::string &id, double x, double y);

    /** inserts a node into the map */
/*    static bool insert(const std::string &id, double x, double y,
        const std::string &type);*/

    /** inserts a node into the map */
    static Position2D insert(const std::string &id);

    /** inserts a node into the map */
    static bool insert(NBNode *node);

    /// Removes the given node, deleting it
    static bool erase(NBNode *node);

    /** returns the node with the given name */
    static NBNode *retrieve(const std::string &id);

    /** returns the node with the given coordinates */
    static NBNode *retrieve(double x, double y);

    /// returns the begin of the dictionary
    static NodeCont::iterator begin();

    /// returns the end of the dictionary
    static NodeCont::iterator end();

    /// resets the node positions in a way that they begin from (0, 0)
    static bool normaliseNodePositions();
    static bool reshiftNodePositions(double xoff, double yoff, double rot);

    /// divides the incoming lanes on outgoing lanes
    static bool computeLanes2Lanes();

    /// build the list of outgoing edges and lanes
    static bool computeLogics(OptionsCont &oc);

    /// sorts the nodes' edges
    static bool sortNodesEdges();

    static std::vector<std::string> getInternalNamesList();
    /// writes the number nodes into the given ostream
    static void writeXMLNumber(std::ostream &into);

    static void writeXMLInternalLinks(std::ostream &into);
    static void writeXMLInternalEdgePos(std::ostream &into);
    static void writeXMLInternalSuccInfos(std::ostream &into);


    /// writes the nodes into the given ostream
    static void writeXML(std::ostream &into);

    /// returns the number of known nodes
    static int size();

    /** returns the number of known nodes */
    static int getNo();

    /** deletes all nodes */
    static void clear();

    /// reports how many nodes were loaded
    static void report();

    /// Joins edges connecting the same nodes
    static bool recheckEdges();

    /// Removes dummy edges (edges lying completely within a node)
    static bool removeDummyEdges();

    static void searchEdgeInNode(std::string nodeid, std::string edgeid);

    static std::string getFreeID();

    /// Returns the minimum position
    static Position2D getNetworkOffset();

    static bool computeNodeShapes();

    static void printNodePositions();

    static bool removeUnwishedNodes();


private:
    /** the running internal id */
    static int     _internalID;

    /** the map of names to nodes */
    static NodeCont   _nodes;

    /// The minimum network position
    static Position2D myNetworkOffset;

private:
    /** invalid copy constructor */
    NBNodeCont(const NBNodeCont &s);

    /** invalid assignment operator */
    NBNodeCont &operator=(const NBNodeCont &s);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBNodeCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
