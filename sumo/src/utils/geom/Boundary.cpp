/****************************************************************************/
/// @file    Boundary.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A class that stores the 2D geometrical boundary
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
#include <utility>

#include "GeomHelper.h"
#include "Boundary.h"
#include "PositionVector.h"
#include "Position.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Boundary::Boundary()
    : myXmin(10000000000.0), myXmax(-10000000000.0),
      myYmin(10000000000.0), myYmax(-10000000000.0),
      myWasInitialised(false) {}


Boundary::Boundary(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2)
    : myXmin(10000000000.0), myXmax(-10000000000.0),
      myYmin(10000000000.0), myYmax(-10000000000.0),
      myWasInitialised(false) {
    add(x1, y1);
    add(x2, y2);
}


Boundary::~Boundary() {}


void
Boundary::reset() {
    myXmin = 10000000000.0;
    myXmax = -10000000000.0;
    myYmin = 10000000000.0;
    myYmax = -10000000000.0;
    myWasInitialised = false;
}


void
Boundary::add(SUMOReal x, SUMOReal y) {
    if (!myWasInitialised) {
        myYmin = y;
        myYmax = y;
        myXmin = x;
        myXmax = x;
    } else {
        myXmin = myXmin < x ? myXmin : x;
        myXmax = myXmax > x ? myXmax : x;
        myYmin = myYmin < y ? myYmin : y;
        myYmax = myYmax > y ? myYmax : y;
    }
    myWasInitialised = true;
}


void
Boundary::add(const Position& p) {
    add(p.x(), p.y());
}


void
Boundary::add(const Boundary& p) {
    add(p.xmin(), p.ymin());
    add(p.xmax(), p.ymax());
}


Position
Boundary::getCenter() const {
    return Position((myXmin + myXmax) / (SUMOReal) 2.0, (myYmin + myYmax) / (SUMOReal) 2.0);
}


SUMOReal
Boundary::xmin() const {
    return myXmin;
}


SUMOReal
Boundary::xmax() const {
    return myXmax;
}


SUMOReal
Boundary::ymin() const {
    return myYmin;
}


SUMOReal
Boundary::ymax() const {
    return myYmax;
}


SUMOReal
Boundary::getWidth() const {
    return myXmax - myXmin;
}


SUMOReal
Boundary::getHeight() const {
    return myYmax - myYmin;
}


bool
Boundary::around(const Position& p, SUMOReal offset) const {
    return
        (p.x() <= myXmax + offset && p.x() >= myXmin - offset) &&
        (p.y() <= myYmax + offset && p.y() >= myYmin - offset);
}


bool
Boundary::overlapsWith(const AbstractPoly& p, SUMOReal offset) const {
    if (
        // check whether one of my points lies within the given poly
        partialWithin(p, offset) ||
        // check whether the polygon lies within me
        p.partialWithin(*this, offset)) {
        return true;
    }
    // check whether the bounderies cross
    return
        p.crosses(Position(myXmax + offset, myYmax + offset), Position(myXmin - offset, myYmax + offset))
        ||
        p.crosses(Position(myXmin - offset, myYmax + offset), Position(myXmin - offset, myYmin - offset))
        ||
        p.crosses(Position(myXmin - offset, myYmin - offset), Position(myXmax + offset, myYmin - offset))
        ||
        p.crosses(Position(myXmax + offset, myYmin - offset), Position(myXmax + offset, myYmax + offset));
}


bool
Boundary::crosses(const Position& p1, const Position& p2) const {
    return
        GeomHelper::intersects(p1, p2, Position(myXmax, myYmax), Position(myXmin, myYmax))
        ||
        GeomHelper::intersects(p1, p2, Position(myXmin, myYmax), Position(myXmin, myYmin))
        ||
        GeomHelper::intersects(p1, p2, Position(myXmin, myYmin), Position(myXmax, myYmin))
        ||
        GeomHelper::intersects(p1, p2, Position(myXmax, myYmin), Position(myXmax, myYmax));
}


bool
Boundary::partialWithin(const AbstractPoly& poly, SUMOReal offset) const {
    return
        poly.around(Position(myXmax, myYmax), offset) ||
        poly.around(Position(myXmin, myYmax), offset) ||
        poly.around(Position(myXmax, myYmin), offset) ||
        poly.around(Position(myXmin, myYmin), offset);
}


Boundary&
Boundary::grow(SUMOReal by) {
    myXmax += by;
    myYmax += by;
    myXmin -= by;
    myYmin -= by;
    return *this;
}

void
Boundary::growWidth(SUMOReal by) {
    myXmin -= by;
    myXmax += by;
}


void
Boundary::growHeight(SUMOReal by) {
    myYmin -= by;
    myYmax += by;
}

void
Boundary::flipY() {
    myYmin *= -1.0;
    myYmax *= -1.0;
    SUMOReal tmp = myYmin;
    myYmin = myYmax;
    myYmax = tmp;
}



std::ostream&
operator<<(std::ostream& os, const Boundary& b) {
    os << b.myXmin << "," << b.myYmin << "," << b.myXmax << "," << b.myYmax;
    return os;
}


void
Boundary::set(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax) {
    myXmin = xmin;
    myYmin = ymin;
    myXmax = xmax;
    myYmax = ymax;
}


void
Boundary::moveby(SUMOReal x, SUMOReal y) {
    myXmin += x;
    myYmin += y;
    myXmax += x;
    myYmax += y;
}



/****************************************************************************/

