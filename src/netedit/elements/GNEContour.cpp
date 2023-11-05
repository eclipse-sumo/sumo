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
    myCachedPosition(new Position()),
    myCachedShape(new PositionVector()),
    myDottedGeometries(new std::vector<GUIDottedGeometry>()),
    myCachedDoubles(new std::vector<double>()) {
}


GNEContour::~GNEContour() {
    delete myCachedPosition;
    delete myCachedShape;
    delete myDottedGeometries;
    delete myCachedDoubles;
}


void
GNEContour::reset() {
    myCachedPosition->set(Position::INVALID);
    myCachedShape->clear();
    myDottedGeometries->clear();
    myCachedDoubles->clear();
}


void
GNEContour::drawDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                    const double scale, const bool addOffset, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::FROM, shape, scale, addOffset, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::TO, shape, scale, addOffset, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::RELATED, shape, scale, addOffset, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::OVER, shape, scale, addOffset, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, scale, addOffset, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::FRONT, shape, scale, addOffset, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, scale, addOffset, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourClosed(s, GUIDottedGeometry::DottedContourType::SELECT, shape, scale, addOffset, lineWidth);
        }
    }
}


void
GNEContour::drawDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape,
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
GNEContour::drawDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width,
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
GNEContour::drawDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius,
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
GNEContour::drawDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem,
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
GNEContour::drawDottedContourEdges(const GUIVisualizationSettings& s, const GNEEdge* fromEdge, const GNEEdge* toEdge,
                                   const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(1)) {
        // basic contours
        if (myAC->checkDrawFromContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::FROM, fromEdge, toEdge, lineWidth);
        }
        if (myAC->checkDrawToContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::TO, fromEdge, toEdge, lineWidth);
        }
        if (myAC->checkDrawRelatedContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::RELATED, fromEdge, toEdge, lineWidth);
        }
        if (myAC->checkDrawOverContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::OVER, fromEdge, toEdge, lineWidth);
        }
        // inspect contour
        if (myAC->checkDrawInspectContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::INSPECT, fromEdge, toEdge, lineWidth);
        }
        // front contour
        if (myAC->checkDrawFrontContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::FRONT, fromEdge, toEdge, lineWidth);
        }
        // delete contour
        if (myAC->checkDrawDeleteContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::REMOVE, fromEdge, toEdge, lineWidth);
        }
        // select contour
        if (myAC->checkDrawSelectContour()) {
            buildAndDrawDottedContourEdges(s, GUIDottedGeometry::DottedContourType::SELECT, fromEdge, toEdge, lineWidth);
        }
    }
}


void
GNEContour::drawInnenContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                   const double scale, const double lineWidth) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->empty() || myCachedDoubles->empty()) {
        myDottedGeometries->push_back(GUIDottedGeometry());
        myCachedDoubles->push_back(double(0));
    }
    // check if dotted geometry has to be updated
    if ((*myCachedShape != shape) || (myCachedDoubles->at(0) != scale)) {
        // declare scaled shape
        PositionVector scaledShape = shape;
        // scale shape
        scaledShape.scaleRelative(scale);
        // close
        scaledShape.closePolygon();
        // calculate geometry without resampling
        myDottedGeometries->at(0) = GUIDottedGeometry(s, scaledShape, true, false);
        // finally update cached shape
        *myCachedShape = shape;
        myCachedDoubles->at(0) = scale;
    }
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
GNEContour::buildAndDrawDottedContourClosed(const GUIVisualizationSettings& s, const GUIDottedGeometry::DottedContourType type,
        const PositionVector& shape, const double scale, const bool addOffset, const double lineWidth) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->empty() || myCachedDoubles->empty()) {
        myDottedGeometries->push_back(GUIDottedGeometry());
        myCachedDoubles->push_back(double(0));
    }
    // check if dotted geometry has to be updated
    if ((*myCachedShape != shape) || (myCachedDoubles->at(0) != scale)) {
        // declare scaled shape
        PositionVector scaledShape = shape;
        // scale shape
        scaledShape.scaleRelative(scale);
        // close
        scaledShape.closePolygon();
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, scaledShape, true, true);
        // finally update cached shape
        *myCachedShape = shape;
        myCachedDoubles->at(0) = scale;
    }
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometry
    myDottedGeometries->at(0).drawDottedGeometry(s, type, myDottedGeometryColor, addOffset, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContour::buildAndDrawDottedContourExtruded(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
        const PositionVector& shape, const double extrusionWidth, const double scale,
        const bool drawFirstExtrem, const bool drawLastExtrem, const double lineWidth) const {
    // first change size of myDottedGeometries
    if ((myDottedGeometries->size() != 4) || myCachedDoubles->empty()) {
        myDottedGeometries->clear();
        for (int i = 0; i < 4; i++) {
            myDottedGeometries->push_back(GUIDottedGeometry());
        }
        myCachedDoubles->push_back(double(0));
    }
    // check if dotted geometry has to be updated
    if ((*myCachedShape != shape) || (myCachedDoubles->at(0) != scale)) {
        // create top and bot geometries
        myDottedGeometries->at(0) = GUIDottedGeometry(s, shape, false, true);
        myDottedGeometries->at(2) = GUIDottedGeometry(s, shape.reverse(), false, true);
        // move geometries top and bot
        myDottedGeometries->at(0).moveShapeToSide(extrusionWidth * scale * -1);
        myDottedGeometries->at(2).moveShapeToSide(extrusionWidth * scale * -1);
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
        // finally update cached shape
        *myCachedShape = shape;
        // update scale
        myCachedDoubles->at(0) = scale;
    }
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometry top
    myDottedGeometries->at(0).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // draw dotted geometry right
    myDottedGeometries->at(1).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // draw dotted geometry bot
    myDottedGeometries->at(2).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // draw dotted geometry left
    myDottedGeometries->at(3).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContour::buildAndDrawDottedContourRectangle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
        const Position& pos, const double width, const double height,
        const double offsetX, const double offsetY, const double rot,
        const double scale, const double lineWidth) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->empty() || (myCachedDoubles->size() != 4)) {
        myDottedGeometries->push_back(GUIDottedGeometry());
        myCachedDoubles->clear();
        for (int i = 0; i < 4; i++) {
            myCachedDoubles->push_back(double(0));
        }
    }
    // continue depending of cached positiosn
    if ((*myCachedPosition != pos) || (myCachedDoubles->at(0) != width) ||
            (myCachedDoubles->at(1) != height) || (myCachedDoubles->at(2) != rot) || (myCachedDoubles->at(3) != scale)) {
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
        // update cached position
        *myCachedPosition = pos;
        // updated double values
        myCachedDoubles->at(0) = width;
        myCachedDoubles->at(1) = height;
        myCachedDoubles->at(2) = rot;
        myCachedDoubles->at(3) = scale;
    }
    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometry
    myDottedGeometries->at(0).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContour::buildAndDrawDottedContourCircle(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
        const Position& pos, double radius, const double scale, const double lineWidth) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->empty() || myCachedDoubles->empty()) {
        myDottedGeometries->push_back(GUIDottedGeometry());
        myCachedDoubles->push_back(double(0));
    }
    // continue depending of radius and scale
    if ((*myCachedPosition != pos) || (myCachedDoubles->at(0) != (radius * scale))) {
        // get vertex circle shape
        const auto circleShape = GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, (radius * scale) < 2 ? 8 : 16);
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, circleShape, true, true);
        // update cached position
        *myCachedPosition = pos;
        // update cached scale
        myCachedDoubles->at(0) = radius * scale;
    }
    // draw cached shape
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometry
    myDottedGeometries->at(0).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEContour::buildAndDrawDottedContourEdge(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
        const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem,
        const double lineWidth) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->size() != 4) {
        myDottedGeometries->clear();
        for (int i = 0; i < 4; i++) {
            myDottedGeometries->push_back(GUIDottedGeometry());
        }
    }
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
        const GNELane* topLane = lefthand ? edge->getLanes().front() : edge->getLanes().back();
        const GNELane* botLane = lefthand ? edge->getLanes().back() : edge->getLanes().front();
        // create a temporal shape
        PositionVector edgeShape = topLane->getLaneGeometry().getShape();
        edgeShape.append(botLane->getLaneGeometry().getShape());
        // check if recalculate dotted geometries
        if (*myCachedShape != edgeShape) {
            // obtain both LaneDrawingConstants
            GNELane::LaneDrawingConstants laneDrawingConstantsFront(s, topLane);
            GNELane::LaneDrawingConstants laneDrawingConstantsBack(s, botLane);
            // create top and bot geometries
            myDottedGeometries->at(0) = GUIDottedGeometry(s, topLane->getLaneGeometry().getShape(), false, true);
            myDottedGeometries->at(2) = GUIDottedGeometry(s, botLane->getLaneGeometry().getShape().reverse(), false, true);
            // move geometries top and bot
            myDottedGeometries->at(0).moveShapeToSide(laneDrawingConstantsFront.halfWidth * laneDrawingConstantsFront.exaggeration * -1);
            myDottedGeometries->at(2).moveShapeToSide(laneDrawingConstantsBack.halfWidth * laneDrawingConstantsBack.exaggeration * -1);
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
            // update cached shape
            *myCachedShape = edgeShape;
        }
        // reset dotted geometry color
        myDottedGeometryColor.reset();
        // Push draw matrix
        GLHelper::pushMatrix();
        // translate to front
        glTranslated(0, 0, GLO_DOTTEDCONTOUR);
        // draw dotted geometry top
        myDottedGeometries->at(0).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
        // draw dotted geometry right
        myDottedGeometries->at(1).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
        // draw dotted geometry bot
        myDottedGeometries->at(2).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
        // draw dotted geometry left
        myDottedGeometries->at(3).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEContour::buildAndDrawDottedContourEdges(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
        const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */, const double lineWidth) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->size() != 4) {
        myDottedGeometries->clear();
        for (int i = 0; i < 4; i++) {
            myDottedGeometries->push_back(GUIDottedGeometry());
        }
    }

    /* Finish */

    // reset dotted geometry color
    myDottedGeometryColor.reset();
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR);
    // draw dotted geometry top
    myDottedGeometries->at(0).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // draw dotted geometry right
    myDottedGeometries->at(1).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // draw dotted geometry bot
    myDottedGeometries->at(2).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // draw dotted geometry left
    myDottedGeometries->at(3).drawDottedGeometry(s, type, myDottedGeometryColor, true, lineWidth);
    // pop matrix
    GLHelper::popMatrix();
}

/****************************************************************************/
