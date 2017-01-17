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
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
    PositionVector compute();

private:
    typedef std::map<NBEdge*, PositionVector> GeomsMap;

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


    /** @brief Joins edges and computes ccw/cw boundaries
     *
     * This method goes through all edges and stores each edge's ccw and cw
     *  boundary in geomsCCW/geomsCW. This boundary is extrapolated by 100m
     *  at the node's position.
     * In addition, "same" is filled so that this map contains a list of
     *  all edges within the value-vector which direction at the node differs
     *  less than 1 from the key-edge's direction.
     */
    void joinSameDirectionEdges(std::map<NBEdge*, std::set<NBEdge*> >& same,
                                GeomsMap& geomsCCW,
                                GeomsMap& geomsCW);

    /** @brief Joins edges and computes ccw/cw boundaries
     *
     * This methods joins edges which are in marked as being "same" in the means
     *  as given by joinSameDirectionEdges. The result (list of so-to-say "directions"
     *  is returned; additionally, the boundaries of these directions are stored in
     *  ccwBoundary/cwBoundary.
     */
    EdgeVector computeUniqueDirectionList(
        std::map<NBEdge*, std::set<NBEdge*> >& same,
        GeomsMap& geomsCCW,
        GeomsMap& geomsCW);

    /** @brief Compute smoothed corner shape
     * @param[in] begShape
     * @param[in] endShape
     * @param[in] begPoint
     * @param[in] endPoint
     * @param[in] cornerDetail
     * @return shape to be appended between begPoint and endPoint
     */
    PositionVector getSmoothCorner(PositionVector begShape, PositionVector endShape,
                                   const Position& begPoint, const Position& endPoint, int cornerDetail);

    /** @brief Initialize neighbors and angles
     * @param[in] edges The list of edges sorted in clockwise direction
     * @param[in] current An iterator to the current edge
     * @param[in] geomsCW geometry map
     * @param[in] geomsCCW geometry map
     * @param[out] cwi An iterator to the clockwise neighbor
     * @param[out] ccwi An iterator to the counter-clockwise neighbor
     * @param[out] cad The angle difference to the clockwise neighbor
     * @param[out] ccad The angle difference to the counter-clockwise neighbor
     */
    static void initNeighbors(const EdgeVector& edges, const EdgeVector::const_iterator& current,
                              GeomsMap& geomsCW,
                              GeomsMap& geomsCCW,
                              EdgeVector::const_iterator& cwi,
                              EdgeVector::const_iterator& ccwi,
                              SUMOReal& cad,
                              SUMOReal& ccad);

    /// @return whether trying to intersect these edges would probably fail
    bool badIntersection(const NBEdge* e1, const NBEdge* e2,
                         const PositionVector& e1cw, const PositionVector& e2ccw, SUMOReal distance);

    /// @brief return the intersection point closest to the given offset
    SUMOReal closestIntersection(const PositionVector& geom1, const PositionVector& geom2, SUMOReal offset);

private:
    /// The node to compute the geometry for
    const NBNode& myNode;

private:
    /// @brief Invalidated assignment operator
    NBNodeShapeComputer& operator=(const NBNodeShapeComputer& s);

};

#endif

/****************************************************************************/

