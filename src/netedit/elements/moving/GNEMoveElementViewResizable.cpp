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
/// @file    GNEMoveElementViewResizable.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over view and resized
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElementViewResizable.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementViewResizable::GNEMoveElementViewResizable(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
        ResizingFormat resizingFormat, SumoXMLAttr posAttr,
        Position& position, double& width, double& height) :
    GNEMoveElementView(element, attributesFormat, posAttr, position),
    myWidth(width),
    myHeight(height),
    myEditWidth((resizingFormat == ResizingFormat::WIDTH_HEIGHT) || (resizingFormat == ResizingFormat::WIDTH_LENGTH)),
    myEditHeight(resizingFormat == ResizingFormat::WIDTH_HEIGHT),
    myEditLength(resizingFormat == ResizingFormat::WIDTH_LENGTH) {
}


GNEMoveElementViewResizable::~GNEMoveElementViewResizable() {}


GNEMoveOperation*
GNEMoveElementViewResizable::getMoveOperation() {
    if (myMovedElement->drawMovingGeometryPoints()) {
        // get snap radius
        const auto snap_radius = myMovedElement->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        const auto snapRadiusSquared = (snap_radius * snap_radius);
        // get mouse position
        const Position mousePosition = myMovedElement->getNet()->getViewNet()->getPositionInformation();
        // check what we're editing
        if (myEditLength && (myShapeHeight.size() > 0) && (myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit length
            return new GNEMoveOperation(this, myShapeHeight, false, GNEMoveOperation::OperationType::LENGTH);
        } else if (myEditWidth && (myShapeWidth.size() > 0) && (myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, true, GNEMoveOperation::OperationType::WIDTH);
        } else if (myEditWidth && (myShapeWidth.size() > 0) && (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, false, GNEMoveOperation::OperationType::WIDTH);
        } else if (myEditHeight && (myShapeHeight.size() > 0) && (myShapeHeight.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, true, GNEMoveOperation::OperationType::HEIGHT);
        } else if (myEditHeight && (myShapeHeight.size() > 0) && (myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, false, GNEMoveOperation::OperationType::HEIGHT);
        } else {
            return nullptr;
        }
    } else {
        // we're moving a position
        return GNEMoveElementView::getMoveOperation();
    }
}


void
GNEMoveElementViewResizable::setMoveShape(const GNEMoveResult& moveResult) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        myShapeHeight[1] = moveResult.shapeToUpdate[1];
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        myShapeWidth = moveResult.shapeToUpdate;
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::HEIGHT) {
        myShapeHeight = moveResult.shapeToUpdate;
    } else {
        // we're moving a position
        GNEMoveElementView::setMoveShape(moveResult);
    }
}


void
GNEMoveElementViewResizable::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        undoList->begin(myMovedElement, TLF("length of %", myMovedElement->getTagStr()));
        myMovedElement->setAttribute(SUMO_ATTR_LENGTH, toString(myShapeHeight[0].distanceTo2D(moveResult.shapeToUpdate[1])), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        undoList->begin(myMovedElement, TLF("width of %", myMovedElement->getTagStr()));
        myMovedElement->setAttribute(SUMO_ATTR_WIDTH, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::HEIGHT) {
        undoList->begin(myMovedElement, TLF("height of %", myMovedElement->getTagStr()));
        myMovedElement->setAttribute(SUMO_ATTR_HEIGHT, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else {
        // we're moving a position
        GNEMoveElementView::commitMoveShape(moveResult, undoList);
    }
}

/****************************************************************************/
