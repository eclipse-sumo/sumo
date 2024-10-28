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
/// @file    GNEParkingSpace.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// A class for visualizing ParkingSpace geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEParkingArea;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEParkingSpace
 * @brief vehicle space used by GNEParkingAreas
 */
class GNEParkingSpace : public GNEAdditional, public Parameterised {

public:
    /// @brief Constructor
    GNEParkingSpace(GNENet* net);

    /**@brief Constructor
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] parkingAreaParent pointer to Parking Area parent
     * @param[in] pos position X-Y-Z
     * @param[in] width ParkingArea's width
     * @param[in] length ParkingArea's length
     * @param[in] angle ParkingArea's angle
     * @param[in] slope ParkingArea's slope (of this space)
     * @param[in] name ParkingArea's name
     * @param[in] parameters generic parameters
     */
    GNEParkingSpace(GNENet* net, GNEAdditional* parkingAreaParent, const Position& pos, const std::string& width,
                    const std::string& length, const std::string& angle, double slope, const std::string& name,
                    const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEParkingSpace();

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

    /// @brief check if current additional is valid to be written into XML (must be reimplemented in all detector children)
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem (must be reimplemented in all detector children)
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem (must be reimplemented in all detector children)
    void fixAdditionalProblem();

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const;

    /// @}

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /// @}

    /// @name inherited from GUIGlObject
    /// @{

    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    std::string getParentName() const;

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
    /// @brief position of Parking Space in view
    Position myPosition;

    /// @brief width of Parking Space
    std::string myWidth;

    /// @brief Length of Parking Space
    std::string myLength;

    /// @brief shape width of Parking Space
    PositionVector myShapeWidth;

    /// @brief shape length of Parking Space
    PositionVector myShapeLength;

    /// @brief Angle of Parking Space
    std::string myAngle;

    /// @brief Slope of Parking Space
    double mySlope;

    /// @brief variable used for moving contour up
    GNEContour myMovingContourUp;

    /// @brief variable used for moving contour down
    GNEContour myMovingContourDown;

    /// @brief variable used for moving contour left
    GNEContour myMovingContourLeft;

    /// @brief variable used for moving contour right
    GNEContour myMovingContourRight;

private:
    /// @brief draw space
    void drawSpace(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                   const double width, const bool movingGeometryPoints) const;

    /// @brief calculate space contour
    void calculateSpaceContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const double width, const double exaggeration, const bool movingGeometryPoints) const;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEParkingSpace(const GNEParkingSpace&) = delete;

    /// @brief Invalidated assignment operator.
    GNEParkingSpace& operator=(const GNEParkingSpace&) = delete;
};
