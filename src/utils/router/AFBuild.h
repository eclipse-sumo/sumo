/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    AFBuild.h
/// @author  Ruediger Ebendt
/// @date    01.12.2023
///
// Class for building the arc flags for (multi-level) arc flag routing
/****************************************************************************/
#pragma once
#include <config.h>
#include <memory>
#include <vector>
#include <unordered_set>
#include <math.h>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <cinttypes>
#include <utility>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include "KDTreePartition.h"
#include "Node2EdgeRouter.h"
#include "FlippedEdge.h"
#include "AFCentralizedSPTree.h"

//#define AFBU_WRITE_QGIS_FILTERS
#ifdef AFBU_WRITE_QGIS_FILTERS
#include <fstream>
#include <sstream>
#endif

//#define AFBU_DEBUG_LEVEL_0
//#define AFBU_DEBUG_LEVEL_1
//#define AFBU_DEBUG_LEVEL_2

#ifdef AFBU_DEBUG_LEVEL_2
#define AFBU_DEBUG_LEVEL_1
#endif

#ifdef AFBU_DEBUG_LEVEL_1
#define AFBU_DEBUG_LEVEL_0
#endif

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

// ===========================================================================
// class declarations
// ===========================================================================
template<class E, class N, class V>
class AFRouter;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class AFBuild
 * @brief Builds the flags for (multi-level) arc flag routing (Hilger et al.) in its multi-level variant 
 * (also called "stripped SHARC" by Delling et al.)
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge )
 * @param N The node class to use (MSJunction/RONode)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 */
template<class E, class N, class V>
class AFBuild {
public:
    /// @brief Maximum difference of two path lengths considered to be equal   
    const double EPS = 0.009;
    typedef typename KDTreePartition<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>::Cell Cell;
    typedef typename AFInfo<FlippedEdge<E, N, V>>::ArcInfo ArcInfo;
    typedef typename AFInfo<E>::FlagInfo FlagInfo;
    typedef AbstractLookupTable<FlippedEdge<E, N, V>, V> FlippedLookupTable;

    /** @brief Constructor
     * @param[in] flippedEdges The flipped (aka reversed / backward) edges
     * @param[in] flippedPartition The k-d tree partition of the backward graph with flipped edges
     * @param[in] numberOfLevels The number of levels
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] flippedOperation The operation for a backward graph with flipped edges
     * @param[in] flippedLookup The lookup table for a backward graph with flipped edges
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered or not
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered or not
     * @param[in] toProhibit The list of explicitly prohibited edges
     */ 
    AFBuild(
        const std::vector<FlippedEdge<E, N, V>*>& flippedEdges,
        const KDTreePartition<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>* const flippedPartition,
        int numberOfLevels, bool unbuildIsWarning, 
        typename SUMOAbstractRouter<FlippedEdge<E, N, V>, V>::Operation flippedOperation,
        const std::shared_ptr<const FlippedLookupTable> flippedLookup = nullptr,
        const bool havePermissions = false, const bool haveRestrictions = false, 
        const std::vector<FlippedEdge<E, N, V>*>* toProhibit = nullptr) :
        myFlippedEdges(flippedEdges),
        myFlippedPartition(flippedPartition),
        myNumberOfLevels(numberOfLevels),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myFlippedOperation(flippedOperation),
        myFlippedLookupTable(flippedLookup),
        myHavePermissions(havePermissions),
        myHaveRestrictions(haveRestrictions),
        myProhibited(toProhibit),
        myNode2EdgeRouter(new Node2EdgeRouter<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>(myFlippedEdges, 
            unbuildIsWarning, myFlippedOperation, myFlippedLookupTable, myHavePermissions, myHaveRestrictions)) 
    {
        myCentralizedSPTree = new AFCentralizedSPTree<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>(myFlippedEdges, 
            myArcInfos, unbuildIsWarning, myNode2EdgeRouter, myHavePermissions, myHaveRestrictions);
        if (toProhibit) {
            myNode2EdgeRouter->prohibit(*toProhibit);
        }
    }

    /// @brief Destructor
    ~AFBuild() {
        delete myCentralizedSPTree;
        delete myNode2EdgeRouter;
    }

    /// @brief Returns the operation for a backward graph with flipped edges
    typename SUMOAbstractRouter<FlippedEdge<E, N, V>, V>::Operation getFlippedOperation() {
        return myFlippedOperation;
    }
    /// @brief Returns the lookup table for the backward graph with flipped edges
    const std::shared_ptr<const FlippedLookupTable> getFlippedLookup() {
        return myFlippedLookupTable;
    }
    /** @brief Initialize the arc flag build
     * @param[in] msTime The start time of the routes in milliseconds
     * @param[in] vehicle The vehicle 
     * @param[in] flagInfos The arc flag informations
     */  
    void init(SUMOTime time, const V* const vehicle, std::vector<FlagInfo*>& flagInfos);
    /** @brief Set the flipped partition 
     * param[in] flippedPartition The flipped partition
     */
    void setFlippedPartition(const KDTreePartition<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>* flippedPartition) {
        myFlippedPartition = flippedPartition;
    }
    /** @brief Converts a partition level number to a SHARC level number
     * @param[in] partitionLevel The partition level
     * @return The SHARC level number corresponding to the given partition level number
     */ 
    int partitionLevel2SHARCLevel(int partitionLevel) {
        return AFRouter<E, N, V>::partitionLevel2SHARCLevel(partitionLevel, myNumberOfLevels);
    }
    /** @brief Converts a SHARC level number to a partition level number
     * @param[in] sHARCLevel The SHARC level
     * @return The partition level number corresponding to the given SHARC level number
     */ 
    int sHARCLevel2PartitionLevel(int sHARCLevel) {
        return AFRouter<E, N, V>::sHARCLevel2PartitionLevel(sHARCLevel, myNumberOfLevels);
    }

protected:
    /** @brief Computes the arc flags for all cells at a given level
     * @param[in] msTime The start time of the routes in milliseconds
     * @param[in] sHARCLevel The SHARC level
     * @param[in] vehicle The vehicle
     */ 
    void computeArcFlags(SUMOTime msTime, int sHARCLevel, const V* const vehicle);
    /** @brief Computes the arc flags for a given cell
     * @param[in] msTime The start time of the routes in milliseconds
     * @param sHARCLevel The SHARC level
     * @param[in] cell The cell
     * @param[in] vehicle The vehicle
     */
    void computeArcFlags(SUMOTime msTime, const int sHARCLevel, const Cell* cell, const V* const vehicle);
    /** @brief Computes the arc flags for a given cell (naive version)
     * @param[in] msTime The start time of the routes in milliseconds
     * @param sHARCLevel The SHARC level
     * @param[in] cell The cell
     * @param[in] vehicle The vehicle
     */
    void computeArcFlagsNaive(SUMOTime msTime, const int sHARCLevel, const Cell* cell, const V* const vehicle);
    /** @brief Put the arc flag of the edge in arcInfo  
     * @note wrt the passed SHARC level, and the boolean flag indicating whether the respective cell is a left or lower one or not
     * @param[in] arcInfo The arc information
     * @param[in] sHARCLevel The SHARC level
     * @param[in] isLeftOrLowerCell The boolean flag indicating whether the respective cell is a left or lower one or not
     */
    void putArcFlag(ArcInfo* arcInfo, const int sHARCLevel, const bool isLeftOrLowerCell);
    /// @brief The flipped edges
    const std::vector<FlippedEdge<E, N, V>*>& myFlippedEdges;
    /// @brief The partition for the backward graph with flipped edges
    const KDTreePartition<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>* myFlippedPartition;
    /// @brief The number of levels
    const int myNumberOfLevels;
    /// @brief The handler for routing errors
    MsgHandler* const myErrorMsgHandler;
    /// @brief The object's operation to perform on a backward graph with flipped edges
    typename SUMOAbstractRouter<FlippedEdge<E, N, V>, V>::Operation myFlippedOperation;
    /// @brief The lookup table for travel time heuristics
    const std::shared_ptr<const FlippedLookupTable> myFlippedLookupTable;
    /// @brief The boolean flag indicating whether edge permissions need to be considered or not
    const bool myHavePermissions;
    /// @brief The boolean flag indicating whether edge restrictions need to be considered or not
    const bool myHaveRestrictions;
    /// @brief The list of explicitly prohibited edges
    const std::vector<FlippedEdge<E, N, V>*>* myProhibited;
    /// @brief The node-to-edge router (for a backward graph with flipped edges)
    Node2EdgeRouter<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>* myNode2EdgeRouter;
    /// @brief A Dijkstra based centralized label-correcting algorithm 
    // @note Builds shortest path trees for all boundary nodes at once
    // @note It operates on a backward graph with flipped edges
    AFCentralizedSPTree<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>* myCentralizedSPTree;
    /// @brief The container of arc informations (for the centralized shortest path tree)
    std::vector<ArcInfo*> myArcInfos;

private: 
    /** @brief Initialize the boundary edges 
     * param[in] boundaryEdges The boundary edges
     */
    void initBoundaryEdges(const std::unordered_set<const FlippedEdge<E, N, V>*>& boundaryEdges);
    /** @brief Initialize the supercell edges
     * @param[in] supercell The supercell
     * @param[in] boundaryEdges The boundary edges
     * @param[in] numberOfBoundaryNodes The number of boundary nodes
     */
    void initSupercellEdges(const Cell* supercell, 
        const std::unordered_set<const FlippedEdge<E, N, V>*>& boundaryEdges, 
        size_t numberOfBoundaryNodes);
    /** @brief Helper method for computeArcFlags(), which computes the arc flags for a given cell 
     * @param[in] msTime The start time of the routes in milliseconds
     * @param[in] sHARCLevel The SHARC level
     * @param[in] cell The cell
     * @param[in] vehicle The vehicle
     */
    void computeArcFlagsAux(SUMOTime msTime, const int sHARCLevel, const Cell* cell, const V* const vehicle);
}; // end of class AFBuild declaration

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
void AFBuild<E, N, V>::init(SUMOTime msTime, const V* const vehicle, std::vector<FlagInfo*>& flagInfos) {
    if (myArcInfos.empty()) {
        for (const FlippedEdge<E, N, V>* const flippedEdge : myFlippedEdges) {
            myArcInfos.push_back(new ArcInfo(flippedEdge));
        }
    }
    int sHARCLevel;
    for (sHARCLevel = 0; sHARCLevel < myNumberOfLevels - 1; sHARCLevel++) {
#ifdef AFBU_DEBUG_LEVEL_0
        std::cout << "Starting computation of flags of level " << sHARCLevel << " (levels run from 0 to "
            << myNumberOfLevels - 2 << ")." << std::endl;
#endif
#ifdef AFBU_DEBUG_LEVEL_2
        if (sHARCLevel != 0) {
            continue;
        }
#endif
        computeArcFlags(msTime, sHARCLevel, vehicle);
    }
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Copying arc flags from the arc infos... " << std::endl;
#endif
    int index = 0;
    for (const ArcInfo* arcInfo : myArcInfos) {
        flagInfos[index++]->arcFlags = arcInfo->arcFlags;
        delete arcInfo;
    }
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Arc flags copied from the arc infos. " << std::endl;
#endif
    myArcInfos.clear();
}

template<class E, class N, class V>
void AFBuild<E, N, V>::computeArcFlags(SUMOTime msTime, const int sHARCLevel, const V* const vehicle) {
    try {
        assert(myFlippedPartition);
        const std::vector<const Cell*>& levelCells = myFlippedPartition->getCellsAtLevel(sHARCLevel2PartitionLevel(sHARCLevel));
#ifdef AFBU_DEBUG_LEVEL_0
        int i = 0;
#endif
        for (const Cell* cell : levelCells) {
#ifdef AFBU_DEBUG_LEVEL_0
            std::cout << "Starting to compute core flags of the " << i++ << "th cell..." << std::endl;
#endif
#ifdef AFBU_DEBUG_LEVEL_2
            if (cell->getNumber() == 4) {
#endif
                // kept to make comparisons possible
                //computeArcFlagsNaive(msTime, sHARCLevel, cell, vehicle);
                computeArcFlags(msTime, sHARCLevel, cell, vehicle);
                // clean up (all except the computed flags, of course)
#ifdef AFBU_DEBUG_LEVEL_0
                std::cout << "Cleaning up after computeArcFlags..." << std::endl;
#endif
                for (ArcInfo* arcInfo : myArcInfos) {
                    arcInfo->effortsToBoundaryNodes.clear();
                    arcInfo->touched = false;
                }
#ifdef AFBU_DEBUG_LEVEL_0
                std::cout << "Cleaned up." << std::endl;
#endif
#ifdef AFBU_DEBUG_LEVEL_2
            }
#endif
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        exit(-1);
    }
}

template<class E, class N, class V>
void AFBuild<E, N, V>::computeArcFlagsNaive(SUMOTime msTime, const int sHARCLevel, const Cell* cell, const V* const vehicle) {
    const Cell* supercell = cell->getSupercell();
    const std::unordered_set<const FlippedEdge<E, N, V>*>& boundaryEdges = cell->getOutgoingBoundaryEdges();
    const std::vector<const FlippedNode<E, N, V>*>& boundaryNodes = cell->getBoundaryFromNodes();
#ifdef AFBU_DEBUG_LEVEL_1
    std::cout << "Number of boundary edges: " << boundaryEdges.size() << std::endl;
    std::cout << "Number of boundary nodes: " << boundaryNodes.size() << std::endl;
    std::cout << "Cell number: " << cell->getNumber() << std::endl;
    std::cout << "Supercell number: " << supercell->getNumber() << std::endl;
#endif
    //
    // initialization of arc flag vectors
    initBoundaryEdges(boundaryEdges);

#ifdef AFBU_DEBUG_LEVEL_0
    long long int startTime = SysUtils::getCurrentMillis();
#endif
    for (const FlippedNode<E, N, V>* boundaryNode : boundaryNodes) {
        for (const FlippedEdge<E, N, V>* boundaryEdge : boundaryEdges) {
            assert(!boundaryEdge->isInternal());
            ArcInfo* arcInfo = myArcInfos[boundaryEdge->getNumericalID()];
            if (boundaryNode == boundaryEdge->getFromJunction()) {
                arcInfo->effortsToBoundaryNodes.push_back(0.);
                continue;
            }
            // compute effort
            std::vector<const FlippedEdge<E, N, V>*> into;
#ifdef AFBU_DEBUG_LEVEL_2
            std::vector<const FlippedEdge<E, N, V>*> into2;
#endif
            if (myNode2EdgeRouter->computeNode2Edge(boundaryNode, boundaryEdge, vehicle, msTime, into)) {
                double recomputedEffort = myNode2EdgeRouter->recomputeCostsNoLastEdge(into, vehicle, msTime);
                arcInfo->effortsToBoundaryNodes.push_back(recomputedEffort);
#ifdef AFBU_DEBUG_LEVEL_2
                if (!into.empty() && myNode2EdgeRouter->compute(into[0], boundaryEdge, vehicle, msTime, into2)) {
                    double recomputedEffort2 = myNode2EdgeRouter->recomputeCosts(into2, vehicle, msTime);

                    std::cout << "node2Edge router succeeded, effort: " << recomputedEffort << ", effort incl. last edge: " << recomputedEffort2 << std::endl;
                    assert(recomputedEffort <= recomputedEffort2);
                }
#endif
            }
            else {
                arcInfo->effortsToBoundaryNodes.push_back(UNREACHABLE);
#ifdef AFBU_DEBUG_LEVEL_2
                std::cout << "UNREACHABLE!" << std::endl;
#endif
            }
        }
    }
#ifdef AFBU_DEBUG_LEVEL_0
    long long int timeSpent = SysUtils::getCurrentMillis() - startTime;
    std::cout << "Initial distance computation spent " + elapsedMs2string(timeSpent) + "." << std::endl;
 #endif

    // initialize all supercell edges' labels, arc flag vectors for the centralized shortest path tree algorithm / arc flag build
    initSupercellEdges(supercell, boundaryEdges, boundaryNodes.size());
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Initialization of all supercell edges' labels and arc flag vectors done. Starting the centralized shortest path tree algorithm..." << std::endl;
#endif
    if (myCentralizedSPTree->computeCentralizedSPTree(msTime, cell, vehicle)) {
        computeArcFlagsAux(msTime, sHARCLevel, cell, vehicle);
    }
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Centralized shortest path tree algorithm finished." << std::endl;
#endif
}

template<class E, class N, class V>
void AFBuild<E, N, V>::computeArcFlags(SUMOTime msTime, const int sHARCLevel, const Cell* cell, const V* const vehicle) {
    const Cell* supercell = cell->getSupercell();
    const std::unordered_set<const FlippedEdge<E, N, V>*>& boundaryEdges = cell->getOutgoingBoundaryEdges(); 
    const std::vector<const FlippedNode<E, N, V>*>& boundaryNodes = cell->getBoundaryFromNodes();
#ifdef AFBU_DEBUG_LEVEL_1
    std::cout << "Number of boundary edges: " << boundaryEdges.size() << std::endl;
    std::cout << "Number of boundary nodes: " << boundaryNodes.size() << std::endl;
    std::cout << "Cell number: " << cell->getNumber() << std::endl;
    std::cout << "Supercell number: " << supercell->getNumber() << std::endl;
#endif
    // initialization of arc flag vectors
    initBoundaryEdges(boundaryEdges);
#ifdef AFBU_DEBUG_LEVEL_1
    long long int startTime = SysUtils::getCurrentMillis();
#endif
    std::map<const FlippedEdge<E, N, V>*, std::vector<const FlippedEdge<E, N, V>*>> incomingEdgesOfOutgoingBoundaryEdges;
    size_t numberOfBoundaryNodes = boundaryNodes.size();
    for (const FlippedEdge<E, N, V>* boundaryEdge : boundaryEdges) {
        incomingEdgesOfOutgoingBoundaryEdges[boundaryEdge] = std::vector<const FlippedEdge<E, N, V>*>(numberOfBoundaryNodes);
    }
    int index = 0; // boundary node index
    for (const FlippedNode<E, N, V>* boundaryNode : boundaryNodes) {
        myNode2EdgeRouter->reset(vehicle);
        if (myNode2EdgeRouter->computeNode2Edges(boundaryNode, boundaryEdges, vehicle, msTime)) {
#ifdef AFBU_DEBUG_LEVEL_2
            std::cout << "Node-to-edge router succeeded." << std::endl;
#endif
        }
        for (const FlippedEdge<E, N, V>* boundaryEdge : boundaryEdges) {
            assert(!boundaryEdge->isInternal());
            ArcInfo* arcInfo = myArcInfos[boundaryEdge->getNumericalID()];
            if (boundaryNode == boundaryEdge->getFromJunction()) {
                arcInfo->effortsToBoundaryNodes.push_back(0.);
                (incomingEdgesOfOutgoingBoundaryEdges[boundaryEdge])[index] = nullptr;
                continue;
            }
            double effort = (myNode2EdgeRouter->edgeInfo(boundaryEdge))->effort;
            const FlippedEdge<E, N, V>* incomingEdge 
                = (myNode2EdgeRouter->edgeInfo(boundaryEdge))->prev ? 
                    (myNode2EdgeRouter->edgeInfo(boundaryEdge))->prev->edge : nullptr;
            (incomingEdgesOfOutgoingBoundaryEdges[boundaryEdge])[index] = incomingEdge;
            arcInfo->effortsToBoundaryNodes.push_back(effort == std::numeric_limits<double>::max() ? UNREACHABLE : effort);
        }
        index++;
    } // end for boundary nodes
#ifdef AFBU_DEBUG_LEVEL_0
    long long int timeSpent = SysUtils::getCurrentMillis() - startTime;
    std::cout << "Initial distance computation spent " + elapsedMs2string(timeSpent) + "." << std::endl;
#endif
    // initialize all supercell edges' labels and arc flag vectors for the centralized shortest path DAG algorithm / arc flag build
    initSupercellEdges(supercell, boundaryEdges, boundaryNodes.size());
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Initialization of all supercell edges' labels and arc flag vectors done. Starting the centralized shortest path tree algorithm..." << std::endl;
#endif
#ifdef AFBU_DEBUG_LEVEL_0
    startTime = SysUtils::getCurrentMillis();
#endif
    if (myCentralizedSPTree->computeCentralizedSPTree(msTime, cell, vehicle, incomingEdgesOfOutgoingBoundaryEdges)) {
#ifdef AFBU_DEBUG_LEVEL_0
        timeSpent = SysUtils::getCurrentMillis() - startTime;
        std::cout << "Centralized SP tree computation spent " + elapsedMs2string(timeSpent) + "." << std::endl;
#endif
        computeArcFlagsAux(msTime, sHARCLevel, cell, vehicle);
    }
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Centralized shortest path tree algorithm finished." << std::endl;
#endif
}

template<class E, class N, class V>
void AFBuild<E, N, V>::computeArcFlagsAux(SUMOTime msTime, const int sHARCLevel, const Cell* cell, const V* const vehicle) {
    const Cell* supercell = cell->getSupercell();
    std::pair<typename std::vector<const FlippedNode<E, N, V>*>::const_iterator,
        typename std::vector<const FlippedNode<E, N, V>*>::const_iterator> supercellNodeIterators = supercell->nodeIterators();
    typename std::vector<const FlippedNode<E, N, V>*>::const_iterator first = supercellNodeIterators.first;
    typename std::vector<const FlippedNode<E, N, V>*>::const_iterator last = supercellNodeIterators.second;
    typename std::vector<const FlippedNode<E, N, V>*>::const_iterator iter;
    std::unordered_set<ArcInfo*> arcInfosOnAShortestPath;
#ifdef AFBU_DEBUG_LEVEL_1
    int numberOfSupercellEdges = 0;
#endif
    for (iter = first; iter != last; iter++) {
        const std::vector<const FlippedEdge<E, N, V>*> incomingEdges = (*iter)->getIncoming();
        for (const FlippedEdge<E, N, V>* supercellEdge : incomingEdges) {
            if (supercellEdge->isInternal()) {
                continue;
            }
            if ((myNode2EdgeRouter->edgeInfo(supercellEdge))->prohibited
                || myNode2EdgeRouter->isProhibited(supercellEdge, vehicle)) {
                continue;
            }
#ifdef AFBU_DEBUG_LEVEL_1
            numberOfSupercellEdges++;
#endif
            ArcInfo* supercellArcInfo = myArcInfos[supercellEdge->getNumericalID()];
            // start by initializing to set of all supercell edge arc infos
            arcInfosOnAShortestPath.insert(supercellArcInfo);
        }
    }
#ifdef AFBU_DEBUG_LEVEL_1
    std::cout << "Number of supercell edges: " << numberOfSupercellEdges << std::endl;
#endif
    const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
#ifdef AFBU_DEBUG_LEVEL_1
    std::cout << "Identifying shortest paths..." << std::endl;
#endif
#ifdef AFBU_DEBUG_LEVEL_0
    long long int startTime = SysUtils::getCurrentMillis();
#endif
    std::unordered_set<ArcInfo*> erasedEdges;
    for (auto iter2 = arcInfosOnAShortestPath.begin(); iter2 != arcInfosOnAShortestPath.end(); )
    {
        const ArcInfo* arcInfo = *iter2;
        assert(!arcInfo->edge->isInternal());
        assert(myNode2EdgeRouter);
        assert(!(myNode2EdgeRouter->edgeInfo(arcInfo->edge))->prohibited
            && !myNode2EdgeRouter->isProhibited(arcInfo->edge, vehicle));
        size_t numberOfBoundaryNodes = arcInfo->effortsToBoundaryNodes.size();
        size_t index;
        bool onShortestPath = false;
        // attempt to prove that the edge is on a shortest path for at least one boundary node
        for (index = 0; index < numberOfBoundaryNodes; index++) {
            double effort1ToBoundaryNode = arcInfo->effortsToBoundaryNodes[index];
            if (effort1ToBoundaryNode == UNREACHABLE) {
                continue;
            }
            if (effort1ToBoundaryNode == std::numeric_limits<double>::max()) {
                continue;
            }
            double sTime = STEPS2TIME(msTime);
            double edgeEffort = myNode2EdgeRouter->getEffort(arcInfo->edge, vehicle, sTime);
            sTime += myNode2EdgeRouter->getTravelTime(arcInfo->edge, vehicle, sTime, edgeEffort);
            double oldEdgeEffort = edgeEffort;
            double oldSTime = sTime;

            for (const std::pair<const FlippedEdge<E, N, V>*, const FlippedEdge<E, N, V>*>& follower : arcInfo->edge->getViaSuccessors(vClass)) {
                assert(!follower.first->isInternal());
                ArcInfo* followerInfo = myArcInfos[follower.first->getNumericalID()];

                // check whether it can be used
                if ((myNode2EdgeRouter->edgeInfo(follower.first))->prohibited
                    || myNode2EdgeRouter->isProhibited(follower.first, vehicle)) {
                    myErrorMsgHandler->inform("Vehicle '" + Named::getIDSecure(vehicle) + "' is not allowed on source edge '" + followerInfo->edge->getID() + "'.");
                    continue;
                }
                if (followerInfo->effortsToBoundaryNodes.empty()) {
                    continue;
                }
                double effort2ToBoundaryNode = followerInfo->effortsToBoundaryNodes[index];
                if (effort2ToBoundaryNode == UNREACHABLE) {
                    continue;
                }
                if (effort2ToBoundaryNode == std::numeric_limits<double>::max()) {
                    continue;
                }

                // add via efforts to current follower to edge effort
                double length = 0.; // dummy length for call of updateViaEdgeCost
                myNode2EdgeRouter->updateViaEdgeCost(follower.second, vehicle,
                    sTime /* has been updated to the time where the edge has been traversed */, edgeEffort, length);
                // test whether edge is on a shortest path to a boundary node
                if (effort1ToBoundaryNode + edgeEffort /* edge effort incl. via efforts to current follower of edge */
                    <= effort2ToBoundaryNode /* efforts incl. via efforts to current follower o. e. */
                    + EPS && effort1ToBoundaryNode + edgeEffort >= effort2ToBoundaryNode - EPS) {
                    onShortestPath = true;
                    break; // a shortest path to one boundary node suffices
                }
                edgeEffort = oldEdgeEffort;
                sTime = oldSTime;
            } // end of loop over outgoing edges
        } // loop over indexes
        if (!onShortestPath) {
            erasedEdges.insert(*iter2);
            iter2 = arcInfosOnAShortestPath.erase(iter2); // not on a shortest path, remove it
        }
        else {
            ++iter2;
        }
    } // loop over edge infos

#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Edges clearly not on a shortest path have been removed. Number of edges on a shortest path is now: "
        << arcInfosOnAShortestPath.size() << std::endl;
#endif

    // set arc flags (for level sHARCLevel) for all edges completely inside the cell  
    // (done since these edges all have a to-node inside the cell, i.e. we have to set the own-cell flag for them). 
    std::unordered_set<const FlippedEdge<E, N, V>*>* pEdgesInsideCell = cell->edgeSet(vehicle);
    std::unordered_set<const FlippedEdge<E, N, V>*> edgesInsideCell = *pEdgesInsideCell;
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Adding all edges completely inside the cell to set of edges on a shortest path, cell no:"
        << cell->getNumber() << std::endl;
#endif
    for (const FlippedEdge<E, N, V>* edgeInsideCell : edgesInsideCell) {
        ArcInfo* arcInfo = myArcInfos[edgeInsideCell->getNumericalID()];
        if ((myNode2EdgeRouter->edgeInfo(edgeInsideCell))->prohibited
            || myNode2EdgeRouter->isProhibited(edgeInsideCell, vehicle)) {
            continue;
        }
        arcInfosOnAShortestPath.insert(arcInfo);
    }
    delete pEdgesInsideCell;
#ifdef AFBU_DEBUG_LEVEL_0
    std::cout << "Edges inside cell added." << std::endl;
    long long int timeSpent = SysUtils::getCurrentMillis() - startTime;
    std::cout << "Shortest path identification spent " + elapsedMs2string(timeSpent) + "." << std::endl;
#endif

#ifdef AFBU_WRITE_QGIS_FILTERS
    std::string qgisFilterString = "id IN (";
    std::unordered_set<const FlippedNode<E, N, V>*> nodesOnAShortestPath;
    std::unordered_set<const FlippedNode<E, N, V>*> erasedNodes;
    for (const ArcInfo* arcInfo : arcInfosOnAShortestPath) {
        assert(!(myNode2EdgeRouter->edgeInfo(arcInfo->edge))->prohibited
            && !myNode2EdgeRouter->isProhibited(arcInfo->edge, vehicle));
        nodesOnAShortestPath.insert(arcInfo->edge->getFromJunction());
        nodesOnAShortestPath.insert(arcInfo->edge->getToJunction());
    }
    for (const ArcInfo* erasedEdgeArcInfo : erasedEdges) {
        erasedNodes.insert(erasedEdgeArcInfo->edge->getFromJunction());
        erasedNodes.insert(erasedEdgeArcInfo->edge->getToJunction());
    }
    size_t k = 0;
    // go through the relevant nodes of the supercell
    size_t numberOfNodesOnAShortestPath = nodesOnAShortestPath.size();
    for (const FlippedNode<E, N, V>* node : nodesOnAShortestPath) {
        k++;
        qgisFilterString += node->getID() + (k < numberOfNodesOnAShortestPath ? ", " : "");
    }
    qgisFilterString += ")";
    std::ostringstream pathAndFileName;
    pathAndFileName << "./filter_superset_nodes_cell_" << cell->getNumber() << "_supercell_" << supercell->getNumber() << ".qqf";
    std::ofstream file;
    file.open(pathAndFileName.str());
    std::ostringstream content;
    content << "<Query>" << qgisFilterString << "</Query>";
    file << content.str();
    file.close();
    // erased nodes
    k = 0;
    qgisFilterString.clear();
    qgisFilterString = "id IN (";
    // go through the erased nodes of the supercell
    size_t numberOfErasedNodes = erasedNodes.size();
    for (const FlippedNode<E, N, V>* node : erasedNodes) {
        k++;
        qgisFilterString += node->getID() + (k < numberOfErasedNodes ? ", " : "");
    }
    qgisFilterString += ")";
    pathAndFileName.str("");
    pathAndFileName.clear();
    pathAndFileName << "./filter_erased_nodes_cell_" << cell->getNumber() << "_supercell_" << supercell->getNumber() << ".qqf";
    file.clear();
    file.open(pathAndFileName.str());
    content.str("");
    content.clear();
    content << "<Query>" << qgisFilterString << "</Query>";
    file << content.str();
    file.close();
#endif
    // put arc flags for level 'sHARCLevel' for all supercell edges which are on a shortest path 
    for (ArcInfo* arcInfo : arcInfosOnAShortestPath) {
        putArcFlag(arcInfo, sHARCLevel, cell->isLeftOrLowerCell());
    }
}

template<class E, class N, class V>
void AFBuild<E, N, V>::putArcFlag(ArcInfo* arcInfo, const int sHARCLevel, const bool isLeftOrLowerCell)
{
    assert(arcInfo);
    (arcInfo->arcFlags)[sHARCLevel * 2 + (isLeftOrLowerCell ? 0 : 1)] = 1;
}

template<class E, class N, class V>
void AFBuild<E, N, V>::initBoundaryEdges(const std::unordered_set<const FlippedEdge<E, N, V>*>& boundaryEdges) {
    // initialization of arc flag vectors
    for (const FlippedEdge<E, N, V>* boundaryEdge : boundaryEdges) {
        assert(!boundaryEdge->isInternal());
        ArcInfo* arcInfo;
        arcInfo = myArcInfos[boundaryEdge->getNumericalID()];
        if (arcInfo->arcFlags.empty()) {
            std::fill_n(std::back_inserter(arcInfo->arcFlags),
                myFlippedPartition->numberOfArcFlags(), false);
        }
        arcInfo->effortsToBoundaryNodes.clear();
    }
}

template<class E, class N, class V>
void AFBuild<E, N, V>::initSupercellEdges(
    const Cell* supercell,
    const std::unordered_set<const FlippedEdge<E, N, V>*>& boundaryEdges,
    size_t numberOfBoundaryNodes) {
    std::pair<typename std::vector<const FlippedNode<E, N, V>*>::const_iterator,
        typename std::vector<const FlippedNode<E, N, V>*>::const_iterator> supercellNodeIterators = supercell->nodeIterators();
    typename std::vector<const FlippedNode<E, N, V>*>::const_iterator first = supercellNodeIterators.first;
    typename std::vector<const FlippedNode<E, N, V>*>::const_iterator last = supercellNodeIterators.second;
    typename std::vector<const FlippedNode<E, N, V>*>::const_iterator iter;
    for (iter = first; iter != last; iter++) {
        const std::vector<const FlippedEdge<E, N, V>*> incomingEdges = (*iter)->getIncoming();
        for (const FlippedEdge<E, N, V>* supercellEdge : incomingEdges) {
            if (supercellEdge->isInternal()) {
                continue;
            }
            if (boundaryEdges.count(supercellEdge)) {
                continue;
            }
            ArcInfo* supercellArcInfo;
            supercellArcInfo = myArcInfos[supercellEdge->getNumericalID()];
            if (supercellArcInfo->arcFlags.empty()) {
                std::fill_n(std::back_inserter(supercellArcInfo->arcFlags),
                    myFlippedPartition->numberOfArcFlags(), false);
            }
            supercellArcInfo->effortsToBoundaryNodes.clear();
            std::fill_n(std::back_inserter(supercellArcInfo->effortsToBoundaryNodes),
                numberOfBoundaryNodes, std::numeric_limits<double>::max());
        }
    }
}
