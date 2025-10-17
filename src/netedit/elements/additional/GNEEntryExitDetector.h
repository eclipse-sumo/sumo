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
/// @file    GNEEntryExitDetector.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDetector.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEMoveElementLaneSingle;
class GNEMultiEntryExitDetector;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEEntryExitDetector : public GNEDetector {

public:
    /// @brief default Constructor
    GNEEntryExitDetector(SumoXMLTag entryExitTag, GNENet* net);

    /**@brief Constructor
     * @param[in] entryExitTag Child Tag (Either SUMO_TAG_DET_ENTRY or SUMO_TAG_DET_EXIT)
     * @param[in] parent pointer to GNEMultiEntryExitDetector of this GNEEntryExitDetector belongs
     * @param[in] lane Lane of this detector is placed
     * @param[in] pos position of the detector on the lane
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] parameters generic parameters
     */
    GNEEntryExitDetector(SumoXMLTag entryExitTag, GNEAdditional* parent, GNELane* lane, const double pos,
                         const bool friendlyPos, const Parameterised::Map& parameters);

    /// @brief destructor
    ~GNEEntryExitDetector();

    /// @brief methods to retrieve the elements linked to this GNEAdditional
    /// @{

    /// @brief get GNEMoveElement associated with this GNEAdditional
    GNEMoveElement* getMoveElement() const override;

    /// @}

    /// @name members and functions relative to write additionals into XML
    /// @{

    /**@brief write additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if current additional is valid to be written into XML
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem
    void fixAdditionalProblem();

    /// @}

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

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
    std::string getAttribute(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in double format
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /// @}

protected:
    /// @brief position over lane
    double myPosOverLane = 0;

    /// @brief friendly position
    bool myFriendlyPos = false;

    /// @brief move element lane single
    GNEMoveElementLaneSingle* myMoveElementLaneSingle = nullptr;

private:
    /// @brief draw body
    void drawBody(const GUIVisualizationSettings::Detail d, const RGBColor& color, const double exaggeration) const;

    /// @brief draw entry logo
    void drawEntryLogo(const GUIVisualizationSettings::Detail d, const RGBColor& color, const double exaggeration) const;

    /// @brief draw E3 logo
    void drawE3Logo(const GUIVisualizationSettings::Detail d, const RGBColor& color, const double exaggeration) const;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNEEntryExitDetector(const GNEEntryExitDetector&) = delete;

    /// @brief Invalidated assignment operator.
    GNEEntryExitDetector& operator=(const GNEEntryExitDetector&) = delete;
};
