/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The popup menu of a globject.
/****************************************************************************/
#ifndef GUIGLObjectPopupMenu_h
#define GUIGLObjectPopupMenu_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <fx.h>
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
    /** @brief Constructor
     * @param[in] app The main window for instantiation of other windows
     * @param[in] parent The parent view for changing it
     * @param[in] o The object of interest
     */
    GUIGLObjectPopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o);

    /// @brief Destructor
    virtual ~GUIGLObjectPopupMenu();

    /// @brief Insert a sub-menu pane in this GUIGLObjectPopupMenu
    void insertMenuPaneChild(FXMenuPane* child);

    /// @brief return the real owner of this popup
    inline GUISUMOAbstractView* getParentView() {
        return myParent;
    }

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

    /// @brief Called if the cursor position shall be copied to clipboard
    long onCmdCopyCursorPosition(FXObject*, FXSelector, void*);

    /// @brief Called if the cursor geo-position shall be copied to clipboard
    long onCmdCopyCursorGeoPosition(FXObject*, FXSelector, void*);

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
    GUIGLObjectPopupMenu() { }

    /// @brief The parent window
    GUISUMOAbstractView* myParent;

    /// @brief The object that belongs to this popup-menu
    GUIGlObject* myObject;

    /// @brief The main application
    GUIMainWindow* myApplication;

    /// @brief The position within the network the cursor was above when instanting the popup
    Position myNetworkPosition;

    /// @brief vector mit Sub-MenuPanes
    std::vector<FXMenuPane*> myMenuPanes;
};


#endif

/****************************************************************************/

