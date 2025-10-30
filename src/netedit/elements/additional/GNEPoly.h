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
#include <utils/xml/CommonXMLStructure.h>

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GeoConvHelper;
class GNEMoveElementShape;
class GNENetworkElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPoly : public TesselatedPolygon, public GNEAdditional {

public:
    /// @brief needed to avoid diamond problem between SUMOPolygon and GNEAdditional
    using GNEAdditional::getID;

    /// @brief default Constructor
    GNEPoly(SumoXMLTag tag, GNENet* net);

    /** @brief Constructor for polygons
     * @param[in] id The name of the polygon
     * @param[in] net net in which this polygon is placed
     * @param[in] filename file in which this element is stored
     * @param[in] type The (abstract) type of the polygon
     * @param[in] shape The shape of the polygon
     * @param[in] geo specify if shape was loaded as GEO
     * @param[in] color The color of the polygon
     * @param[in] layer The layer of the polygon
     * @param[in] angle The rotation of the polygon
     * @param[in] imgFile The raster image of the polygon
     * @param[in] fill Whether the polygon shall be filled
     * @param[in] lineWidth Line width when drawing unfilled polygon
     * @param[in] name Poly's name
     * @param[in] parameters generic parameters
     */
    GNEPoly(const std::string& id, GNENet* net, const std::string& filename, const std::string& type, const PositionVector& shape,
            bool geo, bool fill, double lineWidth, const RGBColor& color, double layer, double angle, const std::string& imgFile,
            const std::string& name, const Parameterised::Map& parameters);

    /** @brief Constructor for JuPedSim elements
     * @param[in] id The name of the polygon
     * @param[in] net net in which this polygon is placed
     * @param[in] filename file in which this element is stored
     * @param[in] shape The shape of the polygon
     * @param[in] geo specify if shape was loaded as GEO
     * @param[in] name Poly's name
     * @param[in] parameters generic parameters
     */
    GNEPoly(SumoXMLTag tag, const std::string& id, GNENet* net, const std::string& filename, const PositionVector& shape,
            bool geo, const std::string& name, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEPoly();

    /// @brief methods to retrieve the elements linked to this poly
    /// @{

    /// @brief get GNEMoveElement associated with this poly
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this poly
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this poly (constant)
    const Parameterised* getParameters() const override;

    /// @}

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

    /// @brief check if current additional is valid to be written into XML (must be reimplemented in all detector children)
    bool isAdditionalValid() const override;

    /// @brief return a string with the current additional problem (must be reimplemented in all detector children)
    std::string getAdditionalProblem() const override;

    /// @brief fix additional problem (must be reimplemented in all detector children)
    void fixAdditionalProblem() override;

    /// @brief Returns the numerical id of the object
    GUIGlID getGlID() const;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

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

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

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

    /// @brief get SUMOBaseObject with all polygon attributes
    CommonXMLStructure::SumoBaseObject* getSumoBaseObject() const;

protected:
    /// @brief move element shape
    GNEMoveElementShape* myMoveElementShape = nullptr;

    /// @brief Latitude of Polygon
    PositionVector myGeoShape;

    /// @brief flag to indicate if polygon is closed
    bool myClosedShape = false;

    /// @brief flag to indicate if polygon is simplified
    bool mySimplifiedShape = false;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief draw polygon
    void drawPolygon(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                     const RGBColor& color, const double exaggeration) const;

    /// @brief draw contour
    void drawPolygonContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const RGBColor& color, const double exaggeration) const;

    /// @brief draw geometry points
    void drawGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const RGBColor& color, const double exaggeration) const;

    /// @brief draw polygon name and type
    void drawPolygonNameAndType(const GUIVisualizationSettings& s) const;

    /// @brief Invalidated copy constructor.
    GNEPoly(const GNEPoly&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPoly& operator=(const GNEPoly&) = delete;
};
