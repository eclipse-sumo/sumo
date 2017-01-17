/****************************************************************************/
/// @file    GUIMainWindow.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
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
#include <map>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>
#include <utils/foxtools/MFXMutex.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIEvent;
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
class GUIMainWindow : public FXMainWindow {
public:
    GUIMainWindow(FXApp* a);
    virtual ~GUIMainWindow();
    /// Adds a further child window to the list
    void addChild(FXMDIChild* child, bool updateOnSimStep = true);
    void addChild(FXMainWindow* child, bool updateOnSimStep = true);

    /// removes the given child window from the list
    void removeChild(FXMDIChild* child);
    void removeChild(FXMainWindow*  child);

    std::vector<std::string> getViewIDs() const;
    FXMDIChild* getViewByID(const std::string& id) const;

    void updateChildren();

    FXFont* getBoldFont();

    FXGLVisual* getGLVisual() const;

    virtual FXGLCanvas* getBuildGLCanvas() const = 0;

    virtual SUMOTime getCurrentSimTime() const = 0;

    virtual void setStatusBarText(const std::string&) { }

    FXLabel& getCartesianLabel();
    FXLabel& getGeoLabel();

    /// @brief return whether the gui is in gaming mode
    bool isGaming() const {
        return myAmGaming;
    }

    /// @brief return whether to list internal structures
    bool listInternal() const {
        return myListInternal;
    }

    /// @brief return whether to list parking vehicles
    bool listParking() const {
        return myListParking;
    }

    /// @brief return whether to list teleporting vehicles
    bool listTeleporting() const {
        return myListTeleporting;
    }

    static GUIMainWindow* getInstance();

    /** @brief Returns the delay (should be overwritten by subclasses if applicable)
     * @return parsed delay in milliseconds
     */
    virtual SUMOReal getDelay() const {
        return 0;
    }

    /** @brief Sets the delay of the parent application
     */
    virtual void setDelay(SUMOReal) {}

    /** @brief Sends an event from the application thread to the GUI and waits until it is handled
     * @param event the event to send
     */
    virtual void sendBlockingEvent(GUIEvent* event) {
        UNUSED_PARAMETER(event);
    }

    /** @brief get the active view or 0 */
    GUISUMOAbstractView* getActiveView() const;

protected:
    std::vector<FXMDIChild*> mySubWindows;
    std::vector<FXMainWindow*> myTrackerWindows;
    /// A lock to make the removal and addition of trackers secure
    MFXMutex myTrackerLock;

    /// Font used for popup-menu titles
    FXFont* myBoldFont;

    /// The multi view panel
    FXMDIClient* myMDIClient;

    /// The status bar
    FXStatusBar* myStatusbar;

    /// Labels for the current cartesian and geo-coordinate
    FXLabel* myCartesianCoordinate, *myGeoCoordinate;
    FXHorizontalFrame* myCartesianFrame, *myGeoFrame;

    /// The gl-visual used
    FXGLVisual* myGLVisual;

    FXDockSite* myTopDock, *myBottomDock, *myLeftDock, *myRightDock;

    /// information whether the gui is currently in gaming mode
    bool myAmGaming;

    /// information whether the locator should list internal structures
    bool myListInternal;

    /// information whether the locator should list parking vehicles
    bool myListParking;

    /// information whether the locator should list teleporting vehicles
    bool myListTeleporting;

    /// the singleton window instance
    static GUIMainWindow* myInstance;

protected:
    GUIMainWindow() { }

    /// @brief perform initial window positioning and sizing according to user options / previous call
    void setWindowSizeAndPos();


};


#endif

/****************************************************************************/

