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
/// @file    GNELane.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A class for visualizing Lane geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNECandidateElement.h>
#include <netedit/elements/moving/GNEMoveResult.h>
#include <netedit/GNELane2laneConnection.h>

#include "GNENetworkElement.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEConnection;
class GNEEdge;
class GNEMoveElementLane;
class GNEMoveOperation;
class GNENet;
class GNETLSEditorFrame;
class GUIGLObjectPopupMenu;
class PositionVector;

// ===========================================================================
// class definitions
// ===========================================================================

class GNELane : public GNENetworkElement, public GNECandidateElement, public FXDelegator {
    /// @brief FOX-declaration
    FXDECLARE(GNELane)

public:
    /// @brief class for lane drawing constants
    class DrawingConstants {

    public:
        /// @brief parameter constructor
        DrawingConstants(const GNELane* lane);

        /// @brief update lane drawing constants
        void update(const GUIVisualizationSettings& s);

        /// @brief get exaggeration
        double getExaggeration() const;

        /// @brief get lane drawing width
        double getDrawingWidth() const;

        /// @brief get internal lane drawing width
        double getInternalDrawingWidth() const;

        /// @brief get lane offset
        double getOffset() const;

        /// @brief get detail
        GUIVisualizationSettings::Detail getDetail() const;

        /// @brief draw as railway
        bool drawAsRailway() const;

        /// @brief draw superposed
        bool drawSuperposed() const;

    protected:
        /// @brief lane
        const GNELane* myLane;

        /// @brief exaggeration
        double myExaggeration = 0;

        /// @brief lane drawing width
        double myDrawingWidth = 0;

        /// @brief internal lane drawing width (used for drawing selected lanes)
        double myInternalDrawingWidth = 0;

        /// @brief lane offset
        double myOffset = 0;

        /// @brief detail level
        GUIVisualizationSettings::Detail myDetail = GUIVisualizationSettings::Detail::Level4;

        /// @brief draw as railway
        bool myDrawAsRailway = false;

        /// @brief draw supersposed (reduced width so that the lane markings below are visible)
        bool myDrawSuperposed = false;

    private:
        /// @brief invalidate default constructor
        DrawingConstants() = delete;

        /// @brief Invalidated copy constructor.
        DrawingConstants(const DrawingConstants&) = delete;

        /// @brief Invalidated assignment operator.
        DrawingConstants& operator=(const DrawingConstants&) = delete;
    };

    /**@brief Constructor
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] the edge this lane belongs to
     * @param[in] the index of this lane
     */
    GNELane(GNEEdge* edge, const int index);

    /// @brief Destructor
    ~GNELane();

    /// @brief methods to retrieve the elements linked to this lane
    /// @{

    /// @brief get GNEMoveElement associated with this lane
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this lane
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this lane
    const Parameterised* getParameters() const override;

    /// @}

    /// @brief get parent edge
    GNEEdge* getParentEdge() const;

    /// @brief check if current lane allow pedestrians
    bool allowPedestrians() const;

    /// @name Functions related with geometry of element
    /// @{

    /// @brief get lane geometry
    const GUIGeometry& getLaneGeometry() const;

    /// @brief get elements shape
    const PositionVector& getLaneShape() const;

    /// @brief get rotations of the single shape parts
    const std::vector<double>& getShapeRotations() const;

    /// @brief get lengths of the single shape parts
    const std::vector<double>& getShapeLengths() const;

    /// @brief get lane drawing constants (previously calculated in drawGL())
    const DrawingConstants* getDrawingConstants() const;

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

    /// @brief returns a vector with the incoming GNEConnections of this lane
    std::vector<GNEConnection*> getGNEIncomingConnections();

    /// @brief returns a vector with the outgoing GNEConnections of this lane
    std::vector<GNEConnection*> getGNEOutcomingConnections();

    /// @brief update IDs of incoming connections of this lane
    void updateConnectionIDs();

    /// @brief get length geometry factor
    double getLengthGeometryFactor() const;

    /// @name inherited from GUIGlObject
    /// @{
    // @brief Returns the name of the parent object (if any)
    // @return This object's parent id
    std::string getParentName() const;

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief multiplexes message to two targets
    long onDefault(FXObject*, FXSelector, void*);

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

    /// @brief returns the index of the lane
    int getIndex() const;

    /// @brief returns the current speed of lane
    double getSpeed() const;

    /* @brief method for setting the index of the lane
     * @param[in] index The new index of lane
     */
    void setIndex(int index);

    /** @brief returns the parameteric length of the lane
     *  @note is the same as their Edge parent, and cannot be never nullptr
     */
    double getLaneParametricLength() const;

    /// @brief returns the length of the lane's shape
    double getLaneShapeLength() const;

    /// @brief check if this lane is restricted
    bool isRestricted(SUMOVehicleClass vclass) const;

    /// @brief get Lane2laneConnection struct
    const GNELane2laneConnection& getLane2laneConnections() const;

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const override;
    std::string getAttributeForSelection(SumoXMLAttr key) const;

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

    /* @brief method for getting the Attribute of an XML key in Position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const;

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

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;

    /// @}

    /* @brief method for setting the special color of the lane
     * @param[in] color Pointer to new special color
     */
    void setSpecialColor(const RGBColor* Color2, double colorValue = std::numeric_limits<double>::max());

    /// @brief return value for lane coloring according to the given scheme
    double getColorValue(const GUIVisualizationSettings& s, int activeScheme) const;

    /// @brief draw overlapped routes
    void drawOverlappedRoutes(const int numRoutes) const;

    /// @brief draw laneStopOffset
    void drawLaneStopOffset(const GUIVisualizationSettings& s) const;

protected:
    /// @brief FOX needs this
    GNELane();

private:
    /// @brief move element lane
    GNEMoveElementLane* myMoveElementLane = nullptr;

    /// @brief The index of this lane
    int myIndex;

    /// @brief lane geometry
    GUIGeometry myLaneGeometry;

    /// @brief LaneDrawingConstants
    DrawingConstants* myDrawingConstants;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{

    /// @brief Position of textures of restricted lanes
    std::vector<Position> myLaneRestrictedTexturePositions;

    /// @brief Rotations of textures of restricted lanes
    std::vector<double> myLaneRestrictedTextureRotations;
    /// @}

    /// @brief optional special color
    const RGBColor* mySpecialColor;

    /// @brief optional value that corresponds to which the special color corresponds
    double mySpecialColorValue;

    /// @brief The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;

    /// @brief lane2lane connections
    GNELane2laneConnection myLane2laneConnections;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief draw lane
    void drawLane(const GUIVisualizationSettings& s, const double layer) const;

    /// @brief draw selected lane
    void drawSelectedLane(const GUIVisualizationSettings& s) const;

    /// @brief draw shape edited
    void drawShapeEdited(const GUIVisualizationSettings& s) const;

    /// @brief draw children
    void drawChildren(const GUIVisualizationSettings& s) const;

    /// @brief draw lane markings
    void drawMarkingsAndBoundings(const GUIVisualizationSettings& s) const;

    /// @brief draw link Number
    void drawLinkNo(const GUIVisualizationSettings& s) const;

    /// @brief draw TLS link Number
    void drawTLSLinkNo(const GUIVisualizationSettings& s) const;

    /// @brief draw lane arrows
    void drawArrows(const GUIVisualizationSettings& s) const;

    /// @brief draw lane to lane connections
    void drawLane2LaneConnections() const;

    /// @brief calculate contour
    void calculateLaneContour(const GUIVisualizationSettings& s, const double layer) const;

    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(int activeScheme, RGBColor& col) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIVisualizationSettings& s, const GUIColorer& c, RGBColor& col) const;

    /// @brief whether to draw this lane as a waterways
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /// @brief direction indicators for lanes
    void drawDirectionIndicators(const GUIVisualizationSettings& s) const;

    /// @brief draw lane as railway
    void drawLaneAsRailway() const;

    /// @brief draw lane textures
    void drawTextures(const GUIVisualizationSettings& s) const;

    /// @brief draw start and end geometry points
    void drawStartEndGeometryPoints(const GUIVisualizationSettings& s) const;

    /// @brief set color according to edit mode and visualisation settings
    RGBColor setLaneColor(const GUIVisualizationSettings& s) const;

    /// @brief build edge operations contextual menu
    void buildEdgeOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret);

    /// @brief build lane operations contextual menu
    void buildLaneOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret);

    /// @brief build template oerations contextual menu
    void buildTemplateOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret);

    /// @brief build rechable operations contextual menu
    void buildRechableOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret);

    /// @brief Invalidated copy constructor.
    GNELane(const GNELane&) = delete;

    /// @brief Invalidated assignment operator.
    GNELane& operator=(const GNELane&) = delete;
};
