/****************************************************************************/
/// @file    Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Jun 2004
/// @version $Id$
///
// A 2D-polygon
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

#include "Polygon.h"
#include <utils/common/RGBColor.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
Polygon::Polygon(const std::string& name, const std::string& type,
                 const RGBColor& color, const PositionVector& shape,
                 bool fill)
    : myName(name),  myType(type), myColor(color), myShape(shape), myFill(fill) {}


Polygon::~Polygon() {}



/****************************************************************************/

