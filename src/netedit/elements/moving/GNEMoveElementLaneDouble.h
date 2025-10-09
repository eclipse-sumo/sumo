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
// class definitions
// ===========================================================================

class GNEMoveElementLaneDouble : public GNEMoveElement {

public:
    /**@brief Constructor
     * @param[in] element moved element
     * @param[in] lane Lane of this element belongs
     * @param[in] startPosAttr Start position attribute
     * @param[in] startPosValue Start position value
     * @param[in] endPosAttr End position attribute
     * @param[in] endPosValue End position value
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNEMoveElementLaneDouble(GNEAttributeCarrier* element, GNELane* lane,
                             SumoXMLAttr startPosAttr, double& startPosValue,
                             SumoXMLAttr endPosAttr, double& endPosValue,
                             bool& friendlyPosition);

    /**@brief Constructor
     * @param[in] element moved element
     * @param[in] lanes Lanes of this element belongs
     * @param[in] startPosAttr Start position attribute
     * @param[in] startPosValue Start position value
     * @param[in] endPosAttr End position attribute
     * @param[in] endPosValue End position value
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNEMoveElementLaneDouble(GNEAttributeCarrier* element, const std::vector<GNELane*>& lanes,
                             SumoXMLAttr startPosAttr, double& startPosValue,
                             SumoXMLAttr endPosAttr, double& endPosValue,
                             bool& friendlyPosition);

    /// @brief Destructor
    ~GNEMoveElementLaneDouble();

    /**@brief get lane movable move operation for elements with
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

    /// @name functions related with attributes
    /// @{

    /* @brief method for getting the moving attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getMovingAttribute(const Parameterised* parameterised, SumoXMLAttr key) const;

    /* @brief method for getting the moving attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getMovingAttributeDouble(SumoXMLAttr key) const;

    /* @brief method for setting the moving attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const;

    /// @brief method for setting the moving attribute and nothing else (used in GNEChange_Attribute)
    void setMovingAttribute(Parameterised* parameterised, SumoXMLAttr key, const std::string& value);

    /// @}

    /// @brief check if current moving element is valid to be written into XML
    bool isMoveElementValid() const;

    /// @brief return a string with the current moving problem
    std::string getMovingProblem() const;

    /// @brief fix moving problem
    void fixMovingProblem();

    /// @brief write move attributes
    void writeMoveAttributes(OutputDevice& device, const bool writeLength) const;

    /// @brief get start offset position over lane
    double getStartFixedPositionOverLane() const;

    /// @brief get end offset position over lane
    double getEndFixedPositionOverLane() const;

    /// @brief default element size
    static const double defaultSize;

private:
    /// @brief startPos attribute
    SumoXMLAttr myStartPosAttr;

    /// @brief The start position over lane
    double& myStartPosValue;

    /// @brief end pos attribute
    SumoXMLAttr myEndPosAttr;

    /// @brief The end position over lane
    double& myEndPosPosValue;

    /// @brief Flag for friendly position
    bool& myFriendlyPosition;

    /// @brief size (only use in AttributeCarrier templates)
    double myTemplateSize = defaultSize;

    /// @brief force size (only used in AttributeCarrier templates
    bool myTemplateForceSize = false;

    /// @brief reference position
    ReferencePosition myReferencePosition = ReferencePosition::CENTER;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief set size
    void setSize(const std::string& value, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEMoveElementLaneDouble(const GNEMoveElementLaneDouble&) = delete;

    /// @brief Invalidated assignment operator
    GNEMoveElementLaneDouble& operator=(const GNEMoveElementLaneDouble& src) = delete;
};
