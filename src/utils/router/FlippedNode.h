/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    FlippedNode.h
/// @author  Ruediger Ebendt
/// @date    01.12.2023
///
// Wrapper around an RONode used for backward search. It swaps incoming 
// with outgoing edges, and replaces the original edges by reversed 
// ones (i.e., by instances of FlippedEdge)
/****************************************************************************/
#pragma once
#include <config.h>
#include <vector>
#include "FlippedEdge.h"

// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the node type representing nodes used for backward search
template<class E, class N, class V>
class FlippedNode {
public:
    typedef std::vector<const FlippedEdge<E, N, V>*> ConstFlippedEdgeVector;

    /** Constructor
     * @param[in] originalNode The original node
     */
    FlippedNode(const N* originalNode) : 
        myOriginalNode(originalNode) {}

    /// @brief Destructor
    ~FlippedNode() {}

    /** @brief Returns the position of the node
     * @return This node's position
     */
    const Position& getPosition() const {
        return myOriginalNode->getPosition();
    }
    /** @brief Returns the id(entifier) of the node
     * @return This node's id(entifier)
     */
    const std::string& getID() const {
        return myOriginalNode->getID();
    }
    
    /** @brief Returns the incoming edges of the node
     * @return The incoming edges of the node
     */
    const ConstFlippedEdgeVector& getIncoming() const {
        if (myIncoming.empty()) {
            const std::vector<const E*>& incoming = myOriginalNode->getOutgoing();
            for (const E* edge : incoming) {
                myIncoming.push_back(edge->getFlippedRoutingEdge());
            }
        }
        return myIncoming;
    }

    /** @brief Returns the outgoing edges of the node
      * @return The outgoing edges of the node
      */
    const ConstFlippedEdgeVector& getOutgoing() const {
        if (myOutgoing.empty()) {
            const std::vector<const E*>& outgoing = myOriginalNode->getIncoming();
            for (const E* edge : outgoing) {
                myOutgoing.push_back(edge->getFlippedRoutingEdge());
            }
        }
        return myOutgoing;
    }

    /// @brief Returns the original node
    const N* getOriginalNode() const {
        return myOriginalNode;
    }
private:
    /// @brief The original node
    const N* const myOriginalNode;
    /// @brief The incoming edges
    mutable ConstFlippedEdgeVector myIncoming;
    /// @brief The outgoing edges
    mutable ConstFlippedEdgeVector myOutgoing;
};
