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

#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEMoveOffset.h"
#include "GNEMoveResult.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;
class GNEUndoList;
class GNEViewNet;
class GUIGlObject;
class Parameterised;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElement(GNEAttributeCarrier* movedElement);

    //// @brief empty destructor
    virtual ~GNEMoveElement();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    virtual GNEMoveOperation* getMoveOperation() = 0;

    /// @name functions related with moving attributes
    /// @{

    /// @brief get moving attribute
    virtual std::string getMovingAttribute(SumoXMLAttr key) const = 0;

    /// @brief get moving attribute double
    virtual double getMovingAttributeDouble(SumoXMLAttr key) const = 0;

    /// @brief get moving attribute position
    virtual Position getMovingAttributePosition(SumoXMLAttr key) const = 0;

    /// @brief get moving attribute positionVector
    virtual PositionVector getMovingAttributePositionVector(SumoXMLAttr key) const = 0;

    /// @brief set moving attribute (using undo-list)
    virtual void setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /// @brief check if the given moving attribute is valid
    virtual bool isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const = 0;

    /// @brief set moving attribute
    virtual void setMovingAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @}

    /// @brief remove geometry point in the clicked position
    virtual void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) = 0;

    /// @brief move element the for given offset (note: offset can be X-Y-0, 0-0-Z or X-Y-Z)
    static void moveElement(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset);

    /// @brief commit move element for the given offset
    static void commitMove(const GNEViewNet* viewNet, GNEMoveOperation* moveOperation, const GNEMoveOffset& offset, GNEUndoList* undoList);

    /// @brief move element lateral offset
    double myMovingLateralOffset = 0;

protected:
    /// @brief pointer to element
    GNEAttributeCarrier* myMovedElement = nullptr;

    /// @brief calculate move shape operation
    GNEMoveOperation* getEditShapeOperation(const GUIGlObject* obj, const PositionVector originalShape, const bool maintainShapeClosed);

private:
    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief calculate lane offset (used in calculateLanePosition)
    static double calculateLaneOffset(const GNEViewNet* viewNet, const GNELane* lane, const double firstPosition,
                                      const double lastPosition, const GNEMoveOffset& offset);

    /// @brief calculate lane position over one lane with only one position (accesss, E1, star/end positions, etc.)
    static void calculateLanePosition(double& starPos, const GNEViewNet* viewNet, const GNELane* lane,
                                      const double posOverLane, const GNEMoveOffset& offset);

    /// @brief calculate lane position over one lane with two positions (stoppingPlaces, E2 single lanes)
    static void calculateLanePositions(double& starPos, double& endPos, const GNEViewNet* viewNet, const GNELane* lane,
                                       const double firstPosOverLane, const double lastPosOverLane, const GNEMoveOffset& offset);

    /// @brief calculate lane position over two lane with two positions (E2 Multilanes)
    static void calculateLanePositions(double& starPos, double& endPos, const GNEViewNet* viewNet, const GNELane* firstLane,
                                       const double firstPosOverLane, const GNELane* lastLane, const double lastPosOverLane,
                                       const bool firstLaneClicked, const GNEMoveOffset& offset);

    /// @brief calculate new lane change
    static void calculateNewLaneChange(const GNEViewNet* viewNet, const GNELane* originalLane, const GNELane*& newLane, double& laneOffset);

    /// @brief calculate width/height shape
    static PositionVector calculateExtrapolatedVector(const GNEMoveOperation* moveOperation, const GNEMoveResult& moveResult);

    /// @brief invalidate default constructor
    GNEMoveElement() = delete;

    /// @brief Invalidated copy constructor.
    GNEMoveElement(const GNEMoveElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElement& operator=(const GNEMoveElement&) = delete;
};
