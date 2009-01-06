/****************************************************************************/
/// @file    RODUAEdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Interface for building instances of duarouter-edges
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
class RODUAEdgeBuilder : public ROAbstractEdgeBuilder
{
public:
    /** @brief Constructor
     *
     * @param[in] useBoundariesOnOverride Whether edges shall use a boundary value if the requested is beyond known time scale
     * @todo useBoundariesOnOverride should not be a member of the edges
     */
    RODUAEdgeBuilder(bool useBoundariesOnOverride) throw();


    /// @brief Destructor
    ~RODUAEdgeBuilder() throw();


    /// @name Methods to be implemented, inherited from ROAbstractEdgeBuilder
    /// @{

    /** @brief Builds an edge with the given name
     *
     * This implementation builds a ROEdge.
     *
     * @param[in] name The name of the edge
     * @return A proper instance of the named edge
     * @see ROEdge
     */
    ROEdge *buildEdge(const std::string &name) throw();
    /// @}


protected:
    /// @brief Information whether to use the first/last entry of the time in case it is too short
    bool myUseBoundariesOnOverride;

};


#endif

/****************************************************************************/

