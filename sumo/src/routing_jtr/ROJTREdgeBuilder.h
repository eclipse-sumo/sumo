/****************************************************************************/
/// @file    ROJTREdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of jtrrouter-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROJTREdgeBuilder_h
#define ROJTREdgeBuilder_h


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
 * @class ROJTREdgeBuilder
 * @brief Interface for building instances of jtrrouter-edges
 *
 * This ROAbstractEdgeBuilder implementation builds edges for the jtrrouter
 *  (instances of ROJTREdge).
 *
 * @see ROJTREdge
 */
class ROJTREdgeBuilder : public ROAbstractEdgeBuilder
{
public:
    /// @brief Constructor
    ROJTREdgeBuilder() throw();


    /// @brief Destructor
    ~ROJTREdgeBuilder() throw();


    /// @name Methods to be implemented, inherited from ROAbstractEdgeBuilder
    /// @{

    /** @brief Builds an edge with the given name
     *
     * This implementation builds a ROJTREdge.
     *
     * @param[in] name The name of the edge
     * @return A proper instance of the named edge
     * @see ROJTREdge
     */
    ROEdge *buildEdge(const std::string &name) throw();
    /// @}


};


#endif

/****************************************************************************/

