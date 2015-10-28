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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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


PositionVector::PositionVector(const std::vector<Position>::const_iterator beg, const std::vector<Position>::const_iterator end) {
    std::copy(beg, end, std::back_inserter(*this));
}


PositionVector::PositionVector(const Position& p1, const Position& p2) {
    push_back(p1);
    push_back(p2);
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
        angle += GeomHelper::angle2D(p1, p2);
    }
    Position p1(
        (*(end() - 1)).x() - p.x(),
        (*(end() - 1)).y() - p.y());
    Position p2(
        (*(begin())).x() - p.x(),
        (*(begin())).y() - p.y());
    angle += GeomHelper::angle2D(p1, p2);
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
    return Position::INVALID;
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
    return Position::INVALID;
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
        const Position& p1 = *i;
        const Position& p2 = *(i + 1);
        const SUMOReal nextLength = p1.distanceTo(p2);
        if (seenLength + nextLength > pos) {
            return GeomHelper::legacyDegree(p1.angleTo2D(p2));
        }
        seenLength += nextLength;
    } while (++i != end() - 1);
    const Position& p1 = (*this)[-2];
    const Position& p2 = back();
    return GeomHelper::legacyDegree(p1.angleTo2D(p2));
}

SUMOReal
PositionVector::slopeDegreeAtOffset(SUMOReal pos) const {
    const_iterator i = begin();
    SUMOReal seenLength = 0;
    do {
        const Position& p1 = *i;
        const Position& p2 = *(i + 1);
        const SUMOReal nextLength = p1.distanceTo(p2);
        if (seenLength + nextLength > pos) {
            return RAD2DEG(atan2(p2.z() - p1.z(), p1.distanceTo2D(p2)));
        }
        seenLength += nextLength;
    } while (++i != end() - 1);
    const Position& p1 = (*this)[-2];
    const Position& p2 = back();
    return RAD2DEG(atan2(p2.z() - p1.z(), p1.distanceTo2D(p2)));
}

Position
PositionVector::positionAtOffset(const Position& p1,
                                 const Position& p2,
                                 SUMOReal pos, SUMOReal lateralOffset) {
    const SUMOReal dist = p1.distanceTo(p2);
    if (pos < 0 || dist < pos) {
        return Position::INVALID;
    }
    Position offset;
    if (lateralOffset != 0) {
        std::pair<SUMOReal, SUMOReal> p = GeomHelper::getNormal90D_CW(p1, p2, -lateralOffset); // move in the same direction as Position::move2side
        offset.set(p.first, p.second);
    }
    if (pos == 0.) {
        return p1 + offset;
    }
    return p1 + (p2 - p1) * (pos / dist) + offset;
}


Position
PositionVector::positionAtOffset2D(const Position& p1,
                                   const Position& p2,
                                   SUMOReal pos, SUMOReal lateralOffset) {
    const SUMOReal dist = p1.distanceTo2D(p2);
    if (pos < 0 || dist < pos) {
        return Position::INVALID;
    }
    Position offset;
    if (lateralOffset != 0) {
        std::pair<SUMOReal, SUMOReal> p = GeomHelper::getNormal90D_CW(p1, p2, -lateralOffset); // move in the same direction as Position::move2side
        offset.set(p.first, p.second);
    }
    if (pos == 0.) {
        return p1 + offset;
    }
    return p1 + (p2 - p1) * (pos / dist) + offset;
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
    if (size() < 3) {
        return 0;
    }
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
        const Position p = positionAtOffset(first.back(), *it, where - seen);
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
PositionVector::mirrorX() {
    for (int i = 0; i < static_cast<int>(size()); i++) {
        (*this)[i].mul(1, -1);
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


void
PositionVector::append(const PositionVector& v, SUMOReal sameThreshold) {
    if (size() > 0 && v.size() > 0 && back().distanceTo(v[0]) < sameThreshold) {
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
    if (endOffset < length2D() - POSITION_EPS) {
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


SUMOReal
PositionVector::beginEndAngle() const {
    return front().angleTo2D(back());
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
    SUMOReal nearestPos = GeomHelper::INVALID_OFFSET;
    SUMOReal seen = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        const SUMOReal pos =
            GeomHelper::nearest_offset_on_line_to_point2D(*i, *(i + 1), p, perpendicular);
        const SUMOReal dist = pos == GeomHelper::INVALID_OFFSET ? minDist : p.distanceTo2D(positionAtOffset2D(*i, *(i + 1), pos));
        if (dist < minDist) {
            nearestPos = pos + seen;
            minDist = dist;
        }
        if (perpendicular && i != begin() && pos == GeomHelper::INVALID_OFFSET) {
            // even if perpendicular is set we still need to check the distance to the inner points
            const SUMOReal cornerDist = p.distanceTo2D(*i);
            if (cornerDist < minDist) {
                const SUMOReal pos1 =
                    GeomHelper::nearest_offset_on_line_to_point2D(*(i - 1), *i, p, false);
                const SUMOReal pos2 =
                    GeomHelper::nearest_offset_on_line_to_point2D(*i, *(i + 1), p, false);
                if (pos1 == (*(i - 1)).distanceTo2D(*i) && pos2 == 0.) {
                    nearestPos = seen;
                    minDist = cornerDist;
                }
            }
        }
        seen += (*i).distanceTo2D(*(i + 1));
    }
    return nearestPos;
}


Position
PositionVector::transformToVectorCoordinates(const Position& p, bool extend) const {
    // XXX this duplicates most of the code in nearest_offset_to_point2D. It should be refactored
    if (extend) {
        PositionVector extended = *this;
        const SUMOReal dist = 2 * distance(p);
        extended.extrapolate(dist);
        return extended.transformToVectorCoordinates(p) - Position(dist, 0);
    }
    SUMOReal minDist = std::numeric_limits<SUMOReal>::max();
    SUMOReal nearestPos = -1;
    SUMOReal seen = 0;
    int sign = 1;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        const SUMOReal pos =
            GeomHelper::nearest_offset_on_line_to_point2D(*i, *(i + 1), p, true);
        const SUMOReal dist = pos < 0 ? minDist : p.distanceTo2D(positionAtOffset(*i, *(i + 1), pos));
        if (dist < minDist) {
            nearestPos = pos + seen;
            minDist = dist;
            sign = isLeft(*i, *(i + 1), p) >= 0 ? -1 : 1;
        }
        if (i != begin() && pos == GeomHelper::INVALID_OFFSET) {
            // even if perpendicular is set we still need to check the distance to the inner points
            const SUMOReal cornerDist = p.distanceTo2D(*i);
            if (cornerDist < minDist) {
                const SUMOReal pos1 =
                    GeomHelper::nearest_offset_on_line_to_point2D(*(i - 1), *i, p, false);
                const SUMOReal pos2 =
                    GeomHelper::nearest_offset_on_line_to_point2D(*i, *(i + 1), p, false);
                if (pos1 == (*(i - 1)).distanceTo2D(*i) && pos2 == 0.) {
                    nearestPos = seen;
                    minDist = cornerDist;
                    sign = isLeft(*(i - 1), *i, p) >= 0 ? -1 : 1;
                }
            }
        }
        seen += (*i).distanceTo2D(*(i + 1));
    }
    if (nearestPos != -1) {
        return Position(nearestPos, sign * minDist);
    } else {
        return Position::INVALID;
    }
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
        dist = GeomHelper::closestDistancePointLine2D(p, (*this)[i], (*this)[i + 1], outIntersection);
        if (dist < minDist) {
            insertionIndex = i + 1;
            minDist = dist;
        }
    }
    insertAt(insertionIndex, p);
    return insertionIndex;
}


std::vector<SUMOReal>
PositionVector::intersectsAtLengths2D(const PositionVector& other) const {
    std::vector<SUMOReal> ret;
    for (const_iterator i = other.begin(); i != other.end() - 1; i++) {
        std::vector<SUMOReal> atSegment = intersectsAtLengths2D(*i, *(i + 1));
        copy(atSegment.begin(), atSegment.end(), back_inserter(ret));
    }
    return ret;
}


std::vector<SUMOReal>
PositionVector::intersectsAtLengths2D(const Position& lp1, const Position& lp2) const {
    std::vector<SUMOReal> ret;
    SUMOReal pos = 0;
    for (const_iterator i = begin(); i != end() - 1; i++) {
        const Position& p1 = *i;
        const Position& p2 = *(i + 1);
        if (GeomHelper::intersects(p1, p2, lp1, lp2)) {
            Position p = GeomHelper::intersection_position2D(p1, p2, lp1, lp2);
            ret.push_back(p.distanceTo2D(p1) + pos);
        }
        pos += p1.distanceTo2D(p2);
    }
    return ret;
}


void
PositionVector::extrapolate(const SUMOReal val, const bool onlyFirst) {
    assert(size() > 1);
    Position& p1 = (*this)[0];
    Position& p2 = (*this)[1];
    const Position offset = (p2 - p1) * (val / p1.distanceTo(p2));
    p1.sub(offset);
    if (!onlyFirst) {
        if (size() == 2) {
            p2.add(offset);
        } else {
            const Position& e1 = (*this)[-2];
            Position& e2 = (*this)[-1];
            e2.sub((e1 - e2) * (val / e1.distanceTo(e2)));
        }
    }
}


void
PositionVector::extrapolate2D(const SUMOReal val, const bool onlyFirst) {
    assert(size() > 1);
    Position& p1 = (*this)[0];
    Position& p2 = (*this)[1];
    const Position offset = (p2 - p1) * (val / p1.distanceTo2D(p2));
    p1.sub(offset);
    if (!onlyFirst) {
        if (size() == 2) {
            p2.add(offset);
        } else {
            const Position& e1 = (*this)[-2];
            Position& e2 = (*this)[-1];
            e2.sub((e1 - e2) * (val / e1.distanceTo2D(e2)));
        }
    }
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
            const Position& from = (*this)[i];
            const Position& to = (*this)[i + 1];
            const std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(Position(from.x() - offsets.first,
                                     from.y() - offsets.second, from.z()));
        } else if (i == static_cast<int>(size()) - 1) {
            const Position& from = (*this)[i - 1];
            const Position& to = (*this)[i];
            const std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, to, amount);
            shape.push_back(Position(to.x() - offsets.first,
                                     to.y() - offsets.second, to.z()));
        } else {
            const Position& from = (*this)[i - 1];
            const Position& me = (*this)[i];
            const Position& to = (*this)[i + 1];
            PositionVector fromMe(from, me);
            fromMe.extrapolate2D(me.distanceTo2D(to));
            const double extrapolateDev = fromMe[1].distanceTo2D(to);
            if (fabs(extrapolateDev) < POSITION_EPS) {
                // parallel case, just shift the middle point
                std::pair<SUMOReal, SUMOReal> off =
                    GeomHelper::getNormal90D_CW(from, to, amount);
                shape.push_back(Position(me.x() - off.first, me.y() - off.second, me.z()));
                continue;
            }
            if (fabs(extrapolateDev - 2 * me.distanceTo2D(to)) < POSITION_EPS) {
                // counterparallel case, just shift the middle point
                PositionVector fromMe(from, me);
                fromMe.extrapolate2D(amount);
                shape.push_back(fromMe[1]);
                continue;
            }
            std::pair<SUMOReal, SUMOReal> offsets =
                GeomHelper::getNormal90D_CW(from, me, amount);
            std::pair<SUMOReal, SUMOReal> offsets2 =
                GeomHelper::getNormal90D_CW(me, to, amount);
            PositionVector l1(
                Position(from.x() - offsets.first, from.y() - offsets.second),
                Position(me.x() - offsets.first, me.y() - offsets.second));
            l1.extrapolate2D(100);
            PositionVector l2(
                Position(me.x() - offsets2.first, me.y() - offsets2.second),
                Position(to.x() - offsets2.first, to.y() - offsets2.second));
            l2.extrapolate2D(100);
            if (GeomHelper::intersects(l1[0], l1[1], l2[0], l2[1])) {
                shape.push_back(GeomHelper::intersection_position2D(l1[0], l1[1], l2[0], l2[1]));
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


SUMOReal
PositionVector::angleAt2D(int pos) const {
    assert((int)size() > pos + 1);
    return (*this)[pos].angleTo2D((*this)[pos + 1]);
}


void
PositionVector::closePolygon() {
    if (size() == 0 || (*this)[0] == back()) {
        return;
    }
    push_back((*this)[0]);
}


std::vector<SUMOReal>
PositionVector::distances(const PositionVector& s, bool perpendicular) const {
    std::vector<SUMOReal> ret;
    const_iterator i;
    for (i = begin(); i != end(); i++) {
        const SUMOReal dist = s.distance(*i, perpendicular);
        if (dist != GeomHelper::INVALID_OFFSET) {
            ret.push_back(dist);
        }
    }
    for (i = s.begin(); i != s.end(); i++) {
        const SUMOReal dist = distance(*i, perpendicular);
        if (dist != GeomHelper::INVALID_OFFSET) {
            ret.push_back(dist);
        }
    }
    return ret;
}


SUMOReal
PositionVector::distance(const Position& p, bool perpendicular) const {
    if (size() == 0) {
        return std::numeric_limits<double>::max();
    } else if (size() == 1) {
        return front().distanceTo(p);
    }
    const SUMOReal nearestOffset = nearest_offset_to_point2D(p, perpendicular);
    if (nearestOffset == GeomHelper::INVALID_OFFSET) {
        return GeomHelper::INVALID_OFFSET;
    } else {
        return p.distanceTo2D(positionAtOffset2D(nearestOffset));
    }
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


bool
PositionVector::hasElevation() const {
    if (size() > 2) {
        return false;
    }
    for (const_iterator i = begin(); i != end() - 1; i++) {
        if ((*i).z() != (*(i + 1)).z()) {
            return true;
        }
    }
    return false;
}

/****************************************************************************/

