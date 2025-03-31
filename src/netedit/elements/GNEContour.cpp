/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
    myContourBoundary(new Boundary),
    myCalculatedShape(new PositionVector) {
}


GNEContour::~GNEContour() {
    delete myDottedGeometries;
    delete myContourBoundary;
    delete myCalculatedShape;
}


Boundary
GNEContour::getContourBoundary() const {
    return *myContourBoundary;
}


void
GNEContour::clearContour() const {
    for (int i = 0; i < 3; i++) {
        myDottedGeometries->at(i).clearDottedGeometry();
    }
    myContourBoundary->reset();
    myCalculatedShape->clear();
}


void
GNEContour::calculateContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const GUIGlObject* glObject, const PositionVector& shape, const double layer,
                                        const double scale, const GUIGlObject* boundaryParent, const bool addToSelectedObjects) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && !gViewObjectsHandler.checkRectangleSelection(s, glObject, layer, boundaryParent)) {
        // calculate closed shape contour
        buildContourClosedShape(s, d, shape, scale);
        // check if position or bondary is within closed shape
        if (addToSelectedObjects) {
            gViewObjectsHandler.checkShapeObject(glObject, *myCalculatedShape, *myContourBoundary, layer, nullptr);
        }
    }
}


void
GNEContour::calculateContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GUIGlObject* glObject, const PositionVector& shape, const double layer, const double extrusionWidth,
        const double scale, const bool closeFirstExtrem, const bool closeLastExtrem, const double offset,
        const GNESegment* segment, const GUIGlObject* boundaryParent, const bool addToSelectedObjects) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && !gViewObjectsHandler.checkRectangleSelection(s, glObject, layer, boundaryParent)) {
        // calculate extruded shape
        buildContourExtrudedShape(s, d, shape, extrusionWidth, scale, closeFirstExtrem, closeLastExtrem, offset);
        // check if position or bondary is within extruded shape
        if (addToSelectedObjects) {
            gViewObjectsHandler.checkShapeObject(glObject, *myCalculatedShape, *myContourBoundary, layer, segment);
        }
    }
}


void
GNEContour::calculateContourRectangleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GUIGlObject* glObject, const Position& pos, const double width, const double height, const double layer,
        const double offsetX, const double offsetY, const double rot, const double scale, const GUIGlObject* boundaryParent) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && !gViewObjectsHandler.checkRectangleSelection(s, glObject, layer, boundaryParent)) {
        // calculate rectangle shape
        buildContourRectangle(s, d, pos, width, height, offsetX, offsetY, rot, scale);
        // check if position or bondary is within rectangle shape
        gViewObjectsHandler.checkShapeObject(glObject, *myCalculatedShape, *myContourBoundary, layer, nullptr);
    }
}


void
GNEContour::calculateContourCircleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const GUIGlObject* glObject, const Position& pos, double radius, const double layer,
                                        const double scale, const GUIGlObject* boundaryParent) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && !gViewObjectsHandler.checkRectangleSelection(s, glObject, layer, boundaryParent)) {
        // calculate circle shape
        buildContourCircle(s, d, pos, radius, scale);
        // check if position or bondary is within circle shape
        gViewObjectsHandler.checkCircleObject(d, glObject, pos, (radius * scale), layer);
    }
}


void
GNEContour::calculateContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                 const GNEEdge* edge, const GUIGlObject* elementToRegister, const double layer,
                                 const bool closeFirstExtrem, const bool closeLastExtrem) const {
    // check if mouse is within two lines (only in rectangle selection mode)
    if (s.drawForViewObjectsHandler) {
        // calculate contour edge shape
        buildContourEdge(s, d, edge, closeFirstExtrem, closeLastExtrem);
        // check if position or bondary is within contour shape
        gViewObjectsHandler.checkShapeObject(elementToRegister, *myCalculatedShape, *myContourBoundary, layer, nullptr);
    }
}


void
GNEContour::calculateContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const GNEEdge* fromEdge, const GNEEdge* toEdge) const {
    // calculate contour edges shape
    buildContourEdges(s, d, fromEdge, toEdge);
}


void
GNEContour::calculateContourFirstGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GUIGlObject* glObject, const PositionVector& shape, const double layer, double radius, const double scale) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && (shape.size() > 0)) {
        // check position within geometry of first geometry point
        gViewObjectsHandler.checkGeometryPoint(d, glObject, shape, 0, layer, (radius * scale));
    }
}


void
GNEContour::calculateContourLastGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GUIGlObject* glObject, const PositionVector& shape, const double layer, double radius, const double scale) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && (shape.size() > 0)) {
        // check position within geometry of last geometry point
        gViewObjectsHandler.checkGeometryPoint(d, glObject, shape, (int)shape.size() - 1, layer, (radius * scale));
    }
}


void
GNEContour::calculateContourMiddleGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GUIGlObject* glObject, const PositionVector& shape, const double layer, double radius, const double scale) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler) {
        // check position within geometry of middle geometry points
        for (int i = 1; i < (int)shape.size() - 1; i++) {
            gViewObjectsHandler.checkGeometryPoint(d, glObject, shape, i, layer, (radius * scale));
        }
        // also calculate position over shape
        gViewObjectsHandler.checkPositionOverShape(d, glObject, shape, layer, (radius * scale));
    }
}


void
GNEContour::calculateContourAllGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GUIGlObject* glObject, const PositionVector& shape, const double layer, double radius, const double scale,
        const bool calculatePosOverShape) const {
    // check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler) {
        // check position within geometry of middle geometry points
        for (int i = 0; i < (int)shape.size(); i++) {
            gViewObjectsHandler.checkGeometryPoint(d, glObject, shape, i, layer, (radius * scale));
        }
        // check if calculate position over shape
        if (calculatePosOverShape) {
            gViewObjectsHandler.checkPositionOverShape(d, glObject, shape, layer, (radius * scale));
        }
    }
}


void
GNEContour::calculateContourEdgeGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GNEEdge* edge, const double radius, const bool calculatePosOverShape, const bool firstExtrem, const bool lastExtrem) const {
    // first check if we're in drawForObjectUnderCursor
    if (s.drawForViewObjectsHandler && (gViewObjectsHandler.getSelectionPosition() != Position::INVALID)) {
        // get edge geometry
        const auto& edgeGeometry = edge->getNBEdge()->getGeometry();
        // calculate last geometry point index
        const int lastGeometryPointIndex = (int)edgeGeometry.size() - 1;
        // we have two cases: if cursor is within geometry, or cursor is outher geometry
        if (myCalculatedShape->around(gViewObjectsHandler.getSelectionPosition())) {
            // get squared radius
            const auto squaredRadius = (radius * radius);
            // obtain nearest position and offset over shape
            const auto nearestOffset = edgeGeometry.nearest_offset_to_point2D(gViewObjectsHandler.getSelectionPosition());
            const auto nearestPos = edgeGeometry.positionAtOffset2D(nearestOffset);
            // check position within geometry of middle geometry points
            for (int i = 1; i < lastGeometryPointIndex; i++) {
                if (edgeGeometry[i].distanceSquaredTo2D(nearestPos) <= squaredRadius) {
                    gViewObjectsHandler.checkGeometryPoint(d, edge, edgeGeometry, i, edge->getType(), radius);
                }
            }
            // check extrems
            if (firstExtrem) {
                if (edgeGeometry[0].distanceSquaredTo2D(nearestPos) <= squaredRadius) {
                    gViewObjectsHandler.checkGeometryPoint(d, edge, edgeGeometry, 0, edge->getType(), radius);
                }
            }
            if (lastExtrem) {
                if (edgeGeometry[lastGeometryPointIndex].distanceSquaredTo2D(nearestPos) <= squaredRadius) {
                    gViewObjectsHandler.checkGeometryPoint(d, edge, edgeGeometry, lastGeometryPointIndex, edge->getType(), radius);
                }
            }
            // if list of index is emprt, use nearestPos as pos over shape
            if (calculatePosOverShape && gViewObjectsHandler.getSelectedGeometryPoints(edge).empty()) {
                gViewObjectsHandler.selectPositionOverShape(edge, nearestPos, edge->getType(), nearestOffset);
            }
        } else {
            // check position within geometry of middle geometry points
            for (int i = 1; i < lastGeometryPointIndex; i++) {
                gViewObjectsHandler.checkGeometryPoint(d, edge, edgeGeometry, i, edge->getType(), radius);
            }
            // check extrems
            if (firstExtrem) {
                gViewObjectsHandler.checkGeometryPoint(d, edge, edgeGeometry, 0, edge->getType(), radius);
            }
            if (lastExtrem) {
                gViewObjectsHandler.checkGeometryPoint(d, edge, edgeGeometry, lastGeometryPointIndex, edge->getType(), radius);
            }
        }
    }
}


bool
GNEContour::checkDrawPathContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                 const GNEAttributeCarrier* AC) const {
    // first check if draw dotted contour
    if (s.drawForViewObjectsHandler && !s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours)) {
        // basic contours
        if (AC->checkDrawFromContour()) {
            return true;
        }
        if (AC->checkDrawToContour()) {
            return true;
        }
        if (AC->checkDrawRelatedContour()) {
            return true;
        }
        if (AC->checkDrawOverContour()) {
            return true;
        }
        // inspect contour
        if (AC->checkDrawInspectContour()) {
            return true;
        }
        if (AC->checkDrawInspectContourSmall()) {
            return true;
        }
        // front contour
        if (AC->checkDrawFrontContour()) {
            return true;
        }
        // delete contour
        if (AC->checkDrawDeleteContour()) {
            return true;
        }
        // select contour
        if (AC->checkDrawSelectContour()) {
            return true;
        }
        // move contour
        if (AC->checkDrawMoveContour()) {
            return true;
        }
    }
    // nothing to draw
    return false;
}


bool
GNEContour::drawDottedContours(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const GNEAttributeCarrier* AC, const double lineWidth, const bool addOffset) const {
    // first check if draw dotted contour
    if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours)) {
        // basic contours
        if (AC->checkDrawFromContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::FROM, lineWidth, addOffset);
        }
        if (AC->checkDrawToContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::TO, lineWidth, addOffset);
        }
        if (AC->checkDrawRelatedContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::RELATED, lineWidth, addOffset);
        }
        if (AC->checkDrawOverContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::OVER, lineWidth, addOffset);
        }
        // inspect contour
        if (AC->checkDrawInspectContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::INSPECT, lineWidth, addOffset);
        } else if (AC->checkDrawInspectContourSmall()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::INSPECT, s.dottedContourSettings.segmentWidthSmall, addOffset);
        }
        // front contour
        if (AC->checkDrawFrontContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::FRONT, lineWidth, addOffset);
        }
        // delete contour
        if (AC->checkDrawDeleteContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::REMOVE, lineWidth, addOffset);
        } else if (AC->checkDrawDeleteContourSmall()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::REMOVE, s.dottedContourSettings.segmentWidthSmall, addOffset);
        }
        // select contour
        if (AC->checkDrawSelectContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::SELECT, lineWidth, addOffset);
        }
        // move contour
        if (AC->checkDrawMoveContour()) {
            return drawDottedContour(s, GUIDottedGeometry::DottedContourType::MOVE, lineWidth, addOffset);
        }
        return false;
    }
    return false;
}


void
GNEContour::drawDottedContourGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const GNEAttributeCarrier* AC, const PositionVector& shape, const double radius,
        const double scale, const double lineWidth) const {
    // first check if draw dotted contour
    if (!s.disableDottedContours && (d <= GUIVisualizationSettings::Detail::DottedContours) && AC->checkDrawMoveContour()) {
        // get gl object
        const auto glObject = AC->getGUIGlObject();
        // get geometry points
        const auto& geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(glObject);
        // get temporal position over shape
        const auto& posOverShape = gViewObjectsHandler.getSelectedPositionOverShape(glObject);
        // either draw geometry points or position over shape
        if (geometryPoints.size() > 0) {
            // draw every geometry point
            for (const auto& geometryPoint : geometryPoints) {
                // create circle shape
                const auto circleShape = GUIGeometry::getVertexCircleAroundPosition(shape[geometryPoint], radius * scale, 16);
                // calculate dotted geometry
                const auto dottedGeometry = GUIDottedGeometry(s, d, circleShape, true);
                // reset dotted geometry color
                myDottedGeometryColor.reset();
                // Push draw matrix
                GLHelper::pushMatrix();
                // translate to front
                glTranslated(0, 0, GLO_DOTTEDCONTOUR);
                // draw dotted geometries
                dottedGeometry.drawDottedGeometry(s, GUIDottedGeometry::DottedContourType::MOVE, myDottedGeometryColor, lineWidth, 0);
                // pop matrix
                GLHelper::popMatrix();
            }
        } else if (posOverShape != Position::INVALID) {
            // create circle shape
            const auto circleShape = GUIGeometry::getVertexCircleAroundPosition(posOverShape, radius * scale, 16);
            // calculate dotted geometry
            const auto dottedGeometry = GUIDottedGeometry(s, d, circleShape, true);
            // reset dotted geometry color
            myDottedGeometryColor.reset();
            // Push draw matrix
            GLHelper::pushMatrix();
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR);
            // draw dotted geometries
            dottedGeometry.drawDottedGeometry(s, GUIDottedGeometry::DottedContourType::MOVE, myDottedGeometryColor, lineWidth, 0);
            // pop matrix
            GLHelper::popMatrix();
        }
    }
}


void
GNEContour::drawInnenContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                   const PositionVector& shape, const double scale, const double lineWidth) const {
    // set calculated shape
    *myCalculatedShape = shape;
    // continue only if shape has at least three elements and scale isn't 0
    if ((myCalculatedShape->size() > 2) && (scale > 0)) {
        // scale shape
        myCalculatedShape->scaleRelative(scale);
        // close
        myCalculatedShape->closePolygon();
        // calculate geometry without resampling
        myDottedGeometries->at(0) = GUIDottedGeometry(s, d, *myCalculatedShape, true);
        // reset dotted geometry color
        myDottedGeometryColor.reset();
        // Push draw matrix
        GLHelper::pushMatrix();
        // draw dotted
        myDottedGeometries->at(0).drawInnenGeometry(lineWidth);
        // pop matrix
        GLHelper::popMatrix();
    }
}


bool
GNEContour::drawDottedContour(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                              const double lineWidth, const bool addOffset) const {
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometries
    for (const auto& dottedGeometry : *myDottedGeometries) {
        dottedGeometry.drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth, addOffset);
    }
    // pop matrix
    GLHelper::popMatrix();
    return true;
}


void
GNEContour::buildContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                    const PositionVector& shape, const double scale) const {
    // set calculated shape
    *myCalculatedShape = shape;
    // continue only if shape has at least three elements and scale isn't 0
    if ((myCalculatedShape->size() > 2) && (scale > 0)) {
        // scale shape
        myCalculatedShape->scaleRelative(scale);
        // close
        myCalculatedShape->closePolygon();
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, d, *myCalculatedShape, true);
        // update contour boundary
        *myContourBoundary = myCalculatedShape->getBoxBoundary();
    } else {
        myContourBoundary->reset();
    }
}


void
GNEContour::buildContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                      const PositionVector& shape, const double extrusionWidth, const double scale,
                                      const bool closeFirstExtrem, const bool closeLastExtrem, const double offset) const {
    // reset calculated shape
    myCalculatedShape->clear();
    // avoid empty shapes
    if (shape.size() > 1 && (extrusionWidth > 0)) {
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
        for (const auto& position : myDottedGeometries->at(0).getUnresampledShape()) {
            myCalculatedShape->push_back(position);
        }
        for (const auto& position : myDottedGeometries->at(2).getUnresampledShape()) {
            myCalculatedShape->push_back(position);
        }
        // update contour boundary
        *myContourBoundary = myCalculatedShape->getBoxBoundary();
    } else {
        myContourBoundary->reset();
    }
}


void
GNEContour::buildContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const Position& pos, const double width, const double height, const double offsetX,
                                  const double offsetY, const double rot, const double scale) const {
    // reset calculated shape
    myCalculatedShape->clear();
    // check sizes
    if (((width + height) > 0) && (scale > 0)) {
        // make rectangle
        myCalculatedShape->push_back(Position(0 + width, 0 + height));
        myCalculatedShape->push_back(Position(0 + width, 0 - height));
        myCalculatedShape->push_back(Position(0 - width, 0 - height));
        myCalculatedShape->push_back(Position(0 - width, 0 + height));
        // move shape
        myCalculatedShape->add(offsetX, offsetY, 0);
        // scale
        myCalculatedShape->scaleRelative(scale);
        // rotate shape
        myCalculatedShape->rotate2D(DEG2RAD((rot * -1) + 90));
        // move to position
        myCalculatedShape->add(pos);
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, d, *myCalculatedShape, true);
        // update contour boundary
        *myContourBoundary = myCalculatedShape->getBoxBoundary();
    } else {
        myContourBoundary->reset();
    }
}


void
GNEContour::buildContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const Position& pos, double radius, const double scale) const {
    // reset calculated shape
    myCalculatedShape->clear();
    // get scaled radius
    const double scaledRadius = radius * scale;
    // check scaled radius
    if (scaledRadius > POSITION_EPS) {
        // continue depending of resolution
        if (d <= GUIVisualizationSettings::Detail::CircleResolution32) {
            *myCalculatedShape = GUIGeometry::getVertexCircleAroundPosition(pos, scaledRadius, 16);
        } else if (d <= GUIVisualizationSettings::Detail::CircleResolution16) {
            *myCalculatedShape = GUIGeometry::getVertexCircleAroundPosition(pos, scaledRadius, 8);
        } else {
            myCalculatedShape->push_back(Position(pos.x() - radius, pos.y() - radius));
            myCalculatedShape->push_back(Position(pos.x() - radius, pos.y() + radius));
            myCalculatedShape->push_back(Position(pos.x() + radius, pos.y() + radius));
            myCalculatedShape->push_back(Position(pos.x() + radius, pos.y() - radius));
        }
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, d, *myCalculatedShape, true);
        // update contour boundary
        *myContourBoundary = myCalculatedShape->getBoxBoundary();
    } else {
        myContourBoundary->reset();
    }
}


void
GNEContour::buildContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                             const GNEEdge* edge, const bool closeFirstExtrem, const bool closeLastExtrem) const {
    // reset calculated shape
    myCalculatedShape->clear();
    // set left hand flag
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    // obtain lanes
    const GNELane* topLane = lefthand ? edge->getChildLanes().front() : edge->getChildLanes().back();
    const GNELane* botLane = lefthand ? edge->getChildLanes().back() : edge->getChildLanes().front();
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
    // update calculated shape
    for (const auto& position : myDottedGeometries->at(0).getUnresampledShape()) {
        myCalculatedShape->push_back(position);
    }
    for (const auto& position : myDottedGeometries->at(2).getUnresampledShape()) {
        myCalculatedShape->push_back(position);
    }
    // update contour boundary
    *myContourBoundary = myCalculatedShape->getBoxBoundary();
}


void
GNEContour::buildContourEdges(const GUIVisualizationSettings& /*s*/, const GUIVisualizationSettings::Detail /*d*/,
                              const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */) const {

    // finish
}

/****************************************************************************/
