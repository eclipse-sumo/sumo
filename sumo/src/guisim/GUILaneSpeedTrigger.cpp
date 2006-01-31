//---------------------------------------------------------------------------//
//                        GUILaneSpeedTrigger.cpp -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
//                           -------------------
//  begin                : Mon, 26.04.2004
//  copyright            : (C) 2004 by DLR http://ivf.dlr.de/
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
// Revision 1.14  2006/01/31 10:55:42  dkrajzew
// debugged bad edge retrieval
//
// Revision 1.13  2006/01/26 08:28:53  dkrajzew
// patched MSEdge in order to work with a generic router
//
// Revision 1.12  2006/01/19 08:49:45  dkrajzew
// debugging for the next release
//
// Revision 1.11  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.10  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/07/12 12:17:39  dkrajzew
// we're using polyfonts, now
//
// Revision 1.6  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.4  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.3  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.2  2004/08/02 13:15:21  dkrajzew
// missing "showAsKMH"-initialisation added
//
// Revision 1.1  2004/07/02 08:55:10  dkrajzew
// visualisation of vss added
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
#include "GUILaneSpeedTrigger.h"
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
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUILaneSpeedTrigger.h>

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
 * GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu)
    GUILaneSpeedTriggerPopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu, GUIGLObjectPopupMenu, GUILaneSpeedTriggerPopupMenuMap, ARRAYNUMBER(GUILaneSpeedTriggerPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger) GUIManip_LaneSpeedTriggerMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdPreDef),
    FXMAPFUNC(SEL_UPDATE,   GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onUpdPreDef),
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_OPTION,   GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_CLOSE,    GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdClose),
};

FXIMPLEMENT(GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger, GUIManipulator, GUIManip_LaneSpeedTriggerMap, ARRAYNUMBER(GUIManip_LaneSpeedTriggerMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger - methods
 * ----------------------------------------------------------------------- */
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::GUIManip_LaneSpeedTrigger(
        GUIMainWindow &app,
        const std::string &name, GUILaneSpeedTrigger &o,
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
            static_cast<GUILaneSpeedTrigger*>(myObject)->getDefaultSpeed()*3.6);
    }
    new FXButton(f1,"Close",NULL,this,MID_CLOSE,
        BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverriding(true);
}


GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::~GUIManip_LaneSpeedTrigger()
{
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdUserDef(FXObject*,FXSelector,void*)
{
    mySpeed = (SUMOReal) (myUserDefinedSpeed->getValue() / 3.6);
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverridingValue(mySpeed);
    myParent->updateChildren();
    return 1;
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myChosenValue!=3 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    myParent->updateChildren();
    return 1;
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdPreDef(FXObject*,FXSelector,void*)
{
    mySpeed = (SUMOReal) (SUMOReal) ((myPredefinedValues->getCurrentItem() * 20 + 20)/3.6);
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverridingValue(mySpeed);
    myParent->updateChildren();
    return 1;
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onUpdPreDef(FXObject *sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myChosenValue!=2 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    myParent->updateChildren();
    return 1;
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdChangeOption(FXObject*,FXSelector,void*)
{
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverriding(true);
    switch(myChosenValue) {
    case 0:
        mySpeed = (SUMOReal) static_cast<GUILaneSpeedTrigger*>(myObject)->getDefaultSpeed();
        break;
    case 1:
        mySpeed = (SUMOReal) static_cast<GUILaneSpeedTrigger*>(myObject)->getLoadedSpeed();
        break;
    case 2:
        mySpeed = (SUMOReal) ((myPredefinedValues->getCurrentItem() * 20 + 20)/3.6);
        break;
    case 3:
        mySpeed = (SUMOReal) (myUserDefinedSpeed->getValue() / 3.6);
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



/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::GUILaneSpeedTriggerPopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::~GUILaneSpeedTriggerPopupMenu()
{
}


long
GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::onCmdOpenManip(FXObject*,
                                                                  FXSelector,
                                                                  void*)
{
    static_cast<GUILaneSpeedTrigger*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}


/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger - methods
 * ----------------------------------------------------------------------- */
GUILaneSpeedTrigger::GUILaneSpeedTrigger(const std::string &id,
            MSNet &net, const std::vector<MSLane*> &destLanes,
            const std::string &aXMLFilename)
    : MSLaneSpeedTrigger(id, net, destLanes, aXMLFilename),
    GUIGlObject_AbstractAdd(gIDStorage, "speedtrigger:" + id, GLO_TRIGGER),
    myShowAsKMH(true), myLastValue(-1)
{
    mySGPositions.reserve(destLanes.size());
    myFGPositions.reserve(destLanes.size());
    myFGRotations.reserve(destLanes.size());
    mySGRotations.reserve(destLanes.size());
    vector<MSLane*>::const_iterator i;
    for(i=destLanes.begin(); i!=destLanes.end(); ++i) {
        const GUIEdge *edge = static_cast<const GUIEdge*>(&((*i)->edge()));
        const Position2DVector &v =
            edge->getLaneGeometry((const MSLane *) (*i)).getShape();
        myFGPositions.push_back(v.positionAtLengthPosition(0));
        Line2D l(v.getBegin(), v.getEnd());
        mySGPositions.push_back(l.getPositionAtDistance(0));
        myFGRotations.push_back(-v.rotationDegreeAtLengthPosition(0));
        mySGRotations.push_back(-l.atan2DegreeAngle());
        myDefaultSpeed = (*i)->maxSpeed();
        mySpeedOverrideValue = (*i)->maxSpeed();
    }
}


GUILaneSpeedTrigger::~GUILaneSpeedTrigger()
{
}


GUIGLObjectPopupMenu *
GUILaneSpeedTrigger::getPopUpMenu(GUIMainWindow &app,
                                  GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret =
        new GUILaneSpeedTriggerPopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Open Manipulator...",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
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
GUILaneSpeedTrigger::getParameterWindow(GUIMainWindow &app,
                                        GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUILaneSpeedTrigger, SUMOReal>(this, &GUILaneSpeedTrigger::getCurrentSpeed));
    // close building
    ret->closeBuilding();
    return ret;
}


std::string
GUILaneSpeedTrigger::microsimID() const
{
    return getID();
}


bool
GUILaneSpeedTrigger::active() const
{
    return true;
}


Position2D
GUILaneSpeedTrigger::getPosition() const
{
    return (*(myFGPositions.begin())); // !!!
}


void
GUILaneSpeedTrigger::drawGL_FG(SUMOReal scale, SUMOReal upscale)
{
    doPaint(myFGPositions, myFGRotations, scale, upscale);
}


void
GUILaneSpeedTrigger::drawGL_SG(SUMOReal scale, SUMOReal upscale)
{
    doPaint(mySGPositions, mySGRotations, scale, upscale);
}


void
GUILaneSpeedTrigger::doPaint(const PosCont &poss, const RotCont rots,
                             SUMOReal scale, SUMOReal upscale)
{
    for(size_t i=0; i<poss.size(); ++i) {
        const Position2D &pos = poss[i];
        SUMOReal rot = rots[i];
        glPushMatrix();
        glScaled(upscale, upscale, upscale);
        glTranslated(pos.x(), pos.y(), 0);
        glRotated( rot, 0, 0, 1 );
        glTranslated(0, -1.5, 0);

        int noPoints = 9;
        if(scale>25) {
            noPoints = (int) (9.0 + scale / 10.0);
            if(noPoints>36) {
                noPoints = 36;
            }
        }
        glColor3f(1, 0, 0);
        GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
        if(scale<10) {
            glPopMatrix();
            continue;
        }
        glColor3f(0, 0, 0);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        // draw the speed string
            // not if scale to low
        if(scale<4.5) {
            glPopMatrix();
            continue;
        }
            // compute
        SUMOReal value = (SUMOReal) getCurrentSpeed();
        if(myShowAsKMH) {
            value *= 3.6f;
            // !!!
            if(((int) value+1)%10==0) {
                value = (SUMOReal) (((int) value+1) / 10 * 10);
            }
        }
        if(value!=myLastValue) {
            myLastValue = value;
            myLastValueString = toString<SUMOReal>(myLastValue);
            size_t idx = myLastValueString.find('.');
            if(idx!=string::npos) {
                if(idx>myLastValueString.length()) {
                    idx = myLastValueString.length();
                }
                myLastValueString = myLastValueString.substr(0, idx);
            }
        }
            //draw
        glColor3f(1, 1, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(1.2f);
        SUMOReal w = pfdkGetStringWidth(myLastValueString.c_str());
        glRotated(180, 0, 1, 0);
        glTranslated(-w/2., 0.3, 0);
        pfDrawString(myLastValueString.c_str());
        glPopMatrix();
    }
}


Boundary
GUILaneSpeedTrigger::getBoundary() const
{
    /* !!! */
    Position2D pos = getPosition();
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUILaneSpeedTrigger::openManipulator(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent)
{
    GUIManip_LaneSpeedTrigger *gui =
        new GUIManip_LaneSpeedTrigger(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


