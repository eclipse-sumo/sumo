//---------------------------------------------------------------------------//
//                        GUIManip_LaneSpeedTrigger.cpp -
//  Manipulation GUI for variable speed signs
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
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
// Revision 1.3  2004/11/23 10:06:50  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.2  2004/07/05 09:31:48  dkrajzew
// bug on non-MSVC-conform object initialisation patched
//
// Revision 1.1  2004/07/02 08:24:33  dkrajzew
// possibility to manipulate vss in the gui added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIManip_LaneSpeedTrigger.h"
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUILaneSpeedTrigger.h>


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIManip_LaneSpeedTrigger) GUIManip_LaneSpeedTriggerMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUIManip_LaneSpeedTrigger::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUIManip_LaneSpeedTrigger::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUIManip_LaneSpeedTrigger::MID_OPTION,   GUIManip_LaneSpeedTrigger::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUIManip_LaneSpeedTrigger::MID_CLOSE,    GUIManip_LaneSpeedTrigger::onCmdClose),
};

FXIMPLEMENT(GUIManip_LaneSpeedTrigger, GUIManipulator, GUIManip_LaneSpeedTriggerMap, ARRAYNUMBER(GUIManip_LaneSpeedTriggerMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIManip_LaneSpeedTrigger::GUIManip_LaneSpeedTrigger(
        GUIMainWindow &app,
        const std::string &name, GUILaneSpeedTrigger &o,
        int xpos, int ypos)
    : GUIManipulator(app, name, 0, 0), myChosenValue(0),
    myParent(&app),
    myChosenTarget(myChosenValue, this, MID_OPTION), mySpeedTarget(mySpeed),
    mySpeed(o.getDefaultSpeed()),
    myObject(&o)
{
    FXVerticalFrame *f1 = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

    FXGroupBox *gp = new FXGroupBox(f1, "Change Speed",
        GROUPBOX_TITLE_LEFT|FRAME_RIDGE,
        0, 0, 0, 0,  4, 4, 1, 1, 2, 0);
    FXHorizontalFrame *gf0 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf0, "Loaded", &myChosenTarget, FXDataTarget::ID_OPTION+12,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf1 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf1, "Default", &myChosenTarget, FXDataTarget::ID_OPTION+0,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf2 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf2, "20km/h", &myChosenTarget, FXDataTarget::ID_OPTION+1,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf3 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf3, "40km/h", &myChosenTarget, FXDataTarget::ID_OPTION+2,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf4 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf4, "60km/h", &myChosenTarget, FXDataTarget::ID_OPTION+3,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf5 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf5, "80km/h", &myChosenTarget, FXDataTarget::ID_OPTION+4,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf6 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf6, "100km/h", &myChosenTarget, FXDataTarget::ID_OPTION+5,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf7 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf7, "120km/h", &myChosenTarget, FXDataTarget::ID_OPTION+6,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf8 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf8, "140km/h", &myChosenTarget, FXDataTarget::ID_OPTION+7,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf9 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf9, "160km/h", &myChosenTarget, FXDataTarget::ID_OPTION+8,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf10 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf10, "180km/h", &myChosenTarget, FXDataTarget::ID_OPTION+9,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf11 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf11, "200km/h", &myChosenTarget, FXDataTarget::ID_OPTION+10,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    FXHorizontalFrame *gf12 =
        new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf12, "Free Entry:", &myChosenTarget, FXDataTarget::ID_OPTION+11,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
        myUserDefinedSpeed =
            new FXRealSpinDial(gf12, 10, this, MID_USER_DEF,
                LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myUserDefinedSpeed->setFormatString("%.0f km/h");
        myUserDefinedSpeed->setIncrements(1,10,10);
        myUserDefinedSpeed->setRange(10,300);
        myUserDefinedSpeed->setValue(
            static_cast<GUILaneSpeedTrigger*>(myObject)->getDefaultSpeed()*3.6);


    new FXButton(f1,"Close",NULL,this,MID_CLOSE,
        BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);

    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverriding(true);
}


GUIManip_LaneSpeedTrigger::~GUIManip_LaneSpeedTrigger()
{
}


long
GUIManip_LaneSpeedTrigger::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUIManip_LaneSpeedTrigger::onCmdUserDef(FXObject*,FXSelector,void*)
{
    mySpeed = myUserDefinedSpeed->getValue() / 3.6;
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverridingValue(mySpeed);
    myParent->updateChildren();
    return 1;
}


long
GUIManip_LaneSpeedTrigger::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myChosenValue!=11 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    myParent->updateChildren();
    return 1;
}


long
GUIManip_LaneSpeedTrigger::onCmdChangeOption(FXObject*,FXSelector,void*)
{
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverriding(true);
    switch(myChosenValue) {
    case 0:
        mySpeed = static_cast<GUILaneSpeedTrigger*>(myObject)->getDefaultSpeed();
        break;
    case 1:
        mySpeed = 20 / 3.6;
        break;
    case 2:
        mySpeed = 40 / 3.6;
        break;
    case 3:
        mySpeed = 60 / 3.6;
        break;
    case 4:
        mySpeed = 80 / 3.6;
        break;
    case 5:
        mySpeed = 100 / 3.6;
        break;
    case 6:
        mySpeed = 120 / 3.6;
        break;
    case 7:
        mySpeed = 140 / 3.6;
        break;
    case 8:
        mySpeed = 160 / 3.6;
        break;
    case 9:
        mySpeed = 180 / 3.6;
        break;
    case 10:
        mySpeed = 200 / 3.6;
        break;
    case 11:
        mySpeed = myUserDefinedSpeed->getValue() / 3.6;
        break;
    case 12:
        mySpeed = static_cast<GUILaneSpeedTrigger*>(myObject)->getLoadedSpeed();
        break;
    default:
        throw 1;
    }
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverridingValue(mySpeed);
    myParent->updateChildren();
    if(myChosenValue==12) {
        // !!! lock in between
        static_cast<GUILaneSpeedTrigger*>(myObject)->setOverriding(false);
    }
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


