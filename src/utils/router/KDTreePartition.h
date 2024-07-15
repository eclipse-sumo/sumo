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
/// @file    KDTreePartition.h
/// @author  Ruediger Ebendt
/// @date    01.11.2023
///
// Class for partitioning the router's network's nodes wrt a k-d tree subdivision scheme. 
// All nodes are inserted at once at creation. Offers an O(log n) method to search a 
// node within the bottom / leaf cells (i.e., the cell containing the respective node 
// is returned). Here, n denotes the number of cells.
/****************************************************************************/
#pragma once
#include <config.h>
#include <vector>
#include <unordered_set>
#include <math.h>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <cinttypes>
#include <utility>

#define KDTP_EXCLUDE_INTERNAL_EDGES
// Use KDTP_KEEP_OUTGOING_BOUNDARY_EDGES and KDTP_KEEP_BOUNDARY_FROM_NODES on a reversed graph to build arc infos 
// for the arc flag shortest path routing algorithm
#define KDTP_KEEP_OUTGOING_BOUNDARY_EDGES
// Use KDTP_KEEP_INCOMING_BOUNDARY_EDGES on a forward graph to apply arc flag shortest path routing algorithm
#define KDTP_KEEP_INCOMING_BOUNDARY_EDGES
//#define KDTP_KEEP_BOUNDARY_EDGES
//#define KDTP_KEEP_BOUNDARY_NODES
// Use KDTP_KEEP_BOUNDARY_FROM_NODES on a reversed graph to build arc infos 
// for the arc flag shortest path routing algorithm
#define KDTP_KEEP_BOUNDARY_FROM_NODES
//#define KDTP_KEEP_BOUNDARY_TO_NODES
//#define KDTP_FOR_SYNTHETIC_NETWORKS
// Use KDTP_WRITE_QGIS_FILTERS to filter boundary nodes of cells in QGIS operating on a (e.g. PostGreSQL) database of 
// the (e.g. OSM) network
//#define KDTP_WRITE_QGIS_FILTERS
#ifdef KDTP_WRITE_QGIS_FILTERS
#include <fstream>
#include <sstream>
#endif

//#define KDTP_DEBUG_LEVEL_0
//#define KDTP_DEBUG_LEVEL_1
//#define KDTP_DEBUG_LEVEL_2

#ifdef KDTP_DEBUG_LEVEL_2
#define KDTP_DEBUG_LEVEL_1
#endif

#ifdef KDTP_DEBUG_LEVEL_1
#define KDTP_DEBUG_LEVEL_0
#endif

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class KDTreePartition
 * @brief Partitions the router's network wrt a k-d tree subdivision scheme
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param N The node class to use (MSJunction/RONode)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 */
template<class E, class N, class V>
class KDTreePartition {
public:
    /// @brief Enum type for cell axis
    enum class Axis {
        X, Y
    };

    /**
     * @class NodeXComparator
     * Class to compare (and so sort) nodes by x-coordinate value
     */
    class NodeXComparator {
    public:
       /** @brief Comparing method for X-axis
        * @param[in] firstNode The first node
        * @param[in] secondNode The second node
        * @return true iff first node's x-coordinate is smaller than that of the second
        * @note In case of ties: true iff integer interpretation of first node's id is smaller than that of the second
        */
        bool operator()(const N* firstNode, const N* secondNode) const {
            if (firstNode->getPosition().x() == secondNode->getPosition().x()) { // tie
                std::string str = firstNode->getID();
                std::intmax_t firstValue = std::strtoimax(str.c_str(), nullptr, 10);
                str = secondNode->getID();
                std::intmax_t secondValue = std::strtoimax(str.c_str(), nullptr, 10);
                return firstValue < secondValue;
            }
            return firstNode->getPosition().x() < secondNode->getPosition().x();
        }
    };

    /**
     * @class NodeYComparator
     * Class to compare (and so sort) nodes by y-coordinate value
     */
    class NodeYComparator {
    public:
        /** @brief Comparing method for Y-axis
         * @param[in] firstNode The first node
         * @param[in] secondNode The second node
         * @return true iff first node's y-coordinate is smaller than that of the second
         * @note In case of ties: true iff integer interpretation of first node's id is smaller than that of the second
         */
        bool operator()(const N* firstNode, const N* secondNode) const {
            if (firstNode->getPosition().y() == secondNode->getPosition().y()) { // tie
                std::string str = firstNode->getID();
                std::intmax_t firstValue = std::strtoimax(str.c_str(), nullptr, 10);
                str = secondNode->getID();
                std::intmax_t secondValue = std::strtoimax(str.c_str(), nullptr, 10);
                return firstValue < secondValue;
            }
            return firstNode->getPosition().y() < secondNode->getPosition().y();
        }
    };

    /**
     * @class Cell 
     * @brief Represents an element of the node partition (i.e. a node set)
     */
    class Cell {
    public:
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS        
        /** @brief Constructor
         * @note Works recursively, builds the median-based k-d tree
         * @param[in] cells The vector of all cells
         * @param[in] sortedNodes The vector of nodes
         * @note Initially unsorted, gets sorted wrt to the divisional scheme of the k-dtree after instantiation of the k-d tree
         * @param[in] numberOfLevels The number of levels
         * @param[in] level The level
         * @param[in] levelCells The vector of all level cell vectors
         * @param[in] axis The axis (X or X)
         * @param[in] fromInclusive The from-index (inclusive)
         * @param[in] toExclusive The to-index (exclusive)
         * @param[in] supercell The supercell
         * @param[in] minX The minimum X-value of nodes in the cell
         * @param[in] maxX The maximum X-value of nodes in the cell
         * @param[in] minY The minimum Y-value of nodes in the cell
         * @param[in] maxY The maximum Y-value of nodes in the cell
         * @param[in] northernConflictNodes The northern spatial conflict nodes
         * @param[in] easternConflictNodes The eastern spatial conflict nodes
         * @param[in] southernConflictNodes The southern spatial conflict nodes
         * @param[in] westernConflictNodes The western spatial conflict nodes
         * @param[in] isLeftOrLowerCell Boolean flag indicating whether this cell is a left or lower cell or not
         * @param[in] vehicle The vehicle. 
         * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered.
         * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered.
         */
        Cell(std::vector<const Cell*>* cells, std::vector<std::vector<const Cell*>>* levelCells, std::vector<const N*>* sortedNodes, int numberOfLevels, 
            int level, Axis axis, size_t fromInclusive, size_t toExclusive, Cell* supercell, double minX, double maxX, double minY, double maxY, 
            std::unordered_set<const N*>* northernConflictNodes, std::unordered_set<const N*>* easternConflictNodes, 
            std::unordered_set<const N*>* southernConflictNodes, std::unordered_set<const N*>* westernConflictNodes, 
            bool isLeftOrLowerCell, const V* const vehicle, const bool havePermissions, const bool haveRestrictions);
#else
         /** @brief Constructor
          * @note Works recursively, builds the median-based k-d tree
          * @param[in] cells The vector of all cells
          * @param[in] levelCells The vector of all level cell vectors
          * @param[in] sortedNodes The vector of nodes
          * @note Initially unsorted, after instantiation gets sorted wrt to the k-d tree subdivision scheme
          * @param[in] numberOfLevels The number of levels
          * @param[in] level The level
          * @param[in] axis The axis (X or X)
          * @param[in] fromInclusive The from-index (inclusive)
          * @param[in] toExclusive The to-index (exclusive)
          * @param[in] supercell The supercell
          * @param[in] minX The minimum X-value of nodes in the cell
          * @param[in] maxX The maximum X-value of nodes in the cell
          * @param[in] minY The minimum Y-value of nodes in the cell
          * @param[in] maxY The maximum Y-value of nodes in the cell
          * @param[in] isLeftOrLowerCell Boolean flag indicating whether this cell is a left or lower cell or not
          * @param[in] vehicle The vehicle 
          * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered or not
          * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered or not
          */
        Cell(std::vector<const Cell*>* cells, std::vector<std::vector<const Cell*>>* levelCells, std::vector<const N*>* sortedNodes, 
            int numberOfLevels, int level, Axis axis, size_t fromInclusive, size_t toExclusive, Cell* supercell, double minX, double maxX, 
            double minY, double maxY, bool isLeftOrLowerCell, const V* const vehicle, const bool havePermissions, const bool haveRestrictions);
#endif

        /// @brief Destructor
        virtual ~Cell() {
            delete myLeftOrLowerSubcell;
            delete myRightOrUpperSubcell;
        }

        /// @brief Returns the axis of the cell's spatial extension (x or y)
        Axis getAxis() const {
            return myAxis;
        };
        /** 
         * @brief Returns all edges situated inside the cell
         * @param[in] vehicle The vehicle
         */ 
        std::unordered_set<const E*>* edgeSet(const V* const vehicle) const;
        /**
         * @brief Returns the number of edges ending in the cell
         * @param[in] vehicle The vehicle.
         */
        size_t numberOfEdgesEndingInCell(const V* const vehicle) const;
        /**
         * @brief Returns the number of edges starting in the cell
         * @param[in] vehicle The vehicle.
         */
        size_t numberOfEdgesStartingInCell(const V* const vehicle) const;
        /// @brief Returns the cell's number
        int getNumber() const {
            return myNumber;
        }
        /// @brief Returns the cell's level
        // @note Level 0: root 
        int getLevel() const {
            return myLevel;
        }
        /** @brief Returns the number of cells
         * @return The number of the cells
         */
        int getNumberOfCells() const {
            return cellCounter();
        }
        /** @brief Returns a pair of iterators (first, last) to iterate over the nodes of the cell
         * @return A pair of iterators (first, last) to iterate over the nodes of the cell
         */  
        std::pair<typename std::vector<const N*>::const_iterator, 
            typename std::vector<const N*>::const_iterator> nodeIterators() const;
        /** @brief Returns a new vector of nodes in the cell
         * @return A new vector of nodes in the cell
         */ 
        std::vector<const N*>* nodes() const;
#ifdef KDTP_KEEP_BOUNDARY_NODES
        /// @brief Returns the vector of boundary nodes
        const std::vector<const N*>& getBoundaryNodes() const {
            return myBoundaryNodes;
        }
#endif
#ifdef KDTP_KEEP_BOUNDARY_FROM_NODES
        /// @brief Returns the vector of boundary nodes which are from-nodes of outgoing boundary edges
        const std::vector<const N*>& getBoundaryFromNodes() const {
            return myBoundaryFromNodes;
        }
#endif
#ifdef KDTP_KEEP_BOUNDARY_TO_NODES
        /// @brief Returns the vector of boundary nodes which are to-nodes of incoming boundary edges
        const std::vector<const N*>& getBoundaryToNodes() const {
            return myBoundaryToNodes;
        }
#endif
#ifdef KDTP_KEEP_BOUNDARY_EDGES
        /// @brief Returns the set of boundary edges
        const std::unordered_set<const E*>& getBoundaryEdges() const {
            return myBoundaryEdges;
        }
#endif
#ifdef KDTP_KEEP_INCOMING_BOUNDARY_EDGES
        /// @brief Returns the set of incoming boundary edges
        const std::unordered_set<const E*>& getIncomingBoundaryEdges() const {
            return myIncomingBoundaryEdges;
        }
#endif
#ifdef KDTP_KEEP_OUTGOING_BOUNDARY_EDGES
        /// @brief Returns the set of outgoing boundary edges
        const std::unordered_set<const E*>& getOutgoingBoundaryEdges() const {
            return myOutgoingBoundaryEdges;
        }
#endif
        /// @brief Returns the left (cell's axis is X) or lower (cell's axis is Y) subcell
        const Cell* getLeftOrLowerSubcell() const {
            return myLeftOrLowerSubcell;
        }
        /// @brief Returns the right (cell's axis is X) or upper (cell's axis is Y) subcell
        const Cell* getRightOrUpperSubcell() const {
            return myRightOrUpperSubcell;
        }
        /// @brief Returns the supercell
        const Cell* getSupercell() const {
            return mySupercell;
        }
        /// @brief Returns the minimum coordinate of a cell node in X-direction (aka left border coordinate)
        double getMinX() const {
            return myMinX;
        }
        /// @brief Returns the maximum coordinate of a cell node in X-direction (aka right border coordinate)
        double getMaxX() const {
            return myMaxX;
        }
        /// @brief Returns the minimum coordinate of a cell node in Y-direction (aka lower border coordinate)
        double getMinY() const {
            return myMinY;
        }
        /// @brief Returns the maximum coordinate of a cell node in Y-direction (aka upper border coordinate)
        double getMaxY() const {
            return myMaxY;
        }
        /** @brief Tests whether the given node belongs to the cell
         * @param node The node
         * @return true iff the given node belongs to the cell
         */
        bool contains(const N* node) const;
        /// @brief Returns the boolean flag indicating whether this cell is a left or lower cell or not
        bool isLeftOrLowerCell() const {
            return myIsLeftOrLowerCell;
        }
        /// @brief Returns the median coordinate
        double getMedianCoordinate() const {
            return myMedianCoordinate;
        }

    private:
        /** @brief Performs one partition step on the set of nodes sorted wrt to the k-d tree subdivision scheme
         * @returns The median index
         */
        size_t partition();
        /** @brief Returns the global cell counter
         * @return The global cell counter
         */

        static int& cellCounter() {
            static int cellCounter{ 0 };
            return cellCounter;
        }

        /** @brief Returns true iff driving the given vehicle on the given edge is prohibited
         * @param[in] edge The edge
         * @param[in] vehicle The vehicle
         * @return true iff driving the given vehicle on the given edge is prohibited
         */
        bool isProhibited(const E* const edge, const V* const vehicle) const {
            return (myHavePermissions && edge->prohibits(vehicle)) || (myHaveRestrictions && edge->restricts(vehicle));
        }
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
        /** @brief Returns a pair with sets of spatial conflict nodes for a) the left / lower subcell and b) the right / upper subcell
         * @param medianIndex The index into mySortedNodes dividing the cell nodes into two halves of equal size
         * @return A pair with sets of spatial conflict nodes for a) the left / lower subcell and b) the right / upper subcell
         */
        std::pair<std::unordered_set<const N*>*, std::unordered_set<const N*>*> spatialConflictNodes(size_t medianIndex) const;
#endif
        /** @brief Tests whether a given node is situated within the spatial bounds of the cell
         * @param node The node
         * @return true iff a given node is situated within the spatial bounds of the cell
         */
        bool isInBounds(const N* node) const;
        /// @brief Completes the information about the spatial extent
        void completeSpatialInfo();
        /** @brief Returns the minimum coordinate of the cell nodes' positions, in the direction of the given axis (X or Y)
         * @param axis The axis
         * @return The minimum coordinate of the cell nodes' positions, in the direction of the given axis (X or Y)
         */ 
        double minAxisValue(Axis axis) const;
        /** @brief Returns the minimum coordinate of the cell nodes' positions, in the direction of the cell's axis (X or Y)
         * @return The minimum coordinate of the cell nodes' positions, in the direction of the cell's axis(X or Y)
         */
        double minAxisValue() const;
       /** @brief Returns the maximum coordinate of the cell nodes' positions, in the direction of the given axis (X or Y)
        * @param axis The axis
        * @return The maximum coordinate of the cell nodes' positions, in the direction of the given axis (X or Y)
        */
        double maxAxisValue(Axis axis) const;
        /** @brief Returns the maximum coordinate of the cell nodes' positions, in the direction of the cell's axis (X or Y)
         * @return The minimum coordinate of the cell nodes' positions, in the direction of the cell's axis(X or Y)
         */
        double maxAxisValue() const;
        /// @brief The cells
        std::vector<const Cell*>* myCells;
        /// @brief The cells of all partitions at all levels of the k-d tree subdivisional scheme
        std::vector<std::vector<const Cell*>>* myLevelCells;
        /// @brief The container with all nodes, sorted wrt to the k-d tree subdivisional scheme
        std::vector<const N*>* mySortedNodes;
        /// @brief The total number of levels of the k-d tree
        const int myNumberOfLevels;
        /// @brief The level
        const int myLevel;
        /// @brief The number
        const int myNumber;
        /// @brief The axis of the spatial extension
        const Axis myAxis;
        /// @brief The from-index (inclusive)
        const size_t myFromInclusive;
        /// @brief The to-index (exclusive)
        const size_t myToExclusive;
#if defined(KDTP_KEEP_BOUNDARY_NODES) || defined(KDTP_WRITE_QGIS_FILTERS)
        /// @brief The nodes on the cell boundary
        std::vector<const N*> myBoundaryNodes;
#endif
#ifdef KDTP_KEEP_BOUNDARY_FROM_NODES
        /// @brief Those nodes on the cell boundary, which are from-nodes of outgoing boundary edges
        std::vector<const N*> myBoundaryFromNodes;
#endif
#ifdef KDTP_KEEP_BOUNDARY_TO_NODES
        /// @brief Those nodes on the cell boundary, which are to-nodes of incoming boundary edges
        std::vector<const N*> myBoundaryToNodes;
#endif
#ifdef KDTP_KEEP_INCOMING_BOUNDARY_EDGES
        /// @brief The incoming edges on the cell boundary
        std::unordered_set<const E*> myIncomingBoundaryEdges;

#endif
#ifdef KDTP_KEEP_OUTGOING_BOUNDARY_EDGES
        /// @brief The outgoing edges on the cell boundary
        // @note Used for arc flag setter's shortest path tree algorithm, working on a reversed graph
        std::unordered_set<const E*> myOutgoingBoundaryEdges;
#endif
#ifdef KDTP_KEEP_BOUNDARY_EDGES
        /// @brief The edges on the cell boundary
        std::unordered_set<const E*> myBoundaryEdges;
#endif
        /// @brief The super cell
        Cell* mySupercell;
        /// @brief The left (cell's axis is X) or lower (cell's axis is Y) subcell
        Cell* myLeftOrLowerSubcell;
        /// @brief The right (cell's axis is X) or upper (cell's axis is Y) subcell
        Cell* myRightOrUpperSubcell;
        /// @brief The minimum x-value of a node in the cell
        double myMinX;
        /// @brief The maximum x-value of a node in the cell
        double myMaxX;
        /// @brief The minimum y-value of a node in the cell
        double myMinY;
        /// @brief The maximum y-value of a node in the cell
        double myMaxY;
        /// @brief The boolean flag indicating whether the information about the cell's spatial extent is complete or not
        bool myHasCompleteSpatialInfo;
        /// @brief The boolean flag indicating whether the cell's nodes are sorted wrt to the cell's axis or not
        bool myHasNodesSortedWrtToMyAxis;
        /// @brief The boolean flag indicating whether this cell is a left/lower cell or not
        bool myIsLeftOrLowerCell;
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
        /// @brief The northern spatial conflict nodes
        std::unordered_set<const N*>* myNorthernConflictNodes;
        /// @brief The eastern spatial conflict nodes
        std::unordered_set<const N*>* myEasternConflictNodes;
        /// @brief The southern spatial conflict nodes
        std::unordered_set<const N*>* mySouthernConflictNodes;
        /// @brief The western spatial conflict nodes
        std::unordered_set<const N*>* myWesternConflictNodes;
#endif
        /// @brief The boolean flag indicating whether edge permissions need to be considered or not
        const bool myHavePermissions;
        /// @brief The boolean flag indicating whether edge restrictions need to be considered or not
        const bool myHaveRestrictions;
        /// @brief The coordinate in the axis' direction of the node at the median index
        double myMedianCoordinate;
    }; // end of class Cell declaration

    /** @brief Constructor
     * @param[in] numberOfLevels The number of levels
     * @param[in] edges The container with all edges of the network
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered or not
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered or not
     */
    KDTreePartition(int numberOfLevels, const std::vector<E*>& edges, 
        const bool havePermissions, const bool haveRestrictions);

    /// @brief Destructor
    ~KDTreePartition() {
        std::vector<const N*>().swap(mySortedNodes);
        std::vector<const Cell*>().swap(myCells);
        std::vector<std::vector<const Cell*>>().swap(myLevelCells);
        delete myRoot;
    };

    /** @brief Initialize the k-d tree wrt to the given vehicle's permissions
     * @param[in] vehicle The vehicle
     */
    void init(const V* const vehicle);
    /** @brief Delete the k-d tree, and create a new one
     * param[in] vehicle The vehicle
     * @note Recreated wrt the network given at construction and the given edge
     */ 
    void reset(const V* const vehicle) {
        delete myRoot;
        init(vehicle);
    }
    /// @brief Returns the root of the k-d tree
    const Cell* getRoot() const {
        return myRoot;
    }
    /// @brief Returns all cells
    const std::vector<const Cell*>& getCells() {
        return myCells;
    }
    /** @brief Returns the cell with the given number
     * @note Returns nullptr, if no such cell exists
     * @param[in] number The cell number
     * @return The cell with the given number
     */ 
    const Cell* cell(int number) const;
    /// @brief Returns all cells of a level
    const std::vector<const Cell*>& getCellsAtLevel(int level) const {
        return myLevelCells[level];
    }
    /** @brief Returns the numbers of the cells which the given node is situated in
     * @param[in] node The node
     * @return The numbers of the cells which the given node is situated in
     */
    std::vector<int>* cellNumbers(const N* node) const;
    /** @brief Returns the conjugated 2D axis
     * @note X->Y / Y->X
     * @param[in] axis The axis
     * @return The conjugated 2D axis
     */
    static Axis flip(Axis axis) {
        return axis == Axis::X ? Axis::Y : Axis::X;
    }
    /** @brief Returns the number of arc flags required in a multi-level approach
     * @note E.g.: number of levels := 3 -> 2 + 2 = 4 bits are required
     * @note No flag(s) for root level 0; each non-leaf cell has two subcells
     * @return The number of arc flags required in a multi-level approach
     */ 
    int numberOfArcFlags() const {
        return 2 * (myNumberOfLevels-1);
    }
    /// @brief Returns the number of levels L incl. the zeroth level
    // @note Levels go from 0 to L-1
    int getNumberOfLevels() const {
        return myNumberOfLevels;
    }
    /** @brief Returns the number of cells at all levels
     * @return The number of cells at all levels
     */
    size_t numberOfCells() const {
        return static_cast<size_t>(std::lround(std::pow(2, myNumberOfLevels)) - 1);
    }
    /** @brief Returns the number of regions, i.e. the number of cells at the shallowest (bottom/leaf) level
     * @return The number of regions, i.e. the number of cells at the shallowest (bottom/leaf) level
     */
    size_t numberOfRegions() const {
        return static_cast<size_t>(std::lround(std::pow(2, myNumberOfLevels - 1)));
    }
    /// @brief Returns true iff the k-d tree is uninitialized
    bool isClean() {
        return myAmClean;
    }
    /** @brief Search a node in the bottom cells (i.e., return the respective cell)
     * @note Uses the position of the node and the divisional structure of the k-d tree for the search. 
     * @note O(log n) where n = number of cells
     * @param[in] node The node
     * @return The bottom cell containing the node, or nullptr if the node could not be found
     */
    const Cell* searchNode(const N* node) const;
    
private:
    /** @brief Helper method for {@link #cellNumbers()}.
     * @param node The node
     * @param cell The cell
     * @param level The level
     * @param nodeCellNumbers The vector of cell numbers for the passed node
     */
    void cellNumbersAux(const N* node, const Cell* cell, int level, std::vector<int>* nodeCellNumbers) const;
    /// @brief The root of the k-d tree
    const Cell* myRoot;
    /// @brief The number of levels
    const int myNumberOfLevels;
    /// @brief The reference to a constant container with pointers to edges
    const std::vector<E*>& myEdges;
    /// @brief The container with all nodes, sorted wrt to the k-d tree subdivision scheme
    std::vector<const N*> mySortedNodes;
    /// @brief The cells 
    std::vector<const Cell*> myCells;
    /// @brief The cells of all partitions at all levels of the k-d tree subdivision scheme
    std::vector<std::vector<const Cell*>> myLevelCells;
    /// @brief The boolean flag indicating whether edge permissions need to be considered or not
    const bool myHavePermissions;
    /// @brief The boolean flag indicating whether edge restrictions need to be considered or not
    const bool myHaveRestrictions;
    /// @brief The boolean flag indicating whether the k-d tree is a clean (empty, uninitialized) instance or not
    bool myAmClean;
}; // end of class KDTreePartition declaration

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
KDTreePartition<E, N, V>::KDTreePartition(int numberOfLevels, const std::vector<E*>& edges, 
    const bool havePermissions, const bool haveRestrictions) :
    myNumberOfLevels(numberOfLevels),
    myEdges(edges),
    myHavePermissions(havePermissions),
    myHaveRestrictions(haveRestrictions),
    myAmClean(true) /* still uninitialized */ {
    if (numberOfLevels <= 0) {
        throw std::invalid_argument("KDTreePartition::KDTreePartition: zero or negative number of levels has been passed!");
    }
}

template<class E, class N, class V>
void KDTreePartition<E, N, V>::init(const V* const vehicle) {
    size_t edgeCounter = 0;
    std::unordered_set<const N*> nodeSet;
    // extract nodes from edges
    mySortedNodes.resize(myEdges.size() * 2);
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Extracting nodes from edges..." << std::endl;
#endif
    for (E* edge : myEdges) {
        if ((myHavePermissions && edge->prohibits(vehicle))
            || (myHaveRestrictions && edge->restricts(vehicle))) {
            continue;
        }
        const N* node = edge->getFromJunction();
        typename std::unordered_set<const N*>::const_iterator it = nodeSet.find(node);
        if (it == nodeSet.end()) {
            nodeSet.insert(node);
            assert(edgeCounter < mySortedNodes.size());
            mySortedNodes[edgeCounter++] = node;
        }
        node = edge->getToJunction();
        it = nodeSet.find(node);
        if (it == nodeSet.end()) {
            nodeSet.insert(node);
            assert(edgeCounter < mySortedNodes.size());
            mySortedNodes[edgeCounter++] = node;
        }
    }
    mySortedNodes.shrink_to_fit();
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Nodes extracted from edges." << std::endl;
#endif
    mySortedNodes.resize(edgeCounter);
    myCells.resize(numberOfCells());
    myLevelCells.resize(myNumberOfLevels);
    // call the recursive cell constructor at the root (instantiates the whole k-d tree of cells)
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Calling root cell constructor..." << std::endl;
#endif
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
    myRoot = new Cell(&myCells, &myLevelCells, &mySortedNodes, myNumberOfLevels, 0, Axis::Y, 0, mySortedNodes.size(), nullptr, -1, -1, -1, -1, 
        nullptr, nullptr, nullptr, nullptr, false, vehicle, havePermissions, haveRestrictions);
#else
    myRoot = new Cell(&myCells, &myLevelCells, &mySortedNodes, myNumberOfLevels, 0, Axis::Y, 0, mySortedNodes.size(), nullptr, -1, -1, -1, -1, 
        false, vehicle, myHavePermissions, myHaveRestrictions);
#endif
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Done." << std::endl;
#endif
    assert(myCells[0] == myRoot);
    assert(myRoot->getNumber() == 0);
    // nodes are now sorted wrt to the k-d tree's subdivisional scheme
#ifdef KDTP_DEBUG_LEVEL_0
    const N* node = mySortedNodes[0];
    std::vector<int>* numbers = cellNumbers(node);
    int i;
    for (i = 0; i < myNumberOfLevels; ++i) {
        std::cout << "Cell numbers of test node: " << (*numbers)[i] << std::endl;
    }
    delete numbers;
    for (i = 0; i < myNumberOfLevels; ++i) {
        const std::vector<const Cell*>& levelCells = getCellsAtLevel(i);
        size_t size = levelCells.size();
        std::cout << "Level " << i << " has " << size << " cells." << std::endl;
        std::cout << "The numbers of the cells are: " << std::endl;
        size_t k = 0;
        for (const Cell* cell : levelCells) {
            std::cout << cell->getNumber() << (k++ < size ? ", " : "") << std::endl;
        }
    }
#endif
    myAmClean = false;
}

template<class E, class N, class V>
const typename KDTreePartition<E, N, V>::Cell* KDTreePartition<E, N, V>::cell(int number) const {
    // for now fast enough since the number of cells is usually small
    for (const KDTreePartition<E, N, V>::Cell* cell : myCells) { 
        if (cell->getNumber() == number) {
            return cell;
        }
    }
    return nullptr;
}

template<class E, class N, class V>
std::vector<int>* KDTreePartition<E, N, V>::cellNumbers(const N* node) const {
    std::vector<int>* nodeCellNumbers = new std::vector<int>(myNumberOfLevels);
    int i;
    for (i = 0; i < myNumberOfLevels; ++i) {
        (*nodeCellNumbers)[i] = -1;
    }
    cellNumbersAux(node, myCells[0], 0, nodeCellNumbers);
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "The following entries in nodeCellNumbers should all be set (!=-1): " << std::endl;
    for (i = 0; i < myNumberOfLevels; ++i) {
        assert((*nodeCellNumbers)[i] != -1);
        std::cout << "nodeCellNumbers[" << i << "]:" << (*nodeCellNumbers)[i] << std::endl;
    }
#endif
    return nodeCellNumbers;
}

template<class E, class N, class V>
void KDTreePartition<E, N, V>::cellNumbersAux(const N* node, const Cell* cell, int level, std::vector<int>* nodeCellNumbers) const {
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Call ...aux, level: " << level << ", cell->getAxis(): " << (cell->getAxis() == Axis::X ? "X" : "Y") << std::endl;
#endif
    assert(level < myNumberOfLevels);
#ifdef KDTP_DEBUG_LEVEL_1
    Axis flippedCellAxis = flip(cell->getAxis());
    double nodeAxisValue = flippedCellAxis == Axis::Y ? node->getPosition().y() : node->getPosition().x();
    std::cout << "Flipped axis of cell (=parent cell axis): " << (flippedCellAxis == Axis::X ? "X" : "Y") 
        << ", cell min axis value in flipped axis: " << (flippedCellAxis == Axis::X ? cell->getMinX() : cell->getMinY()) 
        << ", node axis value in flipped axis: " << nodeAxisValue << ", cell max axis value in flipped axis: " 
        << (flippedCellAxis == Axis::X ? cell->getMaxX() : cell->getMaxY()) << std::endl;
#endif
    if (cell->contains(node)) {
        // node is inside the cell, hence cell is one of the cells of node
        (*nodeCellNumbers)[level] = cell->getNumber();
#ifdef KDTP_DEBUG_LEVEL_1
        std::cout << "Just filled nodeCellNumbers[" << level << "]:" << (*nodeCellNumbers)[level] << std::endl;
#endif
        // only one of the below two calls may actually alter array nodeCellNumbers
        const Cell* leftOrLowerSubcell = cell->getLeftOrLowerSubcell();
        if (leftOrLowerSubcell) { // no more calls at shallowest (i.e. leaf) level
            cellNumbersAux(node, leftOrLowerSubcell, level + 1, nodeCellNumbers);
        }
        const Cell* rightOrUpperSubcell = cell->getRightOrUpperSubcell();
        if (rightOrUpperSubcell) {
            cellNumbersAux(node, rightOrUpperSubcell, level + 1, nodeCellNumbers);
        }
    }
#ifdef KDTP_DEBUG_LEVEL_1
    else { // node is not inside the cell, no need to look for node in subcells (consequently, do nothing more, return)
        std::cout << "Not inside cell, do nothing." << std::endl;
    }
#endif
}

template<class E, class N, class V>
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
KDTreePartition<E, N, V>::Cell::Cell(std::vector<const Cell*>* cells, std::vector<std::vector<const Cell*>>* levelCells, std::vector<const N*>* sortedNodes, int numberOfLevels, int level, 
    Axis axis, size_t fromInclusive, size_t toExclusive, Cell* supercell, double minX, double maxX, double minY, double maxY, std::unordered_set<const N*>* northernConflictNodes, 
    std::unordered_set<const N*>* easternConflictNodes, std::unordered_set<const N*>* southernConflictNodes, std::unordered_set<const N*>* westernConflictNodes, const V* const vehicle,
    const bool havePermissions, const bool haveRestrictions) :
    myCells(cells), 
    myLevelCells(levelCells), 
    mySortedNodes(sortedNodes), 
    myNumberOfLevels(numberOfLevels), 
    myLevel(level), myNumber(cellCounter()++), 
    myAxis(axis), 
    myFromInclusive(fromInclusive), 
    myToExclusive(toExclusive), 
    mySupercell(supercell), 
    myMinX(minX), 
    myMaxX(maxX), 
    myMinY(minY), 
    myMaxY(maxY), 
    myNorthernConflictNodes(northernConflictNodes), 
    myEasternConflictNodes(easternConflictNodes), 
    mySouthernConflictNodes(southernConflictNodes), 
    myWesternConflictNodes(westernConflictNodes), 
    myIsLeftOrLowerCell(isLeftOrLowerCell), 
    myHavePermissions(havePermissions), 
    myHaveRestrictions(haveRestrictions),
    myMedianCoordinate(-1.) {
#else
KDTreePartition<E, N, V>::Cell::Cell(std::vector<const Cell*>*cells, std::vector<std::vector<const Cell*>>* levelCells, std::vector<const N*>*sortedNodes, int numberOfLevels, int level, 
    Axis axis, size_t fromInclusive, size_t toExclusive, Cell* supercell, double minX, double maxX, double minY, double maxY, bool isLeftOrLowerCell, const V* const vehicle,
    const bool havePermissions, const bool haveRestrictions) :
    myCells(cells), 
    myLevelCells(levelCells), 
    mySortedNodes(sortedNodes), 
    myNumberOfLevels(numberOfLevels), 
    myLevel(level), 
    myNumber(cellCounter()++), 
    myAxis(axis), 
    myFromInclusive(fromInclusive), 
    myToExclusive(toExclusive), 
    mySupercell(supercell), 
    myMinX(minX), 
    myMaxX(maxX), 
    myMinY(minY), 
    myMaxY(maxY), 
    myIsLeftOrLowerCell(isLeftOrLowerCell),
    myHavePermissions(havePermissions),
    myHaveRestrictions(haveRestrictions),
    myMedianCoordinate(-1.) {
#endif
    // throw invalid argument exception if fromInclusive is greater than or equal to toExclusive
    if (myFromInclusive >= myToExclusive) {
        throw std::invalid_argument("Cell::Cell: myFromInclusive greater than or equal to myToExclusive!");
    }
    myHasCompleteSpatialInfo = false;
    myHasNodesSortedWrtToMyAxis = false;
    (*myCells)[myNumber] = this;
    (*myLevelCells)[myLevel].push_back(this);
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Cell number: " << myNumber << ", cell's extent: minX=" << myMinX << ", maxX=" 
        << myMaxX << ", minY=" << myMinY << ", maxY=" << myMaxY << std::endl;
    std::vector<const N*>* cellNodes = nodes();
    int cnt = 0;
    for (const N* node : *cellNodes) {
        if (++cnt % 10000 == 0) {
            std::cout << "Testing node " << cnt << std::endl;
        }
        assert(contains(node));
    }
    delete cellNodes;
#endif
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    typename std::vector<const N*>::const_iterator iter;
#ifdef KDTP_WRITE_QGIS_FILTERS
    size_t numberOfNodes = myToExclusive - myFromInclusive;
    size_t k = 0;
    std::string qgisFilterString = "id IN (";
    // go through the nodes of the cell
    for (iter = first; iter != last; iter++) {
        k++;
        qgisFilterString += (*iter)->getID() + (k < numberOfNodes ? ", " : "");
    }
    qgisFilterString += ")";
    std::ostringstream pathAndFileName;
    pathAndFileName << "./filter_nodes_of_cell_" << myNumber << ".qqf";
    std::ofstream file;
    file.open(pathAndFileName.str());
    std::ostringstream content;
    content << "<Query>" << qgisFilterString << "</Query>";
    file << content.str();
    file.close();
#endif
    // compute the set of of edges inside the cell
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Computing the set of of edges inside the cell..." << std::endl;
#endif
    std::unordered_set<const E*>* edgeSet = this->edgeSet(vehicle);
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Done. Now collecting boundary edges..." << std::endl;
    int i = 0;
#endif
    /// go through the nodes of the cell in order to identify and collect boundary nodes / edges
    for (iter = first; iter != last; iter++) {
#if defined(KDTP_KEEP_BOUNDARY_EDGES) || defined(KDTP_KEEP_BOUNDARY_NODES) || defined(KDTP_WRITE_QGIS_FILTERS)
        std::unordered_set<const E*> nodeEdgeSet;
#endif
#ifdef KDTP_DEBUG_LEVEL_1
        if ((i++%10000)==0) {
            std::cout << i << " cell nodes processed..." << std::endl;
        }
#endif
#if defined(KDTP_KEEP_BOUNDARY_EDGES) || defined(KDTP_KEEP_BOUNDARY_NODES) || defined(KDTP_WRITE_QGIS_FILTERS)
        const std::vector<const E*>& incomingEdges = (*iter)->getIncoming();
        for (const E* incomingEdge : incomingEdges) {
            if (isProhibited(incomingEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (incomingEdge->isInternal()) {
                continue;
            }
#endif
            nodeEdgeSet.insert(incomingEdge);
        }
        const std::vector<const E*>& outgoingEdges = (*iter)->getOutgoing();
        for (const E* outgoingEdge : outgoingEdges) {
            if (isProhibited(outgoingEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (outgoingEdge->isInternal()) {
                continue;
            }
#endif
            nodeEdgeSet.insert(outgoingEdge);
        }
        bool containsAll = true;
        for (const E* nodeEdge : nodeEdgeSet) {
            if (edgeSet->find(nodeEdge) == edgeSet->end()) {
                containsAll = false;
#ifndef KDTP_KEEP_BOUNDARY_EDGES
                break; // cancel at first element which is not found
#else
                myBoundaryEdges.insert(nodeEdge); // boundary edge!
#endif
            }
        }
#if defined(KDTP_KEEP_BOUNDARY_NODES) || defined(KDTP_WRITE_QGIS_FILTERS)
        if (!containsAll) {
            myBoundaryNodes.push_back(*iter);
        }
#endif
#endif
#if defined(KDTP_KEEP_INCOMING_BOUNDARY_EDGES) || defined(KDTP_KEEP_BOUNDARY_TO_NODES)
        const std::vector<const E*>& incoming = (*iter)->getIncoming();
        std::unordered_set<const N*> boundaryToNodesSet;
        for (const E* nodeEdge : incoming) {
            if (isProhibited(nodeEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (nodeEdge->isInternal()) {
                continue;
            }
#endif
            assert(nodeEdge->getToJunction() == *iter);
            if (edgeSet->find(nodeEdge) == edgeSet->end()) {
#ifdef KDTP_KEEP_INCOMING_BOUNDARY_EDGES
                myIncomingBoundaryEdges.insert(nodeEdge); // incoming boundary edge
#endif
#ifdef KDTP_KEEP_BOUNDARY_TO_NODES
                boundaryToNodesSet.insert(*iter); ; // boundary to-node!
#endif
            }
        }
#ifdef KDTP_KEEP_BOUNDARY_TO_NODES
        std::copy(boundaryToNodesSet.begin(), boundaryToNodesSet.end(),
            std::back_inserter(myBoundaryToNodes));
#endif
#endif
#if defined(KDTP_KEEP_OUTGOING_BOUNDARY_EDGES) || defined(KDTP_KEEP_BOUNDARY_FROM_NODES)
        const std::vector<const E*>& outgoing = (*iter)->getOutgoing();
        std::unordered_set<const N*> boundaryFromNodesSet;
        for (const E* nodeEdge : outgoing) {
            if (isProhibited(nodeEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (nodeEdge->isInternal()) {
                continue;
            }
#endif
            assert(nodeEdge->getFromJunction() == *iter);
            if (edgeSet->find(nodeEdge) == edgeSet->end()) {
#ifdef KDTP_KEEP_OUTGOING_BOUNDARY_EDGES
                myOutgoingBoundaryEdges.insert(nodeEdge); // outgoing boundary edge
#endif
#ifdef KDTP_KEEP_BOUNDARY_FROM_NODES
                boundaryFromNodesSet.insert(*iter); // boundary from-node
#endif
            }
        }
#ifdef KDTP_KEEP_BOUNDARY_FROM_NODES
        std::copy(boundaryFromNodesSet.begin(), boundaryFromNodesSet.end(), 
            std::back_inserter(myBoundaryFromNodes));
#endif
#endif
    }
    delete edgeSet;
#if defined(KDTP_KEEP_BOUNDARY_NODES) || defined(KDTP_WRITE_QGIS_FILTERS)
    size_t numberOfBoundaryNodes = myBoundaryNodes.size();
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Number of boundary nodes: " << numberOfBoundaryNodes << std::endl;
#endif
#endif
#ifdef KDTP_KEEP_BOUNDARY_FROM_NODES
#ifdef KDTP_DEBUG_LEVEL_1
    size_t numberOfBoundaryFromNodes = myBoundaryFromNodes.size();
    std::cout << "Number of boundary from nodes: " << numberOfBoundaryFromNodes << std::endl;
#endif
#endif
#ifdef KDTP_KEEP_BOUNDARY_TO_NODES
    size_t numberOfBoundaryToNodes = myBoundaryToNodes.size();
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Number of boundary to nodes: " << numberOfBoundaryToNodes << std::endl;
#endif
#endif
#ifdef KDTP_KEEP_BOUNDARY_EDGES
    size_t numberOfBoundaryEdges = myBoundaryEdges.size();
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Number of boundary edges: " << numberOfBoundaryEdges << std::endl;
#endif
#endif
#ifdef KDTP_KEEP_INCOMING_BOUNDARY_EDGES
#ifdef KDTP_DEBUG_LEVEL_1
    size_t numberOfIncomingBoundaryEdges = myIncomingBoundaryEdges.size();
    std::cout << "Number of incoming boundary edges: " << numberOfIncomingBoundaryEdges << std::endl;
#endif
#endif
#ifdef KDTP_KEEP_OUTGOING_BOUNDARY_EDGES
#ifdef KDTP_DEBUG_LEVEL_1
    size_t numberOfOutgoingBoundaryEdges = myOutgoingBoundaryEdges.size();
    std::cout << "Number of outgoing boundary edges: " << numberOfOutgoingBoundaryEdges << std::endl;
#endif
#endif
#ifdef KDTP_WRITE_QGIS_FILTERS
    k = 0;
    qgisFilterString.clear();
    qgisFilterString = "id IN (";
    // go through the boundary nodes of the cell
    for (const N* node : myBoundaryNodes) {
        k++;
        qgisFilterString += node->getID() + (k < numberOfBoundaryNodes ? ", " : "");
    }
#ifndef KDTP_KEEP_BOUNDARY_NODES
    myBoundaryNodes.clear();
#endif
    qgisFilterString += ")";
    pathAndFileName.str("");
    pathAndFileName.clear();
    pathAndFileName << "./filter_boundary_nodes_of_cell_" << myNumber << ".qqf";
    file.clear();
    file.open(pathAndFileName.str());
    content.str("");
    content.clear(); 
    content << "<Query>" << qgisFilterString << "</Query>";
    file << content.str();
    file.close();
 #endif
#ifdef KDTP_DEBUG_LEVEL_1
    std::cout << "Done. Now calling the constructor recursively to instantiate the subcells (left or lower one first)..." << std::endl;
#endif
    size_t medianIndex = partition();
    completeSpatialInfo();
    // create subcells
    if (myLevel < myNumberOfLevels - 1) {
        // determine min/max X/Y-values to pass on to left or lower subcell
        double passMinX = -1, passMaxX = -1, passMinY = -1, passMaxY = -1;
        if (myAxis == Axis::X) { // left subcell
            passMinX = myMinX;
            passMaxX = (*mySortedNodes)[medianIndex]->getPosition().x();
            passMinY = myMinY;
            passMaxY = myMaxY;
        }
        else { // lower subcell
            passMinX = myMinX;
            passMaxX = myMaxX;
            passMinY = myMinY;
            passMaxY = (*mySortedNodes)[medianIndex]->getPosition().y();
        }
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
        // notice that nodes with indexes medianIndex and medianIndex+1 may have the same coordinate in the direction of myAxis 
        // (in that case, they are ordered / distributed between the two subcells with a tie-breaking rule)
        // consequently, the spatial extent of cells alone does not suffice to decide whether a node belongs to a certain cell or not
        // therefore, for each of the two subcells, we compute the set of nodes sharing said coordinate (called the set of "spatial conflict nodes")
        // the size of this set is usually very small compared to the size of the set of all nodes of the subcell  
        // with the spatial extent of a cell and its spatial conflict nodes set, a valid test whether a node belongs to the cell becomes available
        // this test is quicker and requires significantly less memory than one based on pre-computed sets of all cell nodes
        std::pair<std::unordered_set<const N*>*, std::unordered_set<const N*>*> conflictNodes = spatialConflictNodes(medianIndex);
        // determine northern/eastern/southern/western conflict nodes to pass on to left or lower subcell
        std::unordered_set<const N*>* passNorthernConflictNodes;
        std::unordered_set<const N*>* passEasternConflictNodes;
        std::unordered_set<const N*>* passSouthernConflictNodes;
        std::unordered_set<const N*>* passWesternConflictNodes;
        if (myAxis == Axis::X) { // left subcell
            passNorthernConflictNodes = myNorthernConflictNodes;
            passEasternConflictNodes = conflictNodes.first;
            passSouthernConflictNodes = mySouthernConflictNodes;
            passWesternConflictNodes = myWesternConflictNodes;
        }
        else { // lower subcell
            passNorthernConflictNodes = conflictNodes.first;
            passEasternConflictNodes = myEasternConflictNodes;
            passSouthernConflictNodes = mySouthernConflictNodes;
            passWesternConflictNodes = myWesternConflictNodes;
        }
        myLeftOrLowerSubcell = new Cell(myCells, myLevelCells, mySortedNodes, myNumberOfLevels, myLevel + 1, KDTreePartition::flip(myAxis), myFromInclusive, medianIndex + 1, this, 
            passMinX, passMaxX, passMinY, passMaxY, passNorthernConflictNodes, passEasternConflictNodes, passSouthernConflictNodes, passWesternConflictNodes, true, 
            vehicle, myHavePermissions, myHaveRestrictions);
#else
        myLeftOrLowerSubcell = new Cell(myCells, myLevelCells, mySortedNodes, myNumberOfLevels, myLevel + 1, KDTreePartition::flip(myAxis), myFromInclusive, medianIndex + 1, this, 
            passMinX, passMaxX, passMinY, passMaxY, true, vehicle, myHavePermissions, myHaveRestrictions);
#endif
#ifdef KDTP_DEBUG_LEVEL_1
        std::cout << "Left or lower call done. Now calling it for the right or upper subcell..." << std::endl;
#endif
        // determine min/max X/Y-values to pass on to right or upper subcell
        passMinX = -1, passMaxX = -1, passMinY = -1, passMaxY = -1;
        if (myAxis == Axis::X) { // right subcell
            passMinX = (*mySortedNodes)[medianIndex+1]->getPosition().x();
            passMaxX = myMaxX;
            passMinY = myMinY;
            passMaxY = myMaxY;
        }
        else { // upper subcell
            passMinX = myMinX;
            passMaxX = myMaxX;
            passMinY = (*mySortedNodes)[medianIndex+1]->getPosition().y();
            passMaxY = myMaxY;
        }
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
        // determine northern/eastern/southern/western conflict nodes to pass on to right or upper subcell
        if (myAxis == Axis::X) { // right subcell
            passNorthernConflictNodes = myNorthernConflictNodes;
            passEasternConflictNodes = myEasternConflictNodes;
            passSouthernConflictNodes = mySouthernConflictNodes;
            passWesternConflictNodes = conflictNodes.second;
        }
        else { // upper subcell
            passNorthernConflictNodes = myNorthernConflictNodes;
            passEasternConflictNodes = myEasternConflictNodes;
            passSouthernConflictNodes = conflictNodes.second;
            passWesternConflictNodes = myWesternConflictNodes;
        }
        myRightOrUpperSubcell = new Cell(myCells, myLevelCells, mySortedNodes, myNumberOfLevels, myLevel + 1, KDTreePartition::flip(myAxis), medianIndex + 1, myToExclusive, this, 
        passMinX, passMaxX, passMinY, passMaxY, passNorthernConflictNodes, passEasternConflictNodes, passSouthernConflictNodes, passWesternConflictNodes, false, 
            vehicle, myHavePermissions, myHaveRestrictions);
#else
        myRightOrUpperSubcell = new Cell(myCells, myLevelCells, mySortedNodes, myNumberOfLevels, myLevel + 1, KDTreePartition::flip(myAxis), medianIndex + 1, myToExclusive, this, 
            passMinX, passMaxX, passMinY, passMaxY, false, vehicle, myHavePermissions, myHaveRestrictions);
#endif
#ifdef KDTP_DEBUG_LEVEL_1
        std::cout << "Right or upper call done. Returning from constructor..." << std::endl;
#endif
    } else {
        // leaves of the k-d tree: subcell pointers equal nullptr
        myLeftOrLowerSubcell = nullptr;
        myRightOrUpperSubcell = nullptr;
    }
} // end of cell constructor

template<class E, class N, class V>
double KDTreePartition<E, N, V>::Cell::minAxisValue(Axis axis) const {
#ifdef KDTP_DEBUG_LEVEL_1
    double returnValue = -1;
#endif
    if (myHasNodesSortedWrtToMyAxis && axis == myAxis) {
#ifndef KDTP_DEBUG_LEVEL_1
        return (myAxis == Axis::Y ? (*mySortedNodes)[myFromInclusive]->getPosition().y() 
            : (*mySortedNodes)[myFromInclusive]->getPosition().x());
#else
        returnValue = (myAxis == Axis::Y ? (*mySortedNodes)[myFromInclusive]->getPosition().y() 
            : (*mySortedNodes)[myFromInclusive]->getPosition().x());
#endif
    }
    double minAxisValue = std::numeric_limits<double>::max();
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    typename std::vector<const N*>::const_iterator iter;
    // go through the nodes of the cell
    for (iter = first; iter != last; iter++) {
        double nodeAxisValue; 
        nodeAxisValue = (axis == Axis::Y ? (*iter)->getPosition().y() : (*iter)->getPosition().x());
        if (nodeAxisValue < minAxisValue) {
            minAxisValue = nodeAxisValue;
        }
    }
    assert(minAxisValue < std::numeric_limits<double>::max());
#ifdef KDTP_DEBUG_LEVEL_1
    assert( (!myHasNodesSortedWrtToMyAxis || axis != myAxis) || minAxisValue == returnValue);
#endif
    return minAxisValue;
}

template<class E, class N, class V>
double KDTreePartition<E, N, V>::Cell::minAxisValue() const {
    return minAxisValue(myAxis);
}

template<class E, class N, class V>
double KDTreePartition<E, N, V>::Cell::maxAxisValue(Axis axis) const {
#ifdef KDTP_DEBUG_LEVEL_1
    double returnValue = -1;
#endif
    if (myHasNodesSortedWrtToMyAxis && axis == myAxis) {
#ifndef KDTP_DEBUG_LEVEL_1
        return (myAxis == Axis::Y ? (*mySortedNodes)[myToExclusive - 1]->getPosition().y() 
            : (*mySortedNodes)[myToExclusive - 1]->getPosition().x());
#else
        returnValue = (myAxis == Axis::Y ? (*mySortedNodes)[myToExclusive - 1]->getPosition().y() 
            : (*mySortedNodes)[myToExclusive - 1]->getPosition().x());
#endif
        
    }
    double maxAxisValue = -1;
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    typename std::vector<const N*>::const_iterator iter;
    // go through the nodes of the cell
    for (iter = first; iter != last; iter++) {
        double nodeAxisValue;
        nodeAxisValue = (axis == Axis::Y ? (*iter)->getPosition().y() : (*iter)->getPosition().x());
        if (nodeAxisValue > maxAxisValue) {
            maxAxisValue = nodeAxisValue;
        }
    }
    assert(maxAxisValue > 0);
#ifdef KDTP_DEBUG_LEVEL_1
    assert((!myHasNodesSortedWrtToMyAxis || axis != myAxis) || maxAxisValue == returnValue);
#endif
    return maxAxisValue;
}

template<class E, class N, class V>
double KDTreePartition<E, N, V>::Cell::maxAxisValue() const
{
    return maxAxisValue(myAxis);
}

template<class E, class N, class V>
size_t KDTreePartition<E, N, V>::Cell::partition() {
    typename std::vector<const N*>::iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::iterator last = mySortedNodes->begin() + myToExclusive;
    if (myAxis == Axis::Y) {
        std::sort(first, last, NodeYComparator());
    } else {
        std::sort(first, last, NodeXComparator());
    }
    myHasNodesSortedWrtToMyAxis = true;
    if (mySupercell) {
        // sorting at one level destroys order at higher levels (but preserves the median split property)
        mySupercell->myHasNodesSortedWrtToMyAxis = false; 
    }
    size_t length = myToExclusive - myFromInclusive;
    size_t medianIndex = myFromInclusive + (length % 2 == 0 ? length / 2 - 1 : (length + 1) / 2 - 1);
#ifndef KDTP_FOR_SYNTHETIC_NETWORKS
    // notice that nodes with indexes medianIndex and medianIndex+1 may have the same coordinate in the direction of myAxis - 
    // in that case, they would be ordered and distributed between the two subcells with a tie-breaking rule, 
    // and the spatial extent of cells alone would not suffice to decide whether a node belongs to a certain cell or not
    // for real-world road networks, this should occur very rarely
    // therefore, a simple but perfectly acceptable remedy is to shift the median index until the nodes with indexes 
    // medianIndex and medianIndex+1 have different coordinates
    // this will distort the 50%/50% distribution of nodes just a little bit (and only under very rare circumstances)
    // hence we can assume zero impact on performance
    // note that this may happen more frequently for e.g. synthetic networks placing nodes at a regular, constant grid. 
    // for this case, a different solution is provided, see code within #ifdef KDTP_FOR_SYNTHETIC_NETWORKS ... #endif
    const N* medianNode = (*mySortedNodes)[medianIndex];
    const N* afterMedianNode = (*mySortedNodes)[medianIndex+1];
    double medianNodeCoordinate = myAxis == Axis::X ? medianNode->getPosition().x() 
        : medianNode->getPosition().y();
    double afterMedianNodeCoordinate = myAxis == Axis::X ? afterMedianNode->getPosition().x() 
        : afterMedianNode->getPosition().y();
    while (medianNodeCoordinate == afterMedianNodeCoordinate && medianIndex<myToExclusive-3) {
#ifdef KDTP_DEBUG_LEVEL_2
        std::cout << "Found spatially conflicting nodes." << std::endl; 
#endif
        medianIndex++;
        medianNode = (*mySortedNodes)[medianIndex];
        afterMedianNode = (*mySortedNodes)[medianIndex + 1];
        medianNodeCoordinate = myAxis == Axis::X ? medianNode->getPosition().x() 
            : medianNode->getPosition().y();
        afterMedianNodeCoordinate = myAxis == Axis::X ? afterMedianNode->getPosition().x() 
            : afterMedianNode->getPosition().y();
    }
#endif
    myMedianCoordinate = medianNodeCoordinate;
    return medianIndex;
}

template<class E, class N, class V>
void KDTreePartition<E, N, V>::Cell::completeSpatialInfo() {
    // complete missing information about the cell's spatial extent
    if (myMinX < 0) {
        myMinX = minAxisValue(Axis::X);
    }
    if (myMaxX < 0) {
        myMaxX = maxAxisValue(Axis::X);
    }
    if (myMinY < 0) {
        myMinY = minAxisValue(Axis::Y);
    }
    if (myMaxY < 0) {
        myMaxY = maxAxisValue(Axis::Y);
    }
    myHasCompleteSpatialInfo = true;
}

template<class E, class N, class V>
std::unordered_set<const E*>* KDTreePartition<E, N, V>::Cell::edgeSet(const V* const vehicle) const {
    std::unordered_set<const E*>* edgeSet = new std::unordered_set<const E*>();
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    typename std::vector<const N*>::const_iterator iter;
    for (iter = first; iter != last; iter++) {
        const N* edgeNode;
        const std::vector<const E*>& incomingEdges = (*iter)->getIncoming();
        for (const E* incomingEdge : incomingEdges) {
            if (isProhibited(incomingEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (incomingEdge->isInternal()) {
                continue;
            }
#endif
            edgeNode = incomingEdge->getFromJunction();
            if (contains(edgeNode)) {
                edgeSet->insert(incomingEdge);
            }
        }
        const std::vector<const E*>& outgoingEdges = (*iter)->getOutgoing();
        for (const E* outgoingEdge : outgoingEdges) {
            if (isProhibited(outgoingEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (outgoingEdge->isInternal()) {
                continue;
            }
#endif
            edgeNode = outgoingEdge->getToJunction();
            if (contains(edgeNode)) {
                edgeSet->insert(outgoingEdge);
            }
        }
    }
    return edgeSet;
}

template<class E, class N, class V>
size_t KDTreePartition<E, N, V>::Cell::numberOfEdgesEndingInCell(const V* const vehicle) const {
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    typename std::vector<const N*>::const_iterator iter;
    size_t edgeCounter = 0;
    for (iter = first; iter != last; iter++) {
        const std::vector<const E*>& incomingEdges = (*iter)->getIncoming();
        for (const E* incomingEdge : incomingEdges) {
            if (isProhibited(incomingEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
            if (incomingEdge->isInternal()) {
                continue;
            }
            else {
                edgeCounter++;
            }
#endif
        }
    }
    return edgeCounter;
}

template<class E, class N, class V>
size_t KDTreePartition<E, N, V>::Cell::numberOfEdgesStartingInCell(const V* const vehicle) const {
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    typename std::vector<const N*>::const_iterator iter;
    size_t edgeCounter = 0;
    for (iter = first; iter != last; iter++) {
        const std::vector<const E*>& outgoingEdges = (*iter)->getOutgoing();
        for (const E* outgoingEdge : outgoingEdges) {
            if (isProhibited(outgoingEdge, vehicle)) {
                continue;
            }
#ifdef KDTP_EXCLUDE_INTERNAL_EDGES
           if (outgoingEdge->isInternal()) {
                continue;
           }
           else {
               edgeCounter++;
           }
        }
#endif
    }
    return edgeCounter;
}

template<class E, class N, class V>
std::pair<typename std::vector<const N*>::const_iterator, 
    typename std::vector<const N*>::const_iterator> KDTreePartition<E, N, V>::Cell::nodeIterators() const {
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    std::pair<typename std::vector<const N*>::const_iterator, 
        typename std::vector<const N*>::const_iterator> iterators = std::make_pair(first, last);
    return iterators;
}

template<class E, class N, class V>
std::vector<const N*>* KDTreePartition<E, N, V>::Cell::nodes() const {
    typename std::vector<const N*>::const_iterator first = mySortedNodes->begin() + myFromInclusive;
    typename std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
    std::vector<const N*>* nodes = new std::vector<const N*>(first, last);
    return nodes;
}

#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
template<class E, class N, class V>
std::pair<std::unordered_set<const N*>*, 
    std::unordered_set<const N*>*> KDTreePartition<E, N, V>::Cell::spatialConflictNodes(size_t medianIndex) const {
    std::unordered_set<const N*>* leftOrLowerSpatialConflictNodes = new std::unordered_set<const N*>();
    std::unordered_set<const N*>* rightOrUpperSpatialConflictNodes = new std::unordered_set<const N*>();
    std::pair<std::unordered_set<const N*>*, std::unordered_set<const N*>*> spatialConflictNodes 
        = std::make_pair(leftOrLowerSpatialConflictNodes, rightOrUpperSpatialConflictNodes);
    std::vector<const N*>::const_iterator iter, prev;
    if (myAxis == Axis::X) {
        if ((*mySortedNodes)[medianIndex]->getPosition().x() == (*mySortedNodes)[medianIndex + 1]->getPosition().x()) {
            double sharedCoordinate = (*mySortedNodes)[medianIndex]->getPosition().x();
            std::vector<const N*>::iterator firstLeftOrLower = mySortedNodes->begin() + medianIndex;
            // this is last since we go backward
            std::vector<const N*>::iterator last = mySortedNodes->begin() + myFromInclusive - 1;
            for (iter = firstLeftOrLower; ((*iter)->getPosition().x() == sharedCoordinate) && (iter != last); iter--) {
                leftOrLowerSpatialConflictNodes->insert(*iter);
            }
        }
    } else {
        if ((*mySortedNodes)[medianIndex]->getPosition().y() == (*mySortedNodes)[medianIndex + 1]->getPosition().y()) {
            double sharedCoordinate = (*mySortedNodes)[medianIndex]->getPosition().y();
            std::vector<const N*>::iterator firstRightOrUpper = mySortedNodes->begin() + medianIndex + 1;
            std::vector<const N*>::const_iterator last = mySortedNodes->begin() + myToExclusive;
            for (iter = firstRightOrUpper; ((*iter)->getPosition().y() == sharedCoordinate) && (iter != last); iter++) {
                rightOrUpperSpatialConflictNodes->insert(*iter);
            }
        }
    }
    return spatialConflictNodes;
}
#endif

template<class E, class N, class V>
bool KDTreePartition<E, N, V>::Cell::isInBounds(const N* node) const {
    double nodeX = node->getPosition().x();
    double nodeY = node->getPosition().y();
    if (nodeX < myMinX || nodeX > myMaxX || nodeY < myMinY || nodeY > myMaxY) {
        return false;
    }
    return true;
}

template<class E, class N, class V>
bool KDTreePartition<E, N, V>::Cell::contains(const N* node) const
{
    if (myLevel == 0) { // p.d., the root cell contains all nodes
        return true;
    }
    if (!isInBounds(node)) {
#ifdef KDTP_DEBUG_LEVEL_2
        std::cout << "Not in bounds, nX: " << node->getPosition().x() << ", nY: " << node->getPosition().y() << std::endl;
#endif
        return false;
    }
#ifdef KDTP_DEBUG_LEVEL_2
    else {
        std::cout << "Node is in bounds!" << std::endl;
    }
#endif
#ifdef KDTP_FOR_SYNTHETIC_NETWORKS
    //
    double nodeX = node->getPosition().x();
    double nodeY = node->getPosition().y();
#ifdef KDTP_DEBUG_LEVEL_2
    std::cout << "Entered contains, nodeX: " << nodeX << ", nodeY: " << nodeY << std::endl;
#endif
    double northY = -1;
    if (myNorthernConflictNodes  && !myNorthernConflictNodes->empty()) {
        northY = (*(myNorthernConflictNodes->begin()))->getPosition().y();
    }
    double eastX = -1;
    if (myEasternConflictNodes && !myEasternConflictNodes->empty()) {
        eastX = (*(myEasternConflictNodes->begin()))->getPosition().x();
    }
    double southY = -1;
    if (mySouthernConflictNodes && !mySouthernConflictNodes->empty()) {
        southY = (*(mySouthernConflictNodes->begin()))->getPosition().y();
    }
    double westX = -1;
    if (myWesternConflictNodes && !myWesternConflictNodes->empty()) {
        westX = (*(myWesternConflictNodes->begin()))->getPosition().x();
    }
#ifdef KDTP_DEBUG_LEVEL_2
    std::cout << "northY: " << northY << ", eastX: " << eastX << ", southY: " << southY << ", westX: " << westX << std::endl;
#endif
    if (nodeX == eastX) { // we have to test the eastern conflict nodes
#ifdef KDTP_DEBUG_LEVEL_2
        std::cout << "On eastern bound, nX: " << node->getPosition().x() << ", nY: " << node->getPosition().y() << std::endl;
#endif
        return myEasternConflictNodes->find(node) != myEasternConflictNodes->end();
    }
    if (nodeX == westX) { // we have to test the western conflict nodes
#ifdef KDTP_DEBUG_LEVEL_2
        std::cout << "On western bound, nX: " << node->getPosition().x() << ", nY: " << node->getPosition().y() << std::endl;
#endif
        return myWesternConflictNodes->find(node) != myWesternConflictNodes->end();
    }
    if (nodeY == northY) { // we have to test the northern conflict nodes
#ifdef KDTP_DEBUG_LEVEL_2
        std::cout << "On northern bound, nX: " << node->getPosition().x() << ", nY: " << node->getPosition().y() << std::endl;
#endif
        return myNorthernConflictNodes->find(node) != myNorthernConflictNodes->end();
    }
    if (nodeY == southY) { // we have to test the southern conflict nodes
#ifdef KDTP_DEBUG_LEVEL_2
        std::cout << "On southern bound, nX: " << node->getPosition().x() << ", nY: " << node->getPosition().y() << std::endl;
#endif
        return mySouthernConflictNodes->find(node) != myEasternConflictNodes->end();
    }
#endif
#ifdef KDTP_DEBUG_LEVEL_2
    std::cout << "Node does not lie on a border!" << std::endl;
#endif
    return true; // if the node does not lie on a spatial border, the passed in-bounds test suffices
}

template<class E, class N, class V>
const typename KDTreePartition<E, N, V>::Cell* KDTreePartition<E, N, V>::searchNode(const N* node) const
{
    const typename KDTreePartition<E, N, V>::Cell* cell = myRoot;
    if (!myRoot->contains(node)) {
        return nullptr;
    }
    while (true) {
        assert(cell);
        double nodeCoordinate = cell->getAxis() == Axis::X ? node->getPosition().x() : node->getPosition().y();
        const typename KDTreePartition<E, N, V>::Cell* nextCell = nodeCoordinate <= cell->getMedianCoordinate() ?
            cell->getLeftOrLowerSubcell() : cell->getRightOrUpperSubcell();
        if (nextCell == nullptr) {
            return cell;
        }
        else {
            cell = nextCell;
        }
    }
}
