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
    std::copy(v.begin(), v.end(), std::back_inserter(*this));
}


PositionVector::~PositionVector() {}


// ------------ Adding items to the container
void
PositionVector::push_back(const PositionVector& p) {
    copy(p.begin(), p.end(), back_inserter(*this));
}


void
PositionVector::push_front(const Position& p) {
    insert(begin(), p);
}


Position
PositionVector::pop_front() {
    Position first = front();
    erase(begin());
    return first;
}


bool
PositionVector::around(const Position& p, SUMOReal offset) const {
    if (offset != 0) {
        PositionVector tmp(*this);
        tmp.scaleAbsolute(offset);
        return tmp.around(p);
    }
    SUMOReal angle = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        Position p1(
            (*i).x() - p.x(),
            (*i).y() - p.y());
        Position p2(
            (*(i + 1)).x() - p.x(),
            (*(i + 1)).y() - p.y());
        angle += GeomHelper::Angle2D(p1.x(), p1.y(), p2.x(), p2.y());
    }
    Position p1(
        (*(end() - 1)).x() - p.x(),
        (*(end() - 1)).y() - p.y());
    Position p2(
        (*(begin())).x() - p.x(),
        (*(begin())).y() - p.y());
    angle += GeomHelper::Angle2D(p1.x(), p1.y(), p2.x(), p2.y());
    return (!(fabs(angle) < M_PI));
}


bool
PositionVector::overlapsWith(const AbstractPoly& poly, SUMOReal offset) const {
    for (const_iterator i = begin(); i != end() - 1; i++) {
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
    for (const_iterator i = begin(); i != end() - 1; i++) {
        if (GeomHelper::intersects(*i, *(i + 1), p1, p2)) {
            return true;
        }
    }
    //return GeomHelper::intersects(*(end()-1), *(begin()), p1, p2);
    return false;
}


bool
PositionVector::intersects(const PositionVector& v1) const {
    if (size() < 2) {
        return false;
    }
    for (const_iterator i = begin(); i != end() - 1; i++) {
        if (v1.intersects(*i, *(i + 1))) {
            return true;
        }
    }
    //return v1.intersects(*(end()-1), *(begin()));
    return false;
}


Position
PositionVector::intersectsAtPoint(const Position& p1,
                                  const Position& p2) const {
    for (const_iterator i = begin(); i != end() - 1; i++) {
        if (GeomHelper::intersects(*i, *(i + 1), p1, p2)) {
            return GeomHelper::intersection_position2D(*i, *(i + 1), p1, p2);
        }
    }
    return Position(-1, -1);
}


Position
PositionVector::intersectsAtPoint(const PositionVector& v1) const {
    for (const_iterator i = begin(); i != end() - 1; i++) {
        if (v1.intersects(*i, *(i + 1))) {
            return v1.intersectsAtPoint(*i, *(i + 1));
        }
    }
    /*
    if(v1.intersects(*(end()-1), *(begin()))) {
        return v1.intersectsAtPoint(*(end()-1), *(begin()));
    }
    */
    return Position(-1, -1);
}


const Position&
PositionVector::operator[](int index) const {
    if (index >= 0) {
        return at(index);
    } else {
        return at((int)size() + index);
    }
}


Position&
PositionVector::operator[](int index) {
    if (index >= 0) {
        return at(index);
    } else {
        return at((int)size() + index);
    }
}


Position
PositionVector::positionAtOffset(SUMOReal pos, SUMOReal lateralOffset) const {
    const_iterator i = begin();
    SUMOReal seenLength = 0;
    do {
        const SUMOReal nextLength = (*i).distanceTo(*(i + 1));
        if (seenLength + nextLength > pos) {
            return positionAtOffset(*i, *(i + 1), pos - seenLength, lateralOffset);
        }
        seenLength += nextLength;
    } while (++i != end() - 1);
    return back();
}


Position
PositionVector::positionAtOffset2D(SUMOReal pos, SUMOReal lateralOffset) const {
    const_iterator i = begin();
    SUMOReal seenLength = 0;
    do {
        const SUMOReal nextLength = (*i).distanceTo2D(*(i + 1));
        if (seenLength + nextLength > pos) {
            return positionAtOffset2D(*i, *(i + 1), pos - seenLength, lateralOffset);
        }
        seenLength += nextLength;
    } while (++i != end() - 1);
    return back();
}


SUMOReal
PositionVector::rotationDegreeAtOffset(SUMOReal pos) const {
    if (pos < 0) {
        pos += length();
    }
    const_iterator i = begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = (*i).distanceTo(*(i + 1));
        if (seenLength + nextLength > pos) {
            Line l(*i, *(i + 1));
            return l.atan2DegreeAngle();
        }
        seenLength += nextLength;
    } while (++i != end() - 1);
    Line l(*(end() - 2), *(end() - 1));
    return l.atan2DegreeAngle();
}

SUMOReal
PositionVector::slopeDegreeAtOffset(SUMOReal pos) const {
    const_iterator i = begin();
    SUMOReal seenLength = 0;
    do {
        SUMOReal nextLength = (*i).distanceTo(*(i + 1));
        if (seenLength + nextLength > pos) {
            Line l(*i, *(i + 1));
            return l.atan2DegreeSlope();
        }
        seenLength += nextLength;
    } while (++i != end() - 1);
    Line l(*(end() - 2), *(end() - 1));
    return l.atan2DegreeSlope();
}

Position
PositionVector::positionAtOffset(const Position& p1,
                                 const Position& p2,
                                 SUMOReal pos, SUMOReal lateralOffset) {
    const SUMOReal dist = p1.distanceTo(p2);
    if (dist < pos) {
        return Position(-1, -1);
    }
    if (lateralOffset != 0) {
        Line l(p1, p2);
        l.move2side(-lateralOffset); // move in the same direction as Position::move2side
        return l.getPositionAtDistance(pos);
    }
    return p1 + (p2 - p1) * (pos / dist);
}


Position
PositionVector::positionAtOffset2D(const Position& p1,
                                   const Position& p2,
                                   SUMOReal pos, SUMOReal lateralOffset) {
    const SUMOReal dist = p1.distanceTo2D(p2);
    if (dist < pos) {
        return Position(-1, -1);
    }
    if (lateralOffset != 0) {
        Line l(p1, p2);
        l.move2side(-lateralOffset); // move in the same direction as Position::move2side
        return l.getPositionAtDistance2D(pos);
    }
    return p1 + (p2 - p1) * (pos / dist);
}


Boundary
PositionVector::getBoxBoundary() const {
    Boundary ret;
    for (const_iterator i = begin(); i != end(); i++) {
        ret.add(*i);
    }
    return ret;
}


Position
PositionVector::getPolygonCenter() const {
    SUMOReal x = 0;
    SUMOReal y = 0;
    for (const_iterator i = begin(); i != end(); i++) {
        x += (*i).x();
        y += (*i).y();
    }
    return Position(x / (SUMOReal) size(), y / (SUMOReal) size());
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
        if (lengthSum == 0) {
            // it is probably only one point
            return tmp[0];
        }
        return Position(x / lengthSum, y / lengthSum);
    }
}


void
PositionVector::scaleRelative(SUMOReal factor) {
    Position centroid = getCentroid();
    for (int i = 0; i < static_cast<int>(size()); i++) {
        (*this)[i] = centroid + (((*this)[i] - centroid) * factor);
    }
}


void
PositionVector::scaleAbsolute(SUMOReal offset) {
    Position centroid = getCentroid();
    for (int i = 0; i < static_cast<int>(size()); i++) {
        (*this)[i] = centroid + (((*this)[i] - centroid) + offset);
    }
}


Position
PositionVector::getLineCenter() const {
    if (size() == 1) {
        return (*this)[0];
    }
    return positionAtOffset(SUMOReal((length() / 2.)));
}


SUMOReal
PositionVector::length() const {
    SUMOReal len = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        len += (*i).distanceTo(*(i + 1));
    }
    return len;
}

SUMOReal
PositionVector::length2D() const {
    SUMOReal len = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        len += (*i).distanceTo2D(*(i + 1));
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
    for (const_iterator i = begin(); i != end() - 1; i++) {
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


std::pair<PositionVector, PositionVector>
PositionVector::splitAt(SUMOReal where) const {
    if (size() < 2) {
        throw InvalidArgument("Vector to short for splitting");
    }
    if (where <= POSITION_EPS || where >= length() - POSITION_EPS) {
        WRITE_WARNING("Splitting vector close to end (pos: " + toString(where) + ", length: " + toString(length()) + ")");
    }
    PositionVector first, second;
    first.push_back((*this)[0]);
    SUMOReal seen = 0;
    const_iterator it = begin() + 1;
    SUMOReal next = first.back().distanceTo(*it);
    // see how many points we can add to first
    while (where >= seen + next + POSITION_EPS) {
        seen += next;
        first.push_back(*it);
        it++;
        next = first.back().distanceTo(*it);
    }
    if (fabs(where - (seen + next)) > POSITION_EPS || it == end() - 1) {
        // we need to insert a new point because 'where' is not close to an
        // existing point or it is to close to the endpoint
        Line tmpL(first.back(), *it);
        Position p = tmpL.getPositionAtDistance(where - seen);
        first.push_back(p);
        second.push_back(p);
    } else {
        first.push_back(*it);
    }
    // add the remaining points to second
    for (; it != end(); it++) {
        second.push_back(*it);
    }
    assert(first.size() >= 2);
    assert(second.size() >= 2);
    assert(first.back() == second.front());
    assert(fabs(first.length() + second.length() - length()) < 2 * POSITION_EPS);
    return std::pair<PositionVector, PositionVector>(first, second);
}


std::ostream&
operator<<(std::ostream& os, const PositionVector& geom) {
    for (PositionVector::const_iterator i = geom.begin(); i != geom.end(); i++) {
        if (i != geom.begin()) {
            os << " ";
        }
        os << (*i);
    }
    return os;
}


void
PositionVector::sortAsPolyCWByAngle() {
    std::sort(begin(), end(), as_poly_cw_sorter());
}


void
PositionVector::add(SUMOReal xoff, SUMOReal yoff, SUMOReal zoff) {
    for (int i = 0; i < static_cast<int>(size()); i++) {
        (*this)[i].add(xoff, yoff, zoff);
    }
}


void
PositionVector::reshiftRotate(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) {
    for (int i = 0; i < static_cast<int>(size()); i++) {
        (*this)[i].reshiftRotate(xoff, yoff, rot);
    }
}


int
PositionVector::as_poly_cw_sorter::operator()(const Position& p1,
        const Position& p2) const {
    return atan2(p1.x(), p1.y()) < atan2(p2.x(), p2.y());
}



void
PositionVector::sortByIncreasingXY() {
    std::sort(begin(), end(), increasing_x_y_sorter());
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


PositionVector
PositionVector::intersectionPoints2D(const Line& line) const {
    PositionVector ret;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        if (GeomHelper::intersects(*i, *(i + 1), line.p1(), line.p2())) {
            ret.push_back_noDoublePos(GeomHelper::intersection_position2D(
                                          *i, *(i + 1), line.p1(), line.p2()));
        }
    }
    return ret;
}


int
PositionVector::appendWithCrossingPoint(const PositionVector& v) {
    if (back().distanceTo(v[0]) < 2) { // !!! heuristic
        copy(v.begin() + 1, v.end(), back_inserter(*this));
        return 1;
    }
    //
    Line l1((*this)[static_cast<int>(size()) - 2], back());
    l1.extrapolateBy(100);
    Line l2(v[0], v[1]);
    l2.extrapolateBy(100);
    if (l1.intersects(l2) && l1.intersectsAtLength2D(l2) < l1.length2D() - 100) { // !!! heuristic
        Position p = l1.intersectsAt(l2);
        (*this)[static_cast<int>(size()) - 1] = p;
        copy(v.begin() + 1, v.end(), back_inserter(*this));
        return 2;
    } else {
        copy(v.begin(), v.end(), back_inserter(*this));
        return 3;
    }
}


void
PositionVector::append(const PositionVector& v) {
    if (back().distanceTo(v[0]) < 2) {
        copy(v.begin() + 1, v.end(), back_inserter(*this));
    } else {
        copy(v.begin(), v.end(), back_inserter(*this));
    }
}


PositionVector
PositionVector::getSubpart(SUMOReal beginOffset, SUMOReal endOffset) const {
    PositionVector ret;
    Position begPos = front();
    if (beginOffset > POSITION_EPS) {
        begPos = positionAtOffset(beginOffset);
    }
    Position endPos = back();
    if (endOffset < length() - POSITION_EPS) {
        endPos = positionAtOffset(endOffset);
    }
    ret.push_back(begPos);

    SUMOReal seen = 0;
    const_iterator i = begin();
    // skip previous segments
    while ((i + 1) != end()
            &&
            seen + (*i).distanceTo(*(i + 1)) < beginOffset) {
        seen += (*i).distanceTo(*(i + 1));
        i++;
    }
    // append segments in between
    while ((i + 1) != end()
            &&
            seen + (*i).distanceTo(*(i + 1)) < endOffset) {

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
PositionVector::getSubpart2D(SUMOReal beginOffset, SUMOReal endOffset) const {
    PositionVector ret;
    Position begPos = front();
    if (beginOffset > POSITION_EPS) {
        begPos = positionAtOffset2D(beginOffset);
    }
    Position endPos = back();
    if (endOffset < length() - POSITION_EPS) {
        endPos = positionAtOffset2D(endOffset);
    }
    ret.push_back(begPos);

    SUMOReal seen = 0;
    const_iterator i = begin();
    // skip previous segments
    while ((i + 1) != end()
            &&
            seen + (*i).distanceTo2D(*(i + 1)) < beginOffset) {
        seen += (*i).distanceTo2D(*(i + 1));
        i++;
    }
    // append segments in between
    while ((i + 1) != end()
            &&
            seen + (*i).distanceTo2D(*(i + 1)) < endOffset) {

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
                        GeomHelper::extrapolate_first(*(begin()), *(begin() + 1), 100),
                        *(begin() + 1));
//    assert(currDist>=0);
    if (currDist >= 0 && currDist < dist) {
        dist = currDist;
        pos = currPos;
    }

    for (iterator i = begin(); i != end() - 1; i++, currPos++) {
        SUMOReal currDist = GeomHelper::distancePointLine(p, *i, *(i + 1));
        if (currDist >= 0 && currDist < dist) {
            dist = currDist;
            pos = currPos;
        }
    }
    // remove leading items
    for (size_t j = 0; j < pos; j++) {
        erase(begin());
    }
    // replace first item by the new position
    SUMOReal lpos = GeomHelper::nearest_offset_on_line_to_point2D(
                        (*this)[0], (*this)[1], p);
    if (lpos == -1) {
        return;
    }
    Position np = positionAtOffset(lpos);
    if (np != *(begin())) {
        erase(begin());
        if (np != *(begin())) {
            insert(begin(), p);
            assert(size() > 1);
            assert(*(begin()) != *(end() - 1));
        }
    }
}


PositionVector
PositionVector::getSubpartByIndex(int beginIndex, int count) const {
    if (beginIndex < 0) {
        beginIndex += (int)size();
    }
    assert(count > 0);
    assert(beginIndex < (int)size());
    assert(beginIndex + count <= (int)size());
    PositionVector result;
    for (int i = beginIndex; i < beginIndex + count; ++i) {
        result.push_back((*this)[i]);
    }
    return result;
}


void
PositionVector::pruneFromEndAt(const Position& p) {
    // find minimum distance (from the end)
    size_t pos = 0;
    SUMOReal dist = 1000000;
    size_t currPos = 0;
    SUMOReal currDist = GeomHelper::distancePointLine(p,
                        *(end() - 1),
                        GeomHelper::extrapolate_second(*(end() - 2), *(end() - 1), 100));
//    assert(currDist>=0);
    if (currDist >= 0 && currDist < dist) {
        dist = currDist;
        pos = currPos;
    }

    for (reverse_iterator i = rbegin(); i != rend() - 1; i++, currPos++) {
        SUMOReal currDist = GeomHelper::distancePointLine(p, *(i), *(i + 1));
        if (currDist >= 0 && currDist < dist) {
            dist = currDist;
            pos = currPos;
        }
    }
    // remove trailing items
    for (size_t j = 0; j < pos; j++) {
        erase(end() - 1);
    }
    // replace last item by the new position
    SUMOReal lpos =
        GeomHelper::nearest_offset_on_line_to_point2D(
            (*this)[static_cast<int>(size()) - 1], (*this)[static_cast<int>(size()) - 2], p);
    if (lpos == -1) {
        return;
    }
    Position np = positionAtOffset(
                      length() - lpos);
    if (np != *(end() - 1)) {
        erase(end() - 1);
        if (np != *(end() - 1)) {
            push_back(np);
            assert(size() > 1);
            assert(*(begin()) != *(end() - 1));
        }
    }
}


SUMOReal
PositionVector::beginEndAngle() const {
    Line tmp(front(), back());
    return tmp.atan2Angle();
}


void
PositionVector::eraseAt(int i) {
    if (i >= 0) {
        erase(begin() + i);
    } else {
        erase(end() + i);
    }
}


SUMOReal
PositionVector::nearest_offset_to_point2D(const Position& p, bool perpendicular) const {
    SUMOReal minDist = std::numeric_limits<SUMOReal>::max();
    SUMOReal nearestPos = -1;
    SUMOReal seen = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        const SUMOReal pos =
            GeomHelper::nearest_offset_on_line_to_point2D(*i, *(i + 1), p, perpendicular);
        const SUMOReal dist = pos < 0 ? minDist : p.distanceTo2D(Line(*i, *(i + 1)).getPositionAtDistance(pos));
        if (dist < minDist) {
            nearestPos = pos + seen;
            minDist = dist;
        }
        if (perpendicular && i != begin()) {
            // even if perpendicular is set we still need to check the distance to the inner points
            const SUMOReal cornerDist = p.distanceTo2D(*i);
            if (cornerDist < minDist) {
                nearestPos = seen;
                minDist = cornerDist;
            }
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
        dist = p.distanceTo((*this)[i]);
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
        dist = GeomHelper::closestDistancePointLine(p, (*this)[i], (*this)[i + 1], outIntersection);
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
        return front().distanceTo(p);
    }
    Position outIntersection;
    SUMOReal minDist = std::numeric_limits<double>::max();
    for (const_iterator i = begin(); i != end() - 1; i++) {
        minDist = MIN2(minDist, GeomHelper::closestDistancePointLine(
                           p, *i, *(i + 1), outIntersection));
    }
    return minDist;
}


std::vector<SUMOReal>
PositionVector::intersectsAtLengths2D(const PositionVector& other) const {
    std::vector<SUMOReal> ret;
    for (const_iterator i = other.begin(); i != other.end() - 1; i++) {
        std::vector<SUMOReal> atSegment = intersectsAtLengths2D(Line(*i, *(i + 1)));
        copy(atSegment.begin(), atSegment.end(), back_inserter(ret));
    }
    return ret;
}


std::vector<SUMOReal>
PositionVector::intersectsAtLengths2D(const Line& line) const {
    std::vector<SUMOReal> ret;
    SUMOReal pos = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
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
    assert(size() > 1);
    Position nb =
        GeomHelper::extrapolate_first((*this)[0], (*this)[1], val);
    Position ne =
        GeomHelper::extrapolate_second(
            (*this)[static_cast<int>(size()) - 2], (*this)[static_cast<int>(size()) - 1], val);
    erase(begin());
    push_front(nb);
    erase(end() - 1);
    push_back(ne);
}


PositionVector
PositionVector::reverse() const {
    PositionVector ret;
    for (const_reverse_iterator i = rbegin(); i != rend(); i++) {
        ret.push_back(*i);
    }
    return ret;
}


void
PositionVector::move2side(SUMOReal amount) {
    if (size() < 2) {
        return;
    }
    PositionVector shape;
    for (int i = 0; i < static_cast<int>(size()); i++) {
        if (i == 0) {
            Position from = (*this)[i];
            Position to = (*this)[i + 1];
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(Position(from.x() - offsets.first,
                                     from.y() - offsets.second, from.z()));
        } else if (i == static_cast<int>(size()) - 1) {
            Position from = (*this)[i - 1];
            Position to = (*this)[i];
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(Position(to.x() - offsets.first,
                                     to.y() - offsets.second, to.z()));
        } else {
            Position from = (*this)[i - 1];
            Position me = (*this)[i];
            Position to = (*this)[i + 1];
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
    newPos = (*(begin()));
    p = GeomHelper::getNormal90D_CW(*(begin()), *(begin()+1), amount);
    newPos.add(p.first, p.second);
    newCont.push_back(newPos);
    // middle points
    for(const_iterator i=begin()+1; i!=end()-1; i++) {
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
    newPos = (*(end()-1));
    newPos.add(p.first, p.second);
    newCont.push_back(newPos);
    myCont = newCont;
    */
    *this = shape;
}


Line
PositionVector::lineAt(int pos) const {
    assert((int)size() > pos + 1);
    return Line((*this)[pos], (*this)[pos + 1]);
}


Line
PositionVector::getBegLine() const {
    return lineAt(0);
}


Line
PositionVector::getEndLine() const {
    return lineAt((int)size() - 2);
}


void
PositionVector::closePolygon() {
    if ((*this)[0] == back()) {
        return;
    }
    push_back((*this)[0]);
}


std::vector<SUMOReal>
PositionVector::distances(const PositionVector& s) const {
    std::vector<SUMOReal> ret;
    const_iterator i;
    for (i = begin(); i != end(); i++) {
        ret.push_back(s.distance(*i));
    }
    for (i = s.begin(); i != s.end(); i++) {
        ret.push_back(distance(*i));
    }
    return ret;
}


void
PositionVector::insertAt(int index, const Position& p) {
    if (index >= 0) {
        insert(begin() + index, p);
    } else {
        insert(end() + index, p);
    }
}


void
PositionVector::replaceAt(int index, const Position& p) {
    assert(index < static_cast<int>(size()));
    assert(index + static_cast<int>(size()) >= 0);
    if (index >= 0) {
        (*this)[index] = p;
    } else {
        (*this)[index + static_cast<int>(size())] = p;
    }
}


void
PositionVector::push_back_noDoublePos(const Position& p) {
    if (size() == 0 || !p.almostSame(back())) {
        push_back(p);
    }
}


void
PositionVector::push_front_noDoublePos(const Position& p) {
    if (size() == 0 || !p.almostSame(front())) {
        insert(begin(), p);
    }
}


bool
PositionVector::isClosed() const {
    return size() >= 2 && (*this)[0] == back();
}


void
PositionVector::removeDoublePoints(SUMOReal minDist, bool assertLength) {
    if (size() > 1) {
        iterator last = begin();
        for (iterator i = begin() + 1; i != end() && (!assertLength || size() > 2);) {
            if (last->almostSame(*i, minDist)) {
                i = erase(i);
            } else {
                last = i;
                ++i;
            }
        }
    }
}


void
PositionVector::removeColinearPoints() {
    if (size() > 2) {
        Position& last = front();
        for (iterator i = begin() + 1; i != end() - 1;) {
            if (GeomHelper::distancePointLine(*i, last, *(i + 1)) < 0.001) {
                i = erase(i);
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

