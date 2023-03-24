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
/// @file    GNEMoveElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for elements that own a movable shape
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/div/GUIGeometry.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNELane;
class GNEMoveElement;
class GNEUndoList;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief move operation
class GNEMoveOperation {

public:
    enum class OperationType {
        POSITION,
        ENTIRE_SHAPE,
        GEOMETRY_POINTS,
        WIDTH,
        HEIGHT,
        LENGTH,
        ONE_LANE,
        ONE_LANE_MOVEFIRST,
        ONE_LANE_MOVESECOND,
        ONE_LANE_MOVEBOTH,
        TWO_LANES_MOVEFIRST,
        TWO_LANES_MOVESECOND,
        TWO_LANES_MOVEBOTH_FIRST,
        TWO_LANES_MOVEBOTH_SECOND,
    };

    /// @brief constructor for values with a single position (junctions, E3, ParkingSpaces...)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const Position originalPosition);

    /// @brief constructor for entire geometries (Polygon with blocked shapes)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const PositionVector originalShape);

    /// @brief constructor for entire geometries (Polygon with blocked shapes)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const PositionVector originalShape,
                     const bool firstGeometryPoint,
                     const OperationType operationType);

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
                     const bool allowChangeLane,
                     const OperationType operationType);

    /// @brief constructor for elements placed over two lanes with two positions (E2 Multilane, vehicles..)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const GNELane* firstLane,
                     const double firstStartPos,
                     const GNELane* secondLane,
                     const double secondStartPos,
                     const bool allowChangeLane,
                     const OperationType operationType);

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

    /// @brief first position (used for edit with/height
    const bool firstGeometryPoint;

    /// @brief operation type
    const OperationType operationType;

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
    GNEMoveResult(const GNEMoveOperation* moveOperation);

    /// @brief destructor
    ~GNEMoveResult();

    /// @brief clear lanes
    void clearLanes();

    /// @brief shape to update (edited in moveElement)
    PositionVector shapeToUpdate;

    /// @brief shape points to move (of shapeToMove)
    std::vector<int> geometryPointsToMove;

    /// @brief move operation
    const GNEMoveOperation::OperationType operationType;

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

    //// @brief empty destructor
    virtual ~GNEMoveElement() {}

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    virtual GNEMoveOperation* getMoveOperation() = 0;

    /// @brief remove geometry point in the clicked position
    virtual void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) = 0;

    /// @brief move element the for given offset (note: offset can be X-Y-0, 0-0-Z or X-Y-Z)
    static void moveElement(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset);

    /// @brief commit move element for the given offset
    static void commitMove(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset, GNEUndoList* undoList);

protected:
    /// @brief move element lateral offset (used by elements placed over lanes
    double myMoveElementLateralOffset;

    /// @brief calculate move shape operation
    GNEMoveOperation* calculateMoveShapeOperation(const PositionVector originalShape, const Position mousePosition, const double snapRadius, const bool onlyContour);

private:
    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief calculate lane offset
    static double calculateLaneOffset(const GNEViewNet* viewNet, const GNELane* lane, const double firstPosition, const double secondPosition,
                                      const GNEMoveOffset& offset, const double extremFrom, const double extremTo);

    /// @brief calculate single movement over one lane
    static void calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane, const double pos,
                                    const GNEMoveOffset& offset, const double extremFrom, const double extremTo);

    /// @brief calculate double movement over one lane
    static void calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane, const double firstPos,
                                    const double secondPos, const GNEMoveOffset& offset);

    /// @brief calculate double movement over two lanes
    static void calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* firstLane, const double firstPos,
                                    const GNELane* secondLane, const double secondPos, const GNEMoveOffset& offset);

    /// @brief calculate new lane
    static void calculateNewLane(const GNEViewNet* viewNet, const GNELane* originalLane, const GNELane*& newLane, double& laneOffset);

    // @brief adjust both positions
    static void adjustBothPositions(const GNEViewNet* viewNet, const GNEMoveOperation* moveOperation, GNEMoveResult& moveResult, const GNEMoveOffset& offset);

    /// @brief calculate width/height shape
    static PositionVector calculateExtrapolatedVector(const GNEMoveOperation* moveOperation, const GNEMoveResult& moveResult);

    /// @brief Invalidated copy constructor.
    GNEMoveElement(const GNEMoveElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElement& operator=(const GNEMoveElement&) = delete;
};
