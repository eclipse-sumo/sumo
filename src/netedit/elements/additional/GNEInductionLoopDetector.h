/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEInductionLoopDetector.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEDetector.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEInductionLoopDetector
 * Class for detector of type E1
 */
class GNEInductionLoopDetector : public GNEDetector {

public:
    /// @brief default constructor
    GNEInductionLoopDetector(GNENet* net);

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] name E1 detector name
     * @param[in] vehicleTypes space separated list of vehicle type ids to consider
     * @param[in] nextEdges list of edge ids that must all be part of the future route of the vehicle to qualify for detection
     * @param[in] detectPersons detect persons instead of vehicles (pedestrians or passengers)
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] show detector in sumo-gui
     * @param[in] parameters generic parameters
     */
    GNEInductionLoopDetector(const std::string& id, GNELane* lane, GNENet* net, const double pos, const SUMOTime freq, const std::string& filename,
                             const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges, const std::string& detectPersons,
                             const std::string& name, const bool friendlyPos, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEInductionLoopDetector();

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
    void updateGeometry();

    /// @name Function related with drawing
    /// @{

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const;

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
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @}

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEInductionLoopDetector(const GNEInductionLoopDetector&) = delete;

    /// @brief Invalidated assignment operator.
    GNEInductionLoopDetector& operator=(const GNEInductionLoopDetector&) = delete;
};
