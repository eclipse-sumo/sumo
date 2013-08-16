/****************************************************************************/
/// @file    NBEdgeCont.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Storage for edges, including some functionality operating on multiple edges
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
#include <set>
#include "NBCont.h"
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/PositionVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNodeCont;
class NBTypeCont;
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
 * @brief Storage for edges, including some functionality operating on multiple edges
 */
class NBEdgeCont {
public:
    /** @brief Constructor
     * @param[in] tc The net builded; used to obtain types
     */
    NBEdgeCont(NBTypeCont& tc);


    /// @brief Destructor
    ~NBEdgeCont();


    /** @brief Initialises the storage by applying given options
     *
     * Options, mainly steering the acceptance of edges, are parsed
     *  and the according internal variables are set.
     *
     * @param[in] oc The options container to read options from
     * @todo Recheck exceptions
     */
    void applyOptions(OptionsCont& oc);


    /** @brief Deletes all edges */
    void clear();




    /// @name edge access methods
    /// @{

    /** @brief Adds an edge to the dictionary
     *
     * First, it is determined whether the edge shall not be discarded due to any
     *  reasons (being outside a boundary, or not in the optional list of edges to
     *  import, etc.). If so, the edge is deleted and "true" is returned.
     * "true" is also returned if the edge is accepted - no edge with the same
     *  name exists within this container. If another edge with the same name
     *  exists, false is returned.
     *
     * @param[in] edge The edge to add
     * @param[in] ignorePrunning Whether this edge must not be prunned
     * @return Whether the edge was valid (no edge with the same id is already known)
     */
    bool insert(NBEdge* edge, bool ignorePrunning = false);


    /** @brief Returns the edge that has the given id
     *
     * If no edge that has the given id is known, 0 is returned.
     *
     * @param[in] id The id of the edge to retrieve
     * @param[in] bool Whether extracted edges shall be retrieved as well
     * @return The edge with the given id, 0 if no such edge exists
     */
    NBEdge* retrieve(const std::string& id, bool retrieveExtracted = false) const;


    /** @brief Tries to retrieve an edge, even if it is splitted
     *
     * To describe which part of the edge shall be returned, the
     *  id of a second edge, participating at the node and the information
     *  whether to return the outgoing or the incoming is needed.
     *
     * @param[in] id The id of the edge to retrieve
     * @param[in] hint An additional information which helps to retrieve the correct edge
     * @param[in] incoming Whether the edge to find is an incoming edge at the node "hint" participates
     * @return The searched edge
     * @todo Recheck usage
     */
    NBEdge* retrievePossiblySplit(const std::string& id, const std::string& hint, bool incoming) const;


    /** @brief Tries to retrieve an edge, even if it is splitted
     *
     * To describe which part of the edge shall be returned, a
     *  position hint is supplied.
     *
     * @param[in] id The id of the edge to retrieve
     * @param[in] pos An additional about the position of the basic edge's subpart
     * @return The searched edge
     * @todo Recheck usage
     */
    NBEdge* retrievePossiblySplit(const std::string& id, SUMOReal pos) const;


    /** @brief Removes the given edge from the container (deleting it)
     *
     * @param[in] dc The district container, in order to remove the edge from sources/sinks
     * @param[in] edge The edge to remove
     * @todo Recheck whether the district cont is needed - if districts are processed using an external tool
     */
    void erase(NBDistrictCont& dc, NBEdge* edge);


    /** @brief Removes the given edge from the container like erase but does not
     * delete it
     *
     * @param[in] dc The district container, in order to remove the edge from sources/sinks
     * @param[in] edge The edge to remove
     * @param[in] remember Whether to keep this edge for future reference
     * @todo Recheck whether the district cont is needed - if districts are processed using an external tool
     * @todo Recheck whether this is used at all and why
     */
    void extract(NBDistrictCont& dc, NBEdge* edge, bool remember = false);


    /** @brief Returns the pointer to the begin of the stored edges
     * @return The iterator to the beginning of stored edges
     */
    std::map<std::string, NBEdge*>::const_iterator begin() const {
        return myEdges.begin();
    }


    /** @brief Returns the pointer to the end of the stored edges
     * @return The iterator to the end of stored edges
     */
    std::map<std::string, NBEdge*>::const_iterator end() const {
        return myEdges.end();
    }
    /// @}



    /// @name explicit edge manipulation methods
    /// @{

    /** @brief Splits the edge at the position nearest to the given node
     *
     * Uses "splitAt(NBDistrictCont &, NBEdge *, NBNode *, const std::string &, const std::string &, unsigned int , unsigned int)"
     *  to perform the split; the edge names are built by appending "[0]" and "[1]",
     *  respectively. Both edges will have the same number of lanes.
     *
     * @param[in] dc The district container, in order to remove/add the edge from/to sources/sinks
     * @param[in] edge The edge to split
     * @param[in] node The node to split the edge at
     * @return Whether the edge could be split
     * @exception ProcessError If connections between the edges can not be built
     * @see NBEdge::splitAt(NBDistrictCont &, NBEdge *, NBNode *, const std::string &, const std::string &, unsigned int , unsigned int)
     */
    bool splitAt(NBDistrictCont& dc, NBEdge* edge, NBNode* node);


    /** @brief Splits the edge at the position nearest to the given node using the given modifications
     *
     * Determines the position of the split by finding the nearest position on the
     *  edge to the given node. If this position is too near to the edges begin/end,
     *  false is returned.
     *
     * Otherwise, "splitAt(NBDistrictCont &, NBEdge *, SUMOReal, NBNode *, const std::string &, const std::string &, unsigned int , unsigned int)"
     *  is used to perform the split.
     *
     * @param[in] dc The district container, in order to remove/add the edge from/to sources/sinks
     * @param[in] edge The edge to split
     * @param[in] node The node to split the edge at
     * @param[in] firstEdgeName The id the first part of the split edge shall have
     * @param[in] secondEdgeName The id the second part of the split edge shall have
     * @param[in] noLanesFirstEdge The number of lanes the second part of the split edge shall have
     * @param[in] noLanesSecondEdge The number of lanes the second part of the split edge shall have
     * @return Whether the edge could be split
     * @exception ProcessError If connections between the edges can not be built
     * @see NBEdge::splitAt(NBDistrictCont &, NBEdge *, SUMOReal, NBNode *, const std::string &, const std::string &, unsigned int , unsigned int)
     */
    bool splitAt(NBDistrictCont& dc, NBEdge* edge, NBNode* node,
                 const std::string& firstEdgeName, const std::string& secondEdgeName,
                 unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge);


    /** @brief Splits the edge at the position nearest to the given node using the given modifications
     *
     * @param[in] dc The district container, in order to remove/add the edge from/to sources/sinks
     * @param[in] edge The edge to split
     * @param[in] node The node to split the edge at
     * @param[in] firstEdgeName The id the first part of the split edge shall have
     * @param[in] secondEdgeName The id the second part of the split edge shall have
     * @param[in] noLanesFirstEdge The number of lanes the second part of the split edge shall have
     * @param[in] noLanesSecondEdge The number of lanes the second part of the split edge shall have
     * @return Whether the edge could be split
     * @exception ProcessError If connections between the edges can not be built
     */
    bool splitAt(NBDistrictCont& dc, NBEdge* edge, SUMOReal edgepos, NBNode* node,
                 const std::string& firstEdgeName, const std::string& secondEdgeName,
                 unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge);
    /// @}



    /// @name container access methods
    /// @{

    /** @brief Returns the number of edges
     * @return The number of edges stored in this container
     */
    unsigned int size() const {
        return (unsigned int) myEdges.size();
    }


    /** @brief Returns all ids of known edges
     * @return All ids of known edges
     * @todo Recheck usage, probably, filling a given vector would be better...
     */
    std::vector<std::string> getAllNames() const;


    /** @brief Returns the number of edge splits
     * @return How often an edge was split
     */
    unsigned int getNoEdgeSplits() const {
        return myEdgesSplit;
    }
    /// @}



    /// @name Adapting the input
    /// @{

    /** @brief Removes unwished edges (not in keep-edges)
     * @param[in, opt. changed] dc The district container needed to remove edges
     * @todo Recheck usage; check whether keep-edges.postload is really useful
     */
    void removeUnwishedEdges(NBDistrictCont& dc);


    /** @brief Splits edges into multiple if they have a complex geometry
     *
     * Calls "NBEdge::splitGeometry" for all edges within the container which
     *  have more than three positions in their geometry.
     *
     * @param[in] nc The node container needed to build (geometry) nodes
     * @see NBEdge::splitGeometry
     */
    void splitGeometry(NBNodeCont& nc);


    /** @brief
     * @param[in] nc The node container needed to build (geometry) nodes
     * @see NBEdge::reduceGeometry
     */
    void reduceGeometries(const SUMOReal minDist);


    /** @brief
     * @param[in] maxAngle The maximum geometry angle allowed
     * @param[in] minRadius The minimum turning radius allowed at the start and end
     * @param[in] fix Whether to prune geometry points to avoid sharp turns at start and end
     * @see NBEdge::checkGeometry
     */
    void checkGeometries(const SUMOReal maxAngle, const SUMOReal minRadius, bool fix);
    /// @}



    /// @name processing methods
    /// @{

    /** @brief Sorts all lanes of all edges within the container by their direction
     *
     * Calls "NBEdge::sortOutgoingLanesConnections" for all edges within the container.
     *
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @see NBEdge::sortOutgoingLanesConnections
     */
    void sortOutgoingLanesConnections();


    /** @brief Computes for each edge the approached edges
     *
     * Calls "NBEdge::computeEdge2Edges" for all edges within the container.
     *
     * @param[in] noLeftMovers Whether left-moving connections shall be omitted
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @see NBEdge::computeEdge2Edges
     */
    void computeEdge2Edges(bool noLeftMovers);


    /** @brief Computes for each edge which lanes approach the next edges
     *
     * Calls "NBEdge::computeLanes2Edges" for all edges within the container.
     *
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @see NBEdge::computeLanes2Edges
     */
    void computeLanes2Edges();


    /** @brief Rechecks whether all lanes have a successor for each of the stored edges
     *
     * Calls "NBEdge::recheckLanes" for all edges within the container.
     *
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @see NBEdge::recheckLanes
     */
    void recheckLanes();


    /** @brief Appends turnarounds to all edges stored in the container
     *
     * Calls "NBEdge::appendTurnaround" for all edges within the container.
     *
     * @param[in] noTLSControlled Whether the turnaround shall not be connected if the edge is controlled by a tls
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @see NBEdge::appendTurnaround
     */
    void appendTurnarounds(bool noTLSControlled);


    /** @brief Appends turnarounds to all edges stored in the container
     * Calls "NBEdge::appendTurnaround" for edges with the given ids
     * @param[in] ids The list of ids for which to append a turnaround
     * @param[in] noTLSControlled Whether the turnaround shall not be connected if the edge is controlled by a tls
     * @see NBEdge::appendTurnaround
     */
    void appendTurnarounds(const std::set<std::string>& ids, bool noTLSControlled);


    /** @brief Computes the shapes of all edges stored in the container
     *
     * Calls "NBEdge::computeEdgeShape" for all edges within the container.
     *
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @todo Recheck usage
     * @see NBEdge::computeEdgeShape
     */
    void computeEdgeShapes();


    /** @brief Computes the shapes of all lanes of all edges stored in the container
     *
     * Calls "NBEdge::computeLaneShapes" for all edges within the container.
     *
     * @todo Recheck whether a visitor-pattern should be used herefor
     * @todo Recheck usage
     * @see NBEdge::computeLaneShapes
     */
    void computeLaneShapes();


    /** @brief Clears information about controlling traffic lights for all connenections of all edges
     */
    void clearControllingTLInformation() const;


    /** @brief Joins the given edges because they connect the same nodes
     *
     * @param[in] dc The district container needed to remove edges
     * @param[in] tlc The tls container needed to remove edges
     * @param[in] edges The edges to join
     * @todo Recheck and describe usage
     */
    void joinSameNodeConnectingEdges(NBDistrictCont& dc,
                                     NBTrafficLightLogicCont& tlc, EdgeVector edges);


    /** @brief Rechecks whether the lane spread is proper
     *
     * @todo Recheck usage; check whether this is really needed and whether it works at all
     */
    void recheckLaneSpread();
    /// @}



    /** @brief Determines which edges belong to roundabouts and increases their priority
     * @param[out] marked Edges which belong to a roundabout are stored here
     */
    void guessRoundabouts(std::vector<EdgeVector>& marked);


    /** @brief Returns whether the built edges are left-handed
     * @return Whether this edge container is left-handed
     */
    bool isLeftHanded() const {
        return myAmLeftHanded;
    }


    /** @brief Returns whether the edge with the id was ignored during parsing
     * @return Whether the edge with the id was ignored during parsing
     */
    bool wasIgnored(std::string id) const {
        return myIgnoredEdges.count(id) != 0;
    }


    /** @brief Returns whether the edge with the id was deleted explicitly
     */
    bool wasRemoved(std::string id) const {
        return myExtractedEdges.count(id) != 0;
    }

    /** @brief Renames the edge. Throws exception if newID already exists
     */
    void rename(NBEdge* edge, const std::string& newID);



    /// @name Connections handling
    /// @{

    /** @brief Adds a connection which could not be set during loading
     * @param[in] from The id of the edge the connection starts at
     * @param[in] fromLane The number of the lane the connection starts at
     * @param[in] to The id of the edge the connection ends at
     * @param[in] toLane The number of the lane the connection ends at
     * @param[in] mayDefinitelyPass Whether the connection may be passed without braking
     */
    void addPostProcessConnection(const std::string& from, int fromLane, const std::string& to, int toLane, bool mayDefinitelyPass);


    /** @brief Try to set any stored connections
     */
    void recheckPostProcessConnections();
    /// @}

    /// @brief assigns street signs to edges based on toNode types
    void generateStreetSigns();

private:
    /** @brief Returns the edges which have been built by splitting the edge of the given id
     *
     * @param[in] id The id of the original edge
     * @return List of all edges which have been built by splitting the original edge
     * @todo Recheck usage
     */
    EdgeVector getGeneratedFrom(const std::string& id) const;


    /// @brief Returns true if this edge matches one of the removal criteria
    bool ignoreFilterMatch(NBEdge* edge);


private:
    /** @struct PostProcessConnection
     * @brief A structure representing a connection between two lanes
     */
    struct PostProcessConnection {
    public:
        /** @brief Constructor
         * @param[in] from The id of the edge the connection starts at
         * @param[in] fromLane The number of the lane the connection starts at
         * @param[in] to The id of the edge the connection ends at
         * @param[in] toLane The number of the lane the connection ends at
         * @param[in] mayDefinitelyPass Whether the connection may be passed without braking
         */
        PostProcessConnection(const std::string& from_, int fromLane_, const std::string& to_, int toLane_, bool mayDefinitelyPass_)
            : from(from_), fromLane(fromLane_), to(to_), toLane(toLane_), mayDefinitelyPass(mayDefinitelyPass_)
        { }
        /// @brief The id of the edge the connection starts at
        std::string from;
        /// @brief The number of the lane the connection starts at
        int fromLane;
        /// @brief The id of the edge the connection ends at
        std::string to;
        /// @brief The number of the lane the connection ends at
        int toLane;
        /// @brief Whether the connection may be passed without braking
        bool mayDefinitelyPass;
    };

    /// @brief The list of connections to recheck
    std::vector<PostProcessConnection> myConnections;


    /// @brief The type of the dictionary where an edge may be found by its id
    typedef std::map<std::string, NBEdge*> EdgeCont;

    /// @brief The instance of the dictionary (id->edge)
    EdgeCont myEdges;

    /// @brief The extracted nodes which are kept for reference
    EdgeCont myExtractedEdges;

    /// @brief The ids of ignored edges
    std::set<std::string> myIgnoredEdges;

    /// @brief the number of splits of edges during the building
    unsigned int myEdgesSplit;

    /// @brief Whether the network is left-handed
    bool myAmLeftHanded;


    /// @name Settings for accepting/dismissing edges
    /// @{

    /// @brief The minimum speed an edge may have in order to be kept (default: -1)
    SUMOReal myEdgesMinSpeed;

    /// @brief Whether edges shall be joined first, then removed
    bool myRemoveEdgesAfterJoining;

    /// @brief Set of ids of edges which shall explicitly be kept
    std::set<std::string> myEdges2Keep;

    /// @brief Set of ids of edges which shall explicitly be removed
    std::set<std::string> myEdges2Remove;

    /// @brief Set of vehicle types which must be allowed on edges in order to keep them
    SVCPermissions myVehicleClasses2Keep;

    /// @brief Set of vehicle types which need not be supported (edges which allow ONLY these are removed)
    SVCPermissions myVehicleClasses2Remove;

    /// @brief Set of edges types which shall be kept
    std::set<std::string> myTypes2Keep;

    /// @brief Set of edges types which shall be removed
    std::set<std::string> myTypes2Remove;

    /// @brief Boundary within which an edge must be located in order to be kept
    PositionVector myPrunningBoundary;
    /// @}


    /// @brief The network builder; used to obtain type information
    NBTypeCont& myTypeCont;


private:
    /// @brief invalidated copy constructor
    NBEdgeCont(const NBEdgeCont& s);

    /// @brief invalidated assignment operator
    NBEdgeCont& operator=(const NBEdgeCont& s);


};


#endif

/****************************************************************************/

