/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include "Triangle.h"


// ===========================================================================
// method definitions
// ===========================================================================

Triangle::Triangle() {}


Triangle::Triangle(const Position& positionA, const Position& positionB, const Position& positionC) :
    myA(positionA),
    myB(positionB),
    myC(positionC) {
    // calculate area and boundary
    myArea = calculateTriangleArea2D(myA, myB, myC);
    myBoundary.add(positionA);
    myBoundary.add(positionB);
    myBoundary.add(positionC);
}


Triangle::~Triangle() {}


bool
Triangle::isAroundPosition(const Position& pos) const {
    return isAroundPosition(myA, myB, myC, pos);
}


bool
Triangle::isAroundShape(const PositionVector& shape) const {
    if (isBoundaryAround(shape.getBoxBoundary())) {
        return true;
    } else {
        for (const auto& pos : shape) {
            if (isAroundPosition(pos)) {
                return true;
            }
        }
        return false;
    }
}

bool
Triangle::isBoundaryAround(const Boundary& boundary) const {
    if (isAroundPosition(Position(boundary.xmin(), boundary.ymin())) &&
            isAroundPosition(Position(boundary.xmax(), boundary.ymax()))) {
        return true;
    } else {
        return false;
    }
}


bool
Triangle::isCircunferenceAround(const Position& center, const double radius) const {
    if (lineIntersectCircle(myA, myB, center, radius)) {
        return true;
    } else if (lineIntersectCircle(myA, myB, center, radius)) {
        return true;
    } else if (lineIntersectCircle(myA, myB, center, radius)) {
        return true;
    } else {
        return isAroundPosition(center);
    }
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
Triangle::isAroundPosition(const Position& A, const Position& B, const Position& C, const Position& pos) {
    // Calculate cross products for each edge of the triangle
    const double crossAB = crossProduct(A, B, pos);
    const double crossBC = crossProduct(B, C, pos);
    const double crossCA = crossProduct(C, A, pos);
    // Check if all cross products have the same sign
    return (crossAB >= 0 && crossBC >= 0 && crossCA >= 0) ||
           (crossAB <= 0 && crossBC <= 0 && crossCA <= 0);
}


bool
Triangle::isEar(const Position& a, const Position& b, const Position& c, const PositionVector& shape) {
    // Check if triangle ABC is counter-clockwise
    if (crossProduct(a, b, c) <= 0) {
        return false;
    }
    // Check if any other point in the polygon lies inside the triangle
    for (const auto& pos : shape) {
        if ((pos != a) && (pos != b) && (pos != c) && isAroundPosition(a, b, c, pos)) {
            return false;
        }
    }
    return true;
}


double
Triangle::crossProduct(const Position& a, const Position& b, const Position& c) {
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}


double
Triangle::calculateTriangleArea2D(const Position& a, const Position& b, const Position& c) const {
    return std::abs((a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y())) / 2.0);
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
