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
/// @file    GNEInstantInductionLoopDetector.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDetector.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElementLaneSingle;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEInstantInductionLoopDetector : public GNEDetector {

public:
    /// @brief default Constructor
    GNEInstantInductionLoopDetector(GNENet* net);

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] filename file in which this element is stored
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] filename The path to the output file.
     * @param[in] vehicleTypes space separated list of vehicle type ids to consider
     * @param[in] nextEdges list of edge ids that must all be part of the future route of the vehicle to qualify for detection
     * @param[in] detectPersons detect persons instead of vehicles (pedestrians or passengers)
     * @param[in] name E1 Instant detector name
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] parameters generic parameters
     */
    GNEInstantInductionLoopDetector(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane, const double pos,
                                    const std::string& outputFilename, const std::vector<std::string>& vehicleTypes,
                                    const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
                                    const bool friendlyPos, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEInstantInductionLoopDetector();

    /// @brief methods to retrieve the elements linked to this GNEAdditional
    /// @{

    /// @brief get GNEMoveElement associated with this GNEAdditional
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this GNEAdditional
    Parameterised* getParameters() override;

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

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @}

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
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNEInstantInductionLoopDetector(const GNEInstantInductionLoopDetector&) = delete;

    /// @brief Invalidated assignment operator.
    GNEInstantInductionLoopDetector& operator=(const GNEInstantInductionLoopDetector&) = delete;
};
