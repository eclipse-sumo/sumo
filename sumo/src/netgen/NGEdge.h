/****************************************************************************/
/// @file    NGEdge.h
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// A netgen-representation of an edge
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
#ifndef NGNetElements_h
#define NGNetElements_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NGNode;
class NBNode;
class NBEdge;
class NBNetBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NGEdge
 * @brief A netgen-representation of an edge
 *
 * Please note that the edge makes itself known to the from- and the to-nodes
 *  on initialisation and removes this information from the nodes when being
 *  deleted. This implicates that nodes have to be deleted after the edges.
 */
class NGEdge
{
public:
    /** @brief Constructor
     *
     * Adds itself to the start and the end node's lists of connections.
     *
     * @param[in] id The id of the link
     * @param[in] StarNGNode The begin node
     * @param[in] EndNode The end node
     */
    NGEdge(const std::string &id, NGNode *startNode, NGNode *endNode) throw();


    /** @brief Destructor
     *
     * Removes itself from the start and the end node's lists of connections.
     */
    ~NGEdge() throw();


    /** @brief Returns this link's id
     *
     * @return The id of the link
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns this link's start node
     *
     * @return The start node of the link
     */
    NGNode *getStartNode() throw() {
        return myStartNode;
    };


    /** @brief Returns this link's end node
     *
     * @return The end node of the link
     */
    NGNode *getEndNode() throw() {
        return myEndNode;
    };


    /** @brief Builds and returns this link's netbuild-representation
     *
     * Returns an edge built using the known values. Other values, such as the
     *  number of lanes, are gathered from defaults.
     * The starting and the ending node must have been built in prior.
     *
     * @param[in] nb The netbuilder to retrieve the referenced nodes from
     * @return The built edge
     */
    NBEdge *buildNBEdge(NBNetBuilder &nb) const throw(ProcessError);


private:
    /// @brief The id of the edge
    std::string myID;

    /// @brief The node the edge starts at
    NGNode *myStartNode;

    /// @brief The node the edge ends at
    NGNode *myEndNode;

};


/**
 * @typedef NGEdgeList
 * @brief A list of edges (edge pointers)
 */
typedef std::list<NGEdge*> NGEdgeList;

#endif

/****************************************************************************/

