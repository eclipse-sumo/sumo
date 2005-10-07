//---------------------------------------------------------------------------//
//                        GUIDialog_AppSettings.cpp -
//  The application-settings dialog
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 08.03.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.3  2004/11/23 10:00:08  dkrajzew
// new class hierarchy for windows applied
//
// Revision 1.2  2004/04/02 10:56:20  dkrajzew
// first try to implement an option for switching off textures
//
// Revision 1.1  2004/03/19 12:33:36  dkrajzew
// porting to FOX
//
// Revision 1.1  2004/03/19 12:32:26  dkrajzew
// porting to FOX
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIDialog_AppSettings.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_AppSettings) GUIDialog_AppSettingsMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_QUITONSIMEND,    GUIDialog_AppSettings::onCmdQuitOnEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_SURPRESSENDINFO, GUIDialog_AppSettings::onCmdSurpressEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_ALLOWAGGREGATED, GUIDialog_AppSettings::onCmdAllowAggregated),
    FXMAPFUNC(SEL_COMMAND,  MID_ALLOWAFLOATING,  GUIDialog_AppSettings::onCmdAllowAggregatedFloating),
    FXMAPFUNC(SEL_COMMAND,  MID_ALLOWTEXTURES,   GUIDialog_AppSettings::onCmdAllowTextures),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,     GUIDialog_AppSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL, GUIDialog_AppSettings::onCmdCancel),

    FXMAPFUNC(SEL_UPDATE,   MID_ALLOWAFLOATING,  GUIDialog_AppSettings::onUpdAllowAggregatedFloating),
};

FXIMPLEMENT(GUIDialog_AppSettings, FXDialogBox, GUIDialog_AppSettingsMap, ARRAYNUMBER(GUIDialog_AppSettingsMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_AppSettings::GUIDialog_AppSettings(FXMainWindow* parent)
    : FXDialogBox( parent, "Application Settings" ),
    myAppQuitOnEnd(gQuitOnEnd), mySurpressEnd(gSuppressEndInfo),
    myAllowAggregated(gAllowAggregated),
    myAllowAggregatedFloating(gAllowAggregatedFloating),
    myAllowTextures(gAllowTextures)
{
    FXCheckButton *b = 0;
    FXVerticalFrame *f1 = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
    b = new FXCheckButton(f1, "Quit on Simulation End", this ,MID_QUITONSIMEND);
    b->setCheck(myAppQuitOnEnd);
    b = new FXCheckButton(f1, "Surpress End Information", this ,MID_SURPRESSENDINFO);
    b->setCheck(mySurpressEnd);
    new FXHorizontalSeparator(f1,SEPARATOR_GROOVE|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X);
    b = new FXCheckButton(f1, "Allow aggregated Views", this ,MID_ALLOWAGGREGATED);
    b->setCheck(myAllowAggregated);
    b = new FXCheckButton(f1, "Allow floating aggregated Views", this ,MID_ALLOWAFLOATING);
    b->setCheck(myAllowAggregatedFloating);
    b = new FXCheckButton(f1, "Allow Textures", this ,MID_ALLOWTEXTURES);
    b->setCheck(myAllowTextures);
    FXHorizontalFrame *f2 = new FXHorizontalFrame(f1, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
    FXButton *initial=new FXButton(f2,"&OK",NULL,this,MID_SETTINGS_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Cancel",NULL,this,MID_SETTINGS_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    initial->setFocus();
}


GUIDialog_AppSettings::~GUIDialog_AppSettings()
{
}


long
GUIDialog_AppSettings::onCmdOk(FXObject*,FXSelector,void*)
{
    gQuitOnEnd = myAppQuitOnEnd;
    gSuppressEndInfo = mySurpressEnd;
    gAllowAggregated = myAllowAggregated;
    gAllowAggregatedFloating = myAllowAggregatedFloating;
    gAllowTextures = myAllowTextures;
    destroy();
    return 1;
}


long
GUIDialog_AppSettings::onCmdCancel(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUIDialog_AppSettings::onCmdQuitOnEnd(FXObject*,FXSelector,void*)
{
    myAppQuitOnEnd = !myAppQuitOnEnd;
    return 1;
}


long
GUIDialog_AppSettings::onCmdSurpressEnd(FXObject*,FXSelector,void*)
{
    mySurpressEnd = !mySurpressEnd;
    return 1;
}


long
GUIDialog_AppSettings::onCmdAllowAggregated(FXObject*,FXSelector,void*)
{
    myAllowAggregated = !myAllowAggregated;
    if(!myAllowAggregated) {
        myAllowAggregatedFloating = false;
    }
    return 1;
}


long
GUIDialog_AppSettings::onCmdAllowAggregatedFloating(FXObject*,FXSelector,void*)
{
    myAllowAggregatedFloating = !myAllowAggregatedFloating;
    return 1;
}


long
GUIDialog_AppSettings::onUpdAllowAggregatedFloating(FXObject *sender,
                                                    FXSelector,
                                                    void *ptr)
{
    sender->handle(this,
        !myAllowAggregated?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIDialog_AppSettings::onCmdAllowTextures(FXObject*,FXSelector,void*)
{
    myAllowTextures = !myAllowTextures;
    return 1;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


