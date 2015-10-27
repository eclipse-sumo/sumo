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
Line::extrapolateBy2D(SUMOReal length) {
    SUMOReal factor = length / myP1.distanceTo2D(myP2);
    Position offset = (myP2 - myP1) * factor;
    myP1.sub(offset);
    myP2.add(offset);
}


const Position&
Line::p1() const {
    return myP1;
}


const Position&
Line::p2() const {
    return myP2;
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


std::ostream&
operator<<(std::ostream& os, const Line& line) {
    os << line.myP1 << " " << line.myP2;
    return os;
}



/****************************************************************************/
