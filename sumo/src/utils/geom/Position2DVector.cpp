/****************************************************************************/
/// @file    Position2DVector.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A list of 2D-positions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <queue>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utils/common/StdDefs.h>
#include "AbstractPoly.h"
#include "Position2D.h"
#include "Position2DVector.h"
#include "GeomHelper.h"
#include "Line2D.h"
#include "Helper_ConvexHull.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Position2DVector::Position2DVector() throw() {}


Position2DVector::Position2DVector(const std::vector<Position2D> &v) throw() {
    std::copy(v.begin(), v.end(), std::back_inserter(myCont));
}


Position2DVector::~Position2DVector() throw() {}


// ------------ Adding items to the container
void
Position2DVector::push_back(const Position2D &p) throw() {
    myCont.push_back(p);
}


void
Position2DVector::push_back(const Position2DVector &p) throw() {
    copy(p.myCont.begin(), p.myCont.end(), back_inserter(myCont));
}


void
Position2DVector::push_front(const Position2D &p) {
    myCont.push_front(p);
}


bool
Position2DVector::around(const Position2D &p, SUMOReal offset) const {
    if (offset!=0) {
        //throw 1; // !!! not yet implemented
    }
    SUMOReal angle=0;
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
    return (!(fabs(angle) < PI));
}


bool
Position2DVector::overlapsWith(const AbstractPoly &poly, SUMOReal offset) const {
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (poly.around(*i, offset)) {
            return true;
        }
    }
    return false;
}


bool
Position2DVector::intersects(const Position2D &p1, const Position2D &p2) const {
    if (size()<2) {
        return false;
    }
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (GeomHelper::intersects(*i, *(i+1), p1, p2)) {
            return true;
        }
    }
    //return GeomHelper::intersects(*(myCont.end()-1), *(myCont.begin()), p1, p2);
    return false;
}


bool
Position2DVector::intersects(const Position2DVector &v1) const {
    if (size()<2) {
        return false;
    }
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (v1.intersects(*i, *(i+1))) {
            return true;
        }
    }
    //return v1.intersects(*(myCont.end()-1), *(myCont.begin()));
    return false;
}


Position2D
Position2DVector::intersectsAtPoint(const Position2D &p1,
                                    const Position2D &p2) const {
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (GeomHelper::intersects(*i, *(i+1), p1, p2)) {
            return GeomHelper::intersection_position(*i, *(i+1), p1, p2);
        }
    }
    return Position2D(-1, -1);
}


Position2D
Position2DVector::intersectsAtPoint(const Position2DVector &v1) const {
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (v1.intersects(*i, *(i+1))) {
            return v1.intersectsAtPoint(*i, *(i+1));
        }
    }
    /*
    if(v1.intersects(*(myCont.end()-1), *(myCont.begin()))) {
        return v1.intersectsAtPoint(*(myCont.end()-1), *(myCont.begin()));
    }
    */
    return Position2D(-1, -1);
}


void
Position2DVector::clear() {
    myCont.clear();
}


const Position2D &
Position2DVector::operator[](int index) const {
    if (index>=0) {
        return myCont[index];
    } else {
        return myCont[myCont.size()+index];
    }
}


Position2D &
Position2DVector::operator[](int index) {
    if (index>=0) {
        return myCont[index];
    } else {
        return myCont[myCont.size()+index];
    }
}


size_t
Position2DVector::size() const {
    return myCont.size();
}



Position2D
Position2DVector::positionAtLengthPosition(SUMOReal pos) const {
    ContType::const_iterator i=myCont.begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = (*i).distanceTo(*(i+1));
        if (seenLength+nextLength>pos) {
            return positionAtLengthPosition(*i, *(i+1), pos-seenLength);
        }
        seenLength += nextLength;
    } while (++i!=myCont.end()-1);
    return myCont.back();
//    return positionAtLengthPosition(*(myCont.end()-1),
//        *(myCont.begin()), pos-seenLength);
}


SUMOReal
Position2DVector::rotationDegreeAtLengthPosition(SUMOReal pos) const {
    ContType::const_iterator i=myCont.begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = (*i).distanceTo(*(i+1));
        if (seenLength+nextLength>pos) {
            Line2D l(*i, *(i+1));
            return l.atan2DegreeAngle();
        }
        seenLength += nextLength;
    } while (++i!=myCont.end()-1);
    Line2D l(*(myCont.end()-2), *(myCont.end()-1));
    return l.atan2DegreeAngle();
//    Line2D l(*(myCont.end()-1), *(myCont.begin()));
//    return l.atan2DegreeAngle();
}


Position2D
Position2DVector::positionAtLengthPosition(const Position2D &p1,
        const Position2D &p2,
        SUMOReal pos) {
    SUMOReal dist = p1.distanceTo(p2);
    if (dist<pos) {
        return Position2D(-1, -1);
    }
    SUMOReal x = p1.x() + (p2.x() - p1.x()) / dist * pos;
    SUMOReal y = p1.y() + (p2.y() - p1.y()) / dist * pos;
    return Position2D(x, y);
}


Boundary
Position2DVector::getBoxBoundary() const {
    Boundary ret;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end(); i++) {
        ret.add(*i);
    }
    return ret;
}


Position2D
Position2DVector::getPolygonCenter() const {
    SUMOReal x = 0;
    SUMOReal y = 0;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end(); i++) {
        x += (*i).x();
        y += (*i).y();
    }
    return Position2D(x/(SUMOReal) myCont.size(), y/(SUMOReal) myCont.size());
}


Position2D
Position2DVector::getLineCenter() const {
    if (myCont.size()==1) {
        return myCont[0];
    }
    return positionAtLengthPosition(SUMOReal((length()/2.)));
}


SUMOReal
Position2DVector::length() const {
    SUMOReal len = 0;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        len += (*i).distanceTo(*(i+1));
    }
    return len;
}


bool
Position2DVector::partialWithin(const AbstractPoly &poly, SUMOReal offset) const {
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (poly.around(*i, offset)) {
            return true;
        }
    }
    return false;
}



bool
Position2DVector::crosses(const Position2D &p1, const Position2D &p2) const {
    return intersects(p1, p2);
}



const Position2D &
Position2DVector::getBegin() const {
    return myCont[0];
}


const Position2D &
Position2DVector::getEnd() const {
    return myCont.back();
}


std::pair<Position2DVector, Position2DVector>
Position2DVector::splitAt(SUMOReal where) const {
    assert(size()>=2);
    assert(where!=0);
    Position2DVector one, two;
    Position2D last = myCont[0];
    Position2D curr = myCont[0];
    SUMOReal seen = 0;
    SUMOReal currdist = 0;
    one.push_back(myCont[0]);
    ContType::const_iterator i=myCont.begin()+1;

    do {
        last = curr;
        curr = *i;
        currdist = last.distanceTo(curr);
        if (seen+currdist<where&&i!=myCont.begin()+1) {
            one.push_back(last);
        }
        i++;
        seen += currdist;
    } while (seen<where&&i!=myCont.end());
    seen -= currdist;
    i--;

    if (fabs(seen+currdist-where)<POSITION_EPS) {
        one.push_back(curr);
    } else {
        Line2D tmpL(last, curr);
        assert(seen+currdist-where>POSITION_EPS);
        Position2D p = tmpL.getPositionAtDistance(seen+currdist-where);
        one.push_back(p);
        two.push_back(p);
    }

    for (; i!=myCont.end(); i++) {
        two.push_back(*i);
    }
    assert(one.size()>=2);
    assert(two.size()>=2);
    return std::pair<Position2DVector, Position2DVector>(one, two);
}



std::ostream &
operator<<(std::ostream &os, const Position2DVector &geom) {
    for (Position2DVector::ContType::const_iterator i=geom.myCont.begin(); i!=geom.myCont.end(); i++) {
        if (i!=geom.myCont.begin()) {
            os << " ";
        }
        os << (*i).x() << "," << (*i).y();
    }
    return os;
}


void
Position2DVector::sortAsPolyCWByAngle() {
    Position2D c = getPolygonCenter();
    std::sort(myCont.begin(), myCont.end(), as_poly_cw_sorter(c));
}


void
Position2DVector::reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) {
    for (size_t i=0; i<size(); i++) {
        myCont[i].reshiftRotate(xoff, yoff, rot);
    }
}


Position2DVector::as_poly_cw_sorter::as_poly_cw_sorter(Position2D center)
        : myCenter(center) {}



int
Position2DVector::as_poly_cw_sorter::operator()(const Position2D &p1,
        const Position2D &p2) const {
    return atan2(p1.x(), p1.y()) < atan2(p2.x(), p2.y());
}



void
Position2DVector::sortByIncreasingXY() {
    std::sort(myCont.begin(), myCont.end(), increasing_x_y_sorter());
}



Position2DVector::increasing_x_y_sorter::increasing_x_y_sorter() {}



int
Position2DVector::increasing_x_y_sorter::operator()(const Position2D &p1,
        const Position2D &p2) const {
    if (p1.x()!=p2.x()) {
        return p1.x()<p2.x();
    }
    return p1.y()<p2.y();
}



SUMOReal
Position2DVector::isLeft(const Position2D &P0, const Position2D &P1,
                         const Position2D &P2) const {
    return (P1.x() - P0.x())*(P2.y() - P0.y()) - (P2.x() - P0.x())*(P1.y() - P0.y());
}


Position2DVector
Position2DVector::convexHull() const {
    Position2DVector ret = *this;
    ret.sortAsPolyCWByAngle();
    return simpleHull_2D(ret);
}


void
Position2DVector::set(size_t pos, const Position2D &p) {
    myCont[pos] = p;
}



Position2DVector
Position2DVector::intersectsAtPoints(const Position2D &p1,
                                     const Position2D &p2) const {
    Position2DVector ret;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        if (GeomHelper::intersects(*i, *(i+1), p1, p2)) {
            ret.push_back_noDoublePos(GeomHelper::intersection_position(*i, *(i+1), p1, p2));
        }
    }
    return ret;
}


int
Position2DVector::appendWithCrossingPoint(const Position2DVector &v) {
    if (myCont.back().distanceTo(v.myCont[0])<2) { // !!! heuristic
        copy(v.myCont.begin()+1, v.myCont.end(), back_inserter(myCont));
        return 1;
    }
    //
    Line2D l1(myCont[myCont.size()-2], myCont.back());
    l1.extrapolateBy(100);
    Line2D l2(v.myCont[0], v.myCont[1]);
    l2.extrapolateBy(100);
    if (l1.intersects(l2)&&l1.intersectsAtLength(l2)<l1.length()-100) { // !!! heuristic
        Position2D p = l1.intersectsAt(l2);
        myCont[myCont.size()-1] = p;
        copy(v.myCont.begin()+1, v.myCont.end(), back_inserter(myCont));
        return 2;
    } else {
        copy(v.myCont.begin(), v.myCont.end(), back_inserter(myCont));
        return 3;
    }
}


Position2DVector
Position2DVector::getSubpart(SUMOReal begin, SUMOReal end) const {
    Position2DVector ret;
    Position2D begPos = positionAtLengthPosition(begin);
    Position2D endPos = myCont.back();
    if (length()>end) {
        endPos = positionAtLengthPosition(end);
    }
    ret.push_back(begPos);

    SUMOReal seen = 0;
    ContType::const_iterator i = myCont.begin();
    // skip previous segments
    while ((i+1)!=myCont.end()
            &&
            seen+(*i).distanceTo(*(i+1))<begin) {
        seen += (*i).distanceTo(*(i+1));
        i++;
    }
    // append segments in between
    while ((i+1)!=myCont.end()
            &&
            seen+(*i).distanceTo(*(i+1))<end) {

        ret.push_back_noDoublePos(*(i+1));
        /*
        if(ret.at(-1)!=*(i+1)) {
            ret.push_back(*(i+1));
        }
        */
        seen += (*i).distanceTo(*(i+1));
        i++;
    }
    // append end
    ret.push_back_noDoublePos(endPos);
    return ret;
}


void
Position2DVector::pruneFromBeginAt(const Position2D &p) {
    // find minimum distance (from the begin)
    size_t pos = 0;
    SUMOReal dist = 1000000;
    size_t currPos = 0;
    SUMOReal currDist = GeomHelper::distancePointLine(p,
                        GeomHelper::extrapolate_first(*(myCont.begin()), *(myCont.begin()+1), 100),
                        *(myCont.begin()+1));
//    assert(currDist>=0);
    if (currDist>=0&&currDist<dist) {
        dist = currDist;
        pos = currPos;
    }

    for (ContType::iterator i=myCont.begin(); i!=myCont.end()-1; i++, currPos++) {
        SUMOReal currDist = GeomHelper::distancePointLine(p, *i, *(i+1));
        if (currDist>=0&&currDist<dist) {
            dist = currDist;
            pos = currPos;
        }
    }
    // remove leading items
    for (size_t j=0; j<pos; j++) {
        myCont.erase(myCont.begin());
    }
    // replace first item by the new position
    SUMOReal lpos = GeomHelper::nearest_position_on_line_to_point(
                        myCont[0], myCont[1], p);
    if (lpos==-1) {
        return;
    }
    Position2D np = positionAtLengthPosition(lpos);
    if (np!=*(myCont.begin())) {
        myCont.erase(myCont.begin());
        if (np!=*(myCont.begin())) {
            myCont.push_front(np);
            assert(myCont.size()>1);
            assert(*(myCont.begin())!=*(myCont.end()-1));
        }
    }
}


void
Position2DVector::pruneFromEndAt(const Position2D &p) {
    // find minimum distance (from the end)
    size_t pos = 0;
    SUMOReal dist = 1000000;
    size_t currPos = 0;
    SUMOReal currDist = GeomHelper::distancePointLine(p,
                        *(myCont.end()-1),
                        GeomHelper::extrapolate_second(*(myCont.end()-2), *(myCont.end()-1), 100));
//    assert(currDist>=0);
    if (currDist>=0&&currDist<dist) {
        dist = currDist;
        pos = currPos;
    }

    for (ContType::reverse_iterator i=myCont.rbegin(); i!=myCont.rend()-1; i++, currPos++) {
        SUMOReal currDist = GeomHelper::distancePointLine(p, *(i), *(i+1));
        if (currDist>=0&&currDist<dist) {
            dist = currDist;
            pos = currPos;
        }
    }
    // remove trailing items
    for (size_t j=0; j<pos; j++) {
        myCont.erase(myCont.end()-1);
    }
    // replace last item by the new position
    SUMOReal lpos =
        GeomHelper::nearest_position_on_line_to_point(
            myCont[myCont.size()-1], myCont[myCont.size()-2], p);
    if (lpos==-1) {
        return;
    }
    Position2D np = positionAtLengthPosition(
                        length() - lpos);
    if (np!=*(myCont.end()-1)) {
        myCont.erase(myCont.end()-1);
        if (np!=*(myCont.end()-1)) {
            myCont.push_back(np);
            assert(myCont.size()>1);
            assert(*(myCont.begin())!=*(myCont.end()-1));
        }
    }
}


SUMOReal
Position2DVector::beginEndAngle() const {
    Line2D tmp(getBegin(), getEnd());
    return tmp.atan2Angle();
}


void
Position2DVector::eraseAt(int i) {
    if (i>=0) {
        myCont.erase(myCont.begin()+i);
    } else {
        myCont.erase(myCont.end()+i);
    }
}


SUMOReal
Position2DVector::nearest_position_on_line_to_point(const Position2D &p) const {
    SUMOReal shortestDist = -1;
    SUMOReal nearestPos = -1;
    SUMOReal seen = 0;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        SUMOReal pos =
            GeomHelper::nearest_position_on_line_to_point(*i, *(i+1), p);
        SUMOReal dist =
            pos < 0 ? -1 : p.distanceTo(positionAtLengthPosition(pos+seen));
        //
        if (dist>=0&&(shortestDist<0||shortestDist>dist)) {
            nearestPos = pos+seen;
            shortestDist = dist;
        }
        seen += (*i).distanceTo(*(i+1));
        //
    }
    return nearestPos;
}


SUMOReal
Position2DVector::distance(const Position2D &p) const {
    Position2D outIntersection = Position2D();
    SUMOReal minDist = std::numeric_limits<double>::max();
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        minDist = MIN2(minDist, GeomHelper::closestDistancePointLine(
                p, *i, *(i+1), outIntersection));
    }
    return minDist;
}


DoubleVector
Position2DVector::intersectsAtLengths(const Position2DVector &s) const {
    DoubleVector ret;
    SUMOReal pos = 0;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        Line2D l((*i), *(i+1));
        DoubleVector atSegment = l.intersectsAtLengths(s);
        VectorHelper<SUMOReal>::add2All(atSegment, pos);
        copy(atSegment.begin(), atSegment.end(), back_inserter(ret));
        pos += l.length();
    }
    return ret;
}


DoubleVector
Position2DVector::intersectsAtLengths(const Line2D &s) const {
    DoubleVector ret;
    SUMOReal pos = 0;
    for (ContType::const_iterator i=myCont.begin(); i!=myCont.end()-1; i++) {
        Line2D l((*i), *(i+1));
        if (GeomHelper::intersects(l.p1(), l.p2(), s.p1(), s.p2())) {
            Position2D p =
                GeomHelper::intersection_position(l.p1(), l.p2(), s.p1(), s.p2());
            SUMOReal atLength = p.distanceTo(l.p1());
            ret.push_back(atLength+pos);
        }
        pos += l.length();
    }
    return ret;
}


void
Position2DVector::extrapolate(SUMOReal val) {
    assert(myCont.size()>1);
    Position2D nb =
        GeomHelper::extrapolate_first(myCont[0], myCont[1], val);
    Position2D ne =
        GeomHelper::extrapolate_second(
            myCont[myCont.size()-2], myCont[myCont.size()-1], val);
    myCont.erase(myCont.begin());
    push_front(nb);
    myCont.erase(myCont.end()-1);
    push_back(ne);
}


Position2DVector
Position2DVector::reverse() const {
    Position2DVector ret;
    for (ContType::const_reverse_iterator i=myCont.rbegin(); i!=myCont.rend(); i++) {
        ret.push_back(*i);
    }
    return ret;
}


void
Position2DVector::move2side(SUMOReal amount, int index) {
    if (index<0) {
        index = (int) myCont.size() + index;
    }
    if (/*i==myGeom.size()-2||*/index==0) {
        Position2D from = myCont[index];
        Position2D to = myCont[index+1];
        std::pair<SUMOReal, SUMOReal> offsets = GeomHelper::getNormal90D_CW(from, to, amount);
        myCont[index] = Position2D(from.x()-offsets.first, from.y()-offsets.second);
    } else if (index==(int) myCont.size()-1) {
        Position2D from = myCont[index-1];
        Position2D to = myCont[index];
        std::pair<SUMOReal, SUMOReal> offsets = GeomHelper::getNormal90D_CW(from, to, amount);
        myCont[index] = Position2D(to.x()-offsets.first, to.y()-offsets.second);
    } else {
        Position2D from = myCont[index-1];
        Position2D me = myCont[index];
        Position2D to = myCont[index+1];
        std::pair<SUMOReal, SUMOReal> offsets = GeomHelper::getNormal90D_CW(from, me, amount);
        std::pair<SUMOReal, SUMOReal> offsets2 = GeomHelper::getNormal90D_CW(me, to, amount);
        Line2D l1(
            Position2D(from.x()-offsets.first, from.y()-offsets.second),
            Position2D(me.x()-offsets.first, me.y()-offsets.second));
        l1.extrapolateBy(100);
        Line2D l2(
            Position2D(me.x()-offsets2.first, me.y()-offsets2.second),
            Position2D(to.x()-offsets2.first, to.y()-offsets2.second));
        l2.extrapolateBy(100);
        if (l1.intersects(l2)) {
            myCont[index] = l1.intersectsAt(l2);
        } else {
            throw InvalidArgument("no line intersection");
        }
    }

}

void
Position2DVector::move2side(SUMOReal amount) {
    if (myCont.size()<2) {
        return;
    }
    Position2DVector shape;
    for (size_t i=0; i<myCont.size(); i++) {
        if (/*i==myGeom.size()-2||*/i==0) {
            Position2D from = myCont[i];
            Position2D to = myCont[i+1];
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(//.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position2D(from.x()-offsets.first, from.y()-offsets.second));
        } else if (i==myCont.size()-1) {
            Position2D from = myCont[i-1];
            Position2D to = myCont[i];
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(//.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position2D(to.x()-offsets.first, to.y()-offsets.second));
        } else {
            Position2D from = myCont[i-1];
            Position2D me = myCont[i];
            Position2D to = myCont[i+1];
            double sinAngle = sin(GeomHelper::Angle2D(from.x()-me.x(), from.y()-me.y(),
                                  me.x()-to.x(), me.y()-to.y())/2);
            double maxDev = 2 * (from.distanceTo(me) + me.distanceTo(to)) * sinAngle;
            if (fabs(maxDev)<POSITION_EPS) {
                // parallel case, just shift the middle point
                std::pair<SUMOReal, SUMOReal> off =
                    GeomHelper::getNormal90D_CW(from, to, amount);
                shape.push_back(Position2D(me.x()-off.first, me.y()-off.second));
                continue;
            }
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, me, amount);
            std::pair<SUMOReal, SUMOReal> offsets2 =
                GeomHelper::getNormal90D_CW(me, to, amount);
            Line2D l1(
                Position2D(from.x()-offsets.first, from.y()-offsets.second),
                Position2D(me.x()-offsets.first, me.y()-offsets.second));
            l1.extrapolateBy(100);
            Line2D l2(
                Position2D(me.x()-offsets2.first, me.y()-offsets2.second),
                Position2D(to.x()-offsets2.first, to.y()-offsets2.second));
            l2.extrapolateBy(100);
            if (l1.intersects(l2)) {
                shape.push_back(//.push_back(
                    // (methode umbenennen; was heisst hier "-")
                    l1.intersectsAt(l2));
            } else {
                throw InvalidArgument("no line intersection");
            }
        }
    }

    /*
    ContType newCont;
    std::pair<SUMOReal, SUMOReal> p;
    Position2D newPos;
    // first point
    newPos = (*(myCont.begin()));
    p = GeomHelper::getNormal90D_CW(*(myCont.begin()), *(myCont.begin()+1), amount);
    newPos.add(p.first, p.second);
    newCont.push_back(newPos);
    // middle points
    for(ContType::const_iterator i=myCont.begin()+1; i!=myCont.end()-1; i++) {
        std::pair<SUMOReal, SUMOReal> oldp = p;
        newPos = *i;
        newPos.add(p.first, p.second);
        newCont.push_back(newPos);
        p = GeomHelper::getNormal90D_CW(*i, *(i+1), amount);
    //        Position2D newPos(*i);
    //        newPos.add((p.first+oldp.first)/2.0, (p.second+oldp.second)/2.0);
    //        newCont.push_back(newPos);
    }
    // last point
    newPos = (*(myCont.end()-1));
    newPos.add(p.first, p.second);
    newCont.push_back(newPos);
    myCont = newCont;
    */
    myCont = shape.myCont;
}


Line2D
Position2DVector::lineAt(size_t pos) const {
    assert(myCont.size()>pos+1);
    return Line2D(myCont[pos], myCont[pos+1]);
}


Line2D
Position2DVector::getBegLine() const {
    return lineAt(0);
}


Line2D
Position2DVector::getEndLine() const {
    return lineAt(myCont.size()-2);
}


void
Position2DVector::closePolygon() {
    if (myCont[0]==myCont.back()) {
        return;
    }
    push_back(myCont[0]);
}


DoubleVector
Position2DVector::distances(const Position2DVector &s) const {
    DoubleVector ret;
    ContType::const_iterator i;
    for (i=myCont.begin(); i!=myCont.end(); i++) {
        ret.push_back(s.distance(*i));        
    }
    for (i=s.myCont.begin(); i!=s.myCont.end(); i++) {
        ret.push_back(distance(*i));        
    }
    return ret;
}


Position2D
Position2DVector::pop_back() {
    Position2D last = myCont.back();
    myCont.erase(myCont.end()-1);
    return last;
}


Position2D
Position2DVector::pop_front() {
    Position2D first = myCont.front();
    myCont.erase(myCont.begin());
    return first;
}


void
Position2DVector::insertAt(int index, const Position2D &p) {
    if (index>=0) {
        myCont.insert(myCont.begin()+index, p);
    } else {
        myCont.insert(myCont.end()+index, p);
    }
}


void
Position2DVector::push_back_noDoublePos(const Position2D &p) {
    if (size()==0 || !p.almostSame(myCont.back())) {
        myCont.push_back(p);
    }
}


void
Position2DVector::push_front_noDoublePos(const Position2D &p) {
    if (size()==0 || !p.almostSame(myCont.front())) {
        myCont.push_front(p);
    }
}


void
Position2DVector::replaceAt(size_t index, const Position2D &by) {
    assert(size()>index);
    myCont[index] = by;
}


bool
Position2DVector::isClosed() const {
    return myCont.size()>=2&&myCont[0]==myCont.back();
}


void
Position2DVector::removeDoublePoints() {
    if (myCont.size() > 1) {
        ContType::iterator last = myCont.begin();
        for (ContType::iterator i=myCont.begin()+1; i!=myCont.end();) {
            if (last->almostSame(*i)) {
                i = myCont.erase(i);
            } else {
                last = i;
                ++i;
            }
        }
    }
}


void
Position2DVector::removeColinearPoints() {
    if (myCont.size() > 2) {
        Position2D& last = myCont.front();
        for (ContType::iterator i=myCont.begin()+1; i!=myCont.end()-1;) {
            if (GeomHelper::distancePointLine(*i, last, *(i+1)) < 0.001) {
                i = myCont.erase(i);
            } else {
                last = *i;
                ++i;
            }
        }
    }
}


/****************************************************************************/

