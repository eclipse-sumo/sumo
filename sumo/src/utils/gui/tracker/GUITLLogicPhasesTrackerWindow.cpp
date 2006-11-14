//---------------------------------------------------------------------------//
//                        GUITLLogicPhasesTrackerWindow.cpp -
//  A window displaying the phase diagram of a tl-logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct/Nov 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
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
// Revision 1.11  2006/11/14 06:50:35  dkrajzew
// tls tracker now support switches between logics
//
// Revision 1.10  2006/04/11 11:05:55  dkrajzew
// all structures now return their id via getID()
//
// Revision 1.9  2006/02/23 11:37:01  dkrajzew
// made the tls visualisation adaptive
//
// Revision 1.8  2005/10/07 11:45:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:09:38  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:20:06  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 11:55:38  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.4  2005/05/04 09:22:32  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/01 10:10:47  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2005/01/27 14:34:19  dkrajzew
// added the possibility to display a complete phase
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:55  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/08/02 11:43:16  dkrajzew
// ported to fox 1.2; patched missing unlock on unwished program termination
//
// Revision 1.4  2004/03/19 12:42:31  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/26 09:37:07  dkrajzew
// moving of the view when reaching the left border implemented; display of a
//  time scale implemented
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
#include <utils/glutils/polyfonts.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
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
        ValueSource<CompletePhaseDef> *src)
    : FXMainWindow(app.getApp(), "TLS-Tracker",NULL,NULL,DECOR_ALL,
        20,20,300,200),
    myApplication(&app), myTLLogic(&logic), myAmInTrackingMode(true)
{
    // build the toolbar
    myToolBarDrag = new FXToolBarShell(this,FRAME_NORMAL);
    myToolBar = new FXToolBar(this,myToolBarDrag, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myToolBar, myToolBar, FXToolBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE);
        // interval manipulation
    myBeginOffset = new FXRealSpinDial(myToolBar, 10, this, MID_SIMSTEP, LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
    myBeginOffset->setFormatString("%.0fs");
    myBeginOffset->setIncrements(1,10,100);
    myBeginOffset->setRange(60,3600);
    myBeginOffset->setValue(60);
    //
    myConnector = new GLObjectValuePassConnector<CompletePhaseDef>(wrapper, src, this);
    size_t height = myTLLogic->getLinks().size() * 20 + 30 + 8 + 30;
    app.addChild(this, true);
    for(size_t i=0; i<myTLLogic->getLinks().size(); i++) {
        myLinkNames.push_back(toString<size_t>(i));
    }
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
            FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
            0,0,0,0,0,0,0,0);
    myPanel = new
        GUITLLogicPhasesTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle((logic.getID() + " - " + logic.getSubID() + " - tracker").c_str());
    setIcon( GUIIconSubSys::getIcon(ICON_APP_TLSTRACKER) );
    setHeight(height);
}


GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
        GUIMainWindow &app,
        MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &wrapper,
        const MSSimpleTrafficLightLogic::Phases &phases)
    : FXMainWindow(app.getApp(), "TLS-Tracker",NULL,NULL,DECOR_ALL,
        20,20,300,200),
    myApplication(&app), myTLLogic(&logic), myAmInTrackingMode(false),
    myToolBarDrag(0), myBeginOffset(0)
{
    myConnector = 0;
    size_t height = myTLLogic->getLinks().size() * 20 + 30 + 8;
    setTitle("TLS-Tracker");
    app.addChild(this, true);
    for(size_t i=0; i<myTLLogic->getLinks().size(); i++) {
        myLinkNames.push_back(toString<size_t>(i));
    }
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
            FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
            0,0,0,0,0,0,0,0);
    myPanel = new
        GUITLLogicPhasesTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle((logic.getID() + " - " + logic.getSubID() + " - tracker").c_str());
    setIcon( GUIIconSubSys::getIcon(ICON_APP_TLSTRACKER) );
    setHeight(height);
}


GUITLLogicPhasesTrackerWindow::~GUITLLogicPhasesTrackerWindow()
{
    myApplication->removeChild(this);
    delete myConnector;
    // just to quit cleanly on a failure
    if(myLock.locked()) {
        myLock.unlock();
    }
    delete myToolBarDrag;
}


void
GUITLLogicPhasesTrackerWindow::create()
{
    FXMainWindow::create();
    if(myToolBarDrag!=0) {
        myToolBarDrag->create();
    }
}


int
GUITLLogicPhasesTrackerWindow::getMaxGLWidth() const
{
    return myApplication->getMaxGLWidth();
}


int
GUITLLogicPhasesTrackerWindow::getMaxGLHeight() const
{
    return myApplication->getMaxGLHeight();
}


void
GUITLLogicPhasesTrackerWindow::drawValues(GUITLLogicPhasesTrackerPanel &caller)
{
    // compute what shall be shown (what is visible)
    myFirstPhase2Show = 0;
    myFirstPhaseOffset = 0;
    size_t leftOffset = 0;
    myFirstTime2Show = 0;
    if(!myAmInTrackingMode) {
        myPhases.clear();
        myDurations.clear();
        // insert phases
        const MSSimpleTrafficLightLogic::Phases &phases = static_cast<MSSimpleTrafficLightLogic*>(myTLLogic)->getPhases();
        MSSimpleTrafficLightLogic::Phases::const_iterator j;
        myLastTime = 0;
        myBeginTime = 0;
        for(j=phases.begin(); j!=phases.end(); ++j) {
            myPhases.push_back(SimplePhaseDef((*j)->getDriveMask(), (*j)->getYellowMask()));
            myDurations.push_back((*j)->duration);
            myLastTime += (*j)->duration;
        }
    } else {
        myBeginTime = myLastTime - (size_t) myBeginOffset->getValue();
        myFirstTime2Show = myBeginTime;
        // check whether no phases are known at all
        if(myDurations.size()!=0) {
            size_t durs = 0;
            size_t phaseOffset = myDurations.size() - 1;
            DurationsVector::reverse_iterator i=myDurations.rbegin();
            while(/*noSecs>=0&&*/i!=myDurations.rend()) {
                if(durs+(*i)>(size_t) myBeginOffset->getValue()) {
                    myFirstPhase2Show = phaseOffset;
                    myFirstPhaseOffset = (durs+(*i)) - (size_t) myBeginOffset->getValue();
                    break;
                }
                durs += (*i);
                phaseOffset--;
                ++i;
            }
            if(i==myDurations.rend()) {
                // there are too few information stored;
                myFirstPhase2Show = 0;
                myFirstPhaseOffset = 0;
                leftOffset = (size_t) myBeginOffset->getValue() - durs;
            }
        }
    }
    // begin drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-1, -1, 0);
    glScaled(2, 2, 0);
    glDisable(GL_TEXTURE_2D);
    size_t run = 0;
    // draw the horizontal lines dividing the signal groups
    glColor3d(1, 1, 1);
    // compute some values needed more than once
    SUMOReal height = (SUMOReal) caller.getHeightInPixels();
    SUMOReal width = (SUMOReal) caller.getWidthInPixels();
    pfSetScaleXY((SUMOReal) (.08*300./width), (SUMOReal) (.08*300./height));
    SUMOReal h4 = ((SUMOReal) 4 / height);
    SUMOReal h10 = ((SUMOReal) 10 / height);
    SUMOReal h16 = ((SUMOReal) 16 / height);
    SUMOReal h20 = ((SUMOReal) 20 / height);
    // draw the link names and the lines dividing them
    SUMOReal h = (SUMOReal) (1.0 - h10);
    SUMOReal h2 = 12;
    size_t i;

    for(i=0; i<myTLLogic->getLinks().size()+1; i++) {
        // draw the bar
        glBegin(GL_LINES);
        glVertex2d(0, h);
        glVertex2d((SUMOReal) (30. / width), h);
        glEnd();
        // draw the name
        if(i<myTLLogic->getLinks().size()) {
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
    ta *= (SUMOReal) (((width-31.0) / ((SUMOReal) (myLastTime - myBeginTime))));
    x += ta;

        // and the initial phase information
    PhasesVector::iterator pi = myPhases.begin() + myFirstPhase2Show;
    DurationsVector::iterator pd = myDurations.begin() + myFirstPhase2Show;
    size_t fpo = myFirstPhaseOffset;

    // start drawing
    for(i=30; i<width&&pd!=myDurations.end(); ) {
        // the first phase may be drawn incompletely
        size_t duration = *pd - fpo;
        // compute the heigh and the width of the phase
        h = (SUMOReal) (1.0 - h10);
        SUMOReal a = (SUMOReal) duration / width;
        a *= (SUMOReal) (((width-31.0) / ((SUMOReal) (myLastTime - myBeginTime))));
        SUMOReal x2 = x + a;

        // go through the links
        for(size_t j=0; j<myTLLogic->getLinks().size(); j++) {
            // determine the current link's color
            MSLink::LinkState state =
                (*pi).first.test(j)==true
                ? MSLink::LINKSTATE_TL_GREEN
                : (*pi).second.test(j)==true
                    ? MSLink::LINKSTATE_TL_YELLOW
                    : MSLink::LINKSTATE_TL_RED;
            // draw the bar (red is drawn as a line)
            switch(state) {
            case MSLink::LINKSTATE_TL_GREEN:
                glColor3f(0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2d(x, h - h16);
                glVertex2d(x, h - h4);
                glVertex2d(x2, h - h4);
                glVertex2d(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_YELLOW:
                glColor3f(1.0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2d(x, h - h16);
                glVertex2d(x, h - h4);
                glVertex2d(x2, h - h4);
                glVertex2d(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_RED:
                glColor3f(1.0, 0, 0);
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
    if(myPhases.size()!=0) {
        int tickDist = 10;
        // patch distances - hack
        SUMOReal t = myBeginOffset!=0 ? (SUMOReal) myBeginOffset->getValue() : (SUMOReal) (myLastTime - myBeginTime);
        while(t>(width-31.)/4.) {
            tickDist += 10;
            t -= (SUMOReal) ((width-31.)/4.);
        }
        // draw time information
        h = (SUMOReal) (myTLLogic->getLinks().size() * 20 + 12);
        SUMOReal glh = (SUMOReal) (1.0 - myTLLogic->getLinks().size() * h20 - h10);
            // current begin time
        //string timeStr = toString<SUMOTime>(myFirstTime2Show);
        //SUMOReal w = pfdkGetStringWidth(timeStr.c_str());
        pfSetScaleXY((SUMOReal) (.05*300./width), (SUMOReal) (.05*300./height));
            // time ticks
        SUMOTime currTime = myFirstTime2Show;
        int pos = 31;// + /*!!!currTime*/ - myFirstTime2Show;
        SUMOReal glpos = (SUMOReal) pos / (SUMOReal) width;
        while(pos<width+50) {
            string timeStr = toString<SUMOTime>(currTime);
            SUMOReal w = pfdkGetStringWidth(timeStr.c_str());
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

            SUMOReal a = (SUMOReal) tickDist / width;
            a *= (SUMOReal) (((width-31.0) / ((SUMOReal) (myLastTime - myBeginTime))));
            glpos += a;
            SUMOReal a2 = (SUMOReal) tickDist;
            a2 *= (SUMOReal) (((width-31.0) / ((SUMOReal) (myLastTime - myBeginTime))));
            pos += (size_t) a2;
            currTime += tickDist;
        }
    }
}


void
GUITLLogicPhasesTrackerWindow::addValue(CompletePhaseDef def)
{
    // do not draw while adding
    myLock.lock();
    // set the first time if not set before
    if(myPhases.size()==0) {
        myBeginTime = def.first;
    }
    // append or set the phase
    if(myPhases.size()==0||*(myPhases.end()-1)!=def.second) {
        myPhases.push_back(def.second);
        myDurations.push_back(1);
    } else {
        *(myDurations.end()-1) += 1;
    }
    // set the last time a phase was added at
    myLastTime = def.first;
    // allow drawing
    myLock.unlock();
}



long
GUITLLogicPhasesTrackerWindow::onConfigure(FXObject *sender,
                                           FXSelector sel, void *data)
{
    myPanel->onConfigure(sender, sel, data);
    return FXMainWindow::onConfigure(sender, sel, data);
}


long
GUITLLogicPhasesTrackerWindow::onPaint(FXObject *sender,
                                       FXSelector sel, void *data)
{
    myPanel->onPaint(sender, sel, data);
    return FXMainWindow::onPaint(sender, sel, data);
}


long
GUITLLogicPhasesTrackerWindow::onSimStep(FXObject*sender,
                                         FXSelector,void*)
{
    update();
    return 1;
}



void
GUITLLogicPhasesTrackerWindow::setBeginTime(SUMOTime time)
{
    myBeginTime = time;
}


/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel-methods
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel) GUITLLogicPhasesTrackerPanelMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0, GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onPaint),

};

  // Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel,FXGLCanvas,GUITLLogicPhasesTrackerPanelMap,ARRAYNUMBER(GUITLLogicPhasesTrackerPanelMap))



GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::GUITLLogicPhasesTrackerPanel(
        FXComposite *c, GUIMainWindow &app,
        GUITLLogicPhasesTrackerWindow &parent)
    : FXGLCanvas(c, app.getGLVisual(), app.getBuildGLCanvas(), (FXObject*) 0, (FXSelector) 0, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y/*, 0, 0, 300, 200*/),
    myParent(&parent), myApplication(&app)
{
}


GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::~GUITLLogicPhasesTrackerPanel()
{
    // just to quit cleanly on a failure
    if(_lock.locked()) {
        _lock.unlock();
    }
}


size_t
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::getHeightInPixels() const
{
    return _heightInPixels;
}


size_t
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::getWidthInPixels() const
{
    return _widthInPixels;
}


long
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onConfigure(
        FXObject*,FXSelector,void*)
{
    _lock.lock();
    if(makeCurrent()) {
        _widthInPixels = myParent->getMaxGLWidth();
        _heightInPixels = myParent->getMaxGLHeight();
        glViewport( 0, 0, _widthInPixels-1, _heightInPixels-1 );
        glClearColor( 0, 0, 0, 1 );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_LIGHTING );
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND );
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_COLOR_MATERIAL);
        glLineWidth(1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    _lock.unlock();
    return 1;
}


long
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::onPaint(
        FXObject*,FXSelector,void*)
{
    if(!isEnabled()) {
        return 1;
    }
    if(_lock.locked()) {
        return 1;
    }
    _lock.lock();
    if(makeCurrent()) {
        _widthInPixels = getWidth();
        _heightInPixels = getHeight();
        if(_widthInPixels!=0&&_heightInPixels!=0) {
            glViewport( 0, 0, _widthInPixels-1, _heightInPixels-1 );
            glClearColor( 0, 0, 0, 1 );
            glDisable( GL_DEPTH_TEST );
            glDisable( GL_LIGHTING );
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND );
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_COLOR_MATERIAL);
            glLineWidth(1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // draw
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            myParent->drawValues(*this);
            glFlush();
            swapBuffers();
        }
        makeNonCurrent();
    }
    _lock.unlock();
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



