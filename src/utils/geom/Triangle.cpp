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
Triangle::isAround(const Position& pos) const {
    const double areaPAB = calculateTriangleArea2D(pos, myA, myB);
    const double areaPBC = calculateTriangleArea2D(pos, myB, myC);
    const double areaPCA = calculateTriangleArea2D(pos, myC, myA);
    // Check if the sum of the areas matches the total area of ​​the triangle
    return std::abs(myArea - (areaPAB + areaPBC + areaPCA)) < 1e-9;
}


std::vector<Triangle>
Triangle::triangulate(PositionVector shape) {
    std::vector<Triangle> triangles;
    // first open polygon
    shape.openPolygon();
    // greedy algorithm
    while (shape.size() > 3) {
        triangles.push_back(Triangle(shape[0], shape[1], shape[2]));
        shape.erase(shape.begin() + 1);
    }
    // add last triangle
    if (shape.size() > 2) {
        triangles.push_back(Triangle(shape[0], shape[1], shape[0]));
    }
    return triangles;
}


double
Triangle::calculateTriangleArea2D(const Position& a, const Position& b, const Position& c) const {
    return std::abs((a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y())) / 2.0);
}

/****************************************************************************/
