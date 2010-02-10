/****************************************************************************/
/// @file    RODFEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Interface for building instances of dfrouter-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODFEdgeBuilder_h
#define RODFEdgeBuilder_h


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
    RODFEdgeBuilder() throw();


    /// @brief Destructor
    ~RODFEdgeBuilder() throw();


    /// @name Methods to be implemented, inherited from ROAbstractEdgeBuilder
    /// @{

    /** @brief Builds an edge with the given name
     *
     * This implementation builds a RODFEdge.
     *
     * @param[in] name The name of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @return A proper instance of the named edge
     * @see RODFEdge
     */
    ROEdge *buildEdge(const std::string &name, RONode *from, RONode *to) throw();
    /// @}


};


#endif

/****************************************************************************/

