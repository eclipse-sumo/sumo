/****************************************************************************/
/// @file    GUIApplicationWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @date    Sept 2002
/// @version $Id$
///
// The main window of the SUMO-gui.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <string>
#include <sstream>
#include <algorithm>

#include <guisim/GUINet.h>

#include "GUISUMOViewParent.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"
#include "GUIEvent_SimulationLoaded.h"
#include "GUIEvent_SimulationEnded.h"

#include <utils/common/ToString.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>

#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include "GUIGlobals.h"
#include "dialogs/GUIDialog_AboutSUMO.h"
#include "dialogs/GUIDialog_AppSettings.h"
#include "dialogs/GUIDialog_Breakpoints.h"

#ifndef NO_TRACI
#include <traci-server/TraCIServer.h>
#include "TraCIServerAPI_GUI.h"
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GUIApplicationWindow) GUIApplicationWindowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_QUIT,        GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_QUIT,        GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,      GUIApplicationWindow::onCmdQuit),

    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,       GUIApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_NETWORK,      GUIApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,        GUIApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_COMMAND,  MID_RELOAD,            GUIApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE,             GUIApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITCHOSEN,        GUIApplicationWindow::onCmdEditChosen),
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_BREAKPOINTS,  GUIApplicationWindow::onCmdEditBreakpoints),

    FXMAPFUNC(SEL_COMMAND,  MID_APPSETTINGS,        GUIApplicationWindow::onCmdAppSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_GAMING,             GUIApplicationWindow::onCmdGaming),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTINTERNAL,       GUIApplicationWindow::onCmdListInternal),
    FXMAPFUNC(SEL_COMMAND,  MID_ABOUT,              GUIApplicationWindow::onCmdAbout),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_MICROVIEW,      GUIApplicationWindow::onCmdNewView),
#ifdef HAVE_OSG
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_OSGVIEW,        GUIApplicationWindow::onCmdNewOSG),
#endif
    FXMAPFUNC(SEL_COMMAND,  MID_START,              GUIApplicationWindow::onCmdStart),
    FXMAPFUNC(SEL_COMMAND,  MID_STOP,               GUIApplicationWindow::onCmdStop),
    FXMAPFUNC(SEL_COMMAND,  MID_STEP,               GUIApplicationWindow::onCmdStep),
    FXMAPFUNC(SEL_COMMAND,  MID_TIME_TOOGLE,        GUIApplicationWindow::onCmdTimeToggle),
    FXMAPFUNC(SEL_COMMAND,  MID_DELAY_TOOGLE,       GUIApplicationWindow::onCmdDelayToggle),
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW, GUIApplicationWindow::onCmdClearMsgWindow),

    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,       GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,      GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_RELOAD,            GUIApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,        GUIApplicationWindow::onUpdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_MICROVIEW,     GUIApplicationWindow::onUpdAddView),
#ifdef HAVE_OSG
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_OSGVIEW,        GUIApplicationWindow::onUpdAddView),
#endif
    FXMAPFUNC(SEL_UPDATE,   MID_START,             GUIApplicationWindow::onUpdStart),
    FXMAPFUNC(SEL_UPDATE,   MID_STOP,              GUIApplicationWindow::onUpdStop),
    FXMAPFUNC(SEL_UPDATE,   MID_STEP,              GUIApplicationWindow::onUpdStep),
    FXMAPFUNC(SEL_UPDATE,   MID_EDITCHOSEN,        GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_EDIT_BREAKPOINTS,  GUIApplicationWindow::onUpdNeedsSimulation),

    // forward requests to the active view
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,     GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEJUNCTION, GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEEDGE,     GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEVEHICLE,  GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATETLS,      GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEADD,      GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOI,      GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOLY,     GUIApplicationWindow::onUpdNeedsSimulation),

    FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0, GUIApplicationWindow::onClipboardRequest),

    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT, GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_RUNTHREAD_EVENT,  GUIApplicationWindow::onRunThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD, ID_LOADTHREAD_EVENT,       GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD, ID_RUNTHREAD_EVENT,        GUIApplicationWindow::onRunThreadEvent),
};

// Object implementation
FXIMPLEMENT(GUIApplicationWindow, FXMainWindow, GUIApplicationWindowMap, ARRAYNUMBER(GUIApplicationWindowMap))

// ===========================================================================
// member method definitions
// ===========================================================================
GUIApplicationWindow::GUIApplicationWindow(FXApp* a,
        const std::string& configPattern)
    : GUIMainWindow(a),
      myLoadThread(0), myRunThread(0),
      myAmLoading(false),
      myAlternateSimDelay(0),
      myRecentNets(a, "nets"), myConfigPattern(configPattern),
      hadDependentBuild(false),
      myShowTimeAsHMS(false) {
    GUIIconSubSys::init(a);
}


void
GUIApplicationWindow::dependentBuild(bool game) {
    // do this not twice
    if (hadDependentBuild) {
        return;
    }
    hadDependentBuild = true;

    setTarget(this);
    setSelector(MID_WINDOW);

    // build menu bar
    myMenuBarDrag = new FXToolBarShell(this, FRAME_NORMAL);
    myMenuBar = new FXMenuBar(myTopDock, myMenuBarDrag,
                              LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP,
                      TOOLBARGRIP_DOUBLE);
    buildToolBars();
    // build the thread - io
    myLoadThreadEvent.setTarget(this),
                                myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    myRunThreadEvent.setTarget(this),
                               myRunThreadEvent.setSelector(ID_RUNTHREAD_EVENT);

    // build the status bar
    myStatusbar = new FXStatusBar(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | FRAME_RAISED);
    {
        myGeoFrame =
            new FXHorizontalFrame(myStatusbar, LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y | LAYOUT_RIGHT | FRAME_SUNKEN,
                                  0, 0, 20, 0, 0, 0, 0, 0, 0, 0);
        myGeoCoordinate = new FXLabel(myGeoFrame, "N/A", 0, LAYOUT_CENTER_Y);
        myCartesianFrame =
            new FXHorizontalFrame(myStatusbar, LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y | LAYOUT_RIGHT | FRAME_SUNKEN,
                                  0, 0, 20, 0, 0, 0, 0, 0, 0, 0);
        myCartesianCoordinate = new FXLabel(myCartesianFrame, "N/A", 0, LAYOUT_CENTER_Y);
    }

    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this,
                                    SPLITTER_REVERSED | SPLITTER_VERTICAL | LAYOUT_FILL_X | LAYOUT_FILL_Y | SPLITTER_TRACKING | FRAME_RAISED | FRAME_THICK);
    myMDIClient = new FXMDIClient(myMainSplitter,
                                  LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK);
    myMDIMenu = new FXMDIMenu(this, myMDIClient);
    new FXMDIWindowButton(myMenuBar, myMDIMenu, myMDIClient,
                          FXMDIClient::ID_MDI_MENUWINDOW, LAYOUT_LEFT);
    new FXMDIDeleteButton(myMenuBar, myMDIClient,
                          FXMDIClient::ID_MDI_MENUCLOSE, FRAME_RAISED | LAYOUT_RIGHT);
    new FXMDIRestoreButton(myMenuBar, myMDIClient,
                           FXMDIClient::ID_MDI_MENURESTORE, FRAME_RAISED | LAYOUT_RIGHT);
    new FXMDIMinimizeButton(myMenuBar, myMDIClient,
                            FXMDIClient::ID_MDI_MENUMINIMIZE, FRAME_RAISED | LAYOUT_RIGHT);

    // build the message window
    myMessageWindow = new GUIMessageWindow(myMainSplitter);
    // fill menu and tool bar
    fillMenuBar();
    if (game) {
        onCmdGaming(0, 0, 0);
        myMenuBar->hide();
        myToolBar1->hide();
        myToolBar2->hide();
        myToolBar4->hide();
        myToolBar5->hide();
        myMessageWindow->hide();
    }
    // build additional threads
    myLoadThread = new GUILoadThread(getApp(), this, myEvents, myLoadThreadEvent);
    myRunThread = new GUIRunThread(getApp(), this, *mySimDelayTarget, myEvents,
                                   myRunThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");
    // set the caption
    setTitle(MFXUtils::getTitleText(("SUMO " + std::string(VERSION_STRING)).c_str()));

    // start the simulation-thread (it will loop until the application ends deciding by itself whether to perform a step or not)
    myRunThread->start();
    setIcon(GUIIconSubSys::getIcon(ICON_APP));
}


void
GUIApplicationWindow::create() {
    if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 0) {
        setX(getApp()->reg().readIntEntry("SETTINGS", "x", 150));
        setY(getApp()->reg().readIntEntry("SETTINGS", "y", 150));
        setWidth(getApp()->reg().readIntEntry("SETTINGS", "width", 600));
        setHeight(getApp()->reg().readIntEntry("SETTINGS", "height", 400));
    }
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS", "basedir", "");
    FXMainWindow::create();
    myMenuBarDrag->create();
    myToolBarDrag1->create();
    myToolBarDrag2->create();
    myFileMenu->create();
    myEditMenu->create();
    mySettingsMenu->create();
    myLocatorMenu->create();
    myControlMenu->create();
    myWindowsMenu->create();
    myHelpMenu->create();

    FXint width = getApp()->getNormalFont()->getTextWidth("8", 1) * 24;
    myCartesianFrame->setWidth(width);
    myGeoFrame->setWidth(width);

    show(PLACEMENT_SCREEN);
    if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 1) {
        maximize();
    }
    myShowTimeAsHMS = (getApp()->reg().readIntEntry("gui", "timeasHMS", 0) == 1);
    myAlternateSimDelay = getApp()->reg().readIntEntry("gui", "alternateSimDelay", 100);
}


GUIApplicationWindow::~GUIApplicationWindow() {
    myRunThread->prepareDestruction();
    myRunThread->join();
    closeAllWindows();
    //
    GUIIconSubSys::close();
    delete myGLVisual;
    // delete some non-parented windows
    delete myToolBarDrag1;
    //
    delete myRunThread;
    delete myFileMenu;
    delete myEditMenu;
    delete mySettingsMenu;
    delete myLocatorMenu;
    delete myControlMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

    delete myLoadThread;

    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = static_cast<GUIEvent*>(myEvents.top());
        myEvents.pop();
        delete e;
    }
}


void
GUIApplicationWindow::detach() {
    FXMainWindow::detach();
    myMenuBarDrag->detach();
    myToolBarDrag1->detach();
}


void
GUIApplicationWindow::fillMenuBar() {
    // build file menu
    myFileMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&File", NULL, myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Open Simulation...\tCtl-O\tOpen a simulation (Configuration file).",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG);
    new FXMenuCommand(myFileMenu,
                      "Open &Network...\tCtl-N\tOpen a network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_OPEN_NETWORK);
    new FXMenuCommand(myFileMenu,
                      "&Reload\tCtl-R\tReloads the simulation / the network.",
                      GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Close\tCtl-W\tClose the simulation.",
                      GUIIconSubSys::getIcon(ICON_CLOSE), this, MID_CLOSE);
    // Recent files
    FXMenuSeparator* sep1 = new FXMenuSeparator(myFileMenu);
    sep1->setTarget(&myRecentConfigs);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentConfigs, FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(myFileMenu, "C&lear Recent Configurations", NULL, &myRecentConfigs, FXRecentFiles::ID_CLEAR);
    myRecentConfigs.setTarget(this);
    myRecentConfigs.setSelector(MID_RECENTFILE);
    FXMenuSeparator* sep2 = new FXMenuSeparator(myFileMenu);
    sep2->setTarget(&myRecentNets);
    sep2->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myFileMenu, "", 0, &myRecentNets, FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(myFileMenu, "Cl&ear Recent Networks", NULL, &myRecentNets, FXRecentFiles::ID_CLEAR);
    myRecentNets.setTarget(this);
    myRecentNets.setSelector(MID_RECENTFILE);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Quit\tCtl-Q\tQuit the Application.",
                      0, this, MID_QUIT, 0);

    // build edit menu
    myEditMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Edit", NULL, myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Selected...\tCtl-E\tOpens a Dialog for editing the List of Selected Items.",
                      GUIIconSubSys::getIcon(ICON_FLAG), this, MID_EDITCHOSEN);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Breakpoints...\tCtl-B\tOpens a Dialog for editing breakpoints.",
                      0, this, MID_EDIT_BREAKPOINTS);

    // build settings menu
    mySettingsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Settings", NULL, mySettingsMenu);
    new FXMenuCommand(mySettingsMenu,
                      "Application Settings...\t\tOpen a Dialog for Application Settings editing.",
                      NULL, this, MID_APPSETTINGS);
    new FXMenuCheck(mySettingsMenu,
                    "Gaming Mode\t\tToggle gaming mode on/off.",
                    this, MID_GAMING);
    // build Locate menu
    myLocatorMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Locate", NULL, myLocatorMenu);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Junctions\t\tOpen a Dialog for Locating a Junction.",
                      NULL, this, MID_LOCATEJUNCTION);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Edges\t\tOpen a Dialog for Locating an Edge.",
                      NULL, this, MID_LOCATEEDGE);
    if (!MSGlobals::gUseMesoSim) { // there are no gui-vehicles in mesosim
        new FXMenuCommand(myLocatorMenu,
                "Locate &Vehicles\t\tOpen a Dialog for Locating a Vehicle.",
                NULL, this, MID_LOCATEVEHICLE);
    }
    new FXMenuCommand(myLocatorMenu,
                      "Locate &TLS\t\tOpen a Dialog for Locating a Traffic Light.",
                      NULL, this, MID_LOCATETLS);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Additional\t\tOpen a Dialog for Locating an Additional Structure.",
                      NULL, this, MID_LOCATEADD);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &POI\t\tOpen a Dialog for Locating a Point of Intereset.",
                      NULL, this, MID_LOCATEPOI);
    new FXMenuCommand(myLocatorMenu,
                      "Locate P&olygon\t\tOpen a Dialog for Locating a Polygon.",
                      NULL, this, MID_LOCATETLS);
    new FXMenuSeparator(myLocatorMenu);
    new FXMenuCheck(myLocatorMenu,
                    "Show Internal Structures\t\tShow internal junctions and streets in locator Dialog.",
                    this, MID_LISTINTERNAL);
    // build control menu
    myControlMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "Simulation", NULL, myControlMenu);
    new FXMenuCommand(myControlMenu,
                      "Run\tCtl-A\tStart running the simulation.",
                      NULL, this, MID_START);
    new FXMenuCommand(myControlMenu,
                      "Stop\tCtl-S\tStop running the simulation.",
                      NULL, this, MID_STOP);
    new FXMenuCommand(myControlMenu,
                      "Step\tCtl-D\tPerform one simulation step.",
                      NULL, this, MID_STEP);

    // build windows menu
    myWindowsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Windows", NULL, myWindowsMenu);
    new FXMenuCheck(myWindowsMenu,
                    "Show Status Line\t\tToggle the Status Bar on/off.",
                    myStatusbar, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Message Window\t\tToggle the Message Window on/off.",
                    myMessageWindow, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Simulation Time\t\tToggle the Simulation Time on/off.",
                    myToolBar3, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Simulation Delay\t\tToggle the Simulation Delay Entry on/off.",
                    myToolBar4, FXWindow::ID_TOGGLESHOWN);
    addToWindowsMenu(myWindowsMenu);
    new FXMenuSeparator(myWindowsMenu);
    new FXMenuCommand(myWindowsMenu, "Tile &Horizontally",
                      GUIIconSubSys::getIcon(ICON_WINDOWS_TILE_HORI),
                      myMDIClient, FXMDIClient::ID_MDI_TILEHORIZONTAL);
    new FXMenuCommand(myWindowsMenu, "Tile &Vertically",
                      GUIIconSubSys::getIcon(ICON_WINDOWS_TILE_VERT),
                      myMDIClient, FXMDIClient::ID_MDI_TILEVERTICAL);
    new FXMenuCommand(myWindowsMenu, "Cascade",
                      GUIIconSubSys::getIcon(ICON_WINDOWS_CASCADE),
                      myMDIClient, FXMDIClient::ID_MDI_CASCADE);
    new FXMenuCommand(myWindowsMenu, "&Close", NULL,
                      myMDIClient, FXMDIClient::ID_MDI_CLOSE);
    sep1 = new FXMenuSeparator(myWindowsMenu);
    sep1->setTarget(myMDIClient);
    sep1->setSelector(FXMDIClient::ID_MDI_ANY);
    new FXMenuCommand(myWindowsMenu, "", 0, myMDIClient, FXMDIClient::ID_MDI_1);
    new FXMenuCommand(myWindowsMenu, "", 0, myMDIClient, FXMDIClient::ID_MDI_2);
    new FXMenuCommand(myWindowsMenu, "", 0, myMDIClient, FXMDIClient::ID_MDI_3);
    new FXMenuCommand(myWindowsMenu, "", 0, myMDIClient, FXMDIClient::ID_MDI_4);
    new FXMenuCommand(myWindowsMenu, "&Others...", 0, myMDIClient, FXMDIClient::ID_MDI_OVER_5);
    new FXMenuSeparator(myWindowsMenu);
    new FXMenuCommand(myWindowsMenu,
                      "Clear Message Window\t\tClear the message window.",
                      0, this, MID_CLEARMESSAGEWINDOW);

    // build help menu
    myHelpMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Help", NULL, myHelpMenu);
    new FXMenuCommand(myHelpMenu, "&About", GUIIconSubSys::getIcon(ICON_APP),
                      this, MID_ABOUT);
}


void
GUIApplicationWindow::buildToolBars() {
    // build tool bars
    {
        // file and simulation tool bar
        myToolBarDrag1 = new FXToolBarShell(this, FRAME_NORMAL);
        myToolBar1 = new FXToolBar(myTopDock, myToolBarDrag1,
                                   LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | FRAME_RAISED);
        new FXToolBarGrip(myToolBar1, myToolBar1, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        // build file tools
        new FXButton(myToolBar1, "\t\tOpen a simulation (Configuration file).",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(myToolBar1, "\t\tOpen a network.",
                     GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_OPEN_NETWORK,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(myToolBar1, "\t\tReloads the simulation / the network.",
                     GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    }
    {
        // build simulation tools
        myToolBarDrag2 = new FXToolBarShell(this, FRAME_NORMAL);
        myToolBar2 = new FXToolBar(myTopDock, myToolBarDrag2,
                                   LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED);
        new FXToolBarGrip(myToolBar2, myToolBar2, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXButton(myToolBar2, "\t\tStart the loaded simulation.",
                     GUIIconSubSys::getIcon(ICON_START), this, MID_START,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(myToolBar2, "\t\tStop the running simulation.",
                     GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(myToolBar2, "\t\tPerform a single simulation step.",
                     GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
    }
    {
        // Simulation Step Display
        myToolBarDrag3 = new FXToolBarShell(this, FRAME_NORMAL);
        myToolBar3 = new FXToolBar(myTopDock, myToolBarDrag3,
                                   LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED);
        new FXToolBarGrip(myToolBar3, myToolBar3, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXButton(myToolBar3, "Time:\t\tToggle between seconds and hour:minute:seconds display", 0, this, MID_TIME_TOOGLE,
                     BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        myLCDLabel = new FXEX::FXLCDLabel(myToolBar3, 13, 0, 0, JUSTIFY_RIGHT);
        myLCDLabel->setHorizontal(2);
        myLCDLabel->setVertical(6);
        myLCDLabel->setThickness(2);
        myLCDLabel->setGroove(2);
        myLCDLabel->setText("-------------");
    }
    {
        // Simulation Delay
        myToolBarDrag4 = new FXToolBarShell(this, FRAME_NORMAL);
        myToolBar4 = new FXToolBar(myTopDock, myToolBarDrag4,
                                   LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED | LAYOUT_FILL_Y);
        new FXToolBarGrip(myToolBar4, myToolBar4, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXButton(myToolBar4, "Delay (ms):\t\tToggle between alternative delay values", 0, this, MID_DELAY_TOOGLE,
                     BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        mySimDelayTarget =
            new FXRealSpinDial(myToolBar4, 7, 0, MID_SIMDELAY,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_Y);
        mySimDelayTarget->setNumberFormat(0);
        mySimDelayTarget->setIncrements(1, 10, 10);
        mySimDelayTarget->setRange(0, 1000);
        mySimDelayTarget->setValue(0);
    }
    {
        // Views
        myToolBarDrag5 = new FXToolBarShell(this, FRAME_NORMAL);
        myToolBar5 = new FXToolBar(myTopDock, myToolBarDrag5,
                                   LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED);
        new FXToolBarGrip(myToolBar5, myToolBar5, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        // build view tools
        new FXButton(myToolBar5, "\t\tOpen a new microscopic view.",
                     GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_MICROVIEW,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
#ifdef HAVE_OSG
        new FXButton(myToolBar5, "\t\tOpen a new 3D view.",
                     GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_OSGVIEW,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
#endif
    }
}


long
GUIApplicationWindow::onCmdQuit(FXObject*, FXSelector, void*) {
    getApp()->reg().writeIntEntry("SETTINGS", "x", getX());
    getApp()->reg().writeIntEntry("SETTINGS", "y", getY());
    getApp()->reg().writeIntEntry("SETTINGS", "width", getWidth());
    getApp()->reg().writeIntEntry("SETTINGS", "height", getHeight());
    getApp()->reg().writeStringEntry("SETTINGS", "basedir", gCurrentFolder.text());
    getApp()->reg().writeIntEntry("SETTINGS", "maximized", isMaximized() ? 1 : 0);
    getApp()->reg().writeIntEntry("gui", "timeasHMS", myShowTimeAsHMS ? 1 : 0);
    getApp()->reg().writeIntEntry("gui", "alternateSimDelay", myAlternateSimDelay);
    getApp()->exit(0);
    return 1;
}


long
GUIApplicationWindow::onCmdEditChosen(FXObject*, FXSelector, void*) {
    GUIDialog_GLChosenEditor* chooser =
        new GUIDialog_GLChosenEditor(this, &gSelected);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdEditBreakpoints(FXObject*, FXSelector, void*) {
    GUIDialog_Breakpoints* chooser = new GUIDialog_Breakpoints(this);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdOpenConfiguration(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open Simulation Configuration");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(myConfigPattern.c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        load(file, false);
        myRecentConfigs.appendFile(file.c_str());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open Network");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("SUMO nets (*.net.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        load(file, true);
        myRecentNets.appendFile(file.c_str());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdReload(FXObject*, FXSelector, void*) {
    load("", false, true);
    return 1;
}


long
GUIApplicationWindow::onCmdOpenRecent(FXObject* sender, FXSelector, void* data) {
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    }
    std::string file((const char*)data);
    load(file, sender == &myRecentNets);
    return 1;
}


long
GUIApplicationWindow::onCmdClose(FXObject*, FXSelector, void*) {
    closeAllWindows();
    return 1;
}


long
GUIApplicationWindow::onUpdOpen(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myAmLoading ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdReload(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myAmLoading || !myRunThread->simulationAvailable()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdOpenRecent(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myAmLoading ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdAddView(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myAmLoading || !myRunThread->simulationAvailable()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdStart(FXObject*, FXSelector, void*) {
    // check whether a net was loaded successfully
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 1;
    }
    // check whether it was started before and paused;
    if (!myWasStarted) {
        myRunThread->begin();
        myWasStarted = true;
    }
    myRunThread->resume();
    return 1;
}


long
GUIApplicationWindow::onCmdStop(FXObject*, FXSelector, void*) {
    myRunThread->stop();
    return 1;
}


long
GUIApplicationWindow::onCmdStep(FXObject*, FXSelector, void*) {
    // check whether a net was loaded successfully
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 1;
    }
    // check whether it was started before and paused;
    if (!myWasStarted) {
        myRunThread->begin();
        myWasStarted = true;
    }
    myRunThread->singleStep();
    return 1;
}


long
GUIApplicationWindow::onCmdTimeToggle(FXObject*, FXSelector, void*) {
    myShowTimeAsHMS = !myShowTimeAsHMS;
    if (myRunThread->simulationAvailable()) {
        updateTimeLCD(myRunThread->getNet().getCurrentTimeStep());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdDelayToggle(FXObject*, FXSelector, void*) {
    const SUMOTime tmp = myAlternateSimDelay;
    myAlternateSimDelay = mySimDelayTarget->getValue();
    mySimDelayTarget->setValue(tmp);
    return 1;
}


long
GUIApplicationWindow::onCmdClearMsgWindow(FXObject*, FXSelector, void*) {
    myMessageWindow->clear();
    return 1;
}


long
GUIApplicationWindow::onUpdStart(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStartable() || myAmLoading
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStop(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStopable() || myAmLoading
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStep(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStepable() || myAmLoading
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdNeedsSimulation(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   !myRunThread->simulationAvailable() || myAmLoading
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdLocate(FXObject*, FXSelector sel, void*) {
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild()); 
        if (w != 0) {
            w->onCmdLocate(0, sel, 0);
        }
    }
    return 1;
}

long
GUIApplicationWindow::onCmdAppSettings(FXObject*, FXSelector, void*) {
    GUIDialog_AppSettings* d = new GUIDialog_AppSettings(this);
    d->create();
    d->show(PLACEMENT_OWNER);
    return 1;
}


long
GUIApplicationWindow::onCmdGaming(FXObject*, FXSelector, void*) {
    myAmGaming = !myAmGaming;
    if (myAmGaming) {
        mySimDelayTarget->setValue(1000);
    }
    return 1;
}


long
GUIApplicationWindow::onCmdListInternal(FXObject*, FXSelector, void*) {
    myListInternal = !myListInternal;
    return 1;
}


long
GUIApplicationWindow::onCmdNewView(FXObject*, FXSelector, void*) {
    openNewView(GUISUMOViewParent::VIEW_2D_OPENGL);
    return 1;
}


#ifdef HAVE_OSG
long
GUIApplicationWindow::onCmdNewOSG(FXObject*, FXSelector, void*) {
    openNewView(GUISUMOViewParent::VIEW_3D_OSG);
    return 1;
}
#endif


long
GUIApplicationWindow::onCmdAbout(FXObject*, FXSelector, void*) {
    GUIDialog_AboutSUMO* about =
        new GUIDialog_AboutSUMO(this, "About SUMO", 0, 0);
    about->create();
    about->show(PLACEMENT_OWNER);
    return 1;
}


long GUIApplicationWindow::onClipboardRequest(FXObject* /* sender */, FXSelector /* sel */, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXString string = GUIUserIO::clipped.c_str();
    setDNDData(FROM_CLIPBOARD, event->target, string);
    return 1;
}


long
GUIApplicationWindow::onLoadThreadEvent(FXObject*, FXSelector, void*) {
    eventOccured();
    return 1;
}


long
GUIApplicationWindow::onRunThreadEvent(FXObject*, FXSelector, void*) {
    eventOccured();
    return 1;
}


void
GUIApplicationWindow::eventOccured() {
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = static_cast<GUIEvent*>(myEvents.top());
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
            case EVENT_SIMULATION_LOADED:
                handleEvent_SimulationLoaded(e);
                break;
            case EVENT_SIMULATION_STEP:
                if (myRunThread->simulationAvailable()) { // avoid race-condition related crash if reload was pressed
                    handleEvent_SimulationStep(e);
                }
                break;
            case EVENT_MESSAGE_OCCURED:
            case EVENT_WARNING_OCCURED:
            case EVENT_ERROR_OCCURED:
                handleEvent_Message(e);
                break;
            case EVENT_SIMULATION_ENDED:
                handleEvent_SimulationEnded(e);
                break;
            default:
                break;
        }
        delete e;
    }
    myToolBar2->forceRefresh();
    myToolBar3->forceRefresh();
}


void
GUIApplicationWindow::handleEvent_SimulationLoaded(GUIEvent* e) {
    myAmLoading = false;
    GUIEvent_SimulationLoaded* ec = static_cast<GUIEvent_SimulationLoaded*>(e);
    if (ec->myNet != 0) {
#ifndef NO_TRACI
        std::map<int, TraCIServer::CmdExecutor> execs;
        execs[CMD_GET_GUI_VARIABLE] = &TraCIServerAPI_GUI::processGet;
        execs[CMD_SET_GUI_VARIABLE] = &TraCIServerAPI_GUI::processSet;
        try {
            TraCIServer::openSocket(execs);
        } catch (ProcessError& e) {
            myMessageWindow->appendText(EVENT_ERROR_OCCURED, e.what());
            WRITE_ERROR(e.what());
            delete ec->myNet;
            ec->myNet = 0;
        }
#endif
    }

    // check whether the loading was successfull
    if (ec->myNet == 0) {
        // report failure
        setStatusBarText("Loading of '" + ec->myFile + "' failed!");
        if (GUIGlobals::gQuitOnEnd) {
            closeAllWindows();
            getApp()->exit(1);
        }
    } else {
        // initialise simulation thread
        if(!myRunThread->init(ec->myNet, ec->myBegin, ec->myEnd)) {
            if (GUIGlobals::gQuitOnEnd) {
                closeAllWindows();
                getApp()->exit(1);
            }
        } else {
            // report success
            setStatusBarText("'" + ec->myFile + "' loaded.");
            myWasStarted = false;
            // initialise views
            myViewNumber = 0;
            const GUISUMOViewParent::ViewType defaultType = ec->myOsgView ? GUISUMOViewParent::VIEW_3D_OSG : GUISUMOViewParent::VIEW_2D_OPENGL;
            if (ec->mySettingsFiles.size() > 0) {
                // open a view for each file and apply settings
                for (std::vector<std::string>::const_iterator it = ec->mySettingsFiles.begin(); it != ec->mySettingsFiles.end(); ++it) {
                    GUISettingsHandler settings(*it);
                    GUISUMOViewParent::ViewType vt = defaultType;
                    if (settings.getViewType() == "osg" || settings.getViewType() == "3d") {
                        vt = GUISUMOViewParent::VIEW_3D_OSG;
                    }
                    if (settings.getViewType() == "opengl" || settings.getViewType() == "2d") {
                        vt = GUISUMOViewParent::VIEW_2D_OPENGL;
                    }
                    GUISUMOAbstractView* view = openNewView(vt);
                    if (view == 0) {
                        break;
                    }
                    std::string settingsName = settings.addSettings(view);
                    view->addDecals(settings.getDecals());
                    settings.setViewport(view);
                    settings.setSnapshots(view);
                    if (settings.getDelay() > 0) {
                        mySimDelayTarget->setValue(settings.getDelay());
                    }
                    if (settings.getBreakpoints().size() > 0) {
                        GUIGlobals::gBreakpoints = settings.getBreakpoints();
                    }
                }
            } else {
                openNewView(defaultType);
            }

            if (isGaming()) {
                setTitle("SUMO Traffic Light Game");
            } else {
                // set simulation name on the caption
                std::string caption = "SUMO " + std::string(VERSION_STRING);
                setTitle(MFXUtils::getTitleText(caption.c_str(), ec->myFile.c_str()));
            }
            // set simulation step begin information
            updateTimeLCD(ec->myNet->getCurrentTimeStep());
        }
    }
    getApp()->endWaitCursor();
    // start if wished
    if (GUIGlobals::gRunAfterLoad && ec->myNet != 0 && myRunThread->simulationIsStartable()) {
        onCmdStart(0, 0, 0);
    }
    update();
}


void
GUIApplicationWindow::handleEvent_SimulationStep(GUIEvent*) {
    updateChildren();
    updateTimeLCD(myRunThread->getNet().getCurrentTimeStep());
    update();
}


void
GUIApplicationWindow::handleEvent_Message(GUIEvent* e) {
    GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendText(ec->getOwnType(), ec->getMsg());
}


void
GUIApplicationWindow::handleEvent_SimulationEnded(GUIEvent* e) {
    GUIEvent_SimulationEnded* ec = static_cast<GUIEvent_SimulationEnded*>(e);
    onCmdStop(0, 0, 0);
    if (GUIGlobals::gQuitOnEnd) {
        closeAllWindows();
        getApp()->exit(ec->getReason() == MSNet::SIMSTATE_ERROR_IN_SIM);
    } else {
        // build the text
        const std::string text = "Simulation ended at time: " + time2string(ec->getTimeStep()) +
                                 ".\nReason: " + MSNet::getStateMessage(ec->getReason());
        FXMessageBox::warning(this, MBOX_OK, "Simulation ended", "%s", text.c_str());
    }
}



void
GUIApplicationWindow::load(const std::string& file, bool isNet, bool isReload) {
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    if (isReload) {
        myLoadThread->start();
        setStatusBarText("Reloading.");
    } else {
        gSchemeStorage.saveViewport(0, 0, -1); // recenter view
        myLoadThread->load(file, isNet);
        setStatusBarText("Loading '" + file + "'.");
    }
    update();
}


GUISUMOAbstractView*
GUIApplicationWindow::openNewView(GUISUMOViewParent::ViewType vt) {
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 0;
    }
    std::string caption = "View #" + toString(myViewNumber++);
    FXuint opts = MDI_TRACKING;
    GUISUMOViewParent* w = new GUISUMOViewParent(myMDIClient, myMDIMenu, FXString(caption.c_str()),
            this, GUIIconSubSys::getIcon(ICON_APP), opts, 10, 10, 300, 200);
    GUISUMOAbstractView* v = w->init(getBuildGLCanvas(), myRunThread->getNet(), vt);
    w->create();
    if (myMDIClient->numChildren() == 1) {
        w->maximize();
    } else {
        myMDIClient->vertical(true);
    }
    myMDIClient->setActiveChild(w);
    return v;
}


FXGLCanvas*
GUIApplicationWindow::getBuildGLCanvas() const {
    if (myMDIClient->numChildren() == 0) {
        return 0;
    }
    GUISUMOViewParent* share_tmp1 =
        static_cast<GUISUMOViewParent*>(myMDIClient->childAtIndex(0));
    return share_tmp1->getBuildGLCanvas();
}


void
GUIApplicationWindow::closeAllWindows() {
    myTrackerLock.lock();
    myLCDLabel->setText("-------------");
    // remove trackers and other external windows
    size_t i;
    for (i = 0; i < mySubWindows.size(); ++i) {
        mySubWindows[i]->destroy();
    }
    for (i = 0; i < myTrackerWindows.size(); ++i) {
        myTrackerWindows[i]->destroy();
    }
    // delete the simulation
    myRunThread->deleteSim();
    // reset the caption
    setTitle(MFXUtils::getTitleText(("SUMO " + std::string(VERSION_STRING)).c_str()));
    // delete other children
    while (myTrackerWindows.size() != 0) {
        delete myTrackerWindows[0];
    }
    while (mySubWindows.size() != 0) {
        delete mySubWindows[0];
    }
    mySubWindows.clear();
    // clear selected items
    gSelected.clear();
    // add a separator to the log
    myMessageWindow->addSeparator();
    myTrackerLock.unlock();
    // remove coordinate information
    myGeoCoordinate->setText("N/A");
    myCartesianCoordinate->setText("N/A");
    //
    GUITexturesHelper::clearTextures();
    update();
}


FXCursor*
GUIApplicationWindow::getDefaultCursor() {
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


SUMOTime
GUIApplicationWindow::getCurrentSimTime() const {
    return myRunThread->getNet().getCurrentTimeStep();
}


void
GUIApplicationWindow::loadOnStartup() {
    load("", false);
}


void
GUIApplicationWindow::setStatusBarText(const std::string& text) {
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
}


void
GUIApplicationWindow::updateTimeLCD(const SUMOTime time) {
    SUMOReal fracSeconds = STEPS2TIME(time);
    const bool hideFraction = myAmGaming || fmod(TS, 1.) == 0.;
    const int BuffSize = 100;
    char buffer[BuffSize];
    if (myShowTimeAsHMS) {
        const int hours = (int)fracSeconds / 3600;
        const int minutes = ((int)fracSeconds % 3600) / 60;
        fracSeconds = fracSeconds - 3600 * hours - 60 * minutes;
        const std::string format = (hideFraction ?
                                    "%02d-%02d-%02.0f" : "%02d-%02d-%06.3f");
        snprintf(buffer, BuffSize, format.c_str(), hours, minutes, fracSeconds);
    } else {
        const std::string format = (hideFraction ?
                                    "%13.0f" : "%13.3f");
        snprintf(buffer, BuffSize, format.c_str(), fracSeconds);
    }
    myLCDLabel->setText(buffer);
}

/****************************************************************************/

