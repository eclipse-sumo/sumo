/****************************************************************************/
/// @file    GNELane.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing Lane geometry (adapted from GUILaneWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
class GNEAdditional;
class GNEConnection;

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
    /// @brief Definition of the additionals vector
    typedef std::vector<GNEAdditional*> AdditionalVector;

    /**@brief Constructor
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] the edge this lane belongs to
     * @param[in] the index of this lane
     */
    GNELane(GNEEdge& edge, const int index);

    /// @brief Destructor
    ~GNELane();

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

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief multiplexes message to two targets
    long onDefault(FXObject*, FXSelector, void*);

    /// @brief Returns underlying parent edge
    GNEEdge& getParentEdge();

    /// @brief returns a vector with the incoming GNEConnections of this lane
    std::vector<GNEConnection*> getGNEIncomingConnections();

    /// @brief returns a vector with the outgoing GNEConnections of this lane
    std::vector<GNEConnection*> getGNEOutcomingConnections();

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
    const std::vector<SUMOReal>& getShapeRotations() const;

    /// @brief returns the vector with the shape lengths
    const std::vector<SUMOReal>& getShapeLengths() const;

    /// @brief returns the boundry (including lanes)
    Boundary getBoundary() const;

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. junction moved)
    void updateGeometry();

    /// @brief returns the index of the lane
    int getIndex() const;

    /// @nrief returns the current speed of lane
    SUMOReal getSpeed() const;

    /* @brief method for setting the index of the lane
     * @param[in] index The new index of lane
     */
    void setIndex(int index);

    /// @brief returns the parameteric length of the lane
    /// @note is the same as their Edge parent
    SUMOReal getLaneParametricLenght() const;

    /// @brief returns the length of the lane's shape
    SUMOReal getLaneShapeLenght() const;

    /* @brief returns the relative position of an element in the lane's shape depending of the parametric lenght
     *        Examples: Lane with Parametric lenght = 100 and Shape lenght = 250. Position 50 returns 125, Position 80 returns 200
     * @param[in] position to calculate their relative position in the lane's shape [0 < position < LaneParametricLenght()]
     * @return the relative position in the lane's shape
     */
    SUMOReal getPositionRelativeToParametricLenght(SUMOReal position) const;

    /* @brief returns the relative position of an element in the lane's shape depending of the shape's lenght
     *        Examples: Lane with Parametric lenght = 100 and Shape lenght = 250. Position = 100 returns 40, Position 220 returns 88
     * @param[in] position to calculate their relative position in the lane's shape [0 < position < LaneShapeLenght]
     * @return the relative position in the lane's shape
     */
    SUMOReal getPositionRelativeToShapeLenght(SUMOReal position) const;

    /// @brief add additional child to this lane
    void addAdditionalChild(GNEAdditional* additional);

    /// @brief remove additional child to this lane
    void removeAdditionalChild(GNEAdditional* additional);

    /// @brief get additional childs of lane
    const std::vector<GNEAdditional*>& getAdditionalChilds() const;

    /// @brief check if this lane is restricted
    bool isRestricted(SUMOVehicleClass vclass) const;

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

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
    std::vector<SUMOReal> myShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;

    /// @brief Position of textures of restricted lanes
    std::vector<Position> myLaneRestrictedTexturePositions;

    /// @brief Rotations of textures of restricted lanes
    std::vector<SUMOReal> myLaneRestrictedTextureRotations;
    /// @}

    /// @brief list with the additonals vinculated with this lane
    AdditionalVector myAdditionals;

    /// @brief optional special color
    const RGBColor* mySpecialColor;

    /// @brief The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;

    /// @brief the tls-editor for setting multiple links in TLS-mode
    GNETLSEditorFrame* myTLSEditor;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNELane(const GNELane&);

    /// @brief Invalidated assignment operator.
    GNELane& operator=(const GNELane&);

    /// @brief draw lane markings
    void drawMarkings(const bool& selectedEdge, SUMOReal scale) const;

    /// @brief draw link Number
    void drawLinkNo() const;

    /// @brief draw TLS Link Number
    void drawTLSLinkNo() const;

    /// @brief draw link rules
    void drawLinkRules() const;

    /// @brief draw arrows
    void drawArrows() const;

    /// @brief draw lane to lane connections
    void drawLane2LaneConnections() const;

    /// @brief return value for lane coloring according to the given scheme
    SUMOReal getColorValue(int activeScheme) const;

    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(int activeScheme) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIColorer& c) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

    /// @brief whether to draw this lane as a waterways
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /// @brief draw crossties for railroads
    /// @todo: XXX This duplicates the code of GUILane::drawCrossties and needs to be
    void drawCrossties(SUMOReal length, SUMOReal spacing, SUMOReal halfWidth) const;

    /// @brief direction indicators for lanes
    void drawDirectionIndicators() const;
};


#endif

/****************************************************************************/

