/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GUIMainWindow.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <vector>
#include <string>
#include <map>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIEvent;
class GUIGlChildWindow;
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
class GUIMainWindow : public FXMainWindow {

public:
    /// @brief constructor
    GUIMainWindow(FXApp* a);

    /// @brief destructor
    virtual ~GUIMainWindow();

    /// @brief Adds a further child window to the list (GUIGlChildWindow)
    void addGLChild(GUIGlChildWindow* child);

    /// @brief Adds a further child window to the list (FXMainWindow)
    void addChild(FXMainWindow* child);

    /// @brief removes the given child window from the list (GUIGlChildWindow)
    void removeGLChild(GUIGlChildWindow* child);

    /// @brief removes the given child window from the list (FXMainWindow)
    void removeChild(FXMainWindow* child);

    /// @brief get top dock
    FXDockSite* getTopDock();

    /// @brief get view IDs
    std::vector<std::string> getViewIDs() const;

    /// @brief get specific view by ID
    GUIGlChildWindow* getViewByID(const std::string& id) const;

    /// @brief get views
    const std::vector<GUIGlChildWindow*>& getViews() const;

    /// @brief update childrens
    void updateChildren();

    /// @brief get bold front
    FXFont* getBoldFont();

    /// @brief get fallback front
    FXFont* getFallbackFont();

    /// @brief get GL Visual
    FXGLVisual* getGLVisual() const;

    /// @brief get build GL Canvas (must be implemented in all children)
    virtual FXGLCanvas* getBuildGLCanvas() const = 0;

    /// @brief get current sim time (must be implemented in all children)
    virtual SUMOTime getCurrentSimTime() const = 0;

    /// @brief get tracker interval (must be implemented in all children)
    virtual double getTrackerInterval() const = 0;

    /// @brief get status bar text (can be implemented in children)
    virtual void setStatusBarText(const std::string&) { }

    /// @brief get cartesian label
    FXLabel& getCartesianLabel();

    /// @brief get geo label
    FXLabel& getGeoLabel();

    /// @brief return whether the gui is in gaming mode
    bool isGaming() const;

    /// @brief return whether to list internal structures
    bool listInternal() const;

    /// @brief return whether to list parking vehicles
    bool listParking() const;

    /// @brief return whether to list teleporting vehicles
    bool listTeleporting() const;

    /// @brief get instance
    static GUIMainWindow* getInstance();

    /** @brief Returns the delay (should be overwritten by subclasses if applicable)
     * @return parsed delay in milliseconds
     */
    virtual double getDelay() const {
        return 0.;
    }

    /// @brief Sets the delay of the parent application
    virtual void setDelay(double) {}

    /// @brief Sets the breakpoints of the parent application
    virtual void setBreakpoints(const std::vector<SUMOTime>&) {}

    /** @brief Sends an event from the application thread to the GUI and waits until it is handled
     * @param event the event to send
     */
    virtual void sendBlockingEvent(GUIEvent* event) {
        UNUSED_PARAMETER(event);
    }

    /// @brief get the active view or 0
    GUISUMOAbstractView* getActiveView() const;

    /// @brief Toggle full screen mode
    virtual long onCmdFullScreen(FXObject*, FXSelector, void*) {
        return 1;
    }

    bool isFullScreen() {
        return myAmFullScreen;
    }

    /// @brief retrieve breakpoints if provided by the application
    virtual const std::vector<SUMOTime> retrieveBreakpoints() const {
        return std::vector<SUMOTime>();
    }

protected:
    /// @brief fox need this
    FOX_CONSTRUCTOR(GUIMainWindow)

    /// @brief whether to show the window in full screen mode
    bool myAmFullScreen;

    std::vector<GUIGlChildWindow*> myGLWindows;

    std::vector<FXMainWindow*> myTrackerWindows;

    /// A lock to make the removal and addition of trackers secure
    FXMutex myTrackerLock;

    /// Font used for popup-menu titles
    FXFont* myBoldFont;

    /// Fallback font for extended characters support
    FXFont* myFallbackFont;

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

    /// @brief perform initial window positioning and sizing according to user options / previous call
    void setWindowSizeAndPos();

    /// @brief record window position and size in registry
    void storeWindowSizeAndPos();
};
