/****************************************************************************/
/// @file    NBNodeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id: $
///
// A container for all of the nets nodes
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
#ifndef NBNodeCont_h
#define NBNodeCont_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <iostream>
#include <utils/geom/Position2D.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBJunctionLogicCont.h>
#include "NBNode.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBDistrict;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
class NBNodeCont
{
public:
    /** definition of the map of names to nodes */
    typedef std::map<std::string, NBNode*> NodeCont;

public:
    NBNodeCont();

    ~NBNodeCont();

    /** inserts a node into the map */
    bool insert(const std::string &id, const Position2D &position,
                NBDistrict *district);

    /** inserts a node into the map */
    bool insert(const std::string &id, const Position2D &position);

    /** inserts a node into the map */
    /*    bool insert(const std::string &id, SUMOReal x, SUMOReal y,
            const std::string &type);*/

    /** inserts a node into the map */
    Position2D insert(const std::string &id);

    /** inserts a node into the map */
    bool insert(NBNode *node);

    /// Removes the given node, deleting it
    bool erase(NBNode *node);

    /** returns the node with the given name */
    NBNode *retrieve(const std::string &id);

    /** returns the node with the given coordinates */
    NBNode *retrieve(const Position2D &position);

    /// returns the begin of the dictionary
    NodeCont::iterator begin();

    /// returns the end of the dictionary
    NodeCont::iterator end();

    /// resets the node positions in a way that they begin from (0, 0)
    bool normaliseNodePositions();
    bool reshiftNodePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    /// divides the incoming lanes on outgoing lanes
    bool computeLanes2Lanes();

    /// build the list of outgoing edges and lanes
    bool computeLogics(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
                       OptionsCont &oc);

    /// sorts the nodes' edges
    bool sortNodesEdges(const NBTypeCont &tc);

    std::vector<std::string> getInternalNamesList();
    /// writes the number nodes into the given ostream
    void writeXMLNumber(std::ostream &into);

    void writeXMLInternalLinks(std::ostream &into);
    void writeXMLInternalSuccInfos(std::ostream &into);
    void writeXMLInternalNodes(std::ostream &into);


    /// writes the nodes into the given ostream
    void writeXML(std::ostream &into);

    /// returns the number of known nodes
    int size();

    /** returns the number of known nodes */
    int getNo();

    /** deletes all nodes */
    void clear();

    /// reports how many nodes were loaded
    void report();

    /// Joins edges connecting the same nodes
    bool recheckEdges(NBDistrictCont &dc, NBTrafficLightLogicCont &tlc,
                      NBEdgeCont &ec);

    /// Removes dummy edges (edges lying completely within a node)
    bool removeDummyEdges(NBDistrictCont &dc, NBEdgeCont &ec,
                          NBTrafficLightLogicCont &tc);

    void searchEdgeInNode(const NBEdgeCont &ec,
                          std::string nodeid, std::string edgeid);

    std::string getFreeID();

    /// Returns the minimum position
    const Position2D &getNetworkOffset() const;

    bool computeNodeShapes(OptionsCont &oc);

    void printNodePositions();

    bool removeUnwishedNodes(NBDistrictCont &dc, NBEdgeCont &ec,
                             NBTrafficLightLogicCont &tlc);

    bool guessRamps(OptionsCont &oc, NBEdgeCont &ec, NBDistrictCont &dc);
    bool guessTLs(OptionsCont &oc, NBTrafficLightLogicCont &tlc);

    void setAsTLControlled(const std::string &name,
                           NBTrafficLightLogicCont &tlc);

    bool savePlain(const std::string &file);

    bool writeTLSasPOIs(const std::string &file);

    const Boundary &getOrigBoundary() const
    {
        return myOrigBoundary;
    }

    const Boundary &getConvBoundary() const
    {
        return myConvBoundary;
    };

    void addGeoreference(const Position2D &p);

private:
    bool mayNeedOnRamp(OptionsCont &oc, NBNode *cur) const;
    bool mayNeedOffRamp(OptionsCont &oc, NBNode *cur) const;
    void buildOnRamp(OptionsCont &oc, NBNode *cur,
                     NBEdgeCont &ec, NBDistrictCont &dc, std::vector<NBEdge*> &incremented);
    void buildOffRamp(OptionsCont &oc, NBNode *cur,
                      NBEdgeCont &ec, NBDistrictCont &dc, std::vector<NBEdge*> &incremented);

    void checkHighwayRampOrder(NBEdge *&pot_highway, NBEdge *&pot_ramp);


private:
    /** the running internal id */
    int     _internalID;

    /** the map of names to nodes */
    NodeCont   _nodes;

    /// The position the network was shifted by
    Position2D myNetworkOffset;

    /// Boundaries of the network (original, converted)
    Boundary myOrigBoundary, myConvBoundary;

private:
    /** invalid copy constructor */
    NBNodeCont(const NBNodeCont &s);

    /** invalid assignment operator */
    NBNodeCont &operator=(const NBNodeCont &s);
};


#endif

/****************************************************************************/

