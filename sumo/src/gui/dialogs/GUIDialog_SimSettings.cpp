/****************************************************************************/
/// @file    GUIDialog_SimSettings.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// The simulation-settings dialog
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

#include "GUIDialog_SimSettings.h"

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX-callbacks
// ===========================================================================
FXDEFMAP(GUIDialog_SimSettings) GUIDialog_SimSettingsMap[]= {
    //________Message_Type____________ID________________________Message_Handler________
    FXMAPFUNC(SEL_COMMAND,  MID_QUITONSIMEND,    GUIDialog_SimSettings::onCmdQuitOnEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_SUPPRESSENDINFO, GUIDialog_SimSettings::onCmdSuppressEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,     GUIDialog_SimSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL, GUIDialog_SimSettings::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GUIDialog_SimSettings, FXDialogBox, GUIDialog_SimSettingsMap, ARRAYNUMBER(GUIDialog_SimSettingsMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GUIDialog_SimSettings::GUIDialog_SimSettings(FXMainWindow* parent,
        bool *quitOnEnd, bool *suppressEnd)
        : FXDialogBox(parent, "Simulation Settings"),
        myAppQuitOnEnd(*quitOnEnd), mySuppressEnd(*suppressEnd),
        mySetAppQuitOnEnd(quitOnEnd), mySetSuppressEnd(suppressEnd)
{
    FXCheckButton *b = 0;
    FXVerticalFrame *f1 = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
    b = new FXCheckButton(f1, "Quit on Simulation End", this ,MID_QUITONSIMEND);
    b->setCheck(myAppQuitOnEnd);
    b = new FXCheckButton(f1, "Suppress End Information", this ,MID_SUPPRESSENDINFO);
    b->setCheck(mySuppressEnd);
    FXHorizontalFrame *f2 = new FXHorizontalFrame(f1, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
    FXButton *initial=new FXButton(f2,"&OK",NULL,this,MID_SETTINGS_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Cancel",NULL,this,MID_SETTINGS_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    initial->setFocus();
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_SimSettings::~GUIDialog_SimSettings()
{}


long
GUIDialog_SimSettings::onCmdOk(FXObject*,FXSelector,void*)
{
    *mySetAppQuitOnEnd = myAppQuitOnEnd;
    *mySetSuppressEnd = mySuppressEnd;
    destroy();
    return 1;
}


long
GUIDialog_SimSettings::onCmdCancel(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUIDialog_SimSettings::onCmdQuitOnEnd(FXObject*,FXSelector,void*)
{
    myAppQuitOnEnd = !myAppQuitOnEnd;
    return 1;
}


long
GUIDialog_SimSettings::onCmdSuppressEnd(FXObject*,FXSelector,void*)
{
    mySuppressEnd = !mySuppressEnd;
    return 1;
}



/****************************************************************************/

