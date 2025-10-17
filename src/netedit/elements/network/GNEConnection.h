/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2016-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEConnection.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A class for represent connections between Lanes
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBEdge.h>
#include <utils/gui/div/GUIGeometry.h>

#include "GNENetworkElement.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNEMoveElementConnection;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEConnection : public GNENetworkElement {

    /// @brief friend classses
    friend class GNEMoveElementConnection;

public:
    /** Constructor
     * @param[in] from The edge the vehicles leave
     * @param[in] connection NBEdge::Connection in which the rest of parameters are defined
     * @param[in] uncontrolled if set to true, This connection will not be TLS-controlled despite its node being controlled.
    **/
    GNEConnection(GNELane* from, GNELane* to);

    /// @brief Destructor
    ~GNEConnection();

    /// @brief methods to retrieve the elements linked to this connection
    /// @{

    /// @brief get GNEMoveElement associated with this connection
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this connection
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this connection (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @name Functions related with geometry of element
    /// @{

    /// @brief get connection shape
    const PositionVector& getConnectionShape() const;

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns position of hierarchical element in view
    Position getPositionInView() const;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const override;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const override;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const override;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const override;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const override;

    /// @brief check if draw delete contour small (pink/white)
    bool checkDrawDeleteContourSmall() const override;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const override;

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

    /// @}

    /// @brief get the name of the edge the vehicles leave
    GNEEdge* getEdgeFrom() const;

    /// @brief get the name of the edge the vehicles may reach when leaving "from"
    GNEEdge* getEdgeTo() const;

    /// @briefthe get lane of the incoming lane
    GNELane* getLaneFrom() const;

    /// @briefthe get lane of the outgoing lane
    GNELane* getLaneTo() const;

    /// @briefthe get lane index of the incoming lane
    int getFromLaneIndex() const;

    /// @briefthe get lane index of the outgoing lane
    int getToLaneIndex() const;

    /// @brief get Edge::Connection
    NBEdge::Connection& getNBEdgeConnection() const;

    /// @brief get NBConnection
    NBConnection getNBConnection() const;

    /// @brief get LinkState
    LinkState getLinkState() const;

    /// @brief check that connection's Geometry has to be updated
    void markConnectionGeometryDeprecated();

    /// @brief update internal ID of Connection
    void updateConnectionID();

    /// @brief recompute cached myLinkState
    void updateLinkState();

    /// @brief smoothShape
    void smootShape();

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @brief delete element
    void deleteGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @}

    /* @brief method for setting the special color of the connection
     * @param[in] color Pointer to new special color
     */
    void setSpecialColor(const RGBColor* Color2);

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

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;

    /// @}

protected:
    /// @brief move element connection
    GNEMoveElementConnection* myMoveElementConnection = nullptr;

    /// @brief Linkstate. @note cached because after 'undo' the connection needs to be drawn while the node logic (NBRequest) has not been recomputed
    LinkState myLinkState;

    /// @brief optional special color
    const RGBColor* mySpecialColor;

    /// @brief connection geometry
    GUIGeometry myConnectionGeometry;

    /// @brief junction maker geometry
    GUIGeometry myInternalJunctionMarkerGeometry;

    /// @brief flag to indicate that connection's shape has to be updated
    bool myShapeDeprecated;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief manage change of tlLinkindices
    void changeTLIndex(SumoXMLAttr key, int tlIndex, int tlIndex2, GNEUndoList* undoList);

    /// @brief check if the edgeConnection vinculated with this connection exists
    bool existNBEdgeConnection() const;

    /// @brief check if draw connection
    bool checkDrawConnection() const;

    /// @brief get connection color
    RGBColor getConnectionColor(const GUIVisualizationSettings& s) const;

    /// @brief draw connection
    void drawConnection(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                        const GUIGeometry& superposedGeometry, const double exaggeration) const;

    /// @brief draw arrows over connections
    void drawConnectionArrows(const GUIVisualizationSettings& s, const GUIGeometry& superposedGeometry,
                              const RGBColor& color) const;

    /// @brief draw edge value
    void drawEdgeValues(const GUIVisualizationSettings& s, const PositionVector& shape) const;

    /// @brief calculate connection contour
    void calculateConnectionContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                    const PositionVector& shape, const double exaggeration) const;

    /// @brief Invalidated copy constructor.
    GNEConnection(const GNEConnection&) = delete;

    /// @brief Invalidated assignment operator.
    GNEConnection& operator=(const GNEConnection&) = delete;
};
