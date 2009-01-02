/****************************************************************************/
/// @file    NBNodeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Container for nodes during the netbuilding process
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/geom/Position2D.h>
#include "NBEdgeCont.h"
#include "NBJunctionLogicCont.h"
#include "NBNode.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBDistrict;
class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNodeCont
 * @brief Container for nodes during the netbuilding process
 */
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
    void normaliseNodePositions();
    void reshiftNodePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    /// divides the incoming lanes on outgoing lanes
    void computeLanes2Lanes();

    /// build the list of outgoing edges and lanes
    void computeLogics(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
                       OptionsCont &oc);

    /// sorts the nodes' edges
    void sortNodesEdges(const NBTypeCont &tc);

    void writeXMLInternalLinks(OutputDevice &into);
    void writeXMLInternalSuccInfos(OutputDevice &into);
    void writeXMLInternalNodes(OutputDevice &into);


    /// writes the nodes into the given ostream
    void writeXML(OutputDevice &into);

    /** @brief Returns the number of known nodes
     * @return The number of nodes stored in this container
     */
    unsigned int size() const throw() {
        return (unsigned int) myNodes.size();
    }

    /** deletes all nodes */
    void clear();

    /// Joins edges connecting the same nodes
    void recheckEdges(NBDistrictCont &dc, NBTrafficLightLogicCont &tlc,
                      NBEdgeCont &ec);

    /// Removes dummy edges (edges lying completely within a node)
    void removeDummyEdges(NBDistrictCont &dc, NBEdgeCont &ec,
                          NBTrafficLightLogicCont &tc);

    std::string getFreeID();

    void computeNodeShapes();

    /** @brief Removes "unwished" nodes
     *
     * Removes nodes if a) no incoming/outgoing edges exist or
     *  b) if the node is a "geometry" node. In the second case,
     *  edges that participate at the node will be joined.
     * Whether the node is a geometry node or not, is determined
     *  by a call to NBNode::checkIsRemovable.
     * The node is removed from the list of tls-controlled nodes.
     * @param[in, mod] dc The district container needed if a node shall be removed
     * @param[in, mod] ec The edge container needed for joining edges
     * @param[in, mod] tlc The traffic lights container to remove nodes from
     * @param[in] removeGeometryNodes Whether geometry nodes shall also be removed
     */
    void removeUnwishedNodes(NBDistrictCont &dc, NBEdgeCont &ec,
                             NBTrafficLightLogicCont &tlc,
                             bool removeGeometryNodes) throw();

    void guessRamps(OptionsCont &oc, NBEdgeCont &ec, NBDistrictCont &dc);
    void guessTLs(OptionsCont &oc, NBTrafficLightLogicCont &tlc);

    void setAsTLControlled(NBNode *node, NBTrafficLightLogicCont &tlc, std::string id="");

    bool savePlain(const std::string &file);

    /** @brief Writes positions of traffic lights as a list of points of interest (POIs)
     *
     * @param[in] device The device to write the pois into
     * @exception IOError (not yet implemented)
     */
    void writeTLSasPOIs(OutputDevice &device) throw(IOError);

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
    int     myInternalID;

    /** the map of names to nodes */
    NodeCont   myNodes;

private:
    /** invalid copy constructor */
    NBNodeCont(const NBNodeCont &s);

    /** invalid assignment operator */
    NBNodeCont &operator=(const NBNodeCont &s);
};


#endif

/****************************************************************************/

