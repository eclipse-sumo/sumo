/****************************************************************************/
/// @file    NBEdgeCont.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Storage for edges, including some functionality operating on multiple edges
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
#include <set>
#include "NBCont.h"
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
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
    /// @brief Constructor
    NBEdgeCont() throw();


    /// @brief Destructor
    ~NBEdgeCont() throw();


    /** @brief Initialises the storage by applying given options
     *
     * Options, mainly steering the acceptance of edges, are parsed
     *  and the according internal variables are set.
     *
     * @param[in] oc The options container to read options from
     * @todo Recheck exceptions
     */
    void applyOptions(OptionsCont &oc);


    /** @brief Deletes all edges */
    void clear() throw();



    
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
     * @return Whether the edge was valid (no edge with the same id is already known)
     */
    bool insert(NBEdge *edge) throw();


    /** @brief Returns the edge that has the given id
     *
     * If no edge that has the given id is known, 0 is returned.
     *
     * @param[in] id The id of the edge to retrieve
     * @return The edge with the given id, 0 if no such edge exists
     */
    NBEdge *retrieve(const std::string &id) const throw();


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
    NBEdge *retrievePossiblySplitted(const std::string &id, 
        const std::string &hint, bool incoming) const throw();


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
    NBEdge *retrievePossiblySplitted(const std::string &id, SUMOReal pos) const throw();


    /** @brief Removes the given edge from the container (deleting it) 
     *
     * @param[in] dc The district container, in order to remove the edge from sources/sinks
     * @param[in] edge The edge to remove
     * @todo Recheck whether the district cont is needed - if districts are processed using an external tool
     */
    void erase(NBDistrictCont &dc, NBEdge *edge) throw();
    /// @}



    /// @name explicite edge manipulation methods
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
    bool splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node) throw(ProcessError);


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
    bool splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node,
                 const std::string &firstEdgeName, const std::string &secondEdgeName,
                 unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge) throw(ProcessError);


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
    bool splitAt(NBDistrictCont &dc, NBEdge *edge, SUMOReal edgepos, NBNode *node,
                 const std::string &firstEdgeName, const std::string &secondEdgeName,
                 unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge) throw(ProcessError);
    /// @}



    /** returns the number of edges */
    int size();

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

    /** deletes all edges */

    /// joins the given edges as they connect the same nodes
    void joinSameNodeConnectingEdges(NBDistrictCont &dc,
                                     NBTrafficLightLogicCont &tlc, EdgeVector edges);

    /// moves the geometry of the edges by the network offset
    void normaliseEdgePositions();
    void reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    void computeEdgeShapes();

    std::vector<std::string> getAllNames();


    /** @brief Removes unwished edges (not in keep-edges)
     *
     * @param[in] dc The district container needed to remove edges 
     * @todo Recheck usage; check whether keep-edges.postload is really useful
     */
    void removeUnwishedEdges(NBDistrictCont &dc) throw();

    void recomputeLaneShapes();

    void splitGeometry(NBNodeCont &nc);

    void recheckLaneSpread();

    void recheckEdgeGeomsForDoublePositions();

    size_t getNoEdgeSplits();

    /// @name output methods
    /// @{

    /** @brief Writes the edge definitions with lanes into the given stream 
     *
     * Calls "NBEdge::writeXMLStep1" for all edges within the container.
     * 
     * @param[in] into The stream to write the definieions into
     * @exception IOError (not yet implemented)
     */
    void writeXMLStep1(OutputDevice &into) throw(IOError);


    /** @brief Writes the successor definitions of edges into the given stream 
     *
     * Calls "NBEdge::writeXMLStep2" for all edges within the container.
     * 
     * @param[in] into The stream to write the definieions into
     * @param[in] includeInternal Whether internal successors shal be written, too
     * @exception IOError (not yet implemented)
     */
    void writeXMLStep2(OutputDevice &into, bool includeInternal) throw(IOError);


    /** @brief Writes the stored edges as an XML-edge-file
     * @param[in] file The path to write the edge definitions into
     * @exception IOError If the file could not be opened
     */
    void savePlain(const std::string &file) throw(IOError);
    /// @}


private:
    /** @brief Returns the edges which have been built by splitting the edge of the given id
     *
     * @param[in] id The id of the original edge
     * @return List of all edges which have been built by splitting the original edge
     * @todo Recheck usage
     */
    EdgeVector getGeneratedFrom(const std::string &id) const throw();


private:
    /// @brief The type of the dictionary where an edge may be found by her id
    typedef std::map<std::string, NBEdge*> EdgeCont;

    /// @brief The instance of the dictionary (id->edge)
    EdgeCont myEdges;

    /// @brief the number of splits of edges during the building
    unsigned int myEdgesSplit;


    /// @name Settings for accepting/dismissing edges
    /// @{

    /// @brief The minimum speed an edge may have in order to be kept (default: -1)
    SUMOReal myEdgesMinSpeed;

    /// @brief Whether edges shall be joined first, then removed
    bool myRemoveEdgesAfterJoining;

    /// @brief Vector of ids of edges which shall explicitely be kept
    std::vector<std::string> myEdges2Keep;

    /// @brief Vector of ids of edges which shall explicitely be removed
    std::vector<std::string> myEdges2Remove;

    /// @brief Vector of vehicle types which must be allowed on edges in order to keep them
    std::set<SUMOVehicleClass> myVehicleClasses2Keep;

    /// @brief Vector of vehicle types which must not be disallowed on edges in order to keep them
    std::set<SUMOVehicleClass> myVehicleClasses2Remove;

    /// @}

private:
    /// @brief invalidated copy constructor
    NBEdgeCont(const NBEdgeCont &s);

    /// @brief invalidated assignment operator
    NBEdgeCont &operator=(const NBEdgeCont &s);


};


#endif

/****************************************************************************/

