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
    myDottedGeometries(new std::vector<GUIDottedGeometry>(4)) {
}


GNEContour::~GNEContour() {
    delete myDottedGeometries;
}


Boundary
GNEContour::getContourBoundary() const {
    Boundary b;
    for (const auto &dottedGeometry : *myDottedGeometries) {
        if (dottedGeometry.getUnresampledShape().size() > 0) {
            b.add(dottedGeometry.getUnresampledShape().getBoxBoundary());
        }
    }
    return b;
}


void
GNEContour::drawDottedContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, const double scale, const bool addOffset, const double lineWidth) const {
    // check if mouse is within geometry (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // first build dotted contour
        const auto closedShape = buildDottedContourClosed(s, d, shape, scale);
        gObjectsInPosition.checkShapeElement(myAC->getGUIGlObject(), closedShape);
    } else {
        drawDottedContours(s, d, addOffset, lineWidth);
    }
}


void
GNEContour::drawDottedContourExtruded(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, const double extrusionWidth, const double scale, const bool drawFirstExtrem,
        const bool drawLastExtrem, const double offset, const double lineWidth) const {
    // check if mouse is within two lines (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // first build dotted contour
        const auto extrudedShape = buildDottedContourExtruded(s, d, shape, extrusionWidth, scale, drawFirstExtrem, drawLastExtrem, offset);
        gObjectsInPosition.checkShapeElement(myAC->getGUIGlObject(), extrudedShape);
    } else {
        drawDottedContours(s, d, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const Position& pos, const double width, const double height, const double offsetX, const double offsetY,
        const double rot, const double scale, const double lineWidth) const {
    // check if mouse is within geometry (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // first build dotted contour
        const auto rectangleShape = buildDottedContourRectangle(s, d, pos, width, height, offsetX, offsetY, rot, scale);
        gObjectsInPosition.checkShapeElement(myAC->getGUIGlObject(), rectangleShape);
    } else {
        drawDottedContours(s, d, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const Position& pos, double radius, const double scale, const double lineWidth) const {
    // check if mouse is within geometry (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // build dotted contour
        buildDottedContourCircle(s, d, pos, radius, scale);
        gObjectsInPosition.checkCircleElement(myAC->getGUIGlObject(), pos, (radius * scale));
    } else {
        drawDottedContours(s, d, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, GeometryPoint geometryPoints, double radius, const double scale, const double lineWidth) const {
    // get object
    const auto &glObject = myAC->getGUIGlObject();
    // declare distance
    const auto scaledRadius = (radius * scale);
    // check if we're calculating the mouse position over geometry or drawing dotted geometry
    if (s.drawForObjectUnderCursor) {
        // iterate over all geometry points
        for (int i = 0; i < (int)shape.size(); i++) {
            const bool first = (i == 0); 
            const bool last = ((i + 1) == (int)shape.size());
            // check conditions
            if ((geometryPoints == GeometryPoint::ALL) ||
                (first && (geometryPoints == GeometryPoint::FROM)) ||
                (last && (geometryPoints == GeometryPoint::TO)) ||
                (!first && !last && (geometryPoints == GeometryPoint::MIDDLE))) {
                // check position within geometry
                gObjectsInPosition.checkGeometryPoint(glObject, i, shape[i], scaledRadius);
            }
        }
        // check if mouse is over shape
        if (s.drawForObjectUnderCursor) {
            // check position over shape
            gObjectsInPosition.checkPositionOverShape(glObject, shape, scaledRadius);
        }
    } else if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours)) {
        // get all geometry points
        const auto &geometryPointIndexes = gObjectsInPosition.getGeometryPoints(glObject);
        // either draw geometry point indexes or pos over shape, but not together
        if (geometryPointIndexes.size() > 0) {
            // draw all geometry points
            for (const auto &geometryPointIndex : geometryPointIndexes) {
                // build dotted contour circle
                buildDottedContourCircle(s, d, shape[geometryPointIndex], radius, scale);
                // draw geometry point
                drawDottedContour(s, GUIDottedGeometry::DottedContourType::MOVE, 0, lineWidth);
            }
        } else {
            // check if draw dotted contour over shape
            const auto &posOverShape = gObjectsInPosition.getPositionOverShape(glObject);
            if (posOverShape != Position::INVALID) {
                // build dotted contour circle
                buildDottedContourCircle(s, d, posOverShape, radius, scale);
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
        const auto contourShape = buildDottedContourEdge(s, d, edge, drawFirstExtrem, drawLastExtrem);
        gObjectsInPosition.checkShapeElement(myAC->getGUIGlObject(), contourShape);
    } else {
        drawDottedContours(s, d, true, lineWidth);
    }
}


void
GNEContour::drawDottedContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GNEEdge* fromEdge, const GNEEdge* toEdge, const double lineWidth) const {
    // first build dotted contour (only in rectangle selection mode)
    buildDottedContourEdges(s, d, fromEdge, toEdge);
    // draw dotted contours
    drawDottedContours(s, d, true, lineWidth);
}


void
GNEContour::drawInnenContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                   const PositionVector& shape, const double scale, const double lineWidth) const {
    // declare scaled shape
    PositionVector scaledShape = shape;
    // scale shape
    scaledShape.scaleRelative(scale);
    // close
    scaledShape.closePolygon();
    // calculate geometry without resampling
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, scaledShape, true);
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // draw dotted
    myDottedGeometries->at(0).drawInnenGeometry(lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


PositionVector
GNEContour::buildDottedContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const PositionVector& shape, const double scale) const {
    // declare scaled shape
    PositionVector scaledShape = shape;
    // scale shape
    scaledShape.scaleRelative(scale);
    // close
    scaledShape.closePolygon();
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, scaledShape, true);
    // finally return scaled shape (used for check mouse pos)
    return scaledShape;
}


PositionVector
GNEContour::buildDottedContourExtruded(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                       const PositionVector& shape, const double extrusionWidth, const double scale,
                                       const bool drawFirstExtrem, const bool drawLastExtrem, const double offset) const {
    // create top and bot geometries
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, shape, false);
    myDottedGeometries->at(2) = GUIDottedGeometry(s, d, shape.reverse(), false);
    // move geometries top and bot
    myDottedGeometries->at(0).moveShapeToSide((extrusionWidth * scale * -1) + offset);
    myDottedGeometries->at(2).moveShapeToSide((extrusionWidth * scale * -1) - offset);
    // create left and right geometries
    if (drawFirstExtrem) {
        myDottedGeometries->at(3) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(2).getBackPosition(),
            myDottedGeometries->at(0).getFrontPosition()
        }, false);
    }
    if (drawLastExtrem) {
        myDottedGeometries->at(1) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(0).getBackPosition(),
            myDottedGeometries->at(2).getFrontPosition()
        }, false);
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
GNEContour::buildDottedContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                        const Position& pos, const double width, const double height, const double offsetX,
                                        const double offsetY, const double rot, const double scale) const {
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
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, rectangleShape, true);
    // return rectangle shape used for check position over mouse
    return rectangleShape;
}


PositionVector
GNEContour::buildDottedContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                     const Position& pos, double radius, const double scale) const {
    // get resolution
    int resolution = 1;
    if (d <= GUIVisualizationSettings::Detail::CircleResolution32) {
        resolution = 32;
    } else if (d <= GUIVisualizationSettings::Detail::CircleResolution16) {
        resolution = 16;
    } else if (d <= GUIVisualizationSettings::Detail::CircleResolution8) {
        resolution = 8;
    } else {
        resolution = 4;
    }
    // get vertex circle shape
    const auto circleShape = GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, resolution);
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, circleShape, true);
    // return circle shape used for check position over mouse
    return circleShape;
}


PositionVector
GNEContour::buildDottedContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                   const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // set left hand flag
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    // obtain lanes
    const GNELane* topLane = lefthand ? edge->getLanes().front() : edge->getLanes().back();
    const GNELane* botLane = lefthand ? edge->getLanes().back() : edge->getLanes().front();
    // create top and bot geometries
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, topLane->getLaneGeometry().getShape(), false);
    myDottedGeometries->at(2) = GUIDottedGeometry(s, d, botLane->getLaneGeometry().getShape().reverse(), false);
    // move geometries top and bot
    myDottedGeometries->at(0).moveShapeToSide((topLane->getDrawingConstants()->getDrawingWidth() * -1) + topLane->getDrawingConstants()->getOffset());
    myDottedGeometries->at(2).moveShapeToSide((botLane->getDrawingConstants()->getDrawingWidth() * -1) - botLane->getDrawingConstants()->getOffset());
    // create left and right geometries
    if (drawFirstExtrem) {
        myDottedGeometries->at(3) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(2).getBackPosition(),
            myDottedGeometries->at(0).getFrontPosition()
        }, false);
    }
    if (drawLastExtrem) {
        myDottedGeometries->at(1) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(0).getBackPosition(),
            myDottedGeometries->at(2).getFrontPosition()
        }, false);
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
GNEContour::buildDottedContourEdges(const GUIVisualizationSettings& /*s*/, const GUIVisualizationSettings::Detail /*d*/,
                                    const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */) const {

    /* FINISH */
    return PositionVector();
}


void
GNEContour::drawDottedContours(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const bool addOffset, const double lineWidth) const {
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
