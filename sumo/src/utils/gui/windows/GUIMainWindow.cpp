/****************************************************************************/
/// @file    GUIMainWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <algorithm>
#include <fx.h>
// fx3d includes windows.h so we need to guard against macro pollution
#ifdef WIN32
#define NOMINMAX
#endif
#include <fx3d.h>
#ifdef WIN32
#undef NOMINMAX
#endif
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplCheck.h>
#include <utils/common/TplConvert.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/options/OptionsCont.h>
#include "GUIAppEnum.h"
#include "GUIMainWindow.h"
#include "GUIGlChildWindow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
GUIMainWindow* GUIMainWindow::myInstance = 0;

// ===========================================================================
// member method definitions
// ===========================================================================
GUIMainWindow::GUIMainWindow(FXApp* a) :
    FXMainWindow(a, "SUMO-gui main window", NULL, NULL, DECOR_ALL, 20, 20, 600, 400),
    myGLVisual(new FXGLVisual(a, VISUAL_DOUBLEBUFFER)),
    myAmGaming(false),
    myListInternal(false),
    myListParking(true),
    myListTeleporting(false) {

    FXFontDesc fdesc;
    getApp()->getNormalFont()->getFontDesc(fdesc);
    fdesc.weight = FXFont::Bold;
    myBoldFont = new FXFont(getApp(), fdesc);

    myTopDock = new FXDockSite(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X);
    myBottomDock = new FXDockSite(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X);
    myLeftDock = new FXDockSite(this, LAYOUT_SIDE_LEFT | LAYOUT_FILL_Y);
    myRightDock = new FXDockSite(this, LAYOUT_SIDE_RIGHT | LAYOUT_FILL_Y);
    if (myInstance != 0) {
        throw ProcessError("MainWindow initialized twice");
    }
    myInstance = this;
    //myGLVisual->setStencilSize(8); // enable stencil buffer
}


GUIMainWindow::~GUIMainWindow() {
    delete myBoldFont;
    delete myTopDock;
    delete myBottomDock;
    delete myLeftDock;
    delete myRightDock;
}



void
GUIMainWindow::addChild(FXMDIChild* child, bool /*updateOnSimStep !!!*/) {
    mySubWindows.push_back(child);
}


void
GUIMainWindow::removeChild(FXMDIChild* child) {
    std::vector<FXMDIChild*>::iterator i = std::find(mySubWindows.begin(), mySubWindows.end(), child);
    if (i != mySubWindows.end()) {
        mySubWindows.erase(i);
    }
}


void
GUIMainWindow::addChild(FXMainWindow* child, bool /*updateOnSimStep !!!*/) {
    myTrackerLock.lock();
    myTrackerWindows.push_back(child);
    myTrackerLock.unlock();
}


void
GUIMainWindow::removeChild(FXMainWindow* child) {
    myTrackerLock.lock();
    std::vector<FXMainWindow*>::iterator i = std::find(myTrackerWindows.begin(), myTrackerWindows.end(), child);
    myTrackerWindows.erase(i);
    myTrackerLock.unlock();
}


std::vector<std::string>
GUIMainWindow::getViewIDs() const {
    std::vector<std::string> ret;
    for (std::vector<FXMDIChild*>::const_iterator i = mySubWindows.begin(); i != mySubWindows.end(); ++i) {
        ret.push_back((*i)->getTitle().text());
    }
    return ret;
}


FXMDIChild*
GUIMainWindow::getViewByID(const std::string& id) const {
    for (std::vector<FXMDIChild*>::const_iterator i = mySubWindows.begin(); i != mySubWindows.end(); ++i) {
        if (std::string((*i)->getTitle().text()) == id) {
            return *i;
        }
    }
    return 0;
}


FXFont*
GUIMainWindow::getBoldFont() {
    return myBoldFont;
}


void
GUIMainWindow::updateChildren() {
    // inform views
    myMDIClient->forallWindows(this, FXSEL(SEL_COMMAND, MID_SIMSTEP), 0);
    // inform other windows
    myTrackerLock.lock();
    for (int i = 0; i < (int)myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->handle(this, FXSEL(SEL_COMMAND, MID_SIMSTEP), 0);
    }
    myTrackerLock.unlock();
}


FXGLVisual*
GUIMainWindow::getGLVisual() const {
    return myGLVisual;
}


FXLabel&
GUIMainWindow::getCartesianLabel() {
    return *myCartesianCoordinate;
}


FXLabel&
GUIMainWindow::getGeoLabel() {
    return *myGeoCoordinate;
}


GUIMainWindow*
GUIMainWindow::getInstance() {
    if (myInstance != 0) {
        return myInstance;
    }
    throw ProcessError("A GUIMainWindow instance was not yet constructed.");
}


GUISUMOAbstractView*
GUIMainWindow::getActiveView() const {
    GUIGlChildWindow* w = dynamic_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    if (w != 0) {
        return w->getView();
    }
    return 0;
}

void
GUIMainWindow::setWindowSizeAndPos() {
    int windowWidth = getApp()->reg().readIntEntry("SETTINGS", "width", 600);
    int windowHeight = getApp()->reg().readIntEntry("SETTINGS", "height", 400);
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("window-size")) {
        std::vector<std::string> windowSize = oc.getStringVector("window-size");
        if (windowSize.size() != 2
                || !TplCheck::_str2int(windowSize[0])
                || !TplCheck::_str2int(windowSize[1])) {
            WRITE_ERROR("option window-size requires INT,INT");
        } else {
            windowWidth = TplConvert::_str2int(windowSize[0]);
            windowHeight = TplConvert::_str2int(windowSize[1]);
        }
    }
    if (oc.isSet("window-size") || getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 0 || oc.isSet("window-pos")) {
        // when restoring previous pos, make sure the window fits fully onto the current screen
        int x = MAX2(0, MIN2(getApp()->reg().readIntEntry("SETTINGS", "x", 150), getApp()->getRootWindow()->getWidth() - windowWidth));
        int y = MAX2(0, MIN2(getApp()->reg().readIntEntry("SETTINGS", "y", 150), getApp()->getRootWindow()->getHeight() - windowHeight));
        if (oc.isSet("window-pos")) {
            std::vector<std::string> windowPos = oc.getStringVector("window-pos");
            if (windowPos.size() != 2
                    || !TplCheck::_str2int(windowPos[0])
                    || !TplCheck::_str2int(windowPos[1])
               ) {
                WRITE_ERROR("option window-pos requires INT,INT");
            } else {
                x = TplConvert::_str2int(windowPos[0]);
                y = TplConvert::_str2int(windowPos[1]);
            }
        }
        setX(x);
        setY(y);
        setWidth(windowWidth);
        setHeight(windowHeight);
    }
}

/****************************************************************************/

