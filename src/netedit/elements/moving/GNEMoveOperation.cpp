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
/// @file    GNEMoveOperation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for define move operation
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEMoveOperation.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const Position _originalPosition) :
    moveElement(_moveElement),
    originalShape({_originalPosition}),
              shapeToMove({_originalPosition}),
operationType(OperationType::POSITION) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const PositionVector _originalShape) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    shapeToMove(_originalShape),
    operationType(OperationType::ENTIRE_SHAPE) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const PositionVector _originalShape,
                                   const bool _firstGeometryPoint,
                                   const OperationType _operationType) :
    moveElement(_moveElement),
    originalShape(_originalShape),
    shapeToMove(_originalShape),
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
    operationType(OperationType::LANE) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const GNELane* _lane,
                                   const double _firstPosition,
                                   const double _lastPosition,
                                   const bool _allowChangeLane) :
    moveElement(_moveElement),
    firstLane(_lane),
    firstPosition((_firstPosition == INVALID_DOUBLE) ? INVALID_DOUBLE : _firstPosition * _lane->getLengthGeometryFactor()),
    lastPosition((_lastPosition == INVALID_DOUBLE) ? INVALID_DOUBLE : _lastPosition * _lane->getLengthGeometryFactor()),
    allowChangeLane(_allowChangeLane),
    operationType(OperationType::LANE) {
}


GNEMoveOperation::GNEMoveOperation(GNEMoveElement* _moveElement,
                                   const GNELane* _firstLane,
                                   const double _firstStartPos,
                                   const GNELane* _lastLane,
                                   const double _lastStartPos,
                                   const double _clickedFirstLane,
                                   const bool _allowChangeLane) :
    moveElement(_moveElement),
    firstLane(_firstLane),
    firstPosition((_firstStartPos != INVALID_DOUBLE) ? _firstStartPos * _firstLane->getLengthGeometryFactor() : INVALID_DOUBLE),
    lastLane(_lastLane),
    lastPosition((_lastStartPos != INVALID_DOUBLE) ? _lastStartPos * _lastLane->getLengthGeometryFactor() : INVALID_DOUBLE),
    clickedFirstLane(_clickedFirstLane),
    allowChangeLane(_allowChangeLane),
    operationType(OperationType::LANE) {
}


GNEMoveOperation::~GNEMoveOperation() {}

/****************************************************************************/
