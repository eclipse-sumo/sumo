/****************************************************************************/
/// @file    NBEdgeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A container for all of the net's edges
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
#ifndef NBEdgeCont_h
#define NBEdgeCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include "NBCont.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBSection;
class NBNodeCont;
class NBEdge;
class NBNode;
class OptionsCont;
class NBDistrictCont;
class NBTrafficLightLogicCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBEdgeCont
 *
 */
class NBEdgeCont
{
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
    void computeEdge2Edges();

    /// computes edges, step2: computation of which lanes approach the edges)
    void computeLanes2Edges();

    /** sorts all lanes of all edges within the container by their direction */
    void sortOutgoingLanesConnections();

    /** computes the turn-around directions of all edges within the
        container */
    void computeTurningDirections();

    /** rechecks whether all lanes have a successor */
    void recheckLanes();

    /** appends turnarounds */
    void appendTurnarounds();

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

    /** writes the edge definitions with lanes and connected edges
        into the given stream */
    void writeXMLStep1(OutputDevice &into);

    /** writes the successor definitions of edges */
    void writeXMLStep2(OutputDevice &into, bool includeInternal);

    /** returns the number of edges */
    int size();

    /** deletes all edges */
    void clear();

    /// joins the given edges as they connect the same nodes
    void joinSameNodeConnectingEdges(NBDistrictCont &dc,
                                     NBTrafficLightLogicCont &tlc, EdgeVector edges);

    /// moves the geometry of the edges by the network offset
    void normaliseEdgePositions();
    void reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    void computeEdgeShapes();

    std::vector<std::string> getAllNames();

    bool savePlain(const std::string &file);

    void removeUnwishedEdges(NBDistrictCont &dc, OptionsCont &oc);

    void recomputeLaneShapes();

    void splitGeometry(NBNodeCont &nc);

    void recheckLaneSpread();

    void recheckEdgeGeomsForDoublePositions();

    size_t getNoEdgeSplits();

private:
    std::vector<std::string> buildPossibilities(
        const std::vector<std::string> &s);


    EdgeVector getGeneratedFrom(const std::string &id) const;

private:
    /// the type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBEdge*> EdgeCont;

    /// the instance of the dictionary
    EdgeCont myEdges;

    /// the number of splits of edges during the building
    size_t myEdgesSplit;

private:
    /** invalid copy constructor */
    NBEdgeCont(const NBEdgeCont &s);

    /** invalid assignment operator */
    NBEdgeCont &operator=(const NBEdgeCont &s);

};


#endif

/****************************************************************************/

