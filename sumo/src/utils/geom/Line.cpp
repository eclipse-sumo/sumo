/****************************************************************************/
/// @file    Line.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2003-08-14
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "Position.h"
#include "Line.h"
#include "GeomHelper.h"
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================

Line::Line() {}


Line::Line(const Position& p1, const Position& p2)
    : myP1(p1), myP2(p2) {}


Line::~Line() {}


void
Line::extrapolateBy(SUMOReal length) {
    SUMOReal factor = length / myP1.distanceTo(myP2);
    Position offset = (myP2 - myP1) * factor;
    myP1.sub(offset);
    myP2.add(offset);
}


void
Line::extrapolateFirstBy(SUMOReal length) {
    myP1 = GeomHelper::extrapolate_first(myP1, myP2, length);
}


void
Line::extrapolateSecondBy(SUMOReal length) {
    myP2 = GeomHelper::extrapolate_second(myP1, myP2, length);
}

const Position&
Line::p1() const {
    return myP1;
}


const Position&
Line::p2() const {
    return myP2;
}


Position
Line::getPositionAtDistance(SUMOReal offset) const {
    const SUMOReal length = myP1.distanceTo(myP2);
    if (length == 0) {
        if (offset != 0) {
            throw InvalidArgument("Invalid offset " + toString(offset) + " for Line with length " + toString(length));;
        }
        return myP1;
    }
    return myP1 + (myP2 - myP1) * (offset / length);
}


Position
Line::getPositionAtDistance2D(SUMOReal offset) const {
    const SUMOReal length = myP1.distanceTo2D(myP2);
    if (length == 0) {
        if (offset != 0) {
            throw InvalidArgument("Invalid offset " + toString(offset) + " for Line with length " + toString(length));;
        }
        return myP1;
    }
    return myP1 + (myP2 - myP1) * (offset / length);
}


void
Line::move2side(SUMOReal amount) {
    std::pair<SUMOReal, SUMOReal> p = GeomHelper::getNormal90D_CW(myP1, myP2, amount);
    myP1.add(p.first, p.second);
    myP2.add(p.first, p.second);
}


std::vector<SUMOReal>
Line::intersectsAtLengths2D(const PositionVector& v) {
    PositionVector p = v.intersectionPoints2D(*this);
    std::vector<SUMOReal> ret;
    for (size_t i = 0; i < p.size(); i++) {
        ret.push_back(myP1.distanceTo2D(p[int(i)]));
    }
    return ret;
}


SUMOReal
Line::atan2Angle() const {
    return atan2(myP1.x() - myP2.x(), myP1.y() - myP2.y());
}


SUMOReal
Line::atan2DegreeAngle() const {
    return RAD2DEG(atan2(myP1.x() - myP2.x(), myP1.y() - myP2.y()));
}


SUMOReal
Line::atan2PositiveAngle() const {
    SUMOReal angle = atan2Angle();
    if (angle < 0) {
        angle = (SUMOReal) M_PI * (SUMOReal) 2.0 + angle;
    }
    return angle;
}


SUMOReal
Line::atan2DegreeSlope() const {
    return RAD2DEG(atan2(myP2.z() - myP1.z(), myP1.distanceTo2D(myP2)));
}


Position
Line::intersectsAt(const Line& l) const {
    return GeomHelper::intersection_position2D(myP1, myP2, l.myP1, l.myP2);
}


bool
Line::intersects(const Line& l) const {
    return GeomHelper::intersects(myP1, myP2, l.myP1, l.myP2);
}


SUMOReal
Line::length2D() const {
    return myP1.distanceTo2D(myP2);
}


SUMOReal
Line::length() const {
    return myP1.distanceTo(myP2);
}


void
Line::add(SUMOReal x, SUMOReal y) {
    myP1.add(x, y);
    myP2.add(x, y);
}


void
Line::add(const Position& p) {
    myP1.add(p.x(), p.y(), p.z());
    myP2.add(p.x(), p.y(), p.z());
}


void
Line::sub(SUMOReal x, SUMOReal y) {
    myP1.sub(x, y);
    myP2.sub(x, y);
}



Line&
Line::reverse() {
    Position tmp(myP1);
    myP1 = myP2;
    myP2 = tmp;
    return *this;
}


SUMOReal
Line::intersectsAtLength2D(const Line& v) {
    Position pos =
        GeomHelper::intersection_position2D(myP1, myP2, v.myP1, v.myP2);
    return GeomHelper::nearest_offset_on_line_to_point2D(myP1, myP2, pos);
}


void
Line::rotateAtP1(SUMOReal rot) {
    Position p = myP2;
    p.sub(myP1);
    p.reshiftRotate(0, 0, rot);
    p.add(myP1);
    myP2 = p;
}


/****************************************************************************/
