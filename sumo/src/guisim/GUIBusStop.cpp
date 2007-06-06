/****************************************************************************/
/// @file    GUIBusStop.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// Class that realises the setting of a lane's maximum speed triggered by
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/Boundary.h>
#include <utils/glutils/GLHelper.h>
#include <utils/common/ToString.h>
#include <utils/helpers/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUIBusStop.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/glutils/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUIBusStop.h>

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
/* -------------------------------------------------------------------------
 * GUIBusStop::GUIBusStopPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIBusStop::GUIBusStopPopupMenu)
GUIBusStopPopupMenuMap[]=
    {
        FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUIBusStop::GUIBusStopPopupMenu::onCmdOpenManip),

    };

// Object implementation
FXIMPLEMENT(GUIBusStop::GUIBusStopPopupMenu, GUIGLObjectPopupMenu, GUIBusStopPopupMenuMap, ARRAYNUMBER(GUIBusStopPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUIBusStop::GUIManip_LaneSpeedTrigger - mapping
 * ----------------------------------------------------------------------- */
/*
FXDEFMAP(GUIBusStop::GUIManip_LaneSpeedTrigger) GUIManip_LaneSpeedTriggerMap[]=
{
   FXMAPFUNC(SEL_COMMAND,  GUIBusStop::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdUserDef),
   FXMAPFUNC(SEL_UPDATE,   GUIBusStop::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUIBusStop::GUIManip_LaneSpeedTrigger::onUpdUserDef),
   FXMAPFUNC(SEL_COMMAND,  GUIBusStop::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdPreDef),
   FXMAPFUNC(SEL_UPDATE,   GUIBusStop::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUIBusStop::GUIManip_LaneSpeedTrigger::onUpdPreDef),
   FXMAPFUNC(SEL_COMMAND,  GUIBusStop::GUIManip_LaneSpeedTrigger::MID_OPTION,   GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdChangeOption),
   FXMAPFUNC(SEL_COMMAND,  GUIBusStop::GUIManip_LaneSpeedTrigger::MID_CLOSE,    GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdClose),
};

FXIMPLEMENT(GUIBusStop::GUIManip_LaneSpeedTrigger, GUIManipulator, GUIManip_LaneSpeedTriggerMap, ARRAYNUMBER(GUIManip_LaneSpeedTriggerMap))
*/

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIBusStop::GUIManip_LaneSpeedTrigger - methods
 * ----------------------------------------------------------------------- */
/*
GUIBusStop::GUIManip_LaneSpeedTrigger::GUIManip_LaneSpeedTrigger(
       GUIMainWindow &app,
       const std::string &name, GUIBusStop &o,
       int xpos, int ypos)
   : GUIManipulator(app, name, 0, 0), myChosenValue(0),
   myParent(&app),
   myChosenTarget(myChosenValue, this, MID_OPTION), mySpeedTarget(mySpeed),
   mySpeed(o.getDefaultSpeed()),
   myObject(&o)
{
   FXVerticalFrame *f1 =
       new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

   FXGroupBox *gp = new FXGroupBox(f1, "Change Speed",
       GROUPBOX_TITLE_LEFT|FRAME_RIDGE,
       0, 0, 0, 0,  4, 4, 1, 1, 2, 0);
   {
       // default
       FXHorizontalFrame *gf1 =
           new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
       new FXRadioButton(gf1, "Default", &myChosenTarget, FXDataTarget::ID_OPTION+0,
           ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
           0, 0, 0, 0,   2, 2, 0, 0);
   }
   {
       // loaded
       FXHorizontalFrame *gf0 =
           new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
       new FXRadioButton(gf0, "Loaded", &myChosenTarget, FXDataTarget::ID_OPTION+1,
           ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
           0, 0, 0, 0,   2, 2, 0, 0);
   }
   {
       // predefined
       FXHorizontalFrame *gf2 =
           new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
       new FXRadioButton(gf2, "Predefined: ", &myChosenTarget, FXDataTarget::ID_OPTION+2,
           ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y,
           0, 0, 0, 0,   2, 2, 0, 0);
       myPredefinedValues =
           new FXComboBox(gf2, 10, this, MID_PRE_DEF,
               ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y);
       myPredefinedValues->appendItem("20 km/h");
       myPredefinedValues->appendItem("40 km/h");
       myPredefinedValues->appendItem("60 km/h");
       myPredefinedValues->appendItem("80 km/h");
       myPredefinedValues->appendItem("100 km/h");
       myPredefinedValues->appendItem("120 km/h");
       myPredefinedValues->appendItem("140 km/h");
       myPredefinedValues->appendItem("160 km/h");
       myPredefinedValues->appendItem("180 km/h");
       myPredefinedValues->appendItem("200 km/h");
       myPredefinedValues->setNumVisible(5);
   }
   {
       // free
       FXHorizontalFrame *gf12 =
           new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
       new FXRadioButton(gf12, "Free Entry: ", &myChosenTarget, FXDataTarget::ID_OPTION+3,
           ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y,
           0, 0, 0, 0,   2, 2, 0, 0);
       myUserDefinedSpeed =
           new FXRealSpinDial(gf12, 10, this, MID_USER_DEF,
               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
       myUserDefinedSpeed->setFormatString("%.0f km/h");
       myUserDefinedSpeed->setIncrements(1,10,10);
       myUserDefinedSpeed->setRange(0,300);
       myUserDefinedSpeed->setValue(
           static_cast<GUIBusStop*>(myObject)->getDefaultSpeed()*3.6);
   }
   new FXButton(f1,"Close",NULL,this,MID_CLOSE,
       BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
   static_cast<GUIBusStop*>(myObject)->setOverriding(true);
}


GUIBusStop::GUIManip_LaneSpeedTrigger::~GUIManip_LaneSpeedTrigger()
{
}


long
GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdClose(FXObject*,FXSelector,void*)
{
   destroy();
   return 1;
}


long
GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdUserDef(FXObject*,FXSelector,void*)
{
   mySpeed = (SUMOReal) (myUserDefinedSpeed->getValue() / 3.6);
   static_cast<GUIBusStop*>(myObject)->setOverridingValue(mySpeed);
   myParent->updateChildren();
   return 1;
}


long
GUIBusStop::GUIManip_LaneSpeedTrigger::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
   sender->handle(this,
       myChosenValue!=3 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
       ptr);
   myParent->updateChildren();
   return 1;
}


long
GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdPreDef(FXObject*,FXSelector,void*)
{
   mySpeed = (SUMOReal) (SUMOReal) ((myPredefinedValues->getCurrentItem() * 20 + 20)/3.6);
   static_cast<GUIBusStop*>(myObject)->setOverridingValue(mySpeed);
   myParent->updateChildren();
   return 1;
}


long
GUIBusStop::GUIManip_LaneSpeedTrigger::onUpdPreDef(FXObject *sender,FXSelector,void*ptr)
{
   sender->handle(this,
       myChosenValue!=2 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
       ptr);
   myParent->updateChildren();
   return 1;
}


long
GUIBusStop::GUIManip_LaneSpeedTrigger::onCmdChangeOption(FXObject*,FXSelector,void*)
{
}
*/


/* -------------------------------------------------------------------------
 * GUIBusStop::GUIBusStopPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIBusStop::GUIBusStopPopupMenu::GUIBusStopPopupMenu(
    GUIMainWindow &app, GUISUMOAbstractView &parent,
    GUIGlObject &o)
        : GUIGLObjectPopupMenu(app, parent, o)
{}


GUIBusStop::GUIBusStopPopupMenu::~GUIBusStopPopupMenu()
{}


long
GUIBusStop::GUIBusStopPopupMenu::onCmdOpenManip(FXObject*, FXSelector, void*)
{
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIBusStop - methods
 * ----------------------------------------------------------------------- */
GUIBusStop::GUIBusStop(const std::string &id, MSNet &,
                       const std::vector<std::string> &lines, MSLane &lane,
                       SUMOReal frompos, SUMOReal topos)
        : MSBusStop(id, lines, lane, frompos, topos),
        GUIGlObject_AbstractAdd(gIDStorage, "bus_stop:" + id, GLO_TRIGGER)
{
    // full geometry
    {
        myFGShape = lane.getShape();
        myFGShape.move2side((SUMOReal) 1.65);
        myFGShape = myFGShape.getSubpart(frompos, topos);
        myFGShapeRotations.reserve(myFGShape.size()-1);
        myFGShapeLengths.reserve(myFGShape.size()-1);
        for (size_t i=0; i<myFGShape.size()-1; i++) {
            const Position2D &f = myFGShape[i];
            const Position2D &s = myFGShape[i+1];
            myFGShapeLengths.push_back(GeomHelper::distance(f, s));
            myFGShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265);
        }
        Position2DVector tmp = myFGShape;
        tmp.move2side(1.5);
        myFGSignPos = tmp.center();
    }
}


GUIBusStop::~GUIBusStop()
{}


GUIGLObjectPopupMenu *
GUIBusStop::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIBusStopPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //(nothing to show, see below) buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIBusStop::getParameterWindow(GUIMainWindow &,
                               GUISUMOAbstractView &)
{
    return 0;
}


const std::string &
GUIBusStop::microsimID() const
{
    return getID();
}


bool
GUIBusStop::active() const
{
    return true;
}


Position2D
GUIBusStop::getPosition() const
{
    return myFGShape.center(); //!!!!
}


void
GUIBusStop::drawGL(SUMOReal scale, SUMOReal upscale)
{
    // draw the area
    glColor3f((SUMOReal)(76./255.), (SUMOReal)(170./255.), (SUMOReal)(50./255.));
    size_t i;
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, 1.0);
    // draw the lines
    if (scale>=35) {
        glColor3f((SUMOReal)(76./255.), (SUMOReal)(170./255.), (SUMOReal)(50./255.));
        for (i=0; i!=myLines.size(); ++i) {
            glPushMatrix();
            glScaled(upscale, upscale, upscale);
            glTranslated(myFGSignPos.x()+1.2, myFGSignPos.y()+0.8, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(.5f);
            //glRotated(180, 0, 1, 0);
            glTranslated(0, -(double)i*.5, 0);
            glScaled(1, -1, 1);
            pfDrawString(myLines[i].c_str());
            glPopMatrix();
        }
    }

    // draw the sign
    glPushMatrix();
    glScaled(upscale, upscale, upscale);
    glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
    //glRotated( rot, 0, 0, 1 );
    //glTranslated(0, -1.5, 0);
    int noPoints = 9;
    if (scale>25) {
        noPoints = (int)(9.0 + scale / 10.0);
        if (noPoints>36) {
            noPoints = 36;
        }
    }
    glColor3f((SUMOReal)(76./255.), (SUMOReal)(170./255.), (SUMOReal)(50./255.));
    GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
    if (scale<10) {
        glPopMatrix();
        return;
    }
    glColor3f((SUMOReal)(255./255.), (SUMOReal)(235./255.), (SUMOReal)(0./255.));
    GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);
    // draw the H
    // not if scale to low
    if (scale<4.5) {
        glPopMatrix();
        return;
    }
    glColor3f((SUMOReal)(76./255.), (SUMOReal)(170./255.), (SUMOReal)(50./255.));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    pfSetPosition(0, 0);
    pfSetScale(1.6f);
    SUMOReal w = pfdkGetStringWidth("H");
    glRotated(180, 0, 1, 0);
    glTranslated(-w/2., 0.4, 0);
    pfDrawString("H");
    glPopMatrix();
}


Boundary
GUIBusStop::getBoundary() const
{
    /* !!! */
    Position2D pos = getPosition();
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUIBusStop::openManipulator(GUIMainWindow &,
                            GUISUMOAbstractView &)
{
    /*
    GUIManip_LaneSpeedTrigger *gui =
        new GUIManip_LaneSpeedTrigger(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
    */
    return 0;
}



/****************************************************************************/

