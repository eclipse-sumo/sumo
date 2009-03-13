/****************************************************************************/
/// @file    Polygon2D.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "Polygon2D.h"
#include <utils/common/RGBColor.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member definitions
// ===========================================================================
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////
Polygon2D::Polygon2D(const std::string name, const std::string type,
                     const RGBColor &color, const Position2DVector &Pos,
                     bool fill)
        : myName(name),  myType(type), myColor(color), myPos(Pos), myFill(fill) {}


Polygon2D::~Polygon2D() {}


//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////
// return the name of the Polygon
const std::string &
Polygon2D::getName() const {
    return myName;
}


// return the typ of the Polygon
const std::string &
Polygon2D::getType() const {
    return myType;
}


// return the Color of the polygon
const RGBColor &
Polygon2D::getColor() const {
    return myColor;
}


// return the Positions Vector of the Polygon
const Position2DVector &
Polygon2D::getPosition2DVector() const {
    return myPos;
}


// add the Position2DVector to the polygon
void
Polygon2D::addPolyPosition(Position2DVector &myNewPos) {
    myPos = myNewPos;

}


bool
Polygon2D::fill() const {
    return myFill;
}



/****************************************************************************/

