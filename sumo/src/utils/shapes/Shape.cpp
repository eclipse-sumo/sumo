/****************************************************************************/
/// @file    Shape.cpp
/// @author  Jakob Erdmann
/// @date    Oct 2012
/// @version $Id$
///
// A 2D- or 3D-Shape
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

#include "Shape.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static member definitions
// ===========================================================================
const std::string Shape::DEFAULT_TYPE = "";
const SUMOReal Shape::DEFAULT_LAYER = 128;
const SUMOReal Shape::DEFAULT_ANGLE = 0;
const std::string Shape::DEFAULT_IMG_FILE = "";
const SUMOReal Shape::DEFAULT_IMG_WIDTH = 1;
const SUMOReal Shape::DEFAULT_IMG_HEIGHT = 1;

// ===========================================================================
// member definitions
// ===========================================================================
Shape::Shape(const std::string& id, const std::string& type,
             const RGBColor& color, SUMOReal layer,
             SUMOReal angle, const std::string& imgFile) :
    Named(id),
    myType(type),
    myColor(color),
    myLayer(layer),
    myAngle(angle),
    myImgFile(imgFile)
{}


Shape::~Shape() {}


/****************************************************************************/

