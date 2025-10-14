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
/// @file    GNEMoveElementVehicle.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for elements that can be moved over a edge with two positions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/iodevices/OutputDevice.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEEdge;
class GNEVehicle;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementVehicle : public GNEMoveElement {

public:
    /**@brief Constructor
     * @param[in] vehicle moved vehicle
     * @param[in] fromEdge Edge of this element belongs
     * @param[in] departPos Start position value
     * @param[in] toEdge Edge of this element belongs
     * @param[in] arrivalPos End position value
     */
    GNEMoveElementVehicle(GNEVehicle* vehicle, GNEEdge* fromEdge, double& departPos,
                          GNEEdge* toEdge, double& arrivalPos);

    /// @brief Destructor
    ~GNEMoveElementVehicle();

    /**@brief get edge movable move operation for elements with
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

    /// @brief vehicle arrival position radius
    static const double arrivalPositionDiameter;

private:
    /// @brief vehicle
    GNEVehicle* myVehicle = nullptr;

    /// @brief departPos
    double& myDepartPos;

    /// @brief arrivalPos
    double& myArrivalPos;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief Invalidated copy constructor.
    GNEMoveElementVehicle(const GNEMoveElementVehicle&) = delete;

    /// @brief Invalidated assignment operator
    GNEMoveElementVehicle& operator=(const GNEMoveElementVehicle& src) = delete;
};
