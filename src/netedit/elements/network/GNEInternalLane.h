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
/// @file    GNEInternalLane.h
/// @author  Jakob Erdmann
/// @date    June 2011
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNENetworkElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class GNETLSEditorFrame;
class PositionVector;


// ===========================================================================
// class definitions
// ===========================================================================

class GNEInternalLane : public GNENetworkElement, public FXDelegator {
    /// @brief FOX-declaration
    FXDECLARE(GNEInternalLane)

public:
    /**@brief Constructor
     * @param[in] editor The editor to notify about changes
     * @param[in] junctionParent junction parent
     * @param[in] id The id of this internal lane
     * @param[in] shape The shape of the lane
     * @param[in] tlIndex The tl-index of the lane
     */
    GNEInternalLane(GNETLSEditorFrame* editor, GNEJunction* junctionParent, const std::string& id,
                    const PositionVector& shape, int tlIndex, LinkState state = LINKSTATE_DEADEND);

    /// @brief Destructor
    ~GNEInternalLane();

    /// @brief methods to retrieve the elements linked to this internalLane
    /// @{

    /// @brief get GNEMoveElement associated with this internalLane
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this internalLane
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this internalLane (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @name Functions related with geometry of element
    /// @{

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

    /// @name Functions related with move elements
    /// @{
    /// @brief get move operation for the given shapeOffset (can be nullptr)
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);
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

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

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

    /// @brief set the linkState (controls drawing color)
    void setLinkState(LinkState state);

    /// @brief whether link state has been modified
    LinkState getLinkState() const;

    /// @brief multiplexes message to two targets
    long onDefault(FXObject*, FXSelector, void*);

    /// @brief get Traffic Light index
    int getTLIndex() const;

    /// @brief long names for link states
    static const StringBijection<FXuint> LinkStateNames;

    /// @brief return the color for each linkstate
    static RGBColor colorForLinksState(FXuint state);

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

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /// @}

protected:
    /// @brief FOX needs this
    GNEInternalLane();

private:
    /// @brief pointer to junction parent
    GNEJunction* myJunctionParent;

    /// @brief internal lane geometry
    GUIGeometry myInternalLaneGeometry;

    /// @brief the state of the link (used for visualization)
    FXuint myState;
    FXDataTarget myStateTarget;

    /// @brief the original state of the link (used for tracking modification)
    LinkState myOrigState;

    /// @brief data target for selection state
    FXDataTarget stateTarget;

    /// @brief the editor to inform about changes
    GNETLSEditorFrame* myEditor;

    /// @brief the tl-index of this lane
    int myTlIndex;

    /// @brief the created popup
    GUIGLObjectPopupMenu* myPopup;

    /// @brief linkstates names values
    static StringBijection<FXuint>::Entry linkStateNamesValues[];

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNEInternalLane(const GNEInternalLane&) = delete;

    /// @brief Invalidated assignment operator.
    GNEInternalLane& operator=(const GNEInternalLane&) = delete;
};
