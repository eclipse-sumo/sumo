/****************************************************************************/
/// @file    GUIMainWindow.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
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

#include <string>
#include "GUIMainWindow.h"
#include <algorithm>
#include "GUIAppEnum.h"
#include <fx3d.h>
#include <utils/common/StringUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIMainWindow::GUIMainWindow(FXApp* a, int glWidth, int glHeight)
        : FXMainWindow(a,"SUMO-gui main window",NULL,NULL,DECOR_ALL,20,20,600,400),
        myGLVisual(new FXGLVisual(a, VISUAL_DOUBLEBUFFER|VISUAL_STEREO)),
        myGLWidth(glWidth), myGLHeight(glHeight), myRunAtBegin(false) {

    FXFontDesc fdesc;
    getApp()->getNormalFont()->getFontDesc(fdesc);
    fdesc.weight = FXFont::Bold;
    myBoldFont = new FXFont(getApp(), fdesc);

    myTopDock=new FXDockSite(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
    myBottomDock=new FXDockSite(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
    myLeftDock=new FXDockSite(this,LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y);
    myRightDock=new FXDockSite(this,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y);
}


GUIMainWindow::~GUIMainWindow() {
    delete myBoldFont;
    delete myTopDock;
    delete myBottomDock;
    delete myLeftDock;
    delete myRightDock;
}



void
GUIMainWindow::addChild(FXMDIChild *child, bool /*updateOnSimStep !!!*/) {
    mySubWindows.push_back(child);
}


void
GUIMainWindow::removeChild(FXMDIChild *child) {
    std::vector<FXMDIChild*>::iterator i =
        std::find(mySubWindows.begin(), mySubWindows.end(), child);
    if (i!=mySubWindows.end()) {
        mySubWindows.erase(i);
    }
}


void
GUIMainWindow::addChild(FXMainWindow *child, bool /*updateOnSimStep !!!*/) {
    myTrackerLock.lock();
    myTrackerWindows.push_back(child);
    myTrackerLock.unlock();
}


void
GUIMainWindow::removeChild(FXMainWindow *child) {
    myTrackerLock.lock();
    std::vector<FXMainWindow*>::iterator i =
        std::find(myTrackerWindows.begin(), myTrackerWindows.end(), child);
    myTrackerWindows.erase(i);
    myTrackerLock.unlock();
}


FXFont *
GUIMainWindow::getBoldFont() {
    return myBoldFont;
}


int
GUIMainWindow::getMaxGLWidth() const {
    return myGLWidth;
}


int
GUIMainWindow::getMaxGLHeight() const {
    return myGLHeight;
}


void
GUIMainWindow::updateChildren() {
    // inform views
    myMDIClient->forallWindows(this, FXSEL(SEL_COMMAND, MID_SIMSTEP), 0);
    // inform other windows
    myTrackerLock.lock();
    for (size_t i=0; i<myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->handle(this,FXSEL(SEL_COMMAND,MID_SIMSTEP), 0);
    }
    myTrackerLock.unlock();
}


FXGLVisual *
GUIMainWindow::getGLVisual() const {
    return myGLVisual;
}


FXLabel &
GUIMainWindow::getCartesianLabel() {
    return *myCartesianCoordinate;
}


FXLabel &
GUIMainWindow::getGeoLabel() {
    return *myGeoCoordinate;
}



/****************************************************************************/

