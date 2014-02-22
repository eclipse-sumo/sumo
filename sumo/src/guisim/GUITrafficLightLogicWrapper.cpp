/****************************************************************************/
/// @file    GUITrafficLightLogicWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Oct/Nov 2003
/// @version $Id$
///
// A wrapper for tl-logics to allow their visualisation and interaction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSLane.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/FuncBinding_StringParam.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUITLLogicPhasesTrackerWindow.h>
#include <gui/GUIGlobals.h>
#include "GUITrafficLightLogicWrapper.h"
#include "GUINet.h"
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu)
GUITrafficLightLogicWrapperPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPHASES,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_TRACKPHASES,            GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_SWITCH_OFF,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLS2Off),
    FXMAPFUNCS(SEL_COMMAND, MID_SWITCH, MID_SWITCH + 20, GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic),
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
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o)
    : GUIGLObjectPopupMenu(app, parent, o) {}


GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::~GUITrafficLightLogicWrapperPopupMenu() {}



long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases(
    FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->begin2TrackPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases(
    FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->showPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLS2Off(
    FXObject*, FXSelector /*sel*/, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(-1);
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic(
    FXObject*, FXSelector sel, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(FXSELID(sel) - MID_SWITCH);
    return 1;
}



/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapper(
    MSTLLogicControl& control, MSTrafficLightLogic& tll) :
    GUIGlObject(GLO_TLLOGIC, tll.getID()),
    myTLLogicControl(control), myTLLogic(tll) {}


GUITrafficLightLogicWrapper::~GUITrafficLightLogicWrapper() {}


GUIGLObjectPopupMenu*
GUITrafficLightLogicWrapper::getPopUpMenu(GUIMainWindow& app,
        GUISUMOAbstractView& parent) {
    myApp = &app;
    GUIGLObjectPopupMenu* ret = new GUITrafficLightLogicWrapperPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    const MSTLLogicControl::TLSLogicVariants& vars = myTLLogicControl.get(myTLLogic.getID());
    std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
    if (logics.size() > 1) {
        std::vector<MSTrafficLightLogic*>::const_iterator i;
        size_t index = 0;
        for (i = logics.begin(); i != logics.end(); ++i, ++index) {
            if (!vars.isActive(*i)) {
                new FXMenuCommand(ret, ("Switch to '" + (*i)->getProgramID() + "'").c_str(),
                                  GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, (FXSelector)(MID_SWITCH + index));
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
    GUITLLogicPhasesTrackerWindow* window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
                                          new FuncBinding_StringParam<MSTLLogicControl, std::pair<SUMOTime, MSPhaseDefinition> >
                                          (&MSNet::getInstance()->getTLSControl(), &MSTLLogicControl::getPhaseDef, myTLLogic.getID()));
    window->create();
    window->show();
}


void
GUITrafficLightLogicWrapper::showPhases() {
    GUITLLogicPhasesTrackerWindow* window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
                                          static_cast<MSSimpleTrafficLightLogic&>(myTLLogic).getPhases());
    window->setBeginTime(0);
    window->create();
    window->show();
}


GUIParameterTableWindow*
GUITrafficLightLogicWrapper::getParameterWindow(GUIMainWindow&,
        GUISUMOAbstractView&) {
    return 0;
}


Boundary
GUITrafficLightLogicWrapper::getCenteringBoundary() const {
    Boundary ret;
    const MSTrafficLightLogic::LaneVectorVector& lanes = myTLLogic.getLanes();
    for (MSTrafficLightLogic::LaneVectorVector::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        const MSTrafficLightLogic::LaneVector& lanes2 = (*i);
        for (MSTrafficLightLogic::LaneVector::const_iterator j = lanes2.begin(); j != lanes2.end(); ++j) {
            ret.add((*j)->getShape()[-1]);
        }
    }
    ret.grow(20);
    return ret;
}


void
GUITrafficLightLogicWrapper::switchTLSLogic(int to) {
    if (to == -1) {
        myTLLogicControl.switchTo(myTLLogic.getID(), "off");
        MSTrafficLightLogic* tll = myTLLogicControl.getActive(myTLLogic.getID());
        GUINet::getGUIInstance()->createTLWrapper(tll);
    } else {
        const MSTLLogicControl::TLSLogicVariants& vars = myTLLogicControl.get(myTLLogic.getID());
        std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
        myTLLogicControl.switchTo(myTLLogic.getID(), logics[to]->getProgramID());
    }
}


int
GUITrafficLightLogicWrapper::getLinkIndex(const MSLink* const link) const {
    return myTLLogic.getLinkIndex(link);
}


void
GUITrafficLightLogicWrapper::drawGL(const GUIVisualizationSettings& s) const {
    if (s.gaming) {
        if (!MSNet::getInstance()->getTLSControl().isActive(&myTLLogic)) {
            return;
        };
        const std::string& curState = myTLLogic.getCurrentPhaseDef().getState();
        if (curState.find_first_of("gG") == std::string::npos) {
            // no link is 'green' at the moment. find those that turn green next
            const MSTrafficLightLogic::Phases& phases = myTLLogic.getPhases();
            unsigned int curPhaseIdx = myTLLogic.getCurrentPhaseIndex();
            unsigned int phaseIdx = (curPhaseIdx + 1) % phases.size();
            std::vector<unsigned int> nextGreen;
            while (phaseIdx != curPhaseIdx) {
                const std::string& state = phases[phaseIdx]->getState();
                for (unsigned int linkIdx = 0; linkIdx < state.size(); linkIdx++) {
                    if ((LinkState)state[linkIdx] == LINKSTATE_TL_GREEN_MINOR ||
                            (LinkState)state[linkIdx] == LINKSTATE_TL_GREEN_MAJOR) {
                        nextGreen.push_back(linkIdx);
                    }
                }
                if (nextGreen.size() > 0) {
                    break;
                }
                phaseIdx = (phaseIdx + 1) % phases.size();
            }
            // highlight nextGreen links
            for (std::vector<unsigned int>::iterator it_idx = nextGreen.begin(); it_idx != nextGreen.end(); it_idx++) {
                const MSTrafficLightLogic::LaneVector& lanes = myTLLogic.getLanesAt(*it_idx);
                for (MSTrafficLightLogic::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
                    glPushMatrix();
                    glColor3d(0, 1, 0);
                    Position pos = (*it_lane)->getShape().back();
                    glTranslated(pos.x(), pos.y(), GLO_MAX);
                    GLHelper::drawFilledCircle((*it_lane)->getWidth() / 2.);
                    glPopMatrix();
                }
            }
        }
    }
}


/****************************************************************************/

