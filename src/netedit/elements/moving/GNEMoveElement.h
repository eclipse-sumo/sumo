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
/// @file    GNEMoveElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for elements that can be moved
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveOffset.h"
#include "GNEMoveResult.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNELane;
class GNEMoveOperation;
class GNEUndoList;
class GNEViewNet;
class GUIGlObject;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief move element
class GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElement();

    //// @brief empty destructor
    virtual ~GNEMoveElement();

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

    /// @brief set move element lateral offset
    void setMoveElementLateralOffset(const double value);

protected:
    /// @brief move element lateral offset (used by elements placed over lanes
    double myMoveElementLateralOffset = 0;

    /// @brief calculate move shape operation
    GNEMoveOperation* calculateMoveShapeOperation(const GUIGlObject* obj, const PositionVector originalShape, const bool maintainShapeClosed);

private:
    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief calculate lane offset
    static double calculateLaneOffset(const GNEViewNet* viewNet, const GNELane* lane, const double firstPosition, const double lastPosition,
                                      const GNEMoveOffset& offset, const double extremFrom, const double extremTo);

    /// @brief calculate single movement over one lane
    static void calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane, const double pos,
                                    const GNEMoveOffset& offset, const double extremFrom, const double extremTo);

    /// @brief calculate double movement over one lane
    static void calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* lane, const double firstPos,
                                    const double lastPos, const GNEMoveOffset& offset);

    /// @brief calculate double movement over two lanes
    static void calculateMoveResult(GNEMoveResult& moveResult, const GNEViewNet* viewNet, const GNELane* firstLane, const double firstPos,
                                    const GNELane* lastLane, const double lastPos, const GNEMoveOffset& offset);

    /// @brief calculate new lane change
    static void calculateNewLaneChange(const GNEViewNet* viewNet, const GNELane* originalLane, const GNELane*& newLane, double& laneOffset);

    /// @brief calculate width/height shape
    static PositionVector calculateExtrapolatedVector(const GNEMoveOperation* moveOperation, const GNEMoveResult& moveResult);

    /// @brief Invalidated copy constructor.
    GNEMoveElement(const GNEMoveElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElement& operator=(const GNEMoveElement&) = delete;
};
