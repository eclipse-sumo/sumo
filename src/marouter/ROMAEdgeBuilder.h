/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROMAEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of duarouter-edges
/****************************************************************************/
#ifndef ROMAEdgeBuilder_h
#define ROMAEdgeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <router/ROAbstractEdgeBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RONode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROMAEdgeBuilder
 * @brief Interface for building instances of duarouter-edges
 *
 * This ROAbstractEdgeBuilder implementation builds edges for the duarouter
 *  (instances of ROEdge).
 *
 * @see ROEdge
 */
class ROMAEdgeBuilder : public ROAbstractEdgeBuilder {
public:
    /** @brief Constructor
     */
    ROMAEdgeBuilder();


    /// @brief Destructor
    ~ROMAEdgeBuilder();


    /// @name Methods to be implemented, inherited from ROAbstractEdgeBuilder
    /// @{

    /** @brief Builds an edge with the given name
     *
     * This implementation builds a ROEdge.
     *
     * @param[in] name The name of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] priority The edge priority (road class)
     * @return A proper instance of the named edge
     * @see ROEdge
     */
    ROEdge* buildEdge(const std::string& name, RONode* from, RONode* to, const int priority);
    /// @}


};


#endif

/****************************************************************************/

