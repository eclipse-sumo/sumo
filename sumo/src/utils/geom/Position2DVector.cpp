//---------------------------------------------------------------------------//
//                        Position2DVector.cpp -
//  A list of 2D-positions
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
// Revision 1.8  2003/07/07 08:48:35  dkrajzew
// remved an unneeded debug-output
//
// Revision 1.7  2003/06/05 14:33:45  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <queue>
#include <cmath>
#include <iostream>
#include "AbstractPoly.h"
#include "Position2D.h"
#include "Position2DVector.h"
#include "GeomHelper.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
Position2DVector::Position2DVector()
{
}


Position2DVector::~Position2DVector()
{
}


void
Position2DVector::push_back(const Position2D &p)
{
    myCont.push_back(p);
}


void
Position2DVector::push_front(const Position2D &p)
{
    myCont.push_front(p);
}


bool
Position2DVector::around(const Position2D &p, double offset) const
{
    if(offset!=0) {
        //throw 1; // !!! not yet implemented
    }
    double angle=0;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
       Position2D p1(
           (*i).x() - p.x(),
           (*i).y() - p.y());
       Position2D p2(
           (*(i+1)).x() - p.x(),
           (*(i+1)).y() - p.y());
       angle += GeomHelper::Angle2D(p1.x(), p1.y(), p2.x(), p2.y());
    }
    Position2D p1(
        (*(myCont.end()-1)).x() - p.x(),
        (*(myCont.end()-1)).y() - p.y());
    Position2D p2(
        (*(myCont.begin())).x() - p.x(),
        (*(myCont.begin())).y() - p.y());
    angle += GeomHelper::Angle2D(p1.x(), p1.y(), p2.x(), p2.y());
    return (!(fabs(angle) < 3.1415926535897932384626433832795));
}


bool
Position2DVector::overlapsWith(const AbstractPoly &poly, double offset) const
{
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if(poly.around(*i, offset)) {
            return true;
        }
    }
    return false;
}


bool
Position2DVector::intersects(const Position2D &p1, const Position2D &p2) const
{
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if(GeomHelper::intersects(*i, *(i+1), p1, p2)) {
            return true;
        }
    }
    return GeomHelper::intersects(*(myCont.end()-1), *(myCont.begin()), p1, p2);
}


bool
Position2DVector::intersects(const Position2DVector &v1) const
{
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if(v1.intersects(*i, *(i+1))) {
            return true;
        }
    }
    return v1.intersects(*(myCont.end()-1), *(myCont.begin()));
}


Position2D
Position2DVector::intersectsAtPoint(const Position2D &p1,
                                    const Position2D &p2) const
{
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if(GeomHelper::intersects(*i, *(i+1), p1, p2)) {
            return GeomHelper::intersection_position(*i, *(i+1), p1, p2);
        }
    }
    if(GeomHelper::intersects(*(myCont.end()-1), *(myCont.begin()), p1, p2)) {
        return GeomHelper::intersection_position(
            *(myCont.end()-1), *(myCont.begin()), p1, p2);
    }
    return Position2D(-1, -1);
}


Position2D
Position2DVector::intersectsAtPoint(const Position2DVector &v1) const
{
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if(v1.intersects(*i, *(i+1))) {
            return v1.intersectsAtPoint(*i, *(i+1));
        }
    }
    if(v1.intersects(*(myCont.end()-1), *(myCont.begin()))) {
        return v1.intersectsAtPoint(*(myCont.end()-1), *(myCont.begin()));
    }
    return Position2D(-1, -1);
}


void
Position2DVector::clear()
{
    myCont.clear();
}


const Position2D &
Position2DVector::at(size_t i) const
{
    return myCont[i];
}


size_t
Position2DVector::size() const
{
    return myCont.size();
}



Position2D
Position2DVector::positionAtLengthPosition(double pos) const
{
    ContType::const_iterator i=myCont.begin();
    double seenLength = 0;
    do {
        double nextLength = GeomHelper::distance(*i, *(i+1));
        if(seenLength+nextLength>pos) {
            return positionAtLengthPosition(*i, *(i+1), pos-seenLength);
        }
        seenLength += nextLength;
    } while(++i!=myCont.end()-1);
    return positionAtLengthPosition(*(myCont.end()-1),
        *(myCont.begin()), pos-seenLength);
}


Position2D
Position2DVector::positionAtLengthPosition(const Position2D &p1,
                                           const Position2D &p2,
                                           double pos)
{
    double dist = GeomHelper::distance(p1, p2);
    if(dist<pos) {
        return Position2D(-1, -1);
    }
    double x = p1.x() + (p2.x() - p1.x()) / dist * pos;
    double y = p1.y() + (p2.y() - p1.y()) / dist * pos;
    return Position2D(x, y);
}


Boundery
Position2DVector::getBoxBoundery() const
{
    Boundery ret;
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end(); i++) {
        ret.add(*i);
    }
    return ret;
}


Position2D
Position2DVector::center() const
{
    double x = 0;
    double y = 0;
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end(); i++) {
        x += (*i).x();
        y += (*i).y();
    }
    return Position2D(x/(double) myCont.size(), y/(double) myCont.size());
}


double
Position2DVector::length() const
{
    double len = 0;
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        len += GeomHelper::distance(*i, *(i+1));
    }
    return len;
}


bool
Position2DVector::partialWithin(const AbstractPoly &poly, double offset) const
{
    for(ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if(poly.around(*i, offset)) {
            return true;
        }
    }
    return false;
}



bool
Position2DVector::crosses(const Position2D &p1, const Position2D &p2) const
{
    return intersects(p1, p2);
}



const Position2D &
Position2DVector::getBegin() const
{
    return myCont[0];
}


const Position2D &
Position2DVector::getEnd() const
{
    return myCont[myCont.size()-1];
}


std::pair<Position2DVector, Position2DVector>
Position2DVector::splitAt(double where) const
{
    Position2DVector one;
    double tmp = 0;
    ContType::const_iterator i=myCont.begin();
    Position2D last = myCont[0];
    for(; i!=myCont.end()&&tmp<where; i++) {
        Position2D curr = (*i);
        if(i!=myCont.begin()) {
            tmp += GeomHelper::distance(last, curr);
        }
        if(tmp<where) {
            one.push_back(*i);
        }
        last = curr;
    }
    Position2DVector two;
    two.push_back(last);
    for(; i!=myCont.end(); i++) {
        two.push_back(*i);
    }
    return std::pair<Position2DVector, Position2DVector>(one, two);
}



std::ostream &
operator<<(std::ostream &os, const Position2DVector &geom)
{
    for(Position2DVector::ContType::const_iterator i=geom.myCont.begin(); i!=geom.myCont.end(); i++) {
        if(i!=geom.myCont.begin()) {
            os << " ";
        }
        os << (*i).x() << "," << (*i).y();
    }
    return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "Position2DVector.icc"
//#endif

// Local Variables:
// mode:C++
// End:


