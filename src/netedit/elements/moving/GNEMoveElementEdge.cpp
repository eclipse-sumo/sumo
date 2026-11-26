/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveElementEdge.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving edge shapes
/****************************************************************************/
#include <config.h>

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/geom/GeomHelper.h>

#include "GNEMoveElementEdge.h"

// ===========================================================================
// defines
// ===========================================================================

#define ENDPOINT_TOLERANCE 2

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementEdge::GNEMoveElementEdge(GNEEdge* edge) :
    GNEMoveElement(edge),
    myEdge(edge) {
}


GNEMoveElementEdge::~GNEMoveElementEdge() {}


GNEEdge*
GNEMoveElementEdge::getEdge() const {
    return myEdge;
}


GNEMoveOperation*
GNEMoveElementEdge::getMoveOperation() {
    // get geometry point radius
    const double geometryPointRadius = myEdge->getGeometryPointRadius();
    // get move multiple element values
    const auto& moveMultipleElementValues = myEdge->getNet()->getViewNet()->getMoveMultipleElementValues();
    // check if edge is selected and we're moving a selection
    if (!myEdge->isShapeEdited() && myEdge->isAttributeCarrierSelected() &&
            (moveMultipleElementValues.isMovingSelection() || moveMultipleElementValues.isMovingSelectedEdge())) {
        // check if both junctions are selected
        if (myEdge->getFromJunction()->isAttributeCarrierSelected() && myEdge->getToJunction()->isAttributeCarrierSelected()) {
            return processMoveBothJunctionSelected();
        } else if (myEdge->getFromJunction()->isAttributeCarrierSelected()) {
            return processMoveFromJunctionSelected(geometryPointRadius);
        } else if (myEdge->getToJunction()->isAttributeCarrierSelected()) {
            return processMoveToJunctionSelected(geometryPointRadius);
        } else if (moveMultipleElementValues.isMovingSelectedEdge()) {
            if (myEdge->getNet()->getAttributeCarriers()->getNumberOfSelectedEdges() == 1) {
                // special case: when only a single edge is selected, move all shape points (including custom end points)
                return processMoveBothJunctionSelected();
            } else {
                // synchronized movement of a single point
                return processNoneJunctionSelected(geometryPointRadius);
            }
        } else {
            // calculate move shape operation
            return getEditShapeOperation(myEdge, myEdge->getNBEdge()->getGeometry(), false);
        }
    } else {
        // calculate move shape operation
        return getEditShapeOperation(myEdge, myEdge->getNBEdge()->getGeometry(), false);
    }
}


std::string
GNEMoveElementEdge::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementEdge::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementEdge::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementEdge::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementEdge::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementEdge::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementEdge::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementEdge::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // get geometry point radius
    const double geometryPointRadius = myEdge->getGeometryPointRadius();
    // declare shape to move
    PositionVector shape = myEdge->getNBEdge()->getGeometry();
    // obtain flags for start and end positions
    const bool customStartPosition = (myEdge->getNBEdge()->getGeometry().front().distanceSquaredTo2D(myEdge->getFromJunction()->getNBNode()->getPosition()) > ENDPOINT_TOLERANCE);
    const bool customEndPosition = (myEdge->getNBEdge()->getGeometry().back().distanceSquaredTo2D(myEdge->getToJunction()->getNBNode()->getPosition()) > ENDPOINT_TOLERANCE);
    // get variable for last index
    const int lastIndex = (int)myEdge->getNBEdge()->getGeometry().size() - 1;
    // flag to enable/disable remove geometry point
    bool removeGeometryPoint = true;
    // obtain index
    const int index = myEdge->getNBEdge()->getGeometry().indexOfClosest(clickedPosition, true);
    // check index
    if (index == -1) {
        removeGeometryPoint = false;
    }
    // check distance
    if (shape[index].distanceSquaredTo2D(clickedPosition) > (geometryPointRadius * geometryPointRadius)) {
        removeGeometryPoint = false;
    }
    // check custom start position
    if (!customStartPosition && (index == 0)) {
        removeGeometryPoint = false;
    }
    // check custom end position
    if (!customEndPosition && (index == lastIndex)) {
        removeGeometryPoint = false;
    }
    // check if we can remove geometry point
    if (removeGeometryPoint) {
        // check if we're removing first geometry proint
        if (index == 0) {
            // commit new geometry start
            undoList->begin(myEdge, TLF("remove first geometry point of %", myEdge->getTagStr()));
            GNEChange_Attribute::changeAttribute(myEdge, GNE_ATTR_SHAPE_START, "", undoList);
            undoList->end();
        } else if (index == lastIndex) {
            // commit new geometry end
            undoList->begin(myEdge, TLF("remove last geometry point of %", myEdge->getTagStr()));
            GNEChange_Attribute::changeAttribute(myEdge, GNE_ATTR_SHAPE_END, "", undoList);
            undoList->end();
        } else {
            // remove geometry point
            shape.erase(shape.begin() + index);
            // get innen shape
            shape.pop_front();
            shape.pop_back();
            // remove double points
            shape.removeDoublePoints((geometryPointRadius * geometryPointRadius));
            // commit new shape
            undoList->begin(myEdge, TLF("remove geometry point of %", myEdge->getTagStr()));
            GNEChange_Attribute::changeAttribute(myEdge, SUMO_ATTR_SHAPE, toString(shape), undoList);
            undoList->end();
        }
    }
}


void
GNEMoveElementEdge::setMoveShape(const GNEMoveResult& moveResult) {
    // get start and end points
    const Position shapeStart = moveResult.shapeToUpdate.front();
    const Position shapeEnd = moveResult.shapeToUpdate.back();
    // get innen shape
    PositionVector innenShape = moveResult.shapeToUpdate;
    innenShape.pop_front();
    innenShape.pop_back();
    // set shape start
    if (std::find(moveResult.geometryPointsToMove.begin(), moveResult.geometryPointsToMove.end(), 0) != moveResult.geometryPointsToMove.end()) {
        myEdge->setShapeStartPos(shapeStart);
    }
    // set innen geometry
    myEdge->setGeometry(innenShape, true);
    // set shape end
    if (std::find(moveResult.geometryPointsToMove.begin(), moveResult.geometryPointsToMove.end(), ((int)moveResult.shapeToUpdate.size() - 1)) != moveResult.geometryPointsToMove.end()) {
        myEdge->setShapeEndPos(shapeEnd);
    }
}


void
GNEMoveElementEdge::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // make sure that newShape isn't empty
    if (moveResult.shapeToUpdate.size() > 1) {
        // get innen shape
        PositionVector innenShapeToUpdate = moveResult.shapeToUpdate;
        innenShapeToUpdate.pop_front();
        innenShapeToUpdate.pop_back();
        // commit new shape
        undoList->begin(myEdge, TLF("moving shape of %", myEdge->getTagStr()));
        // update start position
        if (std::find(moveResult.geometryPointsToMove.begin(), moveResult.geometryPointsToMove.end(), 0) != moveResult.geometryPointsToMove.end()) {
            GNEChange_Attribute::changeAttribute(myEdge, GNE_ATTR_SHAPE_START, toString(moveResult.shapeToUpdate.front()), undoList);
        }
        // check if update shape
        if ((innenShapeToUpdate.size() > 0) || (myEdge->getNBEdge()->getInnerGeometry().size() > 0)) {
            GNEChange_Attribute::changeAttribute(myEdge, SUMO_ATTR_SHAPE, toString(innenShapeToUpdate), undoList);
        }
        // update end position
        if (std::find(moveResult.geometryPointsToMove.begin(), moveResult.geometryPointsToMove.end(), ((int)moveResult.shapeToUpdate.size() - 1)) != moveResult.geometryPointsToMove.end()) {
            GNEChange_Attribute::changeAttribute(myEdge, GNE_ATTR_SHAPE_END, toString(moveResult.shapeToUpdate.back()), undoList);
        }
        undoList->end();
    }
}


double
GNEMoveElementEdge::getMoveEdgeOffset() const {
    const auto viewNet = myEdge->getNet()->getViewNet();
    const GNEEdge* clickedEdge = viewNet->getViewObjectsSelector().getEdgeFront();
    if (clickedEdge == nullptr) {
        return INVALID_DOUBLE;
    }
    const auto& clickedShape = clickedEdge->getNBEdge()->getGeometry();
    const double clickedOffset = clickedShape.nearest_offset_to_point2D(viewNet->getPositionInformation(), false);
    if (clickedOffset == GeomHelper::INVALID_OFFSET) {
        return INVALID_DOUBLE;
    }
    if (clickedEdge == myEdge) {
        return clickedOffset;
    }
    const double myLength = myEdge->getNBEdge()->getGeometry().length2D();
    const bool clickedFromSelected = clickedEdge->getFromJunction()->isAttributeCarrierSelected();
    const bool clickedToSelected = clickedEdge->getToJunction()->isAttributeCarrierSelected();
    const bool myFromSelected = myEdge->getFromJunction()->isAttributeCarrierSelected();
    const bool myToSelected = myEdge->getToJunction()->isAttributeCarrierSelected();
    if ((clickedFromSelected != clickedToSelected) && (myFromSelected != myToSelected)) {
        const double distFromSelected = clickedFromSelected ? clickedOffset : (clickedShape.length2D() - clickedOffset);
        return myFromSelected ? distFromSelected : (myLength - distFromSelected);
    } else if (clickedOffset <= myLength) {
        if (myEdge->isConvexAngle() == clickedEdge->isConvexAngle()) {
            return clickedOffset;
        } else {
            return myLength - clickedOffset;
        }
    } else {
        return INVALID_DOUBLE;
    }
}


GNEMoveOperation*
GNEMoveElementEdge::processMoveFromJunctionSelected(const double snapRadius) {
    const PositionVector& originalShape = myEdge->getNBEdge()->getGeometry();
    if (originalShape.size() < 2) {
        return nullptr;
    }
    PositionVector shapeToMove = originalShape;
    const int lastIndex = (int)originalShape.size() - 1;
    const bool onlyMoveCenter = myEdge->getNet()->getViewParent()->getMoveFrame()->getNetworkMoveOptions()->getMoveOnlyJunctionCenter();
    const int firstMovedIndex = onlyMoveCenter ? 1 : 0;
    const std::vector<int> originalIndexes = onlyMoveCenter ? std::vector<int>() : std::vector<int>({0});
    std::vector<int> indexes;
    // check if we're moving a selected edge or a selected junction
    if (myEdge->getNet()->getViewNet()->getMoveMultipleElementValues().isMovingSelectedEdge()) {
        const double offset = getMoveEdgeOffset();
        const double length = originalShape.length2D();
        if ((offset == INVALID_DOUBLE) || (offset <= 0)) {
            return nullptr;
        } else if (offset >= length) {
            if (originalShape.size() <= 2) {
                return nullptr;
            }
            for (int i = firstMovedIndex; i < lastIndex; i++) {
                indexes.push_back(i);
            }
            return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
        }
        const Position offsetPosition = originalShape.positionAtOffset2D(offset);
        const int nearestIndex = originalShape.indexOfClosest(offsetPosition);
        if ((nearestIndex == -1) || (offsetPosition == Position::INVALID)) {
            return nullptr;
        } else if (offsetPosition.distanceSquaredTo2D(shapeToMove[nearestIndex]) <= (snapRadius * snapRadius)) {
            if (nearestIndex == 0) {
                return nullptr;
            }
            const int endIndex = MIN2(nearestIndex, lastIndex - 1);
            if (endIndex < 1) {
                return nullptr;
            }
            for (int i = firstMovedIndex; i <= endIndex; i++) {
                indexes.push_back(i);
            }
            return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
        } else {
            const int newIndex = shapeToMove.insertAtClosest(offsetPosition, true);
            const int endIndex = MIN2(newIndex, (int)shapeToMove.size() - 2);
            if (endIndex < 1) {
                return nullptr;
            }
            for (int i = firstMovedIndex; i <= endIndex; i++) {
                indexes.push_back(i);
            }
            return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
        }
    } else if (originalShape.size() > 2) {
        // moving selected junction: move all existing inner geometry points
        for (int i = firstMovedIndex; i < lastIndex; i++) {
            indexes.push_back(i);
        }
        return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
    } else {
        return nullptr;
    }
}


GNEMoveOperation*
GNEMoveElementEdge::processMoveToJunctionSelected(const double snapRadius) {
    const PositionVector& originalShape = myEdge->getNBEdge()->getGeometry();
    if (originalShape.size() < 2) {
        return nullptr;
    }
    PositionVector shapeToMove = originalShape;
    const int lastIndex = (int)originalShape.size() - 1;
    const bool onlyMoveCenter = myEdge->getNet()->getViewParent()->getMoveFrame()->getNetworkMoveOptions()->getMoveOnlyJunctionCenter();
    const std::vector<int> originalIndexes = onlyMoveCenter ? std::vector<int>() : std::vector<int>({lastIndex});
    std::vector<int> indexes;
    // check if we're moving a selected edge or a selected junction
    if (myEdge->getNet()->getViewNet()->getMoveMultipleElementValues().isMovingSelectedEdge()) {
        const double offset = getMoveEdgeOffset();
        const double length = originalShape.length2D();
        if ((offset == INVALID_DOUBLE) || (offset >= length)) {
            return nullptr;
        } else if (offset <= 0) {
            if (originalShape.size() <= 2) {
                return nullptr;
            }
            const int endMovedIndex = onlyMoveCenter ? (lastIndex - 1) : lastIndex;
            for (int i = 1; i <= endMovedIndex; i++) {
                indexes.push_back(i);
            }
            return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
        }
        const Position offsetPosition = originalShape.positionAtOffset2D(offset);
        const int nearestIndex = originalShape.indexOfClosest(offsetPosition);
        if ((nearestIndex == -1) || (offsetPosition == Position::INVALID)) {
            return nullptr;
        } else if (offsetPosition.distanceSquaredTo2D(shapeToMove[nearestIndex]) <= (snapRadius * snapRadius)) {
            if (nearestIndex == lastIndex) {
                return nullptr;
            }
            const int startIndex = MAX2(1, nearestIndex);
            if (startIndex >= lastIndex) {
                return nullptr;
            }
            const int endMovedIndex = onlyMoveCenter ? (lastIndex - 1) : lastIndex;
            for (int i = startIndex; i <= endMovedIndex; i++) {
                indexes.push_back(i);
            }
            return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
        } else {
            const int newIndex = shapeToMove.insertAtClosest(offsetPosition, true);
            const int newLastIndex = (int)shapeToMove.size() - 1;
            const int startIndex = MAX2(1, newIndex);
            if (startIndex >= newLastIndex) {
                return nullptr;
            }
            const int endMovedIndex = onlyMoveCenter ? (newLastIndex - 1) : newLastIndex;
            for (int i = startIndex; i <= endMovedIndex; i++) {
                indexes.push_back(i);
            }
            return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
        }
    } else if (originalShape.size() > 2) {
        // moving selected junction: move all existing inner geometry points
        const int endMovedIndex = onlyMoveCenter ? (lastIndex - 1) : lastIndex;
        for (int i = 1; i <= endMovedIndex; i++) {
            indexes.push_back(i);
        }
        return new GNEMoveOperation(this, originalShape, originalIndexes, shapeToMove, indexes);
    } else {
        return nullptr;
    }
}


GNEMoveOperation*
GNEMoveElementEdge::processMoveBothJunctionSelected() {
    std::vector<int> geometryPointsToMove;
    for (int i = 0; i < (int)myEdge->getNBEdge()->getGeometry().size(); i++) {
        geometryPointsToMove.push_back(i);
    }
    // move entire shape (including extremes)
    return new GNEMoveOperation(this, myEdge->getNBEdge()->getGeometry(), geometryPointsToMove, myEdge->getNBEdge()->getGeometry(), geometryPointsToMove);
}


GNEMoveOperation*
GNEMoveElementEdge::processNoneJunctionSelected(const double snapRadius) {
    // declare shape to move
    PositionVector shapeToMove = myEdge->getNBEdge()->getGeometry();
    // obtain offset
    const double offset = getMoveEdgeOffset();
    if (offset == INVALID_DOUBLE) {
        return nullptr;
    }
    // obtain offset position
    const Position offsetPosition = myEdge->getNBEdge()->getGeometry().positionAtOffset2D(offset);
    // obtain nearest index to offset position
    const int nearestIndex = myEdge->getNBEdge()->getGeometry().indexOfClosest(offsetPosition);
    // check conditions
    if ((nearestIndex == -1) || (offsetPosition == Position::INVALID)) {
        return nullptr;
    } else if (offsetPosition.distanceSquaredTo2D(shapeToMove[nearestIndex]) <= (snapRadius * snapRadius)) {
        // move geometry point without creating new geometry point
        return new GNEMoveOperation(this, myEdge->getNBEdge()->getGeometry(), {nearestIndex}, shapeToMove, {nearestIndex});
    } else {
        // create new geometry point and keep new index
        const int newIndex = shapeToMove.insertAtClosest(offsetPosition, true);
        // move after setting new geometry point in shapeToMove
        return new GNEMoveOperation(this, myEdge->getNBEdge()->getGeometry(), {}, shapeToMove, {newIndex});
    }
}


/****************************************************************************/
