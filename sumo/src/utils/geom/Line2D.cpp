/****************************************************************************/
/// @file    Line2D.cpp
/// @author  Daniel Krajzewicz
/// @date    2003-08-14
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "Position2D.h"
#include "Line2D.h"
#include "GeomHelper.h"
#include <cassert>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================

Line2D::Line2D()
{}


Line2D::Line2D(const Position2D &p1, const Position2D &p2)
        : myP1(p1), myP2(p2)
{}


Line2D::~Line2D()
{}


void
Line2D::extrapolateBy(SUMOReal length)
{
    SUMOReal oldlen = GeomHelper::distance(myP1, myP2);
    SUMOReal x1 = myP1.x() - (myP2.x() - myP1.x()) * (length) / oldlen;
    SUMOReal y1 = myP1.y() - (myP2.y() - myP1.y()) * (length) / oldlen;
    SUMOReal x2 = myP2.x() - (myP1.x() - myP2.x()) * (length) / oldlen;
    SUMOReal y2 = myP2.y() - (myP1.y() - myP2.y()) * (length) / oldlen;
    myP1 = Position2D(x1, y1);
    myP2 = Position2D(x2, y2);
}


void
Line2D::extrapolateFirstBy(SUMOReal length)
{
    myP1 = GeomHelper::extrapolate_first(myP1, myP2, length);
}


void
Line2D::extrapolateSecondBy(SUMOReal length)
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
Line2D::getPositionAtDistance(SUMOReal offset) const
{
    SUMOReal length = GeomHelper::distance(myP1, myP2);
    if (length==0) {
        if (offset!=0) {
            throw 1;
        }
        return myP1;
    }
    SUMOReal x = myP1.x() + (myP2.x() - myP1.x()) / length * offset;
    SUMOReal y = myP1.y() + (myP2.y() - myP1.y()) / length * offset;
    /*    SUMOReal x2 = myP2.x() - (myP1.x() - myP2.x())  / length * offset;
        SUMOReal y2 = myP2.y() - (myP1.y() - myP2.y()) / length * offset;*/
    return Position2D(x, y);
}


void
Line2D::move2side(SUMOReal amount)
{
    std::pair<SUMOReal, SUMOReal> p = GeomHelper::getNormal90D_CW(myP1, myP2, amount);
    myP1.add(p.first, p.second);
    myP2.add(p.first, p.second);
}


DoubleVector
Line2D::intersectsAtLengths(const Position2DVector &v)
{
    Position2DVector p = v.intersectsAtPoints(myP1, myP2);
    DoubleVector ret;
    for (size_t i=0; i<p.size(); i++) {
        ret.push_back(GeomHelper::distance(myP1, p[(int) i]));
    }
    return ret;
}


SUMOReal
Line2D::atan2Angle() const
{
    return atan2(myP1.x()-myP2.x(), myP1.y()-myP2.y());
}


SUMOReal
Line2D::atan2DegreeAngle() const
{
    return (SUMOReal) atan2(myP1.x()-myP2.x(), myP1.y()-myP2.y()) *(SUMOReal) 180.0 / (SUMOReal) 3.1415926535897932384626433832795;
}


SUMOReal
Line2D::atan2PositiveAngle() const
{
    SUMOReal angle = atan2Angle();
    if (angle<0) {
        angle = (SUMOReal) PI * (SUMOReal) 2.0 + angle;
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


SUMOReal
Line2D::length() const
{
    return sqrt(
               (myP1.x()-myP2.x())*(myP1.x()-myP2.x())
               +
               (myP1.y()-myP2.y())*(myP1.y()-myP2.y()));
}


void
Line2D::add(SUMOReal x, SUMOReal y)
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
Line2D::sub(SUMOReal x, SUMOReal y)
{
    myP1.sub(x, y);
    myP2.sub(x, y);
}




SUMOReal
Line2D::distanceTo(const Position2D &p) const
{
    SUMOReal LineMag;
    SUMOReal U;

    LineMag = GeomHelper::Magnitude(myP2, myP1);

    U = (((p.x() - myP1.x()) * (myP2.x() - myP1.x())) +
         ((p.y() - myP1.y()) * (myP2.y() - myP1.y())) /*+
                                            ( ( Point->Z - LineStart->Z ) * ( LineEnd->Z - LineStart->Z ) ) )*/
        )
        /
        (LineMag * LineMag);

    if (U < 0.0f || U > 1.0f)
        return -1;   // closest point does not fall within the line segment

    Position2D Intersection(
        myP1.x() + U *(myP2.x() - myP1.x()),
        myP1.y() + U *(myP2.y() - myP1.y()));
//    Intersection.Z = LineStart->Z + U * ( LineEnd->Z - LineStart->Z );

    SUMOReal Distance = GeomHelper::Magnitude(p, Intersection);

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


SUMOReal
Line2D::nearestPositionTo(const Position2D &p)
{
    return GeomHelper::nearest_position_on_line_to_point(myP1, myP2, p);
}


SUMOReal
Line2D::intersectsAtLength(const Line2D &v)
{
    Position2D pos =
        GeomHelper::intersection_position(myP1, myP2, v.myP1, v.myP2);
    return nearestPositionTo(pos);
}


void
Line2D::rotateDegAtP1(double degs)
{
    Position2D p = myP2;
    p.sub(myP1);
    p.reshiftRotate(0, 0, (SUMOReal)(degs*3.1415926535897932384626433832795/180.));
    p.add(myP1);
    myP2 = p;
}


void
Line2D::rotateAround(const Position2D &at, SUMOReal rot)
{
    myP1.add(-at.x(), -at.y());
    myP2.add(-at.x(), -at.y());
    {
        SUMOReal x = myP1.x() * cos(rot) + myP1.y() * sin(rot);
        SUMOReal y = myP1.y() * cos(rot) - myP1.x() * sin(rot);
        myP1 = Position2D(x, y);
    }
    {
        SUMOReal x = myP2.x() * cos(rot) + myP2.y() * sin(rot);
        SUMOReal y = myP2.y() * cos(rot) - myP2.x() * sin(rot);
        myP2 = Position2D(x, y);
    }
    myP1.add(at.x(), at.y());
    myP2.add(at.x(), at.y());
}


/****************************************************************************/

