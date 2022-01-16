/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GUIDottedGeometry.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// File for dotted geometry classes and functions
/****************************************************************************/
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "GUIDottedGeometry.h"
#include "GUIGeometry.h"


#define MAXIMUM_DOTTEDGEOMETRYLENGTH 500.0

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GUIDottedGeometry::DottedGeometryColor - methods
// ---------------------------------------------------------------------------

GUIDottedGeometry::DottedGeometryColor::DottedGeometryColor(const GUIVisualizationSettings& settings) :
    mySettings(settings),
    myColorFlag(true) {}


const RGBColor
GUIDottedGeometry::DottedGeometryColor::getColor(DottedContourType type) {
    if (type == DottedContourType::INSPECT) {
        if (myColorFlag) {
            myColorFlag = false;
            return mySettings.dottedContourSettings.firstInspectedColor;
        } else {
            myColorFlag = true;
            return mySettings.dottedContourSettings.secondInspectedColor;
        }
    } else if (type == DottedContourType::FRONT) {
        if (myColorFlag) {
            myColorFlag = false;
            return mySettings.dottedContourSettings.firstFrontColor;
        } else {
            myColorFlag = true;
            return mySettings.dottedContourSettings.secondFrontColor;
        }
    } else if (type == DottedContourType::GREEN) {
        if (myColorFlag) {
            myColorFlag = false;
            return RGBColor::GREEN;
        } else {
            myColorFlag = true;
            return RGBColor::GREEN.changedBrightness(-30);
        }
    } else if (type == DottedContourType::MAGENTA) {
        if (myColorFlag) {
            myColorFlag = false;
            return RGBColor::MAGENTA;
        } else {
            myColorFlag = true;
            return RGBColor::MAGENTA.changedBrightness(-30);
        }
    } else {
        return RGBColor::BLACK;
    }
}


void
GUIDottedGeometry::DottedGeometryColor::changeColor() {
    if (myColorFlag) {
        myColorFlag = false;
    } else {
        myColorFlag = true;
    }
}


void
GUIDottedGeometry::DottedGeometryColor::reset() {
    myColorFlag = true;
}

// ---------------------------------------------------------------------------
// GUIDottedGeometry::Segment - methods
// ---------------------------------------------------------------------------

GUIDottedGeometry::Segment::Segment() :
    offset(-1) {
}


GUIDottedGeometry::Segment::Segment(PositionVector newShape) :
    shape(newShape),
    offset(-1) {
}

// ---------------------------------------------------------------------------
// GUIDottedGeometry - methods
// ---------------------------------------------------------------------------

GUIDottedGeometry::GUIDottedGeometry() :
    myWidth(0) {
}


GUIDottedGeometry::GUIDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape) :
    myWidth(s.dottedContourSettings.segmentWidth) {
    // check if shape has to be closed
    if (closeShape && (shape.size() > 2)) {
        shape.closePolygon();
    }
    if (shape.size() > 1) {
        // get shape
        for (int i = 1; i < (int)shape.size(); i++) {
            myDottedGeometrySegments.push_back(Segment({shape[i - 1], shape[i]}));
        }
        // calculate segment length
        double segmentLength = s.dottedContourSettings.segmentLength;
        if (shape.length2D() > MAXIMUM_DOTTEDGEOMETRYLENGTH) {
            segmentLength = shape.length2D() / (MAXIMUM_DOTTEDGEOMETRYLENGTH * 0.5);
        }
        // resample
        for (auto& segment : myDottedGeometrySegments) {
            segment.shape = segment.shape.resample(segmentLength, true);
        }
        // calculate shape rotations and lenghts
        calculateShapeRotationsAndLengths();
    }
}


GUIDottedGeometry::GUIDottedGeometry(const GUIVisualizationSettings& s,
                                     const GUIDottedGeometry& topDottedGeometry, const bool drawFirstExtrem,
                                     const GUIDottedGeometry& botDottedGeometry, const bool drawLastExtrem) :
    myWidth(s.dottedContourSettings.segmentWidth) {
    // check size of both geometries
    if ((topDottedGeometry.myDottedGeometrySegments.size() > 0) &&
            (botDottedGeometry.myDottedGeometrySegments.size() > 0)) {
        // add extremes
        if (drawFirstExtrem &&
                (topDottedGeometry.myDottedGeometrySegments.front().shape.size() > 0) &&
                (botDottedGeometry.myDottedGeometrySegments.front().shape.size() > 0)) {
            // add first extreme
            myDottedGeometrySegments.push_back(Segment({
                topDottedGeometry.myDottedGeometrySegments.front().shape.front(),
                botDottedGeometry.myDottedGeometrySegments.front().shape.front()}));
        }
        if (drawLastExtrem &&
                (topDottedGeometry.myDottedGeometrySegments.back().shape.size() > 0) &&
                (botDottedGeometry.myDottedGeometrySegments.back().shape.size() > 0)) {
            // add last extreme
            myDottedGeometrySegments.push_back(Segment({
                topDottedGeometry.myDottedGeometrySegments.back().shape.back(),
                botDottedGeometry.myDottedGeometrySegments.back().shape.back()}));
            // invert offset of second dotted geometry
            myDottedGeometrySegments.back().offset *= -1;
        }
    }
    // resample
    for (auto& segment : myDottedGeometrySegments) {
        segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
    }
    // calculate shape rotations and lenghts
    calculateShapeRotationsAndLengths();
}


void
GUIDottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, const PositionVector& laneShape) {
    // update settings and width
    myWidth = s.dottedContourSettings.segmentWidth;
    // reset segments
    myDottedGeometrySegments.clear();
    // get shape
    for (int i = 1; i < (int)laneShape.size(); i++) {
        myDottedGeometrySegments.push_back(Segment({laneShape[i - 1], laneShape[i]}));
    }
    // resample
    for (auto& segment : myDottedGeometrySegments) {
        segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
    }
    // calculate shape rotations and lenghts
    calculateShapeRotationsAndLengths();
}


void
GUIDottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape) {
    // update settings and width
    myWidth = s.dottedContourSettings.segmentWidth;
    // reset segments
    myDottedGeometrySegments.clear();
    // check if shape has to be closed
    if (closeShape && (shape.size() > 2)) {
        shape.closePolygon();
    }
    if (shape.size() > 1) {
        // get shape
        for (int i = 1; i < (int)shape.size(); i++) {
            myDottedGeometrySegments.push_back(Segment({shape[i - 1], shape[i]}));
        }
        // resample
        for (auto& segment : myDottedGeometrySegments) {
            segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
        }
        // calculate shape rotations and lenghts
        calculateShapeRotationsAndLengths();
    }
}


void
GUIDottedGeometry::drawDottedGeometry(DottedGeometryColor& dottedGeometryColor,
                                      GUIDottedGeometry::DottedContourType type, const double customWidth) const {
    // get width
    const double width = (customWidth > 0) ? customWidth : myWidth;
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        // iterate over shape
        for (int i = 0; i < ((int)segment.shape.size() - 1); i++) {
            // set color
            GLHelper::setColor(dottedGeometryColor.getColor(type));
            // draw box line
            GLHelper::drawBoxLine(segment.shape[i], segment.rotations.at(i), segment.lengths.at(i), width, 0);
        }
    }
}


void
GUIDottedGeometry::moveShapeToSide(const double value) {
    // move 2 side
    for (auto& segment : myDottedGeometrySegments) {
        segment.shape.move2side(value);
    }
}


double
GUIDottedGeometry::getWidth() const {
    return myWidth;
}


void
GUIDottedGeometry::setWidth(const double width) {
    myWidth = width;
}


void
GUIDottedGeometry::invertOffset() {
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        segment.offset *= -1;
    }
}


void
GUIDottedGeometry::drawDottedContourClosedShape(const DottedContourType type, const GUIVisualizationSettings& s,
        const PositionVector& shape, const double exaggeration, const double lineWidth) {
    if (s.drawDottedContour(exaggeration)) {
        // declare DottedGeometryColor
        DottedGeometryColor dottedGeometryColor(s);
        // scale shape using exaggeration and default dotted geometry width
        PositionVector scaledShape = shape;
        // scale exaggeration
        scaledShape.scaleRelative(exaggeration);
        // calculate dotted geometry
        GUIDottedGeometry dottedGeometry(s, scaledShape, true);
        // Push draw matrix
        GLHelper::pushMatrix();
        // draw inspect or front dotted contour
        if (type == DottedContourType::FRONT) {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
        } else {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
        }
        // draw dotted geometry
        dottedGeometry.drawDottedGeometry(dottedGeometryColor, type, lineWidth);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GUIDottedGeometry::drawDottedContourShape(const DottedContourType type, const GUIVisualizationSettings& s,
        const PositionVector& shape, const double width, const double exaggeration, const bool drawFirstExtrem,
        const bool drawLastExtrem, const double lineWidth) {
    if (s.drawDottedContour(exaggeration)) {
        // declare DottedGeometryColor
        DottedGeometryColor dottedGeometryColor(s);
        // calculate center dotted geometry
        GUIDottedGeometry dottedGeometry(s, shape, false);
        // make a copy of dotted geometry
        GUIDottedGeometry topDottedGeometry = dottedGeometry;
        GUIDottedGeometry botDottedGeometry = dottedGeometry;
        // move geometries top and bot
        topDottedGeometry.moveShapeToSide(width * exaggeration);
        botDottedGeometry.moveShapeToSide(width * exaggeration * -1);
        // invert offset of top dotted geometry
        topDottedGeometry.invertOffset();
        // calculate extremes
        GUIDottedGeometry extremes(s, topDottedGeometry, drawFirstExtrem, botDottedGeometry, drawLastExtrem);
        // Push draw matrix
        GLHelper::pushMatrix();
        // translate to front
        if (type == DottedContourType::FRONT) {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_FRONT);
        } else {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
        }
        // draw top dotted geometry
        topDottedGeometry.drawDottedGeometry(dottedGeometryColor, type, lineWidth);
        // reset color
        dottedGeometryColor.reset();
        // draw top dotted geometry
        botDottedGeometry.drawDottedGeometry(dottedGeometryColor, type, lineWidth);
        // change color
        dottedGeometryColor.changeColor();
        // draw extrem dotted geometry
        extremes.drawDottedGeometry(dottedGeometryColor, type, lineWidth);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GUIDottedGeometry::drawDottedContourCircle(const DottedContourType type, const GUIVisualizationSettings& s,
        const Position& pos, const double radius, const double exaggeration) {
    // continue depending of exaggeratedRadio
    if ((radius * exaggeration) < 2) {
        drawDottedContourClosedShape(type, s, GUIGeometry::getVertexCircleAroundPosition(pos, radius, 8), exaggeration);
    } else {
        drawDottedContourClosedShape(type, s, GUIGeometry::getVertexCircleAroundPosition(pos, radius, 16), exaggeration);
    }
}


void
GUIDottedGeometry::drawDottedSquaredShape(const DottedContourType type, const GUIVisualizationSettings& s, const Position& pos,
        const double width, const double height, const double offsetX, const double offsetY, const double rot, const double exaggeration) {
    if (s.drawDottedContour(exaggeration)) {
        // create shape
        PositionVector shape;
        // make rectangle
        shape.push_back(Position(0 + width, 0 + height));
        shape.push_back(Position(0 + width, 0 - height));
        shape.push_back(Position(0 - width, 0 - height));
        shape.push_back(Position(0 - width, 0 + height));
        // move shape
        shape.add(offsetX, offsetY, 0);
        // rotate shape
        shape.rotate2D(DEG2RAD((rot * -1) + 90));
        // move to position
        shape.add(pos);
        // draw using drawDottedContourClosedShape
        drawDottedContourClosedShape(type, s, shape, exaggeration);
    }
}


void
GUIDottedGeometry::calculateShapeRotationsAndLengths() {
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        // Get number of parts of the shape
        int numberOfSegments = (int)segment.shape.size() - 1;
        // If number of segments is more than 0
        if (numberOfSegments >= 0) {
            // Reserve memory (To improve efficiency)
            segment.rotations.reserve(numberOfSegments);
            segment.lengths.reserve(numberOfSegments);
            // Calculate lengths and rotations for every shape
            for (int i = 0; i < numberOfSegments; i++) {
                segment.rotations.push_back(GUIGeometry::calculateRotation(segment.shape[i], segment.shape[i + 1]));
                segment.lengths.push_back(GUIGeometry::calculateLength(segment.shape[i], segment.shape[i + 1]));
            }
        }
    }
}

/****************************************************************************/
