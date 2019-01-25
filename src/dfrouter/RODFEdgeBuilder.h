/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RODFEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Interface for building instances of dfrouter-edges
/****************************************************************************/
#ifndef RODFEdgeBuilder_h
#define RODFEdgeBuilder_h


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
 * @class RODFEdgeBuilder
 * @brief Interface for building instances of dfrouter-edges
 *
 * This ROAbstractEdgeBuilder implementation builds edges for the duarouter
 *  (instances of RODFEdge).
 *
 * @see RODFEdge
 */
class RODFEdgeBuilder : public ROAbstractEdgeBuilder {
public:
    /// @brief Constructor
    RODFEdgeBuilder();


    /// @brief Destructor
    ~RODFEdgeBuilder();


    /// @name Methods to be implemented, inherited from ROAbstractEdgeBuilder
    /// @{

    /** @brief Builds an edge with the given name
     *
     * This implementation builds a RODFEdge.
     *
     * @param[in] name The name of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] priority The edge priority (road class)
     * @return A proper instance of the named edge
     * @see RODFEdge
     */
    ROEdge* buildEdge(const std::string& name, RONode* from, RONode* to, const int priority);
    /// @}


};


#endif

/****************************************************************************/

