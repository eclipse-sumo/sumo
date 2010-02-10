/****************************************************************************/
/// @file    SUMOAbstractRouter.h
/// @author  Daniel Krajzewicz
/// @date    25.Jan 2006
/// @version $Id$
///
// The dijkstra-router
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
#ifndef SUMOAbstractRouter_h
#define SUMOAbstractRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOAbstractRouter
 * The interface for routing the vehicles over the network.
 */
template<class E, class V>
class SUMOAbstractRouter {
public:
    /**
     * @class ROAbstractEdgeEffortRetriever
     * This interface has to be implemented in order to get the real efforts of edges
     */
    class ROAbstractEdgeEffortRetriever {
    public:
        /// Constructor
        ROAbstractEdgeEffortRetriever() { }

        /// Destructor
        virtual ~ROAbstractEdgeEffortRetriever() { }

        /// This function should return the effort to use
        virtual SUMOReal getEffort(const V *const, SUMOTime time, const E * const edge,
                                   SUMOReal dist) = 0;

        /// Returns the name of this retriever
        virtual const std::string &getID() const = 0;

    };


public:
    /// Constructor
    SUMOAbstractRouter() { }

    /// Destructor
    virtual ~SUMOAbstractRouter() { }

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const V * const vehicle,
                         SUMOTime time, std::vector<const E*> &into) = 0;

    virtual SUMOReal recomputeCosts(const std::vector<const E*> &edges,
                                    const V * const v, SUMOTime time) throw() = 0;

};


template<class E, class V>
struct prohibited_withRestrictions {
public:
    inline bool operator()(const E *edge, const V *vehicle) {
        if (std::find(myProhibited.begin(), myProhibited.end(), edge)!=myProhibited.end()) {
            return true;
        }
        return edge->prohibits(vehicle);
    }

    void prohibit(const std::vector<E*> &toProhibit) {
        myProhibited = toProhibit;
    }

protected:
    std::vector<E*> myProhibited;

};

template<class E, class V>
struct prohibited_noRestrictions {
public:
    inline bool operator()(const E *, const V *) {
        return false;
    }
};




#endif

/****************************************************************************/

