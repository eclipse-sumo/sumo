/****************************************************************************/
/// @file    Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Jun 2004
/// @version $Id$
///
// A 2D-polygon
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

#include "Polygon.h"
using namespace SUMO;

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
Polygon::Polygon(const std::string& id, const std::string& type,
                 const RGBColor& color, const PositionVector& shape, bool fill,
                 SUMOReal layer, SUMOReal angle, const std::string& imgFile) :
    Shape(id, type, color, layer, angle, imgFile),
    myShape(shape),
    myFill(fill)
{}


Polygon::~Polygon() {}



/****************************************************************************/

