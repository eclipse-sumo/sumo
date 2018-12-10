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
#include <config.h>

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
#include "EffortCalculator.h"
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
 * @param BASE The base class to use (SUMOAbstractRouterPermissions/SUMOAbstractRouter)
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class BASE>
class DijkstraRouter : public BASE {
public:
    /**
     * @class EdgeInfoByEffortComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByEffortComparator {
    public:
        /// Comparing method
        bool operator()(const typename BASE::EdgeInfo* nod1, const typename BASE::EdgeInfo* nod2) const {
            if (nod1->effort == nod2->effort) {
                return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
            }
            return nod1->effort > nod2->effort;
        }
    };


    /// Constructor
    DijkstraRouter(const std::vector<E*>& edges, bool unbuildIsWarning, typename BASE::Operation effortOperation,
                   typename BASE::Operation ttOperation = nullptr, bool silent = false, EffortCalculator* calc = nullptr) :
        BASE("DijkstraRouter", effortOperation, ttOperation),
        myErrorMsgHandler(unbuildIsWarning ?  MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        mySilent(silent), myExternalEffort(calc) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            myEdgeInfos.push_back(EdgeInfo(*i));
        }
    }

    /// Destructor
    virtual ~DijkstraRouter() { }

    virtual SUMOAbstractRouter<E, V>* clone() {
        return new DijkstraRouter<E, V, BASE>(myEdgeInfos, myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, this->myTTOperation, mySilent, myExternalEffort);
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
        if (this->isProhibited(from, vehicle)) {
            myErrorMsgHandler->inform("Vehicle '" + vehicle->getID() + "' is not allowed on source edge '" + from->getID() + "'.");
            return false;
        }
        if (this->isProhibited(to, vehicle)) {
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
            fromInfo->effort = 0.;
            fromInfo->prev = nullptr;
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
#ifdef DijkstraRouter_DEBUG_QUERY
            std::cout << "DEBUG: hit '" << minEdge->getID() << "' Eff: " << minimumInfo->effort << ", Leave: " << minimumInfo->leaveTime << " Q: ";
            for (typename std::vector<EdgeInfo*>::iterator it = myFrontierList.begin(); it != myFrontierList.end(); it++) {
                std::cout << (*it)->effort << "," << (*it)->edge->getID() << " ";
            }
            std::cout << "\n";
#endif
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
            const E* viaEdge = minimumInfo->via;
            double effort = minimumInfo->effort;
            double leaveTime = minimumInfo->leaveTime;
            while (viaEdge != nullptr && viaEdge->isInternal()) {
                const double viaEffortDelta = this->getEffort(viaEdge, vehicle, leaveTime);
                leaveTime += this->getTravelTime(viaEdge, vehicle, leaveTime, viaEffortDelta);
                effort += viaEffortDelta;
                viaEdge = viaEdge->getViaSuccessors().front().first;
            }
            const double effortDelta = this->getEffort(minEdge, vehicle, leaveTime);
            leaveTime += this->getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);
            effort += effortDelta;
            if (myExternalEffort != nullptr) {
                myExternalEffort->update(minEdge->getNumericalID(), minimumInfo->prev->edge->getNumericalID(), minEdge->getLength());
            }
            assert(effort >= minimumInfo->effort);
            assert(leaveTime >= minimumInfo->leaveTime);
            // check all ways from the node with the minimal length
            for (const std::pair<const E*, const E*>& follower : minEdge->getViaSuccessors(vClass)) {
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower.first->getNumericalID()]);
                // check whether it can be used
                if (this->isProhibited(follower.first, vehicle)) {
                    continue;
                }
                const double oldEffort = followerInfo->effort;
                if (!followerInfo->visited && effort < oldEffort) {
                    followerInfo->effort = effort;
                    followerInfo->leaveTime = leaveTime;
                    followerInfo->prev = minimumInfo;
                    followerInfo->via = follower.second;
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
        std::cout << "visited " + toString(num_visited) + " edges (unsuccessful path length: " + toString(into.size()) + ")\n";
#endif
        if (to != 0 && !mySilent) {
            myErrorMsgHandler->inform("No connection between edge '" + from->getID() + "' and edge '" + to->getID() + "' found.");
        }
        return false;
    }


    /// Builds the path from marked edges
    void buildPathFrom(const typename BASE::EdgeInfo* rbegin, std::vector<const E*>& edges) {
        std::vector<const E*> tmp;
        while (rbegin != 0) {
            tmp.push_back(rbegin->edge);
            rbegin = rbegin->prev;
        }
        std::copy(tmp.rbegin(), tmp.rend(), std::back_inserter(edges));
    }

    const typename BASE::EdgeInfo& getEdgeInfo(int index) const {
        return myEdgeInfos[index];
    }

private:
    DijkstraRouter(const std::vector<typename BASE::EdgeInfo>& edgeInfos, bool unbuildIsWarning,
                   typename BASE::Operation effortOperation, typename BASE::Operation ttOperation, bool silent, EffortCalculator* calc) :
        BASE("DijkstraRouter", effortOperation, ttOperation),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        mySilent(silent),
        myExternalEffort(calc) {
        for (const EdgeInfo& ei : edgeInfos) {
            myEdgeInfos.push_back(EdgeInfo(ei.edge));
        }
    }

private:
    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

    /// @brief whether to supress warning/error if no route was found
    bool mySilent;

    EffortCalculator* const myExternalEffort;

    /// The container of edge information
    std::vector<typename BASE::EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<typename BASE::EdgeInfo*> myFrontierList;
    /// @brief list of visited Edges (for resetting)
    std::vector<typename BASE::EdgeInfo*> myFound;

    EdgeInfoByEffortComparator myComparator;
};


#endif

/****************************************************************************/
