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
    GNEOverheadWire(GNENet* net);

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
    GNEOverheadWire(const std::string& id, std::vector<GNELane*> lanes, GNEAdditional* substation, GNENet* net,
                    const double startPos, const double endPos, const bool friendlyPos,
                    const std::vector<std::string>& forbiddenInnerLanes, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEOverheadWire();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();

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

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const;

    /// @}

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @name inherited from GNEPathManager::PathElement
    /// @{

    /// @brief compute pathElement
    void computePathElement();

    /**@brief Draws partial object over lane
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment lane segment
     * @param[in] offsetFront front offset
     */
    void drawLanePartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const;

    /**@brief Draws partial object over junction
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment junction segment
     * @param[in] offsetFront front offset
     */
    void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const;

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

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

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

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;

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
