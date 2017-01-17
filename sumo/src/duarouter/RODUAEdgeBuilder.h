/****************************************************************************/
/// @file    RODUAEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of duarouter-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODUAEdgeBuilder_h
#define RODUAEdgeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
 * @class RODUAEdgeBuilder
 * @brief Interface for building instances of duarouter-edges
 *
 * This ROAbstractEdgeBuilder implementation builds edges for the duarouter
 *  (instances of ROEdge).
 *
 * @see ROEdge
 */
class RODUAEdgeBuilder : public ROAbstractEdgeBuilder {
public:
    /** @brief Constructor
     */
    RODUAEdgeBuilder();


    /// @brief Destructor
    ~RODUAEdgeBuilder();


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

