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
/// @file    GNEMoveElementPlanParent.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for elements that can be moved over a edge with two positions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

#include "GNEMoveElement.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEEdge;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveElementPlanParent : public GNEMoveElement {

public:
    /**@brief Constructor
     * @param[in] planParent moved planParent
     * @param[in] departPos Start position value
     * @param[in] departPosProcedure depart pos definition
     */
    GNEMoveElementPlanParent(GNEDemandElement* planParent, double& departPos, DepartPosDefinition& departPosProcedure);

    /// @brief Destructor
    ~GNEMoveElementPlanParent();

    /**@brief get edge movable move operation for elements with
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation();

    /// @name functions related with moving attributes
    /// @{

    /// @brief get moving attribute
    std::string getMovingAttribute(SumoXMLAttr key) const;

    /// @brief get moving attribute (double)
    double getMovingAttributeDouble(SumoXMLAttr key) const;

    /// @brief set moving attribute (using undo-list)
    void setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /// @brief check if the given moving attribute is valid
    bool isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const;

    /// @brief set moving attribute
    void setMovingAttribute(SumoXMLAttr key, const std::string& value);

    /// @}

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

private:
    /// @brief planParent
    GNEDemandElement* myPlanParent = nullptr;

    /// @brief departPos
    double& myDepartPos;

    /// @brief arrivalPos
    DepartPosDefinition& myDepartPosProcedure;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEMoveElementPlanParent(const GNEMoveElementPlanParent&) = delete;

    /// @brief Invalidated assignment operator
    GNEMoveElementPlanParent& operator=(const GNEMoveElementPlanParent& src) = delete;
};
