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
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEContour.h"
#include "GNEAttributeCarrier.h"


// ===========================================================================
// static members
// ===========================================================================

GUIDottedGeometry::DottedGeometryColor GNEContour::myDottedGeometryColor;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEContour::GNEContour() :
    myDottedGeometries(new std::vector<GUIDottedGeometry>(4)),
    myContourBoundary(new Boundary) {
}


GNEContour::~GNEContour() {
    delete myDottedGeometries;
    delete myContourBoundary;
}


Boundary
GNEContour::getContourBoundary() const {
    return *myContourBoundary;
}


void
GNEContour::calculateContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const GUIGlObject* glObject, const PositionVector& shape, const double scale) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForObjectUnderCursor) {
        // calculate closed shape contour
        const auto closedShape = buildContourClosedShape(s, d, shape, scale);
        // check if position or bondary is within closed shape
        gViewObjectsHandler.checkShapeElement(d, glObject, closedShape);
    }
}


void
GNEContour::calculateContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                          const GUIGlObject* glObject, const PositionVector& shape, const double extrusionWidth,
                                          const double scale, const bool closeFirstExtrem, const bool closeLastExtrem,
                                          const double offset) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForObjectUnderCursor) {
        // calculate extruded shape
        const auto extrudedShape = buildContourExtrudedShape(s, d, shape, extrusionWidth, scale, closeFirstExtrem, closeLastExtrem, offset);
        // check if position or bondary is within extruded shape
        gViewObjectsHandler.checkShapeElement(d, glObject, extrudedShape);
    }
}


void
GNEContour::calculateContourRectangleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                           const GUIGlObject* glObject, const Position& pos, const double width, const double height,
                                           const double offsetX, const double offsetY, const double rot, const double scale) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForObjectUnderCursor) {
        // calculate rectangle shape
        const auto rectangleShape = buildContourRectangle(s, d, pos, width, height, offsetX, offsetY, rot, scale);
        // check if position or bondary is within rectangle shape
        gViewObjectsHandler.checkShapeElement(d, glObject, rectangleShape);
    }
}


void
GNEContour::calculateContourCircleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const GUIGlObject* glObject, const Position& pos, double radius, const double scale) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForObjectUnderCursor) {
        // calculate circle shape
        buildContourCircle(s, d, pos, radius, scale);
        // check if position or bondary is within circle shape
        gViewObjectsHandler.checkCircleElement(d, glObject, pos, (radius * scale));
    }
}


void
GNEContour::calculateContourGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                           const GUIGlObject* glObject, const PositionVector& shape, GeometryPoint geometryPoints,
                                           double radius, const double scale, const double lineWidth) const {
    return;
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
                gViewObjectsHandler.checkGeometryPoint(d, glObject, i, shape[i], scaledRadius);
            }
        }
        // check if mouse is over shape
        if (s.drawForObjectUnderCursor) {
            // check position over shape
            gViewObjectsHandler.checkPositionOverShape(d, glObject, shape, scaledRadius);
        }
    } else if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours)) {
        // get all geometry points
        const auto &geometryPointIndexes = gViewObjectsHandler.getGeometryPoints(glObject);
        // either draw geometry point indexes or pos over shape, but not together
        if (geometryPointIndexes.size() > 0) {
            // draw all geometry points
            for (const auto &geometryPointIndex : geometryPointIndexes) {
                // build dotted contour circle
                buildContourCircle(s, d, shape[geometryPointIndex], radius, scale);
                // draw geometry point
                drawDottedContour(s, GUIDottedGeometry::DottedContourType::MOVE, lineWidth, 0);
            }
        } else {
            // check if draw dotted contour over shape
            const auto &posOverShape = gViewObjectsHandler.getPositionOverShape(glObject);
            if (posOverShape != Position::INVALID) {
                // build dotted contour circle
                buildContourCircle(s, d, posOverShape, radius, scale);
                // draw geometry point
                drawDottedContour(s, GUIDottedGeometry::DottedContourType::MOVE, lineWidth, 0);
            }
        }
    }
}


void
GNEContour::calculateContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                 const GNEEdge* edge, const bool closeFirstExtrem, const bool closeLastExtrem) const {
    // check if mouse is within two lines (only in rectangle selection mode)
    if (s.drawForObjectUnderCursor) {
        // calculate contour edge shape
        const auto contourShape = buildContourEdge(s, d, edge, closeFirstExtrem, closeLastExtrem);
        // check if position or bondary is within contour shape
        gViewObjectsHandler.checkShapeElement(d, edge, contourShape);
    }
}


void
GNEContour::calculateContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const GNEEdge* fromEdge, const GNEEdge* toEdge) const {
    // calculate contour edges shape
    buildContourEdges(s, d, fromEdge, toEdge);
}


void
GNEContour::drawDottedContours(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const GNEAttributeCarrier *AC, const double lineWidth, const bool addOffset) const {
    // first check if draw dotted contour
    if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours)) {
        // basic contours
        if (AC->checkDrawFromContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::FROM, lineWidth, addOffset);
        }
        if (AC->checkDrawToContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::TO, lineWidth, addOffset);
        }
        if (AC->checkDrawRelatedContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::RELATED, lineWidth, addOffset);
        }
        if (AC->checkDrawOverContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::OVER, lineWidth, addOffset);
        }
        // inspect contour
        if (AC->checkDrawInspectContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::INSPECT, lineWidth, addOffset);
        }
        // front contour
        if (AC->checkDrawFrontContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::FRONT, lineWidth, addOffset);
        }
        // delete contour
        if (AC->checkDrawDeleteContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::REMOVE, lineWidth, addOffset);
        }
        // select contour
        if (AC->checkDrawSelectContour()) {
            drawDottedContour(s, GUIDottedGeometry::DottedContourType::SELECT, lineWidth, addOffset);
        }
    }
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
    // update contour boundary
    updateContourBondary();
    // pop matrix
    GLHelper::popMatrix();
}


PositionVector
GNEContour::buildContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                    const PositionVector& shape, const double scale) const {
    // declare scaled shape
    PositionVector scaledShape = shape;
    // scale shape
    scaledShape.scaleRelative(scale);
    // close
    scaledShape.closePolygon();
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, scaledShape, true);
    // update contour boundary
    updateContourBondary();
    // finally return scaled shape (used for check mouse pos)
    return scaledShape;
}


PositionVector
GNEContour::buildContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                      const PositionVector& shape, const double extrusionWidth, const double scale,
                                      const bool closeFirstExtrem, const bool closeLastExtrem, const double offset) const {
    // create top and bot geometries
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, shape, false);
    myDottedGeometries->at(2) = GUIDottedGeometry(s, d, shape.reverse(), false);
    // move geometries top and bot
    myDottedGeometries->at(0).moveShapeToSide((extrusionWidth * scale * -1) + offset);
    myDottedGeometries->at(2).moveShapeToSide((extrusionWidth * scale * -1) - offset);
    // create left and right geometries
    if (closeFirstExtrem) {
        myDottedGeometries->at(3) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(2).getBackPosition(),
            myDottedGeometries->at(0).getFrontPosition()
        }, false);
    }
    if (closeLastExtrem) {
        myDottedGeometries->at(1) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(0).getBackPosition(),
            myDottedGeometries->at(2).getFrontPosition()
        }, false);
    }
    // update contour boundary
    updateContourBondary();
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
GNEContour::buildContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
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
    // update contour boundary
    updateContourBondary();
    // return rectangle shape used for check position over mouse
    return rectangleShape;
}


PositionVector
GNEContour::buildContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                               const Position& pos, double radius, const double scale) const {
    // get resolution
    PositionVector circleShape;
    // continue depending of resolution
    if (d <= GUIVisualizationSettings::Detail::CircleResolution32) {
        circleShape = GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, 16);
    } else if (d <= GUIVisualizationSettings::Detail::CircleResolution16) {
        circleShape = GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, 8);
    } else {
        circleShape.push_back(Position(pos.x() - radius, pos.y() - radius));
        circleShape.push_back(Position(pos.x() - radius, pos.y() + radius));
        circleShape.push_back(Position(pos.x() + radius, pos.y() + radius));
        circleShape.push_back(Position(pos.x() + radius, pos.y() - radius));
    }
    // calculate dotted geometry
    myDottedGeometries->at(0) = GUIDottedGeometry(s, d, circleShape, true);
    // update contour boundary
    updateContourBondary();
    // return circle shape used for check position over mouse
    return circleShape;
}


PositionVector
GNEContour::buildContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                             const GNEEdge* edge, const bool closeFirstExtrem, const bool closeLastExtrem) const {
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
    if (closeFirstExtrem) {
        myDottedGeometries->at(3) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(2).getBackPosition(),
            myDottedGeometries->at(0).getFrontPosition()
        }, false);
    }
    if (closeLastExtrem) {
        myDottedGeometries->at(1) = GUIDottedGeometry(s, d, {
            myDottedGeometries->at(0).getBackPosition(),
            myDottedGeometries->at(2).getFrontPosition()
        }, false);
    }
    // update contour boundary
    updateContourBondary();
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
GNEContour::buildContourEdges(const GUIVisualizationSettings& /*s*/, const GUIVisualizationSettings::Detail /*d*/,
                              const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */) const {

    /* FINISH */
    return PositionVector();
}


void
GNEContour::updateContourBondary() const {
    myContourBoundary->reset();
    for (const auto &dottedGeometry : *myDottedGeometries) {
        if (dottedGeometry.getUnresampledShape().size() > 0) {
            myContourBoundary->add(dottedGeometry.getUnresampledShape().getBoxBoundary());
        }
    }
}


void
GNEContour::drawDottedContour(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                              const double lineWidth, const bool addOffset) const {
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometries
    for (const auto &dottedGeometry : *myDottedGeometries) {
        dottedGeometry.drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth, addOffset);
    }
    // pop matrix
    GLHelper::popMatrix();
}

/****************************************************************************/
