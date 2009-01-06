/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The popup menu which is displayed when pressing the right mouse button over
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
 */
class GUIGLObjectPopupMenu : public FXMenuPane
{
    // FOX-declarations
    FXDECLARE(GUIGLObjectPopupMenu)
public:
    /// Constructor
    GUIGLObjectPopupMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent, GUIGlObject &o);

    /// Destructor
    virtual ~GUIGLObjectPopupMenu();

public:
    /// Called if the assigned objects shall be centered
    long onCmdCenter(FXObject*,FXSelector,void*);

    /// Called if the name shall be copied to clipboard
    long onCmdCopyName(FXObject*,FXSelector,void*);

    /// Called if the typed name shall be copied to clipboard
    long onCmdCopyTypedName(FXObject*,FXSelector,void*);

    /// Called if the parameter of this object shall be shown
    long onCmdShowPars(FXObject*,FXSelector,void*);

    /// Called if the object shall be added to the list of selected objects
    long onCmdAddSelected(FXObject*,FXSelector,void*);

    /// Called if the object shall be removed from the list of selected objects
    long onCmdRemoveSelected(FXObject*,FXSelector,void*);

    /// Called if the assigned objects shall be centered
    long onCmdRename(FXObject*,FXSelector,void*);

    /// Called if the assigned objects shall be moved
    long onCmdMoveTo(FXObject*,FXSelector,void*);

    /// Called if the assigned objects shall be recolored
    long onCmdChangeCol(FXObject*,FXSelector,void*);

    /// Called if the assigned objects shall be changed in typ
    long onCmdChangeTyp(FXObject*,FXSelector,void*);

    /// Called if the assigned objects shall be deleted
    long onCmdDelete(FXObject*,FXSelector,void*);


protected:
    /// The parent window
    GUISUMOAbstractView *myParent; // !!! needed?

    /// The object that belongs to this popup-menu
    GUIGlObject *myObject;

    /// The main application
    GUIMainWindow *myApplication;

protected:
    /// FOX needs this
    GUIGLObjectPopupMenu() { }

};


#endif

/****************************************************************************/

