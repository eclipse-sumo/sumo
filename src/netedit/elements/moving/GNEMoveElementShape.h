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
/// @file    GNEMoveElementShape.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for shape elements that can be moved over view
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementShape : public GNEMoveElement {

public:
    /// @brief constructor
    GNEMoveElementShape(GNEAttributeCarrier* element);

    /// @brief constructor
    GNEMoveElementShape(GNEAttributeCarrier* element, PositionVector& shape, const bool alwaysClosed);

    /// @brief constructor
    GNEMoveElementShape(GNEAttributeCarrier* element, PositionVector& shape, const Position& position, const bool alwaysClosed);

    //// @brief empty destructor
    ~GNEMoveElementShape();

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

    /// @brief get moving attribute position vector
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


//protected:
    /// @brief center position
    Position myCenterPosition;

private:
    /// @brief reference to Shape
    PositionVector& myMovingShape;

    /// @brief empty position vector
    static PositionVector EMPTY_SHAPE;

    /// @brief check if shape must be always closed
    const bool myAlwaysClosed = false;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief Invalidated copy constructor.
    GNEMoveElementShape(const GNEMoveElementShape&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElementShape& operator=(const GNEMoveElementShape&) = delete;
};
