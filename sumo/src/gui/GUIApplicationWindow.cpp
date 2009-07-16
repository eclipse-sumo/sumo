/****************************************************************************/
/// @file    GUIApplicationWindow.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The main window of the SUMO-gui.
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <fx.h>
#include <fx3d.h>
#include <string>
#include <sstream>
#include <algorithm>

#include <guisim/GUINet.h>

#include "GUISUMOViewParent.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"

#include <utils/common/ToString.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include "GUIEvent_SimulationLoaded.h"
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include "GUIGlobals.h"
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include "dialogs/GUIDialog_AboutSUMO.h"
#include "dialogs/GUIDialog_AppSettings.h"
#include "dialogs/GUIDialog_Breakpoints.h"
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUISettingsHandler.h>
#include <guisim/GUISelectionLoader.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GUIApplicationWindow) GUIApplicationWindowMap[]= {
    //________Message_Type____________ID________________________Message_Handler________
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
    FXMAPFUNC(SEL_COMMAND,  MID_ABOUT,              GUIApplicationWindow::onCmdAbout),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_MICROVIEW,      GUIApplicationWindow::onCmdNewView),
    FXMAPFUNC(SEL_COMMAND,  MID_START,              GUIApplicationWindow::onCmdStart),
    FXMAPFUNC(SEL_COMMAND,  MID_STOP,               GUIApplicationWindow::onCmdStop),
    FXMAPFUNC(SEL_COMMAND,  MID_STEP,               GUIApplicationWindow::onCmdStep),
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW, GUIApplicationWindow::onCmdClearMsgWindow),

    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,       GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,      GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_RELOAD,            GUIApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,        GUIApplicationWindow::onUpdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_MICROVIEW,     GUIApplicationWindow::onUpdAddMicro),
    FXMAPFUNC(SEL_UPDATE,   MID_START,             GUIApplicationWindow::onUpdStart),
    FXMAPFUNC(SEL_UPDATE,   MID_STOP,              GUIApplicationWindow::onUpdStop),
    FXMAPFUNC(SEL_UPDATE,   MID_STEP,              GUIApplicationWindow::onUpdStep),
    FXMAPFUNC(SEL_UPDATE,   MID_EDITCHOSEN,        GUIApplicationWindow::onUpdEditChosen),
    FXMAPFUNC(SEL_UPDATE,   MID_EDIT_BREAKPOINTS,  GUIApplicationWindow::onUpdEditBreakpoints),

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
        const std::string &configPattern)
        : GUIMainWindow(a),
        myLoadThread(0), myRunThread(0),
        myAmLoading(false),
        mySimDelay(50), myConfigPattern(configPattern), hadDependentBuild(false), myRecentNets(a, "nets") {
    GUIIconSubSys::init(a);
}


void
GUIApplicationWindow::dependentBuild() {
    // do this not twice
    if (hadDependentBuild) {
        return;
    }
    hadDependentBuild = true;

    setTarget(this);
    setSelector(MID_WINDOW);
    GUITexturesHelper::init(getApp());

    // build menu bar
    myMenuBarDrag=new FXToolBarShell(this,FRAME_NORMAL);
    myMenuBar = new FXMenuBar(myTopDock, myMenuBarDrag,
                              LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP,
                      TOOLBARGRIP_DOUBLE);
    buildToolBars();
    // build the thread - io
    myLoadThreadEvent.setTarget(this),
    myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    myRunThreadEvent.setTarget(this),
    myRunThreadEvent.setSelector(ID_RUNTHREAD_EVENT);

    // build the status bar
    myStatusbar = new FXStatusBar(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_RAISED);
    {
        myGeoFrame =
            new FXHorizontalFrame(myStatusbar, LAYOUT_FIX_WIDTH|LAYOUT_FILL_Y|LAYOUT_RIGHT|FRAME_SUNKEN,
                                  0,0,20,0, 0,0,0,0, 0,0);
        myGeoCoordinate = new FXLabel(myGeoFrame, "N/A", 0, LAYOUT_CENTER_Y);
        myCartesianFrame =
            new FXHorizontalFrame(myStatusbar, LAYOUT_FIX_WIDTH|LAYOUT_FILL_Y|LAYOUT_RIGHT|FRAME_SUNKEN,
                                  0,0,20,0, 0,0,0,0, 0,0);
        myCartesianCoordinate = new FXLabel(myCartesianFrame, "N/A", 0, LAYOUT_CENTER_Y);
    }

    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this,
                                    SPLITTER_REVERSED|SPLITTER_VERTICAL|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_TRACKING|FRAME_RAISED|FRAME_THICK);
    myMDIClient = new FXMDIClient(myMainSplitter,
                                  LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK);
    myMDIMenu = new FXMDIMenu(this,myMDIClient);
    new FXMDIWindowButton(myMenuBar,myMDIMenu,myMDIClient,
                          FXMDIClient::ID_MDI_MENUWINDOW,LAYOUT_LEFT);
    new FXMDIDeleteButton(myMenuBar,myMDIClient,
                          FXMDIClient::ID_MDI_MENUCLOSE,FRAME_RAISED|LAYOUT_RIGHT);
    new FXMDIRestoreButton(myMenuBar,myMDIClient,
                           FXMDIClient::ID_MDI_MENURESTORE,FRAME_RAISED|LAYOUT_RIGHT);
    new FXMDIMinimizeButton(myMenuBar,myMDIClient,
                            FXMDIClient::ID_MDI_MENUMINIMIZE,FRAME_RAISED|LAYOUT_RIGHT);

    // build the message window
    myMessageWindow = new GUIMessageWindow(myMainSplitter);
    // fill menu and tool bar
    fillMenuBar();
    // build additional threads
    myLoadThread = new GUILoadThread(this, myEvents, myLoadThreadEvent);
    myRunThread = new GUIRunThread(this, *mySimDelayTarget, myEvents,
                                   myRunThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");
    /*
    myProgressBar =
        new FXProgressBar(myStatusbar, 0, 0, PROGRESSBAR_NORMAL|LAYOUT_FILL_X, 200);
    */
    // set the caption
    setTitle(MFXUtils::getTitleText(("SUMO " + std::string(VERSION_STRING)).c_str()));

    // start the simulation-thread
    //  (it will loop until the application ends deciding by itself whether
    //        to perform a step or not)
    myRunThread->start();
    //}
    setIcon(GUIIconSubSys::getIcon(ICON_APP));
}


void
GUIApplicationWindow::create() {
    if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0)==0) {
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
    myWindowsMenu->create();
    myHelpMenu->create();

    FXint width = getApp()->getNormalFont()->getTextWidth("8", 1)*22;
    myCartesianFrame->setWidth(width);
    myGeoFrame->setWidth(width);

    show(PLACEMENT_SCREEN);
    if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0)==1) {
        maximize();
    }
}





GUIApplicationWindow::~GUIApplicationWindow() {
    myRunThread->prepareDestruction();
    myRunThread->join();
    closeAllWindows();
    //
    GUIIconSubSys::close();
    GUITexturesHelper::close();
    delete myGLVisual;
    // delete some non-parented windows
    delete myToolBarDrag1;
    //
    delete myRunThread;
    delete myFileMenu;
    delete myEditMenu;
    delete mySettingsMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

    delete myLoadThread;

    delete gGradients;

    while (!myEvents.empty()) {
        // get the next event
        GUIEvent *e = static_cast<GUIEvent*>(myEvents.top());
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
    new FXMenuTitle(myMenuBar,"&File",NULL,myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Open Simulation...\tCtl-O\tOpen a simulation (Configuration file).",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG),this,MID_OPEN_CONFIG);
    new FXMenuCommand(myFileMenu,
                      "Open &Network...\tCtl-N\tOpen a network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET),this,MID_OPEN_NETWORK);
    new FXMenuCommand(myFileMenu,
                      "&Reload\tCtl-R\tReloads the simulation / the network.",
                      GUIIconSubSys::getIcon(ICON_RELOAD),this,MID_RELOAD);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Close\tCtl-C\tClose the simulation.",
                      GUIIconSubSys::getIcon(ICON_CLOSE),this,MID_CLOSE);
    // Recent files
    FXMenuSeparator* sep1=new FXMenuSeparator(myFileMenu);
    sep1->setTarget(&myRecentConfigs);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myFileMenu,"",0,&myRecentConfigs,FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(myFileMenu,"C&lear Recent Configurations",NULL,&myRecentConfigs,FXRecentFiles::ID_CLEAR);
    myRecentConfigs.setTarget(this);
    myRecentConfigs.setSelector(MID_RECENTFILE);
    FXMenuSeparator* sep2=new FXMenuSeparator(myFileMenu);
    sep2->setTarget(&myRecentNets);
    sep2->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myFileMenu,"",0,&myRecentNets,FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(myFileMenu,"Cl&ear Recent Networks",NULL,&myRecentNets,FXRecentFiles::ID_CLEAR);
    myRecentNets.setTarget(this);
    myRecentNets.setSelector(MID_RECENTFILE);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Quit\tCtl-Q\tQuit the Application.",
                      0, this, MID_QUIT, 0);

    // build edit menu
    myEditMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Edit",NULL,myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Chosen...\t\tOpens a Dialog for editing the List of chosen Items.",
                      GUIIconSubSys::getIcon(ICON_FLAG), this, MID_EDITCHOSEN);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Breakpoints...\t\tOpens a Dialog for editing breakpoints.",
                      0, this, MID_EDIT_BREAKPOINTS);

    // build settings menu
    mySettingsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Settings",NULL,mySettingsMenu);
    new FXMenuCommand(mySettingsMenu,
                      "Application Settings...\t\tOpen a Dialog for Application Settings editing.",
                      NULL,this,MID_APPSETTINGS);

    // build windows menu
    myWindowsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Windows",NULL,myWindowsMenu);
    new FXMenuCheck(myWindowsMenu,
                    "Show Status Line\t\tToggle this Status Bar on/off.",
                    myStatusbar,FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Message Window\t\tToggle the Message Window on/off.",
                    myMessageWindow,FXWindow::ID_TOGGLESHOWN);
    /*
    new FXMenuCheck(myWindowsMenu,
        "Show Toolbar\t\tToggle the Toolbar on/off.",
        myToolBar1, FXWindow::ID_TOGGLESHOWN);
        */
    new FXMenuCheck(myWindowsMenu,
                    "Show Simulation Time\t\tToggle the Simulation Time on/off.",
                    myToolBar3, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Simulation Delay\t\tToggle the Simulation Delay Entry on/off.",
                    myToolBar4, FXWindow::ID_TOGGLESHOWN);
    addToWindowsMenu(myWindowsMenu);
    new FXMenuSeparator(myWindowsMenu);
    new FXMenuCommand(myWindowsMenu,"Tile &Horizontally",
                      GUIIconSubSys::getIcon(ICON_WINDOWS_TILE_HORI),
                      myMDIClient,FXMDIClient::ID_MDI_TILEHORIZONTAL);
    new FXMenuCommand(myWindowsMenu,"Tile &Vertically",
                      GUIIconSubSys::getIcon(ICON_WINDOWS_TILE_VERT),
                      myMDIClient,FXMDIClient::ID_MDI_TILEVERTICAL);
    new FXMenuCommand(myWindowsMenu,"C&ascade",
                      GUIIconSubSys::getIcon(ICON_WINDOWS_CASCADE),
                      myMDIClient,FXMDIClient::ID_MDI_CASCADE);
    new FXMenuCommand(myWindowsMenu,"&Close",NULL,
                      myMDIClient,FXMDIClient::ID_MDI_CLOSE);
    sep1=new FXMenuSeparator(myWindowsMenu);
    sep1->setTarget(myMDIClient);
    sep1->setSelector(FXMDIClient::ID_MDI_ANY);
    new FXMenuCommand(myWindowsMenu,"",0,myMDIClient,FXMDIClient::ID_MDI_1);
    new FXMenuCommand(myWindowsMenu,"",0,myMDIClient,FXMDIClient::ID_MDI_2);
    new FXMenuCommand(myWindowsMenu,"",0,myMDIClient,FXMDIClient::ID_MDI_3);
    new FXMenuCommand(myWindowsMenu,"",0,myMDIClient,FXMDIClient::ID_MDI_4);
    new FXMenuCommand(myWindowsMenu,"&Others...",0,myMDIClient,FXMDIClient::ID_MDI_OVER_5);
    new FXMenuSeparator(myWindowsMenu);
    new FXMenuCommand(myWindowsMenu,
                      "Clear Message Window\t\tClear the message window.",
                      0, this, MID_CLEARMESSAGEWINDOW);

    // build help menu
    myHelpMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Help",NULL,myHelpMenu);
    new FXMenuCommand(myHelpMenu,"&About", GUIIconSubSys::getIcon(ICON_APP),
                      this, MID_ABOUT);
}


void
GUIApplicationWindow::buildToolBars() {
    // build tool bars
    {
        // file and simulation tool bar
        myToolBarDrag1=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar1 = new FXToolBar(myTopDock,myToolBarDrag1,
                                   LAYOUT_DOCK_NEXT|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar1, myToolBar1, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        // build file tools
        new FXButton(myToolBar1,"\t\tOpen a simulation (Configuration file).",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar1,"\t\tOpen a network.",
                     GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_OPEN_NETWORK,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar1,"\t\tReloads the simulation / the network.",
                     GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
    {
        // build simulation tools
        myToolBarDrag2=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar2 = new FXToolBar(myTopDock,myToolBarDrag2,
                                   LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar2, myToolBar2, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXButton(myToolBar2,"\t\tStart the loaded simulation.",
                     GUIIconSubSys::getIcon(ICON_START), this, MID_START,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar2,"\t\tStop the running simulation.",
                     GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar2,"\t\tPerform a single simulation step.",
                     GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        /*
                new FXButton(myToolBar2,"\t\tCompute strategies.",
                    GUIIconSubSys::getIcon(ICON_CUT_SWELL), this, MID_CUTSWELL,
                    ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        */

    }
    {
        // Simulation Step Display
        myToolBarDrag3=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar3 = new FXToolBar(myTopDock,myToolBarDrag3,
                                   LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar3, myToolBar3, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXLabel(myToolBar3, "Next Step:", 0, LAYOUT_CENTER_Y);
        myLCDLabel = new FXEX::FXLCDLabel(myToolBar3, 6, 0, 0,
                                          FXEX::LCDLABEL_LEADING_ZEROS);
        myLCDLabel->setHorizontal(2);
        myLCDLabel->setVertical(6);
        myLCDLabel->setThickness(2);
        myLCDLabel->setGroove(2);
        myLCDLabel->setText("-----------");
    }
    {
        // Simulation Delay
        myToolBarDrag4=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar4 = new FXToolBar(myTopDock,myToolBarDrag4,
                                   LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED|LAYOUT_FILL_Y);
        new FXToolBarGrip(myToolBar4, myToolBar4, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXLabel(myToolBar4, "Delay (ms):", 0, LAYOUT_CENTER_Y);
        mySimDelayTarget =
            new FXRealSpinDial(myToolBar4, 7, 0, MID_SIMDELAY,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_Y);
        mySimDelayTarget->setNumberFormat(0);
        mySimDelayTarget->setIncrements(1,10,10);
        mySimDelayTarget->setRange(0,1000);
        mySimDelayTarget->setValue(0);
    }
    {
        // Views
        myToolBarDrag5=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar5 = new FXToolBar(myTopDock,myToolBarDrag5,
                                   LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar5, myToolBar5, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        // build view tools
        new FXButton(myToolBar5,"\t\tOpen a new microscopic view.",
                     GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_MICROVIEW,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
}


long
GUIApplicationWindow::onCmdQuit(FXObject*,FXSelector,void*) {
    getApp()->reg().writeIntEntry("SETTINGS", "x", getX());
    getApp()->reg().writeIntEntry("SETTINGS", "y", getY());
    getApp()->reg().writeIntEntry("SETTINGS", "width", getWidth());
    getApp()->reg().writeIntEntry("SETTINGS", "height", getHeight());
    getApp()->reg().writeStringEntry("SETTINGS", "basedir", gCurrentFolder.text());
    if (isMaximized()) {
        getApp()->reg().writeIntEntry("SETTINGS", "maximized", 1);
    } else {
        getApp()->reg().writeIntEntry("SETTINGS", "maximized", 0);
    }
    getApp()->exit(0);
    return 1;
}


long
GUIApplicationWindow::onCmdEditChosen(FXObject*,FXSelector,void*) {
    GUIDialog_GLChosenEditor *chooser =
        new GUIDialog_GLChosenEditor(this, &gSelected);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdEditBreakpoints(FXObject*,FXSelector,void*) {
    GUIDialog_Breakpoints *chooser =
        new GUIDialog_Breakpoints(this);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdOpenConfiguration(FXObject*,FXSelector,void*) {
    // get the new file name
    FXFileDialog opendialog(this,"Open Simulation Configuration");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(myConfigPattern.c_str());
    if (gCurrentFolder.length()!=0) {
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
GUIApplicationWindow::onCmdOpenNetwork(FXObject*,FXSelector,void*) {
    // get the new file name
    FXFileDialog opendialog(this,"Open Network");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("SUMO nets (*.net.xml)\nAll files (*)");
    if (gCurrentFolder.length()!=0) {
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
GUIApplicationWindow::onCmdReload(FXObject*,FXSelector,void*) {
    load("", false, true);
    return 1;
}


long
GUIApplicationWindow::onCmdOpenRecent(FXObject*sender,FXSelector,void *data) {
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    }
    std::string file((const char*)data);
    load(file, sender == &myRecentNets);
    return 1;
}


long
GUIApplicationWindow::onCmdClose(FXObject*,FXSelector,void*) {
    closeAllWindows();
    return 1;
}


long
GUIApplicationWindow::onUpdOpen(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   myAmLoading?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdReload(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   myAmLoading||myLoadThread->getFileName()==""
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdOpenRecent(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   myAmLoading?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdAddMicro(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   myAmLoading||!myRunThread->simulationAvailable()
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdStart(FXObject*,FXSelector,void*) {
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
GUIApplicationWindow::onCmdStop(FXObject*,FXSelector,void*) {
    myRunThread->stop();
    return 1;
}


long
GUIApplicationWindow::onCmdStep(FXObject*,FXSelector,void*) {
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
GUIApplicationWindow::onCmdClearMsgWindow(FXObject*,FXSelector,void*) {
    myMessageWindow->clear();
    return 1;
}


long
GUIApplicationWindow::onUpdStart(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStartable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStop(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStopable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStep(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStepable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdEditChosen(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   !myRunThread->simulationAvailable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdEditBreakpoints(FXObject *sender,FXSelector,void *ptr) {
    sender->handle(this,
                   !myRunThread->simulationAvailable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdAppSettings(FXObject*,FXSelector,void*) {
    GUIDialog_AppSettings *d = new GUIDialog_AppSettings(this);
    d->create();
    d->show(PLACEMENT_OWNER);
    return 1;
}


long
GUIApplicationWindow::onCmdNewView(FXObject*,FXSelector,void*) {
    openNewView();
    return 1;
}


long
GUIApplicationWindow::onCmdAbout(FXObject*,FXSelector,void*) {
    GUIDialog_AboutSUMO *about =
        new GUIDialog_AboutSUMO(this, "About SUMO", 0, 0);
    about->create();
    about->show(PLACEMENT_OWNER);
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
        GUIEvent *e = static_cast<GUIEvent*>(myEvents.top());
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
        case EVENT_SIMULATION_LOADED:
            handleEvent_SimulationLoaded(e);
            break;
        case EVENT_SIMULATION_STEP:
            handleEvent_SimulationStep(e);
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
GUIApplicationWindow::handleEvent_SimulationLoaded(GUIEvent *e) {
    GUITexturesHelper::init(getApp());
    myAmLoading = false;
    GUIEvent_SimulationLoaded *ec = static_cast<GUIEvent_SimulationLoaded*>(e);
    // check whether the loading was successfull
    if (ec->myNet==0) {
        // report failure
        setStatusBarText("Loading of '" + ec->myFile + "' failed!");
        if (gQuitOnEnd) {
            closeAllWindows();
            getApp()->exit(1);
        }
    } else {
        // report success
        setStatusBarText("'" + ec->myFile + "' loaded.");
        // initialise simulation thread
        myRunThread->init(ec->myNet, ec->myBegin, ec->myEnd);
        myWasStarted = false;
        // initialise views
        myViewNumber = 0;
        GUISUMOAbstractView* view = openNewView();
        if (view && ec->mySettingsFile != "") {
            GUISettingsHandler settings(ec->mySettingsFile);
            std::string settingsName = settings.addSettings(view);
            view->addDecals(settings.getDecals());
            settings.setViewport(view);
            settings.setSnapshots(view);
        }
        // set simulation name on the caption
        std::string caption = "SUMO " + std::string(VERSION_STRING);
        setTitle(MFXUtils::getTitleText(caption.c_str(), ec->myFile.c_str()));
        std::ostringstream str;
        // set simulation step begin information
        str << (int) ec->myNet->getCurrentTimeStep();
        myLCDLabel->setText(str.str().c_str());
    }
    getApp()->endWaitCursor();
    // start if wished
    if (myRunAtBegin&&ec->myNet!=0&&myRunThread->simulationIsStartable()) {
        onCmdStart(0, 0, 0);
    }
    update();
}


void
GUIApplicationWindow::handleEvent_SimulationStep(GUIEvent *) {
    updateChildren();
    std::ostringstream str;
    str << (int) myRunThread->getNet().getCurrentTimeStep();
    myLCDLabel->setText(str.str().c_str());
    update();
}


void
GUIApplicationWindow::handleEvent_Message(GUIEvent *e) {
    GUIEvent_Message *ec =
        static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendText(ec->getOwnType(), ec->getMsg());
}


void
GUIApplicationWindow::handleEvent_SimulationEnded(GUIEvent *e) {
    GUIEvent_SimulationEnded *ec =
        static_cast<GUIEvent_SimulationEnded*>(e);
    if (!gQuitOnEnd) {
        // build the text
        std::stringstream text;
        text << "The simulation has ended at time step "
        << ec->getTimeStep() << ".\n";
        switch (ec->getReason()) {
        case GUIEvent_SimulationEnded::ER_NO_VEHICLES:
            text << "Reason: All vehicles have left the simulation.";
            break;
        case GUIEvent_SimulationEnded::ER_END_STEP_REACHED:
            text << "Reason: The final simulation step has been reached.";
            break;
        case GUIEvent_SimulationEnded::ER_ERROR_IN_SIM:
            text << "Reason: An error occured (see log).";
            break;
        default:
            text << "Unknown reason!";
            break;
        }
        onCmdStop(0, 0, 0);
        FXMessageBox::warning(this, MBOX_OK, "Simulation Ended",
                              text.str().c_str());
    } else {
        onCmdStop(0, 0, 0);
    }
    if (gQuitOnEnd) {
        closeAllWindows();
        getApp()->exit(ec->getReason() == GUIEvent_SimulationEnded::ER_ERROR_IN_SIM);
    }
}



void
GUIApplicationWindow::load(const std::string &file, bool isNet, bool isReload) {
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    if (isReload) {
        myLoadThread->start();
        setStatusBarText("Reloading.");
    } else {
        myLoadThread->load(file, isNet);
        setStatusBarText("Loading '" + file + "'.");
    }
    update();
}



GUISUMOAbstractView*
GUIApplicationWindow::openNewView() {
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 0;
    }
    std::string caption = "View #" + toString(myViewNumber++);
    FXuint opts = MDI_TRACKING;
    GUISUMOViewParent* w = 0;
    GUISUMOAbstractView* v = 0;
    if (myMDIClient->numChildren()==0) {
        w = new GUISUMOViewParent(myMDIClient, 0,
                                  myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
                                  this, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
        v = w->init(0, myRunThread->getNet());
    } else {
        w = new GUISUMOViewParent(myMDIClient, getBuildGLCanvas(),
                                  myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
                                  this, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
        v = w->init(getBuildGLCanvas(), myRunThread->getNet());
    }
    w->create();
    if (myMDIClient->numChildren()==1) {
        w->maximize();
    } else {
        myMDIClient->vertical(true);
    }
    myMDIClient->setActiveChild(w);
    return v;
}


FXGLCanvas *
GUIApplicationWindow::getBuildGLCanvas() const {
    if (myMDIClient->numChildren()==0) {
        return 0;
    }
    GUISUMOViewParent *share_tmp1 =
        static_cast<GUISUMOViewParent*>(myMDIClient->childAtIndex(0));
    return share_tmp1->getBuildGLCanvas();
}


void
GUIApplicationWindow::closeAllWindows() {
    myTrackerLock.lock();
    myLCDLabel->setText("-----------");
    // remove trackers and other external windows
    size_t i;
    for (i=0; i<mySubWindows.size(); ++i) {
        mySubWindows[i]->destroy();
    }
    for (i=0; i<myTrackerWindows.size(); ++i) {
        myTrackerWindows[i]->destroy();
    }
    // delete the simulation
    myRunThread->deleteSim();
    // reset the caption
    setTitle(MFXUtils::getTitleText(("SUMO " + std::string(VERSION_STRING)).c_str()));
    // delete other children
    while (myTrackerWindows.size()!=0) {
        delete myTrackerWindows[0];
    }
    while (mySubWindows.size()!=0) {
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
    update();
}


FXCursor *
GUIApplicationWindow::getDefaultCursor() {
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


SUMOTime
GUIApplicationWindow::getCurrentSimTime() const {
    return myRunThread->getNet().getCurrentTimeStep();
}


void
GUIApplicationWindow::loadOnStartup(const std::string &config, bool run) {
    myRunAtBegin = run;
    load(config, false);
}
/*
long
GUIApplicationWindow::onCmdCutSwell(FXObject*, FXSelector, void*)
{
    /
    GUIDialog_CutSwell *about =
        new GUIDialog_CutSwell(this, "Simulating...", 0, 0);
    about->create();
    about->show(PLACEMENT_OWNER);
    /
    string prev = myStatusbar->getStatusLine()->getText().text();
    string text = string("Computing strategies.");
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
    for(int i=0; i<100; ++i) {
        fxsleep(3000);
        myProgressBar->setProgress(i);
    }
//    delete about;
    myProgressBar->setProgress(0);
    myStatusbar->getStatusLine()->setText(prev.c_str());
    myStatusbar->getStatusLine()->setNormalText(prev.c_str());
    return 1;
}
*/


void
GUIApplicationWindow::setStatusBarText(const std::string &text) {
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
}


bool
GUIApplicationWindow::loadSelection(const std::string &file, std::string &msg) throw() {
    return GUISelectionLoader::loadSelection(file, msg);
}


/****************************************************************************/

