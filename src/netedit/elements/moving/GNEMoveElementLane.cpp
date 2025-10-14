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
/// @file    GNEMoveElementLane.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving lane shapes
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElementLane.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementLane::GNEMoveElementLane(GNELane* lane) :
    GNEMoveElement(lane),
    myLane(lane) {
}


GNEMoveElementLane::~GNEMoveElementLane() {}


GNEMoveOperation*
GNEMoveElementLane::getMoveOperation() {
    // edit depending if shape is being edited
    if (myLane->isShapeEdited()) {
        // calculate move shape operation
        return getEditShapeOperation(myLane, myLane->getLaneShape(), false);
    } else {
        return nullptr;
    }
}


std::string
GNEMoveElementLane::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementLane::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementLane::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementLane::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementLane::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementLane::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementLane::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementLane::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (myLane->isShapeEdited()) {
        // get original shape
        PositionVector shape = myLane->getLaneShape();
        // check shape size
        if (shape.size() > 2) {
            // obtain index
            int index = shape.indexOfClosest(clickedPosition);
            // get snap radius
            const double snap_radius = myLane->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.laneGeometryPointRadius;
            // check if we have to create a new index
            if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
                // remove geometry point
                shape.erase(shape.begin() + index);
                // commit new shape
                undoList->begin(myLane, TLF("remove geometry point of %", myLane->getTagStr()));
                GNEChange_Attribute::changeAttribute(myLane, SUMO_ATTR_CUSTOMSHAPE, toString(shape), undoList);
                undoList->end();
            }
        }
    }
}


void
GNEMoveElementLane::setMoveShape(const GNEMoveResult& moveResult) {
    // set custom shape
    myLane->getParentEdges().front()->getNBEdge()->getLaneStruct(myLane->getIndex()).customShape = moveResult.shapeToUpdate;
    // update geometry
    myLane->updateGeometry();
}


void
GNEMoveElementLane::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->begin(myLane, TLF("moving custom shape of %", myLane->getTagStr()));
    GNEChange_Attribute::changeAttribute(myLane, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate), undoList);
    undoList->end();
}

/****************************************************************************/
