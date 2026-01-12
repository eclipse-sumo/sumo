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
/// @file    Triangle.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2025
///
// A simple triangle defined in 3D
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include "Triangle.h"


// ===========================================================================
// static member definitions
// ===========================================================================
const Triangle Triangle::INVALID = Triangle();


// ===========================================================================
// method definitions
// ===========================================================================
Triangle::Triangle() {}


Triangle::Triangle(const Position& positionA, const Position& positionB, const Position& positionC) :
    myA(positionA),
    myB(positionB),
    myC(positionC) {
    // calculate boundary
    myBoundary.add(positionA);
    myBoundary.add(positionB);
    myBoundary.add(positionC);
}


Triangle::~Triangle() {}


bool
Triangle::isPositionWithin(const Position& pos) const {
    return isPositionWithin(myA, myB, myC, pos);
}


bool
Triangle::isBoundaryFullWithin(const Boundary& boundary) const {
    return isPositionWithin(Position(boundary.xmax(), boundary.ymax())) &&
           isPositionWithin(Position(boundary.xmin(), boundary.ymin())) &&
           isPositionWithin(Position(boundary.xmax(), boundary.ymin())) &&
           isPositionWithin(Position(boundary.xmin(), boundary.ymax()));
}


bool
Triangle::intersectWithShape(const PositionVector& shape) const {
    return intersectWithShape(shape, shape.getBoxBoundary());
}


bool
Triangle::intersectWithShape(const PositionVector& shape, const Boundary& shapeBoundary) const {
    // Check if the triangle's vertices are within the shape
    if (shape.around(myA) || shape.around(myB) || shape.around(myC)) {
        return true;
    }
    // Check if at least two corners of the shape boundary are within the triangle.
    // (This acts as a heuristic to detect overlap without checking every edge first)
    int cornersInside = 0;
    if (isPositionWithin(Position(shapeBoundary.xmax(), shapeBoundary.ymax()))) {
        cornersInside++;
    }
    if (isPositionWithin(Position(shapeBoundary.xmin(), shapeBoundary.ymin()))) {
        cornersInside++;
    }
    if ((cornersInside < 2) && isPositionWithin(Position(shapeBoundary.xmax(), shapeBoundary.ymin()))) {
        cornersInside++;
    }
    if ((cornersInside < 2) && isPositionWithin(Position(shapeBoundary.xmin(), shapeBoundary.ymax()))) {
        cornersInside++;
    }
    if (cornersInside >= 2) {
        return true;
    }
    // At this point, we need to check if any line of the shape intersects with the triangle.
    // We treat the shape as a closed polygon.
    const int shapeSize = (int)shape.size();
    for (int i = 0; i < shapeSize; i++) {
        const Position& p1 = shape[i];
        // Wrap around to the first point
        const Position& p2 = shape[(i + 1) % shapeSize];
        // Avoid checking a zero-length segment if the shape is already explicitly closed in data
        if (p1 == p2) {
            continue;
        } else if (lineIntersectsTriangle(p1, p2)) {
            return true;
        }
    }
    return false;
}


bool
Triangle::intersectWithCircle(const Position& center, const double radius) const {
    const auto squaredRadius = radius * radius;
    return ((center.distanceSquaredTo2D(myA) <= squaredRadius) ||
            (center.distanceSquaredTo2D(myB) <= squaredRadius) ||
            (center.distanceSquaredTo2D(myC) <= squaredRadius) ||
            isPositionWithin(center) ||
            lineIntersectCircle(myA, myB, center, radius) ||
            lineIntersectCircle(myB, myC, center, radius) ||
            lineIntersectCircle(myC, myA, center, radius));
}


const Boundary&
Triangle::getBoundary() const {
    return myBoundary;
}


const PositionVector
Triangle::getShape() const {
    return PositionVector({myA, myB, myC});
}


std::vector<Triangle>
Triangle::triangulate(PositionVector shape) {
    std::vector<Triangle> triangles;
    // first open polygon
    shape.openPolygon();
    // we need at leas three vertex
    if (shape.size() >= 3) {
        // greedy algorithm
        while (shape.size() > 3) {
            int shapeSize = (int)shape.size();
            int earIndex = -1;
            // first find an "ear"
            for (int i = 0; (i < shapeSize) && (earIndex == -1); i++) {
                const auto& earA = shape[(i + shapeSize - 1) % shapeSize];
                const auto& earB = shape[i];
                const auto& earC = shape[(i + 1) % shapeSize];
                if (isEar(earA, earB, earC, shape)) {
                    earIndex = i;
                }
            }
            if (earIndex != -1) {
                triangles.push_back(Triangle(
                                        shape[(earIndex + shapeSize - 1) % shapeSize],
                                        shape[earIndex],
                                        shape[(earIndex + 1) % shapeSize])
                                   );
                shape.erase(shape.begin() + earIndex);
            } else {
                // simply remove the first three
                triangles.push_back(Triangle(shape[0], shape[1], shape[2]));
                shape.erase(shape.begin() + 1);
            }
        }
        // add last triangle
        triangles.push_back(Triangle(shape[0], shape[1], shape[2]));
    }
    return triangles;
}


bool
Triangle::operator==(const Triangle& other) const {
    return myA == other.myA && myB == other.myB && myC == other.myC;
}


bool
Triangle::operator!=(const Triangle& other) const {
    return !(*this == other);
}


bool
Triangle::isPositionWithin(const Position& A, const Position& B, const Position& C, const Position& pos) {
    // Calculate cross products for each edge of the triangle
    const double crossAB = crossProduct(A, B, pos);
    const double crossBC = crossProduct(B, C, pos);
    const double crossCA = crossProduct(C, A, pos);
    // Check if all cross products have the same sign
    return ((crossAB >= 0) && (crossBC >= 0) && (crossCA >= 0)) ||
           ((crossAB <= 0) && (crossBC <= 0) && (crossCA <= 0));
}


bool
Triangle::isEar(const Position& a, const Position& b, const Position& c, const PositionVector& shape) {
    // Check if triangle ABC is counter-clockwise
    if (crossProduct(a, b, c) <= 0) {
        return false;
    }
    // Check if any other point in the polygon lies inside the triangle
    for (const auto& pos : shape) {
        if ((pos != a) && (pos != b) && (pos != c) && isPositionWithin(a, b, c, pos)) {
            return false;
        }
    }
    return true;
}


double
Triangle::crossProduct(const Position& a, const Position& b, const Position& c) {
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}


int
Triangle::orientation(const Position& p, const Position& q, const Position& r) const {
    const double val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
    if (val > 0) {
        // Clockwise
        return 1;
    } else if (val < 0) {
        // Counterclockwise
        return -1;
    } else {
        // Collinear
        return 0;
    }
}


bool
Triangle::onSegment(const Position& p, const Position& q, const Position& r) const {
    return (q.x() >= std::min(p.x(), r.x()) && q.x() <= std::max(p.x(), r.x()) &&
            q.y() >= std::min(p.y(), r.y()) && q.y() <= std::max(p.y(), r.y()));
}


bool
Triangle::segmentsIntersect(const Position& p1, const Position& q1, const Position& p2, const Position& q2) const {
    const int o1 = orientation(p1, q1, p2);
    const int o2 = orientation(p1, q1, q2);
    const int o3 = orientation(p2, q2, p1);
    const int o4 = orientation(p2, q2, q1);
    // General case: segments intersect if they have different orientations
    // Special cases: checking if points are collinear and on segment
    if ((o1 != o2) && (o3 != o4)) {
        return true;
    } else if ((o1 == 0) && onSegment(p1, p2, q1)) {
        return true;
    } else if ((o2 == 0) && onSegment(p1, q2, q1)) {
        return true;
    } else if ((o3 == 0) && onSegment(p2, p1, q2)) {
        return true;
    } else if ((o4 == 0) && onSegment(p2, q1, q2)) {
        return true;
    } else {
        return false;
    }
}


bool
Triangle::lineIntersectsTriangle(const Position& p1, const Position& p2) const {
    return segmentsIntersect(p1, p2, myA, myB) ||
           segmentsIntersect(p1, p2, myB, myC) ||
           segmentsIntersect(p1, p2, myC, myA);
}


bool
Triangle::lineIntersectCircle(const Position& posA, const Position& posB, const Position& center, const double radius) const {
    // Calculate coefficients of the quadratic equation
    const double dx = posB.x() - posA.x();
    const double dy = posB.y() - posA.y();
    const double a = dx * dx + dy * dy;
    const double b = 2 * (dx * (posA.x() - center.x()) + dy * (posA.y() - center.y()));
    const double c = (posA.x() - center.x()) * (posA.x() - center.x()) + (posA.y() - center.y()) * (posA.y() - center.y()) - radius * radius;
    // Calculate the discriminant
    const double discriminant = (b * b - 4 * a * c);
    // Check the discriminant to determine the intersection
    if (discriminant >= 0) {
        // Calculate the two possible values of t
        const double sqrtDiscriminant = sqrt(discriminant);
        const double t1 = (-b + sqrtDiscriminant) / (2 * a);
        const double t2 = (-b - sqrtDiscriminant) / (2 * a);
        // if at least t1 or t2 is between [0,1], then intersect
        return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
    } else {
        return false;
    }
}

/****************************************************************************/
