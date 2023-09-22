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

#include "GNEContourElement.h"
#include "GNEAttributeCarrier.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEContourElement::GNEContourElement(GNEAttributeCarrier* AC) :
    myAC(AC),
    myCachedShape(new PositionVector()),
    myDottedGeometryA(new GUIDottedGeometry()),
    myDottedGeometryB(new GUIDottedGeometry()),
    myDottedGeometryC(new GUIDottedGeometry()) {
}


GNEContourElement::~GNEContourElement() {
    delete myCachedShape;
    delete myDottedGeometryA;
    delete myDottedGeometryB;
    delete myDottedGeometryC;
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
