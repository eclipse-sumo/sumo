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
    myCachedScale(new double(0)),
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
    delete myCachedScale;
    delete myDottedGeometryColor;
    delete myDottedGeometryA;
    delete myDottedGeometryB;
    delete myDottedGeometryC;
}


void
GNEContourElement::drawDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector &shape,
                                           const double scale, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::FROM, shape, scale, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::TO, shape, scale, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::RELATED, shape, scale, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::OVER, shape, scale, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, scale, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::FRONT, shape, scale, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, scale, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::SELECT, shape, scale, lineWidth);
        }
    }
}


void
GNEContourElement::drawDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector &shape,
                                             const double extrusionWidth, const double scale, const bool drawFirstExtrem,
                                             const bool drawLastExtrem, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::FROM, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::TO, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::RELATED, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::OVER, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::FRONT, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourExtruded(s, GUIDottedGeometry::DottedContourType::SELECT, shape, extrusionWidth,
                                              scale, drawFirstExtrem, drawLastExtrem, lineWidth);
        }
    }
}


void
GNEContourElement::drawDottedContourRectangle(const GUIVisualizationSettings& s, const Position &pos, const double width,
                                              const double height, const double offsetX, const double offsetY, const double rot,
                                              const double scale, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FROM, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::TO, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::RELATED, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::OVER, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::INSPECT, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FRONT, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::REMOVE, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::SELECT, pos,
                                               width, height, offsetX, offsetY, rot, scale, lineWidth);
        }
    }
}


void
GNEContourElement::drawDottedContourCircle(const GUIVisualizationSettings& s, const Position &pos, double radius,
                                           const double scale, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::FROM, pos, radius, scale, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::TO, pos, radius, scale, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::RELATED, pos, radius, scale, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::OVER, pos, radius, scale, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::INSPECT, pos, radius, scale, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::FRONT, pos, radius, scale, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::REMOVE, pos, radius, scale, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourCircle(s, GUIDottedGeometry::DottedContourType::SELECT, pos, radius, scale, lineWidth);
        }
    }
}


void
GNEContourElement::drawDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem,
                                         const bool drawLastExtrem, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(1)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::FROM, edge,
                                         drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::TO, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::RELATED, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::OVER, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::INSPECT, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::FRONT, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::REMOVE, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::SELECT, edge,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
        }
    }
}


void
GNEContourElement::buildAndDrawDottedContourClosed(const GUIVisualizationSettings& s, const GUIDottedGeometry::DottedContourType type,
                                                   const PositionVector &shape, const double scale, const double lineWidth) const {
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
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourExtruded(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                     const PositionVector &shape, const double extrusionWidth, const double scale,
                                                     const bool drawFirstExtrem, const bool drawLastExtrem, const double lineWidth) const {
    // check if dotted geometry has to be updated
    if ((*myCachedShape != shape)  || (*myCachedScale != scale)) {
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
        // update scale
        *myCachedScale = scale;
    }
    // reset dotted geometry color
    myDottedGeometryColor->reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
    // reset color
    myDottedGeometryColor->reset();
    // draw top dotted geometry
    myDottedGeometryB->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
    // change color
    myDottedGeometryColor->changeColor();
    // draw extrem dotted geometry
    myDottedGeometryC->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourRectangle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                      const Position &pos, const double width, const double height,
                                                      const double offsetX, const double offsetY, const double rot,
                                                      const double scale, const double lineWidth) const {
    // continue depending of cached positiosn
    if ((*myCachedPosition != pos) || (*myCachedWidth != width) || (*myCachedHeight != height) || (*myCachedRot != rot) || (*myCachedScale != scale)) {
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
        *myCachedScale = scale;
    }
    // reset dotted geometry color
    myDottedGeometryColor->reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourCircle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                   const Position &pos, double radius, const double scale, const double lineWidth) const {
    // continue depending of radius and scale
    if ((*myCachedPosition != pos) || (*myCachedScale != (radius * scale))) {
        // calculate dotted geometry
        *myDottedGeometryA = GUIDottedGeometry(s, GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, (radius * scale) < 2? 8 : 16), true);
        // update cached position
        *myCachedPosition = pos;
        // update cached scale
        *myCachedScale = radius * scale;
    }
    // draw cached shape
    myDottedGeometryColor->reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContourElement::buildAndDrawDottedContourEdge(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                                                 const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem,
                                                 const double lineWidth) const {
    // continue depending of lanes
    if (edge->getLanes().size() == 1) {
        // get lane constants
        GNELane::LaneDrawingConstants laneDrawingConstants(s, edge->getLanes().front());
        // draw dottes contours
        buildAndDrawDottedContourExtruded(s, type, edge->getLanes().front()->getLaneShape(),
                                          laneDrawingConstants.halfWidth, laneDrawingConstants.exaggeration,
                                          drawFirstExtrem, drawLastExtrem, lineWidth);
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
            *myDottedGeometryC = GUIDottedGeometry(s, *myDottedGeometryA, drawFirstExtrem, *myDottedGeometryB, drawLastExtrem);
            // update cached shape
            *myCachedShape = edgeShape;
        }
        // reset dotted geometry color
        myDottedGeometryColor->reset();
        // Push draw matrix
        GLHelper::pushMatrix();
        // translate to front
        glTranslated(0, 0, GLO_DOTTEDCONTOUR);
        // draw top dotted geometry
        myDottedGeometryA->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
        // reset color
        myDottedGeometryColor->reset();
        // draw top dotted geometry
        myDottedGeometryB->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
        // change color
        myDottedGeometryColor->changeColor();
        // draw extrem dotted geometry
        myDottedGeometryC->drawDottedGeometry(s, type, myDottedGeometryColor, lineWidth);
        // pop matrix
        GLHelper::popMatrix();
    }
}

/****************************************************************************/
