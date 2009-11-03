/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The popup menu of a globject
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <cassert>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include "GUIGLObjectPopupMenu.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIGLObjectPopupMenu) GUIGLObjectPopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_CENTER,         GUIGLObjectPopupMenu::onCmdCenter),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_NAME,      GUIGLObjectPopupMenu::onCmdCopyName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_TYPED_NAME, GUIGLObjectPopupMenu::onCmdCopyTypedName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_CURSOR_POSITION, GUIGLObjectPopupMenu::onCmdCopyCursorPosition),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPARS,       GUIGLObjectPopupMenu::onCmdShowPars),
    FXMAPFUNC(SEL_COMMAND,  MID_ADDSELECT,      GUIGLObjectPopupMenu::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_REMOVESELECT,   GUIGLObjectPopupMenu::onCmdRemoveSelected)
};

// Object implementation
FXIMPLEMENT(GUIGLObjectPopupMenu, FXMenuPane, GUIGLObjectPopupMenuMap, ARRAYNUMBER(GUIGLObjectPopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIGLObjectPopupMenu::GUIGLObjectPopupMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent,
        GUIGlObject &o) throw()
        : FXMenuPane(&parent), myParent(&parent), myObject(&o),
        myApplication(&app), myNetworkPosition(parent.getPositionInformation()) {
}


GUIGLObjectPopupMenu::~GUIGLObjectPopupMenu() throw() {}


long
GUIGLObjectPopupMenu::onCmdCenter(FXObject*,FXSelector,void*) {
    myParent->centerTo(myObject);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyName(FXObject*,FXSelector,void*) {
    GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getMicrosimID());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyTypedName(FXObject*,FXSelector,void*) {
    GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getFullName());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyCursorPosition(FXObject*,FXSelector,void*) {
    GUIUserIO::copyToClipboard(*myParent->getApp(), toString(myNetworkPosition));
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdShowPars(FXObject*,FXSelector,void*) {
    myObject->getParameterWindow(*myApplication, *myParent); // !!! showParameterWindow would be more appropriate
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdAddSelected(FXObject*,FXSelector,void*) {
    gSelected.select(myObject->getType(), myObject->getGlID());
    myParent->update();
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdRemoveSelected(FXObject*,FXSelector,void*) {
    gSelected.deselect(myObject->getType(), myObject->getGlID());
    myParent->update();
    return 1;
}


/****************************************************************************/

