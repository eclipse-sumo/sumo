/****************************************************************************/
/// @file    GNEInternalLane.h
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id$
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
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
#ifndef GNEInternalLane_h
#define GNEInternalLane_h


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
class GNETLSEditorFrame;
class PositionVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEInternalLane
 * @brief This object is responsible for drawing a shape and for supplying a
 * a popup menu. Messages are routeted to an internal dataTarget and to the
 * editor (hence inheritance from FXDelegator)
 */
class GNEInternalLane : public GUIGlObject, public FXDelegator {
    /// @brief FOX-declaration
    FXDECLARE(GNEInternalLane)

public:

    /**@brief Constructor
     * @param[in] editor The editor to notify about changes
     * @param[in] id The id of this internal lane
     * @param[in] shape The shape of the lane
     * @param[in] tlIndex The tl-index of the lane
     */
    GNEInternalLane(GNETLSEditorFrame* editor, const std::string& id, const PositionVector& shape, int tlIndex, LinkState state = LINKSTATE_DEADEND);

    /// @brief Destructor
    virtual ~GNEInternalLane();

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

    /// @brief set the linkState (controls drawing color)
    void setLinkState(LinkState state);

    /// @brief whether link state has been modfied
    LinkState getLinkState() const;

    /// @brief multiplexes message to two targets
    long onDefault(FXObject*, FXSelector, void*);

    /// @brief get Traffic Light index
    int getTLIndex() const;

    /// @brief long names for link states
    static const StringBijection<FXuint> LinkStateNames;

    /// @brief return the color for each linkstate
    static RGBColor colorForLinksState(FXuint state);

protected:
    /// @brief FOX needs this
    GNEInternalLane();

private:
    /// @brief the shape of the edge
    const PositionVector myShape;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    /// @}

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
    /// @brief return the color for each linkstate
    static const std::string& longNameForLinkState(FXuint state);

    /// @brief Invalidated copy constructor.
    GNEInternalLane(const GNEInternalLane&);

    /// @brief Invalidated assignment operator.
    GNEInternalLane& operator=(const GNEInternalLane&);
};


#endif

/****************************************************************************/

