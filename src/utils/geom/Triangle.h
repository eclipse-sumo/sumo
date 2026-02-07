/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    Triangle.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2025
///
// A simple triangle defined in 3D
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>

#include "PositionVector.h"
#include "Boundary.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Triangle
 * @brief A simple triangle defined in 3D
 */
class Triangle {

public:
    /// @brief invalid triangle
    static const Triangle INVALID;

    /// @brief default constructor
    Triangle();

    /// @brief parameter constructor
    Triangle(const Position& positionA, const Position& positionB, const Position& positionC);

    /// @brief destructor
    ~Triangle();

    /// @brief get triangle boundary
    const Boundary& getBoundary() const;

    /// @brief get shape boundary
    const PositionVector getShape() const;

    /// @brief check if the given position is within this triangle
    bool isPositionWithin(const Position& pos) const;

    /// @brief check if the given position is FULL within this triangle
    bool isBoundaryFullWithin(const Boundary& boundary) const;

    /// @brief check if the given shape is within or intersect with this triangle
    bool intersectWithShape(const PositionVector& shape) const;

    /// @brief check if the given shape is within or intersect with this triangle
    bool intersectWithShape(const PositionVector& shape, const Boundary& shapeBoundary) const;

    /// @brief check if the given circle intersect with this triangle
    bool intersectWithCircle(const Position& center, const double radius) const;

    // @brief triangulate using Ear Clipping algorithm
    static std::vector<Triangle> triangulate(PositionVector shape);

    /// @brief equalityoperators
    bool operator==(const Triangle& other) const;

    /// @brief inequality operator
    bool operator!=(const Triangle& other) const;

private:
    /// @name functions used for triangulation
    /// @{
    /// @brief check if the given position is within this triangle
    static bool isPositionWithin(const Position& A, const Position& B, const Position& C, const Position& pos);

    /// @brief Check if the triangle (A, B, C) is an ear
    static bool isEar(const Position& a, const Position& b, const Position& c, const PositionVector& shape);

    /// @brief calculate cross product of the given points
    static double crossProduct(const Position& a, const Position& b, const Position& c);

    /// @}

    /// @name functions used for check if a shape intersect with the triangle
    /// @{
    /// @brief Compute the orientation of ordered triplet (p, q, r)
    int orientation(const Position& p, const Position& q, const Position& r) const;

    /// @brief check if point q lies on segment pr
    bool onSegment(const Position& p, const Position& q, const Position& r) const;

    /// @brief check if two line segments (p1,q1) and (p2,q2) intersect
    bool segmentsIntersect(const Position& p1, const Position& q1, const Position& p2, const Position& q2) const;

    /// @brief check if a line segment (p1, p2) intersects this triangle
    bool lineIntersectsTriangle(const Position& p1, const Position& p2) const;

    /// @}

    /// @brief function to check if line between posA and posB intersect circle
    bool lineIntersectCircle(const Position& posA, const Position& posB, const Position& center, const double radius) const;

    /// @brief first triangle position
    Position myA = Position::INVALID;

    /// @brief second triangle position
    Position myB = Position::INVALID;

    /// @brief third triangle position
    Position myC = Position::INVALID;

    /// @brief triangle boundary
    Boundary myBoundary;

    /// @brief epsilon used for certain comparations
    static const double EPSILON;
};
