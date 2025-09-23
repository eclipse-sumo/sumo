/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over view
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementView.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementView::GNEMoveElementView(GNEAttributeCarrier* element) :
    GNEMoveElement(element) {
}


GNEMoveElementView::GNEMoveElementView(GNEAttributeCarrier* element, const Position& position) :
    GNEMoveElement(element),
    myPosition(position) {
}


GNEMoveElementView::~GNEMoveElementView() {}


GNEMoveOperation*
GNEMoveElementView::getMoveOperation() {
    if (myMovedElement->drawMovingGeometryPoints()) {
        // get snap radius
        const double snap_radius = myMovedElement->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        // get mouse position
        const Position mousePosition = myMovedElement->getNet()->getViewNet()->getPositionInformation();
        // check if we're editing width or height
        if (myShapeLength.back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit length
            return new GNEMoveOperation(this, myShapeLength, false, GNEMoveOperation::OperationType::LENGTH);
        } else if (myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, true, GNEMoveOperation::OperationType::WIDTH);
        } else if (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, false, GNEMoveOperation::OperationType::WIDTH);
        } else {
            return nullptr;
        }
    } else {
        // move entire space
        return new GNEMoveOperation(this, myPosition);
    }
}


void
GNEMoveElementView::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementView::setMoveShape(const GNEMoveResult& moveResult) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        myShapeLength[1] = moveResult.shapeToUpdate[1];
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        myShapeWidth = moveResult.shapeToUpdate;
    } else {
        myPosition = moveResult.shapeToUpdate.front();
    }
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementView::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        undoList->begin(myMovedElement, TLF("length of %", myMovedElement->getTagStr()));
        myMovedElement->setAttribute(SUMO_ATTR_LENGTH, toString(myShapeLength[0].distanceTo2D(moveResult.shapeToUpdate[1])), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        undoList->begin(myMovedElement, TLF("width of %", myMovedElement->getTagStr()));
        myMovedElement->setAttribute(SUMO_ATTR_WIDTH, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else {
        undoList->begin(myMovedElement, TLF("position of %", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
        undoList->end();
    }
}

/****************************************************************************/
