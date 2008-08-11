/****************************************************************************/
/// @file    MSEdgeContinuations.h
/// @author  Daniel Krajzewicz
/// @date    2005-11-09
/// @version $Id$
///
// Stores predeccessor-successor-relations of MSEdges.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSEdgeContinuations_h
#define MSEdgeContinuations_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEdgeContinuations
 * @brief Stores predeccessor-successor-relations of MSEdges.
 */
class MSEdgeContinuations
{
public:
    /// @brief Constructor
    MSEdgeContinuations() throw();


    /// @brief Destrcutor
    ~MSEdgeContinuations() throw();


    /** @brief Adds a connection between edges
     * 
     * @param[in] to The connected edge
     * @param[in] from The edge that connects the first one
     */
    void add(const MSEdge * const to, const MSEdge * const from) throw();


    /** @brief Returns the list of edges that yield in the given
     *
     * @param[in] toEdge The list asked for
     * @return List of edges which yield in the given
     */
    const std::vector<const MSEdge * const> &getInFrontOfEdge(const MSEdge &toEdge) const throw();


    /** @brief Returns whether the given edge has any incoming edge
     *
     * @param[in] toEdge The edge to examine
     * @return Whether any edge yields in the given
     */
    bool hasFurther(const MSEdge &toEdge) const throw();

private:
    /// @brief Internal container that stores for each edge the list of edges that connect it
    std::map<const MSEdge * const, std::vector<const MSEdge * const> > myContinuations;


};


#endif

/****************************************************************************/

