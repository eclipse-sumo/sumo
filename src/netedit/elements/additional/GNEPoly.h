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
/// @file    GNEPoly.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPolygon and NLHandler)
/****************************************************************************/
#pragma once
#include <config.h>
#include <utils/gui/globjects/GUIPolygon.h>

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GeoConvHelper;
class GNENetworkElement;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPoly
 *
 * In the case the represented junction's shape is empty, the boundary
 *  is computed using the junction's position to which an offset of 1m to each
 *  side is added.
 */
class GNEPoly : public TesselatedPolygon, public GNEAdditional {

public:
    /// @brief needed to avoid diamond problem between SUMOPolygon and GNEAdditional
    using GNEAdditional::getID;

    /// @brief default Constructor
    GNEPoly(GNENet* net);

    /** @brief Constructor
     * @param[in] net net in which this polygon is placed
     * @param[in] id The name of the polygon
     * @param[in] type The (abstract) type of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] geo specifiy if shape was loaded as GEO
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] relativePath set image file as relative path
     * @param[in] fill Whether the polygon shall be filled
     * @param[in] lineWidth Line width when drawing unfilled polygon
     * @param[in] name Poly's name
     * @param[in] parameters generic parameters
     */
    GNEPoly(GNENet* net, const std::string& id, const std::string& type, const PositionVector& shape, bool geo, bool fill,
            double lineWidth, const RGBColor& color, double layer, double angle, const std::string& imgFile, bool relativePath,
            const std::string& name, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEPoly();

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation() override;

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) override;

    /// @brief gererate a new ID for an element child
    std::string generateChildID(SumoXMLTag childTag);

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
    /* @brief method for getting the Attribute of an XML key
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

    /**@brief method for checking if the key and their conrrespond attribute are valids
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

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
     * @param pos position of new/existent vertex
     * @param snapToGrid enable or disable snapToActiveGrid
     * @return index of position vector
     */
    int getVertexIndex(Position pos, bool snapToGrid);

    /// @brief delete the geometry point closest to the given pos
    void deleteGeometryPoint(const Position& pos, bool allowUndo = true);

    /// @brief check if polygon is closed
    bool isPolygonClosed() const;

    /// @brief open polygon
    void openPolygon(bool allowUndo = true);

    /// @brief close polygon
    void closePolygon(bool allowUndo = true);

    /// @brief change first geometry point
    void changeFirstGeometryPoint(int oldIndex, bool allowUndo = true);

    /// @brief replace the current shape with a rectangle
    void simplifyShape(bool allowUndo = true);

protected:
    /// @brief Latitude of Polygon
    PositionVector myGeoShape;

    /// @brief flag to indicate if polygon is simplified
    bool mySimplifiedShape;

    /// @brief geometry for lengths/rotations
    GUIGeometry myPolygonGeometry;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult) override;

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) override;

    /// @brief Invalidated copy constructor.
    GNEPoly(const GNEPoly&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPoly& operator=(const GNEPoly&) = delete;
};
