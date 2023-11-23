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
    // first build dotted contour
    buildDottedContourClosed(s, shape, scale);
    // check if mouse is within geometry
    gPostDrawing.positionWithinClosedShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), *myCachedShape);
    // draw dotted contours
    drawDottedContours(s, s.drawDottedContour(scale), addOffset, lineWidth);
}


void
GNEContour::drawDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape,
                                      const double extrusionWidth, const double scale, const bool drawFirstExtrem,
                                      const bool drawLastExtrem, const double lineWidth) const {
    // first build dotted contour
    buildDottedContourExtruded(s, shape, extrusionWidth, scale, drawFirstExtrem, drawLastExtrem);
    // check if mouse is within geometry
    gPostDrawing.positionWithinShapeLine(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), shape, extrusionWidth * scale);
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
    gPostDrawing.positionWithinClosedShape(myAC->getGUIGlObject(), myAC->getNet()->getViewNet()->getPositionInformation(), *myCachedShape);
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
GNEContour::buildDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape, const double scale) const {
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
}


void
GNEContour::buildDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape, const double extrusionWidth, const double scale,
        const bool drawFirstExtrem, const bool drawLastExtrem) const {
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
}


void
GNEContour::buildDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width, const double height,
        const double offsetX, const double offsetY, const double rot, const double scale) const {
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
        // save rectangleShape (needed for element under cursor)
        *myCachedShape = rectangleShape;
        // updated double values
        myCachedDoubles->at(0) = width;
        myCachedDoubles->at(1) = height;
        myCachedDoubles->at(2) = rot;
        myCachedDoubles->at(3) = scale;
    }
}


void
GNEContour::buildDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius, const double scale) const {
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
}


void
GNEContour::buildDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem) const {
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
        buildDottedContourExtruded(s, edge->getLanes().front()->getLaneShape(),
                                   laneDrawingConstants.halfWidth, laneDrawingConstants.exaggeration,
                                   drawFirstExtrem, drawLastExtrem);
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
    }
}


void
GNEContour::buildDottedContourEdges(const GUIVisualizationSettings& /*s*/,const GNEEdge* /* fromEdge */, const GNEEdge* /* toEdge */) const {
    // first change size of myDottedGeometries
    if (myDottedGeometries->size() != 4) {
        myDottedGeometries->clear();
        for (int i = 0; i < 4; i++) {
            myDottedGeometries->push_back(GUIDottedGeometry());
        }
    }
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
