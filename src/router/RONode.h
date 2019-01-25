/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RONode.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Base class for nodes used by the router
/****************************************************************************/
#ifndef RONode_h
#define RONode_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <utils/common/Named.h>
#include <utils/geom/Position.h>

// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;

typedef std::vector<const ROEdge*> ConstROEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONode
 * @brief Base class for nodes used by the router
 */
class RONode : public Named {
public:
    /** @brief Constructor
     * @param[in] id The id of the node
     */
    RONode(const std::string& id);


    /// @brief Destructor
    ~RONode();


    /** @brief Sets the position of the node
     * @param[in] p The node's position
     */
    void setPosition(const Position& p);


    /** @brief Returns the position of the node
     * @return This node's position
     */
    const Position& getPosition() const {
        return myPosition;
    }


    inline const ConstROEdgeVector& getIncoming() const {
        return myIncoming;
    }

    inline const ConstROEdgeVector& getOutgoing() const {
        return myOutgoing;
    }

    void addIncoming(ROEdge* edge) {
        myIncoming.push_back(edge);
    }

    void addOutgoing(ROEdge* edge) {
        myOutgoing.push_back(edge);
    }

private:
    /// @brief This node's position
    Position myPosition;

    /// @brief incoming edges
    ConstROEdgeVector myIncoming;
    /// @brief outgoing edges
    ConstROEdgeVector myOutgoing;


private:
    /// @brief Invalidated copy constructor
    RONode(const RONode& src);

    /// @brief Invalidated assignment operator
    RONode& operator=(const RONode& src);

};


#endif

/****************************************************************************/

