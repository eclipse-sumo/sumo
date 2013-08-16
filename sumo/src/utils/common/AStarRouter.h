/****************************************************************************/
/// @file    AStarRouter.h
/// @author  Jakob Erdmann
/// @date    January 2012
/// @version $Id$
///
// A* Algorithm using euclidean distance heuristic.
// Based on DijkstraRouterTT. For routing by effort a novel heuristic would be needed.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AStarRouterTT_h
#define AStarRouterTT_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <string>
#include <functional>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <iterator>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include "SUMOAbstractRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AStarRouterTT
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
class AStarRouterTTBase : public SUMOAbstractRouter<E, V>, public PF {
    using SUMOAbstractRouter<E, V>::startQuery;
    using SUMOAbstractRouter<E, V>::endQuery;

public:
    /// Constructor
    AStarRouterTTBase(size_t noE, bool unbuildIsWarning):
        SUMOAbstractRouter<E, V>("AStarRouter"),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()) {
        for (size_t i = 0; i < noE; i++) {
            myEdgeInfos.push_back(EdgeInfo(i));
        }
    }

    /// Destructor
    virtual ~AStarRouterTTBase() {}

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(size_t id) :
            edge(E::dictionary(id)),
            traveltime(std::numeric_limits<SUMOReal>::max()),
            heuristicTime(std::numeric_limits<SUMOReal>::max()),
            prev(0),
            visited(false)
        {}

        /// The current edge
        const E* edge;

        /// Effort to reach the edge
        SUMOReal traveltime;

        /// Estimated time to reach the edge (traveltime + lower bound on remaining time)
        SUMOReal heuristicTime;

        /// The previous edge
        EdgeInfo* prev;

        /// The previous edge
        bool visited;

        inline void reset() {
            // heuristicTime is set before adding to the frontier, thus no reset is needed
            traveltime = std::numeric_limits<SUMOReal>::max();
            visited = false;
        }

    };

    /**
     * @class EdgeInfoComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoComparator {
    public:
        /// Comparing method
        bool operator()(const EdgeInfo* nod1, const EdgeInfo* nod2) const {
            if (nod1->heuristicTime == nod2->heuristicTime) {
                return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
            }
            return nod1->heuristicTime > nod2->heuristicTime;
        }
    };

    virtual SUMOReal getEffort(const E* const e, const V* const v, SUMOReal t) const = 0;


    void init() {
        // all EdgeInfos touched in the previous query are either in myFrontierList or myFound: clean those up
        for (typename std::vector<EdgeInfo*>::iterator i = myFrontierList.begin(); i != myFrontierList.end(); i++) {
            (*i)->reset();
        }
        myFrontierList.clear();
        for (typename std::vector<EdgeInfo*>::iterator i = myFound.begin(); i != myFound.end(); i++) {
            (*i)->reset();
        }
        myFound.clear();
    }


    /** @brief Builds the route between the given edges using the minimum travel time */
    virtual void compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, std::vector<const E*>& into) {
        assert(from != 0 && to != 0);
        startQuery();
        const SUMOReal time = STEPS2TIME(msTime);
        init();
        // add begin node
        EdgeInfo* const fromInfo = &(myEdgeInfos[from->getNumericalID()]);
        fromInfo->traveltime = 0;
        fromInfo->prev = 0;
        myFrontierList.push_back(fromInfo);
        // loop
        int num_visited = 0;
        while (!myFrontierList.empty()) {
            num_visited += 1;
            // use the node with the minimal length
            EdgeInfo* const minimumInfo = myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            pop_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
            myFrontierList.pop_back();
            myFound.push_back(minimumInfo);
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumInfo, into);
                endQuery(num_visited);
                // DEBUG
                //std::cout << "visited " + toString(num_visited) + " edges (final path length: " + toString(into.size()) + ")\n";
                return;
            }
            minimumInfo->visited = true;
            const SUMOReal traveltime = minimumInfo->traveltime + getEffort(minEdge, vehicle, time + minimumInfo->traveltime);
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            const unsigned int length_size = minEdge->getNoFollowing();
            for (i = 0; i < length_size; i++) {
                const E* const follower = minEdge->getFollower(i);
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const SUMOReal oldEffort = followerInfo->traveltime;
                if (!followerInfo->visited && traveltime < oldEffort) {
                    // admissible A* heuristic: straight line distance at maximum speed
                    const SUMOReal heuristic_remaining = minEdge->getDistanceTo(to) / vehicle->getMaxSpeed();
                    followerInfo->traveltime = traveltime;
                    followerInfo->heuristicTime = traveltime + heuristic_remaining;
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
        endQuery(num_visited);
        myErrorMsgHandler->inform("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
    }


    SUMOReal recomputeCosts(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime) const {
        const SUMOReal time = STEPS2TIME(msTime);
        SUMOReal costs = 0;
        for (typename std::vector<const E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                return -1;
            }
            costs += getEffort(*i, v, time + costs);
        }
        return costs;
    }

public:
    /// Builds the path from marked edges
    void buildPathFrom(EdgeInfo* rbegin, std::vector<const E*>& edges) {
        std::deque<const E*> tmp;
        while (rbegin != 0) {
            tmp.push_front((E*) rbegin->edge);  // !!!
            rbegin = rbegin->prev;
        }
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(edges));
    }

protected:
    /// The container of edge information
    std::vector<EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<EdgeInfo*> myFrontierList;
    /// @brief list of visited Edges (for resetting)
    std::vector<EdgeInfo*> myFound;

    EdgeInfoComparator myComparator;

    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

};


template<class E, class V, class PF>
class AStarRouterTT_ByProxi : public AStarRouterTTBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(* Operation)(const E* const, const V* const, SUMOReal);

    AStarRouterTT_ByProxi(size_t noE, bool unbuildIsWarningOnly, Operation operation):
        AStarRouterTTBase<E, V, PF>(noE, unbuildIsWarningOnly),
        myOperation(operation) {}

    inline SUMOReal getEffort(const E* const e, const V* const v, SUMOReal t) const {
        return (*myOperation)(e, v, t);
    }

private:
    /// @brief The object's operation to perform.
    Operation myOperation;
};


template<class E, class V, class PF>
class AStarRouterTT_Direct : public AStarRouterTTBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(E::* Operation)(const V* const, SUMOReal) const;

    AStarRouterTT_Direct(size_t noE, bool unbuildIsWarningOnly, Operation operation)
        : AStarRouterTTBase<E, V, PF>(noE, unbuildIsWarningOnly), myOperation(operation) {}

    inline SUMOReal getEffort(const E* const e, const V* const v, SUMOReal t) const {
        return (e->*myOperation)(v, t);
    }

private:
    Operation myOperation;
};


#endif

/****************************************************************************/

