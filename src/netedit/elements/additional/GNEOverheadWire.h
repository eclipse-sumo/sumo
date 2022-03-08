/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEOverheadWire.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEAdditional.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEOverheadWire
 * class for overhead wires
 */
class GNEOverheadWire : public GNEAdditional, public Parameterised {

public:
    /// @brief default Constructor
    GNEOverheadWire(SumoXMLTag tag, GNENet* net);

    /**@brief Constructor for Multi-Lane detectors
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] id Overhead wire ID
     * @param[in] lanes vector of lanes Lane of this OverheadWire belongs
     * @param[in] lane Lane over which the segment is placed
     * @param[in] substationId Substation to which the circuit is connected
     * @param[in] laneIDs list of consecutive lanes of the circuit
     * @param[in] startPos Starting position in the specified lane
     * @param[in] endPos Ending position in the specified lane
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] forbiddenInnerLanes Inner lanes, where placing of overhead wire is restricted
     * @param[in] parameters generic parameters
     */
    GNEOverheadWire(const std::string& id, std::vector<GNELane*> lanes, GNEAdditional *substation, GNENet* net, 
                    const double startPos, const double endPos, const bool friendlyPos,
                    const std::vector<std::string>& forbiddenInnerLanes, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEOverheadWire();

    /// @name members and functions relative to write additionals into XML
    /// @{
    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if current additional is valid to be writed into XML
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem
    void fixAdditionalProblem();
    /// @}

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;
    /// @}

protected:
    /// @brief start position over lane
    double myStartPos;

    /// @brief end position over lane
    double myEndPos;

    /// @brief friendly position
    bool myFriendlyPosition;

    /// @brief forbidden inner lanes
    std::vector<std::string> myForbiddenInnerLanes;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief get start position over lane that is applicable to the shape
    double getStartGeometryPositionOverLane() const;

    /// @brief get end position over lane that is applicable to the shape
    double getEndGeometryPositionOverLane() const;

    /// @brief Invalidated copy constructor.
    GNEOverheadWire(const GNEOverheadWire&) = delete;

    /// @brief Invalidated assignment operator.
    GNEOverheadWire& operator=(const GNEOverheadWire&) = delete;
};
