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
/// @file    GNEMoveElementView.cpp
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


GNEMoveElementView::GNEMoveElementView(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
                                       const Position& position) :
    GNEMoveElement(element),
    myPosOverView(position),
    myAttributesFormat(attributesFormat) {
}


GNEMoveElementView::GNEMoveElementView(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
                                       const Position& position, const double width, const double height, const double length) :
    GNEMoveElement(element),
    myPosOverView(position),
    myWidth(width),
    myHeight(height),
    myLength(length),
    myAttributesFormat(attributesFormat)  {
}


GNEMoveElementView::~GNEMoveElementView() {}


GNEMoveOperation*
GNEMoveElementView::getMoveOperation() {
    if (myMovedElement->drawMovingGeometryPoints()) {
        // get snap radius
        const auto snap_radius = myMovedElement->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        const auto snapRadiusSquared = (snap_radius * snap_radius);
        // get mouse position
        const Position mousePosition = myMovedElement->getNet()->getViewNet()->getPositionInformation();
        // check what we're editing
        if ((myShapeLength.size() > 0) && (myShapeLength.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit length
            return new GNEMoveOperation(this, myShapeLength, false, GNEMoveOperation::OperationType::LENGTH);
        } else if ((myShapeWidth.size() > 0) && (myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, true, GNEMoveOperation::OperationType::WIDTH);
        } else if ((myShapeWidth.size() > 0) && (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, false, GNEMoveOperation::OperationType::WIDTH);
        } else if ((myShapeHeight.size() > 0) && (myShapeHeight.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, true, GNEMoveOperation::OperationType::HEIGHT);
        } else if ((myShapeHeight.size() > 0) && (myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, false, GNEMoveOperation::OperationType::HEIGHT);
        } else {
            return nullptr;
        }
    } else {
        // move entire space
        return new GNEMoveOperation(this, myPosOverView);
    }
}


void
GNEMoveElementView::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementView::writeMoveAttributes(OutputDevice& device) const {
    // position format
    if (myAttributesFormat == AttributesFormat::POSITION) {
        device.writeAttr(SUMO_ATTR_POSITION, myPosOverView);
    } else {
        // x-y format
        if (myAttributesFormat == AttributesFormat::CARTESIAN) {
            device.writeAttr(SUMO_ATTR_X, myPosOverView.x());
            device.writeAttr(SUMO_ATTR_Y, myPosOverView.y());
        }
        // geo format
        if (myAttributesFormat == AttributesFormat::GEO) {
            device.setPrecision(gPrecisionGeo);
            device.writeAttr(SUMO_ATTR_LON, myMovedElement->getAttributeDouble(SUMO_ATTR_LON));
            device.writeAttr(SUMO_ATTR_LAT, myMovedElement->getAttributeDouble(SUMO_ATTR_LAT));
            device.setPrecision();
        }
        // z
        if (myPosOverView.z() != 0) {
            device.writeAttr(SUMO_ATTR_Z, myPosOverView.z());
        }
    }
}


void
GNEMoveElementView::setMoveShape(const GNEMoveResult& moveResult) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::LENGTH) {
        myShapeLength[1] = moveResult.shapeToUpdate[1];
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        myShapeWidth = moveResult.shapeToUpdate;
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::HEIGHT) {
        myShapeHeight = moveResult.shapeToUpdate;
    } else {
        myPosOverView = moveResult.shapeToUpdate.front();
        // only update geometry in this case (because in the others the shapes are reset)
        myMovedElement->updateGeometry();
    }
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
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::HEIGHT) {
        undoList->begin(myMovedElement, TLF("height of %", myMovedElement->getTagStr()));
        myMovedElement->setAttribute(SUMO_ATTR_HEIGHT, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else {
        undoList->begin(myMovedElement, TLF("position of %", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
        undoList->end();
    }
}

/****************************************************************************/
