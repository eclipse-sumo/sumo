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


Triangle::Triangle(const Position &positionA, const Position &positionB, const Position &positionC) :
    myA(positionA),
    myB(positionB),
    myC(positionC) {
    myArea = calculateTriangleArea2D(myA, myB, myC);
}


Triangle::~Triangle() {}


bool
Triangle::isAround(const Position &pos) const {
    if (myArea == -1) {
        return false;
    } else {
        const double areaPAB = calculateTriangleArea2D(pos, myA, myB);
        const double areaPBC = calculateTriangleArea2D(pos, myB, myC);
        const double areaPCA = calculateTriangleArea2D(pos, myC, myA);
        // Check if the sum of the areas matches the total area of ​​the triangle
        return std::abs(myArea - (areaPAB + areaPBC + areaPCA)) < 1e-9;
    }
}


std::vector<Triangle>
Triangle::triangulate(const PositionVector& shape) {
    std::vector<Triangle> triangles;
    // check that shape has at least three positions
    if (shape.size() < 3) {
        return triangles;
    } else {
        PositionVector remainingPoints = shape;
        // greedy algorithm
        while (remainingPoints.size() > 3) {
            const int numRemainingPoints = (int)remainingPoints.size();
            for (int i = 0; i < numRemainingPoints; i++) {
                int prevIndex = (i - 1 + numRemainingPoints) % numRemainingPoints;
                int nextIndex = (i + 1) % numRemainingPoints;
                // get previous, current and next point
                const Position& previousPoint = remainingPoints[prevIndex];
                const Position& currentPoint = remainingPoints[i];
                const Position& nextPoint = remainingPoints[nextIndex];
                // check that the vertex is an "ear"
                if (isLeft(currentPoint, previousPoint, nextPoint)) {
                    bool isEar = true;
                    for (int j = 0; j < numRemainingPoints; j++) {
                        if (j != i && j != prevIndex && j != nextIndex) {
                            if (isInsideTriangle2D(remainingPoints[j], previousPoint, currentPoint, nextPoint)) {
                                isEar = false;
                                break;
                            }
                        }
                    }
                    if (isEar) {
                        triangles.push_back(Triangle(previousPoint, currentPoint, nextPoint));
                        remainingPoints.erase(remainingPoints.begin() + i);
                        break;
                    }
                }
            }
        }
        // insert the last triangle
        if (remainingPoints.size() == 3) {
            triangles.push_back(Triangle(remainingPoints[0], remainingPoints[1], remainingPoints[2]));
        }
        return triangles;
    }
}


bool
Triangle::isLeft(const Position& p, const Position& a, const Position& b) {
    return ((b.x() - a.x()) * (p.y() - a.y())) > ((b.y() - a.y()) * (p.x() - a.x()));
}


double
Triangle::calculateTriangleArea2D(const Position& a, const Position& b, const Position& c) {
    return std::abs((a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y())) / 2.0);
}


bool
Triangle::isInsideTriangle2D(const Position& pos, const Position& a, const Position& b, const Position& c) {
    const double areaABC = calculateTriangleArea2D(a, b, c);
    const double areaPAB = calculateTriangleArea2D(pos, a, b);
    const double areaPBC = calculateTriangleArea2D(pos, b, c);
    const double areaPCA = calculateTriangleArea2D(pos, c, a);
    // Check if the sum of the areas matches the total area of ​​the triangle
    return std::abs(areaABC - (areaPAB + areaPBC + areaPCA)) < 1e-9;
}

/****************************************************************************/
