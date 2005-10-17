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
// Revision 1.24  2005/10/17 09:02:44  dkrajzew
// got rid of the old MSVC memory leak checker; memory leaks removed
//
// Revision 1.23  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.22  2005/09/23 06:01:05  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.21  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.20  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.19  2004/12/16 12:16:30  dkrajzew
// a further network prune option added
//
// Revision 1.18  2004/08/02 13:11:39  dkrajzew
// made some deprovements or so
//
// Revision 1.17  2004/07/02 09:30:55  dkrajzew
// removal of edges with a too low speed added
//
// Revision 1.16  2004/01/12 15:25:09  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.15  2004/01/12 15:09:28  dkrajzew
// some work on the documentation
//
// Revision 1.14  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.13  2003/08/14 13:51:51  dkrajzew
// reshifting of networks added
//
// Revision 1.12  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.11  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler); this handler
//  puts it further to output instances. changes: no verbose-parameter needed;
//  messages are exported to singleton
//
// Revision 1.10  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.9  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on
//  tl-import; further work on vissim-import
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
// windows eol removed; template class definition inclusion depends now on
//  the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed;
//  Comments improved
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
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

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include "NBCont.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBSection;
class NBNodeCont;
class NBEdge;
class NBNode;
class OptionsCont;
class NBDistrictCont;
class NBTrafficLightLogicCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBEdgeCont
 *
 */
class NBEdgeCont {
public:
    NBEdgeCont();
    ~NBEdgeCont();

    /** adds an edge to the dictionary;
        returns false if the edge already was in the dictionary */
    bool insert(NBEdge *edge);

    /// returns the edge that has the given id
    NBEdge *retrieve(const std::string &id) const;

    /** @brief Tries to retrieve an edge, even if it is splitted
        To describe which part of the edge shall be returned, the
        id of a second edge, participating at the node and the information
        whether to return the outgoing or the incoming is needed */
    NBEdge *retrievePossiblySplitted(
        const std::string &id, const std::string &hint, bool incoming) const;

    /** @brief Tries to retrieve an edge, even if it is splitted
        To describe which part of the edge shall be returned, a
        position hint is supplied */
    NBEdge *retrievePossiblySplitted(
        const std::string &id, SUMOReal pos) const;

    /** computes edges, step1: computation of approached edges */
    bool computeEdge2Edges();

    /// computes edges, step2: computation of which lanes approach the edges)
    bool computeLanes2Edges();

    /** sorts all lanes of all edges within the container by their direction */
    bool sortOutgoingLanesConnections();

    /** computes the turn-around directions of all edges within the
        container */
    bool computeTurningDirections();

    /** rechecks whether all lanes have a successor */
    bool recheckLanes();

    /** appends turnarounds */
    bool appendTurnarounds();

    /** @brief Splits the edge at the position nearest to the given node */
    bool splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node);

    /** @brief Splits the edge at the position nearest to the given node using the given modifications */
    bool splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node,
        const std::string &firstEdgeName, const std::string &secondEdgeName,
        size_t noLanesFirstEdge, size_t noLanesSecondEdge);

    /** @brief Splits the edge at the position nearest to the given node using the given modifications */
    bool splitAt(NBDistrictCont &dc, NBEdge *edge, SUMOReal edgepos, NBNode *node,
        const std::string &firstEdgeName, const std::string &secondEdgeName,
        size_t noLanesFirstEdge, size_t noLanesSecondEdge);

    /** Removes the given edge from the container (deleting it) */
    void erase(NBDistrictCont &dc, NBEdge *edge);

    /** writes the list of edge names into the given stream */
    void writeXMLEdgeList(std::ostream &into,
        std::vector<std::string> toAdd);

    /** writes the edge definitions with lanes and connected edges
        into the given stream */
    void writeXMLStep1(std::ostream &into);

    /** writes the successor definitions of edges */
    void writeXMLStep2(std::ostream &into);

    /** writes the positions of edges */
    void writeXMLStep3(std::ostream &into);

    /** returns the size of the edges */
    int size();

    /** returns the number of known edges */
    int getNo();

    /** deletes all edges */
    void clear();

    /// reports how many edges were loaded
    void report();

    /// joins the given edges as they connect the same nodes
    void joinSameNodeConnectingEdges(NBDistrictCont &dc,
        NBTrafficLightLogicCont &tlc, EdgeVector edges);

    /// !!! debug only
    void search(NBEdge *e);

    /// moves the geometry of the edges by the network offset
    bool normaliseEdgePositions(const NBNodeCont &nc);
    bool reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    bool computeEdgeShapes();

    std::vector<std::string> getAllNames();

    bool savePlain(const std::string &file);

    bool removeUnwishedEdges(NBDistrictCont &dc, OptionsCont &oc);

private:
    std::vector<std::string> buildPossibilities(
        const std::vector<std::string> &s);


    EdgeVector getGeneratedFrom(const std::string &id) const;

private:
    /// the type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBEdge*> EdgeCont;

    /// the instance of the dictionary
    EdgeCont _edges;

    /// the number of splits of edges during the building
    size_t EdgesSplit;

private:
    /** invalid copy constructor */
    NBEdgeCont(const NBEdgeCont &s);

    /** invalid assignment operator */
    NBEdgeCont &operator=(const NBEdgeCont &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
