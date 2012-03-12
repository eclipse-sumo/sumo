/****************************************************************************/
/// @file    NBAlgorithms.h
/// @author  Daniel Krajzewicz
/// @date    02. March 2012
/// @version $Id$
///
// Algorithms for network computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBAlgorithms_h
#define NBAlgorithms_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBNodeCont;
class NBTypeCont;

// ===========================================================================
// class definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBTurningDirectionsComputer
// ---------------------------------------------------------------------------
/* @class NBTurningDirectionsComputer
 * @brief Computes turnaround destinations for all edges (if exist)
 */
class NBTurningDirectionsComputer {
public:
    /** @brief Computes turnaround destinations for all edges (if exist)
     * @param[in] nc The container of nodes to loop along
     */
    static void computeTurnDirections(NBNodeCont &nc);

    /** @brief Computes turnaround destinations for all incoming edges of the given nodes (if any)
     * @param[in] node The node for which to compute turnaround destinations
     * @note: This is needed by NETEDIT
     */
    static void computeTurnDirectionsForNode(NBNode* node);

private:
    /** @struct Combination
     * @brief Stores the information about the angle between an incoming ("from") and an outgoing ("to") edge
     *
     * Note that the angle is increased by 360 if the edges connect the same two nodes in
     *  opposite direction.
     */
    struct Combination {
        NBEdge *from;
        NBEdge *to;
        SUMOReal angle;
    };


    /** @class combination_by_angle_sorter
     * @brief Sorts "Combination"s by decreasing angle
     */
    class combination_by_angle_sorter {
    public:
        explicit combination_by_angle_sorter() { }
        int operator()(const Combination& c1, const Combination& c2) const {
            if (c1.angle!=c2.angle) {
                return c1.angle > c2.angle;
            }
            if (c1.from!=c2.from) {
                return c1.from->getID() < c2.from->getID();
            }
            return c1.to->getID() < c2.to->getID();
        }
    };
};



// ---------------------------------------------------------------------------
// NBNodesEdgesSorter
// ---------------------------------------------------------------------------
/* @class NBNodesEdgesSorter
 * @brief Sorts a node's edges clockwise regarding driving direction
 */
class NBNodesEdgesSorter {
public:
    /** @brief Sorts a node's edges clockwise regarding driving direction
     * @param[in] nc The container of nodes to loop along
     * @param[in] leftHand Whether the network is left-handed
     */
    static void sortNodesEdges(NBNodeCont &nc, bool leftHand);

private:
    /** @brief Assures correct order for same-angle opposite-direction edges
     * @param[in] n The currently processed node
     * @param[in] leftHand Whether the network is left-handed
     * @param[in] i1 Pointer to first edge
     * @param[in] i2 Pointer to second edge
     */
    static void swapWhenReversed(const NBNode * const n, bool leftHand,
                          const std::vector<NBEdge*>::iterator& i1,
                          const std::vector<NBEdge*>::iterator& i2);


    /** @class edge_by_junction_angle_sorter
     * @brief Sorts incoming and outgoing edges clockwise around the given node
     */
    class edge_by_junction_angle_sorter {
    public:
        explicit edge_by_junction_angle_sorter(NBNode* n) : myNode(n) {}
        int operator()(NBEdge* e1, NBEdge* e2) const {
            return getConvAngle(e1) < getConvAngle(e2);
        }

    private:
        /// @brief Converts the angle of the edge if it is an incoming edge
        SUMOReal getConvAngle(NBEdge* e) const {
            SUMOReal angle = e->getAngleAtNode(myNode);
            if (angle < 0.) {
                angle = 360. + angle;
            }
            // convert angle if the edge is an outgoing edge
            if (e->getFromNode() == myNode) {
                angle += (SUMOReal) 180.;
                if (angle >= (SUMOReal) 360.) {
                    angle -= (SUMOReal) 360.;
                }
            }
            if (angle < 0.1 || angle > 359.9) {
                angle = (SUMOReal) 0.;
            }
            assert(angle >= (SUMOReal)0 && angle < (SUMOReal)360);
            return angle;
        }

    private:
        /// @brief The node to compute the relative angle of
        NBNode* myNode;

    };
};



// ---------------------------------------------------------------------------
// NBNodeTypeComputer
// ---------------------------------------------------------------------------
/* @class NBNodeTypeComputer
 * @brief Computes node types
 */
class NBNodeTypeComputer {
public:
    /** @brief Computes node types
     * @param[in] nc The container of nodes to loop along
     */
    static void computeNodeTypes(NBNodeCont &nc);

private:
    /**
     * @class JunctionTypesMatrix
     * @brief A class that stores the relationship between incoming edges and the junction type resulting from their types.
     */
    class JunctionTypesMatrix {
    public:
        /// @brief Constructor
        JunctionTypesMatrix();

        /// @brief Destructor
        ~JunctionTypesMatrix();

        /** @brief returns the type of the junction on the crossing of edges of the given types
         * @param[in] speed1 The first edge's speed
         * @param[in] speed2 The second edge's speed
         * @return The resulting intersection type
         */
        SumoXMLNodeType getType(SUMOReal speed1, SUMOReal speed2) const;


private:
        /** @brief returns the one-char name of the junction type between the two given ranges */
        char getNameAt(size_t pos1, size_t pos2) const;

        /** @class priority_finder
         * @brief Searches for the named priority in the range container
         */
        class range_finder {
        public:
            explicit range_finder(SUMOReal speed) : mySpeed(speed) { }

            bool operator()(const std::pair<SUMOReal, SUMOReal> &range) {
                return mySpeed >= range.first && mySpeed < range.second;
            }

        private:
            SUMOReal mySpeed;
        };

    private:
        /// @brief A container type for the resulting junction types (cross matrix)
        typedef std::vector<std::string> StringCont;

        /// @brief A map of chars to ints
        typedef std::map<char, SumoXMLNodeType> CharToVal;

        /// @brief A container for edge priority ranges
        std::vector<std::pair<SUMOReal, SUMOReal> > myRanges;

        /** @brief A container for the resulting junction types (cross matrix)
         *
         * The informations are stored as chars:
         * 't': Traffic Light Junction
         * 'r': Right-before-Left Junction
         * 'p': Priority Junction
         * 'x': no Junction */
        StringCont myValues;

        /// @brief A map of chars to their NBNode-representation
        CharToVal  myMap;

    };
};


#endif

/****************************************************************************/

