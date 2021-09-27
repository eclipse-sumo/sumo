/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveElement.h
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
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief move operation
class GNEMoveOperation {

public:
    /// @brief constructor for values with a single position (junctions, E3, ParkingSpaces...)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const Position originalPosition);

    /// @brief constructor for entire geometries (Polygon with blocked shapes)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const PositionVector originalShape);

    /// @brief constructor for elements with editable shapes (edges, polygons...)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const PositionVector originalShape,
                     const std::vector<int> originalgeometryPoints,
                     const PositionVector shapeToMove,
                     const std::vector<int> geometryPointsToMove);

    /// @brief constructor for elements placed over lanes with one position (detectors, vehicles...)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const GNELane* lane,
                     const double firstPosition,
                     const bool allowChangeLane);

    /// @brief constructor for elements placed over same lanes with two positions (StoppingPlaces)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const GNELane* lane,
                     const double firstPosition,
                     const double secondPosition,
                     const bool allowChangeLane);

    /// @brief constructor for elements placed over two lanes with two positions (E2 Multilane, vehicles..)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const GNELane* firstLane,
                     const double firstStartPos,
                     const GNELane* secondLane,
                     const double secondStartPos,
                     const bool allowChangeLane);

    /// @brief destructor
    ~GNEMoveOperation();

    /// @brief move element
    GNEMoveElement* moveElement;

    /// @brief original shape
    const PositionVector originalShape;

    /// @brief original shape points to move (of original shape)
    const std::vector<int> originalGeometryPoints;

    /// @brief original first lane
    const GNELane* firstLane = nullptr;

    /// @brief original first Position
    const double firstPosition = INVALID_DOUBLE;

    /// @brief original second lane
    const GNELane* secondLane = nullptr;

    /// @brief original second Position
    const double secondPosition = INVALID_DOUBLE;

    /**@brief shape to move
     * @note: it can be different of originalShape, for example due a new geometry point
     */
    const PositionVector shapeToMove;

    /// @brief shape points to move (of shapeToMove)
    const std::vector<int> geometryPointsToMove;

    /// @brief allow change lane
    const bool allowChangeLane;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveOperation(const GNEMoveOperation&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveOperation& operator=(const GNEMoveOperation&) = delete;
};

/// @brief move offset
class GNEMoveOffset {

public:
    /// @brief constructor
    GNEMoveOffset();

    /// @brief constructor for X-Y move
    GNEMoveOffset(const double x, const double y);

    /// @brief constructor for Z move
    GNEMoveOffset(const double z);

    /// @brief destructor
    ~GNEMoveOffset();

    /// @brief X
    const double x;

    /// @brief Y
    const double y;

    /// @brief Z
    const double z;
};

/// @brief move result
class GNEMoveResult {

public:
    /// @brief constructor
    GNEMoveResult();

    /// @brief destructor
    ~GNEMoveResult();

    /// @brief clear lanes
    void clearLanes();

    /// @brief shape to update (edited in moveElement)
    PositionVector shapeToUpdate;

    /// @brief shape points to move (of shapeToMove)
    std::vector<int> geometryPointsToMove;

    /// @brief lane offset
    double firstLaneOffset;

    /// @brief new first Lane
    const GNELane* newFirstLane;

    /// @brief new first position
    double newFirstPos;

    /// @brief lane offset
    double secondLaneOffset;

    /// @brief new second Lane
    const GNELane* newSecondLane;

    /// @brief new second position
    double newSecondPos;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveResult(const GNEMoveResult&) = delete;
};


/// @brief move element
class GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElement();

    /**@brief get move operation for the given shapeOffset
     * @note returned GNEMoveOperation can be nullptr
     */
    virtual GNEMoveOperation* getMoveOperation(const double shapeOffset) = 0;

    /// @brief remove geometry point in the clicked position
    virtual void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) = 0;

    /// @brief move element the for given offset (note: offset can be X-Y-0, 0-0-Z or X-Y-Z)
    static void moveElement(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset);

    /// @brief commit move element for the given offset
    static void commitMove(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset, GNEUndoList* undoList);

protected:
    /// @brief move element lateral offset (used by elements placed over lanes
    double myMoveElementLateralOffset;

private:
    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief calculate single movement over one lane
    static void calculateSingleMovementOverOneLane(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane, const double pos, const GNEMoveOffset& offset);

    /// @brief calculate double movement over one lane
    static void calculateDoubleMovementOverOneLane(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNEMoveOperation* moveOperation, const GNEMoveOffset& offset);

    /// @brief calculate double movement over two lanes
    static void calculateDoubleMovementOverTwoLanes(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNEMoveOperation* moveOperation, const GNEMoveOffset& offset);

    /// @brief calculate new lane
    static void calculateNewLane(const GNEViewNet* viewNet, const GNELane* originalLane, const GNELane*& newLane, double& laneOffset);

    /// @brief Invalidated copy constructor.
    GNEMoveElement(const GNEMoveElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElement& operator=(const GNEMoveElement&) = delete;
};
