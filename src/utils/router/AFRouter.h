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
/// @file    AFRouter.h
/// @author  Ruediger Ebendt
/// @date    01.12.2023
///
// Realizes an arc flag routing algorithm (Hilger et al.) in its multi-level variant 
// (also called "stripped SHARC" by Delling et al.)
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
#include "AStarLookupTable.h"
#include "SUMOAbstractRouter.h"
#include "KDTreePartition.h"
#include "FlippedEdge.h"
#include "AFInfo.h"
#include "AFBuilder.h"

#define UNREACHABLE (std::numeric_limits<double>::max() / 1000.0)
#define AFRO_WRITE_QGIS_FILTERS

//#define AFRO_DEBUG_LEVEL_0
//#define AFRO_DEBUG_LEVEL_1
//#define AFRO_DEBUG_LEVEL_2
//#define AFRO_DEBUG_LEVEL_3

#ifdef AFRO_DEBUG_LEVEL_3
#define AFRO_DEBUG_LEVEL_2
#endif

#ifdef AFRO_DEBUG_LEVEL_2
#define AFRO_DEBUG_LEVEL_1
#endif

#ifdef AFRO_DEBUG_LEVEL_1
#define AFRO_DEBUG_LEVEL_0
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AFRouter
 * Computes the shortest path through a network with an arc flag routing 
 * algorithm (Hilger et al.) in its multi-level variant (also called 
 * "stripped SHARC" by Delling et al.)
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param N The node class to use (MSJunction/RONode)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 *
 * The router is edge-based 
 * It must know the number of edges for internal reasons
 * and whether a missing connection between two given edges (unbuild route) shall
 * be reported as an error or as a warning
 *
 */
template<class E, class N, class V>
class AFRouter : public SUMOAbstractRouter<E, V> {
public:
    typedef AbstractLookupTable<E, V> LookupTable;
    typedef typename KDTreePartition<E, N, V>::Cell Cell;
    typedef typename AFInfo<E>::FlagInfo FlagInfo;
    typedef AbstractLookupTable<FlippedEdge<E, N, V>, V> FlippedLookupTable;
    
    /** @brief Returns the edge information for the passed edge
     * @param[in] edge The edge
     * @note Non-const version
     * @return The edge information
     */
    typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo(const E* const edge) {
        return &(this->myEdgeInfos[edge->getNumericalID()]);
    }

    /** @brief Returns the edge information for the passed edge
     * @param[in] edge The edge
     * @note Const version
     * @return The edge information
     */
    const typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo(const E* const edge) const {
        return &(this->myEdgeInfos[edge->getNumericalID()]);
    }

    /**
     * @class EdgeInfoComparator
     * 
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoComparator {
    public:
        /** @brief Comparing method
         * @param[in] edgeInfo1 First edge information
         * @param[in] edgeInfo2 Second edge information
         * @return true iff heuristic effort of first edge information is greater than that of the second
         * @note In case of ties: true iff first edge information's numerical id is greater than that of the second
         */ 
        
        bool operator()(const typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo1, const typename SUMOAbstractRouter<E, V>::EdgeInfo* edgeInfo2) const {
            if (edgeInfo1->heuristicEffort == edgeInfo2->heuristicEffort) {
                return edgeInfo1->edge->getNumericalID() > edgeInfo2->edge->getNumericalID();
            }
            return edgeInfo1->heuristicEffort > edgeInfo2->heuristicEffort;
        }
    };

    /** @brief Constructor
     * @param[in] edges The edges
     * @param[in] partition A partition of the router's network wrt a k-d tree subdivision scheme
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] operation The operation for a forward graph
     * @param[in] flippedOperation The operation for a backward graph with flipped edges
     * @param[in] weightPeriod The validity duration of one weight interval
     * @param[in] lookup The lookup table for a forward graph
     * @param[in] flippedLookup The lookup table for a backward graph  with flipped edges
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered or not
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered or not
     */ 
    AFRouter(const std::vector<E*>& edges, 
        const KDTreePartition<E, N, V>* partition,
        bool unbuildIsWarning,
        typename SUMOAbstractRouter<E, V>::Operation operation, typename SUMOAbstractRouter<FlippedEdge<E, N, V>, V>::Operation flippedOperation,
        SUMOTime weightPeriod, const std::shared_ptr<const LookupTable> lookup = nullptr,
        const std::shared_ptr<const FlippedLookupTable> flippedLookup = nullptr,
        const bool havePermissions = false, const bool haveRestrictions = false) :
        SUMOAbstractRouter<E, V>("arcFlagRouter", unbuildIsWarning, operation, nullptr, havePermissions, haveRestrictions),
        myFlagInfos(nullptr),
        myPartition(partition),
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS),
        myWeightPeriod(weightPeriod),
        myValidUntil(0),
        myBuilder(new AFBuilder<E, N, V>(myPartition->getNumberOfLevels(), edges, unbuildIsWarning,
            flippedOperation, flippedLookup, havePermissions, haveRestrictions)),
        myType("arcFlagRouter"),
        myQueryVisits(0),
        myNumQueries(0),
        myQueryStartTime(0),
        myQueryTimeSum(0),
#ifdef AFRO_DEBUG_LEVEL_2
        myFlagContextStartTime(0),
        myFlagContextTimeSum(0),
#endif
        myLastSettledEdgeCell(nullptr),
        myTargetEdgeCellLevel0(nullptr)
    {
        for (const E* const edge : edges) {
            this->myEdgeInfos.push_back(typename SUMOAbstractRouter<E, V>::EdgeInfo(edge));
            myMaxSpeed = MAX2(myMaxSpeed, edge->getSpeedLimit() * MAX2(1.0, edge->getLengthGeometryFactor()));
        }
    }

    /** @brief "Normal" cloning constructor for uninitialized or time-dependent instances
     * @param[in] edges The edges
     * @param[in] partition A partition of the router's network wrt a k-d tree subdivision scheme
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] operation The operation for a forward graph
     * @param[in] flippedOperation The operation for a backward graph with flipped edges
     * @param[in] weightPeriod The validity duration of one weight interval
     * @param[in] lookup The lookup table for a forward graph
     * @param[in] flippedLookup The lookup table for a backward graph with flipped edges
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered
     */
    AFRouter(const std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo>& edgeInfos,
        const std::vector<E*>& edges, 
        const KDTreePartition<E, N, V>* partition,
        bool unbuildIsWarning,
        typename SUMOAbstractRouter<E, V>::Operation operation, 
        typename SUMOAbstractRouter<FlippedEdge<E, N, V>, V>::Operation flippedOperation,
        SUMOTime weightPeriod, const std::shared_ptr<const LookupTable> lookup = nullptr,
        const std::shared_ptr<const FlippedLookupTable> flippedLookup = nullptr,
        const bool havePermissions = false, const bool haveRestrictions = false) :
        SUMOAbstractRouter<E, V>("arcFlagRouter", unbuildIsWarning, operation, nullptr, havePermissions, haveRestrictions),
        myFlagInfos(nullptr),
        myPartition(partition),
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS),
        myWeightPeriod(weightPeriod),
        myValidUntil(0),
        myBuilder(new AFBuilder<E, N, V>(myPartition->getNumberOfLevels(), edges, unbuildIsWarning,
            flippedOperation, flippedLookup, havePermissions, haveRestrictions)), 
        myType("arcFlagRouter"),
        myQueryVisits(0),
        myNumQueries(0),
        myQueryStartTime(0),
        myQueryTimeSum(0),
#ifdef AFRO_DEBUG_LEVEL_2
        myFlagContextStartTime(0),
        myFlagContextTimeSum(0),
#endif
        myLastSettledEdgeCell(nullptr),
        myTargetEdgeCellLevel0(nullptr)
    {
        for (const auto& edgeInfo : edgeInfos) {
            this->myEdgeInfos.push_back(typename SUMOAbstractRouter<E, V>::EdgeInfo(edgeInfo.edge));
            myMaxSpeed = MAX2(myMaxSpeed, edgeInfo.edge->getSpeedLimit() * edgeInfo.edge->getLengthGeometryFactor());
        }
    }

    /** @brief Special cloning constructor, only for time-independent instances which never rebuild arc infos
     * @param[in] edgeInfos The vector of edge information
     * @param[in] partition A partition of the router's network wrt a k-d tree subdivision scheme
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] operation The operation for a forward graph
     * @param[in] flagInfos The vector of arc flag information
     * @param[in] lookup The lookup table for a forward graph
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered
     */
   AFRouter(const std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo>& edgeInfos, 
        const KDTreePartition<E, N, V>* partition,
        bool unbuildIsWarning, typename SUMOAbstractRouter<E, V>::Operation operation, 
        std::vector<FlagInfo*>* flagInfos, 
        const std::shared_ptr<const LookupTable> lookup = nullptr,
        const bool havePermissions = false, const bool haveRestrictions = false) :
        SUMOAbstractRouter<E, V>("arcFlagRouterClone", unbuildIsWarning, operation, nullptr, havePermissions, haveRestrictions),
        myFlagInfos(flagInfos),
        myPartition(partition),
        myLookupTable(lookup),
        myMaxSpeed(NUMERICAL_EPS),
        myWeightPeriod(SUMOTime_MAX),
        myValidUntil(SUMOTime_MAX),
        myBuilder(nullptr), 
        myType("arcFlagRouterClone"),
        myQueryVisits(0),
        myNumQueries(0),
        myQueryStartTime(0),
        myQueryTimeSum(0),
#ifdef AFRO_DEBUG_LEVEL_2
        myFlagContextStartTime(0),
        myFlagContextTimeSum(0),
#endif
        myLastSettledEdgeCell(nullptr),
        myTargetEdgeCellLevel0(nullptr)
    {
        for (const auto& edgeInfo : edgeInfos) {
            this->myEdgeInfos.push_back(typename SUMOAbstractRouter<E, V>::EdgeInfo(edgeInfo.edge));
            myMaxSpeed = MAX2(myMaxSpeed, edgeInfo.edge->getSpeedLimit() * edgeInfo.edge->getLengthGeometryFactor());
        }
    }

    /// @brief Destructor
    virtual ~AFRouter() {
        delete myBuilder;
    }

    /// @brief Cloning method
    virtual SUMOAbstractRouter<E, V>* clone() {
        // I am either a clone myself, or I am already initialized and time-independent 
        // (i.e., I have been created with a maximum weight period)
        if (myWeightPeriod == SUMOTime_MAX && myFlagInfos != nullptr) { 
            // we only need the arc infos once:
            return new AFRouter(this->myEdgeInfos, myPartition, this->myErrorMsgHandler == MsgHandler::getWarningInstance(), 
                this->myOperation, myFlagInfos, myLookupTable, this->myHavePermissions, this->myHaveRestrictions);
        }
        // I am not a clone: I am either uninitialized, or initialized but time-dependent: 
        // create another such guy (also flagged as a non-clone)
        return new AFRouter(this->myEdgeInfos, myBuilder->getEdges(), myPartition, 
            this->myErrorMsgHandler == MsgHandler::getWarningInstance(),
            this->myOperation, myBuilder->getArcFlagBuild()->getFlippedOperation(),
            myWeightPeriod, myLookupTable, myBuilder->getArcFlagBuild()->getFlippedLookup(), 
            this->myHavePermissions, this->myHaveRestrictions);
    }

    /** @brief Converts a partition level number to a SHARC level number
     * @param[in] partitionLevel The partition level
     * @param[in] numberOfPartitionLevels The number of partition levels
     * @return The SHARC level number
     */ 
    static int partitionLevel2SHARCLevel(int partitionLevel, int numberOfPartitionLevels)
    {
        // heads up: partition levels must start at zero, with zero being an illegal argument 
        // (since it would corresponds to level L = V, which is not a valid SHARC level)
        if (partitionLevel <= 0) {
            throw std::invalid_argument("partitionLevel2SHARCLevel: given partition level is zero (0) or below. This does not correspond to a valid SHARC level. Partition levels valid for conversion to SHARC levels go from one to number of partition levels minus one.");
        }
        // heads up: partition levels must start at zero
        if (partitionLevel > numberOfPartitionLevels - 1) {
            throw std::invalid_argument("partitionLevel2SHARCLevel: given partition level exceeds the number of partition levels minus one. Most likely you did not start the partition level numbering at zero (0), which is required here.");
        }
        return (numberOfPartitionLevels - 1) - partitionLevel;
    }

    /** @brief Converts a SHARC level number to a partition level number
     * @param[in] sHARCLevel The SHARC level
     * @param[in] numberOfPartitionLevels The number of partition levels
     * @return The partition level number
     */
    static int sHARCLevel2PartitionLevel(int sHARCLevel, int numberOfPartitionLevels)
    {
        int numberOfSHARCLevels = numberOfPartitionLevels - 1;
        if (sHARCLevel < 0) {
            throw std::invalid_argument("sHARCLevel2PartitionLevel: given SHARC level is negative.");
        }
        // heads up: SHARC levels must start at zero (0), 
        // and end at number of partition levels minus two
        if (sHARCLevel > numberOfSHARCLevels - 1) {
            throw std::invalid_argument("sHARCLevel2PartitionLevel: given SHARC level exceeds the number of SHARC levels minus one. Most likely you did not start the SHARC level numbering at zero (0), which is required here.");
        }
        return numberOfSHARCLevels - sHARCLevel;
    }

    /** @brief Returns the arc flag of the edge in flagInfo wrt flagContext
     * @param[in] flagInfo The arc flag information
     * @param[in] flagContext The flag context tuple
     * @return The flag indicating whether the arc flag is set or not, wrt given arc flag information and context
     */
    static bool flag(const FlagInfo* flagInfo, const std::tuple<int, int, bool> flagContext)
    {
            assert(flagInfo);
            return flagInfo->arcFlags.empty() ? true : /* play it safe */
                (flagInfo->arcFlags)[std::get<0>(flagContext) /* assumed to be the SHARC level */ * 2
                + std::get<1>(flagContext) /* assumed to be the cell index */];
            
    }

    /** @brief Returns the arc flags of the passed edge
     * @param[in] edge The edge
     * @return The arc flags of the given edge
     */ 
    std::vector<bool>& flags(const E* edge);
    
    /** @brief Trigger arc flags rebuild
     * @param[in] The vehicle
     */  
    virtual void reset(const V* const vehicle) {
        if (myValidUntil == 0) {
            myValidUntil = myWeightPeriod;
        }
        assert(myBuilder);
#ifdef AFRO_DEBUG_LEVEL_0
        long long int firstCallStart = 0;
        long long int firstCallTime = 0;
        firstCallStart = SysUtils::getCurrentMillis();
        std::cout << "Calling arc flag router for the first time during current weight period (arc flags build). This might take a while... " << std::endl;
#endif
        myFlagInfos = &(myBuilder->build(myValidUntil - myWeightPeriod, vehicle));
#ifdef AFRO_DEBUG_LEVEL_0
        firstCallTime = (SysUtils::getCurrentMillis() - firstCallStart);
        std::cout << "Time spent for arc flags build: " << elapsedMs2string(firstCallTime) << std::endl;
#endif
     }

    /** @brief Initialize the arc flag router
     * param[in] edgeID The edge id(entifier)
     * param[in] msTime The start time of the routes in milliseconds
     */   
    void init(const int edgeID, const SUMOTime msTime);
    /** @brief Returns the flag context for a route query from given settled edge to the target edge
     * @param[in] settledEdge The settled edge
     * @param[in] targetEdge The target edge
     */
    std::tuple<int, int, bool> flagContext(const E* settledEdge, const E* targetEdge);
    /// @brief Kept for runtime comparisons
    std::tuple<int, int, bool> flagContextNaive(const E* settledEdge, const E* targetEdge);

    /** @brief Builds the route between the given edges using the minimum travel time
     * param[in] from The from-/start/source/head edge
     * param[in] to The to-/end/target/tail edge
     * param[in] vehicle The vehicle
     * param[in] msTime The start time of the routes in milliseconds
     * param[out] into The vector of edges, into which the solution route is written
     * @param[in] silent The boolean flag indicating whether the method stays silent or puts out messages
     */
    bool compute(const E* from, const E* to, const V* const vehicle,
        SUMOTime msTime, std::vector<const E*>& into, bool silent = false) {
        assert(from != nullptr && to != nullptr);
        // check whether from and to can be used
        if (this->myEdgeInfos[from->getNumericalID()].prohibited || this->isProhibited(from, vehicle)) {
            if (!silent) {
                this->myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on source edge '" + from->getID() + "'.");
            }
            return false;
        }
        if (this->myEdgeInfos[to->getNumericalID()].prohibited || this->isProhibited(to, vehicle)) {
            if (!silent) {
                this->myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on destination edge '" + to->getID() + "'.");
            }
            return false;
        }

        if (msTime >= myValidUntil) {
            assert(myBuilder != nullptr); // only time independent clones do not have a builder
            while (msTime >= myValidUntil) {
                myValidUntil += myWeightPeriod;
            }
            reset(vehicle);
        } 
        // rewind routing start time to building time (this can only be a gross approximation 
        // of time-dependent routing)
        msTime = myValidUntil - myWeightPeriod;

        double length = 0.; // dummy for the via edge cost update
        this->startQuery();
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        this->init(from->getNumericalID(), msTime);
        this->myAmClean = false;
        // loop
        int num_visited = 0;
        int numberOfFollowers = 0;
        int numberOfAvoidedFollowers = 0;
        int numberOfEmptyFlagVectors = 0;
        const bool mayRevisit = myLookupTable != nullptr && !myLookupTable->consistent();
        const double speed = vehicle == nullptr ? myMaxSpeed : MIN2(vehicle->getMaxSpeed(), myMaxSpeed * vehicle->getChosenSpeedFactor());

        while (!this->myFrontierList.empty()) {
            num_visited += 1;
            // use the edge with the minimal length
            auto* const minimumInfo = this->myFrontierList.front();
            const E* const minEdge = minimumInfo->edge;
            // check whether the destination edge was already reached
            if (minEdge == to) {
                this->buildPathFrom(minimumInfo, into);
                this->endQuery(num_visited);
#ifdef AFRO_DEBUG_LEVEL_1
                std::cout << "Found to, to->getID(): " << to->getID() << std::endl;
                std::cout << static_cast<double>(numberOfFollowers - numberOfAvoidedFollowers) / static_cast<double>(num_visited)
                    << " followers considered (out of " << static_cast<double>(numberOfFollowers) / static_cast<double>(num_visited) << ") on average." << std::endl;
                std::cout << static_cast<double>(numberOfFollowers - numberOfAvoidedFollowers)
                    << " followers considered (out of " << static_cast<double>(numberOfFollowers) << ")." << std::endl;
                std::cout << numberOfEmptyFlagVectors << " out of " << numberOfFollowers << " flag vectors of followers were unassigned (i.e., empty)." << std::endl;
                std::cout << "num_visited: " << num_visited << std::endl;
#endif
                return true;
            }
            std::pop_heap(this->myFrontierList.begin(), this->myFrontierList.end(), myComparator);
            this->myFrontierList.pop_back();
            this->myFound.push_back(minimumInfo);
            minimumInfo->visited = true;

            const double effortDelta = this->getEffort(minEdge, vehicle, minimumInfo->leaveTime);
            const double leaveTime = minimumInfo->leaveTime + this->getTravelTime(minEdge, vehicle, minimumInfo->leaveTime, effortDelta);

            // admissible A* heuristic: straight line distance at maximum speed
            // this is calculated from the end of minEdge so it possibly includes via efforts to the followers
            double heuristic_remaining = 0.; 
            double heuristicEffort = minimumInfo->effort + effortDelta + heuristic_remaining;
            // check all ways from the edge with the minimal length
            for (const std::pair<const E*, const E*>& follower : minEdge->getViaSuccessors(vClass)) {
                auto& followerInfo = this->myEdgeInfos[follower.first->getNumericalID()];
                const FlagInfo* followerFlagInfo = (*myFlagInfos)[follower.first->getNumericalID()];
                // check whether it can be used
                if (followerInfo.prohibited || this->isProhibited(follower.first, vehicle)) {
                    continue;
                }
                numberOfFollowers++;
                if (followerFlagInfo->arcFlags.empty()) {
                    numberOfEmptyFlagVectors++;
                }
#ifdef AFRO_DEBUG_LEVEL_2
                myFlagContextStartTime = SysUtils::getCurrentMillis();
#endif
                std::tuple<int, int, bool> flagContext = this->flagContext(follower.first, to);
                //std::tuple<int, int, bool> flagContext = this->flagContextNaive(follower.first, to);
#ifdef AFRO_DEBUG_LEVEL_2
                myFlagContextTimeSum += (SysUtils::getCurrentMillis() - myFlagContextStartTime);
#endif
                if (!flag(followerFlagInfo, flagContext)) {
                    numberOfAvoidedFollowers++;
                    continue;
                } 
                
                // admissible A* heuristic: straight line distance at maximum speed
                // this is calculated from the end of minEdge so it possibly includes via efforts to the followers
                if (heuristic_remaining == 0 && std::get<0>(flagContext) == 0 && std::get<2>(flagContext)) {
                    // arrived at the target cell at level 0? use heuristic
                    heuristic_remaining =
                        (myLookupTable == nullptr ? minEdge->getDistanceTo(to) / speed :
                            myLookupTable->lowerBound(minEdge, to, speed, vehicle->getChosenSpeedFactor(),
                                minEdge->getMinimumTravelTime(nullptr), to->getMinimumTravelTime(nullptr)));
                    if (heuristic_remaining == UNREACHABLE) {
                        break; // -> skip remaining followers, continue with next min heap element
                    }
                    heuristicEffort += heuristic_remaining;
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
                        std::push_heap(this->myFrontierList.begin(), this->myFrontierList.end(), myComparator);
                    }
                    else {
                        auto fi = std::find(this->myFrontierList.begin(), this->myFrontierList.end(), &followerInfo);
                        if (fi == this->myFrontierList.end()) {
                            assert(mayRevisit);
                            this->myFrontierList.push_back(&followerInfo);
                            std::push_heap(this->myFrontierList.begin(), this->myFrontierList.end(), myComparator);
                        }
                        else {
                            std::push_heap(this->myFrontierList.begin(), fi + 1, myComparator);
                        }
                    }
                }
            } // for followers
        }
        this->endQuery(num_visited);
#ifdef AFRO_DEBUG_LEVEL_1
        std::cout << "Queue ran empty, no solution." << std::endl;
        std::cout << static_cast<double>(numberOfFollowers - numberOfAvoidedFollowers) / static_cast<double>(num_visited) 
            << " followers considered (out of " << static_cast<double>(numberOfFollowers) / static_cast<double>(num_visited) << ") on average." << std::endl;
        std::cout << static_cast<double>(numberOfFollowers - numberOfAvoidedFollowers)
            << " followers considered (out of " << static_cast<double>(numberOfFollowers) << ")." << std::endl;
        std::cout << numberOfEmptyFlagVectors << " out of " << numberOfFollowers << " flag vectors of followers were unassigned (i.e., empty)." << std::endl;
        std::cout << "num_visited: " << num_visited << std::endl;
#endif
        if (!silent) {
            this->myErrorMsgHandler->informf("No connection between edge '%' and edge '%' found.", from->getID(), to->getID());
        }
        return false;
    }

    /// @brief Start timer for query time sum
    void startQuery();
    /// @brief Stop timer for query time sum
    void endQuery(int visits);
    /// @brief Report query time statistics
    void reportStatistics();
    /// @brief Reset query time statistics
    void resetStatistics();
    /// @brief Bulk mode is not supported
    virtual void setBulkMode(const bool mode) {
        UNUSED_PARAMETER(mode);
        throw std::runtime_error("Bulk mode is not supported by the arc flag router.");
    }

protected:
    /// @brief Edge infos containing the associated edge and its arc flags 
    std::vector<FlagInfo*>* myFlagInfos;
    /// @brief The partition
    const KDTreePartition<E, N, V>* myPartition;
    /// @brief The comparator for edge information
    EdgeInfoComparator myComparator;
    /// @brief The lookup table for travel time heuristics
    const std::shared_ptr<const LookupTable> myLookupTable;
    /// @brief The maximum speed in the network
    double myMaxSpeed;
    /// @brief The validity duration of one weight interval
    const SUMOTime myWeightPeriod;
    /// @brief The validity duration of the current flag infos (exclusive)
    SUMOTime myValidUntil;
    /// @brief The builder
    AFBuilder<E, N, V>* myBuilder;
    /// @brief The type of this router
    /// @note The one in SUMOAbstractRouter is private, required for more flexible performance logging (see below)
    const std::string myType;
    /// @brief Counters for performance logging
    /// @note The ones in SUMOAbstractRouter are private - introduced to reset stats / recalculate before destruction
    long long int myQueryVisits;
    long long int myNumQueries;
    /// @brief The time spent querying in milliseconds
    /// @note The ones in SUMOAbstractRouter are private - introduced to reset stats / recalculate before destruction
    long long int myQueryStartTime;
    long long int myQueryTimeSum;
#ifdef AFRO_DEBUG_LEVEL_2
    /// @brief The time spent for flagContext in milliseconds
    long long int myFlagContextStartTime;
    long long int myFlagContextTimeSum;
#endif
private:
    /// @brief The cell of the last settled edge
    const Cell* myLastSettledEdgeCell;
    /// @brief The last flag context
    std::tuple<int, int, bool> myLastFlagContext;
    /// @brief The cell of the target edge at SHARC level 0
    const Cell* myTargetEdgeCellLevel0;
};

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
std::vector<bool>& AFRouter<E, N, V>::flags(const E* edge) {
    assert(edge);
    if (!myFlagInfos) {
        throw std::runtime_error("flag infos not initialized, call compute() at least once before calling flags().");
    }
    return ((*myFlagInfos)[edge->getNumericalID()])->arcFlags;
}

template<class E, class N, class V>
void AFRouter<E, N, V>::init(const int edgeID, const SUMOTime msTime) {
    // all EdgeInfos touched in the previous query are either in myFrontierList or myFound: clean those up
    myTargetEdgeCellLevel0 = nullptr;
    for (auto& edgeInfo : this->myFrontierList) {
        edgeInfo->reset();
    }
    this->myFrontierList.clear();
    for (auto& edgeInfo : this->myFound) {
        edgeInfo->reset();
    }
    this->myFound.clear();
    if (edgeID > -1) {
        // add begin node
        auto& fromInfo = this->myEdgeInfos[edgeID];
        fromInfo.heuristicEffort = 0.;
        fromInfo.effort = 0.;
        fromInfo.leaveTime = STEPS2TIME(msTime);
        fromInfo.prev = nullptr;
        this->myFrontierList.push_back(&fromInfo);
    }
}

template<class E, class N, class V>
std::tuple<int, int, bool> AFRouter<E, N, V>::flagContextNaive(const E* settledEdge, const E* targetEdge)
{
    assert(settledEdge != nullptr && targetEdge != nullptr);
    int sHARCLevel;
    for (sHARCLevel = 0; sHARCLevel < myPartition->getNumberOfLevels() - 1; sHARCLevel++) {
        int partitionLevel = sHARCLevel2PartitionLevel(sHARCLevel, myPartition->getNumberOfLevels());
        const std::vector<const Cell*>& levelCells = myPartition->getCellsAtLevel(partitionLevel);
        typename std::vector<const Cell*>::const_iterator first = levelCells.begin();
        typename std::vector<const Cell*>::const_iterator last = levelCells.end();
        typename std::vector<const Cell*>::const_iterator iter;
        const Cell* settledEdgeCell = nullptr;
        const Cell* targetEdgeCell = nullptr;
        // go through the cells of the level
        for (iter = first; iter != last; iter++) {
            // myPartition is assumed to partition a non-reversed (forward) graph
            if (!settledEdgeCell && (*iter)->contains(settledEdge->getFromJunction())) {
                settledEdgeCell = *iter;
            }
            if (!targetEdgeCell && (*iter)->contains(targetEdge->getFromJunction())) {
                targetEdgeCell = *iter;
            }
            if (settledEdgeCell && targetEdgeCell) {
                break;
            }
        }
        assert(settledEdgeCell && targetEdgeCell); // we should find both edges on each level
        if (settledEdgeCell->getSupercell() == targetEdgeCell->getSupercell()) {
            return std::make_tuple(sHARCLevel, targetEdgeCell->isLeftOrLowerCell() ? 0 : 1, 
                settledEdgeCell == targetEdgeCell);
        }
    }
    // we should never arrive here
    throw std::runtime_error("flagContext: relevant level could not be determined.");
}

template<class E, class N, class V>
std::tuple<int, int, bool> AFRouter<E, N, V>::flagContext(const E* settledEdge, const E* targetEdge)
{
    assert(settledEdge != nullptr && targetEdge != nullptr);
    int sHARCLevel = 0; // lowest level with smallest cells
    const Cell* settledEdgeCell = nullptr;
    const Cell* targetEdgeCell = nullptr;
    if (myLastSettledEdgeCell
        && myLastSettledEdgeCell->contains(settledEdge->getFromJunction())) {
        // exploit the partial locality of Dijkstra's algorithm: settled edge is still 
        // in the same cell as the last one? Then we can simply return the 
        // last flagContext tuple again.
        return myLastFlagContext;
    }
    int numberOfPartitionLevels = myPartition->getNumberOfLevels();
    if (numberOfPartitionLevels <= 4) { // small number of bottom cells -> go through them, no use of k-d tree
        int partitionLevel = sHARCLevel2PartitionLevel(sHARCLevel, myPartition->getNumberOfLevels());
        const std::vector<const Cell*>& levelCells = myPartition->getCellsAtLevel(partitionLevel);
        typename std::vector<const Cell*>::const_iterator first = levelCells.begin();
        typename std::vector<const Cell*>::const_iterator last = levelCells.end();
        typename std::vector<const Cell*>::const_iterator iter;
        // go through the cells of the level
        for (iter = first; iter != last; iter++) {
            // myPartition is assumed to partition a non-reversed (forward) graph
            if (!settledEdgeCell 
                && (*iter)->contains(settledEdge->getFromJunction())) {
                settledEdgeCell = *iter;
            }
            if (!targetEdgeCell && myTargetEdgeCellLevel0) {
                targetEdgeCell = myTargetEdgeCellLevel0;
            } else if (!targetEdgeCell 
                && (*iter)->contains(targetEdge->getFromJunction())) {
                myTargetEdgeCellLevel0 = *iter;
                targetEdgeCell = myTargetEdgeCellLevel0;
            }
            if (settledEdgeCell && targetEdgeCell) {
                assert(myTargetEdgeCellLevel0);
                break;
            }
        }
    } else { // larger number of bottom cells -> use a k-d tree
        settledEdgeCell = myPartition->searchNode(settledEdge->getFromJunction());
        if (!targetEdgeCell && myTargetEdgeCellLevel0) {
            // search only once per query
            targetEdgeCell = myTargetEdgeCellLevel0;
        }
        else if (!targetEdgeCell) {
            myTargetEdgeCellLevel0 = myPartition->searchNode(targetEdge->getFromJunction());
            targetEdgeCell = myTargetEdgeCellLevel0; // myTargetEdgeCellLevel0 is reset in init()
        }
    }
    assert(settledEdgeCell && targetEdgeCell); // we should find both edges on each level
    while (settledEdgeCell->getSupercell() != targetEdgeCell->getSupercell()) {
        settledEdgeCell = settledEdgeCell->getSupercell();
        targetEdgeCell = targetEdgeCell->getSupercell();
        sHARCLevel++;
    }
    myLastSettledEdgeCell = settledEdgeCell;
    std::tuple<int, int, bool> flagContext = std::make_tuple(sHARCLevel, targetEdgeCell->isLeftOrLowerCell() ? 0 : 1,
        settledEdgeCell == targetEdgeCell);
    myLastFlagContext = flagContext;
    return flagContext;
}

template<class E, class N, class V>
void AFRouter<E, N, V>::startQuery() {
    myNumQueries++;
    myQueryStartTime = SysUtils::getCurrentMillis();
    SUMOAbstractRouter<E, V>::startQuery();
}

template<class E, class N, class V>
void AFRouter<E, N, V>::endQuery(int visits) {
    myQueryVisits += visits;
    myQueryTimeSum += (SysUtils::getCurrentMillis() - myQueryStartTime);
    SUMOAbstractRouter<E, V>::endQuery(visits);
}

template<class E, class N, class V>
void AFRouter<E, N, V>::reportStatistics() {
    if (myNumQueries > 0) {
        WRITE_MESSAGE(myType + " answered " + toString(myNumQueries) + " queries and explored " + toString((double)myQueryVisits / (double)myNumQueries) + " edges on average.");
        WRITE_MESSAGE(myType + " spent " + elapsedMs2string(myQueryTimeSum) + " answering queries (" + toString((double)myQueryTimeSum / (double)myNumQueries) + " ms on average).");
#ifdef AFRO_DEBUG_LEVEL_2
        WRITE_MESSAGE("flagContext spent " + elapsedMs2string(myFlagContextTimeSum) + " (" + toString((double)myFlagContextTimeSum / (double)myNumQueries) + " ms on average).");
#endif
    }
}

template<class E, class N, class V>
void AFRouter<E, N, V>::resetStatistics() {
    myNumQueries = 0;
    myQueryVisits = 0;
    myQueryTimeSum = 0;
    myQueryStartTime = 0;
}
