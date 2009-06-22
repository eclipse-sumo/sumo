/****************************************************************************/
/// @file    GUIMainWindow.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
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
#ifndef GUIMainWindow_h
#define GUIMainWindow_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <vector>
#include <utils/foxtools/MFXMutex.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GUIMainWindow : public FXMainWindow {
public:
    GUIMainWindow(FXApp* a);
    virtual ~GUIMainWindow();
    /// Adds a further child window to the list
    void addChild(FXMDIChild *child, bool updateOnSimStep=true);
    void addChild(FXMainWindow *child, bool updateOnSimStep=true);

    /// removes the given child window from the list
    void removeChild(FXMDIChild *child);
    void removeChild(FXMainWindow  *child);

    void updateChildren();

    FXFont *getBoldFont();

    FXGLVisual *getGLVisual() const;

    virtual FXGLCanvas *getBuildGLCanvas() const = 0;

    virtual SUMOTime getCurrentSimTime() const = 0;

    virtual void setStatusBarText(const std::string &) { }

    FXLabel &getCartesianLabel();
    FXLabel &getGeoLabel();

protected:
    std::vector<FXMDIChild*> mySubWindows;
    std::vector<FXMainWindow*> myTrackerWindows;
    /// A lock to make the removal and addition of trackers secure
    MFXMutex myTrackerLock;

    /// Font used for popup-menu titles
    FXFont *myBoldFont;

    /// The multi view panel
    FXMDIClient *myMDIClient;

    /// The status bar
    FXStatusBar *myStatusbar;

    /// Labels for the current cartesian and geo-coordinate
    FXLabel *myCartesianCoordinate, *myGeoCoordinate;
    FXHorizontalFrame *myCartesianFrame, *myGeoFrame;


    /// The gl-visual used
    FXGLVisual *myGLVisual;

    FXDockSite *myTopDock, *myBottomDock, *myLeftDock, *myRightDock;

    bool myRunAtBegin;

protected:
    GUIMainWindow() { }

};


#endif

/****************************************************************************/

