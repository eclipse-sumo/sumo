/****************************************************************************/
/// @file    DijkstraRouterTT.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Dijkstra shortest path algorithm using travel time
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef DijkstraRouterTT_h
#define DijkstraRouterTT_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <iterator>
#include <utils/common/InstancePool.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include "SUMOAbstractRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DijkstraRouterTT
 * @brief Computes the shortest path through a network using the Dijkstra algorithm.
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withRestrictions/prohibited_noRestrictions)
 * @param EC The class to retrieve the effort for an edge from
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class PF>
class DijkstraRouterTTBase : public SUMOAbstractRouter<E, V>, public PF {
public:
    /// Constructor
    DijkstraRouterTTBase(size_t noE, bool unbuildIsWarningOnly)
            : myUnbuildIsWarningOnly(unbuildIsWarningOnly) {
        for (size_t i = 0; i < noE; i++) {
            myEdgeInfos.push_back(EdgeInfo(i));
        }
    }

    /// Destructor
    virtual ~DijkstraRouterTTBase() { }

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(size_t id)
                : edge(E::dictionary(id)), traveltime(0), prev(0), visited(false) {}

        /// The current edge
        const E *edge;

        /// Effort to reach the edge
        SUMOReal traveltime;

        /// The previous edge
        EdgeInfo *prev;

        /// The previous edge
        bool visited;

    };

    /**
     * @class EdgeInfoByEffortComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByTTComparator {
    public:
        /// Comparing method
        bool operator()(const EdgeInfo *nod1, const EdgeInfo *nod2) const {
            if (nod1->traveltime == nod2->traveltime) {
                return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
            }
            return nod1->traveltime>nod2->traveltime;
        }
    };

    virtual SUMOReal getEffort(const E * const e, const V * const v, SUMOReal t) = 0;


    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const V * const vehicle,
                         SUMOTime msTime, std::vector<const E*> &into) {

        SUMOReal time = (SUMOReal) msTime / 1000.;
        for (typename std::vector<EdgeInfo>::iterator i=myEdgeInfos.begin(); i!=myEdgeInfos.end(); i++) {
            (*i).traveltime = std::numeric_limits<SUMOReal>::max();
            (*i).visited = false;
        }
        assert(from!=0&&to!=0);
        myFrontierList.clear();
        // add begin node
        EdgeInfo* const fromInfo = &(myEdgeInfos[from->getNumericalID()]);
        fromInfo->traveltime = 0;
        fromInfo->prev = 0;
        myFrontierList.push_back(fromInfo);
        // loop
        while (!myFrontierList.empty()) {
            // use the node with the minimal length
            EdgeInfo * const minimumInfo = myFrontierList.front();
            const E * const minEdge = minimumInfo->edge;
            pop_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
            myFrontierList.pop_back();
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumInfo, into);
                return;
            }
            minimumInfo->visited = true;
            const SUMOReal traveltime = minimumInfo->traveltime + getEffort(minEdge, vehicle, time + (SUMOTime)minimumInfo->traveltime);
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            const unsigned int length_size = minEdge->getNoFollowing();
            for (i=0; i<length_size; i++) {
                const E* const follower = minEdge->getFollower(i);
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const SUMOReal oldEffort = followerInfo->traveltime;
                if (!followerInfo->visited && traveltime < oldEffort) {
                    followerInfo->traveltime = traveltime;
                    followerInfo->prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<SUMOReal>::max()) {
                        myFrontierList.push_back(followerInfo);
                        push_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
                    } else {
                        push_heap(myFrontierList.begin(),
                                  find(myFrontierList.begin(), myFrontierList.end(), followerInfo) + 1,
                                  myComparator);
                    }
                }
            }
        }
        if (!myUnbuildIsWarningOnly) {
            MsgHandler::getErrorInstance()->inform("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        } else {
            WRITE_WARNING("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        }
    }


    SUMOReal recomputeCosts(const std::vector<const E*> &edges, const V * const v, SUMOTime msTime) throw() {
        SUMOReal time = (SUMOReal) msTime / 1000.;
        SUMOReal costs = 0;
        for (typename std::vector<const E*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                return -1;
            }
            costs += getEffort(*i, v, (SUMOTime)(time + costs));
        }
        return costs;
    }

public:
    /// Builds the path from marked edges
    void buildPathFrom(EdgeInfo *rbegin, std::vector<const E *> &edges) {
        std::deque<const E*> tmp;
        while (rbegin!=0) {
            tmp.push_front((E *) rbegin->edge); // !!!
            rbegin = rbegin->prev;
        }
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(edges));
    }

protected:
    /// The container of edge information
    std::vector<EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<EdgeInfo*> myFrontierList;

    EdgeInfoByTTComparator myComparator;

    bool myUnbuildIsWarningOnly;

};


template<class E, class V, class PF, class EC>
class DijkstraRouterTT_ByProxi : public DijkstraRouterTTBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(EC::* Operation)(const E * const, const V * const, SUMOReal) const;

    DijkstraRouterTT_ByProxi(size_t noE, bool unbuildIsWarningOnly, EC* receiver, Operation operation)
            : DijkstraRouterTTBase<E, V, PF>(noE, unbuildIsWarningOnly),
            myReceiver(receiver), myOperation(operation) {}

    inline SUMOReal getEffort(const E * const e, const V * const v, SUMOReal t) {
        return (myReceiver->*myOperation)(e, v, t);
    }

private:
    /// @brief The object the action is directed to.
    EC* myReceiver;

    /// @brief The object's operation to perform.
    Operation myOperation;


};


template<class E, class V, class PF>
class DijkstraRouterTT_Direct : public DijkstraRouterTTBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(E::* Operation)(const V * const, SUMOReal) const;

    DijkstraRouterTT_Direct(size_t noE, bool unbuildIsWarningOnly, Operation operation)
            : DijkstraRouterTTBase<E, V, PF>(noE, unbuildIsWarningOnly), myOperation(operation) {}

    inline SUMOReal getEffort(const E * const e, const V * const v, SUMOReal t) {
        return (e->*myOperation)(v, t);
    }

private:
    Operation myOperation;

};


#endif

/****************************************************************************/

