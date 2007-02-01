/****************************************************************************/
/// @file    SUMOAbstractRouter.h
/// @author  Daniel Krajzewicz
/// @date    25.Jan 2006
/// @version $Id: $
///
// The dijkstra-router
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
#ifndef SUMOAbstractRouter_h
#define SUMOAbstractRouter_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOAbstractRouter
 * The interface for routing the vehicles over the network.
 */
template<class E, class V>
class SUMOAbstractRouter
{
public:
    /**
     * @class ROAbstractEdgeEffortRetriever
     * This interface has to be implemented in order to get the real efforts of edges
     */
    class ROAbstractEdgeEffortRetriever
    {
    public:
        /// Constructor
        ROAbstractEdgeEffortRetriever()
        { }

        /// Destructor
        virtual ~ROAbstractEdgeEffortRetriever()
        { }

        /// This function should return the effort to use
        virtual SUMOReal getEffort(const V *const, SUMOTime time, const E * const edge,
                                   SUMOReal dist) = 0;

        /// Returns the name of this retriever
        virtual const std::string &getID() const = 0;

    };


public:
    /// Constructor
    SUMOAbstractRouter()
    { }

    /// Destructor
    virtual ~SUMOAbstractRouter()
    { }

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const V * const vehicle,
                         SUMOTime time, std::vector<const E*> &into) = 0;

};


#endif

/****************************************************************************/

