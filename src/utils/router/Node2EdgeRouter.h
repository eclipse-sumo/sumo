/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Node2EdgeRouter.h
/// @author  Ruediger Ebendt
/// @date    01.01.2024
///
// Simple extension of edge-based AStarRouter, adding a) a new method computeNode2Edge, 
// which computes a shortest path starting at a given node and ending at a given edge, 
// and b) a new method computeNode2Edges, which computes shortest paths, each starting 
// at the given node and ending at one of the given edges
/****************************************************************************/
#pragma once
#include <config.h>
#include <unordered_set>
#include <utils/common/StdDefs.h>
#include "AStarRouter.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Node2EdgeRouter
 * Computes shortest paths from a node to one edge (using A*) or to all edges 
 * (using Dijkstra's algorithm)
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 *
 * The router is edge-based. It must know the number of edges for internal reasons 
 * and whether a missing connection between two given edges (unbuild route) shall 
 * be reported as an error or as a warning
 *
 */
template<class E, class N, class V>
class Node2EdgeRouter : public AStarRouter<E, V> {
public:
    typedef AbstractLookupTable<E, V> LookupTable;
 
    /** @brief Returns the edge information for the passed edge
     * @param[in] edge The edge
     * @note Non-const version
     */
    typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo(const E* const edge) {
        return &(this->myEdgeInfos[edge->getNumericalID()]);
    }
    /** @brief Returns the edge information for the passed edge
     * @param[in] edge The edge
     * @note Const version
     */
    const typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo(const E* const edge) const {
        return &(this->myEdgeInfos[edge->getNumericalID()]);
    }

    /** @brief Constructor
     * @param[in] edges The edges
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] operation The operation
     * @param[in] lookup The lookup table
     * @param[in] havePermissions The flag indicating whether to respect edge permissions
     * @param[in] haveRestrictions The flag indicating whether to respect edge restrictions
     */
    Node2EdgeRouter(const std::vector<E*>& edges, bool unbuildIsWarning, typename SUMOAbstractRouter<E, V>::Operation operation, 
        const std::shared_ptr<const LookupTable> lookup = nullptr,
        const bool havePermissions = false, const bool haveRestrictions = false) :
        AStarRouter<E, V>(edges, unbuildIsWarning, operation, lookup, havePermissions, haveRestrictions) {
    }

    /** @brief Cloning constructor
     * @param[in] edgeInfos The vector of edge information
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] operation The operation
     * @param[in] lookup The lookup table
     * @param[in] havePermissions The flag indicating whether to respect edge permissions
     * @param[in] haveRestrictions The flag indicating whether to respect edge restrictions
     */
    Node2EdgeRouter(const std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo>& edgeInfos, bool unbuildIsWarning, typename SUMOAbstractRouter<E, V>::Operation operation, const std::shared_ptr<const LookupTable> lookup = nullptr,
                const bool havePermissions = false, const bool haveRestrictions = false) :
        AStarRouter<E, V>(edgeInfos, unbuildIsWarning, operation, lookup, havePermissions, haveRestrictions) {
    }

    /// @brief Destructor
    virtual ~Node2EdgeRouter() {
        WRITE_MESSAGE("The following stats for AStarRouter (which might also be empty) belong to Node2EdgeRouter, a derived class:");
    }

    /// @brief Cloning method
    virtual SUMOAbstractRouter<E, V>* clone() {
        return new Node2EdgeRouter<E, N, V>(this->myEdgeInfos, this->myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation, this->myLookupTable,
                                     this->myHavePermissions, this->myHaveRestrictions);
    }

    /** @brief Reset method
     * @param[in] vehicle The vehicle
     */
    virtual void reset(const V* const vehicle) {
        UNUSED_PARAMETER(vehicle);
        for (auto& edgeInfo : this->myFrontierList) {
            edgeInfo->reset();
        }
        this->myFrontierList.clear();
        for (auto& edgeInfo : this->myFound) {
            edgeInfo->reset();
        }
        this->myFound.clear();
    }

    /** @brief Builds the routes between the given node and all given edges using the minimum travel time
     * @param[in] fromNode The node which the routes start with
     * @param[in] toEdges The edges at which the routes end
     * @param[in] vehicle The vehicle
     * @param[in] msTime The start time of the routes in milliseconds
     * @param[in] silent The boolean flag indicating whether the method stays silent or puts out messages
     */
    bool computeNode2Edges(const N* fromNode, const std::unordered_set<const E*>& toEdges, const V* const vehicle,
                 SUMOTime msTime, bool silent = false) {
        assert(fromNode != nullptr);
        // check whether fromNode and to can be used
        bool found = false;
        bool allVisited = true; // we try to disprove this 
        for (const E* from : fromNode->getOutgoing()) {
            if (from->isInternal()) {
                continue;
            }
            if (this->myEdgeInfos[from->getNumericalID()].prohibited || this->isProhibited(from, vehicle)) {
                if (!silent) {
                    this->myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on source edge '" + from->getID() + "'.");
                }
            }
            else {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
        const std::vector<const E*>& fromEdges = fromNode->getOutgoing();

        if (fromEdges.empty() || toEdges.empty()) { // nothing to do here
            return false;
        }

        double length = 0.; // dummy for the via edge cost update
        this->startQuery();
#ifdef ASTAR_DEBUG_QUERY
        if (ASTAR_DEBUG_COND) {
            std::cout << "DEBUG: starting search for '" << Named::getIDSecure(vehicle) << "' speed: " << MIN2(vehicle->getMaxSpeed(), myMaxSpeed * vehicle->getChosenSpeedFactor()) << " time: " << STEPS2TIME(msTime) << "\n";
        }
#endif

        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        init(fromEdges, vehicle, msTime);
        this->myAmClean = false;
        // loop
        int num_visited = 0;
        while (!this->myFrontierList.empty()) {
            num_visited += 1;
            // use the edge with the minimal length
            auto* const minimumInfo = this->myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            // check whether the destination edge was already reached
            if (toEdges.count(minEdge)) {
                for (const E* to : toEdges) {
                    const auto& toInfo = this->myEdgeInfos[to->getNumericalID()];
                    if (!toInfo.visited) {
                        allVisited = false;
                        break;
                    }
                }
                if (allVisited) {
                    this->endQuery(num_visited);
                    return true;
                }
            }
            std::pop_heap(this->myFrontierList.begin(), this->myFrontierList.end(), this->myComparator);
            this->myFrontierList.pop_back();
            this->myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
            const double effortDelta = this->getEffort(minEdge, vehicle, minimumInfo->leaveTime);
            const double leaveTime = minimumInfo->leaveTime + this->getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);

            const double heuristic_remaining = 0;
            if (heuristic_remaining == UNREACHABLE) {
                continue;
            }
            const double heuristicEffort = minimumInfo->effort + effortDelta + heuristic_remaining;
            // check all ways from the edge with the minimal length
            for (const std::pair<const E*, const E*>& follower : minEdge->getViaSuccessors(vClass)) {
                auto& followerInfo = this->myEdgeInfos[follower.first->getNumericalID()];
                // check whether it can be used
                if (followerInfo.prohibited || this->isProhibited(follower.first, vehicle)) {
                    continue;
                }
                double effort = minimumInfo->effort + effortDelta;
                double time = leaveTime;
                this->updateViaEdgeCost(follower.second, vehicle, time, effort, length);
                const double oldEffort = followerInfo.effort;
                if ((!followerInfo.visited) && effort < oldEffort) {
                    followerInfo.effort = effort;
                    // if we use the effort including the via effort below we would count the via twice as shown by the ticket676 test
                    // but we should never get below the real effort, see #12463
                    followerInfo.heuristicEffort = MAX2(MIN2(heuristicEffort, followerInfo.heuristicEffort), effort);
                    followerInfo.leaveTime = time;
                    followerInfo.prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<double>::max()) {
                        this->myFrontierList.push_back(&followerInfo);
                        std::push_heap(this->myFrontierList.begin(), this->myFrontierList.end(), this->myComparator);
                    } else {
                        auto fi = std::find(this->myFrontierList.begin(), this->myFrontierList.end(), &followerInfo);
                        assert(fi != this->myFrontierList.end());
                        std::push_heap(this->myFrontierList.begin(), fi + 1, this->myComparator);
                    }
                }
            }
        }
        this->endQuery(num_visited);
        for (const E* to : toEdges) {
            const auto& toInfo = this->myEdgeInfos[to->getNumericalID()];
            if (toInfo.visited) {
                if (!silent) {
                    this->myErrorMsgHandler->informf("Only some connections between node '%' and the given edges were found.", fromNode->getID());
                }
                return true; // at least one connection could be found
            }
        }
        if (!silent) {
            this->myErrorMsgHandler->informf("No connections between node '%' and the given edges were found.", fromNode->getID());
        }
        return false;
    }

    /** @brief Builds the route between the given node and edge using the minimum travel time
     * @param[in] fromNode The node the route starts with
     * @param[in] to The edge at which the route ends
     * @param[in] vehicle The vehicle
     * @param[in] msTime The start time of the route in milliseconds
     * @param[out] into The vector of edges, into which the solution route is written
     * @param[in] silent The boolean flag indicating whether the method stays silent or puts out messages
     */
    bool computeNode2Edge(const N* fromNode, const E* to, const V* const vehicle,
        SUMOTime msTime, std::vector<const E*>& into, bool silent = false) {
        assert(fromNode != nullptr && to != nullptr);
        // check whether fromNode and to can be used
        bool found = false;
        for (const E* from : fromNode->getOutgoing()) {
            if (from->isInternal()) {
                continue;
            }
            if (this->myEdgeInfos[from->getNumericalID()].prohibited || this->isProhibited(from, vehicle)) {
                if (!silent) {
                    this->myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on source edge '" + from->getID() + "'.");
                }
            }
            else {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
        const std::vector<const E*>& fromEdges = fromNode->getOutgoing();

        if (fromEdges.empty()) { // nothing to do here
            return false;
        }
        if (this->myEdgeInfos[to->getNumericalID()].prohibited || this->isProhibited(to, vehicle)) {
            if (!silent) {
                this->myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on destination edge '" + to->getID() + "'.");
            }
            return false;
        }
        double length = 0.; // dummy for the via edge cost update
        this->startQuery();

        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        init(fromEdges, vehicle, msTime);
        this->myAmClean = false;
        // loop
        int num_visited = 0;
        const bool mayRevisit = this->myLookupTable != nullptr && !this->myLookupTable->consistent();
        const double speed = vehicle == nullptr ? this->myMaxSpeed : MIN2(vehicle->getMaxSpeed(), this->myMaxSpeed * vehicle->getChosenSpeedFactor());
        while (!this->myFrontierList.empty()) {
            num_visited += 1;
            // use the edge with the minimal length
            auto* const minimumInfo = this->myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            // check whether the destination edge was already reached
            if (minEdge == to) {
                this->buildPathFrom(minimumInfo, into);
                this->endQuery(num_visited);
                return true;
            }
            std::pop_heap(this->myFrontierList.begin(), this->myFrontierList.end(), this->myComparator);
            this->myFrontierList.pop_back();
            this->myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
            const double effortDelta = this->getEffort(minEdge, vehicle, minimumInfo->leaveTime);
            const double leaveTime = minimumInfo->leaveTime + this->getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);

            // admissible A* heuristic: straight line distance at maximum speed
            // this is calculated from the end of minEdge so it possibly includes via efforts to the followers
            const double heuristic_remaining = (this->myLookupTable == nullptr ? minEdge->getDistanceTo(to) / speed :
                this->myLookupTable->lowerBound(minEdge, to, speed, vehicle->getChosenSpeedFactor(),
                    minEdge->getMinimumTravelTime(nullptr), to->getMinimumTravelTime(nullptr)));
            //const double heuristic_remaining = 0.;
            if (heuristic_remaining == UNREACHABLE) {
                continue;
            }
            const double heuristicEffort = minimumInfo->effort + effortDelta + heuristic_remaining;
            // check all ways from the edge with the minimal length
            for (const std::pair<const E*, const E*>& follower : minEdge->getViaSuccessors(vClass)) {
                auto& followerInfo = this->myEdgeInfos[follower.first->getNumericalID()];
                // check whether it can be used
                if (followerInfo.prohibited || this->isProhibited(follower.first, vehicle)) {
                    continue;
                }
                double effort = minimumInfo->effort + effortDelta;
                double time = leaveTime;
                this->updateViaEdgeCost(follower.second, vehicle, time, effort, length);
                const double oldEffort = followerInfo.effort;
                if ((!followerInfo.visited || mayRevisit) && effort < oldEffort) {
                    followerInfo.effort = effort;
                    // if we use the effort including the via effort below we would count the via twice as shown by the ticket676 test
                    // but we should never get below the real effort, see #12463
                    followerInfo.heuristicEffort = MAX2(MIN2(heuristicEffort, followerInfo.heuristicEffort), effort);
                    followerInfo.leaveTime = time;
                    followerInfo.prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<double>::max()) {
                        this->myFrontierList.push_back(&followerInfo);
                        std::push_heap(this->myFrontierList.begin(), this->myFrontierList.end(), this->myComparator);
                    }
                    else {
                        auto fi = std::find(this->myFrontierList.begin(), this->myFrontierList.end(), &followerInfo);
                        if (fi == this->myFrontierList.end()) {
                            assert(mayRevisit);
                            this->myFrontierList.push_back(&followerInfo);
                            std::push_heap(this->myFrontierList.begin(), this->myFrontierList.end(), this->myComparator);
                        }
                        else {
                            std::push_heap(this->myFrontierList.begin(), fi + 1, this->myComparator);
                        }
                    }
                }
            }
        }
        this->endQuery(num_visited);
        if (!silent) {
            this->myErrorMsgHandler->informf("No connection between node '%' and edge '%' found.", fromNode->getID(), to->getID());
        }
        return false;
    }

    /** @brief Updates the via cost up to a given edge
     * @param[in] prev The previous edge
     * @param[in] e The given edge
     * @param[in] v The vehicle
     * @param[in,out] time The passed and updated start time in seconds
     * @param[in,out] effort The passed and updated effort
     * @param[in,out] length The passed and updated length
     */
    void updateViaCostUpToEdge(const E* const prev, const E* const e, const V* const v, double& time, double& effort, double& length) const;

    /** @brief Returns the recomputed cost for traversing the given edges, excluding the last one
     * @param[in] edges The vector of edges
     * @param[in] v The vehicle
     * @param[in] msTime The start time in milliseconds
     * @param[in,out] lengthp The passed and updated length
     */
    double recomputeCostsNoLastEdge(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime, double* lengthp = nullptr) const {
        const E* lastEdge = edges.back();
        double time = STEPS2TIME(msTime);
        double effort = 0.;
        double length = 0.;
        if (lengthp == nullptr) {
            lengthp = &length;
        }
        else {
            *lengthp = 0.;
        }
        const E* prev = nullptr;
        for (const E* const e : edges) {
            if (e == lastEdge) {
                break;
            }
            if (isProhibited(e, v)) {
                return -1;
            }
            updateViaCost(prev, e, v, time, effort, *lengthp);
            prev = e;
        }
        updateViaCostUpToEdge(prev, lastEdge, v, time, effort, *lengthp);
        return effort;
    }

    /// @brief Bulk mode is not supported
    virtual void setBulkMode(const bool mode) {
        UNUSED_PARAMETER(mode);
        throw std::runtime_error("Bulk mode is not supported by the node-to-edge router.");
    }

protected:
    /** @brief Initialize the node-to-edge router
     * @param[in] fromEdges The vector of start edges
     * @param[in] vehicle The vehicle
     * @param[in] msTime The start time in milliseconds
     */
    void init(std::vector<const E*> fromEdges, const V* const vehicle, const SUMOTime msTime);
};

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
void Node2EdgeRouter<E, N, V>::updateViaCostUpToEdge(const E* const prev, const E* const e, const V* const v, double& time, double& effort, double& length) const {
    assert(prev && e);
    for (const std::pair<const E*, const E*>& follower : prev->getViaSuccessors()) {
        if (follower.first == e) {
            updateViaEdgeCost(follower.second, v, time, effort, length);
            break;
        }
    }
}

template<class E, class N, class V>
void Node2EdgeRouter<E, N, V>::init(std::vector<const E*> fromEdges, const V* const vehicle, const SUMOTime msTime) {
    // all EdgeInfos touched in the previous query are either in myFrontierList or myFound: clean those up
    for (auto& edgeInfo : this->myFrontierList) {
        edgeInfo->reset();
    }
    this->myFrontierList.clear();
    for (auto& edgeInfo : this->myFound) {
        edgeInfo->reset();
    }
    this->myFound.clear();
    for (const E* from : fromEdges) {
        if (from->isInternal()) {
            continue;
        }
        int edgeID = from->getNumericalID();
        auto& fromInfo = this->myEdgeInfos[edgeID];
        if (fromInfo.prohibited || this->isProhibited(from, vehicle)) {
            continue;
        }
        fromInfo.effort = 0.;
        fromInfo.heuristicEffort = 0.;
        fromInfo.prev = nullptr;
        fromInfo.leaveTime = STEPS2TIME(msTime);
        this->myFrontierList.push_back(&fromInfo);
    }
}

