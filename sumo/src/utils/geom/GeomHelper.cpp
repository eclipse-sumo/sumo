//---------------------------------------------------------------------------//
//                        GeomHelper.cpp -
//  Some geometrical helpers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2003/04/07 12:22:30  dkrajzew
// first steps towards a junctions geometry
//
// Revision 1.5  2003/03/20 16:41:10  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/12 16:37:16  dkrajzew
// debugging
//
// Revision 1.3  2003/03/03 15:25:25  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:50:20  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cmath>
#include "GeomHelper.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
GeomHelper::intersects(double x1, double y1, double x2, double y2,
                       double x3, double y3, double x4, double y4)
{
    /* Compute a1, b1, c1, where line joining points 1 and 2
     * is "a1 x  +  b1 y  +  c1  =  0".
     */
    double a1 = y2 - y1;
    double b1 = x1 - x2;
    double c1 = x2 * y1 - x1 * y2;

    /* Compute r3 and r4.
     */
    float r3 = float(a1 * x3 + b1 * y3 + c1);
    float r4 = float(a1 * x4 + b1 * y4 + c1);

    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
     * same side of line 1, the line segments do not intersect.
     */
    if ( r3 != 0 &&
         r4 != 0 &&
         ((r3<0&&r4<0)||(r3>0&&r4>0)))
        return ( false );

    /* Compute a2, b2, c2 */
    double a2 = y4 - y3;
    double b2 = x3 - x4;
    double c2 = x4 * y3 - x3 * y4;

    /* Compute r1 and r2 */
    double r1 = a2 * x1 + b2 * y1 + c2;
    double r2 = a2 * x2 + b2 * y2 + c2;

    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
     * on same side of second line segment, the line segments do
     * not intersect.
     */
    if ( r1 != 0 &&
         r2 != 0 &&
         ((r1<0&&r2<0)||(r1>0&&r2>0)))
        return ( false );

    /* Line segments intersect: compute intersection point.
     */
    double denom = a1 * b2 - a2 * b1;
    if ( denom == 0 )
        return ( false );
    double offset = denom < 0 ? - denom / 2 : denom / 2;

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


double
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
/*void Intersect_Lines(float p11.x(),float p11.y(),float p12.x(),float p12.y(),
                     float p21.x(),float p21.y(),float p22.x(),float p22.y(),
                     float *xi,float *yi)
{*/
    // this function computes the intersection of the sent lines
    // and returns the intersection point, note that the function assumes
    // the lines intersect. the function can handle vertical as well
    // as horizontal lines. note the function isn't very clever, it simply
    //applies the math, but we don't need speed since this is a
    //pre-processing step

    double a1,b1,c1, // constants of linear equations
          a2,b2,c2,
        det_inv, m1, m2;  // the inverse of the determinant of the coefficient

    // compute slopes, note the cludge for infinity, however, this will
    // be close enough

    if ((p12.x()-p11.x())!=0)
        m1 = (p12.y()-p11.y())/(p12.x()-p11.x());
    else
        m1 = (float)1e+10;   // close enough to infinity

    if ((p22.x()-p21.x())!=0)
        m2 = (p22.y()-p21.y())/(p22.x()-p21.x());
    else
        m2 = (float)1e+10;   // close enough to infinity

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
        ((a2*c1 - a1*c2)*det_inv) );
}



/*
   Return the angle between two vectors on a plane
   The angle is from vector 1 to vector 2, positive anticlockwise
   The result is between -pi -> pi
*/
double
GeomHelper::Angle2D(double x1, double y1, double x2, double y2)
{
   double dtheta,theta1,theta2;

   theta1 = atan2(y1,x1);
   theta2 = atan2(y2,x2);
   dtheta = theta2 - theta1;
   while (dtheta > 3.1415926535897932384626433832795)
      dtheta -= (2.0*3.1415926535897932384626433832795);
   while (dtheta < -3.1415926535897932384626433832795)
      dtheta += (2.0*3.1415926535897932384626433832795);

   return(dtheta);
}

/*
Position2D
GeomHelper::position_at_length_position(const Position2DVector &poly,
                                        double pos)
{
    Position2DVector::const_iterator i=poly.begin();
    double seenLength = 0;
    do {
        double nextLength = distance(*i, *(i+1));
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
                                        double pos)
{
    double dist = distance(p1, p2);
    double x = p1.x() + (p2.x() - p1.x()) / dist * pos;
    double y = p1.y() + (p2.y() - p1.y()) / dist * pos;
    return Position2D(x, y);
}
*/


Position2D
GeomHelper::interpolate(const Position2D &p1,
                        const Position2D &p2, double length)
{
    double oldlen = distance(p1, p2);
    double x = p1.x() + (p2.x() - p1.x()) * length / oldlen;
    double y = p1.y() + (p2.y() - p1.y()) * length / oldlen;
    return Position2D(x, y);
}


double
GeomHelper::nearest_position_on_line_to_point(const Position2D &l1,
                                              const Position2D &l2,
                                              const Position2D &p)
{
    return DistancePointLine(p, l1, l2);
}


double
GeomHelper::Magnitude(const Position2D &Point1,
                      const Position2D &Point2 )
{
    double x = Point2.x() - Point1.x();
    double y = Point2.y() - Point1.y();
    return sqrt(x*x + y*y);
}


double
GeomHelper::DistancePointLine(const Position2D &Point,
                              const Position2D &LineStart,
                              const Position2D &LineEnd
                              /*float &Distance */)
{
    float LineMag;
    float U;

    LineMag = Magnitude( LineEnd, LineStart );

    U = ( ( ( Point.x() - LineStart.x() ) * ( LineEnd.x() - LineStart.x() ) ) +
        ( ( Point.y() - LineStart.y() ) * ( LineEnd.y() - LineStart.y() ) ) /*+
        ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) )*/
         )
        /
        ( LineMag * LineMag );

    if( U < 0.0f || U > 1.0f )
        return -1;   // closest point does not fall within the line segment

    Position2D Intersection(
        LineStart.x() + U * ( LineEnd.x() - LineStart.x() ),
        LineStart.y() + U * ( LineEnd.y() - LineStart.y() ) );
//    Intersection.Z = LineStart->Z + U * ( LineEnd->Z - LineStart->Z );

    float Distance = Magnitude( Point, Intersection );

    return Distance;
}



void
GeomHelper::transfer_to_side(Position2D &p,
                             const Position2D &lineBeg,
                             const Position2D &lineEnd,
                             double amount)
{
    double dx = lineBeg.x() - lineEnd.x();
    double dy = lineBeg.y() - lineEnd.y();
    double length = sqrt(
        (lineBeg.x() - lineEnd.x())*(lineBeg.x() - lineEnd.x())
        +
        (lineBeg.y() - lineEnd.y())*(lineBeg.y() - lineEnd.y()) );
    if(dx<0) { // fromX<toX -> to right
        if(dy>0) { // to up right -> lanes to down right (+, +)
            p.add(dy*amount/length, -dx*amount/length);
        } else if (dy<0) { // to down right -> lanes to down left (-, +)
            p.add(dy*amount/length, -dx*amount/length);
        } else { // to right -> lanes to down (0, +)
            p.add(0, -dx*amount/length);
        }
    } else if(dx>0) { // fromX>toX -> to left
        if(dy>0) { // to up left -> lanes to up right (+, -)
            p.add(dy*amount/length, -dx*amount/length);
        } else if (dy<0) { // to down left -> lanes to up left (-, -)
            p.add(dy*amount/length, -dx*amount/length);
        } else { // to left -> lanes to up (0, -)
            p.add(0, -dx*amount/length);
        }
    } else { // fromX==toX
        if(dy>0) { // to up -> lanes to right (+, 0)
            p.add(dy*amount/length, 0);
        } else if (dy<0) { // to down -> lanes to left (-, 0)
            p.add(dy*amount/length, 0);
        } else { // zero !
            throw 1;
        }
    }
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GeomHelper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


