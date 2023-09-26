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
    myCachedShape(new PositionVector()),
    myCachedScale(new double(0)),
    myDottedGeometryColor(new GUIDottedGeometry::DottedGeometryColor()),
    myDottedGeometryA(new GUIDottedGeometry()),
    myDottedGeometryB(new GUIDottedGeometry()),
    myDottedGeometryC(new GUIDottedGeometry()) {
}


GNEContourElement::~GNEContourElement() {
    delete myCachedShape;
    delete myCachedScale;
    delete myDottedGeometryColor;
    delete myDottedGeometryA;
    delete myDottedGeometryB;
    delete myDottedGeometryC;
}


void
GNEContourElement::drawDottedContour(const GNENet *net, const PositionVector &shape, double width, double scale) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FROM, shape, width, scale);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::TO, shape, width, scale);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::RELATED, shape, width, scale);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::OVER, shape, width, scale);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(myAC)) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, width, scale);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == myAC) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, width, scale);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(myAC)) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, width, scale);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(myAC)) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, width, scale);
    }
}


void
GNEContourElement::drawDottedContour(const GNENet *net, const PositionVector &shape, double width, double scale,
                                     const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FROM, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::TO, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::RELATED, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::OVER, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(myAC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == myAC) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FRONT, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(myAC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(myAC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::SELECT, shape, width,
                                           scale, drawFirstExtrem, drawLastExtrem);
    }
}


void
GNEContourElement::drawDottedContour(const GNENet *net, const Position &pos, double width, double height,
                                     double offsetX, double offsetY, double rot, double scale, GNEAdditional* parent) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // check if we're handling symbol
    GNEAttributeCarrier* AC = parent? parent : myAC;
    // basic contours
    if (myAC->checkDrawFromContour()) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::FROM, pos, width, height, offsetX, offsetY, rot, scale);
    }
    if (myAC->checkDrawToContour()) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::TO, pos, width, height, offsetX, offsetY, rot, scale);
    }
    if (myAC->checkDrawRelatedContour()) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::RELATED, pos, width, height, offsetX, offsetY, rot, scale);
    }
    if (myAC->checkDrawOverContour()) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::OVER, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(AC)) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::INSPECT, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // front contour
    if (net->getViewNet()->getFrontAttributeCarrier() == AC) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::FRONT, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // delete contour
    if (net->getViewNet()->checkDrawDeleteContour(AC)) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::REMOVE, pos, width, height, offsetX, offsetY, rot, scale);
    }
    // select contour
    if (net->getViewNet()->checkDrawSelectContour(AC)) {
        buildAndDrawDottedContourSquared(s, GUIDottedGeometry::DottedContourType::SELECT, pos, width, height, offsetX, offsetY, rot, scale);
    }
}


void
GNEContourElement::drawDottedContour(const GNENet *net, const Position &pos, double radius, double scale, GNEAttributeCarrier* parent) const {
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
GNEContourElement::drawDottedContour(const GNEEdge* edge) const {
    // get visualization settings
    const auto &s = edge->getNet()->getViewNet()->getVisualisationSettings();
    // continue 
    if (edge->getLanes().size() == 1) {
        // get lane constants
        GNELane::LaneDrawingConstants laneDrawingConstants(s, edge->getLanes().front());
        // draw dottes contours
        drawDottedContour(edge->getNet(), edge->getLanes().front()->getLaneShape(),
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
        drawDottedContour(edge->getNet(), edgeShape, 1, 1, true, true);
    }
}


void
GNEContourElement::buildAndDrawDottedContourShape(const GUIVisualizationSettings& s, const GUIDottedGeometry::DottedContourType type,
                                                      const PositionVector &shape, double width, double scale) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // check if dotted geometry has to be updated
        if ((*myCachedShape != shape) || (*myCachedScale != scale)) {
            // declare scaled shape
            PositionVector scaledShape = shape;
            // scale shape
            scaledShape.scaleRelative(scale);
            // calculate dotted geometry
            *myDottedGeometryA = GUIDottedGeometry(s, scaledShape, true);
            // finally update cached shape
            *myCachedShape = shape;
            *myCachedScale = scale;
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
GNEContourElement::buildAndDrawDottedContourRectangle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                      const PositionVector &shape, double width, double scale,
                                                      const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // check if dotted geometry has to be updated
    if (*myCachedShape != shape) {
        // calculate center dotted geometry
        GUIDottedGeometry dottedGeometry(s, shape, false);
        // make a copy of dotted geometry
        *myDottedGeometryA = dottedGeometry;
        *myDottedGeometryB = dottedGeometry;
        // move geometries top and bot
        myDottedGeometryA->moveShapeToSide(width * scale);
        myDottedGeometryB->moveShapeToSide(width * scale * -1);
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
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, width < 0.5);
    // reset color
    myDottedGeometryColor->reset();
    // draw top dotted geometry
    myDottedGeometryB->drawDottedGeometry(s, type, myDottedGeometryColor, width < 0.5);
    // change color
    myDottedGeometryColor->changeColor();
    // draw extrem dotted geometry
    myDottedGeometryC->drawDottedGeometry(s, type, myDottedGeometryColor, width < 0.5);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourSquared(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                    const Position &pos, double width, double height,
                                                    double offsetX, double offsetY, double rot, double scale) const {
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
    // draw dotted contour rectangle
    buildAndDrawDottedContourShape(s, type, shape, 1, scale);
}


void
GNEContourElement::buildAndDrawDottedContourCircle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                   const Position &pos, double radius, double scale) const {
    // continue depending of radius and scale
    if ((radius * scale) < 2) {
        buildAndDrawDottedContourShape(s, type, GUIGeometry::getVertexCircleAroundPosition(pos, radius, 8), radius, scale);
    } else {
        buildAndDrawDottedContourShape(s, type, GUIGeometry::getVertexCircleAroundPosition(pos, radius, 16), radius, scale);
    }
}

/****************************************************************************/
