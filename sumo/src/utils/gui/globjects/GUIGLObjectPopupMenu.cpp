/****************************************************************************/
/// @file    GUIGLObjectPopupMenu.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The popup menu which is displayed when pressing the right mouse button over
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <utils/gui/drawer/GUILaneRepresentation.h>
#include <utils/gui/div/GUIUserIO.h>

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
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_NAME,       GUIGLObjectPopupMenu::onCmdCopyName),
    FXMAPFUNC(SEL_COMMAND,  MID_COPY_TYPED_NAME, GUIGLObjectPopupMenu::onCmdCopyTypedName),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPARS,       GUIGLObjectPopupMenu::onCmdShowPars),
    FXMAPFUNC(SEL_COMMAND,  MID_ADDSELECT,      GUIGLObjectPopupMenu::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_ADDSELECT_SUCC, GUIGLObjectPopupMenu::onCmdAddSuccessorsSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_REMOVESELECT,   GUIGLObjectPopupMenu::onCmdRemoveSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_RENAME,         GUIGLObjectPopupMenu::onCmdRename),
    FXMAPFUNC(SEL_COMMAND,  MID_MOVETO,         GUIGLObjectPopupMenu::onCmdMoveTo),
    FXMAPFUNC(SEL_COMMAND,  MID_CHANGECOL,      GUIGLObjectPopupMenu::onCmdChangeCol),
    FXMAPFUNC(SEL_COMMAND,  MID_CHANGETYPE,     GUIGLObjectPopupMenu::onCmdChangeTyp),
    FXMAPFUNC(SEL_COMMAND,  MID_DELETE,         GUIGLObjectPopupMenu::onCmdDelete)
};

// Object implementation
FXIMPLEMENT(GUIGLObjectPopupMenu, FXMenuPane, GUIGLObjectPopupMenuMap, ARRAYNUMBER(GUIGLObjectPopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIGLObjectPopupMenu::GUIGLObjectPopupMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent,
        GUIGlObject &o)
        : FXMenuPane(&parent), myParent(&parent), myObject(&o),
        myApplication(&app)
{}


GUIGLObjectPopupMenu::~GUIGLObjectPopupMenu()
{}


long
GUIGLObjectPopupMenu::onCmdCenter(FXObject*,FXSelector,void*)
{
    myParent->centerTo(myObject);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyName(FXObject*,FXSelector,void*)
{
    GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->microsimID());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdCopyTypedName(FXObject*,FXSelector,void*)
{
    GUIUserIO::copyToClipboard(*myParent->getApp(), myObject->getFullName());
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdShowPars(FXObject*,FXSelector,void*)
{
    myObject->getParameterWindow(*myApplication, *myParent); // !!! showParameetrWindow would be more appropriate
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdAddSelected(FXObject*,FXSelector,void*)
{
    gSelected.select(myObject->getType(), myObject->getGlID());
    myParent->update();
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdAddSuccessorsSelected(FXObject*,FXSelector,void*)
{
    GUILaneRepresentation *lane = static_cast<GUILaneRepresentation*>(myObject);
    lane->selectSucessors();
    myParent->update();
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdRemoveSelected(FXObject*,FXSelector,void*)
{
    gSelected.deselect(myObject->getType(), myObject->getGlID());
    myParent->update();
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdRename(FXObject*,FXSelector,void*)
{
    myParent->rename(myObject);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdMoveTo(FXObject*,FXSelector,void*)
{
    myParent->moveTo(myObject);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdChangeCol(FXObject*,FXSelector,void*)
{
    myParent->changeCol(myObject);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdChangeTyp(FXObject*,FXSelector,void*)
{
    myParent->changeTyp(myObject);
    return 1;
}


long
GUIGLObjectPopupMenu::onCmdDelete(FXObject*,FXSelector,void*)
{
    myParent->deleteObj(myObject);
    return 1;
}



/****************************************************************************/

