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
#include <utils/gui/div/GLHelper.h>

#include "GNEDottedContourThread.h"


// ===========================================================================
// member method definitions
// ===========================================================================
GNEDottedContourThread::GNEDottedContourThread(GNENet *net) :
    FXThread(),
    myNet(net),
    myLockNetElementsQueue(false),
    myLockAdditionalsQueue(false),
    myLockShapeQueue(false),
    myLockDemandElementsQueue(false) {
    // start thread
    start();
}


GNEDottedContourThread::~GNEDottedContourThread() {
    // stop thread
    cancel();
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

/****************************************************************************/
