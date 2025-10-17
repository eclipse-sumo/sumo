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
/// @file    GNEMoveElementShape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for shape elements that can be moved over view
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementShape.h"

// ===========================================================================
// static members
// ===========================================================================

PositionVector GNEMoveElementShape::EMPTY_SHAPE;

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementShape::GNEMoveElementShape(GNEAttributeCarrier* element) :
    GNEMoveElement(element),
    myMovingShape(EMPTY_SHAPE) {
}


GNEMoveElementShape::GNEMoveElementShape(GNEAttributeCarrier* element, PositionVector& shape, const bool alwaysClosed) :
    GNEMoveElement(element),
    myMovingShape(shape),
    myAlwaysClosed(alwaysClosed) {
}


GNEMoveElementShape::GNEMoveElementShape(GNEAttributeCarrier* element, PositionVector& shape, const Position& position, const bool alwaysClosed) :
    GNEMoveElement(element),
    myCenterPosition(position),
    myMovingShape(shape),
    myAlwaysClosed(alwaysClosed) {
}


GNEMoveElementShape::~GNEMoveElementShape() {}


GNEMoveOperation*
GNEMoveElementShape::getMoveOperation() {
    // get snap radius
    const double snap_radius = myMovedElement->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
    // check if we're moving center or shape
    if (myCenterPosition.distanceSquaredTo2D(myMovedElement->getNet()->getViewNet()->getPositionInformation()) < (snap_radius * snap_radius)) {
        // move entire shape
        return new GNEMoveOperation(this, myCenterPosition);
    } else if (myMovedElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getNetworkMoveOptions()->getMoveWholePolygons()) {
        // move entire shape
        return new GNEMoveOperation(this, myMovingShape);
    } else {
        // calculate move shape operation
        return getEditShapeOperation(myMovedElement->getGUIGlObject(), myMovingShape, myAlwaysClosed);
    }
}


void
GNEMoveElementShape::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // get original shape
    PositionVector shape = myMovingShape;
    // check shape size
    if (shape.size() > 3) {
        // obtain index
        int index = shape.indexOfClosest(clickedPosition);
        // get last index
        const int lastIndex = ((int)shape.size() - 1);
        // get snap radius
        const double snap_radius = myMovedElement->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
        // check if we have to create a new index
        if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
            // check if we're deleting the first point
            if ((index == 0) || (index == lastIndex)) {
                // remove both geometry point
                shape.erase(shape.begin() + lastIndex);
                shape.erase(shape.begin());
                // close shape
                shape.closePolygon();
            } else {
                // remove geometry point
                shape.erase(shape.begin() + index);
            }
            // commit new shape
            undoList->begin(myMovedElement, TLF("remove geometry point of %", myMovedElement->getTagStr()));
            GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_SHAPE, toString(shape), undoList);
            undoList->end();
        }
    }
}


std::string
GNEMoveElementShape::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementShape::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementShape::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementShape::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementShape::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    return myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementShape::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementShape::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    return myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementShape::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::POSITION) {
        // update new center
        myCenterPosition = moveResult.shapeToUpdate.front();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::ENTIRE_SHAPE) {
        // update new shape and center
        myCenterPosition.add(moveResult.shapeToUpdate.getCentroid() - myMovingShape.getCentroid());
        myMovingShape = moveResult.shapeToUpdate;
    } else {
        // get lastIndex
        const int lastIndex = (int)moveResult.shapeToUpdate.size() - 1;
        // update new shape
        myMovingShape = moveResult.shapeToUpdate;
        // adjust first and last position
        if (moveResult.geometryPointsToMove.front() == 0) {
            myMovingShape[lastIndex] = moveResult.shapeToUpdate[0];
        } else if (moveResult.geometryPointsToMove.front() == lastIndex) {
            myMovingShape[0] = moveResult.shapeToUpdate[lastIndex];
        }
        myMovingShape.closePolygon();
    }
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementShape::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::POSITION) {
        // commit center
        undoList->begin(myMovedElement, TLF("moving center of %", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_CENTER, toString(moveResult.shapeToUpdate.front()), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::ENTIRE_SHAPE) {
        // calculate offset between old and new shape
        Position newCenter = myCenterPosition;
        newCenter.add(moveResult.shapeToUpdate.getCentroid() - myMovingShape.getCentroid());
        // commit new shape and center
        undoList->begin(myMovedElement, TLF("moving entire shape of %", myMovedElement->getTagStr()));
        if (myMovedElement->getTagProperty()->hasAttribute(SUMO_ATTR_CENTER)) {
            GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_CENTER, toString(newCenter), undoList);
        }
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_SHAPE, toString(moveResult.shapeToUpdate), undoList);
        undoList->end();
    } else {
        // get lastIndex
        const int lastIndex = (int)moveResult.shapeToUpdate.size() - 1;
        // close shapeToUpdate
        auto closedShape = moveResult.shapeToUpdate;
        // adjust first and last position
        if (moveResult.geometryPointsToMove.front() == 0) {
            closedShape[lastIndex] = moveResult.shapeToUpdate[0];
        } else if (moveResult.geometryPointsToMove.front() == lastIndex) {
            closedShape[0] = moveResult.shapeToUpdate[lastIndex];
        }
        // commit new shape
        undoList->begin(myMovedElement, TLF("editing shape of %", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_SHAPE, toString(closedShape), undoList);
        undoList->end();
    }
}

/****************************************************************************/
