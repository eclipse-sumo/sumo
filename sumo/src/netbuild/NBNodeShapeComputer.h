/****************************************************************************/
/// @file    NBNodeShapeComputer.h
/// @author  Daniel Krajzewicz
/// @date    2004-01-12
/// @version $Id$
///
// This class computes shapes of junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <utils/geom/Position2DVector.h>


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
    NBNodeShapeComputer(const NBNode &node);

    /// Destructor
    ~NBNodeShapeComputer();

    /// Computes the shape of the assigned junction
    Position2DVector compute(bool leftHand);

private:
    Position2DVector computeContinuationNodeShape(bool simpleContinuation);

    /** @brief Computes the node geometry using normals
     *
     * In the case the other method does not work, this method computes the geometry
     *  of a node by adding points to the polygon which are computed by building
     *  the normals of participating edges' geometry boundaries (cw/ccw)
     *  at the node's height (the length of the edge the edge would cross the node
     *  point).
     */
    Position2DVector computeNodeShapeByCrosses();


    void replaceLastChecking(Position2DVector &g, bool decenter,
                             Position2DVector counter, size_t counterLanes, SUMOReal counterDist,
                             int laneDiff);


    void replaceFirstChecking(Position2DVector &g, bool decenter,
                              Position2DVector counter, size_t counterLanes, SUMOReal counterDist,
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
    void joinSameDirectionEdges(std::map<NBEdge*, std::vector<NBEdge*> > &same,
                                std::map<NBEdge*, Position2DVector> &geomsCCW,
                                std::map<NBEdge*, Position2DVector> &geomsCW);

    /** @brief Joins edges and computes ccw/cw boundaries
     *
     * This methods joins edges which are in marked as being "same" in the means
     *  as given by joinSameDirectionEdges. The result (list of so-to-say "directions"
     *  is returned; additionally, the boundaries of these directions are stored in
     *  ccwBoundary/cwBoundary.
     */
    std::vector<NBEdge*> computeUniqueDirectionList(
        const std::map<NBEdge*, std::vector<NBEdge*> > &same,
        std::map<NBEdge*, Position2DVector> &geomsCCW,
        std::map<NBEdge*, Position2DVector> &geomsCW,
        std::map<NBEdge*, NBEdge*> &ccwBoundary,
        std::map<NBEdge*, NBEdge*> &cwBoundary);


private:
    /// The node to compute the geometry for
    const NBNode &myNode;

};


#endif

/****************************************************************************/

