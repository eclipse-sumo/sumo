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


const Position&
Line::p1() const {
    return myP1;
}


const Position&
Line::p2() const {
    return myP2;
}


SUMOReal
Line::atan2DegreeAngle() const {
    return RAD2DEG(atan2(myP1.x() - myP2.x(), myP1.y() - myP2.y()));
}


std::ostream&
operator<<(std::ostream& os, const Line& line) {
    os << line.myP1 << " " << line.myP2;
    return os;
}



/****************************************************************************/
