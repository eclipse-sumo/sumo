/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNELane.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing Lane geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#ifndef GNELane_h
#define GNELane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNENetElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNETLSEditorFrame;
class GNEEdge;
class GNENet;
class GNEConnection;
class GNEShape;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNELane
 * @brief This lane is powered by an underlying GNEEdge and basically knows how
 * to draw itself
 */
class GNELane : public GNENetElement, public FXDelegator {
    /// @brief FOX-declaration
    FXDECLARE(GNELane)

public:

    /**@brief Constructor
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] the edge this lane belongs to
     * @param[in] the index of this lane
     */
    GNELane(GNEEdge& edge, const int index);

    /// @brief Destructor
    ~GNELane();

    /// @brief Returns underlying parent edge
    GNEEdge& getParentEdge();

    /// @brief returns a vector with the incoming GNEConnections of this lane
    std::vector<GNEConnection*> getGNEIncomingConnections();

    /// @brief returns a vector with the outgoing GNEConnections of this lane
    std::vector<GNEConnection*> getGNEOutcomingConnections();

    // update IDs of incoming connections of this lane
    void updateConnectionIDs();

    /// @name inherited from GUIGlObject
    /// @{
    // @brief Returns the name of the parent object (if any)
    // @return This object's parent id
    const std::string& getParentName() const;

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

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @brief returns the shape of the lane
    const PositionVector& getShape() const;

    /// @brief returns the vector with the shape rotations
    const std::vector<double>& getShapeRotations() const;

    /// @brief returns the vector with the shape lengths
    const std::vector<double>& getShapeLengths() const;

    /// @brief returns the boundry (including lanes)
    Boundary getBoundary() const;

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. junction moved)
    void updateGeometry();

    /// @brief returns the index of the lane
    int getIndex() const;

    /// @nrief returns the current speed of lane
    double getSpeed() const;

    /* @brief method for setting the index of the lane
     * @param[in] index The new index of lane
     */
    void setIndex(int index);

    /** @brief returns the parameteric length of the lane
     *  @note is the same as their Edge parent, and cannot be never NULL
     */
    double getLaneParametricLength() const;

    /// @brief returns the length of the lane's shape
    double getLaneShapeLength() const;

    /// @brief add shape child to this lane
    void addShapeChild(GNEShape* shape);

    /// @brief remove shape child of this lane
    void removeShapeChild(GNEShape* shape);

    /// @brief get shape childs of lane
    const std::vector<GNEShape*>& getShapeChilds() const;

    /// @brief check if this lane is restricted
    bool isRestricted(SUMOVehicleClass vclass) const;

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
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

    /* @brief method for setting the special color of the lane
     * @param[in] color Pointer to new special color
     */
    void setSpecialColor(const RGBColor* Color2);

    /// @brief return value for lane coloring according to the given scheme
    double getColorValue(int activeScheme) const;

    /// @brief remove lane of Additional Parent
    void removeLaneOfAdditionalParents(GNEUndoList* undoList, bool allowEmpty);

protected:
    /// @brief FOX needs this
    GNELane();

    /// @brief The Edge that to which this lane belongs
    GNEEdge& myParentEdge;

    /// @brief The index of this lane
    int myIndex;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// @brief The rotations of the shape parts
    std::vector<double> myShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<double> myShapeLengths;

    /// @brief Position of textures of restricted lanes
    std::vector<Position> myLaneRestrictedTexturePositions;

    /// @brief Rotations of textures of restricted lanes
    std::vector<double> myLaneRestrictedTextureRotations;
    /// @}

    /// @brief list with the shapes vinculated with this lane
    std::vector<GNEShape*> myShapes;

    /// @brief optional special color
    const RGBColor* mySpecialColor;

    /// @brief The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;

    /// @brief the tls-editor for setting multiple links in TLS-mode
    GNETLSEditorFrame* myTLSEditor;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief draw lane markings
    void drawMarkings(const bool& selectedEdge, double scale) const;

    /// @brief draw link Number
    void drawLinkNo(const GUIVisualizationSettings& s) const;

    /// @brief draw TLS Link Number
    void drawTLSLinkNo(const GUIVisualizationSettings& s) const;

    /// @brief draw link rules
    void drawLinkRules() const;

    /// @brief draw arrows
    void drawArrows() const;

    /// @brief draw lane to lane connections
    void drawLane2LaneConnections() const;

    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(int activeScheme) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIColorer& c) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

    /// @brief whether to draw this lane as a waterways
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /// @brief direction indicators for lanes
    void drawDirectionIndicators() const;

    /// @brief set color according to edit mode and visualisation settings
    void setLaneColor(const GUIVisualizationSettings& s) const;

    /// @brief Invalidated copy constructor.
    GNELane(const GNELane&) = delete;

    /// @brief Invalidated assignment operator.
    GNELane& operator=(const GNELane&) = delete;
};


#endif

/****************************************************************************/

