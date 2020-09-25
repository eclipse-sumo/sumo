/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEMove.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for move shape elements
/****************************************************************************/
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElement.h"


// ===========================================================================
// GNEMoveOperation method definitions
// ===========================================================================

GNEMoveOperation::GNEMoveOperation(GNEMoveElement *_moveElement,
    const Position _originalPosition) :
    moveElement(_moveElement),
    originalShape({_originalPosition}),
    clickedIndex(-1),
    shapeToMove({_originalPosition}),
    lane(nullptr) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement *_moveElement,
    const PositionVector _originalShape) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    clickedIndex(-1),
    shapeToMove(_originalShape),
    lane(nullptr) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement *_moveElement,
    const PositionVector _originalShape,
    const PositionVector _shapeToMove,
    const int _clickedIndex,
    std::vector<int> _geometryPointsToMove) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    clickedIndex(_clickedIndex),
    shapeToMove(_shapeToMove),
    lane(nullptr), 
    geometryPointsToMove(_geometryPointsToMove) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement, 
    const GNELane* _lane, 
    const std::vector<double> _originalPosOverLanes) : 
    moveElement(_moveElement),
    clickedIndex(-1),
    lane(_lane), 
    originalPosOverLanes(_originalPosOverLanes) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
    const GNELane* _lane, 
    const std::vector<double> _originalPosOverLanes, 
    const int _clickedIndex, 
    const std::vector<int> _geometryPointsToMove) :
    moveElement(_moveElement),
    lane(_lane), 
    originalPosOverLanes(_originalPosOverLanes),
    clickedIndex(_clickedIndex),
    geometryPointsToMove(_geometryPointsToMove) {
}


GNEMoveOperation::~GNEMoveOperation() {}

// ===========================================================================
// GNEMoveResult method definitions
// ===========================================================================

GNEMoveResult::GNEMoveResult(GNEMoveOperation* moveOperation) :
    shapeToUpdate(moveOperation->shapeToMove),
    geometryPointsToMove(moveOperation->geometryPointsToMove) {
}


GNEMoveResult::~GNEMoveResult() {}

// ===========================================================================
// GNEMoveElement method definitions
// ===========================================================================

void 
GNEMoveElement::moveElement(GNEMoveOperation* moveOperation, const Position &offset) {
    // declare move result
    GNEMoveResult moveResult(moveOperation);
    // check if we're moving over a lane shape, an entire shape or only certain geometry point
    if (moveOperation->lane) {
        // calculate movement over lane
        moveResult.shapeToUpdate = calculateMovementOverLane(moveOperation, offset);
    } else if (moveOperation->geometryPointsToMove.empty()) {
        // move entire shape
        for (auto &geometryPointIndex : moveResult.shapeToUpdate) {
            if (geometryPointIndex != Position::INVALID) {
                geometryPointIndex.add(offset);
            } else {
                throw ProcessError("trying to move an invalid position");
            }
        }
    } else {
        // move geometry points
        for (const auto &geometryPointIndex : moveOperation->geometryPointsToMove) {
            if (moveResult.shapeToUpdate[geometryPointIndex] != Position::INVALID) {
                moveResult.shapeToUpdate[geometryPointIndex].add(offset);
            } else {
                throw ProcessError("trying to move an invalid position");
            }
        }
    }
    // move shape element
    moveOperation->moveElement->setMoveShape(moveResult);
}


void 
GNEMoveElement::commitMove(GNEMoveOperation* moveOperation, const Position &offset, GNEUndoList* undoList) {
    // declare move result
    GNEMoveResult moveResult(moveOperation);
    // check if we're moving over a lane shape, an entire shape or only certain geometry point
    if (moveOperation->lane) {
        // restore original position over lanes
        PositionVector originalPosOverLanes;
        for (const auto &posOverlane : moveOperation->originalPosOverLanes) {
            originalPosOverLanes.push_back(Position(posOverlane, 0));
        }
        // set shapeToUpdate with originalPosOverLanes
        moveResult.shapeToUpdate = originalPosOverLanes;
        // set originalPosOverLanes in element
        moveOperation->moveElement->setMoveShape(moveResult);
        // calculate movement over lane
        moveResult.shapeToUpdate = calculateMovementOverLane(moveOperation, offset);
    } else {
        // first restore original geometry geometry
        moveOperation->moveElement->setMoveShape(moveResult);
        // check if we're moving an entire shape or  only certain geometry point
        if (moveOperation->geometryPointsToMove.empty()) {
            // move entire shape
            for (auto &geometryPointIndex : moveResult.shapeToUpdate) {
                if (geometryPointIndex != Position::INVALID) {
                    geometryPointIndex.add(offset);
                } else {
                    throw ProcessError("trying to move an invalid position");
                }
            }
        } else {
            // only move certain geometry points
            for (const auto &geometryPointIndex : moveOperation->geometryPointsToMove) {
                if (moveResult.shapeToUpdate[geometryPointIndex] != Position::INVALID) {
                    moveResult.shapeToUpdate[geometryPointIndex].add(offset);
                } else {
                    throw ProcessError("trying to move an invalid position");
                }
            }
            // remove double points (only in commitMove)
            if (moveResult.shapeToUpdate.size() > 2) {
                moveResult.shapeToUpdate.removeDoublePoints(2);
            }
        }
    }
    // commit move shape
    moveOperation->moveElement->commitMoveShape(moveResult, undoList);
}


const PositionVector
GNEMoveElement::calculateMovementOverLane(const GNEMoveOperation* moveOperation, const Position &offset) {
    // declare new shape
    PositionVector newShape;
    // calculate lenght between pos over lanes
    const double centralPosition = (moveOperation->originalPosOverLanes.front() + moveOperation->originalPosOverLanes.back()) * 0.5;
    // calculate middle lenght between first and last pos over lanes
    const double middleLenght = std::abs(moveOperation->originalPosOverLanes.back() - moveOperation->originalPosOverLanes.front()) * 0.5;
    // get lane length
    const double laneLengt = moveOperation->lane->getParentEdge()->getNBEdge()->getFinalLength() * moveOperation->lane->getLengthGeometryFactor();
    // declare position over lane offset
    double posOverLaneOffset = 0;
    // calculate position at offset given by centralPosition
    Position lanePositionAtCentralPosition = moveOperation->lane->getLaneShape().positionAtOffset2D(centralPosition);
    // apply offset to positionAtCentralPosition
    lanePositionAtCentralPosition.add(offset);
    // calculate new posOverLane perpendicular
    const double newPosOverLanePerpendicular = moveOperation->lane->getLaneShape().nearest_offset_to_point2D(lanePositionAtCentralPosition);
    // calculate posOverLaneOffset
    if (newPosOverLanePerpendicular == -1) {
        // calculate new posOverLane non-perpendicular
        const double newPosOverLane = moveOperation->lane->getLaneShape().nearest_offset_to_point2D(lanePositionAtCentralPosition, false);
        // out of lane shape, then place element in lane extremes
        if (newPosOverLane == 0) {
            posOverLaneOffset = moveOperation->originalPosOverLanes.front();
        } else {
            posOverLaneOffset = moveOperation->originalPosOverLanes.back() - laneLengt;
        }
    } else {
        // within of lane shape
        if ((newPosOverLanePerpendicular - middleLenght) < 0) {
            posOverLaneOffset = moveOperation->originalPosOverLanes.front();
        } else if ((newPosOverLanePerpendicular + middleLenght) > laneLengt) {
            posOverLaneOffset = moveOperation->originalPosOverLanes.back() - laneLengt;
        } else {
            posOverLaneOffset = centralPosition - newPosOverLanePerpendicular;
        }
    }
    // apply posOverLaneOffset to all posOverLanes and generate new shape
    for (const auto &posOverlane : moveOperation->originalPosOverLanes) {
        newShape.push_back(Position(posOverlane - posOverLaneOffset, 0));
    }
    // return newShape
    return newShape;
}

/****************************************************************************/
