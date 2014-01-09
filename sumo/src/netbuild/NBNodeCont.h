/****************************************************************************/
/// @file    NBNodeCont.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Yun-Pang Wang
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Container for nodes during the netbuilding process
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/geom/Position.h>
#include "NBEdgeCont.h"
#include "NBNode.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBDistrict;
class OptionsCont;
class OutputDevice;
class NBJoinedEdgesMap;


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
    NBNodeCont();


    /// @brief Destructor
    ~NBNodeCont();



    /// @name Insertion/removal/retrieval of nodes
    /// @{

    /** @brief Inserts a node into the map
     * @param[in] id The node's id
     * @param[in] position The node's position
     * @param[in] A district assigned to the node
     * @return Whether the node could be added (no other with the same id or position is stored)
     */
    bool insert(const std::string& id, const Position& position,
                NBDistrict* district);


    /** @brief Inserts a node into the map
     * @param[in] id The node's id
     * @param[in] position The node's position
     * @return Whether the node could be added (no other with the same id or position is stored)
     */
    bool insert(const std::string& id, const Position& position);


    /** @brief Inserts a node into the map
     * @param[in] id The node's id
     * @return Whether the node could be added (no other with the same id is stored)
     */
    Position insert(const std::string& id);


    /** @brief Inserts a node into the map
     * @param[in] node The node to insert
     * @return Whether the node could be added (no other with the same id or position is stored)
     */
    bool insert(NBNode* node);


    /** @brief Removes the given node, deleting it
     * @param[in] node The node to delete and remove
     * @return Whether the node could be removed (existed)
     */
    bool erase(NBNode* node);


    /** @brief Removes the given node but does not delete it
     * @param[in] node The node to delete and remove
     * @param[in] remember Whether to keep the node for future reference
     * @return Whether the node could be removed (existed)
     */
    bool extract(NBNode* node, bool remember = false);

    /** @brief Returns the node with the given name
     * @param[in] id The id of the node to retrieve
     * @return The node with the given id, or 0 if no such node exists
     */
    NBNode* retrieve(const std::string& id) const;


    /** @brief Returns the node with the given coordinates
     * @param[in] position The position at which the node to retrieve lies
     * @param[in] offset An offset which can be applied in the case positions are blurred
     * @return The node at the given position, or 0 if no such node exists
     */
    NBNode* retrieve(const Position& position, SUMOReal offset = 0.) const;


    /** @brief Returns the pointer to the begin of the stored nodes
     * @return The iterator to the beginning of stored nodes
     */
    std::map<std::string, NBNode*>::const_iterator begin() const {
        return myNodes.begin();
    }


    /** @brief Returns the pointer to the end of the stored nodes
     * @return The iterator to the end of stored nodes
     */
    std::map<std::string, NBNode*>::const_iterator end() const {
        return myNodes.end();
    }
    /// @}



    /// @name Methods for for joining nodes
    /// @{

    /* @brief add ids of nodes wich shall not be joined
     * @param[in] ids A list of ids to exclude from joining
     * @param[in] check Whether to check if these nodes are known
     * @note checking is off by default because all nodes may not have been loaded yet
     */
    void addJoinExclusion(const std::vector<std::string>& ids, bool check = false);


    /** @brief add ids of nodes which shall be joined into a single node
     * @param[in] cluster The cluster to add
     */
    void addCluster2Join(std::set<std::string> cluster);


    /** @brief Joins loaded junction clusters (see NIXMLNodesHandler)
     */
    unsigned int joinLoadedClusters(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc);


    /** @brief Joins junctions that are very close together
     */
    unsigned int joinJunctions(SUMOReal maxdist, NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc);
    /// @}



    /// @name Adapting the input
    /// @{

    /** @brief Removes self-loop edges (edges where the source and the destination node are the same)
     * @param[in, opt. changed] dc The districts container to update
     * @param[in, opt. changed] ec The edge container to remove the edges from
     * @param[in, opt. changed] tc The traffic lights container to update
     * @post Each edge is a uni-directional connection between two different nodes
     */
    void removeSelfLoops(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tc);


    /** @brief Joins edges connecting the same nodes
     * @param[in, opt. changed] dc The districts container to update
     * @param[in, opt. changed] ec The edge container to remove the edges from
     * @param[in, opt. changed] tc The traffic lights container to update
     * @post No two edges with same geometry connecting same nodes exist
     */
    void joinSimilarEdges(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc);


    /** @brief Removes sequences of edges that are not connected with a junction.
     * Simple roads without junctions sometimes remain when converting from OpenStreetMake,
     * but they make no sense. Remaining empty nodes are also deleted.
     *
     * @param[in, opt. changed] dc The district container needed if edges shall be removed
     * @param[in, opt. changed] ec The container with the edge to be tested
     * @param[in, opt. changed] tc The traffic lights container to update
     */
    void removeIsolatedRoads(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tc);


    /** @brief Removes "unwished" nodes
     *
     * Removes nodes if a) no incoming/outgoing edges exist or
     *  b) if the node is a "geometry" node. In the second case,
     *  edges that participate at the node will be joined.
     * Whether the node is a geometry node or not, is determined
     *  by a call to NBNode::checkIsRemovable.
     * The node is removed from the list of tls-controlled nodes.
     * @param[in, opt. changed] dc The district container needed if a node shall be removed
     * @param[in, opt. changed] ec The edge container needed for joining edges
     * @param[in, opt. changed] je The map of joined edges (changes are stored here)
     * @param[in, opt. changed] tlc The traffic lights container to remove nodes from
     * @param[in] removeGeometryNodes Whether geometry nodes shall also be removed
     * @return The number of removed nodes
     */
    unsigned int removeUnwishedNodes(NBDistrictCont& dc, NBEdgeCont& ec, NBJoinedEdgesMap& je,
                                     NBTrafficLightLogicCont& tlc, bool removeGeometryNodes);
    /// @}



    /// @name Methods for guessing/computing traffic lights
    /// @{

    /** @brief Guesses which junctions or junction clusters shall be controlled by tls
     * @param[in] oc The options that steer the guessing process
     * @param[filled] tlc The traffic lights control into which new traffic light definitions shall be stored
     * @todo Recheck exception handling
     */
    void guessTLs(OptionsCont& oc, NBTrafficLightLogicCont& tlc);


    /** @brief Builds clusters of tls-controlled junctions and joins the control if possible
     * @param[changed] tlc The traffic lights control for adding/removing new/prior tls
     * @param[in] maxdist The maximum distance between nodes for clustering
     * @todo Recheck exception handling
     */
    void joinTLS(NBTrafficLightLogicCont& tlc, SUMOReal maxdist);


    /** @brief Sets the given node as being controlled by a tls
     * @param[in] node The node that shall be controlled by a tls
     * @param[in] tlc The traffic lights control into which the new traffic light definition shall be stored
     * @param[in] type The type of the new tls
     * @param[in] id The id of the tls to add
     * @todo Recheck exception handling
     */
    void setAsTLControlled(NBNode* node, NBTrafficLightLogicCont& tlc, TrafficLightType type, std::string id = "");
    /// @}


    /** @brief Renames the node. Throws exception if newID already exists
     */
    void rename(NBNode* node, const std::string& newID);


    /// divides the incoming lanes on outgoing lanes
    void computeLanes2Lanes();

    /// build the list of outgoing edges and lanes
    void computeLogics(const NBEdgeCont& ec, OptionsCont& oc);

    /** @brief Returns the number of known nodes
     * @return The number of nodes stored in this container
     */
    unsigned int size() const {
        return (unsigned int) myNodes.size();
    }

    /** deletes all nodes */
    void clear();



    std::string getFreeID();

    /** @brief Compute the junction shape for this node
     * @param[in] lefhand Whether the network uses left-hand traffic
     * @param[in] mismatchThreshold The threshold for warning about shapes which are away from myPosition
     */
    void computeNodeShapes(bool leftHand, SUMOReal mismatchThreshold=std::numeric_limits<SUMOReal>::max());

    /** @brief Prints statistics about built nodes
     *
     * Goes through stored nodes, computes the numbers of unregulated, priority and right-before-left
     *  junctions and prints them.
     */
    void printBuiltNodesStatistics() const;


    /// @brief get all node names
    std::vector<std::string> getAllNames() const;


    /* @brief analyzes a cluster of nodes which shall be joined
     * @param[in] cluster The nodes to be joined
     * @param[out] id The name for the new node
     * @param[out] pos The position of the new node
     * @param[out] hasTLS Whether the new node has a traffic light
     * @param[out] tlType The type of traffic light (if any)
     */
    void analyzeCluster(std::set<NBNode*> cluster, std::string& id, Position& pos,
                        bool& hasTLS, TrafficLightType& type);

    /// @brief gets all joined clusters (see doc for myClusters2Join)
    void registerJoinedCluster(const std::set<NBNode*>& cluster);

    /// @brief gets all joined clusters (see doc for myClusters2Join)
    const std::vector<std::set<std::string> >& getJoinedClusters() const {
        return myJoinedClusters;
    }


    /* @brief discards traffic lights
     * @param[in] geometryLike Whether only tls at geometry-like nodes shall be discarded
     */
    void discardTrafficLights(NBTrafficLightLogicCont& tlc, bool geometryLike);

private:
    /// @name Helper methods for for joining nodes
    /// @{

    /// @brief Definition of a node cluster container
    typedef std::vector<std::set<NBNode*> > NodeClusters;


    /** @brief Builds node clusters
     *
     * A node cluster is made up from nodes which are near by (distance<maxDist) and connected.
     *
     * @param[in] maxDist The maximum distance between two nodes for clustering
     * @param[in, filled] into The container to store the clusters in
     */
    void generateNodeClusters(SUMOReal maxDist, NodeClusters& into) const;


    // @brief joins the given node clusters
    void joinNodeClusters(NodeClusters clusters,
                          NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc);

    /// @}



    /// @name Helper methods for guessing/computing traffic lights
    /// @{

    /** @brief Returns whethe the given node cluster should be controlled by a tls
     * @param[in] c The node cluster
     * @return Whether this node cluster shall be controlled by a tls
     */
    bool shouldBeTLSControlled(const std::set<NBNode*>& c) const;
    /// @}


private:
    /// @brief The running internal id
    int myInternalID;

    /// @brief Definition of the map of names to nodes
    typedef std::map<std::string, NBNode*> NodeCont;

    /// @brief The map of names to nodes
    NodeCont myNodes;

    /// @brief The extracted nodes which are kept for reference
    std::set<NBNode*> myExtractedNodes;

    // @brief set of node ids which should not be joined
    std::set<std::string> myJoinExclusions;

    // @brief loaded sets of node ids to join (cleared after use)
    std::vector<std::set<std::string> > myClusters2Join;
    // @brief sets of node ids which were joined
    std::vector<std::set<std::string> > myJoinedClusters;

    /// @brief ids found in loaded join clusters used for error checking
    std::set<std::string> myJoined;

private:
    /// @brief invalidated copy constructor
    NBNodeCont(const NBNodeCont& s);

    /// @brief invalidated assignment operator
    NBNodeCont& operator=(const NBNodeCont& s);

};


#endif

/****************************************************************************/

