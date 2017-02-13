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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <string>
#include <sstream>
#include <algorithm>

#include <guisim/GUINet.h>
#include <guisim/GUILane.h>
#include <netload/NLHandler.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSTransportableControl.h>

#include "GUISUMOViewParent.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"
#include "GUIEvent_SimulationLoaded.h"
#include "GUIEvent_SimulationEnded.h"
#include "GUIEvent_Screenshot.h"

#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/FXLinkLabel.h>

#include <utils/xml/XMLSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
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
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/options/OptionsCont.h>
#include "GUIGlobals.h"
#include "dialogs/GUIDialog_AboutSUMO.h"
#include "dialogs/GUIDialog_AppSettings.h"
#include "dialogs/GUIDialog_Breakpoints.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

//#define HAVE_DANGEROUS_SOUNDS

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GUIApplicationWindow) GUIApplicationWindowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_QUIT,        GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_QUIT,        GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,      GUIApplicationWindow::onCmdQuit),

    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,       GUIApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_NETWORK,      GUIApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_SHAPES,       GUIApplicationWindow::onCmdOpenShapes),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,        GUIApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_COMMAND,  MID_RELOAD,            GUIApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE,             GUIApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITCHOSEN,        GUIApplicationWindow::onCmdEditChosen),
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_BREAKPOINTS,  GUIApplicationWindow::onCmdEditBreakpoints),
    FXMAPFUNC(SEL_COMMAND,  MID_NETEDIT,           GUIApplicationWindow::onCmdNetedit),

    FXMAPFUNC(SEL_COMMAND,  MID_APPSETTINGS,        GUIApplicationWindow::onCmdAppSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_GAMING,             GUIApplicationWindow::onCmdGaming),
    FXMAPFUNC(SEL_COMMAND,  MID_FULLSCREEN,         GUIApplicationWindow::onCmdFullScreen),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTINTERNAL,       GUIApplicationWindow::onCmdListInternal),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTPARKING,        GUIApplicationWindow::onCmdListParking),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTTELEPORTING,    GUIApplicationWindow::onCmdListTeleporting),
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

    FXMAPFUNC(SEL_COMMAND,  MID_SHOWNETSTATS,       GUIApplicationWindow::onCmdShowStats),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWVEHSTATS,       GUIApplicationWindow::onCmdShowStats),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPERSONSTATS,    GUIApplicationWindow::onCmdShowStats),

    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,       GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,      GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,      GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_SHAPES,       GUIApplicationWindow::onUpdReload),
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
    FXMAPFUNC(SEL_UPDATE,   MID_NETEDIT,           GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,              GUIApplicationWindow::onCmdHelp),

    // forward requests to the active view
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPERSON,   GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,     GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEJUNCTION, GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEEDGE,     GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEVEHICLE,  GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPERSON,   GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATETLS,      GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEADD,      GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOI,      GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOLY,     GUIApplicationWindow::onUpdNeedsSimulation),
    FXMAPFUNC(SEL_KEYPRESS,              0,     GUIApplicationWindow::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,            0,     GUIApplicationWindow::onKeyRelease),

    FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0, GUIApplicationWindow::onClipboardRequest),

    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT, GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_RUNTHREAD_EVENT,  GUIApplicationWindow::onRunThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD, ID_LOADTHREAD_EVENT,       GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD, ID_RUNTHREAD_EVENT,        GUIApplicationWindow::onRunThreadEvent),
};

// Object implementation
FXIMPLEMENT(GUIApplicationWindow, FXMainWindow, GUIApplicationWindowMap, ARRAYNUMBER(GUIApplicationWindowMap))

// ===========================================================================
// static members
// ===========================================================================
MTRand GUIApplicationWindow::myGamingRNG;

// ===========================================================================
// member method definitions
// ===========================================================================
GUIApplicationWindow::GUIApplicationWindow(FXApp* a, const std::string& configPattern) :
    GUIMainWindow(a),
    myLoadThread(0), myRunThread(0),
    myAmLoading(false),
    myAlternateSimDelay(0),
    myRecentNets(a, "nets"),
    myConfigPattern(configPattern),
    hadDependentBuild(false),
    myShowTimeAsHMS(false),
    myAmFullScreen(false),
    myHaveNotifiedAboutSimEnd(false),
    // game specific
    myJamSoundTime(60),
    myPreviousCollisionNumber(0),
    myWaitingTime(0),
    myTimeLoss(0) {
    GUIIconSubSys::initIcons(a);
}


void
GUIApplicationWindow::dependentBuild() {
    // don't do this twice
    if (hadDependentBuild) {
        return;
    }
    hadDependentBuild = true;

    setTarget(this);
    setSelector(MID_WINDOW);

    // build menu bar
    myMenuBarDrag = new FXToolBarShell(this, GUIDesignToolBarShell3);
    myMenuBar = new FXMenuBar(myTopDock, myMenuBarDrag, GUIDesignBar);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    buildToolBars();
    // build the thread - io
    myLoadThreadEvent.setTarget(this),  myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    myRunThreadEvent.setTarget(this), myRunThreadEvent.setSelector(ID_RUNTHREAD_EVENT);

    // build the status bar
    myStatusbar = new FXStatusBar(this, GUIDesignStatusBar);
    {
        myGeoFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myGeoCoordinate = new FXLabel(myGeoFrame, "N/A\t\tOriginal coordinate (before coordinate transformation in NETCONVERT)", 0, LAYOUT_CENTER_Y);
        myCartesianFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myCartesianCoordinate = new FXLabel(myCartesianFrame, "N/A\t\tNetwork coordinate", 0, LAYOUT_CENTER_Y);
        myStatButtons.push_back(new FXButton(myStatusbar, "-", GUIIconSubSys::getIcon(ICON_GREENVEHICLE), this, MID_SHOWVEHSTATS));
        myStatButtons.push_back(new FXButton(myStatusbar, "-", GUIIconSubSys::getIcon(ICON_GREENPERSON), this, MID_SHOWPERSONSTATS));
        myStatButtons.back()->hide();
        myStatButtons.push_back(new FXButton(myStatusbar, "-", GUIIconSubSys::getIcon(ICON_GREENEDGE), this, MID_SHOWVEHSTATS));
        myStatButtons.back()->hide();
    }

    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this, GUIDesignSplitter | SPLITTER_VERTICAL | SPLITTER_REVERSED);
    myMDIClient = new FXMDIClient(myMainSplitter, GUIDesignSplitterMDI);
    myMDIMenu = new FXMDIMenu(this, myMDIClient);
    new FXMDIWindowButton(myMenuBar, myMDIMenu, myMDIClient, FXMDIClient::ID_MDI_MENUWINDOW, GUIDesignMDIButtonLeft);
    new FXMDIDeleteButton(myMenuBar, myMDIClient, FXMDIClient::ID_MDI_MENUCLOSE, GUIDesignMDIButtonRight);
    new FXMDIRestoreButton(myMenuBar, myMDIClient, FXMDIClient::ID_MDI_MENURESTORE, GUIDesignMDIButtonRight);
    new FXMDIMinimizeButton(myMenuBar, myMDIClient, FXMDIClient::ID_MDI_MENUMINIMIZE, GUIDesignMDIButtonRight);

    // build the message window
    myMessageWindow = new GUIMessageWindow(myMainSplitter);
    // fill menu and tool bar
    fillMenuBar();
    myToolBar6->hide();
    myToolBar7->hide();
    // build additional threads
    myLoadThread = new GUILoadThread(getApp(), this, myEvents, myLoadThreadEvent);
    myRunThread = new GUIRunThread(getApp(), this, *mySimDelayTarget, myEvents, myRunThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");
    // set the caption
    setTitle(MFXUtils::getTitleText("SUMO " VERSION_STRING));

    // start the simulation-thread (it will loop until the application ends deciding by itself whether to perform a step or not)
    myRunThread->start();
    setIcon(GUIIconSubSys::getIcon(ICON_APP));
}


void
GUIApplicationWindow::create() {
    setWindowSizeAndPos();
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS", "basedir", "");
    FXMainWindow::create();
    myMenuBarDrag->create();
    myToolBarDrag1->create();
    myToolBarDrag2->create();
    myToolBarDrag3->create();
    myToolBarDrag4->create();
    myToolBarDrag5->create();
    myToolBarDrag6->create();
    myToolBarDrag7->create();
    myFileMenu->create();
    mySelectByPermissions->create();
    myEditMenu->create();
    mySettingsMenu->create();
    myLocatorMenu->create();
    myControlMenu->create();
    myWindowsMenu->create();
    myHelpMenu->create();

    FXint width = getApp()->getNormalFont()->getTextWidth("8", 1) * 24;
    myCartesianFrame->setWidth(width);
    myGeoFrame->setWidth(width);

    show(PLACEMENT_DEFAULT);
    if (!OptionsCont::getOptions().isSet("window-size")) {
        if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 1) {
            maximize();
        }
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
    delete mySelectByPermissions;
    delete mySettingsMenu;
    delete myLocatorMenu;
    delete myControlMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

    delete myLoadThread;

    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
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
                      "&Open Simulation...\tCtrl+O\tOpen a simulation (Configuration file).",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG);
    new FXMenuCommand(myFileMenu,
                      "Open &Network...\tCtrl+N\tOpen a network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_OPEN_NETWORK);
    new FXMenuCommand(myFileMenu,
                      "Open Shapes \tCtrl+P\tLoad POIs and Polygons for visualization.",
                      GUIIconSubSys::getIcon(ICON_OPEN_SHAPES), this, MID_OPEN_SHAPES);
    new FXMenuCommand(myFileMenu,
                      "&Reload\tCtrl+R\tReloads the simulation / the network.",
                      GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "Close\tCtrl+W\tClose the simulation.",
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
                      "&Quit\tCtrl+Q\tQuit the Application.",
                      0, this, MID_QUIT, 0);

    // build edit menu
    mySelectByPermissions = new FXMenuPane(this);
    std::vector<std::string> vehicleClasses = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::iterator it = vehicleClasses.begin(); it != vehicleClasses.end(); ++it) {
        new FXMenuCommand(mySelectByPermissions,
                          (*it).c_str(), NULL, this, MID_EDITCHOSEN);
    }

    myEditMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Edit", NULL, myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Selected...\tCtrl+E\tOpens a dialog for editing the list of selected items.",
                      GUIIconSubSys::getIcon(ICON_FLAG), this, MID_EDITCHOSEN);
    new FXMenuCascade(myEditMenu,
                      "Select lanes which allow...\t\tOpens a menu for selecting a vehicle class by which to selected lanes.",
                      GUIIconSubSys::getIcon(ICON_FLAG), mySelectByPermissions);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Breakpoints...\tCtrl+B\tOpens a dialog for editing breakpoints.",
                      0, this, MID_EDIT_BREAKPOINTS);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Open in Netedit...\tCtrl+T\tOpens the netedit application with the current network.",
                      0, this, MID_NETEDIT);

    // build settings menu
    mySettingsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Settings", NULL, mySettingsMenu);
    new FXMenuCommand(mySettingsMenu,
                      "Application Settings...\t\tOpen a Dialog for Application Settings editing.",
                      NULL, this, MID_APPSETTINGS);
    new FXMenuCheck(mySettingsMenu,
                    "Gaming Mode\tCtrl+G\tToggle gaming mode on/off.",
                    this, MID_GAMING);
    new FXMenuCheck(mySettingsMenu,
                    "Full Screen Mode\tCtrl+F\tToggle full screen mode on/off.",
                    this, MID_FULLSCREEN);
    // build Locate menu
    myLocatorMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Locate", NULL, myLocatorMenu);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Junctions\t\tOpen a Dialog for Locating a Junction.",
                      GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), this, MID_LOCATEJUNCTION);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Edges\t\tOpen a Dialog for Locating an Edge.",
                      GUIIconSubSys::getIcon(ICON_LOCATEEDGE), this, MID_LOCATEEDGE);
    if (!MSGlobals::gUseMesoSim) { // there are no gui-vehicles in mesosim
        new FXMenuCommand(myLocatorMenu,
                          "Locate &Vehicles\t\tOpen a Dialog for Locating a Vehicle.",
                          GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), this, MID_LOCATEVEHICLE);
    }
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Persons\t\tOpen a Dialog for Locating a Person.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPERSON), this, MID_LOCATEPERSON);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &TLS\t\tOpen a Dialog for Locating a Traffic Light.",
                      GUIIconSubSys::getIcon(ICON_LOCATETLS), this, MID_LOCATETLS);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Additional\t\tOpen a Dialog for Locating an Additional Structure.",
                      GUIIconSubSys::getIcon(ICON_LOCATEADD), this, MID_LOCATEADD);
    new FXMenuCommand(myLocatorMenu,
                      "Locate P&oI\t\tOpen a Dialog for Locating a Point of Intereset.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPOI), this, MID_LOCATEPOI);
    new FXMenuCommand(myLocatorMenu,
                      "Locate Po&lygon\t\tOpen a Dialog for Locating a Polygon.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPOLY), this, MID_LOCATEPOLY);
    new FXMenuSeparator(myLocatorMenu);
    new FXMenuCheck(myLocatorMenu,
                    "Show Internal Structures\t\tShow internal junctions and streets in locator dialog.",
                    this, MID_LISTINTERNAL);
    FXMenuCheck* listParking = new FXMenuCheck(myLocatorMenu,
            "Show Parking Vehicles\t\tShow parking vehicles in locator dialog.",
            this, MID_LISTPARKING);
    listParking->setCheck(myListParking);
    new FXMenuCheck(myLocatorMenu,
                    "Show vehicles outside the road network\t\tShow vehicles that are teleporting or driving remote-controlled outside the road network in locator dialog.",
                    this, MID_LISTTELEPORTING);
    // build control menu
    myControlMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "Simulation", NULL, myControlMenu);
    new FXMenuCommand(myControlMenu,
                      "Run\tCtrl+A\tStart running the simulation.",
                      GUIIconSubSys::getIcon(ICON_START), this, MID_START);
    new FXMenuCommand(myControlMenu,
                      "Stop\tCtrl+S\tStop running the simulation.",
                      GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP);
    new FXMenuCommand(myControlMenu,
                      "Step\tCtrl+D\tPerform one simulation step.",
                      GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP);

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
    new FXMenuCommand(myHelpMenu, "&Online Documentation", 0, this, MID_HELP);
    new FXMenuCommand(myHelpMenu, "&About", GUIIconSubSys::getIcon(ICON_APP),
                      this, MID_ABOUT);
}


void
GUIApplicationWindow::buildToolBars() {
    // build tool bars
    {
        // file and simulation tool bar
        myToolBarDrag1 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar1 = new FXToolBar(myTopDock, myToolBarDrag1, GUIDesignToolBarShell1);
        new FXToolBarGrip(myToolBar1, myToolBar1, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        // build file tools
        new FXButton(myToolBar1, "\t\tOpen a simulation (Configuration file).", GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG, GUIDesignButtonToolbar);
        new FXButton(myToolBar1, "\t\tOpen a network.", GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_OPEN_NETWORK, GUIDesignButtonToolbar);
        new FXButton(myToolBar1, "\t\tReloads the simulation / the network.", GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD, GUIDesignButtonToolbar);
    }
    {
        // simulation toolbar
        myToolBarDrag2 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar2 = new FXToolBar(myTopDock, myToolBarDrag2, GUIDesignToolBarShell2);
        new FXToolBarGrip(myToolBar2, myToolBar2, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        // build simulation tools
        new FXButton(myToolBar2, "\t\tStart the loaded simulation.", GUIIconSubSys::getIcon(ICON_START), this, MID_START, GUIDesignButtonToolbar);
        new FXButton(myToolBar2, "\t\tStop the running simulation.", GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP, GUIDesignButtonToolbar);
        new FXButton(myToolBar2, "\t\tPerform a single simulation step.", GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP, GUIDesignButtonToolbar);
    }
    {
        // Simulation Step Display
        myToolBarDrag3 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar3 = new FXToolBar(myTopDock, myToolBarDrag3, GUIDesignToolBarShell2);
        new FXToolBarGrip(myToolBar3, myToolBar3, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        new FXButton(myToolBar3, "Time:\t\tToggle between seconds and hour:minute:seconds display", 0, this, MID_TIME_TOOGLE, GUIDesignButtonToolbarText);

        myLCDLabel = new FXEX::FXLCDLabel(myToolBar3, 13, 0, 0, JUSTIFY_RIGHT);
        myLCDLabel->setHorizontal(2);
        myLCDLabel->setVertical(6);
        myLCDLabel->setThickness(2);
        myLCDLabel->setGroove(2);
        myLCDLabel->setText("-------------");
    }
    {
        // Simulation Delay
        myToolBarDrag4 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar4 = new FXToolBar(myTopDock, myToolBarDrag4, GUIDesignToolBarShell2);
        new FXToolBarGrip(myToolBar4, myToolBar4, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        new FXButton(myToolBar4, "Delay (ms):\t\tToggle between alternative delay values", 0, this, MID_DELAY_TOOGLE, GUIDesignButtonToolbarText);

        mySimDelayTarget = new FXRealSpinDial(myToolBar4, 7, 0, MID_SIMDELAY, GUIDesignSpinDial);
        mySimDelayTarget->setNumberFormat(0);
        mySimDelayTarget->setIncrements(1, 10, 10);
        mySimDelayTarget->setRange(0, 1000);
        mySimDelayTarget->setValue(0);
    }
    {
        // Views
        myToolBarDrag5 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar5 = new FXToolBar(myTopDock, myToolBarDrag5, GUIDesignToolBarShell2);
        new FXToolBarGrip(myToolBar5, myToolBar5, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        // build view tools
        new FXButton(myToolBar5, "\t\tOpen a new microscopic view.",
                     GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_MICROVIEW, GUIDesignButtonToolbar);
#ifdef HAVE_OSG
        new FXButton(myToolBar5, "\t\tOpen a new 3D view.",
                     GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_OSGVIEW, GUIDesignButtonToolbar);
#endif
    }
    {
        /// game specific stuff
        // total waitingTime
        myToolBarDrag6 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar6 = new FXToolBar(myTopDock, myToolBarDrag6, GUIDesignToolBarShell2);
        new FXToolBarGrip(myToolBar6, myToolBar6, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        new FXLabel(myToolBar6, "Waiting Time:\t\tTime spent waiting accumulated for all vehicles", 0, LAYOUT_TOP | LAYOUT_LEFT);
        myWaitingTimeLabel = new FXEX::FXLCDLabel(myToolBar6, 13, 0, 0, JUSTIFY_RIGHT);
        myWaitingTimeLabel->setHorizontal(2);
        myWaitingTimeLabel->setVertical(6);
        myWaitingTimeLabel->setThickness(2);
        myWaitingTimeLabel->setGroove(2);
        myWaitingTimeLabel->setText("-------------");

        // idealistic time loss
        myToolBarDrag7 = new FXToolBarShell(this, GUIDesignToolBarShell3);
        myToolBar7 = new FXToolBar(myTopDock, myToolBarDrag7, GUIDesignToolBarShell2);
        new FXToolBarGrip(myToolBar7, myToolBar7, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        new FXLabel(myToolBar7, "Time Loss:\t\tTime lost due to being unable to drive with maximum speed for all vehicles", 0, LAYOUT_TOP | LAYOUT_LEFT);
        myTimeLossLabel = new FXEX::FXLCDLabel(myToolBar7, 13, 0, 0, JUSTIFY_RIGHT);
        myTimeLossLabel->setHorizontal(2);
        myTimeLossLabel->setVertical(6);
        myTimeLossLabel->setThickness(2);
        myTimeLossLabel->setGroove(2);
        myTimeLossLabel->setText("-------------");
    }
}


long
GUIApplicationWindow::onCmdQuit(FXObject*, FXSelector, void*) {
    if (!myAmFullScreen) {
        getApp()->reg().writeIntEntry("SETTINGS", "x", getX());
        getApp()->reg().writeIntEntry("SETTINGS", "y", getY());
        getApp()->reg().writeIntEntry("SETTINGS", "width", getWidth());
        getApp()->reg().writeIntEntry("SETTINGS", "height", getHeight());
    }
    getApp()->reg().writeStringEntry("SETTINGS", "basedir", gCurrentFolder.text());
    getApp()->reg().writeIntEntry("SETTINGS", "maximized", isMaximized() ? 1 : 0);
    getApp()->reg().writeIntEntry("gui", "timeasHMS", myShowTimeAsHMS ? 1 : 0);
    getApp()->reg().writeIntEntry("gui", "alternateSimDelay", (int)myAlternateSimDelay);
    getApp()->exit(0);
    return 1;
}


long
GUIApplicationWindow::onCmdEditChosen(FXObject* menu, FXSelector, void*) {
    FXMenuCommand* mc = dynamic_cast<FXMenuCommand*>(menu);
    if (mc->getText() == "Edit Selected...") {
        GUIDialog_GLChosenEditor* chooser =
            new GUIDialog_GLChosenEditor(this, &gSelected);
        chooser->create();
        chooser->show();
    } else {
        if (!myAmLoading && myRunThread->simulationAvailable()) {
            const SUMOVehicleClass svc = SumoVehicleClassStrings.get(mc->getText().text());
            for (MSEdgeVector::const_iterator i = MSEdge::getAllEdges().begin(); i != MSEdge::getAllEdges().end(); ++i) {
                const std::vector<MSLane*>& lanes = (*i)->getLanes();
                for (std::vector<MSLane*>::const_iterator it = lanes.begin(); it != lanes.end(); ++it) {
                    GUILane* lane = dynamic_cast<GUILane*>(*it);
                    assert(lane != 0);
                    if ((lane->getPermissions() & svc) != 0) {
                        gSelected.select(lane->getGlID());
                    }
                }
            }
            if (myMDIClient->numChildren() > 0) {
                GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
                if (w != 0) {
                    // color by selection
                    w->getView()->getVisualisationSettings()->laneColorer.setActive(1);
                }
            }
        }
        updateChildren();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdEditBreakpoints(FXObject*, FXSelector, void*) {
    GUIDialog_Breakpoints* chooser = new GUIDialog_Breakpoints(this, myRunThread->getBreakpoints(), myRunThread->getBreakpointLock());
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdHelp(FXObject*, FXSelector, void*) {
    FXLinkLabel::fxexecute("http://sumo.dlr.de/wiki/SUMO-GUI");
    return 1;
}


long
GUIApplicationWindow::onCmdNetedit(FXObject*, FXSelector, void*) {
    if (mySubWindows.empty()) {
        return 1;
    }
    FXRegistry reg("Netedit", "DLR");
    reg.read();
    const GUISUMOAbstractView* const v = static_cast<GUIGlChildWindow*>(mySubWindows[0])->getView();
    reg.writeRealEntry("viewport", "x", v->getChanger().getXPos());
    reg.writeRealEntry("viewport", "y", v->getChanger().getYPos());
    reg.writeRealEntry("viewport", "z", v->getChanger().getZPos());
    reg.write();
    std::string netedit = "netedit";
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath != 0) {
        std::string newPath = std::string(sumoPath) + "/bin/netedit";
        if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
            netedit = "\"" + newPath + "\"";
        }
    }
    std::string cmd = netedit + " --registry-viewport -s "  + OptionsCont::getOptions().getString("net-file");
    // start in background
#ifndef WIN32
    cmd = cmd + " &";
#else
    // see "help start" for the parameters
    cmd = "start /B \"\" " + cmd;
#endif
    WRITE_MESSAGE("Running " + cmd + ".");
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(cmd);
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
        loadConfigOrNet(file, false);
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
        loadConfigOrNet(file, true);
        myRecentNets.appendFile(file.c_str());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdOpenShapes(FXObject*, FXSelector, void*) {
    // get the shape file name
    FXFileDialog opendialog(this, "Open Shapes");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Additional files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();

        NLShapeHandler handler(file, myRunThread->getNet().getShapeContainer());
        if (!XMLSubSys::runParser(handler, file, false)) {
            WRITE_MESSAGE("Loading of " + file + " failed.");
        }
        update();
    }
    return 1;
}

long
GUIApplicationWindow::onCmdReload(FXObject*, FXSelector, void*) {
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    myLoadThread->start();
    setStatusBarText("Reloading.");
    update();
    return 1;
}


long
GUIApplicationWindow::onCmdOpenRecent(FXObject* sender, FXSelector, void* data) {
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    }
    std::string file((const char*)data);
    loadConfigOrNet(file, sender == &myRecentNets);
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
                   myAmLoading || myLoadThread->getFileName() == ""
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
    getApp()->forceRefresh(); // only callking myToolBar2->forceRefresh somehow loses keyboard focus
    return 1;
}


long
GUIApplicationWindow::onCmdStop(FXObject*, FXSelector, void*) {
    myRunThread->stop();
    getApp()->forceRefresh(); // only callking myToolBar2->forceRefresh somehow loses keyboard focus
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
    myAlternateSimDelay = (SUMOTime)mySimDelayTarget->getValue();
    mySimDelayTarget->setValue((FXdouble)tmp);
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
GUIApplicationWindow::onCmdShowStats(FXObject*, FXSelector, void*) {
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        GUINet::getGUIInstance()->getParameterWindow(*this, *w->getView());
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
        myMenuBar->hide();
        myStatusbar->hide();
        myToolBar1->hide();
        myToolBar2->hide();
        myToolBar4->hide();
        myToolBar5->hide();
        myToolBar6->show();
        myToolBar7->show();
        myMessageWindow->hide();
        myLCDLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        myWaitingTimeLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        myTimeLossLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        gSchemeStorage.getDefault().gaming = true;
    } else {
        myMenuBar->show();
        myStatusbar->show();
        myToolBar1->show();
        myToolBar2->show();
        myToolBar4->show();
        myToolBar5->show();
        myToolBar6->hide();
        myToolBar7->hide();
        myMessageWindow->show();
        myLCDLabel->setFgColor(MFXUtils::getFXColor(RGBColor::GREEN));
        gSchemeStorage.getDefault().gaming = false;
    }
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            w->setToolBarVisibility(!myAmGaming && !myAmFullScreen);
        }
    }
    update();
    return 1;
}


long
GUIApplicationWindow::onCmdFullScreen(FXObject*, FXSelector, void*) {
    myAmFullScreen = !myAmFullScreen;
    if (myAmFullScreen) {
        getApp()->reg().writeIntEntry("SETTINGS", "x", getX());
        getApp()->reg().writeIntEntry("SETTINGS", "y", getY());
        getApp()->reg().writeIntEntry("SETTINGS", "width", getWidth());
        getApp()->reg().writeIntEntry("SETTINGS", "height", getHeight());
        maximize();
        setDecorations(DECOR_NONE);
        place(PLACEMENT_MAXIMIZED);
        myMenuBar->hide();
        myStatusbar->hide();
        myToolBar1->hide();
        myToolBar2->hide();
        myToolBar3->hide();
        myToolBar4->hide();
        myToolBar5->hide();
        myToolBar6->hide();
        myToolBar7->hide();
        myMessageWindow->hide();
        if (myMDIClient->numChildren() > 0) {
            GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
            if (w != 0) {
                w->setToolBarVisibility(false);
            }
        }
        update();
    } else {
        place(PLACEMENT_VISIBLE);
        setDecorations(DECOR_ALL);
        restore();
        myToolBar3->show();
        myAmGaming = !myAmGaming;
        onCmdGaming(0, 0, 0);
        setWidth(getApp()->reg().readIntEntry("SETTINGS", "width", 600));
        setHeight(getApp()->reg().readIntEntry("SETTINGS", "height", 400));
        setX(getApp()->reg().readIntEntry("SETTINGS", "x", 150));
        setY(getApp()->reg().readIntEntry("SETTINGS", "y", 150));
    }
    return 1;
}


long
GUIApplicationWindow::onCmdListInternal(FXObject*, FXSelector, void*) {
    myListInternal = !myListInternal;
    return 1;
}


long
GUIApplicationWindow::onCmdListParking(FXObject*, FXSelector, void*) {
    myListParking = !myListParking;
    return 1;
}

long
GUIApplicationWindow::onCmdListTeleporting(FXObject*, FXSelector, void*) {
    myListTeleporting = !myListTeleporting;
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
    GUIDialog_AboutSUMO* about = new GUIDialog_AboutSUMO(this);
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
        GUIEvent* e = myEvents.top();
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
            case EVENT_STATUS_OCCURED:
                handleEvent_Message(e);
                break;
            case EVENT_SIMULATION_ENDED:
                handleEvent_SimulationEnded(e);
                break;
            case EVENT_SCREENSHOT:
                handleEvent_Screenshot(e);
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
        if (!myRunThread->init(ec->myNet, ec->myBegin, ec->myEnd)) {
            if (GUIGlobals::gQuitOnEnd) {
                closeAllWindows();
                getApp()->exit(1);
            }
        } else {
            // report success
            setStatusBarText("'" + ec->myFile + "' loaded.");
            myWasStarted = false;
            myHaveNotifiedAboutSimEnd = false;
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
                    settings.applyViewport(view);
                    settings.setSnapshots(view);
                    if (settings.getDelay() > 0) {
                        mySimDelayTarget->setValue(settings.getDelay());
                    }
                    if (settings.getBreakpoints().size() > 0) {
                        myRunThread->getBreakpointLock().lock();
                        myRunThread->getBreakpoints().assign(settings.getBreakpoints().begin(), settings.getBreakpoints().end());
                        myRunThread->getBreakpointLock().unlock();
                    }
                    myJamSounds = settings.getEventDistribution("jam");
                    myCollisionSounds = settings.getEventDistribution("collision");
                    if (settings.getJamSoundTime() > 0) {
                        myJamSoundTime = settings.getJamSoundTime();
                    }
                }
            } else {
                openNewView(defaultType);
            }

            if (isGaming()) {
                setTitle("SUMO Interactive Traffic Light");
            } else {
                // set simulation name on the caption
                setTitle(MFXUtils::getTitleText("SUMO " VERSION_STRING, ec->myFile.c_str()));
            }
            // set simulation step begin information
            myLCDLabel->setText("-------------");
            for (std::vector<FXButton*>::const_iterator it = myStatButtons.begin(); it != myStatButtons.end(); ++it) {
                (*it)->setText("-");
            }
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
    const int running = myRunThread->getNet().getVehicleControl().getRunningVehicleNo();
    const int backlog = myRunThread->getNet().getInsertionControl().getWaitingVehicleNo();
    if (backlog > running) {
        if (myStatButtons.front()->getIcon() == GUIIconSubSys::getIcon(ICON_GREENVEHICLE)) {
            myStatButtons.front()->setIcon(GUIIconSubSys::getIcon(ICON_YELLOWVEHICLE));
        }
    } else {
        if (myStatButtons.front()->getIcon() == GUIIconSubSys::getIcon(ICON_YELLOWVEHICLE)) {
            myStatButtons.front()->setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
        }
    }
    myStatButtons.front()->setText(toString(running).c_str());
    if (myRunThread->getNet().hasPersons()) {
        if (!myStatButtons[1]->shown()) {
            myStatButtons[1]->show();
        }
        myStatButtons[1]->setText(toString(myRunThread->getNet().getPersonControl().getRunningNumber()).c_str());
    }
    if (myRunThread->getNet().hasContainers()) {
        if (!myStatButtons[2]->shown()) {
            myStatButtons[2]->show();
        }
        myStatButtons[2]->setText(toString(myRunThread->getNet().getContainerControl().getRunningNumber()).c_str());
    }
    if (myAmGaming) {
        checkGamingEvents();
    }
    if (myRunThread->simulationIsStartable()) {
        getApp()->forceRefresh(); // restores keyboard focus
    }
    update();
}


void
GUIApplicationWindow::handleEvent_Message(GUIEvent* e) {
    GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
    if (ec->getOwnType() == EVENT_STATUS_OCCURED) {
        setStatusBarText(ec->getMsg());
    } else {
        myMessageWindow->appendMsg(ec->getOwnType(), ec->getMsg());
    }
}


void
GUIApplicationWindow::handleEvent_SimulationEnded(GUIEvent* e) {
    GUIEvent_SimulationEnded* ec = static_cast<GUIEvent_SimulationEnded*>(e);
    onCmdStop(0, 0, 0);
    if (GUIGlobals::gQuitOnEnd) {
        closeAllWindows();
        getApp()->exit(ec->getReason() == MSNet::SIMSTATE_ERROR_IN_SIM);
    } else if (GUIGlobals::gDemoAutoReload) {
        onCmdReload(0, 0, 0);
    } else if (!myHaveNotifiedAboutSimEnd) {
        // build the text
        const std::string text = "Simulation ended at time: " + time2string(ec->getTimeStep()) +
                                 ".\nReason: " + MSNet::getStateMessage(ec->getReason()) +
                                 "\nDo you want to close all open files and views?";
        FXuint answer = FXMessageBox::question(this, MBOX_YES_NO, "Simulation ended", "%s", text.c_str());
        if (answer == 1) { //1:yes, 2:no, 4:esc
            closeAllWindows();
        }
        myHaveNotifiedAboutSimEnd = true;
    }
}


void
GUIApplicationWindow::handleEvent_Screenshot(GUIEvent* e) {
    GUIEvent_Screenshot* ec = static_cast<GUIEvent_Screenshot*>(e);
    myEventMutex.lock();
    const std::string error = ec->myView->makeSnapshot(ec->myFile);
    if (error != "") {
        WRITE_WARNING(error);
    }
    myEventCondition.signal();
    myEventMutex.unlock();
}


void
GUIApplicationWindow::checkGamingEvents() {
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();
#ifdef HAVE_DANGEROUS_SOUNDS // disable user-configurable command execution for public build
    if (myJamSounds.getOverallProb() > 0) {
        // play honking sound if some vehicle is waiting too long
        for (; it != end; ++it) {
            // XXX use impatience instead of waiting time ?
            if (it->second->getWaitingTime() > TIME2STEPS(myJamSoundTime)) {
                const std::string cmd = myJamSounds.get(&myGamingRNG);
                if (cmd != "") {
                    // yay! fun with dangerous commands... Never use this over the internet
                    SysUtils::runHiddenCommand(cmd);
                    // one sound per simulation step is enough
                    break;
                }
            }
        }
    }
    if (myCollisionSounds.getOverallProb() > 0) {
        int collisions = MSNet::getInstance()->getVehicleControl().getCollisionCount();
        if (myPreviousCollisionNumber != collisions) {
            const std::string cmd = myCollisionSounds.get(&myGamingRNG);
            if (cmd != "") {
                // yay! fun with dangerous commands... Never use this over the internet
                SysUtils::runHiddenCommand(cmd);
            }
            myPreviousCollisionNumber = collisions;
        }
    }
#endif

    // update performance indicators
    for (it = vc.loadedVehBegin(); it != end; ++it) {
        const MSVehicle* veh = dynamic_cast<MSVehicle*>(it->second);
        assert(veh != 0);
        if (veh->isOnRoad()) {
            const SUMOReal vmax = MIN2(veh->getVehicleType().getMaxSpeed(), veh->getEdge()->getSpeedLimit());
            if (veh->getSpeed() < SUMO_const_haltingSpeed) {
                myWaitingTime += DELTA_T;
            }
            myTimeLoss += TIME2STEPS(TS * (vmax - veh->getSpeed()) / vmax); // may be negative with speedFactor > 1
        }
    }
    myWaitingTimeLabel->setText(time2string(myWaitingTime).c_str());
    myTimeLossLabel->setText(time2string(myTimeLoss).c_str());
}


void
GUIApplicationWindow::loadConfigOrNet(const std::string& file, bool isNet) {
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    gSchemeStorage.saveViewport(0, 0, -1); // recenter view
    myLoadThread->loadConfigOrNet(file, isNet);
    setStatusBarText("Loading '" + file + "'.");
    update();
}


GUISUMOAbstractView*
GUIApplicationWindow::openNewView(GUISUMOViewParent::ViewType vt) {
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 0;
    }
    GUISUMOAbstractView* oldView = 0;
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            oldView = w->getView();
        }
    }
    std::string caption = "View #" + toString(myViewNumber++);
    FXuint opts = MDI_TRACKING;
    GUISUMOViewParent* w = new GUISUMOViewParent(myMDIClient, myMDIMenu, FXString(caption.c_str()),
            this, GUIIconSubSys::getIcon(ICON_APP), opts, 10, 10, 300, 200);
    GUISUMOAbstractView* v = w->init(getBuildGLCanvas(), myRunThread->getNet(), vt);
    if (oldView != 0) {
        // copy viewport
        oldView->copyViewportTo(v);
    }
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
    for (std::vector<FXButton*>::const_iterator it = myStatButtons.begin(); it != myStatButtons.end(); ++it) {
        (*it)->setText("-");
        if (it != myStatButtons.begin()) {
            (*it)->hide();
        }
    }
    // remove trackers and other external windows
    int i;
    for (i = 0; i < (int)mySubWindows.size(); ++i) {
        mySubWindows[i]->destroy();
    }
    for (i = 0; i < (int)myTrackerWindows.size(); ++i) {
        myTrackerWindows[i]->destroy();
    }
    // delete the simulation
    myRunThread->deleteSim();
    // reset the caption
    setTitle(MFXUtils::getTitleText("SUMO " VERSION_STRING));
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
    loadConfigOrNet("", false);
}


void
GUIApplicationWindow::setStatusBarText(const std::string& text) {
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
}


void
GUIApplicationWindow::addRecentFile(const FX::FXString& f, const bool isNet) {
    if (isNet) {
        myRecentNets.appendFile(f);
    } else {
        myRecentConfigs.appendFile(f);
    }
}


void
GUIApplicationWindow::updateTimeLCD(SUMOTime time) {
    time -= DELTA_T; // synchronize displayed time with netstate output
    if (time < 0) {
        myLCDLabel->setText("-------------");
        return;
    }
    if (myAmGaming) {
        // show time counting backwards
        time = myRunThread->getSimEndTime() - time;
    }
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


long
GUIApplicationWindow::onKeyPress(FXObject* o, FXSelector sel, void* data) {
    const long handled = FXMainWindow::onKeyPress(o, sel, data);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            w->onKeyPress(0, sel, data);
        }
    }
    return 0;
}


long
GUIApplicationWindow::onKeyRelease(FXObject* o, FXSelector sel, void* data) {
    const long handled = FXMainWindow::onKeyRelease(o, sel, data);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            w->onKeyRelease(0, sel, data);
        }
    }
    return 0;
}


void
GUIApplicationWindow::sendBlockingEvent(GUIEvent* event) {
    myEventMutex.lock();
    myEvents.add(event);
    myRunThreadEvent.signal();
    myEventCondition.wait(myEventMutex);
    myEventMutex.unlock();
}


/****************************************************************************/
