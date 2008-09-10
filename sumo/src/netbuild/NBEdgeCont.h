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

    /// joins the given edges as they connect the same nodes
    void joinSameNodeConnectingEdges(NBDistrictCont &dc,
                                     NBTrafficLightLogicCont &tlc, EdgeVector edges);

    /// moves the geometry of the edges by the network offset
    void normaliseEdgePositions();
    void reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot);

    void computeEdgeShapes();

    std::vector<std::string> getAllNames();

    bool savePlain(const std::string &file);

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

