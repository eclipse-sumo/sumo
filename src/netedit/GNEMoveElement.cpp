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
    shapeToMove({_originalPosition}) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement *_moveElement,
    const PositionVector _originalShape) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    clickedIndex(-1),
    shapeToMove(_originalShape) {
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
    geometryPointsToMove(_geometryPointsToMove) {
}

// ===========================================================================
// GNEMoveElement method definitions
// ===========================================================================

void 
GNEMoveElement::moveElement(GNEMoveOperation* moveOperation, const Position &offset) {
    // calculate new shape using shapeToMove
    PositionVector newShape = moveOperation->shapeToMove;
    // check if we're moving an entire shape or  only certain geometry point
    if (moveOperation->geometryPointsToMove.empty()) {
        newShape.add(offset);
    } else {
        // only move certain geometry points
        for (const auto &index : moveOperation->geometryPointsToMove) {
            newShape[index].add(offset);
        }
    }
    /*
        if (myNet->getViewNet()->getNetworkViewOptions().menuCheckMoveElevation->shown() && 
            (myNet->getViewNet()->getNetworkViewOptions().menuCheckMoveElevation->getCheck() == TRUE)) {
    */
    // move shape element
    moveOperation->moveElement->setMoveShape(newShape);
}


void 
GNEMoveElement::commitMove(GNEMoveOperation* moveOperation, const Position &offset, GNEUndoList* undoList) {
    // first restore original geometry geometry
    moveOperation->moveElement->setMoveShape(moveOperation->originalShape);
    // calculate new shape using shapeToMove
    PositionVector newShape = moveOperation->shapeToMove;
    // check if we're moving an entire shape or  only certain geometry point
    if (moveOperation->geometryPointsToMove.empty()) {
        newShape.add(offset);
    } else {
        // only move certain geometry points
        for (const auto &index : moveOperation->geometryPointsToMove) {
            newShape[index].add(offset);
        }
    }
    // commit move shape
    moveOperation->moveElement->commitMoveShape(newShape, undoList);
}

/****************************************************************************/
