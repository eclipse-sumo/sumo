#ifndef GUIGLObjectPopupMenu_h
#define GUIGLObjectPopupMenu_h
//---------------------------------------------------------------------------//
//                        GUIGLObjectPopupMenu.h -
//  The popup menu which is displayed when pressing the right mouse button over
//  a gl-object
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2004/07/02 08:25:32  dkrajzew
// possibility to manipulate objects added
//
// Revision 1.1  2004/03/19 12:41:13  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/12 14:09:39  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.2  2003/07/30 08:49:26  dkrajzew
// changed the responsibility of a GLObject
//
// Revision 1.1  2003/06/06 10:24:36  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under
//  linux; GUIGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.2  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <fx.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUISUMOAbstractView;
class GUIGlObject;
class GUIApplicationWindow;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIGLObjectPopupMenu
 */
class GUIGLObjectPopupMenu : public FXMenuPane
{
    // FOX-declarations
    FXDECLARE(GUIGLObjectPopupMenu)
public:
    /// Constructor
    GUIGLObjectPopupMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent, GUIGlObject &o);

    /// Destructor
    ~GUIGLObjectPopupMenu();

public:
    /// Called if the assigned objects shall be centered
    long onCmdCenter(FXObject*,FXSelector,void*);

    /// Called if the parameter of this object shall be shown
    long onCmdShowPars(FXObject*,FXSelector,void*);

    /// Called if the phases shall be shown (only valid if the object is a tllogic
    long onCmdShowPhases(FXObject*,FXSelector,void*);

    /// Called if the object shall be added to the list of selected objects
    long onCmdAddSelected(FXObject*,FXSelector,void*);

    /// Called if the object shall be removed from the list of selected objects
    long onCmdRemoveSelected(FXObject*,FXSelector,void*);

    /** @brief Called if the object's manipulator shall be shown
        This is only valid if the current object is derived from GUIIHaveManipulator */
    long onCmdOpenManip(FXObject*,FXSelector,void*);

protected:
    /// The parent window
    GUISUMOAbstractView *myParent; // !!! needed?

    /// The object that belongs to this popup-menu
    GUIGlObject *myObject;

    /// The main application
    GUIApplicationWindow *myApplication;

protected:
    /// FOX needs this
    GUIGLObjectPopupMenu() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

