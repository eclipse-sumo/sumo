/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEWalkingArea.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// A class for visualizing and editing WalkingAreas
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNENetworkElement.h"
#include <utils/gui/globjects/GUIPolygon.h>
#include <netbuild/NBNode.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNEJunction;
class GNEEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEWalkingArea
 * @brief This object is responsible for drawing a shape and for supplying a
 * a popup menu. Messages are routeted to an internal dataTarget and to the
 * editor (hence inheritance from FXDelegator)
 */
class GNEWalkingArea : public GNENetworkElement {

public:
    /**@brief Constructor
     * @param[in] parentJunction GNEJunction in which this crossing is placed
     * @param[in] ID walkingArea ID
     */
    GNEWalkingArea(GNEJunction* parentJunction, const std::string& ID);

    /// @brief Destructor
    ~GNEWalkingArea();

    /// @name Functions related with geometry of element
    /// @{

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

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const;

    /// @}

    /// @name Functions related with move elements
    /// @{
    /// @brief get move operation for the given shapeOffset (can be nullptr)
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

    /// @}

    /// @brief get parent Junction
    GNEJunction* getParentJunction() const;

    /// @brief get referente to NBode::WalkingArea
    NBNode::WalkingArea& getNBWalkingArea() const;

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

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

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

    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

protected:
    /// @brief the parent junction of this crossing
    GNEJunction* myParentJunction;

    /// @brief An object that stores the shape and its tesselation
    mutable TesselatedPolygon myTesselation;

    /// @brief exaggeration used in tesselation
    mutable double myExaggeration;

    /// @brief variable used for draw innen contour
    GNEContour myInnenContour;

private:
    /// @brief draw walking area
    void drawWalkingArea(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                         const PositionVector& shape, const double exaggeration) const;

    /// @brief check if draw walking area in contour mode
    bool drawInContourMode() const;

    /// @brief draw tesselated walking area
    void drawTesselatedWalkingArea(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d) const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNEWalkingArea(const GNEWalkingArea&) = delete;

    /// @brief Invalidated assignment operator.
    GNEWalkingArea& operator=(const GNEWalkingArea&) = delete;
};
