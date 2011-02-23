/****************************************************************************/
/// @file    GUITLLogicPhasesTrackerWindow.cpp
/// @author  Daniel Krajzewicz
/// @date    Oct/Nov 2003
/// @version $Id$
///
// A window displaying the phase diagram of a tl-logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <vector>
#include <iostream>
#include <utils/gui/windows/GUIMainWindow.h>
#include "GUITLLogicPhasesTrackerWindow.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSLink.h>
#include <utils/common/ToString.h>
#include <guisim/GUITrafficLightLogicWrapper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel-callbacks
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel) GUITLLogicPhasesTrackerPanelMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onPaint),

};

// Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel,FXGLCanvas,GUITLLogicPhasesTrackerPanelMap,ARRAYNUMBER(GUITLLogicPhasesTrackerPanelMap))



/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel-methods
 * ----------------------------------------------------------------------- */
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::GUITLLogicPhasesTrackerPanel(
    FXComposite *c, GUIMainWindow &app,
    GUITLLogicPhasesTrackerWindow &parent) throw()
        : FXGLCanvas(c, app.getGLVisual(), app.getBuildGLCanvas(), (FXObject*) 0, (FXSelector) 0, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y/*, 0, 0, 300, 200*/),
        myParent(&parent), myApplication(&app) {}


GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::~GUITLLogicPhasesTrackerPanel() throw() {}


long
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onConfigure(
    FXObject*,FXSelector,void*) {
    if (makeCurrent()) {
        int widthInPixels = getWidth();
        int heightInPixels = getHeight();
        if (widthInPixels!=0&&heightInPixels!=0) {
            glViewport(0, 0, widthInPixels-1, heightInPixels-1);
            glClearColor(0, 0, 0, 1);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
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
    FXObject*,FXSelector,void*) {
    if (!isEnabled()) {
        return 1;
    }
    if (makeCurrent()) {
        int widthInPixels = getWidth();
        int heightInPixels = getHeight();
        if (widthInPixels!=0&&heightInPixels!=0) {
            glViewport(0, 0, widthInPixels-1, heightInPixels-1);
            glClearColor(0, 0, 0, 1);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
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



/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow - FOX callback mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITLLogicPhasesTrackerWindow) GUITLLogicPhasesTrackerWindowMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0,           GUITLLogicPhasesTrackerWindow::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0,           GUITLLogicPhasesTrackerWindow::onPaint),
    FXMAPFUNC(SEL_COMMAND,   MID_SIMSTEP, GUITLLogicPhasesTrackerWindow::onSimStep),

};

FXIMPLEMENT(GUITLLogicPhasesTrackerWindow,FXMainWindow,GUITLLogicPhasesTrackerWindowMap,ARRAYNUMBER(GUITLLogicPhasesTrackerWindowMap))


/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow-methods
 * ----------------------------------------------------------------------- */
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
    GUIMainWindow &app,
    MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &wrapper,
    ValueSource<std::pair<SUMOTime, MSPhaseDefinition> > *src) throw()
        : FXMainWindow(app.getApp(), "TLS-Tracker",NULL,NULL,DECOR_ALL,
                       20,20,300,200),
        myApplication(&app), myTLLogic(&logic), myAmInTrackingMode(true) {
    // build the toolbar
    myToolBarDrag = new FXToolBarShell(this,FRAME_NORMAL);
    myToolBar = new FXToolBar(this,myToolBarDrag, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myToolBar, myToolBar, FXToolBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE);
    // interval manipulation
    myBeginOffset = new FXRealSpinDial(myToolBar, 10, this, MID_SIMSTEP, LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
    myBeginOffset->setFormatString("%.0f");
    myBeginOffset->setIncrements(1,10,100);
    myBeginOffset->setRange(60,3600);
    myBeginOffset->setValue(240);
    new FXLabel(myToolBar, "(s)", 0, LAYOUT_CENTER_Y);
    //
    myConnector = new GLObjectValuePassConnector<std::pair<SUMOTime, MSPhaseDefinition> >(wrapper, src, this);
    FXint height = (FXint)(myTLLogic->getLinks().size() * 20 + 30 + 8 + 30);
    app.addChild(this, true);
    for (size_t i=0; i<myTLLogic->getLinks().size(); i++) {
        myLinkNames.push_back(toString<size_t>(i));
    }
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
                            FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                            0,0,0,0,0,0,0,0);
    myPanel = new
    GUITLLogicPhasesTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle((logic.getID() + " - " + logic.getProgramID() + " - tracker").c_str());
    setIcon(GUIIconSubSys::getIcon(ICON_APP_TLSTRACKER));
    setHeight(height);
}


GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
    GUIMainWindow &app,
    MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &/*wrapper*/,
    const MSSimpleTrafficLightLogic::Phases &/*phases*/) throw()
        : FXMainWindow(app.getApp(), "TLS-Tracker",NULL,NULL,DECOR_ALL,
                       20,20,300,200),
        myApplication(&app), myTLLogic(&logic), myAmInTrackingMode(false),
        myToolBarDrag(0), myBeginOffset(0) {
    myConnector = 0;
    FXint height = (FXint)(myTLLogic->getLinks().size() * 20 + 30 + 8);
    setTitle("TLS-Tracker");
    app.addChild(this, true);
    for (size_t i=0; i<myTLLogic->getLinks().size(); i++) {
        myLinkNames.push_back(toString<size_t>(i));
    }
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
                            FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                            0,0,0,0,0,0,0,0);
    myPanel = new
    GUITLLogicPhasesTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle((logic.getID() + " - " + logic.getProgramID() + " - tracker").c_str());
    setIcon(GUIIconSubSys::getIcon(ICON_APP_TLSTRACKER));
    setHeight(height);
}


GUITLLogicPhasesTrackerWindow::~GUITLLogicPhasesTrackerWindow() throw() {
    myApplication->removeChild(this);
    delete myConnector;
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
    delete myToolBarDrag;
}


void
GUITLLogicPhasesTrackerWindow::create() {
    FXMainWindow::create();
    if (myToolBarDrag!=0) {
        myToolBarDrag->create();
    }
}


void
GUITLLogicPhasesTrackerWindow::drawValues(GUITLLogicPhasesTrackerPanel &caller) throw() {
    // compute what shall be shown (what is visible)
    myFirstPhase2Show = 0;
    myFirstPhaseOffset = 0;
    size_t leftOffset = 0;
    myFirstTime2Show = 0;
    if (!myAmInTrackingMode) {
        myPhases.clear();
        myDurations.clear();
        // insert phases
        const MSSimpleTrafficLightLogic::Phases &phases = static_cast<MSSimpleTrafficLightLogic*>(myTLLogic)->getPhases();
        MSSimpleTrafficLightLogic::Phases::const_iterator j;
        myLastTime = 0;
        myBeginTime = 0;
        for (j=phases.begin(); j!=phases.end(); ++j) {
            myPhases.push_back(*(*j));
            myDurations.push_back((*j)->duration);
            myLastTime += (*j)->duration;
        }
    } else {
        SUMOTime beginOffset = TIME2STEPS(myBeginOffset->getValue());
        myBeginTime = myLastTime - beginOffset;
        myFirstTime2Show = myBeginTime;
        // check whether no phases are known at all
        if (myDurations.size()!=0) {
            SUMOTime durs = 0;
            size_t phaseOffset = myDurations.size() - 1;
            DurationsVector::reverse_iterator i=myDurations.rbegin();
            while (i!=myDurations.rend()) {
                if (durs+(*i)>beginOffset) {
                    myFirstPhase2Show = phaseOffset;
                    myFirstPhaseOffset = (durs+(*i)) - beginOffset;
                    break;
                }
                durs += (*i);
                phaseOffset--;
                ++i;
            }
            if (i==myDurations.rend()) {
                // there are too few information stored;
                myFirstPhase2Show = 0;
                myFirstPhaseOffset = 0;
                leftOffset = beginOffset - durs;
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
    const SUMOReal height = (SUMOReal) caller.getHeight();
    const SUMOReal width = (SUMOReal) caller.getWidth();
    pfSetScaleXY((SUMOReal)(.08*300./width), (SUMOReal)(.08*300./height));
    const SUMOReal h4 = ((SUMOReal) 4 / height);
    const SUMOReal h10 = ((SUMOReal) 10 / height);
    const SUMOReal h16 = ((SUMOReal) 16 / height);
    const SUMOReal h20 = ((SUMOReal) 20 / height);
    // draw the link names and the lines dividing them
    SUMOReal h = (SUMOReal)(1.0 - h10);
    SUMOReal h2 = 12;
    for (size_t i=0; i<myTLLogic->getLinks().size()+1; i++) {
        // draw the bar
        glBegin(GL_LINES);
        glVertex2d(0, h);
        glVertex2d((SUMOReal)(30. / width), h);
        glEnd();
        // draw the name
        if (i<myTLLogic->getLinks().size()) {
            glRotated(180, 1, 0, 0);
            pfSetPosition(0, 0);
            glTranslated(0.0, -h+h20-h4, 0);
            pfDrawString(myLinkNames[i].c_str());
            glTranslated(-0.0, h-h20+h4, 0);
            glRotated(-180, 1, 0, 0);
            h2 += 20;
        }
        h -= h20;
    }
    glBegin(GL_LINES);
    glVertex2d(0, h+h20);
    glVertex2d(1.0, h+h20);
    glEnd();

    // draw the names closure (vertical line)
    h += (SUMOReal) 20 / height;
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2d((SUMOReal) 30 / width, 1.0);
    glVertex2d((SUMOReal) 30 / width, h);
    glEnd();


    // draw the phases
    // disable value addition while drawing
    myLock.lock();
    // determine the initial offset
    SUMOReal x = ((SUMOReal) 31. / width) ;
    SUMOReal ta = (SUMOReal) leftOffset / width;
    ta *= (SUMOReal)(((width-31.0) / ((SUMOReal)(myLastTime - myBeginTime))));
    x += ta;

    // and the initial phase information
    PhasesVector::iterator pi = myPhases.begin() + myFirstPhase2Show;
    DurationsVector::iterator pd = myDurations.begin() + myFirstPhase2Show;
    size_t fpo = myFirstPhaseOffset;

    // start drawing
    for (size_t i=30; pd!=myDurations.end();) {
        // the first phase may be drawn incompletely
        size_t duration = *pd - fpo;
        // compute the heigh and the width of the phase
        h = (SUMOReal)(1.0 - h10);
        SUMOReal a = (SUMOReal) duration / width;
        a *= (SUMOReal)(((width-31.0) / ((SUMOReal)(myLastTime - myBeginTime))));
        SUMOReal x2 = x + a;

        // go through the links
        for (unsigned int j=0; j<(unsigned int) myTLLogic->getLinks().size(); j++) {
            // determine the current link's color
            MSLink::LinkState state = (*pi).getSignalState(j);
            // draw the bar (red is drawn as a line)
            switch (state) {
            case MSLink::LINKSTATE_TL_GREEN_MAJOR:
            case MSLink::LINKSTATE_TL_GREEN_MINOR:
                glColor3d(0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2d(x, h - h16);
                glVertex2d(x, h - h4);
                glVertex2d(x2, h - h4);
                glVertex2d(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_YELLOW_MAJOR:
            case MSLink::LINKSTATE_TL_YELLOW_MINOR:
                glColor3d(1.0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2d(x, h - h16);
                glVertex2d(x, h - h4);
                glVertex2d(x2, h - h4);
                glVertex2d(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_RED:
                glColor3d(1.0, 0, 0);
                glBegin(GL_LINES);
                glVertex2d(x, h - h10);
                glVertex2d(x2, h - h10);
                glEnd();
                break;
            default:
                break;
            }
            // proceed to next link
            h -= h20;
        }
        // proceed to next phase
        i += duration;
        pi++;
        pd++;
        x = x2;
        // all further phases are drawn in full
        fpo = 0;
    }
    // allow value addition
    myLock.unlock();

    glColor3d(1, 1, 1);
    if (myPhases.size()!=0) {
        SUMOTime tickDist = TIME2STEPS(10);
        // patch distances - hack
        SUMOReal t = myBeginOffset!=0 ? (SUMOReal) myBeginOffset->getValue() : STEPS2TIME(myLastTime - myBeginTime);
        while (t>(width-31.)/4.) {
            tickDist += TIME2STEPS(10);
            t -= (SUMOReal)((width-31.)/4.);
        }
        // draw time information
        h = (SUMOReal)(myTLLogic->getLinks().size() * 20 + 12);
        SUMOReal glh = (SUMOReal)(1.0 - myTLLogic->getLinks().size() * h20 - h10);
        // current begin time
        pfSetScaleXY((SUMOReal)(.05*300./width), (SUMOReal)(.05*300./height));
        // time ticks
        SUMOTime currTime = myFirstTime2Show;
        int pos = 31;// + /*!!!currTime*/ - myFirstTime2Show;
        SUMOReal glpos = (SUMOReal) pos / width;
        while (pos<width+50) {
            const std::string timeStr = time2string(currTime);
            const SUMOReal w = pfdkGetStringWidth(timeStr.c_str());
            glRotated(180, 1, 0, 0);
            pfSetPosition(0, 0);
            glTranslated(glpos-w/2., -glh+h20-h4, 0);
            pfDrawString(timeStr.c_str());
            glTranslated(-glpos+w/2., glh-h20+h4, 0);
            glRotated(-180, 1, 0, 0);

            glBegin(GL_LINES);
            glVertex2d(glpos, glh);
            glVertex2d(glpos, glh-h4);
            glEnd();

            const SUMOReal a = STEPS2TIME(tickDist) * (width-31.0) / STEPS2TIME(myLastTime - myBeginTime);
            pos += (int) a;
            glpos += a / width;
            currTime += tickDist;
        }
    }
}


void
GUITLLogicPhasesTrackerWindow::addValue(std::pair<SUMOTime, MSPhaseDefinition> def) throw() {
    // do not draw while adding
    myLock.lock();
    // set the first time if not set before
    if (myPhases.size()==0) {
        myBeginTime = def.first;
    }
    // append or set the phase
    if (myPhases.size()==0||*(myPhases.end()-1)!=def.second) {
        myPhases.push_back(def.second);
        myDurations.push_back(DELTA_T);
    } else {
        *(myDurations.end()-1) += DELTA_T;
    }
    // set the last time a phase was added at
    myLastTime = def.first;
    // allow drawing
    myLock.unlock();
}


long
GUITLLogicPhasesTrackerWindow::onConfigure(FXObject *sender,
        FXSelector sel, void *data) {
    myPanel->onConfigure(sender, sel, data);
    return FXMainWindow::onConfigure(sender, sel, data);
}


long
GUITLLogicPhasesTrackerWindow::onPaint(FXObject *sender,
                                       FXSelector sel, void *data) {
    myPanel->onPaint(sender, sel, data);
    return FXMainWindow::onPaint(sender, sel, data);
}


long
GUITLLogicPhasesTrackerWindow::onSimStep(FXObject*,
        FXSelector,void*) {
    update();
    return 1;
}


void
GUITLLogicPhasesTrackerWindow::setBeginTime(SUMOTime time) throw() {
    myBeginTime = time;
}


/****************************************************************************/

