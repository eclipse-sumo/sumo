/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNELane.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A class for visualizing Lane geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNECandidateElement.h>
#include <netedit/GNELane2laneConnection.h>

#include "GNENetworkElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNETLSEditorFrame;
class GNEEdge;
class GNENet;
class GNEConnection;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNELane
 * @brief This lane is powered by an underlying GNEEdge and basically knows how
 * to draw itself
 */
class GNELane : public GNENetworkElement, public GNECandidateElement, public FXDelegator {
    /// @brief FOX-declaration
    FXDECLARE(GNELane)

public:
    /// @brief class for LaneDrawingConstants
    class LaneDrawingConstants {

    public:
        /// @brief parameter constructor (reference)
        LaneDrawingConstants(const GNELane* lane);

        /// @brief update lane drawing constants
        void update(const GUIVisualizationSettings& s);

        /// @brief get exaggeration
        double getExaggeration() const;

        /// @brief get half lane width
        double getHalfLaneWidth() const;

        // @brief get compute lane-marking width (intersection points)
        double getMarkWidth() const;

        /// @brief get lane width (but reduced,to make sure that a selected edge can still be seen
        double getWidth() const;

    private:
        /// @brief lane
        const GNELane* myLane;

        /// @brief exaggeration
        double myExaggeration = 0;

        /// @brief half lane width
        double myHalfLaneWidth = 0;

        // @brief compute lane-marking width (intersection points)
        double myMarkWidth = 0;

        /// @brief lane width (but reduced,to make sure that a selected edge can still be seen
        double myWidth = 0;

        /// @brief Invalidated copy constructor.
        LaneDrawingConstants(const LaneDrawingConstants&) = delete;

        /// @brief Invalidated assignment operator.
        LaneDrawingConstants& operator=(const LaneDrawingConstants&) = delete;
    };

    /**@brief Constructor
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] the edge this lane belongs to
     * @param[in] the index of this lane
     */
    GNELane(GNEEdge* edge, const int index);

    /// @brief Destructor
    ~GNELane();

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

    /// @brief get LaneDrawingConstants
    const LaneDrawingConstants* getLaneDrawingConstants() const;

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of hierarchical element in view
    Position getPositionInView() const;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const;

    /// @}

    /// @name Functions related with move elements
    /// @{
    /// @brief get move operation for the given shapeOffset (can be nullptr)
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);
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
    std::string getAttribute(SumoXMLAttr key) const;
    std::string getAttributeForSelection(SumoXMLAttr key) const;

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

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;
    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

    /* @brief method for setting the special color of the lane
     * @param[in] color Pointer to new special color
     */
    void setSpecialColor(const RGBColor* Color2, double colorValue = std::numeric_limits<double>::max());

    /// @brief return value for lane coloring according to the given scheme
    double getColorValue(const GUIVisualizationSettings& s, int activeScheme) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

    /// @brief draw overlapped routes
    void drawOverlappedRoutes(const int numRoutes) const;

    /// @brief draw laneStopOffset
    void drawLaneStopOffset(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

protected:
    /// @brief FOX needs this
    GNELane();

private:
    /// @brief parent edge (GNELanes cannot use hierarchical structures)
    GNEEdge* myParentEdge;

    /// @brief The index of this lane
    int myIndex;

    /// @brief lane geometry
    GUIGeometry myLaneGeometry;

    /// @brief LaneDrawingConstants
    LaneDrawingConstants *myLaneDrawingConstants;

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
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief draw lane
    void drawLane(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw back edge
    void drawBackEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                      const bool drawSpreadSuperposed) const;

    /// @brief draw shape edited
    void drawShapeEdited(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw children
    void drawChildren(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw lane markings
    void drawLaneMarkings(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const bool drawRailway) const;

    /// @brief draw link Number
    void drawLinkNo(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw TLS link Number
    void drawTLSLinkNo(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw lane arrows
    void drawLaneArrows(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const bool spreadSuperposed) const;

    /// @brief draw lane to lane connections
    void drawLane2LaneConnections() const;

    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(int activeScheme, RGBColor& col) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIVisualizationSettings& s, const GUIColorer& c, RGBColor& col) const;

    /// @brief whether to draw this lane as a waterways
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /// @brief direction indicators for lanes
    void drawDirectionIndicators(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                 const bool drawAsRailway, const bool spreadSuperposed) const;

    /// @brief draw lane as railway
    void drawLaneAsRailway(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw lane textures
    void drawTextures(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw start and end geometry points
    void drawStartEndGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

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
