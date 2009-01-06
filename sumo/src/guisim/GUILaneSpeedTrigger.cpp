/****************************************************************************/
/// @file    GUILaneSpeedTrigger.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Changes the speed allowed on a set of lanes (gui version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4355)
#endif


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
#include "GUILaneSpeedTrigger.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUILaneSpeedTrigger.h>

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
 * GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu)
GUILaneSpeedTriggerPopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu, GUIGLObjectPopupMenu, GUILaneSpeedTriggerPopupMenuMap, ARRAYNUMBER(GUILaneSpeedTriggerPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger) GUIManip_LaneSpeedTriggerMap[]= {
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_USER_DEF, GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdPreDef),
    FXMAPFUNC(SEL_UPDATE,   GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_PRE_DEF,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onUpdPreDef),
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_OPTION,   GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::MID_CLOSE,    GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdClose),
};

FXIMPLEMENT(GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger, GUIManipulator, GUIManip_LaneSpeedTriggerMap, ARRAYNUMBER(GUIManip_LaneSpeedTriggerMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger - methods
 * ----------------------------------------------------------------------- */
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::GUIManip_LaneSpeedTrigger(
    GUIMainWindow &app,
    const std::string &name, GUILaneSpeedTrigger &o,
    int /*xpos*/, int /*ypos*/)
        : GUIManipulator(app, name, 0, 0),
        myParent(&app), myChosenValue(0), myChosenTarget(myChosenValue, this, MID_OPTION),
        mySpeed(o.getDefaultSpeed()), mySpeedTarget(mySpeed),
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
                           ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y|COMBOBOX_STATIC);
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
{}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUILaneSpeedTrigger::GUIManip_LaneSpeedTrigger::onCmdUserDef(FXObject*,FXSelector,void*)
{
    mySpeed = (SUMOReal)(myUserDefinedSpeed->getValue() / 3.6);
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
    mySpeed = (SUMOReal)(SUMOReal)((myPredefinedValues->getCurrentItem() * 20 + 20)/3.6);
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
    switch (myChosenValue) {
    case 0:
        mySpeed = (SUMOReal) static_cast<GUILaneSpeedTrigger*>(myObject)->getDefaultSpeed();
        break;
    case 1:
        mySpeed = (SUMOReal) static_cast<GUILaneSpeedTrigger*>(myObject)->getLoadedSpeed();
        break;
    case 2:
        mySpeed = (SUMOReal)((myPredefinedValues->getCurrentItem() * 20 + 20)/3.6);
        break;
    case 3:
        mySpeed = (SUMOReal)(myUserDefinedSpeed->getValue() / 3.6);
        break;
    default:
        // hmmm, should not happen
        break;
    }
    static_cast<GUILaneSpeedTrigger*>(myObject)->setOverridingValue(mySpeed);
    myParent->updateChildren();
    if (myChosenValue==1) {
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
{}


GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::~GUILaneSpeedTriggerPopupMenu()
{}


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
        const std::string &aXMLFilename) throw(ProcessError)
        : MSLaneSpeedTrigger(id, net, destLanes, aXMLFilename),
        GUIGlObject_AbstractAdd(gIDStorage, "speedtrigger:" + id, GLO_TRIGGER),
        myShowAsKMH(true), myLastValue(-1)
{
    myFGPositions.reserve(destLanes.size());
    myFGRotations.reserve(destLanes.size());
    vector<MSLane*>::const_iterator i;
    for (i=destLanes.begin(); i!=destLanes.end(); ++i) {
        const GUIEdge * const edge = static_cast<const GUIEdge * const>((*i)->getEdge());
        const Position2DVector &v = edge->getLaneGeometry((const MSLane *)(*i)).getShape();
        myFGPositions.push_back(v.positionAtLengthPosition(0));
        myBoundary.add(v.positionAtLengthPosition(0));
        Line2D l(v.getBegin(), v.getEnd());
        myFGRotations.push_back(-v.rotationDegreeAtLengthPosition(0));
        myDefaultSpeed = (*i)->maxSpeed();
        mySpeedOverrideValue = (*i)->maxSpeed();
    }
}


GUILaneSpeedTrigger::~GUILaneSpeedTrigger() throw()
{}


GUIGLObjectPopupMenu *
GUILaneSpeedTrigger::getPopUpMenu(GUIMainWindow &app,
                                  GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUILaneSpeedTriggerPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowManipulatorPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUILaneSpeedTrigger::getParameterWindow(GUIMainWindow &app,
                                        GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 1);
    // add items
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUILaneSpeedTrigger, SUMOReal>(this, &GUILaneSpeedTrigger::getCurrentSpeed));
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUILaneSpeedTrigger::getMicrosimID() const throw()
{
    return getID();
}


void
GUILaneSpeedTrigger::drawGL(const GUIVisualizationSettings &s) const throw()
{
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    glPolygonOffset(0, -3);
    for (size_t i=0; i<myFGPositions.size(); ++i) {
        const Position2D &pos = myFGPositions[i];
        SUMOReal rot = myFGRotations[i];
        glPushMatrix();
        glScaled(s.addExaggeration, s.addExaggeration, s.addExaggeration);
        glTranslated(pos.x(), pos.y(), 0);
        glRotated(rot, 0, 0, 1);
        glTranslated(0, -1.5, 0);

        int noPoints = 9;
        if (s.scale>25) {
            noPoints = (int)(9.0 + s.scale / 10.0);
            if (noPoints>36) {
                noPoints = 36;
            }
        }
        glColor3f(1, 0, 0);
        GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
        if (s.scale>=10) {
            glColor3f(0, 0, 0);
            GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
            // draw the speed string
            // not if scale to low
            if (s.scale>=4.5) {
                // compute
                SUMOReal value = (SUMOReal) getCurrentSpeed();
                if (myShowAsKMH) {
                    value *= 3.6f;
                    if (((int) value+1)%10==0) {
                        value = (SUMOReal)(((int) value+1) / 10 * 10);
                    }
                }
                if (value!=myLastValue) {
                    myLastValue = value;
                    myLastValueString = toString<SUMOReal>(myLastValue);
                    size_t idx = myLastValueString.find('.');
                    if (idx!=string::npos) {
                        if (idx>myLastValueString.length()) {
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
            }
        }
        glPopMatrix();
    }
    // (optional) draw name
    if (s.drawAddName) {
        drawGLName(getCenteringBoundary().getCenter(), getMicrosimID(), s.addNameSize / s.scale);
    }
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}


Boundary
GUILaneSpeedTrigger::getCenteringBoundary() const throw()
{
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIManipulator *
GUILaneSpeedTrigger::openManipulator(GUIMainWindow &app,
                                     GUISUMOAbstractView &)
{
    GUIManip_LaneSpeedTrigger *gui =
        new GUIManip_LaneSpeedTrigger(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}



/****************************************************************************/

