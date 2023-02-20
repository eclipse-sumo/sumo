/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUITLLogicPhasesTrackerWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Oct/Nov 2003
///
// A window displaying the phase diagram of a tl-logic
/****************************************************************************/
#include <config.h>

#include <vector>
#include <iostream>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/div/GLHelper.h>
#include "GUITLLogicPhasesTrackerWindow.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSLink.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <guisim/GUITrafficLightLogicWrapper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/div/GUIDesigns.h>
#include <foreign/fontstash/fontstash.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// static member initialisation
// ===========================================================================
int GUITLLogicPhasesTrackerWindow::myLastY(-1);

// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel-callbacks
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel) GUITLLogicPhasesTrackerPanelMap[] = {
    FXMAPFUNC(SEL_CONFIGURE, 0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onPaint),
    FXMAPFUNC(SEL_MOTION,    0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onMouseMove),

};

// Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel, FXGLCanvas, GUITLLogicPhasesTrackerPanelMap, ARRAYNUMBER(GUITLLogicPhasesTrackerPanelMap))



/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel-methods
 * ----------------------------------------------------------------------- */
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::GUITLLogicPhasesTrackerPanel(
    FXComposite* c, GUIMainWindow& app,
    GUITLLogicPhasesTrackerWindow& parent) :
    FXGLCanvas(c, app.getGLVisual(), app.getBuildGLCanvas(), (FXObject*) nullptr, (FXSelector) 0, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y/*, 0, 0, 300, 200*/),
    myParent(&parent)
{}


GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::~GUITLLogicPhasesTrackerPanel() {}


long
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onConfigure(FXObject*, FXSelector, void*) {
    if (makeCurrent()) {
        int widthInPixels = getWidth();
        int heightInPixels = getHeight();
        if (widthInPixels != 0 && heightInPixels != 0) {
            glViewport(0, 0, widthInPixels - 1, heightInPixels - 1);
            glClearColor(0, 0, 0, 1);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_COLOR_MATERIAL);
            glLineWidth(1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    return 1;
}


long
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onPaint(
    FXObject*, FXSelector, void*) {
    if (!isEnabled()) {
        return 1;
    }
    if (makeCurrent()) {
        int widthInPixels = getWidth();
        int heightInPixels = getHeight();
        if (widthInPixels != 0 && heightInPixels != 0) {
            glViewport(0, 0, widthInPixels - 1, heightInPixels - 1);
            glClearColor(0, 0, 0, 1);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_COLOR_MATERIAL);
            glLineWidth(1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // draw
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            myParent->drawValues(*this);
            swapBuffers();
        }
        makeNonCurrent();
    }
    return 1;
}


long
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onMouseMove(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*) ptr;
    myMousePos.setx(event->win_x);
    myMousePos.sety(event->win_y);
    onPaint(nullptr, 0, nullptr);
    return 1;
}

/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow - FOX callback mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITLLogicPhasesTrackerWindow) GUITLLogicPhasesTrackerWindowMap[] = {
    FXMAPFUNC(SEL_CONFIGURE, 0,           GUITLLogicPhasesTrackerWindow::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0,           GUITLLogicPhasesTrackerWindow::onPaint),
    FXMAPFUNC(SEL_COMMAND,   MID_SIMSTEP, GUITLLogicPhasesTrackerWindow::onSimStep),

};

FXIMPLEMENT(GUITLLogicPhasesTrackerWindow, FXMainWindow, GUITLLogicPhasesTrackerWindowMap, ARRAYNUMBER(GUITLLogicPhasesTrackerWindowMap))


/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow-methods
 * ----------------------------------------------------------------------- */
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
    GUIMainWindow& app,
    MSTrafficLightLogic& logic, GUITrafficLightLogicWrapper& wrapper,
    ValueSource<std::pair<SUMOTime, MSPhaseDefinition> >* src) :
    FXMainWindow(app.getApp(), "TLS-Tracker", nullptr, nullptr, DECOR_ALL, 20, 20, 300, 200),
    myApplication(&app),
    myTLLogic(&logic),
    myAmInTrackingMode(true) {
    initToolBar();
    myConnector = new GLObjectValuePassConnector<std::pair<SUMOTime, MSPhaseDefinition> >(wrapper, src, this);
    app.addChild(this);
    for (int i = 0; i < (int)myTLLogic->getLinks().size(); ++i) {
        myLinkNames.push_back(toString<int>(i));
    }
    for (auto item : myTLLogic->getDetectorStates()) {
        std::string detID = item.first;
        if (detID.size() > 4) {
            detID = detID.substr(detID.size() - 4);
        }
        myDetectorNames.push_back(detID);
    }
    for (auto item : myTLLogic->getConditions()) {
        myConditionNames.push_back(item.first);
    }
    FXVerticalFrame* glcanvasFrame =
        new FXVerticalFrame(this,
                            FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y,
                            0, 0, 0, 0, 0, 0, 0, 0);
    myPanel = new GUITLLogicPhasesTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle((logic.getID() + " - " + logic.getProgramID() + " - tracker").c_str());
    setIcon(GUIIconSubSys::getIcon(GUIIcon::APP_TLSTRACKER));
    loadSettings();
    setHeight(computeHeight());
}


GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
    GUIMainWindow& app,
    MSTrafficLightLogic& logic, GUITrafficLightLogicWrapper& /*wrapper*/,
    const MSSimpleTrafficLightLogic::Phases& /*phases*/) :
    FXMainWindow(app.getApp(), "TLS-Tracker", nullptr, nullptr, DECOR_ALL, 20, 20, 300, 200),
    myApplication(&app),
    myTLLogic(&logic),
    myAmInTrackingMode(false),
    myToolBarDrag(nullptr),
    myBeginOffset(nullptr) {
    myConnector = nullptr;
    initToolBar();
    app.addChild(this);
    for (int i = 0; i < (int)myTLLogic->getLinks().size(); ++i) {
        myLinkNames.push_back(toString<int>(i));
    }
    FXVerticalFrame* glcanvasFrame =
        new FXVerticalFrame(this,
                            FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y,
                            0, 0, 0, 0, 0, 0, 0, 0);
    myPanel = new GUITLLogicPhasesTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle((logic.getID() + " - " + logic.getProgramID() + " - phases").c_str());
    setIcon(GUIIconSubSys::getIcon(GUIIcon::APP_TLSTRACKER));
    setHeight(computeHeight());
    setWidth(700);
}


GUITLLogicPhasesTrackerWindow::~GUITLLogicPhasesTrackerWindow() {
    if (myAmInTrackingMode) {
        saveSettings();
        myLastY = -1;
    }
    myApplication->removeChild(this);
    delete myConnector;
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
    delete myToolBarDrag;
}

void
GUITLLogicPhasesTrackerWindow::initToolBar() {
    myToolBarDrag = new FXToolBarShell(this, GUIDesignToolBar);
    myToolBar = new FXToolBar(this, myToolBarDrag, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED);
    new FXToolBarGrip(myToolBar, myToolBar, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);

    if (myAmInTrackingMode) {
        // interval manipulation
        new FXLabel(myToolBar, "range (s):", nullptr, LAYOUT_CENTER_Y);
        myBeginOffset = new FXRealSpinner(myToolBar, 4, this, MID_SIMSTEP, LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        //myBeginOffset->setFormatString("%.0f");
        //myBeginOffset->setIncrements(1, 10, 100);
        myBeginOffset->setIncrement(10);
        myBeginOffset->setRange(60, 3600);
        myBeginOffset->setValue(240);
    }

    new FXLabel(myToolBar, "time style:", nullptr, LAYOUT_CENTER_Y);
    myTimeMode = new FXComboBox(myToolBar, 11, this, MID_SIMSTEP, GUIDesignViewSettingsComboBox1);
    myTimeMode->appendItem("seconds");
    myTimeMode->appendItem("MM:SS");
    myTimeMode->appendItem("time in cycle");
    myTimeMode->setNumVisible(3);

    new FXLabel(myToolBar, "green time", nullptr, LAYOUT_CENTER_Y);
    myGreenMode = new FXComboBox(myToolBar, 6, this, MID_SIMSTEP, GUIDesignViewSettingsComboBox1);
    myGreenMode->appendItem("off");
    myGreenMode->appendItem("phase");
    myGreenMode->appendItem("running");
    myGreenMode->setNumVisible(3);

    myIndexMode = new FXCheckButton(myToolBar, TL("phase names"), this, MID_SIMSTEP);

    if (myAmInTrackingMode) {
        myDetectorMode = new FXCheckButton(myToolBar, TL("detectors"), this, MID_SIMSTEP);
        myConditionMode = new FXCheckButton(myToolBar, TL("conditions"), this, MID_SIMSTEP);
    } else {
        myDetectorMode = nullptr;
        myConditionMode = nullptr;
    }
}


void
GUITLLogicPhasesTrackerWindow::create() {
    FXMainWindow::create();
    if (myToolBarDrag != nullptr) {
        myToolBarDrag->create();
    }
}

int
GUITLLogicPhasesTrackerWindow::computeHeight() {
    int newHeight = (int)myTLLogic->getLinks().size() * 20 + 30 + 8 + 30 + 60;
    if (myAmInTrackingMode) {
        newHeight += 20; // time bar
        if (myDetectorMode->getCheck()) {
            newHeight += (int)myTLLogic->getDetectorStates().size() * 20 + 5;
        }
        if (myConditionMode->getCheck()) {
            newHeight += (int)myTLLogic->getConditions().size() * 20 + 5;
        }
    }
    return newHeight;
}

void
GUITLLogicPhasesTrackerWindow::drawValues(GUITLLogicPhasesTrackerPanel& caller) {
    // compute what shall be shown (what is visible)
    myFirstPhase2Show = 0;
    myFirstPhaseOffset = 0;
    SUMOTime leftOffset = 0;
    myFirstDet2Show = 0;
    myFirstDetOffset = 0;
    myFirstCond2Show = 0;
    myFirstCondOffset = 0;
    myFirstTime2Show = 0;
    if (!myAmInTrackingMode) {
        myPhases.clear();
        myDurations.clear();
        myTimeInCycle.clear();
        myPhaseIndex.clear();
        // insert phases
        MSSimpleTrafficLightLogic* simpleTLLogic = dynamic_cast<MSSimpleTrafficLightLogic*>(myTLLogic);
        if (simpleTLLogic == nullptr) {
            return;
        }
        myLastTime = 0;
        myBeginTime = 0;
        int idx = 0;
        for (MSPhaseDefinition* const phase : simpleTLLogic->getPhases()) {
            myPhases.push_back(*phase);
            myDurations.push_back(phase->duration);
            myTimeInCycle.push_back(myLastTime);
            myPhaseIndex.push_back(idx++);
            myLastTime += phase->duration;
        }
        if (myLastTime <= myBeginTime) {
            WRITE_ERROR(TL("Overflow in time computation occurred."));
            return;
        }
    } else {
        SUMOTime beginOffset = TIME2STEPS(myBeginOffset->getValue());
        myBeginTime = myLastTime - beginOffset;
        myFirstTime2Show = myBeginTime;
        // check whether no phases are known at all
        if (myDurations.size() != 0) {
            SUMOTime durs = 0;
            int phaseOffset = (int)myDurations.size() - 1;
            DurationsVector::reverse_iterator i = myDurations.rbegin();
            while (i != myDurations.rend()) {
                if (durs + (*i) > beginOffset) {
                    myFirstPhase2Show = phaseOffset;
                    myFirstPhaseOffset = (durs + (*i)) - beginOffset;
                    break;
                }
                durs += (*i);
                phaseOffset--;
                ++i;
            }
            if (i == myDurations.rend()) {
                // there are too few information stored;
                myFirstPhase2Show = 0;
                myFirstPhaseOffset = 0;
                leftOffset = beginOffset - durs;
            }
        }
        if (myDetectorDurations.size() != 0) {
            SUMOTime durs = 0;
            int phaseOffset = (int)myDetectorDurations.size() - 1;
            DurationsVector::reverse_iterator i = myDetectorDurations.rbegin();
            while (i != myDetectorDurations.rend()) {
                if (durs + (*i) > beginOffset) {
                    myFirstDet2Show = phaseOffset;
                    myFirstDetOffset = (durs + (*i)) - beginOffset;
                    break;
                }
                durs += (*i);
                phaseOffset--;
                ++i;
            }
            if (i == myDetectorDurations.rend()) {
                // there are too few information stored;
                myFirstDet2Show = 0;
                myFirstDetOffset = 0;
            }
        }
        if (myConditionDurations.size() != 0) {
            SUMOTime durs = 0;
            int phaseOffset = (int)myConditionDurations.size() - 1;
            DurationsVector::reverse_iterator i = myConditionDurations.rbegin();
            while (i != myConditionDurations.rend()) {
                if (durs + (*i) > beginOffset) {
                    myFirstCond2Show = phaseOffset;
                    myFirstCondOffset = (durs + (*i)) - beginOffset;
                    break;
                }
                durs += (*i);
                phaseOffset--;
                ++i;
            }
            if (i == myConditionDurations.rend()) {
                // there are too few information stored;
                myFirstCond2Show = 0;
                myFirstCondOffset = 0;
            }
        }
    }
    // begin drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-1, -1, 0);
    glScaled(2, 2, 1);
    glDisable(GL_TEXTURE_2D);
    // draw the horizontal lines dividing the signal groups
    glColor3d(1, 1, 1);
    // compute some values needed more than once
    const double panelHeight = (double) caller.getHeight();
    const double panelWidth = (double) caller.getWidth();
    const double barWidth = MAX2(1.0, panelWidth - 31);
    const double fontHeight = 0.06 * 300. / panelHeight;
    const double fontWidth = 0.06 * 300. / panelWidth;
    const double h9 = 9. / panelHeight;
    const double hTop = 20. / panelHeight;
    const double h11 = 11. / panelHeight;
    const double stateHeight = 16. / panelHeight;
    const double h20 = 20. / panelHeight;
    const double h30 = 15. / panelHeight;
    const double h35 = 34. / panelHeight;
    const double h60 = 70. / panelHeight;
    const double h75 = 73. / panelHeight;
    const double h80 = 90. / panelHeight;
    const double w30 = 30 / panelWidth;
    double h = 1. - hTop;
    // draw the line below indices
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2d(0, h);
    glVertex2d(1, h);
    glEnd();
    // draw the link names and the lines dividing them
    drawNames(myLinkNames, fontHeight, fontWidth, h20, w30, h, 0);
    glBegin(GL_LINES);
    glVertex2d(0, h + h20);
    glVertex2d(1.0, h + h20);
    glEnd();

    // draw the names closure (vertical line)
    h += h20;
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2d(w30, 1.);
    glVertex2d(w30, h);
    glEnd();

    if (myAmInTrackingMode) {
        // optionally draw detector names
        h -= h60;
        if (myDetectorMode->getCheck()) {
            const double top = h;
            glBegin(GL_LINES);
            glVertex2d(0, h);
            glVertex2d(1.0, h);
            glEnd();
            drawNames(myDetectorNames, fontHeight * 0.7, fontWidth * 0.7, h20, w30, h, 3);
            glBegin(GL_LINES);
            glVertex2d(0, h + h20);
            glVertex2d(1.0, h + h20);
            glEnd();
            // draw the names closure (vertical line)
            glColor3d(1, 1, 1);
            glBegin(GL_LINES);
            glVertex2d(30. / panelWidth, top);
            glVertex2d(30. / panelWidth, h + h20);
            glEnd();
            h -= h30;
        }
        // optionally draw condition names
        if (myConditionMode->getCheck()) {
            const double top = h;
            glBegin(GL_LINES);
            glVertex2d(0, h);
            glVertex2d(1.0, h);
            glEnd();
            drawNames(myConditionNames, fontHeight * 0.7, fontWidth * 0.7, h20, w30, h, 3);
            glBegin(GL_LINES);
            glVertex2d(0, h + h20);
            glVertex2d(1.0, h + h20);
            glEnd();
            // draw the names closure (vertical line)
            glColor3d(1, 1, 1);
            glBegin(GL_LINES);
            glVertex2d(30. / panelWidth, top);
            glVertex2d(30. / panelWidth, h + h20);
            glEnd();
        }
    }

    // draw the phases
    // disable value addition while drawing
    myLock.lock();
    // determine the initial offset
    double x = 31. / panelWidth;
    double ta = (double) leftOffset / panelWidth;
    ta *= barWidth / ((double)(myLastTime - myBeginTime));
    x += ta;

    // and the initial phase information
    PhasesVector::iterator pi = myPhases.begin() + myFirstPhase2Show;
    IndexVector::iterator ii = myPhaseIndex.begin() + myFirstPhase2Show;

    SUMOTime fpo = myFirstPhaseOffset;
    const bool phaseNames = myIndexMode->getCheck() == TRUE;
    std::string lastName = "";
    double spaceForName = 0;

    // start drawing
    std::vector<SUMOTime> runningGreen(myTLLogic->getLinks().size(), 0);
    for (DurationsVector::iterator pd = myDurations.begin() + myFirstPhase2Show; pd != myDurations.end(); ++pd) {
        // the first phase may be drawn incompletely
        SUMOTime duration = *pd - fpo;
        // compute the height and the width of the phase
        h = 1. - hTop;
        double a = (double) duration / panelWidth;
        a *= barWidth / ((double)(myLastTime - myBeginTime));
        const double x2 = x + a;

        // go through the links
        for (int j = 0; j < (int) myTLLogic->getLinks().size(); ++j) {
            // determine the current link's color
            LinkState state = pi->getSignalState(j);
            // draw the bar (red is drawn as a line)
            GLHelper::setColor(GUIVisualizationSettings::getLinkColor(state));
            switch (state) {
                case LINKSTATE_TL_RED:
                case LINKSTATE_TL_REDYELLOW:
                    // draw a thin line
                    glBegin(GL_QUADS);
                    glVertex2d(x, h - h11);
                    glVertex2d(x, h - h9);
                    glVertex2d(x2, h - h9);
                    glVertex2d(x2, h - h11);
                    glEnd();
                    break;
                default:
                    // draw a thick block
                    glBegin(GL_QUADS);
                    glVertex2d(x, h - stateHeight);
                    glVertex2d(x, h);
                    glVertex2d(x2, h);
                    glVertex2d(x2, h - stateHeight);
                    glEnd();
                    break;
            }
            if (myGreenMode->getCurrentItem() != 0) {
                SUMOTime drawnDuration = 0;
                double xOffset = 0;
                if (state == LINKSTATE_TL_GREEN_MINOR || state == LINKSTATE_TL_GREEN_MAJOR) {
                    if (myGreenMode->getCurrentItem() == 1) {
                        drawnDuration = *pd;
                    } else {
                        runningGreen[j] += *pd;
                        if (pd + 1 == myDurations.end()) {
                            drawnDuration = runningGreen[j];
                            xOffset =  -(double)(drawnDuration - *pd) / panelWidth * (barWidth / ((double)(myLastTime - myBeginTime)));
                        }
                    }
                } else {
                    if (runningGreen[j] > 0) {
                        drawnDuration = runningGreen[j];
                        xOffset =  -(double)drawnDuration / panelWidth * (barWidth / ((double)(myLastTime - myBeginTime)));
                    }
                    runningGreen[j] = 0;
                }
                if (drawnDuration > 0) {
                    GLHelper::drawText(toString((int)STEPS2TIME(drawnDuration)),
                                       Position(x + xOffset, h - h9),
                                       0, fontHeight, RGBColor::BLACK, 0, FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE, fontWidth);
                }
            }
            // proceed to next link
            h -= h20;
        }

        // draw phase index / name (no names for intermediate)
        std::string name = phaseNames ? pi->getName() : toString(*ii);
        if (name != lastName) {
            const double lastNameWidth = GLHelper::getTextWidth(lastName, fontWidth);
            if (spaceForName < lastNameWidth) {
                // clear space to avoid overdrawn text
                glColor3d(0, 0, 0);
                glBegin(GL_QUADS);
                glVertex2d(x, 1 - fontHeight);
                glVertex2d(x, 1);
                glVertex2d(1, 1);
                glVertex2d(1, 1 - fontHeight);
                glEnd();
            }
            spaceForName = a;
            GLHelper::drawText(name, Position(x, 1 - hTop), 0, fontHeight, RGBColor::WHITE, 0, FONS_ALIGN_LEFT | FONS_ALIGN_BOTTOM, fontWidth);
        } else {
            spaceForName += a;
        }
        lastName = name;
        // proceed to next phase
        ++pi;
        ++ii;
        x = x2;
        // all further phases are drawn in full
        fpo = 0;
    }

    if (myAmInTrackingMode) {
        h -= h75;
        if (myDetectorMode->getCheck()) {
            glColor3d(0.7, 0.7, 1.0);
            drawAdditionalStates(caller, myDetectorStates, myDetectorDurations, myFirstDetOffset, myFirstDet2Show, h,
                                 panelWidth, (double)leftOffset, barWidth, stateHeight, h20, h);
            h -= h35;
        }
        if (myConditionMode->getCheck()) {
            glColor3d(0.9, 0.6, 0.9);
            drawAdditionalStates(caller, myConditionStates, myConditionDurations, myFirstCondOffset, myFirstCond2Show, h,
                                 panelWidth, (double)leftOffset, barWidth, stateHeight, h20, h);
        }
    }
    // allow value addition
    myLock.unlock();

    if (myPhases.size() != 0) {
        const double timeRange = STEPS2TIME(myLastTime - myBeginTime);
        SUMOTime tickDist = TIME2STEPS(10);
        // patch distances - hack
        double t = myBeginOffset != nullptr ? myBeginOffset->getValue() : timeRange;
        while (t > barWidth / 4.) {
            tickDist += TIME2STEPS(10);
            t -= barWidth / 4.;
        }
        // draw time information
        //h = (double)(myTLLogic->getLinks().size() * 20 + 12);
        double glh = 1. - (double)myTLLogic->getLinks().size() * h20 - hTop;
        // current begin time
        // time ticks
        SUMOTime currTime = myFirstTime2Show;
        double glpos = 31. / panelWidth;
        const double ticSize = 4. / panelHeight;
        if (leftOffset > 0) {
            const double a = STEPS2TIME(leftOffset) * barWidth / timeRange;
            glpos += a / panelWidth;
            currTime += leftOffset;
        } else if (myFirstPhaseOffset > 0) {
            const double a = STEPS2TIME(-myFirstPhaseOffset) * barWidth / timeRange;
            glpos += a / panelWidth;
            currTime -= myFirstPhaseOffset;
        }
        int ticShift = myFirstPhase2Show;
        const bool mmSS = myTimeMode->getCurrentItem() == 1;
        const bool cycleTime = myTimeMode->getCurrentItem() == 2;
        SUMOTime lastTimeInCycle = -1;
        lastName = "";
        pi = myPhases.begin() + myFirstPhase2Show;
        for (DurationsVector::iterator pd = myDurations.begin() + myFirstPhase2Show; pd != myDurations.end(); ++pd) {
            const SUMOTime timeInCycle = myTimeInCycle[pd - myDurations.begin()];
            // draw times at different heights
            ticShift = (ticShift % 3) + 1;
            const std::string timeStr = (mmSS
                                         ? StringUtils::padFront(toString((currTime % 3600000) / 60000), 2, '0') + ":"
                                         + StringUtils::padFront(toString((currTime % 60000) / 1000), 2, '0')
                                         : toString((int)STEPS2TIME(cycleTime ? timeInCycle : currTime)));
            const double w = 10 * (double)timeStr.size() / panelWidth;
            glTranslated(glpos - w / 2., glh - h20 * ticShift, 0);
            GLHelper::drawText(timeStr, Position(0, 0), 1, fontHeight, RGBColor::WHITE, 0, FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE, fontWidth);
            glTranslated(-glpos + w / 2., -glh + h20 * ticShift, 0);

            // draw tic
            glColor3d(1, 1, 1);
            glBegin(GL_LINES);
            glVertex2d(glpos, glh);
            glVertex2d(glpos, glh - ticSize * ticShift);
            glEnd();

            // draw vertical lines for names, detectors and conditions on each phase switch
            if (myAmInTrackingMode) {
                double hStart = 1;
                if (!phaseNames || (pi->getName() != lastName)) {
                    glColor3d(0.4, 0.4, 0.4);
                    glBegin(GL_LINES);
                    glVertex2d(glpos, hStart);
                    hStart -= h20;
                    glVertex2d(glpos, hStart);
                    glEnd();
                }
                lastName = pi->getName();

                hStart = glh - h60;
                if (myDetectorMode->getCheck() && glpos >= w30) {
                    glColor3d(0.4, 0.4, 0.4);
                    glBegin(GL_LINES);
                    glVertex2d(glpos, hStart);
                    hStart -= (double)myDetectorNames.size() * h20;
                    glVertex2d(glpos, hStart);
                    glEnd();
                    hStart -= h35;
                }
                if (myConditionMode->getCheck() && glpos >= w30) {
                    glColor3d(0.4, 0.4, 0.4);
                    glBegin(GL_LINES);
                    glVertex2d(glpos, hStart);
                    glVertex2d(glpos, hStart - (double)myConditionNames.size() * h20);
                    glEnd();
                }
            }

            // draw vertical line for cycle reset
            if (timeInCycle == 0 || timeInCycle < lastTimeInCycle) {
                const double cycle0pos = glpos - STEPS2TIME(timeInCycle) * barWidth / timeRange / panelWidth;
                if (cycle0pos >= 31 / panelWidth) {
                    glColor3d(0.6, 0.6, 0.6);
                    glBegin(GL_LINES);
                    glVertex2d(cycle0pos, 1);
                    glVertex2d(cycle0pos, glh);
                    glEnd();
                    glColor3d(1, 1, 1);
                }
            }

            lastTimeInCycle = timeInCycle;
            tickDist = *pd;
            const double a = STEPS2TIME(tickDist) * barWidth / timeRange;
            glpos += a / panelWidth;
            currTime += tickDist;
            ++pi;
        }

        // draw bottom time bar with fixed spacing
        if (myAmInTrackingMode && (myDetectorMode->getCheck() || myConditionMode->getCheck()) && glpos >= w30) {
            glColor3d(1, 1, 1);
            tickDist = TIME2STEPS(10);
            // patch distances - hack
            t = myBeginOffset != nullptr ? myBeginOffset->getValue() : STEPS2TIME(myLastTime - myBeginTime);
            while (t > barWidth / 4.) {
                tickDist += TIME2STEPS(10);
                t -= barWidth / 4.;
            }
            glh = 1. - (double)myLinkNames.size() * h20 - h80;
            glh -= h20 * (double)(myDetectorMode->getCheck() ? myDetectorNames.size() : myConditionNames.size());
            currTime = myFirstTime2Show;
            int pos = 31;
            glpos = (double) pos / panelWidth;
            if (leftOffset > 0) {
                const double a = STEPS2TIME(leftOffset) * barWidth / timeRange;
                pos += (int)a;
                glpos += a / panelWidth;
                currTime += leftOffset;
            } else if (myFirstPhaseOffset > 0) {
                const double a = -STEPS2TIME(myBeginTime % tickDist) * barWidth / timeRange;
                pos += (int)a;
                glpos += a / panelWidth;
                currTime = myBeginTime - (myBeginTime % tickDist);
            }
            while (pos < panelWidth + 50.) {
                const std::string timeStr = (mmSS
                                             ? StringUtils::padFront(toString((currTime % 3600000) / 60000), 2, '0') + ":"
                                             + StringUtils::padFront(toString((currTime % 60000) / 1000), 2, '0')
                                             : toString((int)STEPS2TIME(cycleTime ? findTimeInCycle(currTime) : currTime)));
                const double w = 10. * (double)timeStr.size() / panelWidth;
                glTranslated(glpos - w / 2., glh - h20, 0);
                GLHelper::drawText(timeStr, Position(0, 0), 1, fontHeight, RGBColor::WHITE, 0, FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE, fontWidth);
                glTranslated(-glpos + w / 2., -glh + h20, 0);

                glBegin(GL_LINES);
                glVertex2d(glpos, glh);
                glVertex2d(glpos, glh - ticSize);
                glEnd();

                const double a = STEPS2TIME(tickDist) * barWidth / STEPS2TIME(myLastTime - myBeginTime);
                pos += (int) a;
                glpos += a / panelWidth;
                currTime += tickDist;
            }
        }
    }
}


void
GUITLLogicPhasesTrackerWindow::drawNames(const std::vector<std::string>& names, double fontHeight, double fontWidth, double divHeight, double divWidth, double& h, int extraLines) {
    int i = 0;
    for (const std::string& name : names) {
        // draw the bar
        glBegin(GL_LINES);
        glVertex2d(0, h);
        glVertex2d(divWidth, h);
        glEnd();
        // draw the name
        glTranslated(0, h - divHeight, 0);
        GLHelper::drawText(name, Position(0, 0), 1, fontHeight, RGBColor::WHITE, 0, FONS_ALIGN_LEFT | FONS_ALIGN_BOTTOM, fontWidth);
        glTranslated(0, -h + divHeight, 0);

        if (extraLines > 0 && i > 0 && i % extraLines == 0) {
            glColor3d(0.4, 0.4, 0.4);
            glBegin(GL_LINES);
            glVertex2d(divWidth, h);
            glVertex2d(1.0, h);
            glEnd();
            glColor3d(1, 1, 1);
        }
        h -= divHeight;
        i++;
    }
    h -= divHeight;
}


void
GUITLLogicPhasesTrackerWindow::drawAdditionalStates(GUITLLogicPhasesTrackerPanel& caller,
        const AdditionalStatesVector& states,
        const DurationsVector& durations, SUMOTime firstOffset, int first2Show, double hStart,
        double panelWidth, double leftOffset, double barWidth, double stateHeight, double h20, double& h) {
    double x = 31. / panelWidth;
    double ta = leftOffset / panelWidth;
    ta *= barWidth / ((double)(myLastTime - myBeginTime));
    x += ta;
    auto di = states.begin() + first2Show;
    SUMOTime fpo = firstOffset;

    double mx = caller.getMousePos().x() / caller.getWidth();
    double my = 1 - caller.getMousePos().y() / caller.getHeight();
    std::string tooltip = "";
    // start drawing
    for (auto pd = durations.begin() + first2Show; pd != durations.end(); ++pd) {
        // the first phase may be drawn incompletely
        SUMOTime duration = *pd - fpo;
        // compute the height and the width of the phase
        h = hStart;
        double a = (double) duration / panelWidth;
        a *= barWidth / ((double)(myLastTime - myBeginTime));
        const double x2 = x + a;
        const bool tooltipX = x < mx && mx < x2;
        //std::cout << SIMTIME << " detStates=" << toString(*di) << "\n";
        // go through the detectors
        for (double j : *di) {
            if (j != 0) {
                // draw a thick block
                glBegin(GL_QUADS);
                glVertex2d(x, h - stateHeight);
                glVertex2d(x, h);
                glVertex2d(x2, h);
                glVertex2d(x2, h - stateHeight);
                glEnd();
                if (tooltipX) {
                    const bool tooltipY = (h - stateHeight) < my && my < h;
                    if (tooltipY) {
                        tooltip = toString((int)j);
                    }
                }
            }
            // proceed to next link
            h -= h20;
        }
        // proceed to next phase
        ++di;
        x = x2;
        // all further phases are drawn in full
        fpo = 0;
    }
    if (tooltip != "") {
        // delay tool tip drawing until all bars are drawn to prevent overwriting
        GLHelper::drawText(tooltip, Position(mx, my), 0, h20, RGBColor::YELLOW, 0, FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE, 20 / caller.getWidth());
    }
}

SUMOTime
GUITLLogicPhasesTrackerWindow::findTimeInCycle(SUMOTime t) {
    // find latest cycle reset before t
    int i = (int)myPhases.size() - 1;
    SUMOTime lookBack = myLastTime - t - myDurations.back();
    //std::cout << SIMTIME << " findTimeInCycle t=" << STEPS2TIME(t)
    //    << " last=" << STEPS2TIME(myLastTime)
    //    << " lastDur=" << STEPS2TIME(myDurations.back())
    //    << " lookBack=" << STEPS2TIME(lookBack)
    //    << " i0=" << i;
    // look backwards through the phases until to the first cycle crossing before t
    while (lookBack > 0 && i > 1) {
        i--;
        lookBack -= myDurations[i];
    }
    SUMOTime timeInCycle = myTimeInCycle[i < 0 ? 0 : i];
    //std::cout << " iF=" << i << " lookBack2=" << STEPS2TIME(lookBack) << " tic=" << STEPS2TIME(timeInCycle) << "\n";
    if (lookBack <= 0) {
        return timeInCycle - lookBack;
    }
    return myTLLogic->mapTimeInCycle(t);
}

void
GUITLLogicPhasesTrackerWindow::addValue(std::pair<SUMOTime, MSPhaseDefinition> def) {
    // do not draw while adding
    myLock.lock();
    // set the first time if not set before
    if (myPhases.size() == 0) {
        myBeginTime = def.first;
    }
    // append or set the phase
    if (myPhases.size() == 0 || myPhases.back() != def.second) {
        myPhases.push_back(def.second);
        myDurations.push_back(DELTA_T);
        myTimeInCycle.push_back(myTLLogic->mapTimeInCycle(def.first - DELTA_T));
        myPhaseIndex.push_back(myTLLogic->getCurrentPhaseIndex());
    } else {
        myDurations.back() += DELTA_T;
    }
    // updated detector states
    std::vector<double> detectorStates;
    for (auto item : myTLLogic->getDetectorStates()) {
        detectorStates.push_back(item.second);
    }
    if (myDetectorStates.size() == 0 || myDetectorStates.back() != detectorStates) {
        myDetectorStates.push_back(detectorStates);
        myDetectorDurations.push_back(DELTA_T);
    } else {
        myDetectorDurations.back() += DELTA_T;
    }
    // updated condition states
    std::vector<double> conditionStates;
    for (auto item : myTLLogic->getConditions()) {
        conditionStates.push_back(item.second);
    }
    if (myConditionStates.size() == 0 || myConditionStates.back() != conditionStates) {
        myConditionStates.push_back(conditionStates);
        myConditionDurations.push_back(DELTA_T);
    } else {
        myConditionDurations.back() += DELTA_T;
    }
    // set the last time a phase was added at
    myLastTime = def.first;
    // allow drawing
    myLock.unlock();
}


long
GUITLLogicPhasesTrackerWindow::onConfigure(FXObject* sender, FXSelector sel, void* ptr) {
    myPanel->onConfigure(sender, sel, ptr);
    return FXMainWindow::onConfigure(sender, sel, ptr);
}


long
GUITLLogicPhasesTrackerWindow::onPaint(FXObject* sender, FXSelector sel, void* ptr) {
    myPanel->onPaint(sender, sel, ptr);
    return FXMainWindow::onPaint(sender, sel, ptr);
}


long
GUITLLogicPhasesTrackerWindow::onSimStep(FXObject* sender, FXSelector, void*) {
    if (sender == myDetectorMode || sender == myConditionMode) {
        resize(getWidth(), computeHeight());
    }
    update();
    return 1;
}


void
GUITLLogicPhasesTrackerWindow::setBeginTime(SUMOTime time) {
    myBeginTime = time;
}


void
GUITLLogicPhasesTrackerWindow::saveSettings() {
    getApp()->reg().writeIntEntry("TL_TRACKER", "x", getX());
    getApp()->reg().writeIntEntry("TL_TRACKER", "y", getY());
    getApp()->reg().writeIntEntry("TL_TRACKER", "width", getWidth());
    getApp()->reg().writeIntEntry("TL_TRACKER", "timeRange", (int)myBeginOffset->getValue());
    getApp()->reg().writeIntEntry("TL_TRACKER", "timeMode", myTimeMode->getCurrentItem());
    getApp()->reg().writeIntEntry("TL_TRACKER", "greenMode", (myGreenMode->getCurrentItem()));
    getApp()->reg().writeIntEntry("TL_TRACKER", "indexMode", (int)(myIndexMode->getCheck()));
    getApp()->reg().writeIntEntry("TL_TRACKER", "detectorMode", (int)(myDetectorMode->getCheck()));
    getApp()->reg().writeIntEntry("TL_TRACKER", "conditionMode", (int)(myConditionMode->getCheck()));
}


void
GUITLLogicPhasesTrackerWindow::loadSettings() {
    // ensure window is visible after switching screen resolutions
    const FXint minSize = 400;
    const FXint minTitlebarHeight = 20;
    setX(MAX2(0, MIN2(getApp()->reg().readIntEntry("TL_TRACKER", "x", 150),
                      getApp()->getRootWindow()->getWidth() - minSize)));
    if (myLastY == -1) {
        myLastY = MAX2(minTitlebarHeight,
                       MIN2(getApp()->reg().readIntEntry("TL_TRACKER", "y", 150),
                            getApp()->getRootWindow()->getHeight() - minSize));
    } else {
        myLastY += getHeight() + 20;
    }
    setY(myLastY);
    setWidth(MAX2(getApp()->reg().readIntEntry("TL_TRACKER", "width", 700), minSize));
    myBeginOffset->setValue(getApp()->reg().readIntEntry("TL_TRACKER", "timeRange", (int)myBeginOffset->getValue()));
    myTimeMode->setCurrentItem(getApp()->reg().readIntEntry("TL_TRACKER", "timeMode", myTimeMode->getCurrentItem()));
    myGreenMode->setCurrentItem(getApp()->reg().readIntEntry("TL_TRACKER", "greenMode", myGreenMode->getCurrentItem()));
    myIndexMode->setCheck((FXbool)getApp()->reg().readIntEntry("TL_TRACKER", "indexMode", (int)(myIndexMode->getCheck())));
    myDetectorMode->setCheck((FXbool)getApp()->reg().readIntEntry("TL_TRACKER", "detectorMode", (int)(myDetectorMode->getCheck())));
    myConditionMode->setCheck((FXbool)getApp()->reg().readIntEntry("TL_TRACKER", "conditionMode", (int)(myConditionMode->getCheck())));
}

/****************************************************************************/
