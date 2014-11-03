/****************************************************************************/
/// @file    BulkStarRouter.h
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    February 2012
/// @version $Id$
///
// A* Algorithm using shortest-path-in-fast-graph-heuristic for bulk routing
// This router is made for routing multiple sources (at different times) to the same destination
// In the reverse graph (using maximum edge speed) a lower-bound shortest path tree from the target is built.
// These optimistic distances are then used as admissable heuristc
// when routing forward in the time-dependent graph from multiple sources.
// @note: this heuristic does not perform well if the actual vehicles are much slower than the maximum edge speeds
// @note: this heuristic also does not perform well if the actual vehicles have usage restrictions
// @todo: add option for setting maximum speed
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef BulkStarRouter_h
#define BulkStarRouter_h


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
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/vehicle/SUMOAbstractRouter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class BulkStarRouterTT
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
class BulkStarRouter: public SUMOAbstractRouter<E, V>, public PF {

public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(* Operation)(const E* const, const V* const, SUMOReal);
    typedef SUMOReal(E::* MinTTOperation)(const V* const) const;


    /// Constructor
    BulkStarRouter(size_t noE, bool unbuildIsWarning, Operation operation, MinTTOperation minTTOperation) :
        SUMOAbstractRouter<E, V>(operation, "BulkStarRouter"),
        myErrorMsgHandler(unbuildIsWarning ?  MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myMinTTOperation(minTTOperation),
        myPreparedDestination(0) {
        for (size_t i = 0; i < noE; i++) {
            myEdgeInfos.push_back(EdgeInfo(i));
        }
    }

    /// Destructor
    virtual ~BulkStarRouter() {}

    virtual SUMOAbstractRouter<E, V>* clone() const {
        return new BulkStarRouter<E, V, PF>(myEdgeInfos.size(), myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, myMinTTOperation);
    }

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
            minRemaining(0),
            prev(0),
            visited(false) {}

        /// The current edge
        const E* edge;

        /// Effort to reach the edge
        SUMOReal traveltime;

        /// Estimated time to reach the edge (traveltime + lower bound on remaining time)
        SUMOReal heuristicTime;

        /// minimum time to destination
        SUMOReal minRemaining;

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

    inline SUMOReal getMinEffort(const E* const e, const V* const v) const {
        return (e->*myMinTTOperation)(v);
    }


    /** @brief Builds a complete shorteset path tree in the (static) reverse
     * graph from destination (Dijkstra until all EdgeInfos are visited)
     * @param[in] destination The common destination for the following routes
     * @param[in] fastestVehicle An optimistic vehicle (fastest, least restriction) for the following routes
     * @param[in] skip Whether to shortcut the preparation (query uses euclidean heuristic)
     * */
    void prepare(const E* destination, const V* fastestVehicle, bool skip) {
        if (skip) {
            myPreparedDestination = 0;
            return;
        }
        myPreparedDestination = destination;
        for (typename std::vector<EdgeInfo>::iterator i = myEdgeInfos.begin(); i != myEdgeInfos.end(); i++) {
            (*i).reset();
            (*i).minRemaining = 0;
        }
        myFrontier.clear();
        myFound.clear();
        // add begin node
        EdgeInfo* const fromInfo = &(myEdgeInfos[destination->getNumericalID()]);
        fromInfo->traveltime = 0;
        fromInfo->prev = 0;
        myFrontier.push_back(fromInfo);
        // loop
        int num_visited = 0;
        while (!myFrontier.empty()) {
            num_visited += 1;
            // use the node with the minimal length
            EdgeInfo* const minimumInfo = myFrontier.front();
            const E* const minEdge = minimumInfo->edge;
            pop_heap(myFrontier.begin(), myFrontier.end(), myComparator);
            myFrontier.pop_back();
            myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
            const SUMOReal traveltime = minimumInfo->traveltime + getMinEffort(minEdge, fastestVehicle);
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            const unsigned int length_size = minEdge->getNumPredecessors();
            for (i = 0; i < length_size; i++) {
                const E* const follower = minEdge->getPredecessor(i);
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                const SUMOReal oldEffort = followerInfo->traveltime;
                if (!followerInfo->visited && traveltime < oldEffort) {
                    followerInfo->traveltime = traveltime;
                    followerInfo->minRemaining = traveltime;
                    followerInfo->heuristicTime = traveltime; // plain dijkstra
                    followerInfo->prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<SUMOReal>::max()) {
                        myFrontier.push_back(followerInfo);
                        push_heap(myFrontier.begin(), myFrontier.end(), myComparator);
                    } else {
                        push_heap(myFrontier.begin(),
                                  find(myFrontier.begin(), myFrontier.end(), followerInfo) + 1,
                                  myComparator);
                    }
                }
            }
        }
        // DEBUG
        //std::cout << "visited " + toString(num_visited) + " edges during pre-computation\n";

        // DEBUG
        //std::vector<const E*> debugPath;
        //for (typename std::vector<EdgeInfo>::iterator it = myEdgeInfos.begin(); it != myEdgeInfos.end(); it++) {
        //    if (it->edge->getID() == "src") {
        //        buildPathFrom(&(*it), debugPath);
        //        std::cout << "shortest path in reverse graph:\n";
        //        for (typename std::vector<const E*>::iterator it_path = debugPath.begin(); it_path != debugPath.end(); it_path++) {
        //            std::cout << (*it_path)->getID() << " ";
        //        }
        //        std::cout << "\n";
        //    }
        //}
    }


    void init() {
        // all EdgeInfos touched in the previous query are either in myFrontier or myFound: clean those up
        for (typename std::vector<EdgeInfo*>::iterator i = myFrontier.begin(); i != myFrontier.end(); i++) {
            (*i)->reset();
        }
        myFrontier.clear();
        for (typename std::vector<EdgeInfo*>::iterator i = myFound.begin(); i != myFound.end(); i++) {
            (*i)->reset();
        }
        myFound.clear();
    }


    /** @brief Builds the route between the given edges using the minimum travel time */
    void compute(const E* from, const E* to, const V* const vehicle,
                 SUMOTime msTime, std::vector<const E*>& into) {
        assert(from != 0 && to != 0);
        this->startQuery();
        init();
        const Prepared prepared = (myPreparedDestination == 0 ?
                                   NO : (myPreparedDestination == to ? YES_EXACT : YES));
        const SUMOReal time = STEPS2TIME(msTime);
        const EdgeInfo& toInfo = myEdgeInfos[to->getNumericalID()];
        EdgeInfo* const fromInfo = &(myEdgeInfos[from->getNumericalID()]);
        fromInfo->traveltime = 0;
        fromInfo->prev = 0;
        myFrontier.push_back(fromInfo);
        // loop
        int num_visited = 0;
        while (!myFrontier.empty()) {
            num_visited += 1;
            // use the node with the minimal length
            EdgeInfo* const minimumInfo = myFrontier.front();
            const E* const minEdge = minimumInfo->edge;
            pop_heap(myFrontier.begin(), myFrontier.end(), myComparator);
            myFrontier.pop_back();
            myFound.push_back(minimumInfo);
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumInfo, into);
                this->endQuery(num_visited);
                // DEBUG
                //std::cout << "visited " + toString(num_visited) + " edges (final path length: " + toString(into.size()) + ")\n";
                return;
            }
            minimumInfo->visited = true;
            const SUMOReal traveltime = minimumInfo->traveltime + this->getEffort(minEdge, vehicle, time + minimumInfo->traveltime);
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            const unsigned int length_size = minEdge->getNumSuccessors();
            for (i = 0; i < length_size; i++) {
                const E* const follower = minEdge->getSuccessor(i);
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const SUMOReal oldEffort = followerInfo->traveltime;
                if (!followerInfo->visited && traveltime < oldEffort) {
                    followerInfo->traveltime = traveltime;
                    // admissible A* heuristic:
                    SUMOReal heuristic_remaining = 0;
                    switch (prepared) {
                        case NO:
                            // straight line distance at maximum speed
                            heuristic_remaining = minEdge->getDistanceTo(to) / vehicle->getMaxSpeed();
                            break;
                        case YES_EXACT:
                            // shortest path for fastest vehicle in uncongested network
                            heuristic_remaining = minimumInfo->minRemaining;
                            break;
                        case YES:
                            // triangle inequality
                            heuristic_remaining = MAX2(
                                                      minimumInfo->minRemaining - toInfo.minRemaining,
                                                      minEdge->getDistanceTo(to) / vehicle->getMaxSpeed());
                            break;
                    }
                    followerInfo->heuristicTime = traveltime + heuristic_remaining;
                    followerInfo->prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<SUMOReal>::max()) {
                        myFrontier.push_back(followerInfo);
                        push_heap(myFrontier.begin(), myFrontier.end(), myComparator);
                    } else {
                        push_heap(myFrontier.begin(),
                                  find(myFrontier.begin(), myFrontier.end(), followerInfo) + 1,
                                  myComparator);
                    }
                }
            }
        }
        this->endQuery(num_visited);
        myErrorMsgHandler->inform("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
    }


    SUMOReal recomputeCosts(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime) const {
        const SUMOReal time = STEPS2TIME(msTime);
        SUMOReal costs = 0;
        for (typename std::vector<const E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                return -1;
            }
            costs += this->getEffort(*i, v, time + costs);
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

private:
    enum Prepared {
        NO,
        YES,
        YES_EXACT // optimistic shortest paths are computed for the current destination
    };

    /// The container of edge information
    std::vector<EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<EdgeInfo*> myFrontier;
    /// @brief list of visited Edges (for resetting)
    std::vector<EdgeInfo*> myFound;

    EdgeInfoComparator myComparator;

    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

    MinTTOperation myMinTTOperation;

    const E* myPreparedDestination;
};


#endif

/****************************************************************************/

