/****************************************************************************/
/// @file    NBNodeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Container for nodes during the netbuilding process
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <vector>
#include <set>
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
class NBNodeCont {
public:
    /// @brief Constructor
    NBNodeCont() throw();


    /// @brief Destructor
    ~NBNodeCont() throw();



    /// @name Insertion/removal/retrieval of nodes
    /// @{

    /** @brief Inserts a node into the map
     * @param[in] id The node's id
     * @param[in] position The node's position
     * @param[in] A district assigned to the node
     * @return Whether the node could be added (no other with the same id or position is stored)
     */
    bool insert(const std::string &id, const Position2D &position,
                NBDistrict *district) throw();


    /** @brief Inserts a node into the map
     * @param[in] id The node's id
     * @param[in] position The node's position
     * @return Whether the node could be added (no other with the same id or position is stored)
     */
    bool insert(const std::string &id, const Position2D &position) throw();


    /** @brief Inserts a node into the map
     * @param[in] id The node's id
     * @return Whether the node could be added (no other with the same id is stored)
     */
    Position2D insert(const std::string &id) throw();


    /** @brief Inserts a node into the map
     * @param[in] node The node to insert
     * @return Whether the node could be added (no other with the same id or position is stored)
     */
    bool insert(NBNode *node) throw();


    /** @brief Removes the given node, deleting it
     * @param[in] node The node to delete and remove
     * @return Whether the node could be removed (existed)
     */
    bool erase(NBNode *node) throw();


    /** @brief Returns the node with the given name
     * @param[in] id The id of the node to retrieve
     * @return The node with the given id, or 0 if no such node exists
     */
    NBNode *retrieve(const std::string &id) const throw();


    /** @brief Returns the node with the given coordinates
     * @param[in] position The position at which the node to retrieve lies
     * @param[in] offset An offset which can be applied in the case positions are blurred
     * @return The node at the given position, or 0 if no such node exists
     */
    NBNode *retrieve(const Position2D &position, SUMOReal offset=0.) const throw();
    /// @}



    /// @name Methods for guessing/computing traffic lights
    /// @{

    /** @brief Guesses which junctions or junction clusters shall be controlled by tls
     * @param[in] oc The options that steer the guessing process
     * @param[filled] tlc The traffic lights control into which new traffic light definitions shall be stored
     * @todo Recheck exception handling
     */
    void guessTLs(OptionsCont &oc, NBTrafficLightLogicCont &tlc);


    /** @brief Builds clusters of tls-controlled junctions and joins the control if possible
     * @param[changed] tlc The traffic lights control for adding/removing new/prior tls
     * @todo Recheck exception handling
     */
    void joinTLS(NBTrafficLightLogicCont &tlc);


    /** @brief Sets the given node as being controlled by a tls
     * @param[in] node The node that shall be controlled by a tls
     * @param[in] tlc The traffic lights control into which the new traffic light definition shall be stored
     * @param[in] id The id of the tls to add
     * @todo Recheck exception handling
     */
    void setAsTLControlled(NBNode *node, NBTrafficLightLogicCont &tlc, std::string id="");
    /// @}

    /// resets the node positions by the specified offset
    void reshiftNodePositions(const SUMOReal xoff, const SUMOReal yoff);

    /// divides the incoming lanes on outgoing lanes
    void computeLanes2Lanes();

    /// build the list of outgoing edges and lanes
    void computeLogics(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
                       OptionsCont &oc);

    /// sorts the nodes' edges
    void sortNodesEdges(bool leftHand, const NBTypeCont &tc);

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

    /** @brief Removes sequences of edges that are not connected with a junction.
     * Simple roads without junctions sometimes remain when converting from OpenStreetMake,
     * but they make no sense. Remaining empty nodes are also deleted.
     *
     * @param[in] dc The district container needed if edges shall be removed
     * @param[in] ec The container with the edge to be tested
     */
    void removeIsolatedRoads(NBDistrictCont &dc, NBEdgeCont &ec, NBTrafficLightLogicCont &tc);

    /// Removes dummy edges (edges lying completely within a node)
    void removeDummyEdges(NBDistrictCont &dc, NBEdgeCont &ec,
                          NBTrafficLightLogicCont &tc);

    std::string getFreeID();

    void computeNodeShapes(bool leftHand);

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

    bool savePlain(const std::string &file);

    /** @brief Writes positions of traffic lights as a list of points of interest (POIs)
     *
     * @param[in] device The device to write the pois into
     * @exception IOError (not yet implemented)
     */
    void writeTLSasPOIs(OutputDevice &device) throw(IOError);


    /** @brief Prints statistics about built nodes
     *
     * Goes through stored nodes, computes the numbers of unregulated, priority and right-before-left
     *  junctions and prints them.
     */
    void printBuiltNodesStatistics() const throw();


private:
    bool mayNeedOnRamp(OptionsCont &oc, NBNode *cur) const;
    bool mayNeedOffRamp(OptionsCont &oc, NBNode *cur) const;
    bool buildOnRamp(OptionsCont &oc, NBNode *cur,
                     NBEdgeCont &ec, NBDistrictCont &dc, std::vector<NBEdge*> &incremented);

    void buildOffRamp(OptionsCont &oc, NBNode *cur,
                      NBEdgeCont &ec, NBDistrictCont &dc, std::vector<NBEdge*> &incremented);

    void checkHighwayRampOrder(NBEdge *&pot_highway, NBEdge *&pot_ramp);


    /// @name Helper methods for guessing/computing traffic lights
    /// @{

    /** @brief Builds node clusters
     *
     * A node cluster is made up from nodes which are near by (distance<maxDist) and connected.
     *
     * @param[in] maxDist The maximum distance between two nodes for clustering
     * @param[in, filled] into The container to store the clusters in
     */
    void generateNodeClusters(SUMOReal maxDist, std::vector<std::set<NBNode*> >&into) const throw();


    /** @brief Returns whethe the given node cluster should be controlled by a tls
     * @param[in] c The node cluster
     * @return Whether this node cluster shall be controlled by a tls
     */
    bool shouldBeTLSControlled(const std::set<NBNode*> &c) const throw();
    /// @}

    /// @brief The running internal id
    int myInternalID;

    /// @brief Definition of the map of names to nodes
    typedef std::map<std::string, NBNode*> NodeCont;

    /// @brief The map of names to nodes
    NodeCont myNodes;

    /// @brief invalidated copy constructor
    NBNodeCont(const NBNodeCont &s);

    /// @brief invalidated assignment operator
    NBNodeCont &operator=(const NBNodeCont &s);
};


#endif

/****************************************************************************/

