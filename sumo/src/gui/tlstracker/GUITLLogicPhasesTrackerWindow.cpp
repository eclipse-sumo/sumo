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
// Revision 1.3  2003/11/26 09:37:07  dkrajzew
// moving of the view when reaching the left border implemented; display of a time scale implemented
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include <vector>
#include <qgl.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <iostream>
#include <gui/GUIApplicationWindow.h>
#include "GUITLLogicPhasesTrackerWindow.h"
#include <microsim/MSTrafficLightLogic.h>
#include <microsim/MSLink.h>
#include <utils/convert/ToString.h>

#ifndef WIN32
#include "GUITLLogicPhasesTrackerWindow.moc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow-methods
 * ----------------------------------------------------------------------- */
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
        GUIApplicationWindow &app, MSTrafficLightLogic &logic)
        : myApplication(app), myTLLogic(logic)
{
    size_t height = myTLLogic.getLinks().size() * 20 + 30;
    setCaption("Tracker");
    setBaseSize(300, height);
    setMinimumSize(300, height);
    app.addChild(this, true);
    for(size_t i=0; i<myTLLogic.getLinks().size(); i++) {
        myLinkNames.push_back(toString<size_t>(i));
    }
    myPanel = new
        GUITLLogicPhasesTrackerPanel(myApplication, *this);
    setCentralWidget(myPanel);
    show();
}


GUITLLogicPhasesTrackerWindow::~GUITLLogicPhasesTrackerWindow()
{
}


bool
GUITLLogicPhasesTrackerWindow::event ( QEvent *e )
{
    if(e->type()!=QEvent::User) {
        return QMainWindow::event(e);
    }
    update();
    return TRUE;
}


int
GUITLLogicPhasesTrackerWindow::getMaxGLWidth() const
{
    return myApplication.getMaxGLWidth();
}


int
GUITLLogicPhasesTrackerWindow::getMaxGLHeight() const
{
    return myApplication.getMaxGLHeight();
}


void
GUITLLogicPhasesTrackerWindow::paintEvent ( QPaintEvent *e )
{
    myPanel->paintEvent(e);
    QMainWindow::paintEvent(e);
}


void
GUITLLogicPhasesTrackerWindow::resizeEvent ( QResizeEvent *e )
{
	QMainWindow::resizeEvent(e);
}


void
GUITLLogicPhasesTrackerWindow::drawValues(GUITLLogicPhasesTrackerPanel &caller)
{
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
    myFontRenderer.SetColor(1, 1, 1);
    // compute some values needed more than once
    double height = (double) caller.getHeightInPixels();
    double width = (double) caller.getWidthInPixels();
    double h4 = ((double) 4 / height);
    double h10 = ((double) 10 / height);
    double h16 = ((double) 16 / height);
    double h20 = ((double) 20 / height);
    // draw the link names and the lines dividing them
    myFontRenderer.SetActiveFont("std10");
    double h = 1.0 - h10;
    double h2 = 12;
    size_t i;
    glBegin(GL_LINES);
    for(i=0; i<myTLLogic.getLinks().size()+1; i++) {
        // draw the bar
        glVertex2f(0, h);
        glVertex2f((double) 30 / width, h);
        // draw the name
        if(i<myTLLogic.getLinks().size()) {
            myFontRenderer.StringOut(2, h2 - h10, myLinkNames[i]);
            h2 += 20;
        }
        h -= h20;
    }
    glVertex2f(0, h+h20);
    glVertex2f(1.0, h+h20);
    glEnd();

    // draw the names closure (vertical line)
    h += (double) 20 / height;
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f((double) 30 / width, 1.0);
    glVertex2f((double) 30 / width, h);
    glEnd();


    // draw the phases
        // disable value addition while drawing
    myLock.lock();
        // determine the initial offset
    double x = (double) 31 / width;
    computeOffsets(width, 31);
        // and the initial phase information
    PhasesVector::iterator pi = myPhases.begin() + myFirstPhase2Show;
    DurationsVector::iterator pd = myDurations.begin() + myFirstPhase2Show;
    size_t fpo = myFirstPhaseOffset;
        // start drawing
    for(i=30; i<width&&pd!=myDurations.end(); ) {
        // the first phase may be drawn incompletely
        size_t duration = *pd - fpo;
        // compute the heigh and the width of the phase
        h = 1.0 - h10;
        double x2 = x + (double) duration / width;
        // go through the links
        for(size_t j=0; j<myTLLogic.getLinks().size(); j++) {
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
                glVertex2f(x, h - h16);
                glVertex2f(x, h - h4);
                glVertex2f(x2, h - h4);
                glVertex2f(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_YELLOW:
                glColor3f(1.0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2f(x, h - h16);
                glVertex2f(x, h - h4);
                glVertex2f(x2, h - h4);
                glVertex2f(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_RED:
                glColor3f(1.0, 0, 0);
                glBegin(GL_LINES);
                glVertex2f(x, h - h10);
                glVertex2f(x2, h - h10);
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

    if(myPhases.size()!=0) {
        myFontRenderer.SetActiveFont("std8");
        int tickDist = 60;
        // draw time information
        h = myTLLogic.getLinks().size() * 20 + 12;
        float glh = 1.0 - myTLLogic.getLinks().size() * h20 - h10;
            // current begin time
        string timeStr = toString<MSNet::Time>(myFirstTime2Show);
        myFontRenderer.StringOut(
            31-myFontRenderer.GetStringWidth(timeStr), h, timeStr);
            // time ticks
        MSNet::Time currTime =
            ((MSNet::Time) (myFirstTime2Show / tickDist) + 1) * tickDist;
        size_t pos = 31 + currTime - myFirstTime2Show;
        float glpos = (pos) / width;
        glColor3d(1, 1, 1);
        while(pos<width+50) {
            timeStr = toString<MSNet::Time>(currTime);
            myFontRenderer.StringOut(
                pos-myFontRenderer.GetStringWidth(timeStr), h, timeStr);
            pos += tickDist;
            currTime += tickDist;
            glBegin(GL_LINES);
            glVertex2f(glpos, glh);
            glVertex2f(glpos, glh-h4);
            glEnd();
            glpos += (tickDist/width);
        }
    }

    // set written strings
    myFontRenderer.Draw(caller.getWidthInPixels(),
        caller.getHeightInPixels());
}


void
GUITLLogicPhasesTrackerWindow::computeOffsets(size_t width,
                                              size_t firstXPixelOffset)
{
    myFirstPhase2Show = 0;
    myFirstPhaseOffset = 0;
    myFirstTime2Show = myBeginTime;
    // check whether no phases are known at all
    if(myDurations.size()==0) {
        return;// std::pair<size_t, size_t>(0, 0);
    }
    // go through the phases starting at their end
    int noSecs = width - firstXPixelOffset;
    size_t timeOffset = 0;
    DurationsVector::reverse_iterator i=myDurations.rbegin();
    while(noSecs>=0&&i!=myDurations.rend()) {
        timeOffset += (*i);
        noSecs -= (*i++);
    }
    // check whether all known phases have enough place
    if(i==myDurations.rend()) {
        return;// std::pair<size_t, size_t>(0, 0);
    }
    // otherwise, return the offsets
    i--;
    size_t phaseOffset = distance(i, myDurations.rend()-1);
    size_t firstOffset = (-noSecs);
    myFirstTime2Show = myLastTime - (timeOffset-firstOffset);
    if(*i==firstOffset) {
        phaseOffset++;
        firstOffset = 0;
    }
    myFirstPhase2Show = phaseOffset;
    myFirstPhaseOffset = firstOffset;
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

void
GUITLLogicPhasesTrackerWindow::setFontRenderer(GUITLLogicPhasesTrackerPanel &caller)
{
    myFontRenderer.add(myApplication.myFonts.get("std8"));
    myFontRenderer.add(myApplication.myFonts.get("std10"));
}


/* -------------------------------------------------------------------------
 * GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel-methods
 * ----------------------------------------------------------------------- */
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::GUITLLogicPhasesTrackerPanel(
        GUIApplicationWindow &app, GUITLLogicPhasesTrackerWindow &parent)
    : QGLWidget(&parent),
    myParent(parent), _noDrawing(0), myApplication(app)
{
}

GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::~GUITLLogicPhasesTrackerPanel()
{
}


void
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::initializeGL()
{
    _lock.lock();
    _widthInPixels = myParent.getMaxGLWidth();
    _heightInPixels = myParent.getMaxGLHeight();
    glViewport( 0, 0, myParent.getMaxGLWidth()-1, myParent.getMaxGLHeight()-1 );
    glClearColor( 0, 0, 0, 1 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND );
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    myParent.setFontRenderer(*this);
    _lock.unlock();
}


void
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::resizeGL( int width, int height )
{
    _lock.lock();
    _widthInPixels = width;
    _heightInPixels = height;
    glViewport( 0, 0, _widthInPixels, _heightInPixels );
    _lock.unlock();
}

void
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::paintGL()
{
    _lock.lock();
    // return if the canvas is not visible
    if(!isVisible()) {
		_lock.unlock();
        return;
    }
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    _widthInPixels = width();
    _heightInPixels = height();
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    myParent.drawValues(*this);
    _noDrawing--;
    glFlush();
    swapBuffers();
    _lock.unlock();
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



