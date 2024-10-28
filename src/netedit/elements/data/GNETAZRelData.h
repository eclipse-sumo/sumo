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
/// @file    GNETAZRelData.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for TAZ relation data
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/elements/GNEContour.h>

#include "GNEGenericData.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETAZRelData
 * @brief An Element which don't belong to GNENet but has influence in the simulation
 */
class GNETAZRelData : public GNEGenericData {

public:
    /**@brief Constructor for two TAZs
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] fromTAZ pointer to from TAZ
     * @param[in] toTAZ pointer to to TAZ
     * @param[in] parameters parameters map
     */
    GNETAZRelData(GNEDataInterval* dataIntervalParent, GNEAdditional* fromTAZ, GNEAdditional* toTAZ,
                  const Parameterised::Map& parameters);

    /**@brief Constructor for one TAZ
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] TAZ pointer to TAZ
     * @param[in] parameters parameters map
     */
    GNETAZRelData(GNEDataInterval* dataIntervalParent, GNEAdditional* TAZ,
                  const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNETAZRelData();

    /// @brief get TAZ rel data color
    RGBColor setColor(const GUIVisualizationSettings& s) const;
    double getColorValue(const GUIVisualizationSettings& s, int activeScheme) const;

    /// @brief check if current TAZ rel data is visible
    bool isGenericDataVisible() const;

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns element position in view
    Position getPositionInView() const;

    /// @name members and functions relative to write data sets into XML
    /// @{
    /**@brief write data set element into a xml file
     * @param[in] device device in which write parameters of data set element
     */
    void writeGenericData(OutputDevice& device) const;

    /// @brief check if current data set is valid to be written into XML (by default true, can be reimplemented in children)
    bool isGenericDataValid() const;

    /// @brief return a string with the current data set problem (by default empty, can be reimplemented in children)
    std::string getGenericDataProblem() const;

    /// @brief fix data set problem (by default throw an exception, has to be reimplemented in children)
    void fixGenericDataProblem();
    /// @}

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    //// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

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

    /// @brief get first path lane
    GNELane* getFirstPathLane() const;

    /// @brief get last path lane
    GNELane* getLastPathLane() const;
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

    /**@brief method for setting the attribute and letting the object perform data set changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

protected:
    /// @brief variable used for draw contours
    GNEContour myTAZRelDataContour;

    /// @brief Geometry for TAZRel data
    GUIGeometry myTAZRelGeometry;

    /// @brief Geometry for TAZRel data (center)
    GUIGeometry myTAZRelGeometryCenter;

    /// @brief TAZRel data width
    mutable double myLastWidth;

private:
    /// @brief check draw conditions
    bool drawTAZRel() const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief sets the color according to the current scheme index and some tazRel function
    bool setFunctionalColor(int activeScheme, RGBColor& col) const;

    /// @brief Invalidated copy constructor.
    GNETAZRelData(const GNETAZRelData&) = delete;

    /// @brief Invalidated assignment operator.
    GNETAZRelData& operator=(const GNETAZRelData&) = delete;
};

/****************************************************************************/
