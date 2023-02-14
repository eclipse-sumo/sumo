/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NBNodeCont.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
///
// Container for nodes during the netbuilding process
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>
#include <vector>
#include <set>
#include <utils/common/NamedRTree.h>
#include <utils/geom/Position.h>
#include "NBCont.h"
#include "NBEdgeCont.h"
#include "NBNode.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBDistrict;
class OptionsCont;
class OutputDevice;
class NBParkingCont;
class NBPTLineCont;
class NBPTStopCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNodeCont
 * @brief Container for nodes during the netbuilding process
 */
class NBNodeCont {

public:
    /// @brief Definition of a node cluster container
    typedef std::vector<NodeSet> NodeClusters;
    typedef std::pair<NBNode*, double> NodeAndDist;

    /// @brief Constructor
    NBNodeCont() {}

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
    bool insert(const std::string& id, const Position& position, NBDistrict* district = 0);

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
    NBNode* retrieve(const Position& position, const double offset = 0.) const;

    /// @brief Returns the pointer to the begin of the stored nodes
    std::map<std::string, NBNode*>::const_iterator begin() const {
        return myNodes.begin();
    }

    /// @brief Returns the pointer to the end of the stored nodes
    std::map<std::string, NBNode*>::const_iterator end() const {
        return myNodes.end();
    }
    /// @}

    /// @name Methods for joining nodes
    /// @{
    /* @brief add ids of nodes wich shall not be joined
     * @param[in] ids A list of ids to exclude from joining
     * @note it does not check whether the nodes exist because all nodes may not have been loaded yet
     */
    void addJoinExclusion(const std::vector<std::string>& ids);

    /** @brief generate id from cluster node ids
     * @param[in] cluster The cluster ids
     * @param[in] prefix The cluster prefix
     * @return the generated id
     */
    std::string createClusterId(const NodeSet& cluster, const std::string& prefix = "cluster_") {
        std::set<std::string> clusterIds;
        for (NBNode* j : cluster) {
            clusterIds.insert(j->getID());
        }
        return createClusterId(clusterIds, prefix);
    }

    /** @brief generate id from cluster node ids
     * @param[in] cluster The cluster ids
     * @param[in] prefix The cluster prefix
     * @return the generated id
     */
    std::string createClusterId(const std::set<std::string>& cluster, const std::string& prefix = "cluster_");

    /** @brief add ids of nodes which shall be joined into a single node
     * @param[in] cluster The cluster to add
     */
    void addCluster2Join(const std::set<std::string>& cluster, NBNode* node);

    /// @brief Joins loaded junction clusters (see NIXMLNodesHandler)
    int joinLoadedClusters(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc);

    /// @brief Joins junctions that are very close together
    int joinJunctions(double maxDist, NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc, NBPTStopCont& sc);

    /// @brief Joins junctions with the same coordinates regardless of topology
    int joinSameJunctions(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc);

    /// @brief return all cluster neighbors for the given node
    static NodeSet getClusterNeighbors(const NBNode* n, double longThreshold, NodeSet& cluster);

    /// @brief whether the given node may continue a slip lane
    static bool isSlipLaneContinuation(const NBNode* cont);

    /// @brief check whether the given node maybe the start of a slip lane
    bool maybeSlipLaneStart(const NBNode* n, EdgeVector& outgoing, double& inAngle) const;
    /// @brief check whether the given node maybe the end of a slip lane
    bool maybeSlipLaneEnd(const NBNode* n, EdgeVector& incoming, double& outAngle) const;

    /// @brief try to find a joinable subset (recursively)
    bool reduceToCircle(NodeSet& cluster, int circleSize, NodeSet startNodes, std::vector<NBNode*> cands = std::vector<NBNode*>()) const;

    /// @brief find closest neighbor for building circle
    NBEdge* shortestEdge(const NodeSet& cluster, const NodeSet& startNodes, const std::vector<NBNode*>& exclude) const;
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
    void joinSimilarEdges(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc, bool removeDuplicates);

    /// @brief fix overlap
    void avoidOverlap();

    /** @brief Removes sequences of edges that are not connected with a junction.
     * Simple roads without junctions sometimes remain when converting from OpenStreetMap,
     * but they make no sense. Remaining empty nodes are also deleted.
     *
     * @param[in, opt. changed] dc The district container needed if edges shall be removed
     * @param[in, opt. changed] ec The container with the edge to be tested
     */
    void removeIsolatedRoads(NBDistrictCont& dc, NBEdgeCont& ec);

    /** @brief Checks the network for weak connectivity and removes all but the largest components.
     * The connectivity check is done regardless of edge direction and vclass.
     *
     * @param[in, opt. changed] dc The district container needed if edges shall be removed
     * @param[in, opt. changed] ec The container with the edge to be tested
     * @param[in] numKeep The number of components to keep
     */
    void removeComponents(NBDistrictCont& dc, NBEdgeCont& ec, const int numKeep, bool hasPTStops);

    /// @brief remove rail components after ptstops are built
    void removeRailComponents(NBDistrictCont& dc, NBEdgeCont& ec, NBPTStopCont& sc);

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
     * @param[in, opt. changed] tlc The traffic lights container to remove nodes from
     * @param[in, opt. changed] sc The pt stops container to update stop edges
     * @param[in, opt. changed] pc The pt stops container to update stop edges
     * @param[in] removeGeometryNodes Whether geometry nodes shall also be removed
     * @return The number of removed nodes
     */
    int removeUnwishedNodes(NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc,
                            NBPTStopCont& sc, NBPTLineCont& lc, NBParkingCont& pc,
                            bool removeGeometryNodes);
    /// @}

    /// @name Methods for guessing/computing traffic lights
    /// @{
    /** @brief Guesses which junctions or junction clusters shall be controlled by tls
     * @param[in] oc The options that steer the guessing process
     * @param[filled] tlc The traffic lights control into which new traffic light definitions shall be stored
     * @todo Recheck exception handling
     */
    void guessTLs(OptionsCont& oc, NBTrafficLightLogicCont& tlc);

    /// @brief recheck myGuessedTLS after node logics are computed
    void recheckGuessedTLS(NBTrafficLightLogicCont& tlc);

    /// @brief check whether a specific guessed tls should keep its type
    bool recheckTLSThreshold(NBNode* node);

    /// @brief compute keepClear status for all connections
    void computeKeepClear();

    /** @brief Builds clusters of tls-controlled junctions and joins the control if possible
     * @param[changed] tlc The traffic lights control for adding/removing new/prior tls
     * @param[in] maxdist The maximum distance between nodes for clustering
     * @todo Recheck exception handling
     */
    void joinTLS(NBTrafficLightLogicCont& tlc, double maxdist);

    /** @brief Sets the given node as being controlled by a tls
     * @param[in] node The node that shall be controlled by a tls
     * @param[in] tlc The traffic lights control into which the new traffic light definition shall be stored
     * @param[in] type The type of the new tls
     * @param[in] id The id of the tls to add
     * @todo Recheck exception handling
     */
    void setAsTLControlled(NBNode* node, NBTrafficLightLogicCont& tlc, TrafficLightType type, std::string id = "");
    /// @}

    /// @brief Returns whether the node with the id was deleted explicitly
    bool wasRemoved(std::string id) const {
        return myExtractedNodes.count(id) != 0;
    }

    /// @brief add prefix to all nodes
    void addPrefix(const std::string& prefix);

    /// @brief Renames the node. Throws exception if newID already exists
    void rename(NBNode* node, const std::string& newID);

    /// divides the incoming lanes on outgoing lanes
    void computeLanes2Lanes();

    /// @brief build the list of outgoing edges and lanes
    void computeLogics(const NBEdgeCont& ec);

    /// @brief compute right-of-way logic for all lane-to-lane connections
    void computeLogics2(const NBEdgeCont& ec, OptionsCont& oc);

    /// @brief Returns the number of nodes stored in this container
    int size() const {
        return (int) myNodes.size();
    }

    /// @brief deletes all nodes
    void clear();

    /// @brief generates a new node ID
    std::string getFreeID();

    /** @brief Compute the junction shape for this node
     * @param[in] mismatchThreshold The threshold for warning about shapes which are away from myPosition
     */
    void computeNodeShapes(double mismatchThreshold = -1);

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
    void analyzeCluster(NodeSet cluster, std::string& id, Position& pos,
                        bool& hasTLS, TrafficLightType& type, SumoXMLNodeType& nodeType);

    /// @brief gets all joined clusters (see doc for myClusters2Join)
    void registerJoinedCluster(const NodeSet& cluster);

    /// @brief gets all joined clusters (see doc for myClusters2Join)
    const std::vector<std::set<std::string> >& getJoinedClusters() const {
        return myJoinedClusters;
    }

    /* @brief discards traffic lights
     * @param[in] geometryLike Whether only tls at geometry-like nodes shall be discarded
     */
    void discardTrafficLights(NBTrafficLightLogicCont& tlc, bool geometryLike, bool guessSignals);

    /// @brief discards rail signals
    void discardRailSignals();

    /// @brief mark a node as being created form a split
    void markAsSplit(const NBNode* node) {
        mySplit.insert(node);
    }

    /// @brief mark a node as explicitly not controlled by a TLS
    void markAsNotTLS(const NBNode* node) {
        myUnsetTLS.insert(node);
    }

    /// @brief remap node IDs accoring to options --numerical-ids and --reserved-ids
    int remapIDs(bool numericaIDs, bool reservedIDs, const std::string& prefix, NBTrafficLightLogicCont& tlc);

    /// @brief guess and mark fringe nodes
    int guessFringe();

    /// @brief apply default values after loading
    void applyConditionalDefaults();

    /// @brief reset all node shapes
    bool resetNodeShapes();

private:

    /// @name Helper methods for for joining nodes
    /// @{
    /** @brief Builds node clusters
     *
     * A node cluster is made up from nodes which are near by (distance<maxDist) and connected.
     *
     * @param[in] maxDist The maximum distance between two nodes for clustering
     * @param[in, filled] into The container to store the clusters in
     */
    void generateNodeClusters(double maxDist, NodeClusters& into) const;

    /// @brief remove geometry-like fringe nodes from cluster
    void pruneClusterFringe(NodeSet& cluster) const;

    /// @brief avoid removal of long edges when joining junction clusters
    static int pruneLongEdges(NodeSet& cluster, double maxDist, const bool dryRun = false);

    /// @brief remove nodes that form a slip lane from cluster
    void pruneSlipLaneNodes(NodeSet& cluster) const;

    /// @brief determine wether the cluster is not too complex for joining
    bool feasibleCluster(const NodeSet& cluster, const std::map<const NBNode*, std::vector<NBNode*> >& ptStopEnds,
                         double maxDist, std::string& reason) const;

    /// @brief joins the given node clusters
    void joinNodeClusters(NodeClusters clusters, NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc, bool resetConnections = false);
    void joinNodeCluster(NodeSet clusters, NBDistrictCont& dc, NBEdgeCont& ec, NBTrafficLightLogicCont& tlc,
                         NBNode* predefined = nullptr, bool resetConnections = false);

    /// @}

    /// @name Helper methods for guessing/computing traffic lights
    /// @{
    /** @brief Returns whethe the given node cluster should be controlled by a tls
     * @param[in] c The node cluster
     * @param[in] laneSpeedThreshold threshold for determining whether a node or cluster should be tls controlled
     * @return Whether this node cluster shall be controlled by a tls
     */
    bool shouldBeTLSControlled(const NodeSet& c, double laneSpeedThreshold, bool recheck = false) const;

    /// @brief check wheter the set of nodes only contains pedestrian crossings
    bool onlyCrossings(const NodeSet& c) const;

    /// @brief check wheter the set of nodes contains traffic lights with custom id
    bool customTLID(const NodeSet& c) const;
    /// @}

    /// @brief update pareto frontier with the given node
    void paretoCheck(NBNode* node, NodeSet& frontier, int xSign, int ySign);

    /// @brief Definition of the map of names to nodes
    typedef std::map<std::string, NBNode*> NodeCont;

    /// @brief The map of names to nodes
    NodeCont myNodes;

    /// @brief The extracted nodes which are kept for reference
    NodeCont myExtractedNodes;

    /// @brief set of node ids which should not be joined
    std::set<std::string> myJoinExclusions;

    /// @brief loaded sets of node ids to join (cleared after use)
    std::vector<std::pair<std::set<std::string>, NBNode*> > myClusters2Join;

    /// @brief sets of node ids which were joined
    std::vector<std::set<std::string> > myJoinedClusters;

    /// @brief ids found in loaded join clusters used for error checking
    std::set<std::string> myJoined;

    /// @brief nodes that were created when splitting an edge
    std::set<const NBNode*> mySplit;

    /// @brief nodes that received a traffic light due to guessing (--tls.guess)
    std::set<NBNode*, ComparatorIdLess> myGuessedTLS;

    /// @brief nodes that are excluded from tls-guessing
    std::set<const NBNode*> myUnsetTLS;

    /// @brief node positions for faster lookup
    NamedRTree myRTree;

    /// @brief network components that must be removed if not connected to the road network via stop access
    std::vector<std::vector<std::string> > myRailComponents;

    /// @brief invalidated copy constructor
    NBNodeCont(const NBNodeCont& s) = delete;

    /// @brief invalidated assignment operator
    NBNodeCont& operator=(const NBNodeCont& s) = delete;
};
