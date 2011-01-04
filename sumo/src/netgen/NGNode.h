/****************************************************************************/
/// @file    NGNode.h
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// A netgen-representation of a node
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NGNode_h
#define NGNode_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/UtilExceptions.h>
#include "NGEdge.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class NBEdge;
class NBNetBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NGNode
 * @brief A netgen-representation of a node
 */
class NGNode {
public:
    /// @brief Constructor
    NGNode() throw();


    /** @brief Constructor
     *
     * @param[in] id The id of the node
     */
    NGNode(const std::string &id) throw();


    /** @brief Constructor
     *
     * @param[in] id The id of the node
     * @param[in] xPos The x-position of the node
     * @param[in] yPos The y-position of the node
     */
    NGNode(const std::string &id, int xPos, int yPos) throw();


    /** @brief Constructor
     *
     * @param[in] id The id of the node
     * @param[in] xPos The x-position of the node
     * @param[in] yPos The y-position of the node
     * @param[in] amCenter Information whether this is the center-node of a spider-net
     */
    NGNode(const std::string &id, int xID, int yID, bool amCenter) throw();


    /// @brief Destructor
    ~NGNode() throw();


    /** @brief Returns this node's id
     *
     * @return The id of the node
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns this node's position
     *
     * @return The position of the node
     */
    const Position2D &getPosition() const throw() {
        return myPosition;
    }


    /** @brief Returns this node's maximum neighbour number
     *
     * @return The maximum neighbour number of the node
     */
    SUMOReal getMaxNeighbours() throw() {
        return myMaxNeighbours;
    }


    /** @brief Sets this node's maximum neighbour number
     *
     * @param[in] value The new maximum neighbour number of the node
     */
    void setMaxNeighbours(SUMOReal value) throw() {
        myMaxNeighbours = value;
    }


    /** @brief Sets a new value for x-position
     *
     * @param[in] value The new x-position of this node
     */
    void setX(SUMOReal x) throw() {
        myPosition.set(x, myPosition.y());
    }


    /** @brief Sets a new value for y-position
     *
     * @param[in] value The new y-position of this node
     */
    void setY(SUMOReal y) throw() {
        myPosition.set(myPosition.x(), y);
    }


    /** @brief Builds and returns this node's netbuild-representation
     *
     * The position of the node is transformed to cartesian using GeoConvHelper::x2cartesian,
     *  first. If this node is the center node of a spider net, a node of the type
     *  NBNode::NODETYPE_NOJUNCTION is returned.
     * Otherwise, a plain node is built and it is checked whether the options
     *  indicate building one of the tls node-types. In this case, a logic is built and
     *  stored. A ProcessError is thrown if this fails (should never happen, in fact).
     *
     * @param[in] nb The netbuilder to retrieve the tls-container from
     * @return The built node
     * @exception ProcessError If the built tls logic could not be added (should never happen)
     * @todo There is no interaction with explicite node setting options? Where is this done?
     * @todo Check whether throwing an exception is really necessary, here
     */
    NBNode *buildNBNode(NBNetBuilder &nb) const throw(ProcessError);


    /** @brief Adds the given link to the internal list
     *
     * @param[in] link The link to add
     */
    void addLink(NGEdge *link) throw();


    /** @brief Removes the given link
     *
     * The given pointer is compared to those in the list. A matching
     *  pointer is removed, not other same connections.
     *
     * @param[in] link The link to remove
     */
    void removeLink(NGEdge *link) throw();


    /** @brief Returns whether the other node is connected
     *
     * @param[in] node The link to check whether it is connected
     * @return Whether the given node is connected
     */
    bool connected(NGNode *node) const throw();


    /** @brief Returns whether the node has the given position
     *
     * @param[in] node The link to check whether it is connected
     * @return Whether the given node is connected
     */
    bool samePos(int xPos, int yPos) const throw() {
        return xID==xPos && yID==yPos;
    }

    // NGRandomNetBuilder needs access to links
    friend class NGRandomNetBuilder;

private:
    /// @brief Integer x-position (x-id)
    int xID;

    /// @brief Integer y-position (y-id)
    int yID;

    /// @brief List of connected links
    NGEdgeList LinkList;

    /// @brief The id of the node
    std::string myID;

    /// @brief The position of the node
    Position2D myPosition;

    /// @brief The maximum number of neighbours
    SUMOReal myMaxNeighbours;

    /// @brief Information whether this is the center of a cpider-net
    bool myAmCenter;

};

/**
 * @typedef NGNodeList
 * @brief A list of nodes (node pointers)
 */
typedef std::list<NGNode*> NGNodeList;



#endif

/****************************************************************************/

