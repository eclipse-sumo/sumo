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
/// @file    GUIGLObjectPopupMenu.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// The popup menu of a globject.
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <utils/foxtools/fxheader.h>
#include <utils/geom/Position.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class GUIGlObject;
class GUIMainWindow;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGLObjectPopupMenu
 * @brief The popup menu of a globject
 */
class GUIGLObjectPopupMenu : public FXMenuPane {
    // FOX-declarations
    FXDECLARE(GUIGLObjectPopupMenu)

public:

    /// @name cursor dialog type
    enum class PopupType {
        ATTRIBUTES,
        PROPERTIES,
        SELECT_ELEMENT,
        DELETE_ELEMENT,
        FRONT_ELEMENT
    };

    /** @brief Constructor
     * @param[in] app The main window for instantiation of other windows
     * @param[in] parent The parent view for changing it
     * @param[in] o The object of interest
     */
    GUIGLObjectPopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o);

    /** @brief Constructor
     * @param[in] app The main window for instantiation of other windows
     * @param[in] parent The parent view for changing it
     */
    GUIGLObjectPopupMenu(GUIMainWindow* app, GUISUMOAbstractView* parent, PopupType popupType);

    /// @brief Destructor
    virtual ~GUIGLObjectPopupMenu();

    /// @brief Insert a sub-menu pane in this GUIGLObjectPopupMenu
    void insertMenuPaneChild(FXMenuPane* child);

    // @brief remove popup menu from objects
    void removePopupFromObject();

    /// @brief return the real owner of this popup
    GUISUMOAbstractView* getParentView();

    /// @brief popup type;
    PopupType getPopupType() const;

    /// @name FX Calls
    /// @{
    /// @brief Called if the assigned objects shall be centered
    long onCmdCenter(FXObject*, FXSelector, void*);

    /// @brief Called if the name shall be copied to clipboard
    long onCmdCopyName(FXObject*, FXSelector, void*);

    /// @brief Called if the typed name shall be copied to clipboard
    long onCmdCopyTypedName(FXObject*, FXSelector, void*);

    /// @brief Called if the edge name shall be copied to clipboard (for lanes only)
    long onCmdCopyEdgeName(FXObject*, FXSelector, void*);

    /// @brief Called if the test coordinates shall be copied to clipboard (only if gui-testing option is enabled)
    long onCmdCopyTestCoordinates(FXObject*, FXSelector, void*);

    /// @brief Called if the cursor position shall be copied to clipboard
    long onCmdCopyCursorPosition(FXObject*, FXSelector, void*);

    /// @brief Called if the cursor geo-position shall be copied to clipboard
    long onCmdCopyCursorGeoPosition(FXObject*, FXSelector, void*);

    /// @brief Called if the current geo-boundary shall be copied to clipboard
    long onCmdCopyViewGeoBoundary(FXObject*, FXSelector, void*);

    /// @brief Called if the cursor geo-position shall be shown online
    long onCmdShowCursorGeoPositionOnline(FXObject*, FXSelector, void*);

    /// @brief Called if the parameter of this object shall be shown
    long onCmdShowPars(FXObject*, FXSelector, void*);

    /// @brief Called if the type parameter of this object shall be shown
    long onCmdShowTypePars(FXObject*, FXSelector, void*);

    /// @brief Called if the object shall be added to the list of selected objects
    long onCmdAddSelected(FXObject*, FXSelector, void*);

    /// @brief Called if the object shall be removed from the list of selected objects
    long onCmdRemoveSelected(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    GUIGLObjectPopupMenu();

    /// @brief The parent window
    GUISUMOAbstractView* myParent;

    /// @brief The object that belongs to this popup-menu
    GUIGlObject* myObject;

    /// @brief The main application
    GUIMainWindow* myApplication;

    /// @brief popup type;
    const PopupType myPopupType;

    /// @brief The position within the network the cursor was above when instanting the popup
    const Position myNetworkPosition;

    /// @brief The test coordinates position when instanting the popup
    const std::string myTestCoordinates;

    /// @brief vector mit Sub-MenuPanes
    std::vector<FXMenuPane*> myMenuPanes;
};
