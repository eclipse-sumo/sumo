/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
#include <utils/xml/CommonXMLStructure.h>

#include "GNEAdditional.h"

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
class GNEPOI : public PointOfInterest, public GNEAdditional {

public:
    /// @brief needed to avoid diamond problem between PointOfInterest and GNEAdditional
    using GNEAdditional::getID;

    /// @brief Constructor
    GNEPOI(SumoXMLTag tag, GNENet* net);

    /**@brief Constructor
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
     */
    GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, const double xLon,
           const double yLat, const bool geo, const double layer, const double angle, const std::string& imgFile,
           const bool relativePath, const double width, const double height, const std::string& name,
           const Parameterised::Map& parameters);

    /**@brief Constructor
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
     */
    GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, GNELane* lane, const double posOverLane, const bool friendlyPos,
           const double posLat, const double layer, const double angle, const std::string& imgFile, const bool relativePath, const double width,
           const double height, const std::string& name, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEPOI();

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation() override;

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) override;

    /// @brief gererate a new ID for an element child
    std::string generateChildID(SumoXMLTag childTag);

    /// @brief get SUMOBaseObject with all POIattributes
    CommonXMLStructure::SumoBaseObject* getSumoBaseObject() const;

    /// @name inherited from GNEAdditional
    /// @{

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns position of shape in view
    Position getPositionInView() const override;

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const override;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid) override;

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement,
                           const GNENetworkElement* newElement, GNEUndoList* undoList) override;

    /**@brief write additional element into a xml file
    * @param[in] device device in which write parameters of additional element
    */
    void writeAdditional(OutputDevice& device) const override;

    /// @brief check if current additional is valid to be writed into XML (must be reimplemented in all detector children)
    bool isAdditionalValid() const override;

    /// @brief return a string with the current additional problem (must be reimplemented in all detector children)
    std::string getAdditionalProblem() const override;

    /// @brief fix additional problem (must be reimplemented in all detector children)
    void fixAdditionalProblem() override;

    /// @brief Returns the numerical id of the object
    GUIGlID getGlID() const;

    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    std::string getParentName() const override;

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) override;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const override;

    double getClickPriority() const override {
        return getShapeLayer();
    }
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /**@brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const override;

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const override;

    /**@brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /**@brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const override;

    /// @}

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;

protected:
    /// @brief shape width of POI
    PositionVector myShapeWidth;

    /// @brief shape height of POI
    PositionVector myShapeHeight;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief Invalidated copy constructor.
    GNEPOI(const GNEPOI&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPOI& operator=(const GNEPOI&) = delete;
};
