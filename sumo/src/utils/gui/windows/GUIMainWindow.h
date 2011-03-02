/****************************************************************************/
/// @file    GUIMainWindow.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#include <string>
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

    std::vector<std::string> getViewIDs() const throw();
    FXMDIChild *getViewByID(const std::string &id) const throw();

    void updateChildren();

    FXFont *getBoldFont();

    FXGLVisual *getGLVisual() const;

    virtual FXGLCanvas *getBuildGLCanvas() const = 0;

    virtual SUMOTime getCurrentSimTime() const = 0;

    virtual void setStatusBarText(const std::string &) { }

    FXLabel &getCartesianLabel();
    FXLabel &getGeoLabel();

    /** @brief Loads a selection from a given file
     * @param[in] file The file to load the selection from
     * @param[out] msg The error message
     * @return Whether the selection could be loaded (fills the error message if not)
     */
    virtual bool loadSelection(const std::string &file, std::string &msg) = 0;

    bool isGaming() const throw();

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

    /// information whether the gui is currently in gaming mode
    bool myAmGaming;

protected:
    GUIMainWindow() { }

};


#endif

/****************************************************************************/

