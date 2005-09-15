//---------------------------------------------------------------------------//
//                        GUITriggeredEmitter.cpp -
//  The gui-version of MSTriggeredEmitter
//                           -------------------
//  begin                : Thu, 21.07.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  author               : Daniel Krajzewicz
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
// Revision 1.1  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/09/09 12:50:30  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/08/01 13:06:51  dksumo
// further triggers added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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
#include <utils/convert/ToString.h>
#include <utils/helpers/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUITriggeredEmitter.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/glutils/polyfonts.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUITriggeredEmitter::GUITriggeredEmitterPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredEmitter::GUITriggeredEmitterPopupMenu)
    GUITriggeredEmitterPopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUITriggeredEmitter::GUITriggeredEmitterPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUITriggeredEmitter::GUITriggeredEmitterPopupMenu, GUIGLObjectPopupMenu, GUITriggeredEmitterPopupMenuMap, ARRAYNUMBER(GUITriggeredEmitterPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUITriggeredEmitter::GUIManip_TriggeredEmitter - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredEmitter::GUIManip_TriggeredEmitter) GUIManip_TriggeredEmitterMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredEmitter::GUIManip_TriggeredEmitter::MID_USER_DEF, GUITriggeredEmitter::GUIManip_TriggeredEmitter::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUITriggeredEmitter::GUIManip_TriggeredEmitter::MID_USER_DEF, GUITriggeredEmitter::GUIManip_TriggeredEmitter::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredEmitter::GUIManip_TriggeredEmitter::MID_OPTION,   GUITriggeredEmitter::GUIManip_TriggeredEmitter::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredEmitter::GUIManip_TriggeredEmitter::MID_CLOSE,    GUITriggeredEmitter::GUIManip_TriggeredEmitter::onCmdClose),
};

FXIMPLEMENT(GUITriggeredEmitter::GUIManip_TriggeredEmitter, GUIManipulator, GUIManip_TriggeredEmitterMap, ARRAYNUMBER(GUIManip_TriggeredEmitterMap))



/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUITriggeredEmitter::GUIManip_TriggeredEmitter - methods
 * ----------------------------------------------------------------------- */
GUITriggeredEmitter::GUIManip_TriggeredEmitter::GUIManip_TriggeredEmitter(
        GUIMainWindow &app,
        const std::string &name, GUITriggeredEmitter &o,
        int xpos, int ypos)
    : GUIManipulator(app, name, 0, 0), myChosenValue(0),
    myParent(&app),
    myChosenTarget(myChosenValue, this, MID_OPTION), myFlowFactorTarget(myFlowFactor),
    myFlowFactor(1800),
    myObject(&o)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

    if(myObject->inUserMode()) {
        myChosenValue = 1;
    }

    FXGroupBox *gp = new FXGroupBox(f1, "Change Flow",
        GROUPBOX_TITLE_LEFT|FRAME_SUNKEN|FRAME_RIDGE,
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
        // free
        FXHorizontalFrame *gf12 =
            new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf12, "User Defined Flow: ", &myChosenTarget, FXDataTarget::ID_OPTION+1,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y,
            0, 0, 0, 0,   2, 2, 0, 0);
        myFlowFactorDial =
            new FXRealSpinDial(gf12, 10, this, MID_USER_DEF,
                LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myFlowFactorDial->setFormatString("%.2f");
        myFlowFactorDial->setIncrements(.1,1,10);
        myFlowFactorDial->setRange(0,4000);
        myFlowFactorDial->setValue(myObject->getUserFlow());
    }
    new FXButton(f1,"Close",NULL,this,MID_CLOSE,
        BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
}


GUITriggeredEmitter::GUIManip_TriggeredEmitter::~GUIManip_TriggeredEmitter()
{
}


long
GUITriggeredEmitter::GUIManip_TriggeredEmitter::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUITriggeredEmitter::GUIManip_TriggeredEmitter::onCmdUserDef(FXObject*,FXSelector,void*)
{
    myFlowFactor = (float) (myFlowFactorDial->getValue());
    static_cast<GUITriggeredEmitter*>(myObject)->setUserFlow(myFlowFactor);
    static_cast<GUITriggeredEmitter*>(myObject)->setUserMode(true);
    myParent->updateChildren();
    return 1;
}


long
GUITriggeredEmitter::GUIManip_TriggeredEmitter::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myChosenValue!=1 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    myParent->updateChildren();
    return 1;
}


long
GUITriggeredEmitter::GUIManip_TriggeredEmitter::onCmdChangeOption(FXObject*,FXSelector,void*)
{
    static_cast<GUITriggeredEmitter*>(myObject)->setUserFlow(myFlowFactor);
    switch(myChosenValue) {
    case 0:
        static_cast<GUITriggeredEmitter*>(myObject)->setUserMode(false);
        break;
    case 1:
        static_cast<GUITriggeredEmitter*>(myObject)->setUserMode(true);
        break;
    default:
        throw 1;
    }
    myParent->updateChildren();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUITriggeredEmitter::GUITriggeredEmitterPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUITriggeredEmitter::GUITriggeredEmitterPopupMenu::GUITriggeredEmitterPopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUITriggeredEmitter::GUITriggeredEmitterPopupMenu::~GUITriggeredEmitterPopupMenu()
{
}


long
GUITriggeredEmitter::GUITriggeredEmitterPopupMenu::onCmdOpenManip(FXObject*,
                                                                  FXSelector,
                                                                  void*)
{
    static_cast<GUITriggeredEmitter*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}


/* -------------------------------------------------------------------------
 * GUITriggeredEmitter - methods
 * ----------------------------------------------------------------------- */
GUITriggeredEmitter::GUITriggeredEmitter(const std::string &id,
            MSNet &net, MSLane *destLanes, double pos,
            const std::string &aXMLFilename)
    : MSTriggeredEmitter(id, net, destLanes, pos, aXMLFilename),
    GUIGlObject_AbstractAdd(gIDStorage,
        string("emitter:") + id, GLO_TRIGGER),
    myShowAsKMH(true), myLastValue(-1)
{
    GUIEdge *edge =
        static_cast<GUIEdge*>(MSEdge::dictionary(destLanes->edge().id()));
    const Position2DVector &v =
        edge->getLaneGeometry(destLanes).getShape();
    if(pos<0) {
        pos = destLanes->length()+ pos;
    }
    myFGPosition = v.positionAtLengthPosition(pos);
    Line2D l(v.getBegin(), v.getEnd());
    mySGPosition = l.getPositionAtDistance(0);
    myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    mySGRotation = -l.atan2DegreeAngle();
}


GUITriggeredEmitter::~GUITriggeredEmitter()
{
}


GUIGLObjectPopupMenu *
GUITriggeredEmitter::getPopUpMenu(GUIMainWindow &app,
                                  GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret =
        new GUITriggeredEmitterPopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Open Manipulator...",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
/*    FXMenuPane *manualSpeed = new FXMenuPane(ret);
    new FXMenuTitle(ret,"&Arrange",NULL,manualSpeed);
    new FXMenuCommand(manualSpeed,"20 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);// MID_020KMH);
    new FXMenuCommand(manualSpeed,"40 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_040KMH);
    new FXMenuCommand(manualSpeed,"60 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_060KMH);
    new FXMenuCommand(manualSpeed,"80 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_080KMH);
    new FXMenuCommand(manualSpeed,"100 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_100KMH);
    new FXMenuCommand(manualSpeed,"120 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_120KMH);
    new FXMenuCommand(manualSpeed,"140 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_140KMH);
    new FXMenuCommand(manualSpeed,"160 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_160KMH);
    new FXMenuCommand(manualSpeed,"180 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_180KMH);
    new FXMenuCommand(manualSpeed,"200 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_200KMH);
        */
    //
    if(gSelected.isSelected(GLO_TRIGGER, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter",
        GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    return ret;
}


GUIParameterTableWindow *
GUITriggeredEmitter::getParameterWindow(GUIMainWindow &app,
                                        GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    /*
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUITriggeredEmitter, double>(this, &GUITriggeredEmitter::getCurrentSpeed));
        */
    // close building
    ret->closeBuilding();
    return ret;
}


std::string
GUITriggeredEmitter::microsimID() const
{
    return getID();
}


bool
GUITriggeredEmitter::active() const
{
    return true;
}


Position2D
GUITriggeredEmitter::getPosition() const
{
    return myFGPosition;
}


void
GUITriggeredEmitter::drawGL_FG(double scale)
{
    doPaint(myFGPosition, myFGRotation, scale);
}


void
GUITriggeredEmitter::drawGL_SG(double scale)
{
    doPaint(mySGPosition, mySGRotation, scale);
}


void
GUITriggeredEmitter::doPaint(const Position2D &pos, double rot,
                             double scale)
{
    glPushMatrix();
    glTranslated(pos.x(), pos.y(), 0);
    glRotated( rot, 0, 0, 1 );

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    // base
            glVertex2d(0-1.5, 0);
            glVertex2d(0-1.5, 8);
            glVertex2d(0+1.5, 8);
            glVertex2d(0+1.5, 0);
            glVertex2d(0-1.5, 0);
            glVertex2d(0+1.5, 8);

    glColor3f(1, 1, 0);
    glVertex2d(0, 1-.5);
    glVertex2d(0-1.25, 1+2-.5);
    glVertex2d(0+1.25, 1+2-.5);

    glVertex2d(0, 3-.5);
    glVertex2d(0-1.25, 3+2-.5);
    glVertex2d(0+1.25, 3+2-.5);

    glColor3f(1, 1, 0);
    glVertex2d(0, 5-.5);
    glVertex2d(0-1.25, 5+2-.5);
    glVertex2d(0+1.25, 5+2-.5);

    glEnd();
    glPopMatrix();
}

Boundary
GUITriggeredEmitter::getBoundary() const
{
    Position2D pos = getPosition();
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUITriggeredEmitter::openManipulator(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent)
{
    GUIManip_TriggeredEmitter *gui =
        new GUIManip_TriggeredEmitter(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


