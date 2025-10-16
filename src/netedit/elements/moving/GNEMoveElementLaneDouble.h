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
/// @file    GNEMoveElementLaneDouble.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over a lane with two positions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/iodevices/OutputDevice.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElementLaneSingle;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementLaneDouble : public GNEMoveElement {

public:
    /**@brief Constructor
     * @param[in] element moved element
     * @param[in] startPosAttr Start position attribute
     * @param[in] startPosValue Start position value
     * @param[in] endPosAttr End position attribute
     * @param[in] endPosValue End position value
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNEMoveElementLaneDouble(GNEAttributeCarrier* element, SumoXMLAttr startPosAttr,
                             double& startPosValue, SumoXMLAttr endPosAttr,
                             double& endPosValue, bool& friendlyPosition);

    /// @brief Destructor
    ~GNEMoveElementLaneDouble();

    /**@brief get lane movable move operation for elements with
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
    void writeMoveAttributes(OutputDevice& device, const bool writeLength) const;

    /// @brief get start offset position over lane
    double getStartFixedPositionOverLane(const bool adjustGeometryFactor) const;

    /// @brief get end offset position over lane
    double getEndFixedPositionOverLane(const bool adjustGeometryFactor) const;

    /// @brief default element size
    static const double defaultSize;

private:
    /// @brief start position
    GNEMoveElementLaneSingle* myStartPos;

    /// @brief end position
    GNEMoveElementLaneSingle* myEndPos;

    /// @brief size (only use in AttributeCarrier templates)
    double myTemplateSize = defaultSize;

    /// @brief force size (only used in AttributeCarrier templates
    bool myTemplateForceSize = false;

    /// @brief reference position
    ReferencePosition myReferencePosition = ReferencePosition::CENTER;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief set size
    void setSize(const std::string& value, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEMoveElementLaneDouble(const GNEMoveElementLaneDouble&) = delete;

    /// @brief Invalidated assignment operator
    GNEMoveElementLaneDouble& operator=(const GNEMoveElementLaneDouble& src) = delete;
};
