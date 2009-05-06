/****************************************************************************/
/// @file    Polygon2D.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// A 2D-polygon
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
Polygon2D::Polygon2D(const std::string &name, const std::string &type,
                     const RGBColor &color, const Position2DVector &shape,
                     bool fill) throw()
        : myName(name),  myType(type), myColor(color), myShape(shape), myFill(fill) {}


Polygon2D::~Polygon2D() throw() {}



/****************************************************************************/

