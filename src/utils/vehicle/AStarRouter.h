/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AStarRouter.h
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    January 2012
/// @version $Id$
///
// A* Algorithm using euclidean distance heuristic.
// Based on DijkstraRouter. For routing by effort a novel heuristic would be needed.
/****************************************************************************/
#ifndef AStarRouter_h
#define AStarRouter_h


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
#include <map>
#include <iostream>
#include <memory>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include "AStarLookupTable.h"
#include "SUMOAbstractRouter.h"

#define UNREACHABLE (std::numeric_limits<double>::max() / 1000.0)

//#define ASTAR_DEBUG_QUERY
//#define ASTAR_DEBUG_QUERY_FOLLOWERS
//#define ASTAR_DEBUG_QUERY_PERF
//#define ASTAR_DEBUG_VISITED
//#define ASTAR_DEBUG_LOOKUPTABLE
//#define ASTAR_DEBUG_LOOKUPTABLE_FROM "disabled"
//#define ASTAR_DEBUG_UNREACHABLE

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
 * @param BASE The base class to use (SUMOAbstractRouterPermissions/SUMOAbstractRouter)
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class BASE>
class AStarRouter : public BASE {
public:
    typedef AbstractLookupTable<E, V> LookupTable;
    typedef FullLookupTable<E, V> FLT;
    typedef LandmarkLookupTable<E, V> LMLT;

    /**
     * @class EdgeInfoComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoComparator {
    public:
        /// Comparing method
        bool operator()(const typename BASE::EdgeInfo* nod1, const typename BASE::EdgeInfo* nod2) const {
            if (nod1->heuristicEffort == nod2->heuristicEffort) {
                return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
            }
            return nod1->heuristicEffort > nod2->heuristicEffort;
        }
    };

    /// Constructor
    AStarRouter(const std::vector<E*>& edges, bool unbuildIsWarning, typename BASE::Operation operation, const std::shared_ptr<const LookupTable> lookup = 0) :
        BASE("AStarRouter", operation),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            myEdgeInfos.push_back(typename BASE::EdgeInfo(*i));
            myMaxSpeed = MAX2(myMaxSpeed, (*i)->getSpeedLimit() * MAX2(1.0, (*i)->getLengthGeometryFactor()));
        }
    }

    AStarRouter(const std::vector<typename BASE::EdgeInfo>& edgeInfos, bool unbuildIsWarning, typename BASE::Operation operation, const std::shared_ptr<const LookupTable> lookup = 0) :
        BASE("AStarRouter", operation),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS) {
        for (const auto& edgeInfo : edgeInfos) {
            myEdgeInfos.push_back(typename BASE::EdgeInfo(edgeInfo.edge));
            myMaxSpeed = MAX2(myMaxSpeed, edgeInfo.edge->getSpeedLimit() * edgeInfo.edge->getLengthGeometryFactor());
        }
    }

    /// Destructor
    virtual ~AStarRouter() {}

    virtual SUMOAbstractRouter<E, V>* clone() {
        return new AStarRouter<E, V, BASE>(myEdgeInfos, myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, myLookupTable);
    }

    void init() {
        // all EdgeInfos touched in the previous query are either in myFrontierList or myFound: clean those up
        for (auto& edgeInfo : myFrontierList) {
            edgeInfo->reset();
        }
        myFrontierList.clear();
        for (auto& edgeInfo : myFound) {
            edgeInfo->reset();
        }
        myFound.clear();
    }


    /** @brief Builds the route between the given edges using the minimum travel time */
    virtual bool compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, std::vector<const E*>& into) {
        assert(from != 0 && to != 0);
        // check whether from and to can be used
        if (this->isProhibited(from, vehicle)) {
            myErrorMsgHandler->inform("Vehicle '" + vehicle->getID() + "' is not allowed on source edge '" + from->getID() + "'.");
            return false;
        }
        if (this->isProhibited(to, vehicle)) {
            myErrorMsgHandler->inform("Vehicle '" + vehicle->getID() + "' is not allowed on destination edge '" + to->getID() + "'.");
            return false;
        }
        double length = 0.; // dummy for the via edge cost update
        this->startQuery();
#ifdef ASTAR_DEBUG_QUERY
        std::cout << "DEBUG: starting search for '" << vehicle->getID() << "' speed: " << MIN2(vehicle->getMaxSpeed(), myMaxSpeed * vehicle->getChosenSpeedFactor()) << " time: " << STEPS2TIME(msTime) << "\n";
#endif
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        if (this->myBulkMode) {
            const auto& toInfo = myEdgeInfos[to->getNumericalID()];
            if (toInfo.visited) {
                buildPathFrom(&toInfo, into);
                this->endQuery(1);
                return true;
            }
        } else {
            init();
            // add begin node
            auto* const fromInfo = &(myEdgeInfos[from->getNumericalID()]);
            fromInfo->effort = 0.;
            fromInfo->prev = nullptr;
            fromInfo->leaveTime = STEPS2TIME(msTime);
            myFrontierList.push_back(fromInfo);
        }
        // loop
        int num_visited = 0;
        const bool mayRevisit = myLookupTable != 0 && !myLookupTable->consistent();
        const double speed = vehicle == nullptr ? myMaxSpeed : MIN2(vehicle->getMaxSpeed(), myMaxSpeed * vehicle->getChosenSpeedFactor());
        while (!myFrontierList.empty()) {
            num_visited += 1;
            // use the node with the minimal length
            auto* const minimumInfo = myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumInfo, into);
                this->endQuery(num_visited);
#ifdef ASTAR_DEBUG_QUERY_PERF
                std::cout << "visited " + toString(num_visited) + " edges (final path length=" + toString(into.size())
                          + " time=" + toString(recomputeCosts(into, vehicle, msTime))
                          + " edges=" + toString(into) + ")\n";
#endif
#ifdef ASTAR_DEBUG_VISITED
                OutputDevice& dev = OutputDevice::getDevice(vehicle->getID() + "_" + time2string(msTime) + "_" + from->getID() + "_" + to->getID());
                for (const auto& i : myEdgeInfos) {
                    if (i.visited) {
                        dev << "edge:" << i.edge->getID() << "\n";
                    }
                }
                dev.close();
#endif
                return true;
            }
            pop_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
            myFrontierList.pop_back();
            myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
#ifdef ASTAR_DEBUG_QUERY
            std::cout << "DEBUG: hit=" << minEdge->getID()
                      << " TT=" << minimumInfo->traveltime
                      << " EF=" << this->getEffort(minEdge, vehicle, time + minimumInfo->traveltime)
                      << " HT=" << minimumInfo->heuristicTime
                      << " Q(TT,HT,Edge)=";
            for (typename std::vector<EdgeInfo*>::iterator it = myFrontierList.begin(); it != myFrontierList.end(); it++) {
                std::cout << (*it)->traveltime << "," << (*it)->heuristicTime << "," << (*it)->edge->getID() << " ";
            }
            std::cout << "\n";
#endif
            const double effortDelta = this->getEffort(minEdge, vehicle, minimumInfo->leaveTime);
            const double leaveTime = minimumInfo->leaveTime + this->getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);

            // admissible A* heuristic: straight line distance at maximum speed
            const double heuristic_remaining = (myLookupTable == nullptr ? minEdge->getDistanceTo(to) / speed :
                                                myLookupTable->lowerBound(minEdge, to, speed, vehicle->getChosenSpeedFactor(), minEdge->getMinimumTravelTime(0), to->getMinimumTravelTime(0)));
            if (heuristic_remaining == UNREACHABLE) {
                continue;
            }
            // check all ways from the node with the minimal length
            for (const std::pair<const E*, const E*>& follower : minEdge->getViaSuccessors(vClass)) {
                auto* const followerInfo = &(myEdgeInfos[follower.first->getNumericalID()]);
                // check whether it can be used
                if (this->isProhibited(follower.first, vehicle)) {
                    continue;
                }
                double effort = minimumInfo->effort + effortDelta;
                double time = leaveTime;
                this->updateViaEdgeCost(follower.second, vehicle, time, effort, length);
                const double oldEffort = followerInfo->effort;
                if ((!followerInfo->visited || mayRevisit) && effort < oldEffort) {
                    followerInfo->effort = effort;
                    followerInfo->heuristicEffort = effort + heuristic_remaining;
                    followerInfo->leaveTime = time;
                    followerInfo->prev = minimumInfo;
                    /* the code below results in fewer edges being looked up but is more costly due to the effort
                       calculations. Overall it resulted in a slowdown in the Berlin tests but could be made configurable someday.
                    followerInfo->heuristicTime = traveltime;
                    if (follower != to) {
                        if (myLookupTable == 0) {
                            // admissible A* heuristic: straight line distance at maximum speed
                            followerInfo->heuristicTime += this->getEffort(follower, vehicle, time + traveltime) + follower->getDistanceTo(to) / speed;
                        } else {
                            followerInfo->heuristicTime += this->getEffort(follower, vehicle, time + traveltime) + (*myLookupTable)[follower->getNumericalID()][to->getNumericalID()] / vehicle->getChosenSpeedFactor();
                        }
                    }*/
#ifdef ASTAR_DEBUG_QUERY_FOLLOWERS
                    std::cout << "   follower=" << followerInfo->edge->getID() << " OEF=" << oldEffort << " TT=" << traveltime << " HR=" << heuristic_remaining << " HT=" << followerInfo->heuristicTime << "\n";
#endif
                    if (oldEffort == std::numeric_limits<double>::max()) {
                        myFrontierList.push_back(followerInfo);
                        push_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
                    } else {
                        auto fi = find(myFrontierList.begin(), myFrontierList.end(), followerInfo);
                        if (fi == myFrontierList.end()) {
                            assert(mayRevisit);
                            myFrontierList.push_back(followerInfo);
                            push_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
                        } else {
                            push_heap(myFrontierList.begin(), fi + 1, myComparator);
                        }
                    }
                }
            }
        }
        this->endQuery(num_visited);
#ifdef ASTAR_DEBUG_QUERY_PERF
        std::cout << "visited " + toString(num_visited) + " edges (unsuccesful path length: " + toString(into.size()) + ")\n";
#endif
        myErrorMsgHandler->inform("No connection between edge '" + from->getID() + "' and edge '" + to->getID() + "' found.");
        return false;
    }

public:
    /// Builds the path from marked edges
    void buildPathFrom(const typename BASE::EdgeInfo* rbegin, std::vector<const E*>& edges) {
        std::vector<const E*> tmp;
        while (rbegin != 0) {
            tmp.push_back(rbegin->edge);
            rbegin = rbegin->prev;
        }
        std::copy(tmp.rbegin(), tmp.rend(), std::back_inserter(edges));
    }

protected:
    /// The container of edge information
    std::vector<typename BASE::EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<typename BASE::EdgeInfo*> myFrontierList;
    /// @brief list of visited Edges (for resetting)
    std::vector<typename BASE::EdgeInfo*> myFound;

    EdgeInfoComparator myComparator;

    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

    /// @brief the lookup table for travel time heuristics
    const std::shared_ptr<const LookupTable> myLookupTable;

    /// @brief maximum speed in the network
    double myMaxSpeed;
};


#endif

/****************************************************************************/

