/****************************************************************************/
/// @file    GeomHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Some static methods performing geometrical operations
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include "Boundary.h"
#include "GeomHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
const SUMOReal GeomHelper::INVALID_OFFSET = -1;


// ===========================================================================
// method definitions
// ===========================================================================

void
GeomHelper::findLineCircleIntersections(const Position& c, SUMOReal radius, const Position& p1, const Position& p2,
                                        std::vector<SUMOReal>& into) {
    const SUMOReal dx = p2.x() - p1.x();
    const SUMOReal dy = p2.y() - p1.y();

    const SUMOReal A = dx * dx + dy * dy;
    const SUMOReal B = 2 * (dx * (p1.x() - c.x()) + dy * (p1.y() - c.y()));
    const SUMOReal C = (p1.x() - c.x()) * (p1.x() - c.x()) + (p1.y() - c.y()) * (p1.y() - c.y()) - radius * radius;

    const SUMOReal det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)) {
        // No real solutions.
        return;
    }
    if (det == 0) {
        // One solution.
        const SUMOReal t = -B / (2 * A);
        if (t >= 0. && t <= 1.) {
            into.push_back(t);
        }
    } else {
        // Two solutions.
        const SUMOReal t = (SUMOReal)((-B + sqrt(det)) / (2 * A));
        Position intersection(p1.x() + t * dx, p1.y() + t * dy);
        if (t >= 0. && t <= 1.) {
            into.push_back(t);
        }
        const SUMOReal t2 = (SUMOReal)((-B - sqrt(det)) / (2 * A));
        if (t2 >= 0. && t2 <= 1.) {
            into.push_back(t2);
        }
    }
}


SUMOReal
GeomHelper::angle2D(const Position& p1, const Position& p2) {
    return angleDiff(atan2(p1.y(), p1.x()), atan2(p2.y(), p2.x()));
}


SUMOReal
GeomHelper::nearest_offset_on_line_to_point2D(const Position& lineStart,
        const Position& lineEnd,
        const Position& p, bool perpendicular) {
    const SUMOReal lineLength2D = lineStart.distanceTo2D(lineEnd);
    if (lineLength2D == 0.0f) {
        return 0.0f;
    } else {
        // scalar product equals length of orthogonal projection times length of vector being projected onto
        // dividing the scalar product by the square of the distance gives the relative position
        const SUMOReal u = (((p.x() - lineStart.x()) * (lineEnd.x() - lineStart.x())) +
                            ((p.y() - lineStart.y()) * (lineEnd.y() - lineStart.y()))
                           ) / (lineLength2D * lineLength2D);
        if (u < 0.0f || u > 1.0f) {  // closest point does not fall within the line segment
            if (perpendicular) {
                return INVALID_OFFSET;
            }
            if (u < 0.0f) {
                return 0.0f;
            }
            return lineLength2D;
        }
        return u * lineLength2D;
    }
}


Position
GeomHelper::crossPoint(const Boundary& b, const PositionVector& v) {
    if (v.intersects(Position(b.xmin(), b.ymin()), Position(b.xmin(), b.ymax()))) {
        return v.intersectionPosition2D(
                   Position(b.xmin(), b.ymin()),
                   Position(b.xmin(), b.ymax()));
    }
    if (v.intersects(Position(b.xmax(), b.ymin()), Position(b.xmax(), b.ymax()))) {
        return v.intersectionPosition2D(
                   Position(b.xmax(), b.ymin()),
                   Position(b.xmax(), b.ymax()));
    }
    if (v.intersects(Position(b.xmin(), b.ymin()), Position(b.xmax(), b.ymin()))) {
        return v.intersectionPosition2D(
                   Position(b.xmin(), b.ymin()),
                   Position(b.xmax(), b.ymin()));
    }
    if (v.intersects(Position(b.xmin(), b.ymax()), Position(b.xmax(), b.ymax()))) {
        return v.intersectionPosition2D(
                   Position(b.xmin(), b.ymax()),
                   Position(b.xmax(), b.ymax()));
    }
    throw 1;
}

SUMOReal
GeomHelper::getCCWAngleDiff(SUMOReal angle1, SUMOReal angle2) {
    SUMOReal v = angle2 - angle1;
    if (v < 0) {
        v = 360 + v;
    }
    return v;
}


SUMOReal
GeomHelper::getCWAngleDiff(SUMOReal angle1, SUMOReal angle2) {
    SUMOReal v = angle1 - angle2;
    if (v < 0) {
        v = 360 + v;
    }
    return v;
}


SUMOReal
GeomHelper::getMinAngleDiff(SUMOReal angle1, SUMOReal angle2) {
    return MIN2(getCWAngleDiff(angle1, angle2), getCCWAngleDiff(angle1, angle2));
}


SUMOReal
GeomHelper::angleDiff(const SUMOReal angle1, const SUMOReal angle2) {
    SUMOReal dtheta = angle2 - angle1;
    while (dtheta > (SUMOReal) M_PI) {
        dtheta -= (SUMOReal)(2.0 * M_PI);
    }
    while (dtheta < (SUMOReal) - M_PI) {
        dtheta += (SUMOReal)(2.0 * M_PI);
    }
    return dtheta;
}


SUMOReal
GeomHelper::naviDegree(const SUMOReal angle) {
    SUMOReal degree = RAD2DEG(M_PI / 2. - angle);
    while (degree >= 360.) {
        degree -= 360.;
    }
    while (degree < 0.) {
        degree += 360.;
    }
    return degree;
}


SUMOReal
GeomHelper::legacyDegree(const SUMOReal angle, const bool positive) {
    SUMOReal degree = -RAD2DEG(M_PI / 2. + angle);
    if (positive) {
        while (degree >= 360.) {
            degree -= 360.;
        }
        while (degree < 0.) {
            degree += 360.;
        }
    } else {
        while (degree >= 180.) {
            degree -= 360.;
        }
        while (degree < -180.) {
            degree += 360.;
        }
    }
    return degree;
}


/****************************************************************************/

