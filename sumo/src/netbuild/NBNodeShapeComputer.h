/****************************************************************************/
/// @file    NBNodeShapeComputer.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2004-01-12
/// @version $Id$
///
// This class computes shapes of junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBNodeShapeComputer_h
#define NBNodeShapeComputer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/PositionVector.h>


// ===========================================================================
// class definitions
// ===========================================================================
class NBNode;
class NBEdge;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class NBNodeShapeComputer
 * @brief This class computes shapes of junctions
 */
class NBNodeShapeComputer {
public:
    /// Constructor
    NBNodeShapeComputer(const NBNode& node);

    /// Destructor
    ~NBNodeShapeComputer();

    /// Computes the shape of the assigned junction
    PositionVector compute(bool leftHand);

private:
    /** @brief Computes the node geometry
     * Edges with the same direction are grouped.
     * Then the node geometry is built from intersection between the borders
     * of adjacent edge groups
     */
    PositionVector computeNodeShapeDefault(bool simpleContinuation);

    /** @brief Computes the node geometry using normals
     *
     * In the case the other method does not work, this method computes the geometry
     *  of a node by adding points to the polygon which are computed by building
     *  the normals of participating edges' geometry boundaries (cw/ccw)
     *  at the node's height (the length of the edge the edge would cross the node
     *  point).
     *
     *  @note This usually gives a very small node shape, appropriate for
     *  dead-ends or turn-around-only situations
     */
    PositionVector computeNodeShapeSmall();


    void replaceLastChecking(PositionVector& g, bool decenter,
                             PositionVector counter, size_t counterLanes, SUMOReal counterDist,
                             int laneDiff);


    void replaceFirstChecking(PositionVector& g, bool decenter,
                              PositionVector counter, size_t counterLanes, SUMOReal counterDist,
                              int laneDiff);

    /** @brief Joins edges and computes ccw/cw boundaries
     *
     * This method goes through all edges and stores each edge's ccw and cw
     *  boundary in geomsCCW/geomsCW. This boundary is extrapolated by 100m
     *  at the node's position.
     * In addition, "same" is filled so that this map contains a list of
     *  all edges within the value-vector which direction at the node differs
     *  less than 1 from the key-edge's direction.
     */
    void joinSameDirectionEdges(std::map<NBEdge*, EdgeVector >& same,
                                std::map<NBEdge*, PositionVector>& geomsCCW,
                                std::map<NBEdge*, PositionVector>& geomsCW);

    /** @brief Joins edges and computes ccw/cw boundaries
     *
     * This methods joins edges which are in marked as being "same" in the means
     *  as given by joinSameDirectionEdges. The result (list of so-to-say "directions"
     *  is returned; additionally, the boundaries of these directions are stored in
     *  ccwBoundary/cwBoundary.
     */
    EdgeVector computeUniqueDirectionList(
        const std::map<NBEdge*, EdgeVector >& same,
        std::map<NBEdge*, PositionVector>& geomsCCW,
        std::map<NBEdge*, PositionVector>& geomsCW,
        std::map<NBEdge*, NBEdge*>& ccwBoundary,
        std::map<NBEdge*, NBEdge*>& cwBoundary);


private:
    /// The node to compute the geometry for
    const NBNode& myNode;

private:
    /// @brief Invalidated assignment operator
    NBNodeShapeComputer& operator=(const NBNodeShapeComputer& s);

};

#endif

/****************************************************************************/

