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
/// @file    GNEMoveElementLaneSingle.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over a lane with only one position
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/iodevices/OutputDevice.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementLaneSingle : public GNEMoveElement {

public:
    /// @brief move element lane double need access to set and commit shape
    friend class GNEMoveElementLaneDouble;

    // declare type of moving
    struct PositionType {
        static const std::string SINGLE;    // Element has only one position
        static const std::string STARPOS;   // Element's start position
        static const std::string ENDPOS;    // Element's end position
    };

    /// @brief constructor
    GNEMoveElementLaneSingle(GNEAttributeCarrier* element, SumoXMLAttr posAttr,
                             double& position, bool& friendlyPos,
                             const std::string& defaultBehavior);

    //// @brief destructor
    ~GNEMoveElementLaneSingle();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();

    /// @name functions related with moving attributes
    /// @{

    /// @brief get moving attribute
    std::string getMovingAttribute(SumoXMLAttr key) const override;

    /// @brief get moving attribute double
    double getMovingAttributeDouble(SumoXMLAttr key) const override;

    /// @brief get moving attribute position
    Position getMovingAttributePosition(SumoXMLAttr key) const override;

    /// @brief get moving attribute positionVector
    PositionVector getMovingAttributePositionVector(SumoXMLAttr key) const override;

    /// @brief set moving attribute (using undo-list)
    void setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /// @brief check if the given moving attribute is valid
    bool isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const override;

    /// @brief set moving attribute
    void setMovingAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @}

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) override;

    /// @brief check if current moving element is valid to be written into XML
    bool isMoveElementValid() const;

    /// @brief return a string with the current moving problem
    std::string getMovingProblem() const;

    /// @brief fix moving problem
    void fixMovingProblem();

    /// @brief write move attributes
    void writeMoveAttributes(OutputDevice& device) const;

    /// @brief get fixed offset position over lane
    double getFixedPositionOverLane(const bool adjustGeometryFactor) const;

private:
    /// @brief pos attribute
    SumoXMLAttr myPosAttr;

    /// @brief position over lane
    double& myPosOverLane;

    /// @brief friendly position
    bool& myFriendlyPos;

    /// @brief default behavior
    const std::string myPositionType;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief Invalidated copy constructor.
    GNEMoveElementLaneSingle(const GNEMoveElementLaneSingle&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElementLaneSingle& operator=(const GNEMoveElementLaneSingle&) = delete;
};
