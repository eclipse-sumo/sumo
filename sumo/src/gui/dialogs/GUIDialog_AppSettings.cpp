/****************************************************************************/
/// @file    GUIDialog_AppSettings.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// The application-settings dialog
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIDialog_AppSettings.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_AppSettings) GUIDialog_AppSettingsMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_QUITONSIMEND,    GUIDialog_AppSettings::onCmdSelect),
    FXMAPFUNC(SEL_COMMAND, MID_AUTOSTART,       GUIDialog_AppSettings::onCmdSelect),
    FXMAPFUNC(SEL_COMMAND, MID_DEMO,            GUIDialog_AppSettings::onCmdSelect),
    FXMAPFUNC(SEL_COMMAND, MID_ALLOWTEXTURES,   GUIDialog_AppSettings::onCmdSelect),
    FXMAPFUNC(SEL_COMMAND, MID_LOCATELINKS,     GUIDialog_AppSettings::onCmdSelect),
    FXMAPFUNC(SEL_COMMAND, MID_SETTINGS_OK,     GUIDialog_AppSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND, MID_SETTINGS_CANCEL, GUIDialog_AppSettings::onCmdCancel),
};

FXIMPLEMENT(GUIDialog_AppSettings, FXDialogBox, GUIDialog_AppSettingsMap, ARRAYNUMBER(GUIDialog_AppSettingsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_AppSettings::GUIDialog_AppSettings(FXMainWindow* parent)
    : FXDialogBox(parent, "Application Settings"),
      myAppQuitOnEnd(GUIGlobals::gQuitOnEnd),
      myAppAutoStart(GUIGlobals::gRunAfterLoad),
      myAppDemo(GUIGlobals::gDemoAutoReload),
      myAllowTextures(GUITexturesHelper::texturesAllowed()),
      myLocateLinks(GUIMessageWindow::locateLinksEnabled()) {
    FXCheckButton* b = 0;
    FXVerticalFrame* f1 = new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    b = new FXCheckButton(f1, "Quit on Simulation End", this, MID_QUITONSIMEND);
    b->setCheck(myAppQuitOnEnd);
    b = new FXCheckButton(f1, "Autostart Simulation on Load and Reload", this, MID_AUTOSTART);
    b->setCheck(myAppAutoStart);
    b = new FXCheckButton(f1, "Reload Simulation after finish (Demo mode)", this, MID_DEMO);
    b->setCheck(myAppDemo);
    b = new FXCheckButton(f1, "Locate elements when clicking on messages", this, MID_LOCATELINKS);
    b->setCheck(myLocateLinks);
    new FXHorizontalSeparator(f1, SEPARATOR_GROOVE | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X);
    b = new FXCheckButton(f1, "Allow Textures", this , MID_ALLOWTEXTURES);
    b->setCheck(myAllowTextures);
    FXHorizontalFrame* f2 = new FXHorizontalFrame(f1, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 10, 10, 5, 5);
    FXButton* initial = new FXButton(f2, "&OK", NULL, this, MID_SETTINGS_OK, BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 30, 30, 4, 4);
    new FXButton(f2, "&Cancel", NULL, this, MID_SETTINGS_CANCEL, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 30, 30, 4, 4);
    initial->setFocus();
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_AppSettings::~GUIDialog_AppSettings() {}


long
GUIDialog_AppSettings::onCmdOk(FXObject*, FXSelector, void*) {
    GUIGlobals::gQuitOnEnd = myAppQuitOnEnd;
    GUIGlobals::gRunAfterLoad = myAppAutoStart;
    GUIGlobals::gDemoAutoReload = myAppDemo;
    GUIGlobals::gRunAfterLoad = myAppAutoStart;
    GUITexturesHelper::allowTextures(myAllowTextures);
    GUIMessageWindow::enableLocateLinks(myLocateLinks);
    destroy();
    return 1;
}


long
GUIDialog_AppSettings::onCmdCancel(FXObject*, FXSelector, void*) {
    destroy();
    return 1;
}


long
GUIDialog_AppSettings::onCmdSelect(FXObject*, FXSelector sel, void*) {
    switch (FXSELID(sel)) {
        case MID_QUITONSIMEND:
            myAppQuitOnEnd = !myAppQuitOnEnd;
            break;
        case MID_AUTOSTART:
            myAppAutoStart = !myAppAutoStart;
            break;
        case MID_DEMO:
            myAppDemo = !myAppDemo;
            break;
        case MID_LOCATELINKS:
            myLocateLinks = !myLocateLinks;
            break;
        case MID_ALLOWTEXTURES:
            myAllowTextures = !myAllowTextures;
            break;
    }
    return 1;
}


/****************************************************************************/
