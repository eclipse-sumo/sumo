/****************************************************************************/
/// @file    GeomHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Some geometrical helpers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <algorithm>
#include <iostream>
#include "GeomHelper.h"
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <utils/geom/Line.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
GeomHelper::intersects(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2,
                       SUMOReal x3, SUMOReal y3, SUMOReal x4, SUMOReal y4) {
    /* Compute a1, b1, c1, where line joining points 1 and 2
     * is "a1 x  +  b1 y  +  c1  =  0".
     */
    SUMOReal a1 = y2 - y1;
    SUMOReal b1 = x1 - x2;
    SUMOReal c1 = x2 * y1 - x1 * y2;

    /* Compute r3 and r4.
     */
    SUMOReal r3 = SUMOReal(a1 * x3 + b1 * y3 + c1);
    SUMOReal r4 = SUMOReal(a1 * x4 + b1 * y4 + c1);

    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
     * same side of line 1, the line segments do not intersect.
     */
    if (r3 != 0 &&
            r4 != 0 &&
            ((r3 < 0 && r4 < 0) || (r3 > 0 && r4 > 0))) {
        return (false);
    }

    /* Compute a2, b2, c2 */
    SUMOReal a2 = y4 - y3;
    SUMOReal b2 = x3 - x4;
    SUMOReal c2 = x4 * y3 - x3 * y4;

    /* Compute r1 and r2 */
    SUMOReal r1 = a2 * x1 + b2 * y1 + c2;
    SUMOReal r2 = a2 * x2 + b2 * y2 + c2;

    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
     * on same side of second line segment, the line segments do
     * not intersect.
     */
    if (r1 != 0 &&
            r2 != 0 &&
            ((r1 < 0 && r2 < 0) || (r1 > 0 && r2 > 0))) {
        return (false);
    }

    /* Line segments intersect: compute intersection point.
     */
    SUMOReal denom = a1 * b2 - a2 * b1;
    if (denom == 0) {
        return (false);
    }
//    SUMOReal offset = denom < 0 ? - denom / 2 : denom / 2;

    return true;
}




bool
GeomHelper::intersects(const Position& p11, const Position& p12,
                       const Position& p21, const Position& p22) {
    return intersects(p11.x(), p11.y(), p12.x(), p12.y(),
                      p21.x(), p21.y(), p22.x(), p22.y());
}


Position
GeomHelper::intersection_position(const Position& p11,
                                  const Position& p12,
                                  const Position& p21,
                                  const Position& p22) {
    /*void Intersect_Lines(SUMOReal p11.x(),SUMOReal p11.y(),SUMOReal p12.x(),SUMOReal p12.y(),
                         SUMOReal p21.x(),SUMOReal p21.y(),SUMOReal p22.x(),SUMOReal p22.y(),
                         SUMOReal *xi,SUMOReal *yi)
    {*/
    // this function computes the intersection of the sent lines
    // and returns the intersection point, note that the function assumes
    // the lines intersect. the function can handle vertical as well
    // as horizontal lines. note the function isn't very clever, it simply
    //applies the math, but we don't need speed since this is a
    //pre-processing step

    double a1, b1, c1, // constants of linear equations
           a2, b2, c2,
           det_inv, m1, m2;  // the inverse of the determinant of the coefficient

    // compute slopes, note the cludge for infinity, however, this will
    // be close enough

    if ((p12.x() - p11.x()) != 0) {
        m1 = (p12.y() - p11.y()) / (p12.x() - p11.x());
    } else {
        m1 = (SUMOReal)1e+10;    // close enough to infinity
    }

    if ((p22.x() - p21.x()) != 0) {
        m2 = (p22.y() - p21.y()) / (p22.x() - p21.x());
    } else {
        m2 = (SUMOReal)1e+10;    // close enough to infinity
    }

    // compute constants

    a1 = m1;
    a2 = m2;

    b1 = -1;
    b2 = -1;

    c1 = (p11.y() - m1 * p11.x());
    c2 = (p21.y() - m2 * p21.x());

    // compute the inverse of the determinate
    det_inv = 1 / (a1 * b2 - a2 * b1);

    // use Kramers rule to compute xi and yi
    return Position(
               ((b1 * c2 - b2 * c1) * det_inv),
               ((a2 * c1 - a1 * c2) * det_inv));
}



/*
   Return the angle between two vectors on a plane
   The angle is from vector 1 to vector 2, positive anticlockwise
   The result is between -pi -> pi
*/
SUMOReal
GeomHelper::Angle2D(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2) {
    SUMOReal dtheta = atan2(y2, x2) - atan2(y1, x1);
    while (dtheta > (SUMOReal) PI) {
        dtheta -= (SUMOReal)(2.0 * PI);
    }
    while (dtheta < (SUMOReal) - PI) {
        dtheta += (SUMOReal)(2.0 * PI);
    }
    return dtheta;
}


Position
GeomHelper::interpolate(const Position& p1,
                        const Position& p2, SUMOReal length) {
    const SUMOReal oldlen = p1.distanceTo(p2);
    const SUMOReal x = p1.x() + (p2.x() - p1.x()) * length / oldlen;
    const SUMOReal y = p1.y() + (p2.y() - p1.y()) * length / oldlen;
    return Position(x, y);
}


Position
GeomHelper::extrapolate_first(const Position& p1,
                              const Position& p2, SUMOReal length) {
    const SUMOReal oldlen = p1.distanceTo(p2);
    const SUMOReal x = p1.x() - (p2.x() - p1.x()) * length / oldlen;
    const SUMOReal y = p1.y() - (p2.y() - p1.y()) * length / oldlen;
    return Position(x, y);
}


Position
GeomHelper::extrapolate_second(const Position& p1,
                               const Position& p2, SUMOReal length) {
    const SUMOReal oldlen = p1.distanceTo(p2);
    const SUMOReal x = p2.x() - (p1.x() - p2.x()) * length / oldlen;
    const SUMOReal y = p2.y() - (p1.y() - p2.y()) * length / oldlen;
    return Position(x, y);
}


SUMOReal
GeomHelper::nearest_position_on_line_to_point2D(const Position& LineStart,
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
    const SUMOReal length = nearest_position_on_line_to_point2D(lineStart, lineEnd, point, false);
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

