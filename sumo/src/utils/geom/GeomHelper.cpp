/****************************************************************************/
/// @file    GeomHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some geometrical helpers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
GeomHelper::intersects(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2,
                       SUMOReal x3, SUMOReal y3, SUMOReal x4, SUMOReal y4)
{
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
            ((r3<0&&r4<0)||(r3>0&&r4>0)))
        return (false);

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
            ((r1<0&&r2<0)||(r1>0&&r2>0)))
        return (false);

    /* Line segments intersect: compute intersection point.
     */
    SUMOReal denom = a1 * b2 - a2 * b1;
    if (denom == 0)
        return (false);
//    SUMOReal offset = denom < 0 ? - denom / 2 : denom / 2;

    return true;
}




bool
GeomHelper::intersects(const Position2D &p11, const Position2D &p12,
                       const Position2D &p21, const Position2D &p22)
{
    return intersects(p11.x(), p11.y(), p12.x(), p12.y(),
                      p21.x(), p21.y(), p22.x(), p22.y());
}



/*
bool
GeomHelper::intersects(const Position2DVector &v1,
                       const Position2DVector &v2)
{
    for(Position2DVector::const_iterator i=v1.begin(); i!=v1.end()-1; i++) {
        if(intersects(*i, *(i+1), v2)) {
            return true;
        }
    }
    return intersects(*(v1.end()-1), *(v1.begin()), v2);
}
*/


SUMOReal
GeomHelper::distance(const Position2D &p1, const Position2D &p2)
{
    return sqrt(
               (p1.x()-p2.x())*(p1.x()-p2.x())
               +
               (p1.y()-p2.y())*(p1.y()-p2.y()));
}

/*
Position2DVector::const_iterator
GeomHelper::find_intersecting_line(const Position2D &p1, const Position2D &p2,
                                   const Position2DVector &poly,
                                   Position2DVector::const_iterator beg)
{
    for(Position2DVector::const_iterator i=beg; i!=poly.end()-1; i++) {
        if(intersects(p1, p2, *(i), *(i+1))) {
            return i;
        }
    }
    if(intersects(p1, p2, *(poly.end()-1), *(poly.begin()))) {
        return poly.end()-1;
    }
    return poly.end();
}
*/
/*
Position2D
GeomHelper::intersection_position(const Position2D &p1,
                                  const Position2D &p2,
                                  const Position2DVector &poly,
                                  Position2DVector::const_iterator at)
{
    if(at==poly.end()-1) {
        return intersection_position(p1, p2, *(poly.end()-1), *(poly.begin()));
    } else {
        return intersection_position(p1, p2, *(at), *(at+1));
    }
}
*/

Position2D
GeomHelper::intersection_position(const Position2D &p11,
                                  const Position2D &p12,
                                  const Position2D &p21,
                                  const Position2D &p22)
{
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

    SUMOReal a1,b1,c1, // constants of linear equations
    a2,b2,c2,
    det_inv, m1, m2;  // the inverse of the determinant of the coefficient

    // compute slopes, note the cludge for infinity, however, this will
    // be close enough

    if ((p12.x()-p11.x())!=0)
        m1 = (p12.y()-p11.y())/(p12.x()-p11.x());
    else
        m1 = (SUMOReal)1e+10;   // close enough to infinity

    if ((p22.x()-p21.x())!=0)
        m2 = (p22.y()-p21.y())/(p22.x()-p21.x());
    else
        m2 = (SUMOReal)1e+10;   // close enough to infinity

    // compute constants

    a1 = m1;
    a2 = m2;

    b1 = -1;
    b2 = -1;

    c1 = (p11.y()-m1*p11.x());
    c2 = (p21.y()-m2*p21.x());

    // compute the inverse of the determinate
    det_inv = 1/(a1*b2 - a2*b1);

    // use Kramers rule to compute xi and yi
    return Position2D(
               ((b1*c2 - b2*c1)*det_inv),
               ((a2*c1 - a1*c2)*det_inv));
}



/*
   Return the angle between two vectors on a plane
   The angle is from vector 1 to vector 2, positive anticlockwise
   The result is between -pi -> pi
*/
SUMOReal
GeomHelper::Angle2D(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2)
{
    SUMOReal dtheta,theta1,theta2;

    theta1 = atan2(y1,x1);
    theta2 = atan2(y2,x2);
    dtheta = theta2 - theta1;
    while (dtheta > (SUMOReal) 3.1415926535897932384626433832795)
        dtheta -= (SUMOReal)(2.0*3.1415926535897932384626433832795);
    while (dtheta < (SUMOReal) -3.1415926535897932384626433832795)
        dtheta += (SUMOReal)(2.0*3.1415926535897932384626433832795);

    return(dtheta);
}

/*
Position2D
GeomHelper::position_at_length_position(const Position2DVector &poly,
                                        SUMOReal pos)
{
    Position2DVector::const_iterator i=poly.begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = distance(*i, *(i+1));
        if(seenLength+nextLength>pos) {
            return position_at_length_position(*i, *(i+1), pos-seenLength);
        }
        seenLength += nextLength;
    } while(++i!=poly.end()-1);
    return position_at_length_position(*(poly.end()-1),
        *(poly.begin()), pos-seenLength);
}
*/
/*
Position2D
GeomHelper::position_at_length_position(const Position2D &p1,
                                        const Position2D &p2,
                                        SUMOReal pos)
{
    SUMOReal dist = distance(p1, p2);
    SUMOReal x = p1.x() + (p2.x() - p1.x()) / dist * pos;
    SUMOReal y = p1.y() + (p2.y() - p1.y()) / dist * pos;
    return Position2D(x, y);
}
*/


Position2D
GeomHelper::interpolate(const Position2D &p1,
                        const Position2D &p2, SUMOReal length)
{
    SUMOReal oldlen = distance(p1, p2);
    SUMOReal x = p1.x() + (p2.x() - p1.x()) * length / oldlen;
    SUMOReal y = p1.y() + (p2.y() - p1.y()) * length / oldlen;
    return Position2D(x, y);
}


Position2D
GeomHelper::extrapolate_first(const Position2D &p1,
                              const Position2D &p2, SUMOReal length)
{
    SUMOReal oldlen = distance(p1, p2);
    SUMOReal x = p1.x() - (p2.x() - p1.x()) * (length) / oldlen;
    SUMOReal y = p1.y() - (p2.y() - p1.y()) * (length) / oldlen;
    return Position2D(x, y);
}


Position2D
GeomHelper::extrapolate_second(const Position2D &p1,
                               const Position2D &p2, SUMOReal length)
{
    SUMOReal oldlen = distance(p1, p2);
    SUMOReal x = p2.x() - (p1.x() - p2.x()) * (length) / oldlen;
    SUMOReal y = p2.y() - (p1.y() - p2.y()) * (length) / oldlen;
    return Position2D(x, y);
}


SUMOReal
GeomHelper::nearest_position_on_line_to_point(const Position2D &LineStart,
        const Position2D &LineEnd,
        const Position2D &Point)
{
    SUMOReal LineMag;
    SUMOReal U;

    LineMag = Magnitude(LineEnd, LineStart);

    U = (((Point.x() - LineStart.x()) * (LineEnd.x() - LineStart.x())) +
         ((Point.y() - LineStart.y()) * (LineEnd.y() - LineStart.y())) /*+
                                            ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) )*/
        )
        /
        (LineMag * LineMag);

    if (U < 0.0f || U > 1.0f)
        return -1;   // closest point does not fall within the line segment

    return U * distance(LineStart, LineEnd);
}


SUMOReal
GeomHelper::Magnitude(const Position2D &Point1,
                      const Position2D &Point2)
{
    SUMOReal x = Point2.x() - Point1.x();
    SUMOReal y = Point2.y() - Point1.y();
    return sqrt(x*x + y*y);
}


SUMOReal
GeomHelper::DistancePointLine(const Position2D &Point,
                              const Position2D &LineStart,
                              const Position2D &LineEnd
                              /*SUMOReal &Distance */)
{
    SUMOReal LineMag;
    SUMOReal U;

    LineMag = Magnitude(LineEnd, LineStart);

    U = (((Point.x() - LineStart.x()) * (LineEnd.x() - LineStart.x())) +
         ((Point.y() - LineStart.y()) * (LineEnd.y() - LineStart.y())) /*+
                                            ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) )*/
        )
        /
        (LineMag * LineMag);

    if (U < 0.0f || U > 1.0f)
        return -1;   // closest point does not fall within the line segment

    Position2D Intersection(
        LineStart.x() + U *(LineEnd.x() - LineStart.x()),
        LineStart.y() + U *(LineEnd.y() - LineStart.y()));
//    Intersection.Z = LineStart->Z + U * ( LineEnd->Z - LineStart->Z );

    SUMOReal Distance = Magnitude(Point, Intersection);

    return Distance;
}



SUMOReal 
GeomHelper::DistancePointLine(const Position2D &Point,
					const Position2D &LineStart, 
					const Position2D &LineEnd,
					Position2D& outIntersection) {
	SUMOReal LineMag;
    SUMOReal U;

    LineMag = Magnitude(LineEnd, LineStart);

    U = (((Point.x() - LineStart.x()) * (LineEnd.x() - LineStart.x())) +
         ((Point.y() - LineStart.y()) * (LineEnd.y() - LineStart.y())) /*+
                                            ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) )*/
        )
        /
        (LineMag * LineMag);

    if (U < 0.0f || U > 1.0f)
        return -1;   // closest point does not fall within the line segment

	outIntersection.set(
        LineStart.x() + U *(LineEnd.x() - LineStart.x()),
        LineStart.y() + U *(LineEnd.y() - LineStart.y()));
//    Intersection.Z = LineStart->Z + U * ( LineEnd->Z - LineStart->Z );

    SUMOReal Distance = Magnitude(Point, outIntersection);

    return Distance;
}



Position2D
GeomHelper::transfer_to_side(Position2D &p,
                             const Position2D &lineBeg,
                             const Position2D &lineEnd,
                             SUMOReal amount)
{
    SUMOReal dx = lineBeg.x() - lineEnd.x();
    SUMOReal dy = lineBeg.y() - lineEnd.y();
    SUMOReal length = sqrt(
                          (lineBeg.x() - lineEnd.x())*(lineBeg.x() - lineEnd.x())
                          +
                          (lineBeg.y() - lineEnd.y())*(lineBeg.y() - lineEnd.y()));
    if (dx<0) { // fromX<toX -> to right
        if (dy>0) { // to up right -> lanes to down right (+, +)
            p.add(dy*amount/length, -dx*amount/length);
        } else if (dy<0) { // to down right -> lanes to down left (-, +)
            p.add(dy*amount/length, -dx*amount/length);
        } else { // to right -> lanes to down (0, +)
            p.add(0, -dx*amount/length);
        }
    } else if (dx>0) { // fromX>toX -> to left
        if (dy>0) { // to up left -> lanes to up right (+, -)
            p.add(dy*amount/length, -dx*amount/length);
        } else if (dy<0) { // to down left -> lanes to up left (-, -)
            p.add(dy*amount/length, -dx*amount/length);
        } else { // to left -> lanes to up (0, -)
            p.add(0, -dx*amount/length);
        }
    } else { // fromX==toX
        if (dy>0) { // to up -> lanes to right (+, 0)
            p.add(dy*amount/length, 0);
        } else if (dy<0) { // to down -> lanes to left (-, 0)
            p.add(dy*amount/length, 0);
        } else { // zero !
            throw 1;
        }
    }
    return p;
}



Position2D
GeomHelper::crossPoint(const Boundary &b, const Position2DVector &v)
{
    if (v.intersects(Position2D(b.xmin(), b.ymin()), Position2D(b.xmin(), b.ymax()))) {
        return v.intersectsAtPoint(
                   Position2D(b.xmin(), b.ymin()),
                   Position2D(b.xmin(), b.ymax()));
    }
    if (v.intersects(Position2D(b.xmax(), b.ymin()), Position2D(b.xmax(), b.ymax()))) {
        return v.intersectsAtPoint(
                   Position2D(b.xmax(), b.ymin()),
                   Position2D(b.xmax(), b.ymax()));
    }
    if (v.intersects(Position2D(b.xmin(), b.ymin()), Position2D(b.xmax(), b.ymin()))) {
        return v.intersectsAtPoint(
                   Position2D(b.xmin(), b.ymin()),
                   Position2D(b.xmax(), b.ymin()));
    }
    if (v.intersects(Position2D(b.xmin(), b.ymax()), Position2D(b.xmax(), b.ymax()))) {
        return v.intersectsAtPoint(
                   Position2D(b.xmin(), b.ymax()),
                   Position2D(b.xmax(), b.ymax()));
    }
    throw 1;
}

std::pair<SUMOReal, SUMOReal>
GeomHelper::getNormal90D_CW(const Position2D &beg,
                            const Position2D &end,
                            SUMOReal wanted_offset)
{
    SUMOReal length = sqrt((beg.x()-end.x())*(beg.x()-end.x()) + (beg.y()-end.y())*(beg.y()-end.y()));
    return getNormal90D_CW(beg.x(), beg.y(), end.x(), end.y(),
                           length, wanted_offset);
}


std::pair<SUMOReal, SUMOReal>
GeomHelper::getNormal90D_CW(const Position2D &beg,
                            const Position2D &end,
                            SUMOReal length, SUMOReal wanted_offset)
{
    return getNormal90D_CW(beg.x(), beg.y(), end.x(), end.y(),
                           length, wanted_offset);
}


std::pair<SUMOReal, SUMOReal>
GeomHelper::getNormal90D_CW(SUMOReal x1, SUMOReal y1,
                            SUMOReal x2, SUMOReal y2,
                            SUMOReal length, SUMOReal wanted_offset) throw(InvalidArgument)
{
    SUMOReal dx = x1 - x2;
    SUMOReal dy = y1 - y2;
    if (dx<0) { // fromX<toX -> to right
        if (dy>0) { // to up right -> lanes to down right (+, +)
            return std::pair<SUMOReal, SUMOReal>
                   (dy*wanted_offset/length, -dx*wanted_offset/length);
        } else if (dy<0) { // to down right -> lanes to down left (-, +)
            return std::pair<SUMOReal, SUMOReal>
                   (dy*wanted_offset/length, -dx*wanted_offset/length);
        } else { // to right -> lanes to down (0, +)
            return std::pair<SUMOReal, SUMOReal>
                   (0, -dx*wanted_offset/length);
        }
    } else if (dx>0) { // fromX>toX -> to left
        if (dy>0) { // to up left -> lanes to up right (+, -)
            return std::pair<SUMOReal, SUMOReal>
                   (dy*wanted_offset/length, -dx*wanted_offset/length);
        } else if (dy<0) { // to down left -> lanes to up left (-, -)
            return std::pair<SUMOReal, SUMOReal>
                   (dy*wanted_offset/length, -dx*wanted_offset/length);
        } else { // to left -> lanes to up (0, -)
            return std::pair<SUMOReal, SUMOReal>
                   (0, -dx*wanted_offset/length);
        }
    } else { // fromX==toX
        if (dy>0) { // to up -> lanes to right (+, 0)
            return std::pair<SUMOReal, SUMOReal>
                   (dy*wanted_offset/length, 0);
        } else if (dy<0) { // to down -> lanes to left (-, 0)
            return std::pair<SUMOReal, SUMOReal>
                   (dy*wanted_offset/length, 0);
        } else { // zero !
            throw InvalidArgument("same points");
        }
    }
}


SUMOReal
GeomHelper::getCCWAngleDiff(SUMOReal angle1, SUMOReal angle2)
{
    if (angle1<0) {
        angle1 = 360 + angle1;
    }
    if (angle2<0) {
        angle2 = 360 + angle2;
    }
    if (angle1>=angle2) {
        return angle1 - angle2;
    }
    return angle1 + 360 - angle2;
}

SUMOReal
GeomHelper::getCWAngleDiff(SUMOReal angle1, SUMOReal angle2)
{
    if (angle1<0) {
        angle1 = 360 + angle1;
    }
    if (angle2<0) {
        angle2 = 360 + angle2;
    }
    if (angle1<=angle2) {
        return angle2 - angle1;
    }
    return 360 - angle1 + angle2;
}



/****************************************************************************/

