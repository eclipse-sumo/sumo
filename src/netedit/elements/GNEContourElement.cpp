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
/// @file    GNEContourElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// A abstract class contour elements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/options/OptionsCont.h>

#include "GNEContourElement.h"
#include "GNEAttributeCarrier.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEContourElement::GNEContourElement(GNEAttributeCarrier* AC) :
    myAC(AC),
    myCachedPosition(new Position()),
    myCachedShape(new PositionVector()),
    myCachedWidth(new double(0)),
    myCachedHeight(new double(0)),
    myCachedRot(new double(0)),
    myDottedGeometryColor(new GUIDottedGeometry::DottedGeometryColor()),
    myDottedGeometryA(new GUIDottedGeometry()),
    myDottedGeometryB(new GUIDottedGeometry()),
    myDottedGeometryC(new GUIDottedGeometry()) {
}


GNEContourElement::~GNEContourElement() {
    delete myCachedPosition;
    delete myCachedShape;
    delete myCachedWidth;
    delete myCachedHeight;
    delete myCachedRot;
    delete myDottedGeometryColor;
    delete myDottedGeometryA;
    delete myDottedGeometryB;
    delete myDottedGeometryC;
}


void
GNEContourElement::drawDottedContourClosed(const GNENet *net, const PositionVector &shape, const double width, const double scale) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::FROM, shape, width, scale);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::TO, shape, width, scale);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::RELATED, shape, width, scale);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::OVER, shape, width, scale);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(myAC)) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, width, scale);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == myAC) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::FRONT, shape, width, scale);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(myAC)) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, width, scale);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(myAC)) {
        buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::SELECT, shape, width, scale);
    }
}


void
GNEContourElement::drawDottedContourExtruded(const GNENet *net, const PositionVector &shape, const double extrusionWidth, const double scale,
                                             const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::FROM, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::TO, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::RELATED, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::OVER, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(myAC)) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == myAC) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::FRONT, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(myAC)) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(myAC)) {
        buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::SELECT, shape, extrusionWidth,
                                          scale, drawFirstExtrem, drawLastExtrem);
    }
}


void
GNEContourElement::drawDottedContourRectangle(const GNENet *net, const Position &pos, const double width, const double height,
                                              const double offsetX, const double offsetY, const double rot, const double scale,
                                              GNEAdditional* parent) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // check if we're handling symbol
    GNEAttributeCarrier* AC = parent? parent : myAC;
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FROM, pos, width, height, offsetX, offsetY, rot, scale);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::TO, pos, width, height, offsetX, offsetY, rot, scale);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::RELATED, pos, width, height, offsetX, offsetY, rot, scale);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::OVER, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(AC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::INSPECT, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == AC) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FRONT, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(AC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::REMOVE, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(AC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::SELECT, pos, width, height, offsetX, offsetY, rot, scale);
    }
}


void
GNEContourElement::drawDottedContourCircle(const GNENet *net, const Position &pos, double radius, const double scale, GNEAttributeCarrier* parent) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // check if we're handling symbol
    GNEAttributeCarrier* AC = parent? parent : myAC;
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::FROM, pos, radius, scale);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::TO, pos, radius, scale);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::RELATED, pos, radius, scale);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::OVER, pos, radius, scale);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(AC)) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::INSPECT, pos, radius, scale);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == AC) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::FRONT, pos, radius, scale);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(AC)) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::REMOVE, pos, radius, scale);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(AC)) {
        buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::SELECT, pos, radius, scale);
    }
}


void
GNEContourElement::drawDottedContourEdge(const GNEEdge* edge) const {
    // get visualization settings
    const auto &s = edge->getNet()->getViewNet()->getVisualisationSettings();
    // continue 
    if (edge->getLanes().size() == 1) {
        // get lane constants
        GNELane::LaneDrawingConstants laneDrawingConstants(s, edge->getLanes().front());
        // draw dottes contours
        drawDottedContourExtruded(edge->getNet(), edge->getLanes().front()->getLaneShape(),
                                  laneDrawingConstants.halfWidth, laneDrawingConstants.exaggeration, true, true);
    } else {
        // set left hand flag
        const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
        // obtain lanes
        const GNELane* topLane = lefthand ? edge->getLanes().back() : edge->getLanes().front();
        const GNELane* botLane = lefthand ? edge->getLanes().front() : edge->getLanes().back();
        // create a temporal shape
        PositionVector edgeShape = topLane->getLaneGeometry().getShape();
        edgeShape.append(botLane->getLaneGeometry().getShape());
        // check if recalculate dotted geometries
        if (*myCachedShape != edgeShape) {
            // obtain a copy of both geometries
            *myDottedGeometryA = GUIDottedGeometry(s, topLane->getLaneGeometry().getShape(), false);
            *myDottedGeometryB = GUIDottedGeometry(s, botLane->getLaneGeometry().getShape(), false);
            // obtain both LaneDrawingConstants
            GNELane::LaneDrawingConstants laneDrawingConstantsFront(s, topLane);
            GNELane::LaneDrawingConstants laneDrawingConstantsBack(s, botLane);
            // move shapes to side
            myDottedGeometryA->moveShapeToSide(laneDrawingConstantsFront.halfWidth * laneDrawingConstantsFront.exaggeration);
            myDottedGeometryB->moveShapeToSide(laneDrawingConstantsBack.halfWidth * laneDrawingConstantsBack.exaggeration * -1);
            // invert offset of top dotted geometry
            myDottedGeometryA->invertOffset();
            // calculate extremes
            *myDottedGeometryC = GUIDottedGeometry(s, *myDottedGeometryA, true, *myDottedGeometryB, true);
            // update cached shape
            *myCachedShape = edgeShape;
        }
        // draw dotted geometries
        drawDottedContourExtruded(edge->getNet(), edgeShape, 1, 1, true, true);
    }
}


void
GNEContourElement::buildAndDrawDottedContourClosed(const GUIVisualizationSettings& s, const GUIDottedGeometry::DottedContourType type,
                                                   const PositionVector &shape, const double width, const double scale) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // check if dotted geometry has to be updated
        if ((*myCachedShape != shape) || (*myCachedRot != scale)) {
            // declare scaled shape
            PositionVector scaledShape = shape;
            // scale shape
            scaledShape.scaleRelative(scale);
            // calculate dotted geometry
            *myDottedGeometryA = GUIDottedGeometry(s, scaledShape, true);
            // finally update cached shape
            *myCachedShape = shape;
            *myCachedRot = scale;
        }
        // reset dotted geometry color
        myDottedGeometryColor->reset();
        // Push draw matrix
        GLHelper::pushMatrix();
        // translate to front
        glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
        // draw top dotted geometry
        myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, width < 0.5);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEContourElement::buildAndDrawDottedContourExtruded(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                     const PositionVector &shape, const double extrusionWidth, const double scale,
                                                     const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // check if dotted geometry has to be updated
    if (*myCachedShape != shape) {
        // calculate center dotted geometry
        GUIDottedGeometry dottedGeometry(s, shape, false);
        // make a copy of dotted geometry
        *myDottedGeometryA = dottedGeometry;
        *myDottedGeometryB = dottedGeometry;
        // move geometries top and bot
        myDottedGeometryA->moveShapeToSide(extrusionWidth * scale);
        myDottedGeometryB->moveShapeToSide(extrusionWidth * scale * -1);
        // invert offset of top dotted geometry
        myDottedGeometryA->invertOffset();
        // calculate extremes
        *myDottedGeometryC = GUIDottedGeometry(s, *myDottedGeometryA, drawFirstExtrem, *myDottedGeometryB, drawLastExtrem);
        // finally update cached shape
        *myCachedShape = shape;
    }
    // reset dotted geometry color
    myDottedGeometryColor->reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, extrusionWidth < 0.5);
    // reset color
    myDottedGeometryColor->reset();
    // draw top dotted geometry
    myDottedGeometryB->drawDottedGeometry(s, type, myDottedGeometryColor, extrusionWidth < 0.5);
    // change color
    myDottedGeometryColor->changeColor();
    // draw extrem dotted geometry
    myDottedGeometryC->drawDottedGeometry(s, type, myDottedGeometryColor, extrusionWidth < 0.5);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourRectangle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                      const Position &pos, const double width, const double height,
                                                      const double offsetX, const double offsetY, const double rot, const double scale) const {
    // continue depending of cached positiosn
    if ((*myCachedPosition != pos) || (*myCachedWidth != width) || (*myCachedHeight != height) || (*myCachedRot != rot)) {
        // create shape
        PositionVector rectangleShape;
        // make rectangle
        rectangleShape.push_back(Position(0 + width, 0 + height));
        rectangleShape.push_back(Position(0 + width, 0 - height));
        rectangleShape.push_back(Position(0 - width, 0 - height));
        rectangleShape.push_back(Position(0 - width, 0 + height));
        // move shape
        rectangleShape.add(offsetX, offsetY, 0);
        // rotate shape
        rectangleShape.rotate2D(DEG2RAD((rot * -1) + 90));
        // move to position
        rectangleShape.add(pos);
        // calculate dotted geometry
        *myDottedGeometryA = GUIDottedGeometry(s, rectangleShape, true);
        // update cached position
        *myCachedPosition = pos;
        // updated values
        *myCachedWidth = width;
        *myCachedHeight = height;
        *myCachedRot = rot;
    }
    // reset dotted geometry color
    myDottedGeometryColor->reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, width < 0.5);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourCircle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                   const Position &pos, double radius, const double scale) const {
    // continue depending of radius and scale
    if ((*myCachedPosition != pos) || (*myCachedRot != (radius * scale))) {
        // calculate dotted geometry
        *myDottedGeometryA = GUIDottedGeometry(s, GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, (radius * scale) < 2? 8 : 16), true);
        // update cached position
        *myCachedPosition = pos;
        // update cached rot
        *myCachedRot = radius * scale;
    }
    // draw cached shape
    myDottedGeometryColor->reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, false);
    // pop matrix
    GLHelper::popMatrix();
}

/****************************************************************************/
