//---------------------------------------------------------------------------//
//                        GUIDialog_SimSettings.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.4  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIDialog_SimSettings.h"

#include <utils/gui/windows/GUIAppEnum.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
FXDEFMAP(GUIDialog_SimSettings) GUIDialog_SimSettingsMap[]=
{
    //________Message_Type____________ID________________________Message_Handler________
    FXMAPFUNC(SEL_COMMAND,  MID_QUITONSIMEND,    GUIDialog_SimSettings::onCmdQuitOnEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_SURPRESSENDINFO, GUIDialog_SimSettings::onCmdSurpressEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_ALLOWAGGREGATED, GUIDialog_SimSettings::onCmdAllowAggregated),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,     GUIDialog_SimSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL, GUIDialog_SimSettings::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GUIDialog_SimSettings, FXDialogBox, GUIDialog_SimSettingsMap, ARRAYNUMBER(GUIDialog_SimSettingsMap))




GUIDialog_SimSettings::GUIDialog_SimSettings(FXMainWindow* parent,
                                             bool *quitOnEnd,
                                             bool *surpressEnd,
                                             bool *allowFloating)
    : FXDialogBox( parent, "Application Settings" ),
    myAppQuitOnEnd(*quitOnEnd), mySurpressEnd(*surpressEnd),
    myAllowFloating(*allowFloating),
    mySetAppQuitOnEnd(quitOnEnd), mySetSurpressEnd(surpressEnd),
    mySetAllowFloating(allowFloating)
{
    FXCheckButton *b = 0;
    FXVerticalFrame *f1 = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
    b = new FXCheckButton(f1, "Quit on Simulation End", this ,MID_QUITONSIMEND);
    b->setCheck(myAppQuitOnEnd);
    b = new FXCheckButton(f1, "Surpress End Information", this ,MID_SURPRESSENDINFO);
    b->setCheck(mySurpressEnd);
    b = new FXCheckButton(f1, "Allow floating aggregated Views", this ,MID_ALLOWAGGREGATED);
    b->setCheck(myAllowFloating);
    FXHorizontalFrame *f2 = new FXHorizontalFrame(f1, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
    FXButton *initial=new FXButton(f2,"&OK",NULL,this,MID_SETTINGS_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Cancel",NULL,this,MID_SETTINGS_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    initial->setFocus();
}


GUIDialog_SimSettings::~GUIDialog_SimSettings()
{
}


long
GUIDialog_SimSettings::onCmdOk(FXObject*,FXSelector,void*)
{
    *mySetAppQuitOnEnd = myAppQuitOnEnd;
    *mySetSurpressEnd = mySurpressEnd;
    *mySetAllowFloating = myAllowFloating;
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
GUIDialog_SimSettings::onCmdSurpressEnd(FXObject*,FXSelector,void*)
{
    mySurpressEnd = !mySurpressEnd;
    return 1;
}


long
GUIDialog_SimSettings::onCmdAllowAggregated(FXObject*,FXSelector,void*)
{
    myAllowFloating = !myAllowFloating;
    return 1;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
