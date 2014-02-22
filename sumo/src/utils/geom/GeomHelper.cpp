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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include <utils/geom/Line.h>
#include "Boundary.h"
#include "GeomHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
GeomHelper::intersects(const SUMOReal x1, const SUMOReal y1,
                       const SUMOReal x2, const SUMOReal y2,
                       const SUMOReal x3, const SUMOReal y3,
                       const SUMOReal x4, const SUMOReal y4,
                       SUMOReal* x, SUMOReal* y, SUMOReal* mu) {
    const SUMOReal eps = std::numeric_limits<SUMOReal>::epsilon();
    const double denominator = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    const double numera = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    const double numerb = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
    /* Are the lines coincident? */
    if (fabs(numera) < eps && fabs(numerb) < eps && fabs(denominator) < eps) {
        SUMOReal a1;
        SUMOReal a2;
        SUMOReal a3;
        SUMOReal a4;
        SUMOReal a = -1e12;
        if (x1 != x2) {
            a1 = x1 < x2 ? x1 : x2;
            a2 = x1 < x2 ? x2 : x1;
            a3 = x3 < x4 ? x3 : x4;
            a4 = x3 < x4 ? x4 : x3;
        } else {
            a1 = y1 < y2 ? y1 : y2;
            a2 = y1 < y2 ? y2 : y1;
            a3 = y3 < y4 ? y3 : y4;
            a4 = y3 < y4 ? y4 : y3;
        }
        if (a1 <= a3 && a3 <= a2) {
            if (a4 < a2) {
                a = (a3 + a4) / 2;
            } else {
                a = (a2 + a3) / 2;
            }
        }
        if (a3 <= a1 && a1 <= a4) {
            if (a2 < a4) {
                a = (a1 + a2) / 2;
            } else {
                a = (a1 + a4) / 2;
            }
        }
        if (a != -1e12) {
            if (x != 0) {
                if (x1 != x2) {
                    *mu = (a - x1) / (x2 - x1);
                    *x = a;
                    *y = y1 + (*mu) * (y2 - y1);
                } else {
                    *x = x1;
                    *y = a;
                    *mu = (a - y1) / (y2 - y1);
                }
            }
            return true;
        }
        return false;
    }
    /* Are the lines parallel */
    if (fabs(denominator) < eps) {
        return false;
    }
    /* Is the intersection along the segments */
    const double mua = numera / denominator;
    const double mub = numerb / denominator;
    if (mua < 0 || mua > 1 || mub < 0 || mub > 1) {
        return false;
    }
    if (x != 0) {
        *x = x1 + mua * (x2 - x1);
        *y = y1 + mua * (y2 - y1);
        *mu = mua;
    }
    return true;
}



bool
GeomHelper::intersects(const Position& p11, const Position& p12,
                       const Position& p21, const Position& p22) {
    return intersects(p11.x(), p11.y(), p12.x(), p12.y(),
                      p21.x(), p21.y(), p22.x(), p22.y(), 0, 0, 0);
}


bool
GeomHelper::pointOnLine(const Position& p, const Position& from, const Position& to) {
    if (p.x() >= MIN2(from.x(), to.x()) && p.x() <= MAX2(from.x(), to.x()) &&
            p.y() >= MIN2(from.y(), to.y()) && p.y() <= MAX2(from.y(), to.y())) {
        return true;
    }
    return false;
}


void
GeomHelper::FindLineCircleIntersections(const Position& c, SUMOReal radius, const Position& p1, const Position& p2,
                                        std::vector<SUMOReal>& into) {
    SUMOReal dx = p2.x() - p1.x();
    SUMOReal dy = p2.y() - p1.y();

    SUMOReal A = dx * dx + dy * dy;
    SUMOReal B = 2 * (dx * (p1.x() - c.x()) + dy * (p1.y() - c.y()));
    SUMOReal C = (p1.x() - c.x()) * (p1.x() - c.x()) + (p1.y() - c.y()) * (p1.y() - c.y()) - radius * radius;

    SUMOReal det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)) {
        // No real solutions.
        return;
    } else if (det == 0) {
        // One solution.
        SUMOReal t = -B / (2 * A);
        Position intersection(p1.x() + t * dx, p1.y() + t * dy);
        if (GeomHelper::pointOnLine(intersection, p1, p2)) {
            into.push_back(t);
        }
        return;
    } else {
        // Two solutions.
        SUMOReal t = (float)((-B + sqrt(det)) / (2 * A));
        Position intersection(p1.x() + t * dx, p1.y() + t * dy);
        if (GeomHelper::pointOnLine(intersection, p1, p2)) {
            into.push_back(t);
        }
        t = (float)((-B - sqrt(det)) / (2 * A));
        intersection.set(p1.x() + t * dx, p1.y() + t * dy);
        if (GeomHelper::pointOnLine(intersection, p1, p2)) {
            into.push_back(t);
        }
        return;
    }
}



Position
GeomHelper::intersection_position2D(const Position& p11,
                                    const Position& p12,
                                    const Position& p21,
                                    const Position& p22) {
    SUMOReal x, y, m;
    if (intersects(p11.x(), p11.y(), p12.x(), p12.y(),
                   p21.x(), p21.y(), p22.x(), p22.y(), &x, &y, &m)) {
        // @todo calculate better "average" z value
        return Position(x, y, p11.z() + m * (p12.z() - p11.z()));
    }
    return Position(-1, -1);
}



/*
   Return the angle between two vectors on a plane
   The angle is from vector 1 to vector 2, positive anticlockwise
   The result is between -pi -> pi
*/
SUMOReal
GeomHelper::Angle2D(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2) {
    SUMOReal dtheta = atan2(y2, x2) - atan2(y1, x1);
    while (dtheta > (SUMOReal) M_PI) {
        dtheta -= (SUMOReal)(2.0 * M_PI);
    }
    while (dtheta < (SUMOReal) - M_PI) {
        dtheta += (SUMOReal)(2.0 * M_PI);
    }
    return dtheta;
}


Position
GeomHelper::interpolate(const Position& p1,
                        const Position& p2, SUMOReal length) {
    const SUMOReal factor = length / p1.distanceTo(p2);
    return p1 + (p2 - p1) * factor;
}


Position
GeomHelper::extrapolate_first(const Position& p1,
                              const Position& p2, SUMOReal length) {
    const SUMOReal factor = length / p1.distanceTo(p2);
    return p1 - (p2 - p1) * factor;
}


Position
GeomHelper::extrapolate_second(const Position& p1,
                               const Position& p2, SUMOReal length) {
    const SUMOReal factor = length / p1.distanceTo(p2);
    return p2 - (p1 - p2) * factor;
}


SUMOReal
GeomHelper::nearest_offset_on_line_to_point2D(const Position& LineStart,
        const Position& LineEnd,
        const Position& Point, bool perpendicular) {
    const SUMOReal lineLength2D = LineStart.distanceTo2D(LineEnd);
    if (lineLength2D == 0.0f) {
        return 0.0f;
    } else {
        // scalar product equals length of orthogonal projection times length of vector being projected onto
        // dividing the scalar product by the square of the distance gives the relative position
        const SUMOReal u = (((Point.x() - LineStart.x()) * (LineEnd.x() - LineStart.x())) +
                            ((Point.y() - LineStart.y()) * (LineEnd.y() - LineStart.y()))
                           ) / (lineLength2D * lineLength2D);
        if (u < 0.0f || u > 1.0f) {  // closest point does not fall within the line segment
            if (perpendicular) {
                return -1;
            }
            if (u < 0.0f) {
                return 0.0f;
            }
            return lineLength2D;
        }
        return u * lineLength2D;
    }
}


SUMOReal
GeomHelper::distancePointLine(const Position& point,
                              const Position& lineStart,
                              const Position& lineEnd) {
    const SUMOReal lineLengthSquared = lineStart.distanceSquaredTo(lineEnd);
    if (lineLengthSquared == 0.0f) {
        return point.distanceTo(lineStart);
    } else {
        // scalar product equals length of orthogonal projection times length of vector being projected onto
        // dividing the scalar product by the square of the distance gives the relative position
        SUMOReal u = (((point.x() - lineStart.x()) * (lineEnd.x() - lineStart.x())) +
                      ((point.y() - lineStart.y()) * (lineEnd.y() - lineStart.y()))
                     ) / lineLengthSquared;
        if (u < 0.0f || u > 1.0f) {
            return -1;    // closest point does not fall within the line segment
        }
        Position intersection(
            lineStart.x() + u * (lineEnd.x() - lineStart.x()),
            lineStart.y() + u * (lineEnd.y() - lineStart.y()));
        return point.distanceTo(intersection);
    }
}


SUMOReal
GeomHelper::closestDistancePointLine(const Position& point,
                                     const Position& lineStart,
                                     const Position& lineEnd,
                                     Position& outIntersection) {
    const SUMOReal length = nearest_offset_on_line_to_point2D(lineStart, lineEnd, point, false);
    outIntersection.set(Line(lineStart, lineEnd).getPositionAtDistance(length));
    return point.distanceTo2D(outIntersection);
}



Position
GeomHelper::transfer_to_side(Position& p,
                             const Position& lineBeg,
                             const Position& lineEnd,
                             SUMOReal amount) {
    const SUMOReal dx = lineBeg.x() - lineEnd.x();
    const SUMOReal dy = lineBeg.y() - lineEnd.y();
    const SUMOReal length = sqrt(dx * dx + dy * dy);
    if (length > 0) {
        p.add(dy * amount / length, -dx * amount / length);
    }
    return p;
}



Position
GeomHelper::crossPoint(const Boundary& b, const PositionVector& v) {
    if (v.intersects(Position(b.xmin(), b.ymin()), Position(b.xmin(), b.ymax()))) {
        return v.intersectsAtPoint(
                   Position(b.xmin(), b.ymin()),
                   Position(b.xmin(), b.ymax()));
    }
    if (v.intersects(Position(b.xmax(), b.ymin()), Position(b.xmax(), b.ymax()))) {
        return v.intersectsAtPoint(
                   Position(b.xmax(), b.ymin()),
                   Position(b.xmax(), b.ymax()));
    }
    if (v.intersects(Position(b.xmin(), b.ymin()), Position(b.xmax(), b.ymin()))) {
        return v.intersectsAtPoint(
                   Position(b.xmin(), b.ymin()),
                   Position(b.xmax(), b.ymin()));
    }
    if (v.intersects(Position(b.xmin(), b.ymax()), Position(b.xmax(), b.ymax()))) {
        return v.intersectsAtPoint(
                   Position(b.xmin(), b.ymax()),
                   Position(b.xmax(), b.ymax()));
    }
    throw 1;
}

std::pair<SUMOReal, SUMOReal>
GeomHelper::getNormal90D_CW(const Position& beg,
                            const Position& end,
                            SUMOReal wanted_offset) {
    return getNormal90D_CW(beg, end, beg.distanceTo2D(end), wanted_offset);
}


std::pair<SUMOReal, SUMOReal>
GeomHelper::getNormal90D_CW(const Position& beg,
                            const Position& end,
                            SUMOReal length, SUMOReal wanted_offset) {
    if (beg == end) {
        throw InvalidArgument("same points at " + toString(beg));
    }
    return std::pair<SUMOReal, SUMOReal>
           ((beg.y() - end.y()) * wanted_offset / length, (end.x() - beg.x()) * wanted_offset / length);
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
GeomHelper::getMaxAngleDiff(SUMOReal angle1, SUMOReal angle2) {
    return MAX2(getCWAngleDiff(angle1, angle2), getCCWAngleDiff(angle1, angle2));
}



/****************************************************************************/

