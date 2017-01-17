/****************************************************************************/
/// @file    AStarRouter.h
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    January 2012
/// @version $Id$
///
// A* Algorithm using euclidean distance heuristic.
// Based on DijkstraRouterTT. For routing by effort a novel heuristic would be needed.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AStarRouter_h
#define AStarRouter_h


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
#include <map>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "SUMOAbstractRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AStarRouter
 * @brief Computes the shortest path through a network using the A* algorithm.
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withPermissions/noProhibitions)
 * @param EC The class to retrieve the effort for an edge from
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class PF>
class AStarRouter : public SUMOAbstractRouter<E, V>, public PF {

public:
    typedef SUMOReal(* Operation)(const E* const, const V* const, SUMOReal);
    typedef std::vector<std::vector<SUMOReal> > LookupTable;

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(const E* e) :
            edge(e),
            traveltime(std::numeric_limits<SUMOReal>::max()),
            heuristicTime(std::numeric_limits<SUMOReal>::max()),
            prev(0),
            visited(false) {
        }

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

    /// Constructor
    AStarRouter(const std::vector<E*>& edges, bool unbuildIsWarning, Operation operation, const LookupTable* const lookup = 0):
        SUMOAbstractRouter<E, V>(operation, "AStarRouter"),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myLookupTable(lookup) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            myEdgeInfos.push_back(EdgeInfo(*i));
        }
    }

    AStarRouter(const std::vector<EdgeInfo>& edgeInfos, bool unbuildIsWarning, Operation operation, const LookupTable* const lookup = 0):
        SUMOAbstractRouter<E, V>(operation, "AStarRouter"),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myLookupTable(lookup) {
        for (typename std::vector<EdgeInfo>::const_iterator i = edgeInfos.begin(); i != edgeInfos.end(); ++i) {
            myEdgeInfos.push_back(*i);
        }
    }

    /// Destructor
    virtual ~AStarRouter() {}

    virtual SUMOAbstractRouter<E, V>* clone() {
        return new AStarRouter<E, V, PF>(myEdgeInfos, myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, myLookupTable);
    }

    static LookupTable* createLookupTable(const std::string& filename, const int size) {
        LookupTable* const result = new LookupTable();
        BinaryInputDevice dev(filename);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                SUMOReal val;
                dev >> val;
                (*result)[i].push_back(val);
            }
        }
        return result;
    }

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
    virtual bool compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, std::vector<const E*>& into) {
        assert(from != 0 && to != 0);
        // check whether from and to can be used
        if (PF::operator()(from, vehicle)) {
            myErrorMsgHandler->inform("Vehicle '" + vehicle->getID() + "' is not allowed on source edge '" + from->getID() + "'.");
            return false;
        }
        if (PF::operator()(to, vehicle)) {
            myErrorMsgHandler->inform("Vehicle '" + vehicle->getID() + "' is not allowed on destination edge '" + to->getID() + "'.");
            return false;
        }
        this->startQuery();
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        const SUMOReal time = STEPS2TIME(msTime);
        if (this->myBulkMode) {
            const EdgeInfo& toInfo = myEdgeInfos[to->getNumericalID()];
            if (toInfo.visited) {
                buildPathFrom(&toInfo, into);
                this->endQuery(1);
                return true;
            }
        } else {
            init();
            // add begin node
            EdgeInfo* const fromInfo = &(myEdgeInfos[from->getNumericalID()]);
            fromInfo->traveltime = 0;
            fromInfo->prev = 0;
            myFrontierList.push_back(fromInfo);
        }
        // loop
        int num_visited = 0;
        while (!myFrontierList.empty()) {
            num_visited += 1;
            // use the node with the minimal length
            EdgeInfo* const minimumInfo = myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumInfo, into);
                this->endQuery(num_visited);
                return true;
            }
            pop_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
            myFrontierList.pop_back();
            myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
            const SUMOReal traveltime = minimumInfo->traveltime + this->getEffort(minEdge, vehicle, time + minimumInfo->traveltime);
            // admissible A* heuristic: straight line distance at maximum speed
            const SUMOReal heuristic_remaining = myLookupTable == 0 ? minEdge->getDistanceTo(to) / vehicle->getMaxSpeed() : (*myLookupTable)[minEdge->getNumericalID()][to->getNumericalID()] / vehicle->getChosenSpeedFactor();
            // check all ways from the node with the minimal length
            const std::vector<E*>& successors = minEdge->getSuccessors(vClass);
            for (typename std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                const E* const follower = *it;
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const SUMOReal oldEffort = followerInfo->traveltime;
                if (!followerInfo->visited && traveltime < oldEffort) {
                    followerInfo->traveltime = traveltime;
                    followerInfo->heuristicTime = traveltime + heuristic_remaining;
                    /* the code below results in fewer edges being looked up but is more costly due to the effort
                       calculations. Overall it resulted in a slowdown in the Berlin tests but could be made configurable someday.
                    followerInfo->heuristicTime = traveltime;
                    if (follower != to) {
                        if (myLookupTable == 0) {
                            // admissible A* heuristic: straight line distance at maximum speed
                            followerInfo->heuristicTime += this->getEffort(follower, vehicle, time + traveltime) + follower->getDistanceTo(to) / vehicle->getMaxSpeed();
                        } else {
                            followerInfo->heuristicTime += this->getEffort(follower, vehicle, time + traveltime) + (*myLookupTable)[follower->getNumericalID()][to->getNumericalID()] / vehicle->getChosenSpeedFactor();
                        }
                    }*/
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
        this->endQuery(num_visited);
        myErrorMsgHandler->inform("No connection between edge '" + from->getID() + "' and edge '" + to->getID() + "' found.");
        return false;
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
    void buildPathFrom(const EdgeInfo* rbegin, std::vector<const E*>& edges) {
        std::vector<const E*> tmp;
        while (rbegin != 0) {
            tmp.push_back(rbegin->edge);
            rbegin = rbegin->prev;
        }
        std::copy(tmp.rbegin(), tmp.rend(), std::back_inserter(edges));
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

    /// @brief the lookup table for travel time heuristics
    const LookupTable* const myLookupTable;
};


#endif

/****************************************************************************/

