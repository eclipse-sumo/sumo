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
/// @file    GNEMoveResult.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for define move results
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveOperation.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveResult {

public:
    /// @brief constructor
    GNEMoveResult(const GNEMoveOperation* moveOperation);

    /// @brief destructor
    ~GNEMoveResult();

    /// @brief shape to update (edited in moveElement)
    PositionVector shapeToUpdate;

    /// @brief shape points to move (of shapeToMove)
    std::vector<int> geometryPointsToMove;

    /// @brief move operation
    const GNEMoveOperation::OperationType operationType;

    /// @brief lane offset
    double firstLaneOffset = 0;

    /// @brief new first Lane
    const GNELane* newFirstLane = nullptr;

    /// @brief new first position
    double newFirstPos = INVALID_DOUBLE;

    /// @brief lane offset
    double lastLaneOffset = 0;

    /// @brief new last Lane
    const GNELane* newLastLane = nullptr;

    /// @brief new last position
    double newLastPos = INVALID_DOUBLE;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveResult(const GNEMoveResult&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveResult& operator=(const GNEMoveResult&) = delete;
};
