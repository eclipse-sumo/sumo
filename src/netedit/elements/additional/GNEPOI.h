/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEPOI.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
/****************************************************************************/
#pragma once
#include <config.h>
#include <utils/shapes/PointOfInterest.h>

#include "GNEShape.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPOI
 *
 * In the case the represented junction's shape is empty, the boundary
 *  is computed using the junction's position to which an offset of 1m to each
 *  side is added.
 */
class GNEPOI : public PointOfInterest, public GNEShape {

public:
    /// @brief needed to avoid diamond problem between PointOfInterest and GNEShape
    using GNEShape::getID;

    /** @brief Constructor
     * @param[in] net net in which this polygon is placed
     * @param[in] id The name of the POI
     * @param[in] type The (abstract) type of the POI
     * @param[in] color The color of the POI
     * @param[in] lon The position X or Lon of the POI
     * @param[in] lat The position Y or Lat of the POI
     * @param[in[ geo use GEO coordinates (lon/lat)
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the shape
     * @param[in] relativePath set image file as relative path
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     * @param[in] name POI's name
     * @param[in] parameters generic parameters
     * @param[in] blockMovement enable or disable shape movement
     */
    GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, const double xLon,
           const double yLat, const bool geo, const double layer, const double angle, const std::string& imgFile, 
           const bool relativePath, const double width, const double height, const std::string &name, 
           const std::map<std::string, std::string> &parameters, const bool blockMovement);

    /** @brief Constructor
     * @param[in] net net in which this polygon is placed
     * @param[in] id The name of the POI
     * @param[in] type The (abstract) type of the POI
     * @param[in] color The color of the POI
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the shape
     * @param[in] relativePath set image file as relative path
     * @param[in] lane lane in which this POILane is placed
     * @param[in] posOverLane Position over lane in which this POILane is placed
     * @param[in] posLat Lateral position over lane
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     * @param[in] name POI's name
     * @param[in] parameters generic parameters
     * @param[in] movementBlocked if movement of POI is blocked
     */
    GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, GNELane* lane, const double posOverLane, const bool friendlyPos, 
           const double posLat, const double layer, const double angle, const std::string& imgFile, const bool relativePath, const double width, 
           const double height, const std::string &name, const std::map<std::string, std::string> &parameters, const bool movementBlocked);

    /// @brief Destructor
    ~GNEPOI();

    /**@brief get move operation for the given shapeOffset
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation(const double shapeOffset);

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

    /// @brief gererate a new ID for an element child
    std::string generateChildID(SumoXMLTag childTag);

    /**@brief Sets a parameter
     * @param[in] key The parameter's name
     * @param[in] value The parameter's value
     */
    void setParameter(const std::string& key, const std::string& value);

    /// @name inherited from GNEShape
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /**@brief writte shape element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeShape(OutputDevice& device);

    /// @brief Returns the numerical id of the object
    GUIGlID getGlID() const;

    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    std::string getParentName() const;

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /**@brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /**@brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /**@brief method for checking if the key and their correspond attribute are valids
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

    /// @brief get parameters map
    const std::map<std::string, std::string>& getACParametersMap() const;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEPOI(const GNEPOI&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPOI& operator=(const GNEPOI&) = delete;
};
