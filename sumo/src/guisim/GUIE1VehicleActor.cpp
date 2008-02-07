/****************************************************************************/
/// @file    GUIE1VehicleActor.cpp
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
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUIE1VehicleActor.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUIE1VehicleActor.h>

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
 * GUIE1VehicleActor::GUIE1VehicleActorPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIE1VehicleActor::GUIE1VehicleActorPopupMenu)
GUIE1VehicleActorPopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUIE1VehicleActor::GUIE1VehicleActorPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUIE1VehicleActor::GUIE1VehicleActorPopupMenu, GUIGLObjectPopupMenu, GUIE1VehicleActorPopupMenuMap, ARRAYNUMBER(GUIE1VehicleActorPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUIE1VehicleActor::GUIManip_LaneSpeedTrigger - mapping
 * ----------------------------------------------------------------------- */
/*
FXDEFMAP(GUIE1VehicleActor::GUIManip_LaneSpeedTrigger) GUIManip_LaneSpeedTriggerMap[]=
{
   FXMAPFUNC(SEL_COMMAND,  GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdUserDef),
   FXMAPFUNC(SEL_UPDATE,   GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onUpdUserDef),
   FXMAPFUNC(SEL_COMMAND,  GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdPreDef),
   FXMAPFUNC(SEL_UPDATE,   GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onUpdPreDef),
   FXMAPFUNC(SEL_COMMAND,  GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::MID_OPTION,   GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdChangeOption),
   FXMAPFUNC(SEL_COMMAND,  GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::MID_CLOSE,    GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdClose),
};

FXIMPLEMENT(GUIE1VehicleActor::GUIManip_LaneSpeedTrigger, GUIManipulator, GUIManip_LaneSpeedTriggerMap, ARRAYNUMBER(GUIManip_LaneSpeedTriggerMap))
*/

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIE1VehicleActor::GUIManip_LaneSpeedTrigger - methods
 * ----------------------------------------------------------------------- */
/*
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::GUIManip_LaneSpeedTrigger(
       GUIMainWindow &app,
       const std::string &name, GUIE1VehicleActor &o,
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
           static_cast<GUIE1VehicleActor*>(myObject)->getDefaultSpeed()*3.6);
   }
   new FXButton(f1,"Close",NULL,this,MID_CLOSE,
       BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
   static_cast<GUIE1VehicleActor*>(myObject)->setOverriding(true);
}


GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::~GUIManip_LaneSpeedTrigger()
{
}


long
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdClose(FXObject*,FXSelector,void*)
{
   destroy();
   return 1;
}


long
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdUserDef(FXObject*,FXSelector,void*)
{
   mySpeed = (SUMOReal) (myUserDefinedSpeed->getValue() / 3.6);
   static_cast<GUIE1VehicleActor*>(myObject)->setOverridingValue(mySpeed);
   myParent->updateChildren();
   return 1;
}


long
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
   sender->handle(this,
       myChosenValue!=3 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
       ptr);
   myParent->updateChildren();
   return 1;
}


long
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdPreDef(FXObject*,FXSelector,void*)
{
   mySpeed = (SUMOReal) (SUMOReal) ((myPredefinedValues->getCurrentItem() * 20 + 20)/3.6);
   static_cast<GUIE1VehicleActor*>(myObject)->setOverridingValue(mySpeed);
   myParent->updateChildren();
   return 1;
}


long
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onUpdPreDef(FXObject *sender,FXSelector,void*ptr)
{
   sender->handle(this,
       myChosenValue!=2 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
       ptr);
   myParent->updateChildren();
   return 1;
}


long
GUIE1VehicleActor::GUIManip_LaneSpeedTrigger::onCmdChangeOption(FXObject*,FXSelector,void*)
{
}
*/


/* -------------------------------------------------------------------------
 * GUIE1VehicleActor::GUIE1VehicleActorPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIE1VehicleActor::GUIE1VehicleActorPopupMenu::GUIE1VehicleActorPopupMenu(
    GUIMainWindow &app, GUISUMOAbstractView &parent,
    GUIGlObject &o)
        : GUIGLObjectPopupMenu(app, parent, o)
{}


GUIE1VehicleActor::GUIE1VehicleActorPopupMenu::~GUIE1VehicleActorPopupMenu()
{}


long
GUIE1VehicleActor::GUIE1VehicleActorPopupMenu::onCmdOpenManip(FXObject*, FXSelector, void*)
{
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIE1VehicleActor - methods
 * ----------------------------------------------------------------------- */
GUIE1VehicleActor::GUIE1VehicleActor(const std::string& id, MSLane* lane,
                                     SUMOReal positionInMeters,
                                     unsigned int laid, unsigned int cellid,
                                     unsigned int type) throw()
        : MSE1VehicleActor(id, lane, positionInMeters, laid, cellid, type),
        GUIGlObject_AbstractAdd(gIDStorage, "actor:" + id, GLO_TRIGGER)
{
    const Position2DVector &v = lane->getShape();
    myFGPosition = v.positionAtLengthPosition(positionInMeters);
    Line2D l(v.getBegin(), v.getEnd());
    SUMOReal sgPos = positionInMeters / v.length() * l.length();
    mySGPosition = l.getPositionAtDistance(sgPos);
    /*
    myBoundary.add(myFGPosition.x()+(SUMOReal) 5.5, myFGPosition.y()+(SUMOReal) 5.5);
    myBoundary.add(myFGPosition.x()-(SUMOReal) 5.5, myFGPosition.y()-(SUMOReal) 5.5);
    myBoundary.add(mySGPosition.x()+(SUMOReal) 5.5, mySGPosition.y()+(SUMOReal) 5.5);
    myBoundary.add(mySGPosition.x()-(SUMOReal) 5.5, mySGPosition.y()-(SUMOReal) 5.5);
    */
    myFGRotation = -v.rotationDegreeAtLengthPosition(positionInMeters);
    mySGRotation = -l.atan2DegreeAngle();
}


GUIE1VehicleActor::~GUIE1VehicleActor() throw()
{}


GUIGLObjectPopupMenu *
GUIE1VehicleActor::getPopUpMenu(GUIMainWindow &app,
                                GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIE1VehicleActorPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIE1VehicleActor::getParameterWindow(GUIMainWindow &app,
                                      GUISUMOAbstractView &)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("_ActorType", false, myLAId);
    ret->mkItem("LA-ID", false, myLAId);
    ret->mkItem("Area-ID", false, myAreaId);
    ret->mkItem("passed vehicles [#]", true,
                new CastingFunctionBinding<MSE1VehicleActor, SUMOReal, unsigned int>(this, &MSE1VehicleActor::getPassedVehicleNumber));
    ret->mkItem("passed cphones [#]", true,
                new CastingFunctionBinding<MSE1VehicleActor, SUMOReal, unsigned int>(this, &MSE1VehicleActor::getPassedCPhoneNumber));
    ret->mkItem("passed connected cphones [#]", true,
                new CastingFunctionBinding<MSE1VehicleActor, SUMOReal, unsigned int>(this, &MSE1VehicleActor::getPassedConnectedCPhoneNumber));
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUIE1VehicleActor::microsimID() const
{
    return getID();
}


bool
GUIE1VehicleActor::active() const
{
    return true;
}


Position2D
GUIE1VehicleActor::getPosition() const
{
    return myFGPosition; //!!!!
}


void
GUIE1VehicleActor::drawGL(SUMOReal scale, SUMOReal upscale)
{
    SUMOReal width = (SUMOReal) 2.0 * scale;
    glLineWidth(1.0);
    // shape
    if (myActorType==1) {
        glColor3f(0, 1, 1);
    } else if (myActorType==2) {
        glColor3f(1, 0, 1);
    } else {
        glColor3f(1, 0, 0);
    }
    glPushMatrix();
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated(myFGRotation, 0, 0, 1);
    glScaled(upscale, upscale, upscale);
    glBegin(GL_QUADS);
    glVertex2d(0-1.0, 2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0, 2);
    glEnd();
    glBegin(GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2d(0, 2-.1);
    glVertex2d(0, -2+.1);
    glEnd();


    // outline
    if (width*upscale>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // position indicator
    if (width*upscale>1) {
        glRotated(90, 0, 0, -1);
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        glEnd();
    }
    if (myActorType==1) {
        glColor3f(0, 1, 1);
    } else if (myActorType==2) {
        glColor3f(1, 0, 1);
    } else {
        glColor3f(1, 0, 0);
    }
    glPopMatrix();
}


Boundary
GUIE1VehicleActor::getBoundary() const
{
    /* !!! */
    Position2D pos = myFGPosition;
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUIE1VehicleActor::openManipulator(GUIMainWindow &,
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

