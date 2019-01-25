/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Shape.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Oct 2012
/// @version $Id$
///
// A 2D- or 3D-Shape
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "Shape.h"

// ===========================================================================
// static member definitions
// ===========================================================================
const std::string Shape::DEFAULT_TYPE = "";
const double Shape::DEFAULT_LAYER = 128;
const double Shape::DEFAULT_LINEWIDTH = 1;
const double Shape::DEFAULT_LAYER_POI = (double)GLO_POI;
const double Shape::DEFAULT_ANGLE = 0;
const std::string Shape::DEFAULT_IMG_FILE = "";
const bool Shape::DEFAULT_RELATIVEPATH = false;
const double Shape::DEFAULT_IMG_WIDTH = 1;
const double Shape::DEFAULT_IMG_HEIGHT = 1;

// ===========================================================================
// member definitions
// ===========================================================================
Shape::Shape(const std::string& id, const std::string& type,
             const RGBColor& color, double layer,
             double angle, const std::string& imgFile, bool relativePath) :
    Named(id),
    myType(type),
    myColor(color),
    myLayer(layer),
    myNaviDegreeAngle(angle),
    myImgFile(imgFile),
    myRelativePath(relativePath) {
}


Shape::~Shape() {}


/****************************************************************************/

