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
// Revision 1.9  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */

#include "Position2D.h"
#include "Line2D.h"
#include "GeomHelper.h"
#include <cassert>


/* =========================================================================
 * member method definitions
 * ======================================================================= */

Line2D::Line2D()
{
}


Line2D::Line2D(const Position2D &p1, const Position2D &p2)
    : myP1(p1), myP2(p2)
{
}


Line2D::~Line2D()
{
}


void
Line2D::extrapolateBy(double length)
{
    double oldlen = GeomHelper::distance(myP1, myP2);
    double x1 = myP1.x() - (myP2.x() - myP1.x()) * (length) / oldlen;
    double y1 = myP1.y() - (myP2.y() - myP1.y()) * (length) / oldlen;
    double x2 = myP2.x() - (myP1.x() - myP2.x()) * (length) / oldlen;
    double y2 = myP2.y() - (myP1.y() - myP2.y()) * (length) / oldlen;
    myP1 = Position2D(x1, y1);
    myP2 = Position2D(x2, y2);
}


void
Line2D::extrapolateFirstBy(double length)
{
    myP1 = GeomHelper::extrapolate_first(myP1, myP2, length);
}


void
Line2D::extrapolateSecondBy(double length)
{
    myP2 = GeomHelper::extrapolate_second(myP1, myP2, length);
}

const Position2D &
Line2D::p1() const
{
    return myP1;
}


const Position2D &
Line2D::p2() const
{
    return myP2;
}


Position2D
Line2D::getPositionAtDistance(double offset) const
{
    double length = GeomHelper::distance(myP1, myP2);
    double x = myP1.x() + (myP2.x() - myP1.x()) / length * offset;
    double y = myP1.y() + (myP2.y() - myP1.y()) / length * offset;
/*    double x2 = myP2.x() - (myP1.x() - myP2.x())  / length * offset;
    double y2 = myP2.y() - (myP1.y() - myP2.y()) / length * offset;*/
    return Position2D(x, y);
}


void
Line2D::move2side(double amount)
{
    std::pair<double, double> p = GeomHelper::getNormal90D_CW(myP1, myP2, amount);
    myP1.add(p.first, p.second);
    myP2.add(p.first, p.second);
}


DoubleVector
Line2D::intersectsAtLengths(const Position2DVector &v)
{
    Position2DVector p = v.intersectsAtPoints(myP1, myP2);
    DoubleVector ret;
    for(size_t i=0; i<p.size(); i++) {
        ret.push_back(GeomHelper::distance(myP1, p.at(i)));
    }
    return ret;
}


double
Line2D::atan2Angle() const
{
    return atan2(myP1.x()-myP2.x(), myP1.y()-myP2.y());
}


double
Line2D::atan2DegreeAngle() const
{
    return atan2(myP1.x()-myP2.x(), myP1.y()-myP2.y()) * 180.0 / 3.1415926535897932384626433832795;
}


double
Line2D::atan2PositiveAngle() const
{
    double angle = atan2Angle();
    if(angle<0) {
        angle = PI * 2.0 + angle;
    }
    return angle;
}

Position2D
Line2D::intersectsAt(const Line2D &l) const
{
    return GeomHelper::intersection_position(myP1, myP2, l.myP1, l.myP2);
}


bool
Line2D::intersects(const Line2D &l) const
{
    return GeomHelper::intersects(myP1, myP2, l.myP1, l.myP2);
}


double
Line2D::length() const
{
    return sqrt(
        (myP1.x()-myP2.x())*(myP1.x()-myP2.x())
        +
        (myP1.y()-myP2.y())*(myP1.y()-myP2.y()));
}


void
Line2D::add(double x, double y)
{
    myP1.add(x, y);
    myP2.add(x, y);
}


void
Line2D::add(const Position2D &p)
{
    myP1.add(p.x(), p.y());
    myP2.add(p.x(), p.y());
}


void
Line2D::sub(double x, double y)
{
    myP1.sub(x, y);
    myP2.sub(x, y);
}




double
Line2D::distanceTo(const Position2D &p) const
{
    double LineMag;
    double U;

    LineMag = GeomHelper::Magnitude( myP2, myP1 );

    U = ( ( ( p.x() - myP1.x() ) * ( myP2.x() - myP1.x() ) ) +
        ( ( p.y() - myP1.y() ) * ( myP2.y() - myP1.y() ) ) /*+
        ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) )*/
         )
        /
        ( LineMag * LineMag );

    if( U < 0.0f || U > 1.0f )
        return -1;   // closest point does not fall within the line segment

    Position2D Intersection(
        myP1.x() + U * ( myP2.x() - myP1.x() ),
        myP1.y() + U * ( myP2.y() - myP1.y() ) );
//    Intersection.Z = LineStart->Z + U * ( LineEnd->Z - LineStart->Z );

    double Distance = GeomHelper::Magnitude( p, Intersection );

    return Distance;
}


Line2D &
Line2D::reverse()
{
    Position2D tmp(myP1);
    myP1 = myP2;
    myP2 = tmp;
    return *this;
}


double
Line2D::nearestPositionTo(const Position2D &p)
{
    return GeomHelper::nearest_position_on_line_to_point(myP1, myP2, p);
}


double
Line2D::intersectsAtLength(const Line2D &v)
{
    Position2D pos =
        GeomHelper::intersection_position(myP1, myP2, v.myP1, v.myP2);
    return nearestPositionTo(pos);
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
