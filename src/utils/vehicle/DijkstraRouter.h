/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    DijkstraRouter.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Dijkstra shortest path algorithm using travel time or other values
/****************************************************************************/
#ifndef DijkstraRouter_h
#define DijkstraRouter_h


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
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include "SUMOAbstractRouter.h"

//#define DijkstraRouter_DEBUG_QUERY
//#define DijkstraRouter_DEBUG_QUERY_PERF

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DijkstraRouter
 * @brief Computes the shortest path through a network using the Dijkstra algorithm.
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
class DijkstraRouter : public SUMOAbstractRouter<E, V>, public PF {

public:
    typedef double(* Operation)(const E* const, const V* const, double);

    /**
     * @class EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(const E* const e)
            : edge(e), effort(std::numeric_limits<double>::max()), leaveTime(0), prev(0), visited(false) {}

        /// The current edge
        const E* const edge;

        /// Effort to reach the edge
        double effort;

        /// The time the vehicle leaves the edge
        double leaveTime;

        /// The previous edge
        const EdgeInfo* prev;

        /// The previous edge
        bool visited;

        inline void reset() {
            effort = std::numeric_limits<double>::max();
            visited = false;
        }

    private:
        /// @brief Invalidated assignment operator
        EdgeInfo& operator=(const EdgeInfo& s) = delete;

    };

    /**
     * @class EdgeInfoByEffortComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByEffortComparator {
    public:
        /// Comparing method
        bool operator()(const EdgeInfo* nod1, const EdgeInfo* nod2) const {
            if (nod1->effort == nod2->effort) {
                return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
            }
            return nod1->effort > nod2->effort;
        }
    };


    /// Constructor
    DijkstraRouter(const std::vector<E*>& edges, bool unbuildIsWarning, Operation effortOperation, Operation ttOperation = nullptr) :
        SUMOAbstractRouter<E, V>(effortOperation, "DijkstraRouter"), myTTOperation(ttOperation),
        myErrorMsgHandler(unbuildIsWarning ?  MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            myEdgeInfos.push_back(EdgeInfo(*i));
        }
    }

    /// Destructor
    virtual ~DijkstraRouter() { }

    virtual SUMOAbstractRouter<E, V>* clone() {
        return new DijkstraRouter<E, V, PF>(myEdgeInfos, myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, myTTOperation);
    }

    inline double getTravelTime(const E* const e, const V* const v, const double t, const double effort) const {
        return myTTOperation == nullptr ? effort : (*myTTOperation)(e, v, t);
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


    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    virtual bool compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, std::vector<const E*>& into) {
        assert(from != 0 && (vehicle == 0 || to != 0));
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
#ifdef DijkstraRouter_DEBUG_QUERY
        std::cout << "DEBUG: starting search for '" << vehicle->getID() << "' time: " << STEPS2TIME(msTime) << "\n";
#endif
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
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
            fromInfo->effort = 0;
            fromInfo->prev = 0;
            fromInfo->leaveTime = STEPS2TIME(msTime);
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
#ifdef DijkstraRouter_DEBUG_QUERY_PERF
                std::cout << "visited " + toString(num_visited) + " edges (final path length=" + toString(into.size()) + " edges=" + toString(into) + ")\n";
#endif
                return true;
            }
            pop_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
            myFrontierList.pop_back();
            myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
#ifdef DijkstraRouter_DEBUG_QUERY
            std::cout << "DEBUG: hit '" << minEdge->getID() << "' Eff: " << minimumInfo->effort << ", TT: " << minimumInfo->leaveTime << " Q: ";
            for (typename std::vector<EdgeInfo*>::iterator it = myFrontierList.begin(); it != myFrontierList.end(); it++) {
                std::cout << (*it)->effort << "," << (*it)->edge->getID() << " ";
            }
            std::cout << "\n";
#endif
            const double effortDelta = this->getEffort(minEdge, vehicle, minimumInfo->leaveTime);
            const double effort = minimumInfo->effort + effortDelta;
            const double leaveTime = minimumInfo->leaveTime + getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);
            // check all ways from the node with the minimal length
            const std::vector<E*>& successors = minEdge->getSuccessors(vClass);
            for (typename std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                const E* const follower = *it;
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const double oldEffort = followerInfo->effort;
                if (!followerInfo->visited && effort < oldEffort) {
                    followerInfo->effort = effort;
                    followerInfo->leaveTime = leaveTime;
                    followerInfo->prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<double>::max()) {
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
#ifdef DijkstraRouter_DEBUG_QUERY_PERF
        std::cout << "visited " + toString(num_visited) + " edges (unsuccesful path length: " + toString(into.size()) + ")\n";
#endif
        if (to != 0) {
            myErrorMsgHandler->inform("No connection between edge '" + from->getID() + "' and edge '" + to->getID() + "' found.");
        }
        return false;
    }


    double recomputeCosts(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime) const {
        double costs = 0;
        double t = STEPS2TIME(msTime);
        for (const E* const e : edges) {
            if (PF::operator()(e, v)) {
                return -1;
            }
            const double effortDelta = this->getEffort(e, v, t);
            costs += effortDelta;
            t += getTravelTime(e, v, t, effortDelta);
        }
        return costs;
    }

    /// Builds the path from marked edges
    void buildPathFrom(const EdgeInfo* rbegin, std::vector<const E*>& edges) {
        std::vector<const E*> tmp;
        while (rbegin != 0) {
            tmp.push_back(rbegin->edge);
            rbegin = rbegin->prev;
        }
        std::copy(tmp.rbegin(), tmp.rend(), std::back_inserter(edges));
    }

    const EdgeInfo& getEdgeInfo(int index) const {
        return myEdgeInfos[index];
    }

private:
    DijkstraRouter(const std::vector<EdgeInfo>& edgeInfos, bool unbuildIsWarning, Operation effortOperation, Operation ttOperation) :
        SUMOAbstractRouter<E, V>(effortOperation, "DijkstraRouter"), myTTOperation(ttOperation),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()) {
        for (const EdgeInfo& ei : edgeInfos) {
            myEdgeInfos.push_back(EdgeInfo(ei.edge));
        }
    }

private:
    /// @brief The object's operation to perform for travel times
    Operation myTTOperation;

    /// The container of edge information
    std::vector<EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<EdgeInfo*> myFrontierList;
    /// @brief list of visited Edges (for resetting)
    std::vector<EdgeInfo*> myFound;

    EdgeInfoByEffortComparator myComparator;

    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;
};


#endif

/****************************************************************************/

