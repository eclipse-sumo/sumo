/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUITrafficLightLogicWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Oct/Nov 2003
///
// A wrapper for tl-logics to allow their visualisation and interaction
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSLane.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSActuatedTrafficLightLogic.h>
#include <microsim/traffic_lights/MSDelayBasedTrafficLightLogic.h>
#include <microsim/traffic_lights/NEMAController.h>
#include <microsim/traffic_lights/MSRailSignal.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/FuncBinding_StringParam.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUITLLogicPhasesTrackerWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUITrafficLightLogicWrapper.h"
#include "GUINet.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu)
GUITrafficLightLogicWrapperPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPHASES,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_TRACKPHASES,            GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOW_DETECTORS,         GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowDetectors),
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
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowDetectors(
    FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_TLLOGIC);
    GUITrafficLightLogicWrapper* w = static_cast<GUITrafficLightLogicWrapper*>(myObject);
    MSActuatedTrafficLightLogic* act = dynamic_cast<MSActuatedTrafficLightLogic*>(&w->getTLLogic());
    if (act != nullptr) {
        act->setShowDetectors(!act->showDetectors());
    } else {
        MSDelayBasedTrafficLightLogic* db = dynamic_cast<MSDelayBasedTrafficLightLogic*>(&w->getTLLogic());
        if (db != nullptr) {
            db->setShowDetectors(!db->showDetectors());
        } else {
            NEMALogic* nema = dynamic_cast<NEMALogic*>(&w->getTLLogic());
            if (nema != nullptr) {
                nema->setShowDetectors(!nema->showDetectors());
            }
        }
    }
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
        int index = 0;
        for (i = logics.begin(); i != logics.end(); ++i, ++index) {
            if (!vars.isActive(*i)) {
                GUIDesigns::buildFXMenuCommand(ret, ("Switch to '" + (*i)->getProgramID() + "'").c_str(),
                                               GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), ret, (FXSelector)(MID_SWITCH + index));
            }
        }
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Switch off", GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), ret, MID_SWITCH_OFF);
    GUIDesigns::buildFXMenuCommand(ret, "Track Phases", nullptr, ret, MID_TRACKPHASES);
    GUIDesigns::buildFXMenuCommand(ret, "Show Phases", nullptr, ret, MID_SHOWPHASES);
    MSActuatedTrafficLightLogic* act = dynamic_cast<MSActuatedTrafficLightLogic*>(&myTLLogic);
    if (act != nullptr) {
        GUIDesigns::buildFXMenuCommand(ret, act->showDetectors() ? "Hide Detectors" : "Show Detectors", nullptr, ret, MID_SHOW_DETECTORS);
    }
    MSDelayBasedTrafficLightLogic* db = dynamic_cast<MSDelayBasedTrafficLightLogic*>(&myTLLogic);
    if (db != nullptr) {
        GUIDesigns::buildFXMenuCommand(ret, db->showDetectors() ? "Hide Detectors" : "Show Detectors", nullptr, ret, MID_SHOW_DETECTORS);
    }
    NEMALogic* nema = dynamic_cast<NEMALogic*>(&myTLLogic);
    if (nema != nullptr) {
        GUIDesigns::buildFXMenuCommand(ret, nema->showDetectors() ? "Hide Detectors" : "Show Detectors", nullptr, ret, MID_SHOW_DETECTORS);
    }
    new FXMenuSeparator(ret);
    MSTrafficLightLogic* tll = getActiveTLLogic();
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    GUIDesigns::buildFXMenuCommand(ret, "phase: " + toString(tll->getCurrentPhaseIndex()), nullptr, nullptr, 0);
    const std::string& name =  tll->getCurrentPhaseDef().getName();
    if (name != "") {
        GUIDesigns::buildFXMenuCommand(ret, "phase name: " + name, nullptr, nullptr, 0);
    }
    new FXMenuSeparator(ret);
    buildShowParamsPopupEntry(ret, false);
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
GUITrafficLightLogicWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    ret->mkItem("tlLogic [id]", false, myTLLogic.getID());
    ret->mkItem("type", false, toString(myTLLogic.getLogicType()));
    ret->mkItem("program", false, myTLLogic.getProgramID());
    ret->mkItem("phase", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentPhase));
    ret->mkItem("phase name", true, new FunctionBindingString<GUITrafficLightLogicWrapper>(this, &GUITrafficLightLogicWrapper::getCurrentPhaseName));
    ret->mkItem("duration", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentDuration));
    ret->mkItem("minDur", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentMinDur));
    ret->mkItem("maxDur", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentMaxDur));
    ret->mkItem("running duration", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getRunningDuration));
    ret->mkItem("earliestEnd", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentEarliestEnd));
    ret->mkItem("latestEnd", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentLatestEnd));
    ret->mkItem("time in cycle", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getCurrentTimeInCycle));
    ret->mkItem("cycle time", true, new FunctionBinding<GUITrafficLightLogicWrapper, int>(this, &GUITrafficLightLogicWrapper::getDefaultCycleTime));
    MSRailSignal* rs = dynamic_cast<MSRailSignal*>(&myTLLogic);
    if (rs != nullptr) {
        ret->mkItem("blocking", true, new FunctionBindingString<MSRailSignal>(rs, &MSRailSignal::getBlockingVehicleIDs));
        ret->mkItem("rival", true, new FunctionBindingString<MSRailSignal>(rs, &MSRailSignal::getRivalVehicleIDs));
        ret->mkItem("priority", true, new FunctionBindingString<MSRailSignal>(rs, &MSRailSignal::getPriorityVehicleIDs));
        ret->mkItem("constraint", true, new FunctionBindingString<MSRailSignal>(rs, &MSRailSignal::getConstraintInfo));
    }
    // close building
    ret->closeBuilding(&myTLLogic);
    return ret;
}

double
GUITrafficLightLogicWrapper::getExaggeration(const GUIVisualizationSettings& /*s*/) const {
    return 1;
}


Boundary
GUITrafficLightLogicWrapper::getCenteringBoundary() const {
    Boundary ret;
    const MSTrafficLightLogic::LaneVectorVector& lanes = myTLLogic.getLaneVectors();
    for (MSTrafficLightLogic::LaneVectorVector::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        const MSTrafficLightLogic::LaneVector& lanes2 = (*i);
        for (MSTrafficLightLogic::LaneVector::const_iterator j = lanes2.begin(); j != lanes2.end(); ++j) {
            ret.add((*j)->getShape()[-1]);
        }
    }
    ret.grow(20);
    return ret;
}

const std::string
GUITrafficLightLogicWrapper::getOptionalName() const {
    return myTLLogic.getParameter("name", "");
}


void
GUITrafficLightLogicWrapper::switchTLSLogic(int to) {
    if (to == -1) {
        myTLLogicControl.switchTo(myTLLogic.getID(), "off");
        GUINet::getGUIInstance()->createTLWrapper(getActiveTLLogic());
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
            int curPhaseIdx = myTLLogic.getCurrentPhaseIndex();
            int phaseIdx = (curPhaseIdx + 1) % phases.size();
            std::vector<int> nextGreen;
            while (phaseIdx != curPhaseIdx) {
                const std::string& state = phases[phaseIdx]->getState();
                for (int linkIdx = 0; linkIdx < (int)state.size(); linkIdx++) {
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
            for (std::vector<int>::iterator it_idx = nextGreen.begin(); it_idx != nextGreen.end(); it_idx++) {
                const MSTrafficLightLogic::LaneVector& lanes = myTLLogic.getLanesAt(*it_idx);
                for (MSTrafficLightLogic::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
                    GLHelper::pushMatrix();
                    // split circle in red and yellow
                    Position pos = (*it_lane)->getShape().back();
                    glTranslated(pos.x(), pos.y(), GLO_MAX);
                    double rot = RAD2DEG((*it_lane)->getShape().angleAt2D((int)(*it_lane)->getShape().size() - 2)) - 90;
                    glRotated(rot, 0, 0, 1);
                    GLHelper::setColor(s.getLinkColor(LINKSTATE_TL_RED));
                    GLHelper::drawFilledCircle((*it_lane)->getWidth() / 2., 8, -90, 90);
                    GLHelper::setColor(s.getLinkColor(LINKSTATE_TL_YELLOW_MAJOR));
                    GLHelper::drawFilledCircle((*it_lane)->getWidth() / 2., 8, 90, 270);
                    GLHelper::popMatrix();
                }
            }
        }
    }
}

MSTrafficLightLogic*
GUITrafficLightLogicWrapper::getActiveTLLogic() const {
    return myTLLogicControl.getActive(myTLLogic.getID());
}

int
GUITrafficLightLogicWrapper::getCurrentPhase() const {
    return getActiveTLLogic()->getCurrentPhaseIndex();
}

std::string
GUITrafficLightLogicWrapper::getCurrentPhaseName() const {
    return getActiveTLLogic()->getCurrentPhaseDef().getName();
}

int
GUITrafficLightLogicWrapper::getCurrentDuration() const {
    return (int)STEPS2TIME(getActiveTLLogic()->getCurrentPhaseDef().duration);
}

int
GUITrafficLightLogicWrapper::getCurrentMinDur() const {
    return (int)STEPS2TIME(getActiveTLLogic()->getCurrentPhaseDef().minDuration);
}

int
GUITrafficLightLogicWrapper::getCurrentMaxDur() const {
    return (int)STEPS2TIME(getActiveTLLogic()->getCurrentPhaseDef().maxDuration);
}

int
GUITrafficLightLogicWrapper::getCurrentEarliestEnd() const {
    const SUMOTime earliestEnd = getActiveTLLogic()->getCurrentPhaseDef().earliestEnd;
    return earliestEnd == MSPhaseDefinition::UNSPECIFIED_DURATION ? -1 : (int)STEPS2TIME(earliestEnd);
}

int
GUITrafficLightLogicWrapper::getCurrentLatestEnd() const {
    const SUMOTime latestEnd = getActiveTLLogic()->getCurrentPhaseDef().latestEnd;
    return latestEnd == MSPhaseDefinition::UNSPECIFIED_DURATION ? -1 : (int)STEPS2TIME(latestEnd);
}

int
GUITrafficLightLogicWrapper::getDefaultCycleTime() const {
    return (int)STEPS2TIME(getActiveTLLogic()->getDefaultCycleTime());
}

int
GUITrafficLightLogicWrapper::getCurrentTimeInCycle() const {
    return (int)STEPS2TIME(getActiveTLLogic()->getTimeInCycle());
}

int
GUITrafficLightLogicWrapper::getRunningDuration() const {
    return (int)(SIMTIME - STEPS2TIME(getActiveTLLogic()->getCurrentPhaseDef().myLastSwitch));
}


/****************************************************************************/
