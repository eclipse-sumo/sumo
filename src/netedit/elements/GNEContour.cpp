/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEContour.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// class used for show contour elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIGlobalObjectsInPosition.h>

#include "GNEContour.h"
#include "GNEAttributeCarrier.h"


// ===========================================================================
// static members
// ===========================================================================

GUIDottedGeometry::DottedGeometryColor GNEContour::myDottedGeometryColor;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEContour::GNEContour(GNEAttributeCarrier* AC) :
    myAC(AC),
    myDottedGeometries(new std::vector<GUIDottedGeometry>()) {
}


GNEContour::~GNEContour() {
    delete myDottedGeometries;
}


Boundary
GNEContour::getContourBoundary() const {
    Boundary b;
    for (const auto &dottedGeometry : *myDottedGeometries) {
        b.add(dottedGeometry.getUnresampledShape().getBoxBoundary());
    }
    return b;
}


void
GNEContour::reset() const {
    myDottedGeometries->clear();
}


void
GNEContour::drawDottedContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, const double scale, const bool addOffset, const double lineWidth) const {
    // check if mouse is within geometry (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // first build dotted contour
        const auto closedShape = buildDottedContourClosed(s, shape, scale);
        gObjectsInPosition.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), closedShape);
    } else {
        drawDottedContours(s, d, scale, addOffset, lineWidth);
    }
}


void
GNEContour::drawDottedContourExtruded(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, const double extrusionWidth, const double scale, const bool drawFirstExtrem,
        const bool drawLastExtrem, const double offset, const double lineWidth) const {
    // check if mouse is within two lines (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // first build dotted contour
        const auto extrudedShape = buildDottedContourExtruded(s, shape, extrusionWidth, scale, drawFirstExtrem, drawLastExtrem, offset);
        gObjectsInPosition.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), extrudedShape);
    } else {
        drawDottedContours(s, d, scale, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const Position& pos, const double width, const double height, const double offsetX, const double offsetY,
        const double rot, const double scale, const double lineWidth) const {
    // check if mouse is within geometry (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // first build dotted contour
        const auto rectangleShape = buildDottedContourRectangle(s, pos, width, height, offsetX, offsetY, rot, scale);
        gObjectsInPosition.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), rectangleShape);
    } else {
        drawDottedContours(s, d, scale, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const Position& pos, double radius, const double scale, const double lineWidth) const {
    // check if mouse is within geometry (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // build dotted contour
        buildDottedContourCircle(s, pos, radius, scale);
        gObjectsInPosition.positionWithinCircle(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), pos, (radius * scale));
    } else {
        drawDottedContours(s, d, scale, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, GeometryPoint geometryPoints, double radius, const double scale, const double lineWidth) const {
    // iterate over all geometry points
    for (int i = 0; i < (int)shape.size(); i++) {
        const bool first = (i == 0); 
        const bool last = ((i + 1) == (int)shape.size());
        // check conditions
        if ((geometryPoints == GeometryPoint::ALL) ||
            (first && (geometryPoints == GeometryPoint::FROM)) ||
            (last && (geometryPoints == GeometryPoint::TO)) ||
            (!first && !last && (geometryPoints == GeometryPoint::MIDDLE))) {
            // continue depending if we're checking position within geometry point or drawing dotted contour
            if (s.drawForObjectUnderCursor) {
                // build dotted contour
                gObjectsInPosition.positionWithinGeometryPoint(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), i, shape[i], (radius * scale));
            } else if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours) &&
                       gObjectsInPosition.isGeometryPointUnderCursor(myAC->getGUIGlObject(), i)) {
                // build dotted contour circle
                buildDottedContourCircle(s, shape[i], radius, scale);
                // draw geometry point
                drawDottedContour(s, GUIDottedGeometry::DottedContourType::MOVE, 0, lineWidth);
            }
        }
    }
}


void
GNEContour::drawDottedContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem, const double lineWidth) const {
    // check if mouse is within two lines (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // build dotted contour
        const auto contourShape = buildDottedContourEdge(s, edge, drawFirstExtrem, drawLastExtrem);
        gObjectsInPosition.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), contourShape);
    } else {
        drawDottedContours(s, d, 1, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GNEEdge* fromEdge, const GNEEdge* toEdge, const double lineWidth) const {
    // first build dotted contour (only in rectangle selection mode)
    buildDottedContourEdges(s, fromEdge, toEdge);
    // draw dotted contours
    drawDottedContours(s, d, 1, true, lineWidth);
}


void
GNEContour::drawInnenContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                   const double scale, const double lineWidth) const {
    // check dotted caches
    checkDottedCaches(1);
    // declare scaled shape
    PositionVector scaledShape = shape;
    // scale shape
    scaledShape.scaleRelative(scale);
    // close
    scaledShape.closePolygon();
    // calculate geometry without resampling
    myDottedGeometries->at(0) = GUIDottedGeometry(s, scaledShape, true, false);
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // draw dotted
    myDottedGeometries->at(0).drawInnenGeometry(lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContour::checkDottedCaches(size_t dottedGeometries) const {
    // check dotted geometries
    if (myDottedGeometries->size() != dottedGeometries) {
        myDottedGeometries->clear();
        for (size_t i = 0; i < dottedGeometries; i++) {
            myDottedGeometries->push_back(GUIDottedGeometry());
        }
    }
}

PositionVector
GNEContour::buildDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape, const double scale) const {
    // check dotted caches
    checkDottedCaches(1);
    // declare scaled shape
    PositionVector scaledShape = shape;
    // scale shape
    scaledShape.scaleRelative(scale);
    // close
    scaledShape.closePolygon();
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, scaledShape, true, true);
    // finally return scaled shape (used for check mouse pos)
    return scaledShape;
}


PositionVector
GNEContour::buildDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape, const double extrusionWidth,
        const double scale, const bool drawFirstExtrem, const bool drawLastExtrem, const double offset) const {
    // check dotted caches
    checkDottedCaches(4);
    // create top and bot geometries
    myDottedGeometries->at(0) = GUIDottedGeometry(s, shape, false, true);
    myDottedGeometries->at(2) = GUIDottedGeometry(s, shape.reverse(), false, true);
    // move geometries top and bot
    myDottedGeometries->at(0).moveShapeToSide((extrusionWidth * scale * -1) + offset);
    myDottedGeometries->at(2).moveShapeToSide((extrusionWidth * scale * -1) - offset);
    // create left and right geometries
    if (drawFirstExtrem) {
        myDottedGeometries->at(3) = GUIDottedGeometry(s, {
            myDottedGeometries->at(2).getBackPosition(),
            myDottedGeometries->at(0).getFrontPosition()
        }, false, true);
    }
    if (drawLastExtrem) {
        myDottedGeometries->at(1) = GUIDottedGeometry(s, {
            myDottedGeometries->at(0).getBackPosition(),
            myDottedGeometries->at(2).getFrontPosition()
        }, false, true);
    }
    // finally create shape used for check position over mouse
    PositionVector extrudedShape;
    for (const auto &position : myDottedGeometries->at(0).getUnresampledShape()) {
        extrudedShape.push_back(position);
    }
    for (const auto &position : myDottedGeometries->at(2).getUnresampledShape()) {
        extrudedShape.push_back(position);
    }
    return extrudedShape;
}


PositionVector
GNEContour::buildDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width, const double height,
        const double offsetX, const double offsetY, const double rot, const double scale) const {
    // check dotted caches
    checkDottedCaches(1);
    // create shape
    PositionVector rectangleShape;
    // make rectangle
    rectangleShape.push_back(Position(0 + width, 0 + height));
    rectangleShape.push_back(Position(0 + width, 0 - height));
    rectangleShape.push_back(Position(0 - width, 0 - height));
    rectangleShape.push_back(Position(0 - width, 0 + height));
    // move shape
    rectangleShape.add(offsetX, offsetY, 0);
    // scale
    rectangleShape.scaleRelative(scale);
    // rotate shape
    rectangleShape.rotate2D(DEG2RAD((rot * -1) + 90));
    // move to position
    rectangleShape.add(pos);
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, rectangleShape, true, true);
    // return rectangle shape used for check position over mouse
    return rectangleShape;
}


PositionVector
GNEContour::buildDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius, const double scale) const {
    // check dotted caches
    checkDottedCaches(1);
    // get vertex circle shape
    const auto circleShape = GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, (radius * scale) < 2 ? 8 : 16);
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, circleShape, true, true);
    // return circle shape used for check position over mouse
    return circleShape;
}


PositionVector
GNEContour::buildDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // check dotted caches
    checkDottedCaches(4);
    // set left hand flag
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    // obtain lanes
    const GNELane* topLane = lefthand ? edge->getLanes().front() : edge->getLanes().back();
    const GNELane* botLane = lefthand ? edge->getLanes().back() : edge->getLanes().front();
    // create top and bot geometries
    myDottedGeometries->at(0) = GUIDottedGeometry(s, topLane->getLaneGeometry().getShape(), false, true);
    myDottedGeometries->at(2) = GUIDottedGeometry(s, botLane->getLaneGeometry().getShape().reverse(), false, true);
    // move geometries top and bot
    myDottedGeometries->at(0).moveShapeToSide((topLane->getDrawingConstants()->getDrawingWidth() * -1) + topLane->getDrawingConstants()->getOffset());
    myDottedGeometries->at(2).moveShapeToSide((botLane->getDrawingConstants()->getDrawingWidth() * -1) - botLane->getDrawingConstants()->getOffset());
    // create left and right geometries
    if (drawFirstExtrem) {
        myDottedGeometries->at(3) = GUIDottedGeometry(s, {
            myDottedGeometries->at(2).getBackPosition(),
            myDottedGeometries->at(0).getFrontPosition()
        }, false, true);
    }
    if (drawLastExtrem) {
        myDottedGeometries->at(1) = GUIDottedGeometry(s, {
            myDottedGeometries->at(0).getBackPosition(),
            myDottedGeometries->at(2).getFrontPosition()
        }, false, true);
    }
    // finally create shape used in positionWithinClosedShape
    PositionVector shapeContour;
    for (const auto &position : myDottedGeometries->at(0).getUnresampledShape()) {
        shapeContour.push_back(position);
    }
    for (const auto &position : myDottedGeometries->at(2).getUnresampledShape()) {
        shapeContour.push_back(position);
    }
    // return shape contour used for check position over mouse
    return shapeContour;
}


PositionVector
GNEContour::buildDottedContourEdges(const GUIVisualizationSettings& /*s*/,const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */) const {
    // check dotted caches
    checkDottedCaches(4);

    /* FINISH */
    return PositionVector();
}


void
GNEContour::drawDottedContours(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const double scale, const bool addOffset, const double lineWidth) const {
    // first check if draw dotted contour
    if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::FROM, addOffset, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::TO, addOffset, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::RELATED, addOffset, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::OVER, addOffset, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::INSPECT, addOffset, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::FRONT, addOffset, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::REMOVE, addOffset, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::SELECT, addOffset, lineWidth);
        }
    }
}


void
GNEContour::drawDottedContour(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type, const bool addOffset, const double lineWidth) const {
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometries
    for (const auto &dottedGeometry : *myDottedGeometries) {
        dottedGeometry.drawDottedGeometry(s, type, myDottedGeometryColor, addOffset, lineWidth);
    }
    // pop matrix
    GLHelper::popMatrix();
}



/****************************************************************************/
