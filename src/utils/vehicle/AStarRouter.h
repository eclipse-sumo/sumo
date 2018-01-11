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
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
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
    typedef AbstractLookupTable<E, V> LookupTable;
    typedef FullLookupTable<E, V> FLT;
    typedef LandmarkLookupTable<E, V> LMLT;
    typedef double(* Operation)(const E* const, const V* const, double);



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
            traveltime(std::numeric_limits<double>::max()),
            heuristicTime(std::numeric_limits<double>::max()),
            prev(0),
            visited(false) {
        }

        /// The current edge
        const E* edge;

        /// Effort to reach the edge
        double traveltime;

        /// Estimated time to reach the edge (traveltime + lower bound on remaining time)
        double heuristicTime;

        /// The previous edge
        EdgeInfo* prev;

        /// The previous edge
        bool visited;

        inline void reset() {
            // heuristicTime is set before adding to the frontier, thus no reset is needed
            traveltime = std::numeric_limits<double>::max();
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
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS) {
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            myEdgeInfos.push_back(EdgeInfo(*i));
            myMaxSpeed = MAX2(myMaxSpeed, (*i)->getSpeedLimit() * MAX2(1.0, (*i)->getLengthGeometryFactor()));
        }
    }

    AStarRouter(const std::vector<EdgeInfo>& edgeInfos, bool unbuildIsWarning, Operation operation, const LookupTable* const lookup = 0):
        SUMOAbstractRouter<E, V>(operation, "AStarRouter"),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS) {
        for (typename std::vector<EdgeInfo>::const_iterator i = edgeInfos.begin(); i != edgeInfos.end(); ++i) {
            myEdgeInfos.push_back(EdgeInfo(i->edge));
            myMaxSpeed = MAX2(myMaxSpeed, i->edge->getSpeedLimit() * i->edge->getLengthGeometryFactor());
        }
    }

    /// Destructor
    virtual ~AStarRouter() {}

    virtual SUMOAbstractRouter<E, V>* clone() {
        return new AStarRouter<E, V, PF>(myEdgeInfos, myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, myLookupTable);
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
#ifdef ASTAR_DEBUG_QUERY
        std::cout << "DEBUG: starting search for '" << vehicle->getID() << "' speed: " << MIN2(vehicle->getMaxSpeed(), myMaxSpeed * vehicle->getChosenSpeedFactor()) << " time: " << STEPS2TIME(msTime) << "\n";
#endif
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        const double time = STEPS2TIME(msTime);
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
        const bool mayRevisit = myLookupTable != 0 && !myLookupTable->consistent();
        const double speed = MIN2(vehicle->getMaxSpeed(), myMaxSpeed * vehicle->getChosenSpeedFactor());
        while (!myFrontierList.empty()) {
            num_visited += 1;
            // use the node with the minimal length
            EdgeInfo* const minimumInfo = myFrontierList.front();
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
                for (typename std::vector<EdgeInfo>::const_iterator i = myEdgeInfos.begin(); i != myEdgeInfos.end(); ++i) {
                    if (i->visited) {
                        dev << "edge:" << i->edge->getID() << "\n";
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
            const double traveltime = minimumInfo->traveltime + this->getEffort(minEdge, vehicle, time + minimumInfo->traveltime);
            // admissible A* heuristic: straight line distance at maximum speed
            const double heuristic_remaining = (myLookupTable == 0 ? minEdge->getDistanceTo(to) / speed :
                                                myLookupTable->lowerBound(minEdge, to, speed, vehicle->getChosenSpeedFactor(), minEdge->getMinimumTravelTime(0), to->getMinimumTravelTime(0)));
            if (heuristic_remaining == UNREACHABLE) {
                continue;
            }
            // check all ways from the node with the minimal length
            const std::vector<E*>& successors = minEdge->getSuccessors(vClass);
            for (typename std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                const E* const follower = *it;
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const double oldEffort = followerInfo->traveltime;
                if ((!followerInfo->visited || mayRevisit) && traveltime < oldEffort) {
                    followerInfo->traveltime = traveltime;
                    followerInfo->heuristicTime = traveltime + heuristic_remaining;
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
                    followerInfo->prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<double>::max()) {
                        myFrontierList.push_back(followerInfo);
                        push_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
                    } else {
                        typename std::vector<EdgeInfo*>::iterator fi = find(myFrontierList.begin(), myFrontierList.end(), followerInfo);
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


    double recomputeCosts(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime) const {
        const double time = STEPS2TIME(msTime);
        double costs = 0;
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

    /// @brief maximum speed in the network
    double myMaxSpeed;
};


#endif

/****************************************************************************/

