/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Shape.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Oct 2012
///
// A 2D- or 3D-Shape
/****************************************************************************/
#include <config.h>

#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/iodevices/OutputDevice.h>

#include "Shape.h"

// ===========================================================================
// static member definitions
// ===========================================================================

const std::string Shape::DEFAULT_TYPE = "";
const double Shape::DEFAULT_LAYER = 0;
const double Shape::DEFAULT_LINEWIDTH = 1;
const double Shape::DEFAULT_LAYER_POI = (double)GLO_POI;
const double Shape::DEFAULT_ANGLE = 0;
const std::string Shape::DEFAULT_IMG_FILE = "";
const double Shape::DEFAULT_IMG_WIDTH = 2.6;
const double Shape::DEFAULT_IMG_HEIGHT = 1;
const std::string Shape::DEFAULT_NAME = "";
const Parameterised::Map Shape::DEFAULT_PARAMETERS = Parameterised::Map();

// ===========================================================================
// member definitions
// ===========================================================================

Shape::Shape(const std::string& id) :
    Named(id),
    myType(DEFAULT_TYPE),
    myColor(RGBColor::BLACK),
    myLayer(DEFAULT_LAYER),
    myNaviDegreeAngle(DEFAULT_ANGLE),
    myImgFile(DEFAULT_IMG_FILE),
    myName(DEFAULT_NAME) {
}


Shape::Shape(const std::string& id, const std::string& type, const RGBColor& color, double layer,
             double angle, const std::string& imgFile, const std::string& name) :
    Named(id),
    myType(type),
    myColor(color),
    myLayer(layer),
    myNaviDegreeAngle(angle),
    myImgFile(imgFile),
    myName(name) {
}


Shape::~Shape() {}


void
Shape::writeShapeAttributes(OutputDevice& device, const RGBColor& defaultColor, const double defaultLayer) const {
    // name
    if (myName != DEFAULT_NAME) {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }
    // type
    if (myType != DEFAULT_TYPE) {
        device.writeAttr(SUMO_ATTR_TYPE, StringUtils::escapeXML(myType));
    }
    // color
    if (myColor != defaultColor) {
        device.writeAttr(SUMO_ATTR_COLOR, myColor);
    }
    // layer
    if (myLayer != defaultLayer) {
        device.writeAttr(SUMO_ATTR_LAYER, myLayer);
    }
    // angle
    if (myNaviDegreeAngle != Shape::DEFAULT_ANGLE) {
        device.writeAttr(SUMO_ATTR_ANGLE, myNaviDegreeAngle);
    }
    // img file
    if (myImgFile != Shape::DEFAULT_IMG_FILE) {
        device.writeAttr(SUMO_ATTR_IMGFILE, myImgFile);
    }
}


const std::string&
Shape::getShapeType() const {
    return myType;
}


const RGBColor&
Shape::getShapeColor() const {
    return myColor;
}


double
Shape::getShapeLayer() const {
    return myLayer;
}


double
Shape::getShapeNaviDegree() const {
    return myNaviDegreeAngle;
}


const std::string&
Shape::getShapeImgFile() const {
    return myImgFile;
}


const std::string&
Shape::getShapeName() const {
    return myName;
}


void
Shape::setShapeType(const std::string& type) {
    myType = type;
}


void
Shape::setShapeColor(const RGBColor& col) {
    myColor = col;
}


void
Shape::setShapeAlpha(unsigned char alpha) {
    myColor.setAlpha(alpha);
}


void
Shape::setShapeLayer(const double layer) {
    myLayer = layer;
}


void
Shape::setShapeNaviDegree(const double angle) {
    myNaviDegreeAngle = angle;
}


void
Shape::setShapeImgFile(const std::string& imgFile) {
    myImgFile = imgFile;
}

void
Shape::setShapeName(const std::string& name) {
    myName = name;
}

/****************************************************************************/
