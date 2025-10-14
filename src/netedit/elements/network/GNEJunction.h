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
/// @file    GNEJunction.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A class for visualizing and editing junctions in netedit (adapted from
// GUIJunctionWrapper)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBNode.h>
#include <netedit/elements/GNECandidateElement.h>
#include <netedit/elements/moving/GNEMoveResult.h>
#include <utils/gui/globjects/GUIPolygon.h>

#include "GNENetworkElement.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEConnection;
class GNECrossing;
class GNEEdge;
class GNEInternalLane;
class GNEMoveElementJunction;
class GNEMoveOperation;
class GNENet;
class GNEWalkingArea;
class NBTrafficLightDefinition;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEJunction : public GNENetworkElement, public GNECandidateElement {

    /// @brief Declare friend class
    friend class GNEChange_TLS;
    friend class GNEChange_Crossing;
    friend class GNEMoveElementJunction;

public:
    /**@brief Constructor
     * @param[in] net The net to inform about gui updates
     * @param[in] nbn The represented node
     * @param[in] loaded Whether the junction was loaded from a file
     */
    GNEJunction(GNENet* net, NBNode* nbn, bool loaded = false);

    /// @brief Destructor
    ~GNEJunction();

    /// @brief methods to retrieve the elements linked to this junction
    /// @{

    /// @brief get GNEMoveElement associated with this junction
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this junction
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this junction (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @name Functions related with geometry of element
    /// @{
    /// @brief get junction shape
    const PositionVector& getJunctionShape() const;

    /// @brief update pre-computed geometry information (including crossings)
    void updateGeometry() override;

    /// @brief update pre-computed geometry information without modifying netbuild structures
    // @note: using an extra function because updateGeometry overrides an abstract virtual function
    void updateGeometryAfterNetbuild(bool rebuildNBNodeCrossings = false);

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

    /// @brief Return net build node
    NBNode* getNBNode() const;

    /// @brief return GNEJunction neighbours
    std::vector<GNEJunction*> getJunctionNeighbours() const;

    /// @brief add incoming GNEEdge
    void addIncomingGNEEdge(GNEEdge* edge);

    /// @brief add outgoing GNEEdge
    void addOutgoingGNEEdge(GNEEdge* edge);

    /// @brief remove incoming GNEEdge
    void removeIncomingGNEEdge(GNEEdge* edge);

    /// @brief remove outgoing GNEEdge
    void removeOutgoingGNEEdge(GNEEdge* edge);

    /// @brief Returns incoming GNEEdges
    const std::vector<GNEEdge*>& getGNEIncomingEdges() const;

    /// @brief Returns incoming GNEEdges
    const std::vector<GNEEdge*>& getGNEOutgoingEdges() const;

    /// @brief Returns GNECrossings
    const std::vector<GNECrossing*>& getGNECrossings() const;

    /// @brief Returns GNEWalkingAreas
    const std::vector<GNEWalkingArea*>& getGNEWalkingAreas() const;

    /// @brief Returns all GNEConnections vinculated with this junction
    std::vector<GNEConnection*> getGNEConnections() const;

    /// @brief marks as first junction in createEdge-mode
    void markAsCreateEdgeSource();

    /// @brief removes mark as first junction in createEdge-mode
    void unMarkAsCreateEdgeSource();

    /// @brief notify the junction of being selected in tls-mode. (used to control drawing)
    void selectTLS(bool selected);

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

    /// @brief set responsibility for deleting internal structures
    void setResponsible(bool newVal);

    /* @brief notify junction that one of its edges has changed its shape, and
     * therefore the junction shape is no longer valid */
    void invalidateShape();

    /* @brief update validity of this junctions logic
     * if the logic is invalidated, existing connections are removed via undo-list
     * so that the previous state can be restored
     * also calls invalidateTLS
     * @param[in] valid The new validity of the junction
     * @note: this should always be called with an active command group
     */
    void setLogicValid(bool valid, GNEUndoList* undoList, const std::string& status = FEATURE_GUESSED);

    /// @brief remove all connections from the given edge
    void removeConnectionsFrom(GNEEdge* edge, GNEUndoList* undoList, bool updateTLS, int lane = -1);

    /// @brief remove all connections to the given edge
    void removeConnectionsTo(GNEEdge* edge, GNEUndoList* undoList, bool updateTLS, int lane = -1);

    /// @brief prevent re-guessing connections at this junction
    void markAsModified(GNEUndoList* undoList);

    /* @brief invalidates loaded or edited TLS
     * @param[in] deletedConnection If a valid connection is given a replacement def with this connection removed
     *   but all other information intact will be computed instead of guessing a new tlDef
     * @note: this should always be called with an active command group
     */
    void invalidateTLS(GNEUndoList* undoList,
                       const NBConnection& deletedConnection = NBConnection::InvalidConnection,
                       const NBConnection& addedConnection = NBConnection::InvalidConnection);

    /// @brief replace one edge by another in all tls connections
    void replaceIncomingConnections(GNEEdge* which, GNEEdge* by, GNEUndoList* undoList);

    /// @brief removes the given edge from all pedestrian crossings
    void removeEdgeFromCrossings(GNEEdge* edge, GNEUndoList* undoList);

    /// @brief whether this junction has a valid logic
    bool isLogicValid();

    /// @brief get GNECrossing if exist, and if not create it if create is enabled
    GNECrossing* retrieveGNECrossing(NBNode::Crossing* NBNodeCrossing, bool createIfNoExist = true);

    /// @brief get GNEWalkingArea if exist, and if not create it if create is enabled
    GNEWalkingArea* retrieveGNEWalkingArea(const std::string& NBNodeWalkingAreaID, bool createIfNoExist = true);

    /// @brief mark connections as deprecated
    void markConnectionsDeprecated(bool includingNeighbours);

    /// @brief set junction Type (using undo/redo)
    void setJunctionType(const std::string& value, GNEUndoList* undoList);

    /// @brief clear walking areas
    void clearWalkingAreas();

    /// @brief rebuilds WalkingAreas objects for this junction
    void rebuildGNEWalkingAreas();

    /// @brief add internal lane
    void addInternalLane(const GNEInternalLane* internalLane);

    /// @brief remove internal lane
    void removeInternalLane(const GNEInternalLane* internalLane);

protected:
    /// @brief move element junction
    GNEMoveElementJunction* myMoveElementJunction = nullptr;

    /// @brief A reference to the represented junction
    NBNode* myNBNode;

    /// @brief edge boundary
    Boundary myJunctionBoundary;

    /// @brief drawing toggle (used to avoid double draws)
    int* myDrawingToggle;

    /// @brief variable used for draw circle contours
    GNEContour myCircleContour;

    /// @brief vector with the (child) incomings GNEEdges vinculated with this junction
    std::vector<GNEEdge*> myGNEIncomingEdges;

    /// @brief vector with the (child) outgoings GNEEdges vinculated with this junction
    std::vector<GNEEdge*> myGNEOutgoingEdges;

    /// @brief the built crossing objects
    std::vector<GNECrossing*> myGNECrossings;

    /// @brief the built walkingArea objects
    std::vector<GNEWalkingArea*> myGNEWalkingAreas;

    /// @brief internal lanes related placed in this junction
    std::vector<const GNEInternalLane*> myInternalLanes;

    /// @brief whether this junction is the first junction for a newly creatededge
    /// @see GNEApplicationWindow::createEdgeSource)
    bool myAmCreateEdgeSource = false;

    /// @brief modification status of the junction logic (all connections across this junction)
    std::string myLogicStatus;

    /// @brief whether we are responsible for deleting myNBNode
    bool myAmResponsible = false;

    /// @brief whether this junctions logic is valid
    bool myHasValidLogic;

    /// @brief whether this junction is selected in tls-mode
    bool myAmTLSSelected = false;

    /// @brief whether this junction probably should have some connections but doesn't
    bool myColorForMissingConnections = false;

    /// @brief An object that stores the shape and its tesselation
    mutable TesselatedPolygon myTesselation;

    /// @brief exaggeration used in tesselation
    mutable double myExaggeration = 1;

private:
    /// @brief check if draw junction as bubble
    bool drawAsBubble(const GUIVisualizationSettings& s, const double junctionShapeArea) const;

    /// @brief draw junction as bubble
    void drawJunctionAsBubble(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                              const double exaggeration) const;

    /// @brief draw junction as bubble
    void drawJunctionAsShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                             const double exaggeration) const;

    /// @brief draw junction center (only in move mode)
    void drawJunctionCenter(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief draw TLS icon
    void drawTLSIcon(const GUIVisualizationSettings& s) const;

    /// @brief draw elevation
    void drawElevation(const GUIVisualizationSettings& s) const;

    /// @brief draw junction name
    void drawJunctionName(const GUIVisualizationSettings& s) const;

    /// @brief draw junction childs
    void drawJunctionChildren(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief calculate contour
    void calculateJunctioncontour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const double exaggeration, const bool drawBubble) const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /**@brief reposition the node at pos without updating GRID and informs the edges
     * @param[in] pos The new position
     * @note: those operations are not added to the undoList.
     */
    void moveJunctionGeometry(const Position& pos, const bool updateEdgeBoundaries);

    /// @brief sets junction color depending on circumstances
    RGBColor setColor(const GUIVisualizationSettings& s, bool bubble) const;

    /// @brief determines color value
    double getColorValue(const GUIVisualizationSettings& s, int activeScheme) const;

    /// @brief compute whether this junction probably should have some connections but doesn't
    void checkMissingConnections();

    /// @brief adds a traffic light
    void addTrafficLight(NBTrafficLightDefinition* tlDef, bool forceInsert);

    /// @brief removes a traffic light
    void removeTrafficLight(NBTrafficLightDefinition* tlDef);

    /// @brief rebuilds crossing objects for this junction
    void rebuildGNECrossings(bool rebuildNBNodeCrossings = true);

    /// @brief remove the given connections from all traffic light definitions of this junction
    void removeTLSConnections(std::vector<NBConnection>& connections, GNEUndoList* undoList);

    /// @brief temporarily mirror coordinates in lefthand network to compute correct crossing geometries
    void mirrorXLeftHand();

    /// @brief build TLS operations contextual menu
    void buildTLSOperations(GUISUMOAbstractView& parent, GUIGLObjectPopupMenu* ret, const int numSelectedJunctions);

    /// @brief Invalidated copy constructor.
    GNEJunction(const GNEJunction&) = delete;

    /// @brief Invalidated assignment operator.
    GNEJunction& operator=(const GNEJunction&) = delete;
};
