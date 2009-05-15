/****************************************************************************/
/// @file    GUITrafficLightLogicWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date    Oct/Nov 2003
/// @version $Id$
///
// A wrapper for tl-logics to allow their visualisation and interaction
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUITLLogicPhasesTrackerWindow.h>
#include <microsim/MSLane.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <guisim/GLObjectValuePassConnector.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/FuncBinding_StringParam.h>
#include "GUITrafficLightLogicWrapper.h"
#include <utils/gui/div/GUIGlobalSelection.h>

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
FXDEFMAP(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu)
GUITrafficLightLogicWrapperPopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPHASES,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_TRACKPHASES,            GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_SWITCH_OFF,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLS2Off),
    FXMAPFUNCS(SEL_COMMAND, MID_SWITCH, MID_SWITCH+20, GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic),
};

// Object implementation
FXIMPLEMENT(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu, GUIGLObjectPopupMenu, GUITrafficLightLogicWrapperPopupMenuMap, ARRAYNUMBER(GUITrafficLightLogicWrapperPopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::GUITrafficLightLogicWrapperPopupMenu(
    GUIMainWindow &app, GUISUMOAbstractView &parent,
    GUIGlObject &o)
        : GUIGLObjectPopupMenu(app, parent, o) {}


GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::~GUITrafficLightLogicWrapperPopupMenu() throw() {}



long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases(
    FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->begin2TrackPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases(
    FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->showPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLS2Off(
    FXObject*,FXSelector sel,void*) {
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(-1);
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic(
    FXObject*,FXSelector sel,void*) {
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(FXSELID(sel)-MID_SWITCH);
    return 1;
}



/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapper(
    GUIGlObjectStorage &idStorage,
    MSTLLogicControl &control, MSTrafficLightLogic &tll) throw()
        : GUIGlObject(idStorage, "tl-logic:"+tll.getID()),
        myTLLogicControl(control), myTLLogic(tll) {}


GUITrafficLightLogicWrapper::~GUITrafficLightLogicWrapper() throw() {}


GUIGLObjectPopupMenu *
GUITrafficLightLogicWrapper::getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent) throw() {
    myApp = &app;
    GUIGLObjectPopupMenu *ret = new GUITrafficLightLogicWrapperPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    const MSTLLogicControl::TLSLogicVariants &vars = myTLLogicControl.get(myTLLogic.getID());
    std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
    if (logics.size()>1) {
        std::vector<MSTrafficLightLogic*>::const_iterator i;
        size_t index = 0;
        for (i=logics.begin(); i!=logics.end(); ++i, ++index) {
            if (!vars.isActive(*i)) {
                new FXMenuCommand(ret, ("Switch to '" + (*i)->getSubID() + "'").c_str(),
                                  GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, (FXSelector)(MID_SWITCH+index));
            }
        }
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, "Switch off", GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_SWITCH_OFF);
    new FXMenuCommand(ret, "Track Phases", 0, ret, MID_TRACKPHASES);
    new FXMenuCommand(ret, "Show Phases", 0, ret, MID_SHOWPHASES);
    new FXMenuSeparator(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


void
GUITrafficLightLogicWrapper::begin2TrackPhases() {
    GUITLLogicPhasesTrackerWindow *window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
                                          new FuncBinding_StringParam<MSTLLogicControl, std::pair<SUMOTime, MSPhaseDefinition> >
                                          (&MSNet::getInstance()->getTLSControl(), &MSTLLogicControl::getPhaseDef, myTLLogic.getID()));
    window->create();
    window->show();
}


void
GUITrafficLightLogicWrapper::showPhases() {
    GUITLLogicPhasesTrackerWindow *window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
                                          static_cast<MSSimpleTrafficLightLogic&>(myTLLogic).getPhases());
    window->setBeginTime(0);
    window->create();
    window->show();
}


GUIParameterTableWindow *
GUITrafficLightLogicWrapper::getParameterWindow(GUIMainWindow &,
        GUISUMOAbstractView &) throw() {
    return 0;
}


const std::string &
GUITrafficLightLogicWrapper::getMicrosimID() const throw() {
    return myTLLogic.getID();
}


Boundary
GUITrafficLightLogicWrapper::getCenteringBoundary() const throw() {
    Boundary ret;
    const MSTrafficLightLogic::LaneVectorVector &lanes = myTLLogic.getLanes();
    for (MSTrafficLightLogic::LaneVectorVector::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
        const MSTrafficLightLogic::LaneVector &lanes2 = (*i);
        for (MSTrafficLightLogic::LaneVector::const_iterator j=lanes2.begin(); j!=lanes2.end(); ++j) {
            ret.add((*j)->getShape()[-1]);
        }
    }
    ret.grow(20);
    return ret;
}


void
GUITrafficLightLogicWrapper::switchTLSLogic(int to) {
    if (to==-1) {
        myTLLogicControl.switchTo(myTLLogic.getID(), "off");
        return;
    }
    const MSTLLogicControl::TLSLogicVariants &vars = myTLLogicControl.get(myTLLogic.getID());
    std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
    myTLLogicControl.switchTo(myTLLogic.getID(), logics[to]->getSubID());
}


int
GUITrafficLightLogicWrapper::getLinkIndex(const MSLink * const link) const {
    return myTLLogic.getLinkIndex(link);
}


void
GUITrafficLightLogicWrapper::drawGL(const GUIVisualizationSettings &s) const throw() {
}


/****************************************************************************/

