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
/// @file    AFCentralizedSPTree.h
/// @author  Ruediger Ebendt
/// @date    01.12.2023
///
// Class for a label-correcting algorithm calculating multiple shortest path 
// trees at once (centralized shortest path tree, cf. Hilger et al.) 
// Used for setting the arc flags for the arc flag router
// @note Intended use is on a backward graph with flipped edges
/****************************************************************************/
#pragma once
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
#include <stdexcept>
#include <cstddef>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SUMOTime.h>
#include "SUMOAbstractRouter.h"
#include "KDTreePartition.h"
#include "AFInfo.h"

//#define CSPT_WRITE_QGIS_FILTERS
//#define CSPT_DEBUG_LEVEL_0

template<class E, class N, class V>
class AFCentralizedSPTree {
public:
    typedef typename KDTreePartition<E, N, V>::Cell Cell;
    typedef typename AFInfo<E>::ArcInfo ArcInfo;
 
    class EdgeInfoComparator {
    public:
        /** @brief Constructor
         * @param[in] arcInfos The arc informations
         */ 
        EdgeInfoComparator(std::vector<ArcInfo*>& arcInfos) : myArcInfos(arcInfos)
        {
        }

        /** @brief Comparing method
         * @param[in] edgeInfo1 The first edge information
         * @param[in] edgeInfo2 The second edge information
         * @return true iff arc info key of the first edge is greater than that of the second
         * @note In case of ties: returns true iff the numerical id of the first edge is greater that that of the second
         */ 
        bool operator()(const typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo1, 
            const typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo2) const {
            int index1 = edgeInfo1->edge->getNumericalID();
            int index2 = edgeInfo2->edge->getNumericalID();
            ArcInfo* arcInfo1 = myArcInfos[index1];
            ArcInfo* arcInfo2 = myArcInfos[index2];
            double key1 = arcInfo1->key;
            double key2 = arcInfo2->key;

            if (key1 == key2) { // tie
                return index1 > index2;
            }
            return key1 > key2;
        }
    private:
        std::vector<ArcInfo*>& myArcInfos;
    };

    /** @brief Constructor
     * @param[in] edges The edges
     * @param[in] arcInfos The arc informations (for arc flag routing)
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] effortProvider The effort provider
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered or not
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered or not
     */ 
    AFCentralizedSPTree(const std::vector<E*>& edges, std::vector<ArcInfo*>& arcInfos, bool unbuildIsWarning,
        SUMOAbstractRouter<E, V>* effortProvider, 
        const bool havePermissions = false, const bool haveRestrictions = false) :
        myArcInfos(arcInfos),
        myHavePermissions(havePermissions),
        myHaveRestrictions(haveRestrictions),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myEffortProvider(effortProvider), 
        myMaxSpeed(NUMERICAL_EPS) {
        for (const E* const edge : edges) {
            myEdgeInfos.push_back(typename SUMOAbstractRouter<E, V>::EdgeInfo(edge));
            myMaxSpeed = MAX2(myMaxSpeed, edge->getSpeedLimit() * MAX2(1.0, edge->getLengthGeometryFactor()));
        }
        myComparator = new EdgeInfoComparator(myArcInfos);
    }
 
    /// @brief Destructor
    ~AFCentralizedSPTree() {
        delete myComparator;
    }
    
    /** @brief Returns true iff driving the given vehicle on the given edge is prohibited 
     * @param[in] edge The edge
     * @param[in] vehicle The vehicle
     * @return true iff driving the given vehicle on the given edge is prohibited
     */ 
    bool isProhibited(const E* const edge, const V* const vehicle) const {
        return (myHavePermissions && edge->prohibits(vehicle)) || (myHaveRestrictions && edge->restricts(vehicle));
    }

    /** @brief Computes a shortest path tree for each boundary edge of the given cell, returns true iff this was successful
     * @note This is done for all such shortest path trees at once (i.e., a centralized shortest path tree is computed, see Hilger et al.)
     * @param[in] msTime The start time of the paths/routes in milliseconds  
     * @param[in] cell The cell as a part of a k-d tree partition of the network
     * @param[in] vehicle The vehicle
     * @param[in] incomingEdgesOfOutgoingBoundaryEdges Maps each outgoing boundary edge to its incoming edges
     * @param[in] silent The boolean flag indicating whether the method stays silent or puts out messages
     * @return true iff the centralized shortest path tree could successfully be calculated
     */ 
    bool computeCentralizedSPTree(SUMOTime msTime, const Cell* cell, const V* const vehicle, 
        const std::map<const E*, std::vector<const E*>>& incomingEdgesOfOutgoingBoundaryEdges,
        bool silent = false) {
        assert(cell != nullptr);
        const std::unordered_set<const E*>& fromEdges = cell->getOutgoingBoundaryEdges();
        if (fromEdges.empty()) { // nothing to do here
            return false;
        }
        double length = 0.; // dummy for the via edge cost update
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        std::vector<const E*> fromEdgesAsVector(fromEdges.begin(), fromEdges.end());
        init(fromEdgesAsVector, vehicle, msTime);
        int num_visited = 0;
        size_t numberOfVisitedFromEdges = 0;
        bool minIsFromEdge = false;
#ifdef CSPT_DEBUG_LEVEL_0
        size_t numberOfTouchedSupercellEdges = 0;
#endif
#ifdef _DEBUG
        const Cell* supercell = cell->getSupercell();
#endif
        const bool mayRevisit = true;

        while (!myFrontierList.empty()) {
            num_visited += 1;
            // use the edge with the minimal length
            typename SUMOAbstractRouter<E, V>::EdgeInfo* minimumInfo = myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            assert(!minEdge->isInternal());
            ArcInfo* minimumArcInfo = myArcInfos[minEdge->getNumericalID()];
            if (minimumInfo->visited || numberOfVisitedFromEdges < fromEdges.size()) {
                minIsFromEdge = incomingEdgesOfOutgoingBoundaryEdges.find(minEdge) != incomingEdgesOfOutgoingBoundaryEdges.end();
            }
            else {
                minIsFromEdge = false;
            }
            if (minIsFromEdge) {
                if (numberOfVisitedFromEdges < fromEdges.size()) {
                    numberOfVisitedFromEdges++;
                }
            }
            assert(incomingEdgesOfOutgoingBoundaryEdges.size() == fromEdges.size());
            assert(minimumArcInfo->key != std::numeric_limits<double>::max() && minimumArcInfo->key != UNREACHABLE);
            size_t index;
#ifdef CSPT_DEBUG_LEVEL_0
            if (supercell->contains(minEdge->getToJunction())) {
                // minEdge is a supercell edge (we check for the to-node since we work on a backward graph)
                if (!minimumArcInfo->touched) {
                    numberOfTouchedSupercellEdges++;
                    minimumArcInfo->touched = true;
                }
            }
#endif
#ifdef CSPT_DEBUG_LEVEL_0
            if (num_visited % 500 == 0) {
                std::cout << "num_visited: " << num_visited << ", numberOfTouchedSupercellEdges: " << numberOfTouchedSupercellEdges
                    << ", minimumArcInfo->key: " << minimumArcInfo->key << std::endl;
            }
#endif
            std::pop_heap(myFrontierList.begin(), myFrontierList.end(), *myComparator);
            myFrontierList.pop_back();
            myFound.push_back(minimumInfo);
            minimumInfo->visited = true;
            const double effortDelta = myEffortProvider->getEffort(minEdge, vehicle, minimumInfo->leaveTime);
            const double leaveTime = minimumInfo->leaveTime + myEffortProvider->getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);

            // check all ways from the edge with the minimal length
            for (const std::pair<const E*, const E*>& follower : minEdge->getViaSuccessors(vClass)) {
                assert(!follower.first->isInternal());
                bool wasPushedToHeap = false;
                auto& followerInfo = myEdgeInfos[follower.first->getNumericalID()];
                ArcInfo* followerArcInfo = myArcInfos[follower.first->getNumericalID()];
                // check whether it can be used
                if (followerInfo.prohibited || isProhibited(follower.first, vehicle)) {
                    if (!silent) {
                        myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on source edge '" + followerInfo.edge->getID() + "'.");
                    }
                    continue;
                }

                if (followerArcInfo->effortsToBoundaryNodes.empty()) { // non-initialized non-supercell edge 
                    assert(!supercell->contains(follower.first->getToJunction()));
                    std::fill_n(std::back_inserter(followerArcInfo->effortsToBoundaryNodes),
                        minimumArcInfo->effortsToBoundaryNodes.size(), std::numeric_limits<double>::max());
                }

                double key = std::numeric_limits<double>::max();
                assert(followerArcInfo->effortsToBoundaryNodes.size() == minimumArcInfo->effortsToBoundaryNodes.size());
                bool hasImproved = false;
                // loop over all boundary nodes
                for (index = 0; index < followerArcInfo->effortsToBoundaryNodes.size(); index++) {
                    // is minEdge a from-edge (i.e., an outgoing boundary edge of the passed cell), 
                    // and 'index' not the index of its from-node (i.e., not of the 'own' boundary node)?
                    if (minIsFromEdge && (incomingEdgesOfOutgoingBoundaryEdges.at(minEdge))[index]) {
                        // if yes, assign the successor edges of the incoming edge of minEdge on the shortest route from 
                        // the boundary node with the index 'index' to followersOfIncomingEdge
                        assert((incomingEdgesOfOutgoingBoundaryEdges.at(minEdge)).size() 
                            == followerArcInfo->effortsToBoundaryNodes.size());
                        const std::vector<std::pair<const E*, const E*>>& followersOfIncomingEdge 
                            = ((incomingEdgesOfOutgoingBoundaryEdges.at(minEdge))[index])->getViaSuccessors(vClass);
                        // is the current follower among said successor edges?
                        bool turningAllowed = false;
                        for (std::pair<const E*, const E*> followerOfIncomingEdge : followersOfIncomingEdge) {
                            if (follower.first == followerOfIncomingEdge.first) {
                                turningAllowed = true;
                                break;
                            }                          
                        }
                        // if not, then turning from said incoming edge to the current follower is not allowed
                        // and we mustn't propagate the distance to said boundary node
                        // instead simply skip this follower
                        if (!turningAllowed) {
                            continue;
                        }
                    }
                    // propagate distances to other boundary nodes
                    double effortToFollower = minimumArcInfo->effortsToBoundaryNodes[index] == UNREACHABLE ?
                        UNREACHABLE : minimumArcInfo->effortsToBoundaryNodes[index] + effortDelta;
                    if (effortToFollower == UNREACHABLE) {
                        continue; // no need to consider this follower 
                    }
                    double time = leaveTime;
                    myEffortProvider->updateViaEdgeCost(follower.second, vehicle, time, effortToFollower, length);
                    if (effortToFollower < key) {
                        key = effortToFollower;
                    }
                    const double oldEffort = followerArcInfo->effortsToBoundaryNodes[index];
                    if (oldEffort != std::numeric_limits<double>::max()) {
                        wasPushedToHeap = true; // must have been pushed to heap during an earlier visit
                    }

                    if ((!followerInfo.visited || mayRevisit)
                        && effortToFollower < oldEffort) {
                        hasImproved = true;
                        followerArcInfo->effortsToBoundaryNodes[index] = effortToFollower;
                    }
                } // end index loop

                if (!hasImproved) {
                    continue; // no need to re-enque this follower, continue w/ next one
                }
                followerArcInfo->key = key;
                if (!wasPushedToHeap) {
                    myFrontierList.push_back(&followerInfo);
                    std::push_heap(myFrontierList.begin(), myFrontierList.end(), *myComparator);
                }
                else {
                    auto fi = std::find(myFrontierList.begin(), myFrontierList.end(), &followerInfo);
                    if (fi == myFrontierList.end()) { // has already been expanded, reinsert into frontier
                        assert(mayRevisit);
                        myFrontierList.push_back(&followerInfo);
                        std::push_heap(myFrontierList.begin(), myFrontierList.end(), *myComparator);
                    }
                    else {
                        std::push_heap(myFrontierList.begin(), fi + 1, *myComparator);
                    }
                }
            } // end follower loop
        } // end while (!myFrontierList.empty())
  
#ifdef CSPT_DEBUG_LEVEL_0
        std::cout << "centralizedSPTree finished (queue empty)." << std::endl;
#endif
        return true;
    }

protected:
    /** @brief Initialize the arc flag router
     * @param[in] fromEdges The container of from-/head/source edges
     * @param[in] vehicle The vehicle
     * @param[in] msTime The start time of the paths/routes in milliseconds 
     */   
    void init(std::vector<const E*> fromEdges, const V* const vehicle, SUMOTime msTime);
    /// @brief The min edge heap
    /// @note A container for reusage of the min edge heap
    std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo*> myFrontierList;
    /// @brief The list of visited edges (for resetting)
    std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo*> myFound;
    /// The container of edge information
    std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo> myEdgeInfos;
    /// @brief The edge informations specific to arc flag routing 
    /// @note As opposed to the standard informations in SUMOAbstractRouter<E, V>::EdgeInfo
    std::vector<ArcInfo*>& myArcInfos;
    /// @brief The boolean flag indicating whether edge permissions need to be considered or not
    const bool myHavePermissions;
    /// @brief The boolean flag indicating whether edge restrictions need to be considered or not
    const bool myHaveRestrictions;
    /// @brief The handler for routing errors
    MsgHandler* const myErrorMsgHandler;
    /// @brief The object's operation to perform
    SUMOAbstractRouter<E, V>* myEffortProvider;
    /// @brief The comparator
    EdgeInfoComparator* myComparator;
    /// @brief The maximum speed in the network
    double myMaxSpeed;
};

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
void AFCentralizedSPTree<E, N, V>::init(std::vector<const E*> fromEdges, const V* const vehicle, SUMOTime msTime) {
    // all EdgeInfos touched in the previous query are either in myFrontierList or myFound: clean those up
    for (auto& edgeInfo : myFrontierList) {
        edgeInfo->reset();
    }
    myFrontierList.clear();
    for (auto& edgeInfo : myFound) {
        edgeInfo->reset();
    }
    for (auto& arcInfo : myArcInfos) {
        arcInfo->reset(); // does not reset effortsToBoundaryNodes
    }
    myFound.clear();
    for (const E* from : fromEdges) {
        if (from->isInternal()) {
            continue;
        }
        int edgeID = from->getNumericalID();
        auto& fromInfo = myEdgeInfos[edgeID];
        if (fromInfo.prohibited || isProhibited(from, vehicle)) {
            continue;
        }
        fromInfo.heuristicEffort = 0.;
        fromInfo.prev = nullptr;
        fromInfo.leaveTime = STEPS2TIME(msTime);
        myFrontierList.push_back(&fromInfo);
        ArcInfo* fromArcInfo = myArcInfos[edgeID];
        fromArcInfo->key = 0;
    }
}
