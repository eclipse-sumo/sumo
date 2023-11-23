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
#include <utils/gui/div/GUIGlobalPostDrawing.h>

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
    myCachedShapes(new std::vector<PositionVector>()),
    myCachedDoubles(new std::vector<double>()),
    myDottedGeometries(new std::vector<GUIDottedGeometry>()) {
}


GNEContour::~GNEContour() {
    delete myCachedShapes;
    delete myCachedDoubles;
    delete myDottedGeometries;
}


void
GNEContour::reset() const {
    myCachedShapes->clear();
    myCachedDoubles->clear();
    myDottedGeometries->clear();
}


void
GNEContour::drawDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                    const double scale, const bool addOffset, const double lineWidth) const {
    // first build dotted contour
    buildDottedContourClosed(s, shape, scale);
    // check if mouse is within geometry
    gPostDrawing.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), myCachedShapes->at(0));
    // draw dotted contours
    drawDottedContours(s, s.drawDottedContour(scale), addOffset, lineWidth);
}


void
GNEContour::drawDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape,
                                      const double extrusionWidth, const double scale, const bool drawFirstExtrem,
                                      const bool drawLastExtrem, const double lineWidth) const {
    // first build dotted contour
    buildDottedContourExtruded(s, shape, extrusionWidth, scale, drawFirstExtrem, drawLastExtrem);
    // check if mouse is within two lines
    gPostDrawing.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), myCachedShapes->at(1));
    // draw dotted contours
    drawDottedContours(s, scale, true, lineWidth);
}


void
GNEContour::drawDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width,
                                       const double height, const double offsetX, const double offsetY, const double rot,
                                       const double scale, const double lineWidth) const {
    // first build dotted contour
    buildDottedContourRectangle(s, pos, width, height, offsetX, offsetY, rot, scale);
    // check if mouse is within geometry
    gPostDrawing.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), myCachedShapes->at(0));
    // draw dotted contours
    drawDottedContours(s, scale, true, lineWidth);
}


void
GNEContour::drawDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius,
                                    const double scale, const double lineWidth) const {
    // first build dotted contour
    buildDottedContourCircle(s, pos, radius, scale);
    // check if mouse is within geometry
    gPostDrawing.positionWithinCircle(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), pos, (radius * scale));
    // draw dotted contours
    drawDottedContours(s, scale, true, lineWidth);
}


void
GNEContour::drawDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem,
                                  const bool drawLastExtrem, const double lineWidth) const {
    // first build dotted contour
    buildDottedContourEdge(s, edge, drawFirstExtrem, drawLastExtrem);
    // check if mouse is within two lines
    gPostDrawing.positionWithinShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), myCachedShapes->at(2));
    // draw dotted contours
    drawDottedContours(s, 1, true, lineWidth);
}


void
GNEContour::drawDottedContourEdges(const GUIVisualizationSettings& s, const GNEEdge* fromEdge, const GNEEdge* toEdge,
                                   const double lineWidth) const {
    // first build dotted contour
    buildDottedContourEdges(s, fromEdge, toEdge);
    // draw dotted contours
    drawDottedContours(s, 1, true, lineWidth);
}


void
GNEContour::drawInnenContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                   const double scale, const double lineWidth) const {
    // check dotted caches
    checkDottedCaches(1, 1, 1);
    // check if dotted geometry has to be updated
    if ((myCachedShapes->at(0) != shape) || (myCachedDoubles->at(0) != scale)) {
        // declare scaled shape
        PositionVector scaledShape = shape;
        // scale shape
        scaledShape.scaleRelative(scale);
        // close
        scaledShape.closePolygon();
        // calculate geometry without resampling
        myDottedGeometries->at(0) = GUIDottedGeometry(s, scaledShape, true, false);
        // finally update cached shape
        myCachedShapes->at(0) = shape;
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
GNEContour::checkDottedCaches(size_t shapes, size_t doubles, size_t dottedGeometries) const {
    // check shapes
    if (myCachedShapes->size() != shapes) {
        myCachedShapes->clear();
        for (size_t i = 0; i < shapes; i++) {
            myCachedShapes->push_back(PositionVector({Position::INVALID}));
        }
    }
    // check doubles
    if (myCachedDoubles->size() != doubles) {
        myCachedDoubles->clear();
        for (size_t i = 0; i < doubles; i++) {
            myCachedDoubles->push_back(double(INVALID_DOUBLE));
        }
    }
    // check dotted geometries
    if (myDottedGeometries->size() != dottedGeometries) {
        myDottedGeometries->clear();
        for (size_t i = 0; i < dottedGeometries; i++) {
            myDottedGeometries->push_back(GUIDottedGeometry());
        }
    }
}

void
GNEContour::buildDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape, const double scale) const {
    // check dotted caches
    checkDottedCaches(1, 1, 1);
    // check if dotted geometry has to be updated
    if ((myCachedShapes->at(0) != shape) || (myCachedDoubles->at(0) != scale)) {
        // declare scaled shape
        PositionVector scaledShape = shape;
        // scale shape
        scaledShape.scaleRelative(scale);
        // close
        scaledShape.closePolygon();
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, scaledShape, true, true);
        // finally update cached shape
        myCachedShapes->at(0) = shape;
        myCachedDoubles->at(0) = scale;
    }
}


void
GNEContour::buildDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape, const double extrusionWidth, const double scale,
        const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // check dotted caches
    checkDottedCaches(2, 1, 4);
    // check if dotted geometry has to be updated
    if ((myCachedShapes->at(0) != shape) || (myCachedDoubles->at(0) != scale)) {
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
        // update cached shape
        myCachedShapes->at(0) = shape;
        // update scale
        myCachedDoubles->at(0) = scale;
        // finally create shape used in positionWithinClosedShape
        myCachedShapes->at(1).clear();
        for (const auto &position : myDottedGeometries->at(0).getUnresampledShape()) {
            myCachedShapes->at(1).push_back(position);
        }
        for (const auto &position : myDottedGeometries->at(2).getUnresampledShape()) {
            myCachedShapes->at(1).push_back(position);
        }
        myCachedShapes->at(1).closePolygon();
    }
}


void
GNEContour::buildDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width, const double height,
        const double offsetX, const double offsetY, const double rot, const double scale) const {
    // check dotted caches
    checkDottedCaches(2, 4, 1);
    // continue depending of cached positiosn
    if ((myCachedShapes->at(1).front() != pos) || (myCachedDoubles->at(0) != width) ||
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
        myCachedShapes->at(1)[0] = pos;
        // save rectangleShape (needed for element under cursor)
        myCachedShapes->at(0) = rectangleShape;
        // updated double values
        myCachedDoubles->at(0) = width;
        myCachedDoubles->at(1) = height;
        myCachedDoubles->at(2) = rot;
        myCachedDoubles->at(3) = scale;
    }
}


void
GNEContour::buildDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius, const double scale) const {
    // check dotted caches
    checkDottedCaches(2, 1, 1);
    // continue depending of radius and scale
    if ((myCachedShapes->at(1).front() != pos) || (myCachedDoubles->at(0) != (radius * scale))) {
        // get vertex circle shape
        const auto circleShape = GUIGeometry::getVertexCircleAroundPosition(pos, radius * scale, (radius * scale) < 2 ? 8 : 16);
        // calculate dotted geometry
        myDottedGeometries->at(0) = GUIDottedGeometry(s, circleShape, true, true);
        // update cached position
        myCachedShapes->at(1)[0] = pos;
        // update cached scale
        myCachedDoubles->at(0) = radius * scale;
    }
}


void
GNEContour::buildDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // check dotted caches
    checkDottedCaches(3, 2, 4);
    // set left hand flag
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    // obtain lanes
    const GNELane* topLane = lefthand ? edge->getLanes().front() : edge->getLanes().back();
    const GNELane* botLane = lefthand ? edge->getLanes().back() : edge->getLanes().front();
    // obtain both LaneDrawingConstants
    GNELane::LaneDrawingConstants laneDrawingConstantsTop(s, topLane);
    GNELane::LaneDrawingConstants laneDrawingConstantsBot(s, botLane);
    // check shapes and widths
    if ((myCachedShapes->at(0) != topLane->getLaneGeometry().getShape()) ||
        (myCachedDoubles->at(0) != (laneDrawingConstantsTop.halfWidth * laneDrawingConstantsTop.exaggeration)) ||
        (myCachedShapes->at(1) != botLane->getLaneGeometry().getShape()) ||
        (myCachedDoubles->at(1) != (laneDrawingConstantsBot.halfWidth * laneDrawingConstantsBot.exaggeration))) {
        // update cached values
        myCachedShapes->at(0) = topLane->getLaneGeometry().getShape();
        myCachedDoubles->at(0) = (laneDrawingConstantsTop.halfWidth * laneDrawingConstantsTop.exaggeration);
        myCachedShapes->at(1) = botLane->getLaneGeometry().getShape();
        myCachedDoubles->at(1) = (laneDrawingConstantsBot.halfWidth * laneDrawingConstantsBot.exaggeration);
        // create top and bot geometries
        myDottedGeometries->at(0) = GUIDottedGeometry(s, myCachedShapes->at(0), false, true);
        myDottedGeometries->at(2) = GUIDottedGeometry(s, myCachedShapes->at(1).reverse(), false, true);
        // move geometries top and bot
        myDottedGeometries->at(0).moveShapeToSide(myCachedDoubles->at(0) * -1);
        myDottedGeometries->at(2).moveShapeToSide(myCachedDoubles->at(1) * -1);
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
        myCachedShapes->at(2).clear();
        for (const auto &position : myDottedGeometries->at(0).getUnresampledShape()) {
            myCachedShapes->at(2).push_back(position);
        }
        for (const auto &position : myDottedGeometries->at(2).getUnresampledShape()) {
            myCachedShapes->at(2).push_back(position);
        }
        myCachedShapes->at(2).closePolygon();
    }
}


void
GNEContour::buildDottedContourEdges(const GUIVisualizationSettings& /*s*/,const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */) const {
    // check dotted caches
    checkDottedCaches(0, 0, 4);
}


void
GNEContour::drawDottedContours(const GUIVisualizationSettings& s, const double scale, const bool addOffset, const double lineWidth) const {
    // first check if draw dotted contour
    if (s.drawDottedContour(scale)) {
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
