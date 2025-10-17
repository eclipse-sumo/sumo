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
/// @file    GNEMoveOperation.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for define move operation
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/StdDefs.h>
#include <utils/geom/PositionVector.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNELane;
class GNEMoveElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveOperation {

public:
    enum class OperationType {
        POSITION,
        ENTIRE_SHAPE,
        GEOMETRY_POINTS,
        WIDTH,
        HEIGHT,
        LENGTH,
        LANE
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
                     const double lastPosition,
                     const bool allowChangeLane);

    /// @brief constructor for elements placed over two lanes with two positions (E2 Multilane, vehicles..)
    GNEMoveOperation(GNEMoveElement* moveElement,
                     const GNELane* firstLane,
                     const double firstStartPos,
                     const GNELane* lastLane,
                     const double lastStartPos,
                     const double clickedFirstLane,
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

    /// @brief original last lane
    const GNELane* lastLane = nullptr;

    /// @brief original last Position
    const double lastPosition = INVALID_DOUBLE;

    /// @check if clicked the first lane or the last lane (used only in multilane elements like E2)
    const bool clickedFirstLane = false;

    /**@brief shape to move
     * @note: it can be different of originalShape, for example due a new geometry point
     */
    const PositionVector shapeToMove;

    /// @brief shape points to move (of shapeToMove)
    const std::vector<int> geometryPointsToMove;

    /// @brief allow change lane
    const bool allowChangeLane = false;

    /// @brief first position (used for edit with/height
    const bool firstGeometryPoint = false;

    /// @brief operation type
    const OperationType operationType;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveOperation(const GNEMoveOperation&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveOperation& operator=(const GNEMoveOperation&) = delete;
};
