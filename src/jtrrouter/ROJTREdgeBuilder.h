/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROJTREdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of jtrrouter-edges
/****************************************************************************/
#ifndef ROJTREdgeBuilder_h
#define ROJTREdgeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <router/ROAbstractEdgeBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTREdgeBuilder
 * @brief Interface for building instances of jtrrouter-edges
 *
 * This ROAbstractEdgeBuilder implementation builds edges for the jtrrouter
 *  (instances of ROJTREdge).
 *
 * @see ROJTREdge
 */
class ROJTREdgeBuilder : public ROAbstractEdgeBuilder {
public:
    /// @brief Constructor
    ROJTREdgeBuilder();


    /// @brief Destructor
    ~ROJTREdgeBuilder();


    /// @name Methods inherited from ROAbstractEdgeBuilder
    /// @{

    /** @brief Builds an edge with the given name
     *
     * This implementation builds a ROJTREdge.
     *
     * @param[in] name The name of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] priority The edge priority (road class)
     * @return A proper instance of the named edge
     * @see ROJTREdge
     */
    ROEdge* buildEdge(const std::string& name, RONode* from, RONode* to, const int priority);
    /// @}


};


#endif

/****************************************************************************/

