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
/// @file    GNEMoveElementCrossing.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving crossing shapes
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElementCrossing.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementCrossing::GNEMoveElementCrossing(GNECrossing* crossing) :
    GNEMoveElement(crossing),
    myCrossing(crossing) {
}


GNEMoveElementCrossing::~GNEMoveElementCrossing() {}


std::string
GNEMoveElementCrossing::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementCrossing::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementCrossing::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementCrossing::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementCrossing::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementCrossing::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementCrossing::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


GNEMoveOperation*
GNEMoveElementCrossing::getMoveOperation() {
    // edit depending if shape is being edited
    if (myCrossing->isShapeEdited()) {
        // calculate move shape operation
        return getEditShapeOperation(myCrossing, myCrossing->getCrossingShape(), false);
    } else {
        return nullptr;
    }
}


void
GNEMoveElementCrossing::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (myCrossing->isShapeEdited()) {
        // get original shape
        PositionVector shape = myCrossing->getCrossingShape();
        // check shape size
        if (shape.size() > 2) {
            // obtain index
            int index = shape.indexOfClosest(clickedPosition);
            // get snap radius
            const double snap_radius = myCrossing->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.crossingGeometryPointRadius;
            // check if we have to create a new index
            if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
                // remove geometry point
                shape.erase(shape.begin() + index);
                // commit new shape
                undoList->begin(myCrossing, TLF("remove geometry point of %", myCrossing->getTagStr()));
                GNEChange_Attribute::changeAttribute(myCrossing, SUMO_ATTR_CUSTOMSHAPE, toString(shape), undoList, true);
                undoList->end();
            }
        }
    }
}


void
GNEMoveElementCrossing::setMoveShape(const GNEMoveResult& moveResult) {
    // set custom shape
    myCrossing->getNBCrossing()->customShape = moveResult.shapeToUpdate;
    // update geometry
    myCrossing->updateGeometry();
}


void
GNEMoveElementCrossing::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->begin(myCrossing, TLF("moving % of %", toString(SUMO_ATTR_CUSTOMSHAPE), myCrossing->getTagStr()));
    GNEChange_Attribute::changeAttribute(myCrossing, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate), undoList, true);
    undoList->end();
}

/****************************************************************************/
