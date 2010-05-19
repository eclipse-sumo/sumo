/****************************************************************************/
/// @file    GUIDialog_AppSettings.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// The application-settings dialog
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include "GUIDialog_AppSettings.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_AppSettings) GUIDialog_AppSettingsMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_QUITONSIMEND,    GUIDialog_AppSettings::onCmdQuitOnEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_ALLOWTEXTURES,   GUIDialog_AppSettings::onCmdAllowTextures),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,     GUIDialog_AppSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL, GUIDialog_AppSettings::onCmdCancel),
};

FXIMPLEMENT(GUIDialog_AppSettings, FXDialogBox, GUIDialog_AppSettingsMap, ARRAYNUMBER(GUIDialog_AppSettingsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_AppSettings::GUIDialog_AppSettings(FXMainWindow* parent) throw()
        : FXDialogBox(parent, "Application Settings"),
        myAppQuitOnEnd(gQuitOnEnd),
        myAllowTextures(gAllowTextures) {
    FXCheckButton *b = 0;
    FXVerticalFrame *f1 = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
    b = new FXCheckButton(f1, "Quit on Simulation End", this ,MID_QUITONSIMEND);
    b->setCheck(myAppQuitOnEnd);
    new FXHorizontalSeparator(f1,SEPARATOR_GROOVE|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X);
    b = new FXCheckButton(f1, "Allow Textures", this ,MID_ALLOWTEXTURES);
    b->setCheck(myAllowTextures);
    b->disable();
    FXHorizontalFrame *f2 = new FXHorizontalFrame(f1, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
    FXButton *initial=new FXButton(f2,"&OK",NULL,this,MID_SETTINGS_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Cancel",NULL,this,MID_SETTINGS_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    initial->setFocus();
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_AppSettings::~GUIDialog_AppSettings() throw() {}


long
GUIDialog_AppSettings::onCmdOk(FXObject*,FXSelector,void*) {
    gQuitOnEnd = myAppQuitOnEnd;
    gAllowTextures = myAllowTextures;
    destroy();
    return 1;
}


long
GUIDialog_AppSettings::onCmdCancel(FXObject*,FXSelector,void*) {
    destroy();
    return 1;
}


long
GUIDialog_AppSettings::onCmdQuitOnEnd(FXObject*,FXSelector,void*) {
    myAppQuitOnEnd = !myAppQuitOnEnd;
    return 1;
}


long
GUIDialog_AppSettings::onCmdAllowTextures(FXObject*,FXSelector,void*) {
    myAllowTextures = !myAllowTextures;
    return 1;
}



/****************************************************************************/

