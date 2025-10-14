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
/// @file    GNEMoveElementEdge.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving edge shapes
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementEdge : public GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElementEdge(GNEEdge* edge);

    //// @brief empty destructor
    ~GNEMoveElementEdge();

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

protected:
    /// @brief pointer to edge
    GNEEdge* myEdge = nullptr;

private:
    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief process moving edge when only from junction is selected
    GNEMoveOperation* processMoveFromJunctionSelected(const PositionVector originalShape, const Position mousePosition, const double snapRadius);

    /// @brief process moving edge when only to junction is selected
    GNEMoveOperation* processMoveToJunctionSelected(const PositionVector originalShape, const Position mousePosition, const double snapRadius);

    /// @brief process moving edge when both junctions are selected
    GNEMoveOperation* processMoveBothJunctionSelected();

    /// @brief process moving edge when none junction are selected
    GNEMoveOperation* processNoneJunctionSelected(const double snapRadius);

    /// @brief invalidate default constructor
    GNEMoveElementEdge() = delete;

    /// @brief Invalidated copy constructor.
    GNEMoveElementEdge(const GNEMoveElementEdge&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElementEdge& operator=(const GNEMoveElementEdge&) = delete;
};
