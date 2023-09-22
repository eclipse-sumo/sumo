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
    myDottedGeometryA(new GUIDottedGeometry()),
    myDottedGeometryB(new GUIDottedGeometry()),
    myDottedGeometryC(new GUIDottedGeometry()) {
}


GNEContourElement::~GNEContourElement() {
    delete myCachedShape;
    delete myCachedScale;
    delete myDottedGeometryA;
    delete myDottedGeometryB;
    delete myDottedGeometryC;
}


void
GNEContourElement::drawDottedContour(const GNENet *net, const PositionVector &shape, double width, double exaggeration) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(myAC)) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, width, exaggeration);
    }
    // front attribute contour
    if (net->getViewNet()->getFrontAttributeCarrier() == myAC) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, width, exaggeration);
    }
    // delete contour
    if (net->getViewNet()->drawDeleteContour(myAC->getGUIGlObject(), myAC)) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, width, exaggeration);
    }
    // select contour
    if (net->getViewNet()->drawSelectContour(myAC->getGUIGlObject(), myAC)) {
        buildAndDrawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, width, exaggeration);
    }
}


void
GNEContourElement::drawDottedContour(const GNENet *net, const PositionVector &shape, double width, double exaggeration,
                                     const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // get VisualisationSettings
    const auto &s = net->getViewNet()->getVisualisationSettings();
    // inspect contour
    if (net->getViewNet()->isAttributeCarrierInspected(myAC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, width,
                                           exaggeration, drawFirstExtrem, drawLastExtrem);
    }
    // front attribute contour
    if (net->getViewNet()->getFrontAttributeCarrier() == myAC) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::FRONT, shape, width,
                                           exaggeration, drawFirstExtrem, drawLastExtrem);
    }
    // delete contour
    if (net->getViewNet()->drawDeleteContour(myAC->getGUIGlObject(), myAC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, width,
                                           exaggeration, drawFirstExtrem, drawLastExtrem);
    }
    // select contour
    if (net->getViewNet()->drawSelectContour(myAC->getGUIGlObject(), myAC)) {
        buildAndDrawDottedContourRectangle(s, GUIDottedGeometry::DottedContourType::SELECT, shape, width,
                                           exaggeration, drawFirstExtrem, drawLastExtrem);
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
        drawDottedContour(edge->getNet(), edge->getLanes().front()->getLaneShape(), laneDrawingConstants.halfWidth, laneDrawingConstants.exaggeration, true, true);
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
            // declare DottedGeometryColor
            GUIDottedGeometry::DottedGeometryColor dottedGeometryColor(s);
            // calculate extremes
            *myDottedGeometryC = GUIDottedGeometry(s, *myDottedGeometryA, true, *myDottedGeometryB, true);
            // update cached shape
            *myCachedShape = edgeShape;
        }
        // draw dotted geometries
        drawDottedContour(edge->getNet(), edgeShape, 0, 1, true, true);
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
        // declare DottedGeometryColor
        GUIDottedGeometry::DottedGeometryColor dottedGeometryColor(s);
        // Push draw matrix
        GLHelper::pushMatrix();
        // translate to front
        glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
        // draw top dotted geometry
        myDottedGeometryA->drawDottedGeometry(s, type, dottedGeometryColor, width);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEContourElement::buildAndDrawDottedContourRectangle(const GUIVisualizationSettings& s, const GUIDottedGeometry::DottedContourType type,
                                                      const PositionVector &shape, double width, double exaggeration,
                                                      const bool drawFirstExtrem, const bool drawLastExtrem) const {
    // check if dotted geometry has to be updated
    if (*myCachedShape != shape) {
        // calculate center dotted geometry
        GUIDottedGeometry dottedGeometry(s, shape, false);
        // make a copy of dotted geometry
        *myDottedGeometryA = dottedGeometry;
        *myDottedGeometryB = dottedGeometry;
        // move geometries top and bot
        myDottedGeometryA->moveShapeToSide(width * exaggeration);
        myDottedGeometryB->moveShapeToSide(width * exaggeration * -1);
        // invert offset of top dotted geometry
        myDottedGeometryA->invertOffset();
        // calculate extremes
        *myDottedGeometryC = GUIDottedGeometry(s, *myDottedGeometryA, drawFirstExtrem, *myDottedGeometryB, drawLastExtrem);
        // finally update cached shape
        *myCachedShape = shape;
    }
    // declare DottedGeometryColor
    GUIDottedGeometry::DottedGeometryColor dottedGeometryColor(s);
    // Push draw matrix
    GLHelper::pushMatrix();
    // translate to front
    glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
    // draw top dotted geometry
    myDottedGeometryA->drawDottedGeometry(s, type, dottedGeometryColor);
    // reset color
    dottedGeometryColor.reset();
    // draw top dotted geometry
    myDottedGeometryB->drawDottedGeometry(s, type, dottedGeometryColor);
    // change color
    dottedGeometryColor.changeColor();
    // draw extrem dotted geometry
    myDottedGeometryC->drawDottedGeometry(s, type, dottedGeometryColor);
    // pop matrix
    GLHelper::popMatrix();
}

/****************************************************************************/
