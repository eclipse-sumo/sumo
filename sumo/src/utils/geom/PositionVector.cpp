/****************************************************************************/
/// @file    PositionVector.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Sept 2002
/// @version $Id$
///
// A list of positions
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

#include <queue>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "AbstractPoly.h"
#include "Position.h"
#include "PositionVector.h"
#include "GeomHelper.h"
#include "Line.h"
#include "Helper_ConvexHull.h"
#include "Boundary.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
PositionVector::PositionVector() {}


PositionVector::PositionVector(const std::vector<Position>& v) {
    std::copy(v.begin(), v.end(), std::back_inserter(myCont));
}


PositionVector::~PositionVector() {}


// ------------ Adding items to the container
void
PositionVector::push_back(const Position& p) {
    myCont.push_back(p);
}


void
PositionVector::push_back(const PositionVector& p) {
    copy(p.myCont.begin(), p.myCont.end(), back_inserter(myCont));
}


void
PositionVector::push_front(const Position& p) {
    myCont.insert(myCont.begin(), p);
}


bool
PositionVector::around(const Position& p, SUMOReal offset) const {
    if (offset != 0) {
        //throw 1; // !!! not yet implemented
    }
    SUMOReal angle = 0;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        Position p1(
            (*i).x() - p.x(),
            (*i).y() - p.y());
        Position p2(
            (*(i + 1)).x() - p.x(),
            (*(i + 1)).y() - p.y());
        angle += GeomHelper::Angle2D(p1.x(), p1.y(), p2.x(), p2.y());
    }
    Position p1(
        (*(myCont.end() - 1)).x() - p.x(),
        (*(myCont.end() - 1)).y() - p.y());
    Position p2(
        (*(myCont.begin())).x() - p.x(),
        (*(myCont.begin())).y() - p.y());
    angle += GeomHelper::Angle2D(p1.x(), p1.y(), p2.x(), p2.y());
    return (!(fabs(angle) < PI));
}


bool
PositionVector::overlapsWith(const AbstractPoly& poly, SUMOReal offset) const {
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (poly.around(*i, offset)) {
            return true;
        }
    }
    return false;
}


bool
PositionVector::intersects(const Position& p1, const Position& p2) const {
    if (size() < 2) {
        return false;
    }
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (GeomHelper::intersects(*i, *(i + 1), p1, p2)) {
            return true;
        }
    }
    //return GeomHelper::intersects(*(myCont.end()-1), *(myCont.begin()), p1, p2);
    return false;
}


bool
PositionVector::intersects(const PositionVector& v1) const {
    if (size() < 2) {
        return false;
    }
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (v1.intersects(*i, *(i + 1))) {
            return true;
        }
    }
    //return v1.intersects(*(myCont.end()-1), *(myCont.begin()));
    return false;
}


Position
PositionVector::intersectsAtPoint(const Position& p1,
                                  const Position& p2) const {
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (GeomHelper::intersects(*i, *(i + 1), p1, p2)) {
            return GeomHelper::intersection_position2D(*i, *(i + 1), p1, p2);
        }
    }
    return Position(-1, -1);
}


Position
PositionVector::intersectsAtPoint(const PositionVector& v1) const {
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (v1.intersects(*i, *(i + 1))) {
            return v1.intersectsAtPoint(*i, *(i + 1));
        }
    }
    /*
    if(v1.intersects(*(myCont.end()-1), *(myCont.begin()))) {
        return v1.intersectsAtPoint(*(myCont.end()-1), *(myCont.begin()));
    }
    */
    return Position(-1, -1);
}


void
PositionVector::clear() {
    myCont.clear();
}


const Position&
PositionVector::operator[](int index) const {
    if (index >= 0) {
        return myCont[index];
    } else {
        return myCont[myCont.size() + index];
    }
}


Position&
PositionVector::operator[](int index) {
    if (index >= 0) {
        return myCont[index];
    } else {
        return myCont[myCont.size() + index];
    }
}


size_t
PositionVector::size() const {
    return myCont.size();
}


Position
PositionVector::positionAtLengthPosition(SUMOReal pos) const {
    ContType::const_iterator i = myCont.begin();
    SUMOReal seenLength = 0;
    do {
        const SUMOReal nextLength = (*i).distanceTo(*(i + 1));
        if (seenLength + nextLength > pos) {
            return positionAtLengthPosition(*i, *(i + 1), pos - seenLength);
        }
        seenLength += nextLength;
    } while (++i != myCont.end() - 1);
    return myCont.back();
}


Position
PositionVector::positionAtLengthPosition2D(SUMOReal pos) const {
    ContType::const_iterator i = myCont.begin();
    SUMOReal seenLength = 0;
    do {
        const SUMOReal nextLength = (*i).distanceTo2D(*(i + 1));
        if (seenLength + nextLength > pos) {
            return positionAtLengthPosition2D(*i, *(i + 1), pos - seenLength);
        }
        seenLength += nextLength;
    } while (++i != myCont.end() - 1);
    return myCont.back();
}


SUMOReal
PositionVector::rotationDegreeAtLengthPosition(SUMOReal pos) const {
    ContType::const_iterator i = myCont.begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = (*i).distanceTo(*(i + 1));
        if (seenLength + nextLength > pos) {
            Line l(*i, *(i + 1));
            return l.atan2DegreeAngle();
        }
        seenLength += nextLength;
    } while (++i != myCont.end() - 1);
    Line l(*(myCont.end() - 2), *(myCont.end() - 1));
    return l.atan2DegreeAngle();
}


SUMOReal
PositionVector::tiltDegreeAtLengthPosition(SUMOReal pos) const {
    ContType::const_iterator i = myCont.begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = (*i).distanceTo(*(i + 1));
        if (seenLength + nextLength > pos) {
            Line l(*i, *(i + 1));
            return l.atan2TiltDegree();
        }
        seenLength += nextLength;
    } while (++i != myCont.end() - 1);
    Line l(*(myCont.end() - 2), *(myCont.end() - 1));
    return l.atan2TiltDegree();
}


Position
PositionVector::positionAtLengthPosition(const Position& p1,
        const Position& p2,
        SUMOReal pos) {
    const SUMOReal dist = p1.distanceTo(p2);
    if (dist < pos) {
        return Position(-1, -1);
    }
    return p1 + (p2 - p1) * (pos / dist);
}


Position
PositionVector::positionAtLengthPosition2D(const Position& p1,
        const Position& p2,
        SUMOReal pos) {
    const SUMOReal dist = p1.distanceTo2D(p2);
    if (dist < pos) {
        return Position(-1, -1);
    }
    return p1 + (p2 - p1) * (pos / dist);
}


Boundary
PositionVector::getBoxBoundary() const {
    Boundary ret;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end(); i++) {
        ret.add(*i);
    }
    return ret;
}


Position
PositionVector::getPolygonCenter() const {
    SUMOReal x = 0;
    SUMOReal y = 0;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end(); i++) {
        x += (*i).x();
        y += (*i).y();
    }
    return Position(x / (SUMOReal) myCont.size(), y / (SUMOReal) myCont.size());
}


Position
PositionVector::getCentroid() const {
    PositionVector tmp = *this;
    if (!isClosed()) { // make sure its closed
        tmp.push_back(tmp[0]);
    }
    const int endIndex = (int)tmp.size() - 1;
    SUMOReal div = 0; // 6 * area including sign
    SUMOReal x = 0;
    SUMOReal y = 0;
    if (tmp.area() != 0) { // numerical instability ?
        // http://en.wikipedia.org/wiki/Polygon
        for (int i = 0; i < endIndex; i++) {
            const SUMOReal z = tmp[i].x() * tmp[i + 1].y() - tmp[i + 1].x() * tmp[i].y();
            div += z; // area formula
            x += (tmp[i].x() + tmp[i + 1].x()) * z;
            y += (tmp[i].y() + tmp[i + 1].y()) * z;
        }
        div *= 3; //  6 / 2, the 2 comes from the area formula
        return Position(x / div, y / div);
    } else {
        // compute by decomposing into line segments
        // http://en.wikipedia.org/wiki/Centroid#By_geometric_decomposition
        SUMOReal lengthSum = 0;
        for (int i = 0; i < endIndex; i++) {
            SUMOReal length = tmp[i].distanceTo(tmp[i + 1]);
            x += (tmp[i].x() + tmp[i + 1].x()) * length / 2;
            y += (tmp[i].y() + tmp[i + 1].y()) * length / 2;
            lengthSum += length;
        }
        return Position(x / lengthSum, y / lengthSum);
    }
}


void
PositionVector::scaleSize(SUMOReal factor) {
    Position centroid = getCentroid();
    for (size_t i = 0; i < size(); i++) {
        myCont[i] = centroid + ((myCont[i] - centroid) * factor);
    }
}


Position
PositionVector::getLineCenter() const {
    if (myCont.size() == 1) {
        return myCont[0];
    }
    return positionAtLengthPosition(SUMOReal((length() / 2.)));
}


SUMOReal
PositionVector::length() const {
    SUMOReal len = 0;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        len += (*i).distanceTo(*(i + 1));
    }
    return len;
}


SUMOReal
PositionVector::area() const {
    SUMOReal area = 0;
    PositionVector tmp = *this;
    if (!isClosed()) { // make sure its closed
        tmp.push_back(tmp[0]);
    }
    const int endIndex = (int)tmp.size() - 1;
    // http://en.wikipedia.org/wiki/Polygon
    for (int i = 0; i < endIndex; i++) {
        area += tmp[i].x() * tmp[i + 1].y() - tmp[i + 1].x() * tmp[i].y();
    }
    if (area < 0) { // we whether we had cw or ccw order
        area *= -1;
    }
    return area / 2;
}


bool
PositionVector::partialWithin(const AbstractPoly& poly, SUMOReal offset) const {
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (poly.around(*i, offset)) {
            return true;
        }
    }
    return false;
}



bool
PositionVector::crosses(const Position& p1, const Position& p2) const {
    return intersects(p1, p2);
}



const Position&
PositionVector::getBegin() const {
    return myCont[0];
}


const Position&
PositionVector::getEnd() const {
    return myCont.back();
}


std::pair<PositionVector, PositionVector>
PositionVector::splitAt(SUMOReal where) const {
    if (size() < 2) {
        throw InvalidArgument("Vector to short for splitting");
    }
    if (where <= POSITION_EPS || where >= length() - POSITION_EPS) {
        WRITE_WARNING("Splitting vector close to end (pos: " + toString(where) + ", length: " + toString(length()) + ")");
    }
    PositionVector first, second;
    first.push_back(myCont[0]);
    SUMOReal seen = 0;
    ContType::const_iterator it = myCont.begin() + 1;
    SUMOReal next = first.getEnd().distanceTo(*it);
    // see how many points we can add to first
    while (where >= seen + next + POSITION_EPS) {
        seen += next;
        first.push_back(*it);
        it++;
        next = first.getEnd().distanceTo(*it);
    }
    if (fabs(where - (seen + next)) > POSITION_EPS || it == myCont.end() - 1) {
        // we need to insert a new point because 'where' is not close to an
        // existing point or it is to close to the endpoint
        Line tmpL(first.getEnd(), *it);
        Position p = tmpL.getPositionAtDistance(where - seen);
        first.push_back(p);
        second.push_back(p);
    } else {
        first.push_back(*it);
    }
    // add the remaining points to second
    for (; it != myCont.end(); it++) {
        second.push_back(*it);
    }
    assert(first.size() >= 2);
    assert(second.size() >= 2);
    assert(first.getEnd() == second.getBegin());
    assert(fabs(first.length() + second.length() - length()) < 2 * POSITION_EPS);
    return std::pair<PositionVector, PositionVector>(first, second);
}


std::ostream&
operator<<(std::ostream& os, const PositionVector& geom) {
    for (PositionVector::ContType::const_iterator i = geom.myCont.begin(); i != geom.myCont.end(); i++) {
        if (i != geom.myCont.begin()) {
            os << " ";
        }
        os << (*i);
    }
    return os;
}


void
PositionVector::sortAsPolyCWByAngle() {
    std::sort(myCont.begin(), myCont.end(), as_poly_cw_sorter());
}


void
PositionVector::add(SUMOReal xoff, SUMOReal yoff, SUMOReal zoff) {
    for (size_t i = 0; i < size(); i++) {
        myCont[i].add(xoff, yoff, zoff);
    }
}


void
PositionVector::reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) {
    for (size_t i = 0; i < size(); i++) {
        myCont[i].reshiftRotate(xoff, yoff, rot);
    }
}


int
PositionVector::as_poly_cw_sorter::operator()(const Position& p1,
        const Position& p2) const {
    return atan2(p1.x(), p1.y()) < atan2(p2.x(), p2.y());
}



void
PositionVector::sortByIncreasingXY() {
    std::sort(myCont.begin(), myCont.end(), increasing_x_y_sorter());
}



PositionVector::increasing_x_y_sorter::increasing_x_y_sorter() {}



int
PositionVector::increasing_x_y_sorter::operator()(const Position& p1,
        const Position& p2) const {
    if (p1.x() != p2.x()) {
        return p1.x() < p2.x();
    }
    return p1.y() < p2.y();
}



SUMOReal
PositionVector::isLeft(const Position& P0, const Position& P1,
                       const Position& P2) const {
    return (P1.x() - P0.x()) * (P2.y() - P0.y()) - (P2.x() - P0.x()) * (P1.y() - P0.y());
}


PositionVector
PositionVector::convexHull() const {
    PositionVector ret = *this;
    ret.sortAsPolyCWByAngle();
    return simpleHull_2D(ret);
}


void
PositionVector::set(size_t pos, const Position& p) {
    myCont[pos] = p;
}


PositionVector
PositionVector::intersectionPoints2D(const Line& line) const {
    PositionVector ret;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        if (GeomHelper::intersects(*i, *(i + 1), line.p1(), line.p2())) {
            ret.push_back_noDoublePos(GeomHelper::intersection_position2D(
                                          *i, *(i + 1), line.p1(), line.p2()));
        }
    }
    return ret;
}


int
PositionVector::appendWithCrossingPoint(const PositionVector& v) {
    if (myCont.back().distanceTo(v.myCont[0]) < 2) { // !!! heuristic
        copy(v.myCont.begin() + 1, v.myCont.end(), back_inserter(myCont));
        return 1;
    }
    //
    Line l1(myCont[myCont.size() - 2], myCont.back());
    l1.extrapolateBy(100);
    Line l2(v.myCont[0], v.myCont[1]);
    l2.extrapolateBy(100);
    if (l1.intersects(l2) && l1.intersectsAtLength2D(l2) < l1.length2D() - 100) { // !!! heuristic
        Position p = l1.intersectsAt(l2);
        myCont[myCont.size() - 1] = p;
        copy(v.myCont.begin() + 1, v.myCont.end(), back_inserter(myCont));
        return 2;
    } else {
        copy(v.myCont.begin(), v.myCont.end(), back_inserter(myCont));
        return 3;
    }
}


PositionVector
PositionVector::getSubpart(SUMOReal begin, SUMOReal end) const {
    PositionVector ret;
    Position begPos = myCont.front();
    if (begin > POSITION_EPS) {
        begPos = positionAtLengthPosition(begin);
    }
    Position endPos = myCont.back();
    if (end < length() - POSITION_EPS) {
        endPos = positionAtLengthPosition(end);
    }
    ret.push_back(begPos);

    SUMOReal seen = 0;
    ContType::const_iterator i = myCont.begin();
    // skip previous segments
    while ((i + 1) != myCont.end()
            &&
            seen + (*i).distanceTo(*(i + 1)) < begin) {
        seen += (*i).distanceTo(*(i + 1));
        i++;
    }
    // append segments in between
    while ((i + 1) != myCont.end()
            &&
            seen + (*i).distanceTo(*(i + 1)) < end) {

        ret.push_back_noDoublePos(*(i + 1));
        /*
        if(ret.at(-1)!=*(i+1)) {
            ret.push_back(*(i+1));
        }
        */
        seen += (*i).distanceTo(*(i + 1));
        i++;
    }
    // append end
    ret.push_back_noDoublePos(endPos);
    return ret;
}


PositionVector
PositionVector::getSubpart2D(SUMOReal begin, SUMOReal end) const {
    PositionVector ret;
    Position begPos = myCont.front();
    if (begin > POSITION_EPS) {
        begPos = positionAtLengthPosition2D(begin);
    }
    Position endPos = myCont.back();
    if (end < length() - POSITION_EPS) {
        endPos = positionAtLengthPosition2D(end);
    }
    ret.push_back(begPos);

    SUMOReal seen = 0;
    ContType::const_iterator i = myCont.begin();
    // skip previous segments
    while ((i + 1) != myCont.end()
            &&
            seen + (*i).distanceTo2D(*(i + 1)) < begin) {
        seen += (*i).distanceTo2D(*(i + 1));
        i++;
    }
    // append segments in between
    while ((i + 1) != myCont.end()
            &&
            seen + (*i).distanceTo2D(*(i + 1)) < end) {

        ret.push_back_noDoublePos(*(i + 1));
        /*
        if(ret.at(-1)!=*(i+1)) {
            ret.push_back(*(i+1));
        }
        */
        seen += (*i).distanceTo2D(*(i + 1));
        i++;
    }
    // append end
    ret.push_back_noDoublePos(endPos);
    return ret;
}


void
PositionVector::pruneFromBeginAt(const Position& p) {
    // find minimum distance (from the begin)
    size_t pos = 0;
    SUMOReal dist = 1000000;
    size_t currPos = 0;
    SUMOReal currDist = GeomHelper::distancePointLine(p,
                        GeomHelper::extrapolate_first(*(myCont.begin()), *(myCont.begin() + 1), 100),
                        *(myCont.begin() + 1));
//    assert(currDist>=0);
    if (currDist >= 0 && currDist < dist) {
        dist = currDist;
        pos = currPos;
    }

    for (ContType::iterator i = myCont.begin(); i != myCont.end() - 1; i++, currPos++) {
        SUMOReal currDist = GeomHelper::distancePointLine(p, *i, *(i + 1));
        if (currDist >= 0 && currDist < dist) {
            dist = currDist;
            pos = currPos;
        }
    }
    // remove leading items
    for (size_t j = 0; j < pos; j++) {
        myCont.erase(myCont.begin());
    }
    // replace first item by the new position
    SUMOReal lpos = GeomHelper::nearest_position_on_line_to_point2D(
                        myCont[0], myCont[1], p);
    if (lpos == -1) {
        return;
    }
    Position np = positionAtLengthPosition(lpos);
    if (np != *(myCont.begin())) {
        myCont.erase(myCont.begin());
        if (np != *(myCont.begin())) {
            myCont.insert(myCont.begin(), p);
            assert(myCont.size() > 1);
            assert(*(myCont.begin()) != *(myCont.end() - 1));
        }
    }
}


void
PositionVector::pruneFromEndAt(const Position& p) {
    // find minimum distance (from the end)
    size_t pos = 0;
    SUMOReal dist = 1000000;
    size_t currPos = 0;
    SUMOReal currDist = GeomHelper::distancePointLine(p,
                        *(myCont.end() - 1),
                        GeomHelper::extrapolate_second(*(myCont.end() - 2), *(myCont.end() - 1), 100));
//    assert(currDist>=0);
    if (currDist >= 0 && currDist < dist) {
        dist = currDist;
        pos = currPos;
    }

    for (ContType::reverse_iterator i = myCont.rbegin(); i != myCont.rend() - 1; i++, currPos++) {
        SUMOReal currDist = GeomHelper::distancePointLine(p, *(i), *(i + 1));
        if (currDist >= 0 && currDist < dist) {
            dist = currDist;
            pos = currPos;
        }
    }
    // remove trailing items
    for (size_t j = 0; j < pos; j++) {
        myCont.erase(myCont.end() - 1);
    }
    // replace last item by the new position
    SUMOReal lpos =
        GeomHelper::nearest_position_on_line_to_point2D(
            myCont[myCont.size() - 1], myCont[myCont.size() - 2], p);
    if (lpos == -1) {
        return;
    }
    Position np = positionAtLengthPosition(
                      length() - lpos);
    if (np != *(myCont.end() - 1)) {
        myCont.erase(myCont.end() - 1);
        if (np != *(myCont.end() - 1)) {
            myCont.push_back(np);
            assert(myCont.size() > 1);
            assert(*(myCont.begin()) != *(myCont.end() - 1));
        }
    }
}


SUMOReal
PositionVector::beginEndAngle() const {
    Line tmp(getBegin(), getEnd());
    return tmp.atan2Angle();
}


void
PositionVector::eraseAt(int i) {
    if (i >= 0) {
        myCont.erase(myCont.begin() + i);
    } else {
        myCont.erase(myCont.end() + i);
    }
}


SUMOReal
PositionVector::nearest_position_on_line_to_point2D(const Position& p, bool perpendicular) const {
    SUMOReal minDist = std::numeric_limits<SUMOReal>::max();
    SUMOReal nearestPos = -1;
    SUMOReal seen = 0;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        const SUMOReal pos =
            GeomHelper::nearest_position_on_line_to_point2D(*i, *(i + 1), p, perpendicular);
        const SUMOReal dist = pos < 0 ? minDist : p.distanceTo2D(Line(*i, *(i + 1)).getPositionAtDistance(pos));
        if (dist < minDist) {
            nearestPos = pos + seen;
            minDist = dist;
        }
        seen += (*i).distanceTo2D(*(i + 1));
    }
    return nearestPos;
}


int
PositionVector::indexOfClosest(const Position& p) const {
    assert(size() > 0);
    SUMOReal minDist = std::numeric_limits<SUMOReal>::max();
    SUMOReal dist;
    int closest = 0;
    for (int i = 0; i < (int)size(); i++) {
        dist = p.distanceTo(myCont[i]);
        if (dist < minDist) {
            closest = i;
            minDist = dist;
        }
    }
    return closest;
}


int
PositionVector::insertAtClosest(const Position& p) {
    Position outIntersection = Position();
    SUMOReal minDist = std::numeric_limits<SUMOReal>::max();
    SUMOReal dist;
    int insertionIndex = 1;
    for (int i = 0; i < (int)size() - 1; i++) {
        dist = GeomHelper::closestDistancePointLine(p, myCont[i], myCont[i + 1], outIntersection);
        if (dist < minDist) {
            insertionIndex = i + 1;
            minDist = dist;
        }
    }
    insertAt(insertionIndex, p);
    return insertionIndex;
}


SUMOReal
PositionVector::distance(const Position& p) const {
    if (size() == 1) {
        return myCont.front().distanceTo(p);
    }
    Position outIntersection;
    SUMOReal minDist = std::numeric_limits<double>::max();
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        minDist = MIN2(minDist, GeomHelper::closestDistancePointLine(
                           p, *i, *(i + 1), outIntersection));
    }
    return minDist;
}


std::vector<SUMOReal>
PositionVector::intersectsAtLengths2D(const PositionVector& other) const {
    std::vector<SUMOReal> ret;
    for (ContType::const_iterator i = other.myCont.begin(); i != other.myCont.end() - 1; i++) {
        std::vector<SUMOReal> atSegment = intersectsAtLengths2D(Line(*i, *(i + 1)));
        copy(atSegment.begin(), atSegment.end(), back_inserter(ret));
    }
    return ret;
}


std::vector<SUMOReal>
PositionVector::intersectsAtLengths2D(const Line& line) const {
    std::vector<SUMOReal> ret;
    SUMOReal pos = 0;
    for (ContType::const_iterator i = myCont.begin(); i != myCont.end() - 1; i++) {
        Line l((*i), *(i + 1));
        if (GeomHelper::intersects(l.p1(), l.p2(), line.p1(), line.p2())) {
            Position p =
                GeomHelper::intersection_position2D(l.p1(), l.p2(), line.p1(), line.p2());
            SUMOReal atLength = p.distanceTo2D(l.p1());
            ret.push_back(atLength + pos);
        }
        pos += l.length2D();
    }
    return ret;
}


void
PositionVector::extrapolate(SUMOReal val) {
    assert(myCont.size() > 1);
    Position nb =
        GeomHelper::extrapolate_first(myCont[0], myCont[1], val);
    Position ne =
        GeomHelper::extrapolate_second(
            myCont[myCont.size() - 2], myCont[myCont.size() - 1], val);
    myCont.erase(myCont.begin());
    push_front(nb);
    myCont.erase(myCont.end() - 1);
    push_back(ne);
}


PositionVector
PositionVector::reverse() const {
    PositionVector ret;
    for (ContType::const_reverse_iterator i = myCont.rbegin(); i != myCont.rend(); i++) {
        ret.push_back(*i);
    }
    return ret;
}


void
PositionVector::move2side(SUMOReal amount) {
    if (myCont.size() < 2) {
        return;
    }
    PositionVector shape;
    for (size_t i = 0; i < myCont.size(); i++) {
        if (i == 0) {
            Position from = myCont[i];
            Position to = myCont[i + 1];
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(Position(from.x() - offsets.first,
                                     from.y() - offsets.second, from.z()));
        } else if (i == myCont.size() - 1) {
            Position from = myCont[i - 1];
            Position to = myCont[i];
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(Position(to.x() - offsets.first,
                                     to.y() - offsets.second, to.z()));
        } else {
            Position from = myCont[i - 1];
            Position me = myCont[i];
            Position to = myCont[i + 1];
            const double sinAngle = sin(GeomHelper::Angle2D(from.x() - me.x(), from.y() - me.y(),
                                        me.x() - to.x(), me.y() - to.y()) / 2);
            const double maxDev = 2 * (from.distanceTo2D(me) + me.distanceTo2D(to)) * sinAngle;
            if (fabs(maxDev) < POSITION_EPS) {
                // parallel case, just shift the middle point
                std::pair<SUMOReal, SUMOReal> off =
                    GeomHelper::getNormal90D_CW(from, to, amount);
                shape.push_back(Position(me.x() - off.first, me.y() - off.second, me.z()));
                continue;
            }
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, me, amount);
            std::pair<SUMOReal, SUMOReal> offsets2 =
                GeomHelper::getNormal90D_CW(me, to, amount);
            Line l1(
                Position(from.x() - offsets.first, from.y() - offsets.second),
                Position(me.x() - offsets.first, me.y() - offsets.second));
            l1.extrapolateBy(100);
            Line l2(
                Position(me.x() - offsets2.first, me.y() - offsets2.second),
                Position(to.x() - offsets2.first, to.y() - offsets2.second));
            l2.extrapolateBy(100);
            if (l1.intersects(l2)) {
                shape.push_back(l1.intersectsAt(l2));
            } else {
                throw InvalidArgument("no line intersection");
            }
        }
    }

    /*
    ContType newCont;
    std::pair<SUMOReal, SUMOReal> p;
    Position newPos;
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
    //        Position newPos(*i);
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


Line
PositionVector::lineAt(size_t pos) const {
    assert(myCont.size() > pos + 1);
    return Line(myCont[pos], myCont[pos + 1]);
}


Line
PositionVector::getBegLine() const {
    return lineAt(0);
}


Line
PositionVector::getEndLine() const {
    return lineAt(myCont.size() - 2);
}


void
PositionVector::closePolygon() {
    if (myCont[0] == myCont.back()) {
        return;
    }
    push_back(myCont[0]);
}


std::vector<SUMOReal>
PositionVector::distances(const PositionVector& s) const {
    std::vector<SUMOReal> ret;
    ContType::const_iterator i;
    for (i = myCont.begin(); i != myCont.end(); i++) {
        ret.push_back(s.distance(*i));
    }
    for (i = s.myCont.begin(); i != s.myCont.end(); i++) {
        ret.push_back(distance(*i));
    }
    return ret;
}


Position
PositionVector::pop_back() {
    Position last = myCont.back();
    myCont.erase(myCont.end() - 1);
    return last;
}


Position
PositionVector::pop_front() {
    Position first = myCont.front();
    myCont.erase(myCont.begin());
    return first;
}


void
PositionVector::insertAt(int index, const Position& p) {
    if (index >= 0) {
        myCont.insert(myCont.begin() + index, p);
    } else {
        myCont.insert(myCont.end() + index, p);
    }
}


void
PositionVector::push_back_noDoublePos(const Position& p) {
    if (size() == 0 || !p.almostSame(myCont.back())) {
        myCont.push_back(p);
    }
}


void
PositionVector::push_front_noDoublePos(const Position& p) {
    if (size() == 0 || !p.almostSame(myCont.front())) {
        myCont.insert(myCont.begin(), p);
    }
}


void
PositionVector::replaceAt(size_t index, const Position& by) {
    assert(size() > index);
    myCont[index] = by;
}


bool
PositionVector::isClosed() const {
    return myCont.size() >= 2 && myCont[0] == myCont.back();
}


void
PositionVector::removeDoublePoints() {
    if (myCont.size() > 1) {
        ContType::iterator last = myCont.begin();
        for (ContType::iterator i = myCont.begin() + 1; i != myCont.end();) {
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
PositionVector::removeColinearPoints() {
    if (myCont.size() > 2) {
        Position& last = myCont.front();
        for (ContType::iterator i = myCont.begin() + 1; i != myCont.end() - 1;) {
            if (GeomHelper::distancePointLine(*i, last, *(i + 1)) < 0.001) {
                i = myCont.erase(i);
            } else {
                last = *i;
                ++i;
            }
        }
    }
}


bool
PositionVector::operator==(const PositionVector& v2) const {
    if (size() == v2.size()) {
        for (int i = 0; i < (int)size(); i++) {
            if ((*this)[i] != v2[i]) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}



/****************************************************************************/

