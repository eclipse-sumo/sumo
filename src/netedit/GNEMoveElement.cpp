/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
// Class used for move shape elements
/****************************************************************************/
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNEMoveFrame.h>

#include "GNEMoveElement.h"


// ===========================================================================
// GNEMoveOperation method definitions
// ===========================================================================

GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const Position _originalPosition) :
    moveElement(_moveElement),
    originalShape({_originalPosition}),
              shapeToMove({_originalPosition}),
              allowChangeLane(false),
              firstGeometryPoint(false),
operationType(OperationType::POSITION) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const PositionVector _originalShape) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    shapeToMove(_originalShape),
    allowChangeLane(false),
    firstGeometryPoint(false),
    operationType(OperationType::ENTIRE_SHAPE) {
}

GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const PositionVector _originalShape,
                                   const bool _firstGeometryPoint,
                                   const OperationType _operationType) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    shapeToMove(_originalShape),
    allowChangeLane(false),
    firstGeometryPoint(_firstGeometryPoint),
    operationType(_operationType) {
}

GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const PositionVector _originalShape,
                                   const std::vector<int> _originalgeometryPoints,
                                   const PositionVector _shapeToMove,
                                   const std::vector<int> _geometryPointsToMove) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    originalGeometryPoints(_originalgeometryPoints),
    shapeToMove(_shapeToMove),
    geometryPointsToMove(_geometryPointsToMove),
    allowChangeLane(false),
    firstGeometryPoint(false),
    operationType(OperationType::GEOMETRY_POINTS) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const GNELane* _lane,
                                   const double _firstPosition,
                                   const bool _allowChangeLane) :
    moveElement(_moveElement),
    firstLane(_lane),
    firstPosition(_firstPosition * _lane->getLengthGeometryFactor()),
    allowChangeLane(_allowChangeLane),
    firstGeometryPoint(false),
    operationType(OperationType::ONE_LANE) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const GNELane* _lane,
                                   const double _firstPosition,
                                   const double _secondPosition,
                                   const bool _allowChangeLane,
                                   const OperationType _operationType) :
    moveElement(_moveElement),
    firstLane(_lane),
    firstPosition(_firstPosition * _lane->getLengthGeometryFactor()),
    secondPosition(_secondPosition * _lane->getLengthGeometryFactor()),
    allowChangeLane(_allowChangeLane),
    firstGeometryPoint(false),
    operationType(_operationType) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const GNELane* _firstLane,
                                   const double _firstStartPos,
                                   const GNELane* _secondLane,
                                   const double _secondStartPos,
                                   const bool _allowChangeLane,
                                   const OperationType _operationType) :
    moveElement(_moveElement),
    firstLane(_firstLane),
    firstPosition((_firstStartPos != INVALID_DOUBLE) ? _firstStartPos * _firstLane->getLengthGeometryFactor() : INVALID_DOUBLE),
    secondLane(_secondLane),
    secondPosition((_secondStartPos != INVALID_DOUBLE) ? _secondStartPos * _secondLane->getLengthGeometryFactor() : INVALID_DOUBLE),
    allowChangeLane(_allowChangeLane),
    firstGeometryPoint(false),
    operationType(_operationType) {
}


GNEMoveOperation::~GNEMoveOperation() {}

// ===========================================================================
// GNEMoveOffset method definitions
// ===========================================================================

GNEMoveOffset::GNEMoveOffset() :
    x(0),
    y(0),
    z(0) {
}


GNEMoveOffset::GNEMoveOffset(const double x_, const double y_) :
    x(x_),
    y(y_),
    z(0) {
}


GNEMoveOffset::GNEMoveOffset(const double z_) :
    x(0),
    y(0),
    z(z_) {
}


GNEMoveOffset::~GNEMoveOffset() {}

// ===========================================================================
// GNEMoveResult method definitions
// ===========================================================================

GNEMoveResult::GNEMoveResult(const GNEMoveOperation* moveOperation) :
    operationType(moveOperation->operationType),
    firstLaneOffset(0),
    newFirstLane(nullptr),
    newFirstPos(0),
    secondLaneOffset(0),
    newSecondLane(nullptr),
    newSecondPos(0) {}


GNEMoveResult::~GNEMoveResult() {}


void
GNEMoveResult::clearLanes() {
    firstLaneOffset = 0;
    newFirstLane = nullptr;
    secondLaneOffset = 0;
    newSecondLane = nullptr;
}

// ===========================================================================
// GNEMoveElement method definitions
// ===========================================================================

GNEMoveElement::GNEMoveElement() :
    myMoveElementLateralOffset(0) {
}


GNEMoveOperation*
GNEMoveElement::calculateMoveShapeOperation(const PositionVector originalShape, const Position mousePosition, const double snapRadius, const bool onlyContour) {
    // calculate squared snapRadius
    const double squaredSnapRadius = (snapRadius * snapRadius);
    // declare shape to move
    PositionVector shapeToMove = originalShape;
    // obtain nearest index
    const int nearestIndex = originalShape.indexOfClosest(mousePosition);
    // obtain nearest position
    const Position nearestPosition = originalShape.positionAtOffset2D(originalShape.nearest_offset_to_point2D(mousePosition));
    // check conditions
    if (nearestIndex == -1) {
        return nullptr;
    } else if (nearestPosition == Position::INVALID) {
        // special case for extremes
        if (mousePosition.distanceSquaredTo2D(shapeToMove[nearestIndex]) <= squaredSnapRadius) {
            // move extrem without creating new geometry point
            return new GNEMoveOperation(this, originalShape, {nearestIndex}, shapeToMove, {nearestIndex});
        } else {
            return nullptr;
        }
    } else if (mousePosition.distanceSquaredTo2D(shapeToMove[nearestIndex]) <= squaredSnapRadius) {
        // move geometry point without creating new geometry point
        return new GNEMoveOperation(this, originalShape, {nearestIndex}, shapeToMove, {nearestIndex});
    } else if (!onlyContour || nearestPosition.distanceSquaredTo2D(mousePosition) <= squaredSnapRadius) {
        // create new geometry point and keep new index (if we clicked near of shape)
        const int newIndex = shapeToMove.insertAtClosest(nearestPosition, true);
        // move after setting new geometry point in shapeToMove
        return new GNEMoveOperation(this, originalShape, {nearestIndex}, shapeToMove, {newIndex});
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
        // calculate movement over lane
        if (moveOperation->secondLane) {
            // continue depending of operationType
            if (moveOperation->operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST) {
                // move only first position
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset,
                                    0, moveOperation->firstLane->getLaneShapeLength());
            } else if (moveOperation->operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND) {
                // move only second position
                calculateMoveResult(moveResult, viewNet, moveOperation->secondLane, moveOperation->secondPosition, offset,
                                    0, moveOperation->secondLane->getLaneShapeLength());
            } else {
                // adjust positions
                adjustBothPositions(viewNet, moveOperation, moveResult, offset);
            }
        } else {
            if (moveOperation->operationType == GNEMoveOperation::OperationType::ONE_LANE) {
                // move first position around the entire lane
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset,
                                    0, moveOperation->firstLane->getLaneShapeLength());
            } else if (moveOperation->operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) {
                // move first position around [0, secondPosition]
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset,
                                    0, moveOperation->secondPosition);
            } else if (moveOperation->operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) {
                // move first position around [firstPosition, laneLength]
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->secondPosition, offset,
                                    moveOperation->firstPosition, moveOperation->firstLane->getLaneShapeLength());
            } else {
                // move both first and second positions
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition,
                                    moveOperation->secondPosition, offset);
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLane(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
            } else {
                moveResult.clearLanes();
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
        moveResult.newSecondLane = moveOperation->secondLane;
        moveResult.newSecondPos = moveOperation->secondPosition;
        // set original positions in element
        moveOperation->moveElement->setMoveShape(moveResult);
        // calculate movement over lane
        if (moveOperation->secondLane) {
            if (moveOperation->operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST) {
                // move only first position
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset,
                                    0, moveOperation->firstLane->getLaneShapeLength());
            } else if (moveOperation->operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND) {
                // move only two position
                calculateMoveResult(moveResult, viewNet, moveOperation->secondLane, moveOperation->secondPosition, offset,
                                    0, moveOperation->secondLane->getLaneShapeLength());
            } else {
                // adjust positions
                adjustBothPositions(viewNet, moveOperation, moveResult, offset);
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLane(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
                calculateNewLane(viewNet, moveOperation->secondLane, moveResult.newSecondLane, moveResult.secondLaneOffset);
            } else {
                moveResult.clearLanes();
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLane(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
                calculateNewLane(viewNet, moveOperation->secondLane, moveResult.newSecondLane, moveResult.secondLaneOffset);
            } else {
                moveResult.clearLanes();
            }
        } else {
            if (moveOperation->operationType == GNEMoveOperation::OperationType::ONE_LANE) {
                // move first position around the entire lane
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset,
                                    0, moveOperation->firstLane->getLaneShapeLength());
            } else if (moveOperation->operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) {
                // move first position around [0, secondPosition]
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset,
                                    0, moveOperation->secondPosition);
            } else if (moveOperation->operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) {
                // move first position around [firstPosition, laneLength]
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->secondPosition, offset,
                                    moveOperation->firstPosition, moveOperation->firstLane->getLaneShapeLength());
            } else {
                // move both first and second positions
                calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition,
                                    moveOperation->secondPosition, offset);
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLane(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
            } else {
                moveResult.clearLanes();
            }
            // calculate new lane
            if (moveOperation->allowChangeLane) {
                calculateNewLane(viewNet, moveOperation->firstLane, moveResult.newFirstLane, moveResult.firstLaneOffset);
            } else {
                moveResult.clearLanes();
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
            if (viewNet->getViewParent()->getMoveFrame()->getCommonModeOptions()->getMergeGeometryPoints() && (moveResult.shapeToUpdate.size() > 2)) {
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
GNEMoveElement::calculateLaneOffset(const GNEViewNet* viewNet, const GNELane* lane, const double firstPosition, const double secondPosition,
                                    const GNEMoveOffset& offset, const double extremFrom, const double extremTo) {
    // declare laneOffset
    double laneOffset = 0;
    // calculate central position between two given positions
    const double offsetCentralPosition = (firstPosition + secondPosition) * 0.5;
    // calculate middle length between two given positions
    const double middleLength = std::abs(secondPosition - firstPosition) * 0.5;
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
            laneOffset = secondPosition - lane->getLaneShape().length2D();
        }
    } else {
        // laneCentralPosition is within of lane shapen, then calculate offset using middlelength
        if ((offsetLaneCentralPositionPerpendicular - middleLength) < extremFrom) {
            laneOffset = firstPosition + extremFrom;
        } else if ((offsetLaneCentralPositionPerpendicular + middleLength) > extremTo) {
            laneOffset = secondPosition - extremTo;
        } else {
            laneOffset = (offsetCentralPosition - offsetLaneCentralPositionPerpendicular);
        }
    }
    return laneOffset;
}


void
GNEMoveElement::calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane,
                                    const double pos, const GNEMoveOffset& offset, const double extremFrom, const double extremTo) {
    // get lane offset
    const double laneOffset = calculateLaneOffset(viewNet, lane, pos, pos, offset, extremFrom, extremTo);
    // update moveResult
    moveResult.newFirstPos = (pos - laneOffset) / lane->getLengthGeometryFactor();
    moveResult.newSecondPos = 0;
}


void
GNEMoveElement::calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane,
                                    const double firstPos, const double secondPos, const GNEMoveOffset& offset) {
    // get lane offset
    const double laneOffset = calculateLaneOffset(viewNet, lane, firstPos, secondPos, offset, 0, lane->getLaneShape().length2D());
    // update moveResult
    moveResult.newFirstPos = (firstPos - laneOffset) / lane->getLengthGeometryFactor();
    moveResult.newSecondPos = (secondPos - laneOffset) / lane->getLengthGeometryFactor();
}


void
GNEMoveElement::calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* firstLane,
                                    const double firstPos, const GNELane* secondLane, const double secondPos, const GNEMoveOffset& offset) {
    // get lane offset of the first lane
    const double laneOffset = calculateLaneOffset(viewNet, firstLane, firstPos, firstPos, offset, secondLane->getLaneShape().length2D() - firstPos, firstLane->getLaneShape().length2D());
    // update moveResult
    moveResult.newFirstPos = (firstPos - laneOffset) / firstLane->getLengthGeometryFactor();
    moveResult.newSecondPos = (secondPos - laneOffset) / firstLane->getLengthGeometryFactor();
}


void
GNEMoveElement::calculateNewLane(const GNEViewNet* viewNet, const GNELane* originalLane, const GNELane*& newLane, double& laneOffset) {
    // get cursor position
    const Position cursorPosition = viewNet->getPositionInformation();
    // iterate over edge lanes
    for (const auto& lane : originalLane->getParentEdge()->getLanes()) {
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


void
GNEMoveElement::adjustBothPositions(const GNEViewNet* viewNet, const GNEMoveOperation* moveOperation, GNEMoveResult& moveResult, const GNEMoveOffset& offset) {
    // get lane shape lengths
    const double firstLaneLength = moveOperation->firstLane->getLaneShapeLength();
    const double secondLaneLength = moveOperation->secondLane->getLaneShapeLength();
    if (moveOperation->operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEBOTH_FIRST) {
        // move only first position
        calculateMoveResult(moveResult, viewNet, moveOperation->firstLane, moveOperation->firstPosition, offset, 0, firstLaneLength);
        // calculate second position
        moveResult.newSecondPos = (moveOperation->secondPosition - (moveOperation->firstPosition - moveResult.newFirstPos));
        // adjust positions
        if (moveResult.newSecondPos < 0) {
            moveResult.newFirstPos = (moveOperation->firstPosition - moveOperation->secondPosition);
            moveResult.newSecondPos = 0;
        } else if (moveResult.newSecondPos > secondLaneLength) {
            moveResult.newFirstPos = (moveOperation->firstPosition + (secondLaneLength - moveOperation->secondPosition));
            moveResult.newSecondPos = secondLaneLength;
        }
    } else if (moveOperation->operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEBOTH_SECOND) {
        // move only second position
        calculateMoveResult(moveResult, viewNet, moveOperation->secondLane, moveOperation->secondPosition, offset, 0, secondLaneLength);
        // swap (because move results is always stored in newFirstPos)
        moveResult.newSecondPos = moveResult.newFirstPos;
        moveResult.newFirstPos = 0;
        // calculate first position
        moveResult.newFirstPos = (moveOperation->firstPosition - (moveOperation->secondPosition - moveResult.newSecondPos));
        // adjust positions
        if (moveResult.newFirstPos < 0) {
            moveResult.newSecondPos = (moveOperation->secondPosition - moveOperation->firstPosition);
            moveResult.newFirstPos = 0;
        } else if (moveResult.newFirstPos > firstLaneLength) {
            moveResult.newSecondPos = (moveOperation->secondPosition + (firstLaneLength - moveOperation->firstPosition));
            moveResult.newFirstPos = firstLaneLength;
        }
    } else {
        throw ProcessError("Invalid move operationType");
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
