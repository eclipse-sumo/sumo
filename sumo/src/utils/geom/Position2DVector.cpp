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
// Revision 1.11  2003/08/20 11:47:38  dkrajzew
// bug in sorting the values by x, then y patched
//
// Revision 1.10  2003/08/14 14:05:50  dkrajzew
// functions to process a nodes geometry added
//
// Revision 1.9  2003/07/16 15:38:04  dkrajzew
// some work on computation and handling of geometry information
//
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


Position2DVector::Position2DVector(size_t fieldSize)
    : myCont(fieldSize)
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


Position2DVector
Position2DVector::resettedBy(double x, double y) const
{
    Position2DVector ret;
    for(size_t i=0; i<size(); i++) {
        Position2D pos = at(i);
        pos.add(x, y);
        ret.push_back(pos);
    }
    return ret;
}


void
Position2DVector::resetBy(double x, double y)
{
    for(size_t i=0; i<size(); i++) {
        myCont[i].add(x, y);
    }
}


void
Position2DVector::resetBy(const Position2D &by)
{
    for(size_t i=0; i<size(); i++) {
        myCont[i].add(by.x(), by.y());
    }
}


void
Position2DVector::sortAsPolyCWByAngle()
{
    Position2D c = center();
    std::sort(myCont.begin(), myCont.end(), as_poly_cw_sorter(c));
}


void
Position2DVector::reshiftRotate(double xoff, double yoff, double rot)
{
    for(size_t i=0; i<size(); i++) {
        myCont[i].reshiftRotate(xoff, yoff, rot);
    }
}


Position2DVector::as_poly_cw_sorter::as_poly_cw_sorter(Position2D center)
    : _center(center)
{
}



int
Position2DVector::as_poly_cw_sorter::operator() (const Position2D &p1,
                                                 const Position2D &p2) const
{
    return atan2(p1.x(), p1.y()) < atan2(p2.x(), p2.y());
}



void
Position2DVector::sortByIncreasingXY()
{
    std::sort(myCont.begin(), myCont.end(), increasing_x_y_sorter());
}



Position2DVector::increasing_x_y_sorter::increasing_x_y_sorter()
{
}



int
Position2DVector::increasing_x_y_sorter::operator() (const Position2D &p1,
                                                 const Position2D &p2) const
{
    if(p1.x()!=p2.x()) {
        return p1.x()<p2.x();
    }
    return p1.y()<p2.y();
}



float
Position2DVector::isLeft(const Position2D &P0, const Position2D &P1,
                         const Position2D &P2 ) const
{
    return (P1.x() - P0.x())*(P2.y() - P0.y()) - (P2.x() - P0.x())*(P1.y() - P0.y());
}


// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// Assume that a class is already given for the object:
// Point with coordinates {float x, y;}
//===================================================================
// isLeft(): tests if a point is Left|On|Right of an infinite line.
// Input: three points P0, P1, and P2
// Return: >0 for P2 left of the line through P0 and P1
// =0 for P2 on the line
// <0 for P2 right of the line
// See: the January 2001 Algorithm on Area of Triangles
Position2DVector
Position2DVector::convexHull() const
{
    if(size()==0) {
        return Position2DVector();
    }
    // build the sorted input point-list
    // copy
    Position2DVector inp;
    inp.myCont = myCont;
    // sort by increasing x, then y
    inp.sortByIncreasingXY();
    int n = inp.size();

    // convex-hull algo
    Position2DVector ret(n);
    // the output ret will be used as the stack
    int bot=0, top=(-1); // indices for bottom and top of the stack
    int i; // array scan index

    // Get the indices of points with min x-coord and min|max y-coord
    int minmin = 0, minmax;
    float xmin = inp.at(0).x();
    for (i=1; i<n; i++)
        if (inp.at(i).x() != xmin)
            break;
    minmax = i-1;

    if (minmax == n-1) { // degenerate case: all x-coords == xmin
        ret.set(++top, inp.at(minmin));
        if (inp.at(minmax).y() != inp.at(minmin).y()) // a nontrivial segment
            ret.set(++top, inp.at(minmax));
        ret.set(++top, inp.at(minmin)); // add polygon endpoint
        return top+1;
    }

    // Get the indices of points with max x-coord and min|max y-coord
    int maxmin, maxmax = n-1;
    float xmax = inp.at(n-1).x();
    for (i=n-2; i>=0; i--)
        if (inp.at(i).x() != xmax)
            break;
    maxmin = i+1;

    // Compute the lower hull on the stack H
    ret.set(++top, inp.at(minmin)); // push minmin point onto stack
    i = minmax;
    while (++i <= maxmin)
    {
        // the lower line joins P[minmin] with P[maxmin]
        if (isLeft( inp.at(minmin), inp.at(maxmin), inp.at(i)) >= 0 && i < maxmin)
            continue; // ignore P[i] above or on the lower line

        while (top > 0) // there are at least 2 points on the stack
        {
            // test if P[i] is left of the line at the stack top
            if (isLeft( ret.at(top-1), ret.at(top), inp.at(i)) > 0)
                break; // P[i] is a new hull vertex
            else
                top--; // pop top point off stack
        }
        ret.set(++top, inp.at(i)); // push P[i] onto stack
    }

    // Next, compute the upper hull on the stack H above the bottom hull
    if (maxmax != maxmin) // if distinct xmax points
        ret.set(++top, inp.at(maxmax)); // push maxmax point onto stack
    bot = top; // the bottom point of the upper hull stack
    i = maxmin;
    while (--i >= minmax)
    {
        // the upper line joins P[maxmax] with P[minmax]
        if (isLeft( inp.at(maxmax), inp.at(minmax), inp.at(i)) >= 0 && i > minmax)
            continue; // ignore P[i] below or on the upper line

        while (top > bot) // at least 2 points on the upper stack
        {
            // test if P[i] is left of the line at the stack top
            if (isLeft( ret.at(top-1), ret.at(top), inp.at(i)) > 0)
                break; // P[i] is a new hull vertex
            else
                top--; // pop top point off stack
        }
        ret.set(++top, inp.at(i)); // push P[i] onto stack
    }
    if (minmax != minmin)
        ret.set(++top, inp.at(minmin)); // push joining endpoint onto stack

    Position2DVector rret;
    for(size_t j=0; j<top; j++) {
        rret.push_back(ret.at(j));
    }
    return rret;
}


void
Position2DVector::set(size_t pos, const Position2D &p)
{
    myCont[pos] = p;
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "Position2DVector.icc"
//#endif

// Local Variables:
// mode:C++
// End:


