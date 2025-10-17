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
/// @date    Mar 2020
///
// Class used for elements that can be moved
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNEMoveFrame.h>

#include "GNEMoveElement.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElement::GNEMoveElement(GNEAttributeCarrier* movedElement) :
    myMovedElement(movedElement) {
}


GNEMoveElement::~GNEMoveElement() {}


std::string
GNEMoveElement::getMovingAttribute(SumoXMLAttr key) const {
    throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
}


double
GNEMoveElement::getMovingAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
}


void
GNEMoveElement::setMovingAttribute(SumoXMLAttr key, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
}


bool
GNEMoveElement::isMovingAttributeValid(SumoXMLAttr key, const std::string& /*value*/) const {
    throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
}


void
GNEMoveElement::setMovingAttribute(SumoXMLAttr key, const std::string& /*value*/) {
    throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
}


GNEMoveOperation*
GNEMoveElement::getEditShapeOperation(const GUIGlObject* obj, const PositionVector originalShape,
                                      const bool maintainShapeClosed) {
    // get moved geometry points
    const auto geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(obj);
    // get pos over shape
    const auto posOverShape = gViewObjectsHandler.getSelectedPositionOverShape(obj);
    // declare shape to move
    PositionVector shapeToMove = originalShape;
    const int lastIndex = (int)shapeToMove.size() - 1;
    // check if move existent geometry points or create new
    if (geometryPoints.size() > 0) {
        // move geometry point without creating new geometry point
        if (maintainShapeClosed && ((geometryPoints.front() == 0) || (geometryPoints.front() == lastIndex))) {
            // move first and last point
            return new GNEMoveOperation(this, originalShape, {0, lastIndex}, shapeToMove, {0, lastIndex});
        } else {
            return new GNEMoveOperation(this, originalShape, {geometryPoints.front()}, shapeToMove, {geometryPoints.front()});
        }
    } else if (posOverShape != Position::INVALID) {
        // create new geometry point and keep new index (if we clicked near of shape)
        const int newIndex = shapeToMove.insertAtClosest(posOverShape, true);
        return new GNEMoveOperation(this, originalShape, {shapeToMove.indexOfClosest(posOverShape)}, shapeToMove, {newIndex});
    } else {
        return nullptr;
    }
}


void
GNEMoveElement::moveElement(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset) {
    // declare move result
    GNEMoveResult moveResult(moveOperation);
    // set geometry points to move
    moveResult.geometryPointsToMove = moveOperation->geometryPointsToMove;
    // check if we're moving over a lane shape, an entire shape or only certain geometry point
    if (moveOperation->firstLane) {
        // calculate movement over lane depending if element has more than one lane
        if (moveOperation->lastLane) {
            if ((moveOperation->firstPosition != INVALID_DOUBLE) && (moveOperation->lastPosition != INVALID_DOUBLE)) {
                // move both first and last positions
                calculateLanePositions(moveResult.newFirstPos, moveResult.newLastPos, viewNet, moveOperation->firstLane,
                                       moveOperation->firstPosition, moveOperation->lastLane, moveOperation->lastPosition,
                                       moveOperation->clickedFirstLane, offset);
            } else if (moveOperation->firstPosition != INVALID_DOUBLE) {
                // move first position
                calculateLanePosition(moveResult.newFirstPos, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset);
            } else if (moveOperation->lastPosition != INVALID_DOUBLE) {
                // move last position
                calculateLanePosition(moveResult.newLastPos, viewNet, moveOperation->lastLane, moveOperation->lastPosition, offset);
            }
        } else {
            // continue depending if we're moving both positions
            if ((moveOperation->firstPosition != INVALID_DOUBLE) && (moveOperation->lastPosition != INVALID_DOUBLE)) {
                // move both first and last positions in the same lane
                calculateLanePositions(moveResult.newFirstPos, moveResult.newLastPos, viewNet, moveOperation->firstLane,
                                       moveOperation->firstPosition, moveOperation->lastPosition, offset);
            } else if (moveOperation->firstPosition != INVALID_DOUBLE) {
                // move first position
                calculateLanePosition(moveResult.newFirstPos, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset);
            } else if (moveOperation->lastPosition != INVALID_DOUBLE) {
                // move last position
                calculateLanePosition(moveResult.newLastPos, viewNet, moveOperation->firstLane, moveOperation->lastPosition, offset);
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLaneChange(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
            }
        }
    } else if (moveOperation->geometryPointsToMove.size() > 0) {
        // set values in moveResult
        moveResult.shapeToUpdate = moveOperation->shapeToMove;
        // move geometry points
        for (const auto& geometryPointIndex : moveOperation->geometryPointsToMove) {
            if (moveResult.shapeToUpdate[geometryPointIndex] != Position::INVALID) {
                // add offset
                moveResult.shapeToUpdate[geometryPointIndex].add(offset.x, offset.y, offset.z);
                // apply snap to active grid
                moveResult.shapeToUpdate[geometryPointIndex] = viewNet->snapToActiveGrid(moveResult.shapeToUpdate[geometryPointIndex]);
            } else {
                throw ProcessError("trying to move an invalid position");
            }
        }
    } else {
        // set values in moveResult
        moveResult.shapeToUpdate = moveOperation->shapeToMove;
        // move entire shape
        for (auto& geometryPointIndex : moveResult.shapeToUpdate) {
            if (geometryPointIndex != Position::INVALID) {
                // add offset
                geometryPointIndex.add(offset.x, offset.y, offset.z);
                // apply snap to active grid
                geometryPointIndex = viewNet->snapToActiveGrid(geometryPointIndex);
            } else {
                throw ProcessError("trying to move an invalid position");
            }
        }
        // check if we're adjusting width or height
        if ((moveOperation->operationType == GNEMoveOperation::OperationType::WIDTH) ||
                (moveOperation->operationType == GNEMoveOperation::OperationType::HEIGHT) ||
                (moveOperation->operationType == GNEMoveOperation::OperationType::LENGTH)) {
            // calculate extrapolate vector
            moveResult.shapeToUpdate = calculateExtrapolatedVector(moveOperation, moveResult);
        }
    }
    // move shape element
    moveOperation->moveElement->setMoveShape(moveResult);
}


void
GNEMoveElement::commitMove(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset, GNEUndoList* undoList) {
    // declare move result
    GNEMoveResult moveResult(moveOperation);
    // check if we're moving over a lane shape, an entire shape or only certain geometry point
    if (moveOperation->firstLane) {
        // calculate original move result
        moveResult.newFirstLane = moveOperation->firstLane;
        moveResult.newFirstPos = moveOperation->firstPosition;
        moveResult.newLastLane = moveOperation->lastLane;
        moveResult.newLastPos = moveOperation->lastPosition;
        // set original positions in element
        moveOperation->moveElement->setMoveShape(moveResult);
        // calculate movement over lane depending if element has more than one lane
        if (moveOperation->lastLane) {
            if ((moveOperation->firstPosition != INVALID_DOUBLE) && (moveOperation->lastPosition != INVALID_DOUBLE)) {
                // move both first and last positions
                calculateLanePositions(moveResult.newFirstPos, moveResult.newLastPos, viewNet, moveOperation->firstLane,
                                       moveOperation->firstPosition, moveOperation->lastLane, moveOperation->lastPosition,
                                       moveOperation->clickedFirstLane, offset);
            } else if (moveOperation->firstPosition != INVALID_DOUBLE) {
                // move first position
                calculateLanePosition(moveResult.newFirstPos, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset);
            } else if (moveOperation->lastPosition != INVALID_DOUBLE) {
                // move last position
                calculateLanePosition(moveResult.newLastPos, viewNet, moveOperation->lastLane, moveOperation->lastPosition, offset);
            }
        } else {
            // continue depending if we're moving both positions
            if ((moveOperation->firstPosition != INVALID_DOUBLE) && (moveOperation->lastPosition != INVALID_DOUBLE)) {
                // move both first and last positions in the same lane
                calculateLanePositions(moveResult.newFirstPos, moveResult.newLastPos, viewNet, moveOperation->firstLane,
                                       moveOperation->firstPosition, moveOperation->lastPosition, offset);
            } else if (moveOperation->firstPosition != INVALID_DOUBLE) {
                // move first position
                calculateLanePosition(moveResult.newFirstPos, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset);
            } else if (moveOperation->lastPosition != INVALID_DOUBLE) {
                // move last position
                calculateLanePosition(moveResult.newLastPos, viewNet, moveOperation->firstLane, moveOperation->lastPosition, offset);
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLaneChange(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
            }
        }
    } else {
        // set original geometry points to move
        moveResult.geometryPointsToMove = moveOperation->originalGeometryPoints;
        // set shapeToUpdate with originalPosOverLanes
        moveResult.shapeToUpdate = moveOperation->originalShape;
        // first restore original geometry geometry
        moveOperation->moveElement->setMoveShape(moveResult);
        // set new geometry points to move
        moveResult.geometryPointsToMove = moveOperation->geometryPointsToMove;
        // set values in moveResult
        moveResult.shapeToUpdate = moveOperation->shapeToMove;
        // check if we're moving an entire shape or only certain geometry point
        if (moveOperation->geometryPointsToMove.size() > 0) {
            // only move certain geometry points
            for (const auto& geometryPointIndex : moveOperation->geometryPointsToMove) {
                if (moveResult.shapeToUpdate[geometryPointIndex] != Position::INVALID) {
                    // add offset
                    moveResult.shapeToUpdate[geometryPointIndex].add(offset.x, offset.y, offset.z);
                    // apply snap to active grid
                    moveResult.shapeToUpdate[geometryPointIndex] = viewNet->snapToActiveGrid(moveResult.shapeToUpdate[geometryPointIndex]);
                } else {
                    throw ProcessError("trying to move an invalid position");
                }
            }
            // remove double points if merge points is enabled (only in commitMove)
            if (viewNet->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getMergeGeometryPoints() && (moveResult.shapeToUpdate.size() > 2)) {
                moveResult.shapeToUpdate.removeDoublePoints(2);
            }
        } else {
            // move entire shape
            for (auto& geometryPointIndex : moveResult.shapeToUpdate) {
                if (geometryPointIndex != Position::INVALID) {
                    // add offset
                    geometryPointIndex.add(offset.x, offset.y, offset.z);
                    // apply snap to active grid
                    geometryPointIndex = viewNet->snapToActiveGrid(geometryPointIndex);
                } else {
                    throw ProcessError("trying to move an invalid position");
                }
            }
            // check if we're adjusting width or height
            if ((moveOperation->operationType == GNEMoveOperation::OperationType::WIDTH) ||
                    (moveOperation->operationType == GNEMoveOperation::OperationType::HEIGHT) ||
                    (moveOperation->operationType == GNEMoveOperation::OperationType::LENGTH)) {
                // calculate extrapolate vector
                moveResult.shapeToUpdate = calculateExtrapolatedVector(moveOperation, moveResult);
            }
        }
    }
    // commit move shape
    moveOperation->moveElement->commitMoveShape(moveResult, undoList);
}


double
GNEMoveElement::calculateLaneOffset(const GNEViewNet* viewNet, const GNELane* lane, const double firstPosition, const double lastPosition,
                                    const GNEMoveOffset& offset) {
    // get lane shape lenght
    const auto laneShapeLength = lane->getLaneShape().length2D();
    // declare laneOffset
    double laneOffset = 0;
    // calculate central position between two given positions
    const double offsetCentralPosition = (firstPosition + lastPosition) * 0.5;
    // calculate middle length between two given positions
    const double middleLength = std::abs(lastPosition - firstPosition) * 0.5;
    // calculate lane position at offset given by offsetCentralPosition
    Position laneCentralPosition = lane->getLaneShape().positionAtOffset2D(offsetCentralPosition);
    // apply offset to positionAtCentralPosition
    laneCentralPosition.add(offset.x, offset.y, offset.z);
    // snap to grid
    laneCentralPosition = viewNet->snapToActiveGrid(laneCentralPosition);
    // calculate offset over lane using laneCentralPosition
    const double offsetLaneCentralPositionPerpendicular = lane->getLaneShape().nearest_offset_to_point2D(laneCentralPosition);
    // check if offset is within lane shape
    if (offsetLaneCentralPositionPerpendicular == -1) {
        // calculate non-perpendicular offset over lane using laneCentralPosition
        const double offsetLaneCentralPosition = lane->getLaneShape().nearest_offset_to_point2D(laneCentralPosition, false);
        // due laneCentralPosition is out of lane shape, then place positions in extremes
        if (offsetLaneCentralPosition == 0) {
            laneOffset = firstPosition;
        } else {
            laneOffset = lastPosition - laneShapeLength;
        }
    } else {
        // laneCentralPosition is within of lane shapen, then calculate offset using middlelength
        if ((offsetLaneCentralPositionPerpendicular - middleLength) < 0) {
            laneOffset = firstPosition;
        } else if ((offsetLaneCentralPositionPerpendicular + middleLength) > laneShapeLength) {
            laneOffset = lastPosition - laneShapeLength;
        } else {
            laneOffset = (offsetCentralPosition - offsetLaneCentralPositionPerpendicular);
        }
    }
    return laneOffset;
}


void
GNEMoveElement::calculateLanePosition(double& posOverLane, const GNEViewNet* viewNet, const GNELane* lane,
                                      const double lanePos, const GNEMoveOffset& offset) {
    // get lane offset
    const double laneOffset = calculateLaneOffset(viewNet, lane, lanePos, lanePos, offset);
    // update lane position
    posOverLane = (lanePos - laneOffset) / lane->getLengthGeometryFactor();
}


void
GNEMoveElement::calculateLanePositions(double& starPos, double& endPos, const GNEViewNet* viewNet, const GNELane* lane,
                                       const double firstPosOverLane, const double lastPosOverLane, const GNEMoveOffset& offset) {
    // get lane offset
    const double laneOffset = calculateLaneOffset(viewNet, lane, firstPosOverLane, lastPosOverLane, offset);
    // update moveResult
    starPos = (firstPosOverLane - laneOffset) / lane->getLengthGeometryFactor();
    endPos = (lastPosOverLane - laneOffset) / lane->getLengthGeometryFactor();
}


void
GNEMoveElement::calculateLanePositions(double& starPos, double& endPos, const GNEViewNet* viewNet, const GNELane* firstLane,
                                       const double firstPosOverLane, const GNELane* lastLane, const double lastPosOverLane,
                                       const bool firstLaneClicked, const GNEMoveOffset& offset) {
    // declare offset
    double laneOffset = 0;
    // calculate offset depending if we clicked over the first or over the second lane
    if (firstLaneClicked) {
        // calculate lane start position for first lane
        calculateLanePosition(starPos, viewNet, firstLane, firstPosOverLane, offset);
        // calculate offset
        laneOffset = (starPos - firstPosOverLane);
        // set end position
        endPos = lastPosOverLane + laneOffset;
        // adjust offset
        const double lastLaneLength = lastLane->getLaneShape().length2D();
        if (endPos > lastLaneLength) {
            laneOffset = (lastLaneLength - lastPosOverLane);
        }
        if (endPos < 0) {
            laneOffset = (0 - lastPosOverLane);
        }
    } else {
        // calculate lane start position for first lane
        calculateLanePosition(endPos, viewNet, lastLane, lastPosOverLane, offset);
        // calculate offset
        laneOffset = (endPos - lastPosOverLane);
        // set start position
        starPos = firstPosOverLane + laneOffset;
        // adjust offset
        const double firstLaneLength = firstLane->getLaneShape().length2D();
        if (starPos > firstLaneLength) {
            laneOffset = (firstLaneLength - firstPosOverLane);
        }
        if (starPos < 0) {
            laneOffset = (0 - firstPosOverLane);
        }
    }
    // set positions with the adjusted offset
    starPos = firstPosOverLane + laneOffset;
    endPos = lastPosOverLane + laneOffset;
}


void
GNEMoveElement::calculateNewLaneChange(const GNEViewNet* viewNet, const GNELane* originalLane, const GNELane*& newLane, double& laneOffset) {
    // get cursor position
    const Position cursorPosition = viewNet->getPositionInformation();
    // iterate over edge lanes
    for (const auto& lane : originalLane->getParentEdge()->getChildLanes()) {
        // avoid moveOperation lane
        if (lane != originalLane) {
            // calculate offset over lane shape
            const double offSet = lane->getLaneShape().nearest_offset_to_point2D(cursorPosition, true);
            // calculate position over lane shape
            const Position posOverLane = lane->getLaneShape().positionAtOffset2D(offSet);
            // check distance
            if (posOverLane.distanceSquaredTo2D(cursorPosition) < 1) {
                // update newlane
                newLane = lane;
                // calculate offset over moveOperation lane
                const double offsetMoveOperationLane = originalLane->getLaneShape().nearest_offset_to_point2D(cursorPosition, true);
                // calculate position over moveOperation lane
                const Position posOverMoveOperationLane = originalLane->getLaneShape().positionAtOffset2D(offsetMoveOperationLane);
                // update moveResult of laneOffset
                laneOffset = posOverLane.distanceTo2D(posOverMoveOperationLane);
                // change sign of  moveResult laneOffset depending of lane index
                if (originalLane->getIndex() < newLane->getIndex()) {
                    laneOffset *= -1;
                }
            }
        }
    }
}


PositionVector
GNEMoveElement::calculateExtrapolatedVector(const GNEMoveOperation* moveOperation, const GNEMoveResult& moveResult) {
    // get original shape half length
    const double halfLength = moveOperation->originalShape.length2D() * -0.5;
    // get original shape and extrapolate
    PositionVector extendedShape = moveOperation->originalShape;
    extendedShape.extrapolate2D(10e5);
    // get geometry point
    const Position geometryPoint = moveOperation->firstGeometryPoint ? moveResult.shapeToUpdate.front() : moveResult.shapeToUpdate.back();
    // calculate offsets to first and last positions
    const double offset = extendedShape.nearest_offset_to_point2D(geometryPoint, false);
    // calculate extrapolate value
    double extrapolateValue = (10e5 - offset);
    // adjust extrapolation
    if (moveOperation->firstGeometryPoint) {
        if (extrapolateValue < halfLength) {
            extrapolateValue = (halfLength - POSITION_EPS);
        }
    } else {
        if (extrapolateValue > halfLength) {
            extrapolateValue = (halfLength - POSITION_EPS);
        }
    }
    // restore shape in in moveResult
    PositionVector extrapolatedShape = moveOperation->shapeToMove;
    // extrapolate
    extrapolatedShape.extrapolate2D(extrapolateValue);
    // check if return reverse
    if (moveOperation->firstGeometryPoint) {
        return extrapolatedShape;
    } else {
        return extrapolatedShape.reverse();
    }
}

/****************************************************************************/
