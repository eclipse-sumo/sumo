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
/// @file    GNEMoveElementJunction.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving junctions
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementJunction.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementJunction::GNEMoveElementJunction(GNEJunction* junction) :
    GNEMoveElement(junction),
    myJunction(junction) {
}


GNEMoveElementJunction::~GNEMoveElementJunction() {}


GNEMoveOperation*
GNEMoveElementJunction::getMoveOperation() {
    // edit depending if shape is being edited
    if (myJunction->isShapeEdited()) {
        // calculate move shape operation
        return getEditShapeOperation(myJunction, myJunction->getNBNode()->getShape(), false);
    } else {
        // return move junction position
        return new GNEMoveOperation(this, myJunction->getNBNode()->getPosition());
    }
}


std::string
GNEMoveElementJunction::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementJunction::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementJunction::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementJunction::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementJunction::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementJunction::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementJunction::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementJunction::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (myJunction->isShapeEdited()) {
        // get original shape
        PositionVector shape = myJunction->getNBNode()->getShape();
        // check shape size
        if (shape.size() > 2) {
            // obtain index
            int index = shape.indexOfClosest(clickedPosition);
            // get snap radius
            const double snap_radius = myJunction->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.junctionGeometryPointRadius;
            // check if we have to create a new index
            if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
                // remove geometry point
                shape.erase(shape.begin() + index);
                // commit new shape
                undoList->begin(myJunction, TLF("remove geometry point of %", myJunction->getTagStr()));
                GNEChange_Attribute::changeAttribute(myJunction, SUMO_ATTR_SHAPE, toString(shape), undoList, true);
                undoList->end();
            }
        }
    }
}


void
GNEMoveElementJunction::setMoveShape(const GNEMoveResult& moveResult) {
    // clear contour
    myJunction->myNetworkElementContour.clearContour();
    // set new position in NBNode without updating grid
    if (myJunction->isShapeEdited()) {
        // set new shape
        myJunction->getNBNode()->setCustomShape(moveResult.shapeToUpdate);
    } else if (moveResult.shapeToUpdate.size() > 0) {
        // obtain NBNode position
        const Position orig = myJunction->getNBNode()->getPosition();
        // move geometry
        myJunction->moveJunctionGeometry(moveResult.shapeToUpdate.front(), false);
        // check if move only center
        const bool onlyMoveCenter = myJunction->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getNetworkMoveOptions()->getMoveOnlyJunctionCenter();
        // set new position of adjacent edges depending if we're moving a selection
        for (const auto& NBEdge : myJunction->getNBNode()->getEdges()) {
            myJunction->getNet()->getAttributeCarriers()->retrieveEdge(NBEdge->getID())->updateJunctionPosition(myJunction, onlyMoveCenter ? myJunction->getNBNode()->getPosition() : orig);
        }
    }
    myJunction->updateGeometry();
}


void
GNEMoveElementJunction::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // make sure that newShape isn't empty
    if (moveResult.shapeToUpdate.size() > 0) {
        // check if we're editing a shape
        if (myJunction->isShapeEdited()) {
            // commit new shape
            undoList->begin(myJunction, TLF("moving shape of %", myJunction->getTagStr()));
            myJunction->setAttribute(SUMO_ATTR_SHAPE, toString(moveResult.shapeToUpdate), undoList);
            undoList->end();
        } else if (myJunction->getNBNode()->hasCustomShape()) {
            // commit new shape
            undoList->begin(myJunction, TLF("moving custom shape of %", myJunction->getTagStr()));
            myJunction->setAttribute(SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
            // calculate offset and apply to custom shape
            const auto customShapeOffset = moveResult.shapeToUpdate.front() - myJunction->getNBNode()->getCenter();
            const auto customShapeMoved = myJunction->getNBNode()->getShape().added(customShapeOffset);
            myJunction->setAttribute(SUMO_ATTR_SHAPE, toString(customShapeMoved), undoList);
            undoList->end();
        } else {
            myJunction->setAttribute(SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
        }
    }
}

/****************************************************************************/
