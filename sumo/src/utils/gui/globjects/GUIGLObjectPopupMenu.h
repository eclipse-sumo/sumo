/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The popup menu of a globject
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIGLObjectPopupMenu_h
#define GUIGLObjectPopupMenu_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <fx.h>
#include <utils/geom/Position2D.h>


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
    GUIGLObjectPopupMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent, GUIGlObject &o) throw();


    /// @brief Destructor
    virtual ~GUIGLObjectPopupMenu() throw();


public:
    /// Called if the assigned objects shall be centered
    long onCmdCenter(FXObject*,FXSelector,void*);

    /// Called if the name shall be copied to clipboard
    long onCmdCopyName(FXObject*,FXSelector,void*);

    /// Called if the typed name shall be copied to clipboard
    long onCmdCopyTypedName(FXObject*,FXSelector,void*);

    /// Called if the cursor position shall be copied to clipboard
    long onCmdCopyCursorPosition(FXObject*,FXSelector,void*);

    /// Called if the parameter of this object shall be shown
    long onCmdShowPars(FXObject*,FXSelector,void*);

    /// Called if the object shall be added to the list of selected objects
    long onCmdAddSelected(FXObject*,FXSelector,void*);

    /// Called if the object shall be removed from the list of selected objects
    long onCmdRemoveSelected(FXObject*,FXSelector,void*);



protected:
    /// @brief The parent window
    GUISUMOAbstractView *myParent;

    /// @brief The object that belongs to this popup-menu
    GUIGlObject *myObject;

    /// @brief The main application
    GUIMainWindow *myApplication;

    /// @brief The position within the network the cursor was above when instanting the popup
    Position2D myNetworkPosition;


protected:
    /// @brief FOX needs this
    GUIGLObjectPopupMenu() { }


};


#endif

/****************************************************************************/

