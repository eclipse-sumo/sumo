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
/// @file    GNEMove.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for elements that own a movable shape
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEGeometry.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElement;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief move operation
class GNEMoveOperation {

public:
    /// @brief constructor for values with a single position
    GNEMoveOperation(GNEMoveElement *moveElement,
        const Position originalPosition);

    /// @brief constructor for entire geometries
    GNEMoveOperation(GNEMoveElement *moveElement, 
        const PositionVector originalShape);

    /// @brief constructor for elements with shape editierbar (edges, polygons...)
    GNEMoveOperation(GNEMoveElement *moveElement, 
        const PositionVector originalShape,
        const PositionVector shapeToMove, 
        const int _clickedIndex,
        const std::vector<int> geometryPointsToMove);

    /// @brief destructor
    ~GNEMoveOperation() {}

    /// @brief move element
    GNEMoveElement *moveElement;

    /// @brief original shape
    const PositionVector originalShape;

    /**@brief shape to move
     * @note: it can be different of originalShape, for example due a new geometry point)
     */
    const PositionVector shapeToMove;

    /// @brief clicked index (of shapeToMove)
    const int clickedIndex;

    /// @brief shape points to move (of shapeToMove)
    const std::vector<int> geometryPointsToMove;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveOperation(const GNEMoveOperation&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveOperation& operator=(const GNEMoveOperation&) = delete;
};


/// @brief move element
class GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElement() {}

    /**@brief get move operation for the given shapeOffset
     * @note returned GNEMoveOperation can be nullptr
     */
    virtual GNEMoveOperation* getMoveOperation(const double shapeOffset) = 0;

    /// @brief move element the for given offset
    static void moveElement(GNEMoveOperation* moveOperation, const Position &offset);

    /// @brief commit move element for the given offset
    static void commitMove(GNEMoveOperation* moveOperation, const Position &offset, GNEUndoList* undoList);

private:
    /// @brief set move shape
    virtual void setMoveShape(const PositionVector& newShape) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const PositionVector& newShape, GNEUndoList* undoList) = 0;

    /// @brief Invalidated copy constructor.
    GNEMoveElement(const GNEMoveElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElement& operator=(const GNEMoveElement&) = delete;
};
