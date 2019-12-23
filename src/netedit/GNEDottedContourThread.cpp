/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDottedContourThread.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2019
///
// The thread that performs the calculation of contours around shapes
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <netbuild/NBFrame.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEDottedContourThread.h"


// ===========================================================================
// member method definitions
// ===========================================================================
GNEDottedContourThread::GNEDottedContourThread(GNENet *net) :
    FXThread(),
    myNet(net),
    myVisualizationSetting(nullptr),
    myLockNetElementsQueue(false),
    myLockAdditionalsQueue(false),
    myLockShapeQueue(false),
    myLockDemandElementsQueue(false) {

}


GNEDottedContourThread::~GNEDottedContourThread() {
    // stop thread
    cancel();
}


void 
GNEDottedContourThread::setVisualizationSettings(GUIVisualizationSettings* s) {
    // set visualization settings
    myVisualizationSetting = s;
    // start thread
    start();
}


void 
GNEDottedContourThread::updateNetElementDottedContour(GNENetElement *netElement) {
    myLockNetElementsQueue = true;
    myNetElements.push(netElement);
    myLockNetElementsQueue = false;
}


FXint
GNEDottedContourThread::run() {
    while (true) {
        // check net elements
        if (myLockNetElementsQueue == false) {
            if (myNetElements.size() > 0) {
                GNENetElement *netElementFront = myNetElements.front();
                if (netElementFront->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
                    calculateJunctionDottedContour(netElementFront);
                } else if (netElementFront->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    calculateEdgeDottedContour(netElementFront); 
                }
                myNetElements.pop();
            }
        }
        // check additionals
        if (myLockAdditionalsQueue == false) {
            if (myAdditionals.size() > 0) {
                myAdditionals.pop();
            }
        }
        // check shape
        if (myLockShapeQueue == false) {
            if (myShapes.size() > 0) {
                myShapes.pop();
            }
        }
        // check demand elements
        if (myLockDemandElementsQueue == false) {
            if (myDemandElements.size() > 0) {
                myDemandElements.pop();
            }
        }
        // sleep
        sleep(100);
    }
    return 0;
}


void 
GNEDottedContourThread::calculateJunctionDottedContour(GNENetElement* junction) {
    // obtain pos and junction shape
    Position pos = GNEAttributeCarrier::parse<Position>(junction->getAttribute(SUMO_ATTR_POSITION));
    PositionVector shape = GNEAttributeCarrier::parse<PositionVector>(junction->getAttribute(SUMO_ATTR_SHAPE));
    // check if we have to calculate buuble or shape
    if (shape.area() < 4) {
        junction->updateDottedGeometry(GLHelper::drawFilledCircleReturnVertices(4, 32));
    } else {
        // close polygon
        shape.closePolygon();
        junction->updateDottedGeometry(shape);
    }
}


void 
GNEDottedContourThread::calculateEdgeDottedContour(GNENetElement* edge) {
    // temporal
    GNEEdge* castedEdge = dynamic_cast<GNEEdge*>(edge);
    // obtain lanes
    const GNELane* frontLane = castedEdge->getLanes().front();
    const GNELane* backLane = castedEdge->getLanes().back();
    // obtain lane widdths
    const double myHalfLaneWidthFront = castedEdge->getNBEdge()->getLaneWidth(frontLane->getIndex()) / 2;
    const double myHalfLaneWidthBack = (myVisualizationSetting->spreadSuperposed && backLane->drawAsRailway(myVisualizationSetting) && 
        castedEdge->getNBEdge()->isBidiRail()) ? 0 : castedEdge->getNBEdge()->getLaneWidth(backLane->getIndex()) / 2;
    // obtain shapes from NBEdge
    PositionVector mainShape = frontLane->getParentEdge()->getNBEdge()->getLaneShape(frontLane->getIndex());
    PositionVector backShape = backLane->getParentEdge()->getNBEdge()->getLaneShape(backLane->getIndex());
    // move to side depending of lefthand
    if (myVisualizationSetting->lefthand) {
        mainShape.move2side(myHalfLaneWidthFront * -1);
        backShape.move2side(myHalfLaneWidthBack);
    } else {
        mainShape.move2side(myHalfLaneWidthFront);
        backShape.move2side(myHalfLaneWidthBack * -1);
    }
    // reverse back shape
    backShape = backShape.reverse();
    // add back shape into mainShape
    for (const auto &position : backShape) {
        mainShape.push_back(position);
    }
    // close polygon
    mainShape.closePolygon();
    // update edge dotted geometry
    edge->updateDottedGeometry(mainShape);
}

/****************************************************************************/
