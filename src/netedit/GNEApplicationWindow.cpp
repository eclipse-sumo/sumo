/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEApplicationWindow.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The main window of Netedit (adapted from GUIApplicationWindow)
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

#include <utils/common/ToString.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netimport/NIFrame.h>
#include <netbuild/NBFrame.h>
#include <netwrite/NWFrame.h>
#include <utils/common/SystemFrame.h>
#include <netimport/NIImporter_SUMO.h>

#include "GNEApplicationWindow.h"
#include "GNELoadThread.h"
#include "GNEEvent_NetworkLoaded.h"
#include "GNEViewParent.h"
#include "GNEViewNet.h"
#include "GNEDialog_About.h"
#include "GNEDialog_Wizard.h"
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNEPOI.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GNEApplicationWindow) GNEApplicationWindowMap[] = {
    // quit calls
    FXMAPFUNC(SEL_COMMAND,  MID_QUIT,                                       GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_QUIT,                                       GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,                                     GNEApplicationWindow::onCmdQuit),

    // toolbar file
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_NEWNETWORK,                 GNEApplicationWindow::onCmdNewNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_NEWNETWORK,                 GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_NETWORK,                               GNEApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,                               GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,                                GNEApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,                                GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_OPENFOREIGN,                GNEApplicationWindow::onCmdOpenForeign),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_OPENFOREIGN,                GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_SHAPES,                                GNEApplicationWindow::onCmdOpenShapes),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_SHAPES,                                GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_ADDITIONALS,                           GNEApplicationWindow::onCmdOpenAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_ADDITIONALS,                           GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,                                 GNEApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,                                 GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_RELOAD,                                     GNEApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_RELOAD,                                     GNEApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVENETWORK,                GNEApplicationWindow::onCmdSaveNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVENETWORK,                GNEApplicationWindow::onUpdSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVENETWORK_AS,             GNEApplicationWindow::onCmdSaveAsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVENETWORK_AS,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEPLAINXML,               GNEApplicationWindow::onCmdSaveAsPlainXML),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEPLAINXML,               GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEJOINED,                 GNEApplicationWindow::onCmdSaveJoined),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEJOINED,                 GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVESHAPES,                 GNEApplicationWindow::onCmdSaveShapes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVESHAPES_AS,              GNEApplicationWindow::onCmdSaveShapesAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVESHAPES_AS,              GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEADDITIONALS,            GNEApplicationWindow::onCmdSaveAdditionals),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,         GNEApplicationWindow::onCmdSaveAdditionalsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE,                                      GNEApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   MID_CLOSE,                                      GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar edit
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_CREATE_EDGE,                    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_CREATE_EDGE,                    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_MOVE,                           GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_MOVE,                           GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_DELETE,                         GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_DELETE,                         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_INSPECT,                        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_INSPECT,                        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_SELECT,                         GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_SELECT,                         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_CONNECT,                        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_CONNECT,                        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_TLS,                            GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_TLS,                            GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_ADDITIONAL,                     GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_ADDITIONAL,                     GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_CROSSING,                       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_CROSSING,                       GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETMODE_POLYGON,                        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SETMODE_POLYGON,                        GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar processing
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PROCESSING_COMPUTEJUNCTIONS,            GNEApplicationWindow::onCmdComputeJunctions),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_PROCESSING_COMPUTEJUNCTIONS,            GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PROCESSING_COMPUTEJUNCTIONS_VOLATILE,   GNEApplicationWindow::onCmdComputeJunctionsVolatile),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_PROCESSING_COMPUTEJUNCTIONS_VOLATILE,   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PROCESSING_CLEANJUNCTIONS,              GNEApplicationWindow::onCmdCleanJunctions),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_PROCESSING_CLEANJUNCTIONS,              GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PROCESSING_JOINJUNCTIONS,               GNEApplicationWindow::onCmdJoinJunctions),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_PROCESSING_JOINJUNCTIONS,               GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PROCESSING_CLEANINVALIDCROSSINGS,       GNEApplicationWindow::onCmdCleanInvalidCrossings),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_PROCESSING_CLEANINVALIDCROSSINGS,       GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_PROCESSING_OPTIONS,                     GNEApplicationWindow::onCmdOptions),

    // Toolbar locate
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,                             GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEJUNCTION,                             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,                                 GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEEDGE,                                 GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,                                  GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATETLS,                                  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,                                  GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEADD,                                  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,                                  GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOI,                                  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,                                 GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOLY,                                 GNEApplicationWindow::onUpdNeedsNetwork),

    // toolbar windows
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW,                         GNEApplicationWindow::onCmdClearMsgWindow),

    // toolbar help
    FXMAPFUNC(SEL_COMMAND,  MID_ABOUT,                                      GNEApplicationWindow::onCmdAbout),

    // key events
    FXMAPFUNC(SEL_KEYPRESS,     0,                                          GNEApplicationWindow::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                                          GNEApplicationWindow::onKeyRelease),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_ESC,                                  GNEApplicationWindow::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_DEL,                             GNEApplicationWindow::onCmdDel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_ENTER,                           GNEApplicationWindow::onCmdEnter),

    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT,                  GNEApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,       ID_LOADTHREAD_EVENT,                  GNEApplicationWindow::onLoadThreadEvent),

    // Other
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_FOCUSFRAME,                            GNEApplicationWindow::onCmdFocusFrame),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNEApplicationWindow::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWPORT,                               GNEApplicationWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0,                                     GNEApplicationWindow::onClipboardRequest),
};

// Object implementation
FXIMPLEMENT(GNEApplicationWindow, FXMainWindow, GNEApplicationWindowMap, ARRAYNUMBER(GNEApplicationWindowMap))

// ===========================================================================
// member method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif
GNEApplicationWindow::GNEApplicationWindow(FXApp* a, const std::string& configPattern) :
    GUIMainWindow(a),
    myLoadThread(0),
    myAmLoading(false),
    myRecentNets(a, "nets"),
    myConfigPattern(configPattern),
    hadDependentBuild(false),
    myNet(0),
    myUndoList(new GNEUndoList(this)),
    myTitlePrefix("NETEDIT " VERSION_STRING) {
    // init icons
    GUIIconSubSys::initIcons(a);
    // init Textures
    GUITextureSubSys::initTextures(a);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


void
GNEApplicationWindow::dependentBuild() {
    // do this not twice
    if (hadDependentBuild) {
        WRITE_ERROR("DEBUG: GNEApplicationWindow::dependentBuild called twice");
        return;
    }
    hadDependentBuild = true;

    setTarget(this);
    setSelector(MID_WINDOW);

    // build menu bar
    myMenuBarDrag = new FXToolBarShell(this, GUIDesignToolBarShell3);
    myMenuBar = new FXMenuBar(myTopDock, myMenuBarDrag, GUIDesignBar);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build the thread - io
    myLoadThreadEvent.setTarget(this),
                                myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);

    // build the status bar
    myStatusbar = new FXStatusBar(this, GUIDesignStatusBar);
    {
        myGeoFrame =
            new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myGeoCoordinate = new FXLabel(myGeoFrame, "N/A\t\tOriginal coordinate (before coordinate transformation in NETCONVERT)", 0, LAYOUT_CENTER_Y);
        myCartesianFrame =
            new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myCartesianCoordinate = new FXLabel(myCartesianFrame, "N/A\t\tNetwork coordinate", 0, LAYOUT_CENTER_Y);
    }
    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this, GUIDesignSplitter | SPLITTER_VERTICAL | SPLITTER_REVERSED);
    myMDIClient = new FXMDIClient(myMainSplitter, GUIDesignSplitterMDI);
    myMDIMenu = new FXMDIMenu(this, myMDIClient);
    // Due netedit only have a view, this buttons must be disabled (see #2807)
    //new FXMDIWindowButton(myMenuBar, myMDIMenu, myMDIClient, FXMDIClient::ID_MDI_MENUWINDOW, GUIDesignMDIButtonLeft);
    //new FXMDIDeleteButton(myMenuBar, myMDIClient, FXMDIClient::ID_MDI_MENUCLOSE, GUIDesignMDIButtonRight);
    //new FXMDIRestoreButton(myMenuBar, myMDIClient, FXMDIClient::ID_MDI_MENURESTORE, GUIDesignMDIButtonRight);
    //new FXMDIMinimizeButton(myMenuBar, myMDIClient, FXMDIClient::ID_MDI_MENUMINIMIZE, GUIDesignMDIButtonRight);

    // build the message window
    myMessageWindow = new GUIMessageWindow(myMainSplitter);
    myMainSplitter->setSplit(1, 65);
    // fill menu and tool bar
    fillMenuBar();
    // build additional threads
    myLoadThread = new GNELoadThread(getApp(), this, myEvents, myLoadThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");
    // set the caption
    setTitle(myTitlePrefix);

    setIcon(GUIIconSubSys::getIcon(ICON_NETEDIT));

    //initialize some hotkeys
    getAccelTable()->addAccel(parseAccel("e"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_CREATE_EDGE));
    getAccelTable()->addAccel(parseAccel("m"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_MOVE));
    getAccelTable()->addAccel(parseAccel("d"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_DELETE));
    getAccelTable()->addAccel(parseAccel("i"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_INSPECT));
    getAccelTable()->addAccel(parseAccel("s"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_SELECT));
    getAccelTable()->addAccel(parseAccel("c"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_CONNECT));
    getAccelTable()->addAccel(parseAccel("t"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_TLS));
    getAccelTable()->addAccel(parseAccel("a"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_ADDITIONAL));
    getAccelTable()->addAccel(parseAccel("r"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_CROSSING));
    getAccelTable()->addAccel(parseAccel("p"), this, FXSEL(SEL_COMMAND, MID_GNE_SETMODE_POLYGON));
    getAccelTable()->addAccel(parseAccel("Esc"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_ESC));
    getAccelTable()->addAccel(parseAccel("Del"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_DEL));
    getAccelTable()->addAccel(parseAccel("Enter"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_ENTER));
    getAccelTable()->addAccel(parseAccel("F12"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_FOCUSFRAME));
    getAccelTable()->addAccel(parseAccel("v"), this, FXSEL(SEL_COMMAND, MID_EDITVIEWPORT));
}

void
GNEApplicationWindow::create() {
    setWindowSizeAndPos();
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS", "basedir", "");
    FXMainWindow::create();
    myMenuBarDrag->create();
    myFileMenu->create();
    myEditMenu->create();
    //mySettingsMenu->create();
    myWindowsMenu->create();
    myHelpMenu->create();

    FXint textWidth = getApp()->getNormalFont()->getTextWidth("8", 1) * 22;
    myCartesianFrame->setWidth(textWidth);
    myGeoFrame->setWidth(textWidth);

    show(PLACEMENT_DEFAULT);
    if (!OptionsCont::getOptions().isSet("window-size")) {
        if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 1) {
            maximize();
        }
    }

}


GNEApplicationWindow::~GNEApplicationWindow() {
    closeAllWindows();
    // Close icons
    GUIIconSubSys::close();
    // Close gifs (Textures)
    GUITextureSubSys::close();
    delete myGLVisual;
    // must delete menus to avoid segfault on removing accelerators
    // (http://www.fox-toolkit.net/faq#TOC-What-happens-when-the-application-s)
    delete myFileMenu;
    delete myEditMenu;
    delete myLocatorMenu;
    delete myProcessingMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

    delete myLoadThread;

    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        delete e;
    }
    delete myUndoList;
}


void
GNEApplicationWindow::detach() {
    FXMainWindow::detach();
    myMenuBarDrag->detach();
}


void
GNEApplicationWindow::fillMenuBar() {
    // build file menu
    myFileMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&File", 0, myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&New Network...\tCtrl+N\tCreate a new network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_GNE_TOOLBARFILE_NEWNETWORK);
    new FXMenuCommand(myFileMenu,
                      "&Open Network...\tCtrl+O\tOpen a SUMO network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_OPEN_NETWORK);
    new FXMenuCommand(myFileMenu,
                      "Open Configura&tion...\tCtrl+T\tOpen a NETCONVERT configuration file.",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG);
    new FXMenuCommand(myFileMenu,
                      "Import &Foreign Network...\t\tImport a foreign network such as OSM.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), this, MID_GNE_TOOLBARFILE_OPENFOREIGN);
    new FXMenuCommand(myFileMenu,
                      "Load S&hapes...\tCtrl+P\tLoad shapes into the network view.",
                      GUIIconSubSys::getIcon(ICON_OPEN_SHAPES), this, MID_OPEN_SHAPES);
    new FXMenuCommand(myFileMenu,
                      "Load A&dditionals...\tCtrl+D\tLoad additional elements.",
                      GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), this, MID_OPEN_ADDITIONALS);
    new FXMenuCommand(myFileMenu,
                      "&Reload\tCtrl+R\tReloads the network.",
                      GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD);
    new FXMenuCommand(myFileMenu,
                      "&Save Network...\tCtrl+S\tSave the network.",
                      GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVENETWORK);
    new FXMenuCommand(myFileMenu,
                      "Save Net&work As...\tCtrl+Shift+S\tSave the network in another file.",
                      GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVENETWORK_AS);
    new FXMenuCommand(myFileMenu,
                      "Save plain XM&L...\tCtrl+L\tSave plain xml representation the network.",
                      GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVEPLAINXML);
    new FXMenuCommand(myFileMenu,
                      "Save &joined junctions...\tCtrl+J\tSave log of joined junctions (allows reproduction of joins).",
                      GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVEJOINED);
    mySaveShapesMenuCommand = new FXMenuCommand(myFileMenu,
            "Save Shapes\tCtrl+Shift+P\tSave shapes elements.",
            GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVESHAPES);
    mySaveShapesMenuCommand->disable();
    new FXMenuCommand(myFileMenu,
                      "Save Shapes As...\t\tSave shapes elements in another files.",
                      GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVESHAPES_AS);
    mySaveAdditionalsMenuCommand = new FXMenuCommand(myFileMenu,
            "Save Additionals\tCtrl+Shift+D\tSave additional elements.",
            GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVEADDITIONALS);
    mySaveAdditionalsMenuCommand->disable();
    new FXMenuCommand(myFileMenu,
                      "Save Additionals As...\t\tSave additional elements in another file.",
                      GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "Close\tCtrl+W\tClose the net&work.",
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
    new FXMenuCommand(myFileMenu, "Clear Recent Configurat&ions", 0, &myRecentConfigs, FXRecentFiles::ID_CLEAR);
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
    new FXMenuCommand(myFileMenu, "Cl&ear Recent Networks", 0, &myRecentNets, FXRecentFiles::ID_CLEAR);
    myRecentNets.setTarget(this);
    myRecentNets.setSelector(MID_RECENTFILE);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Quit\tCtrl+Q\tQuit the Application.",
                      0, this, MID_QUIT, 0);

    // build edit menu
    myEditMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Edit", 0, myEditMenu);

    // build undo/redo command
    new FXMenuCommand(myEditMenu,
                      "&Undo\tCtrl+Z\tUndo the last change.",
                      GUIIconSubSys::getIcon(ICON_UNDO), myUndoList, FXUndoList::ID_UNDO);
    new FXMenuCommand(myEditMenu,
                      "&Redo\tCtrl+Y\tRedo the last change.",
                      GUIIconSubSys::getIcon(ICON_REDO), myUndoList, FXUndoList::ID_REDO);

    new FXMenuSeparator(myEditMenu);

    // build modes command
    new FXMenuCommand(myEditMenu,
                      "&Edge mode\tE\tCreate junction and edges.",
                      GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), this, MID_GNE_SETMODE_CREATE_EDGE);
    new FXMenuCommand(myEditMenu,
                      "&Move mode\tM\tMove elements.",
                      GUIIconSubSys::getIcon(ICON_MODEMOVE), this, MID_GNE_SETMODE_MOVE);
    new FXMenuCommand(myEditMenu,
                      "&Delete mode\tD\tDelete elements.",
                      GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_SETMODE_DELETE);
    new FXMenuCommand(myEditMenu,
                      "&Inspect mode\tI\tInspect elements and change their attributes.",
                      GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_SETMODE_INSPECT);
    new FXMenuCommand(myEditMenu,
                      "&Select mode\tS\tSelect elements.",
                      GUIIconSubSys::getIcon(ICON_MODESELECT), this, MID_GNE_SETMODE_SELECT);
    new FXMenuCommand(myEditMenu,
                      "&Connection mode\tC\tEdit connections between lanes.",
                      GUIIconSubSys::getIcon(ICON_MODECONNECTION), this, MID_GNE_SETMODE_CONNECT);
    new FXMenuCommand(myEditMenu,
                      "&Traffic light mode\tT\tEdit traffic lights over junctions.",
                      GUIIconSubSys::getIcon(ICON_MODETLS), this, MID_GNE_SETMODE_TLS);
    new FXMenuCommand(myEditMenu,
                      "&Additional mode\tA\tCreate additional elements.",
                      GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), this, MID_GNE_SETMODE_ADDITIONAL);
    new FXMenuCommand(myEditMenu,
                      "C&rossing mode\tR\tCreate crossings between edges.",
                      GUIIconSubSys::getIcon(ICON_MODECROSSING), this, MID_GNE_SETMODE_CROSSING);
    new FXMenuCommand(myEditMenu,
                      "&POI-Poly mode\tP\tCreate Points-Of-Interest and polygons.",
                      GUIIconSubSys::getIcon(ICON_MODEPOLYGON), this, MID_GNE_SETMODE_POLYGON);

    /*
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Edit Breakpoints...\t\tOpens a Dialog for editing breakpoints.",
                      0, this, MID_EDIT_BREAKPOINTS);
    */

    // processing menu (trigger netbuild computations)
    myProcessingMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Processing", 0, myProcessingMenu);
    new FXMenuCommand(myProcessingMenu,
                      "Compute Junctions\tF5\tComputes junction shape and logic.",
                      GUIIconSubSys::getIcon(ICON_COMPUTEJUNCTIONS), this, MID_GNE_PROCESSING_COMPUTEJUNCTIONS);
    new FXMenuCommand(myProcessingMenu,
                      "Compute Junctions with volatile options\tShift+F5\tComputes junction shape and logic using volatile junctions.",
                      GUIIconSubSys::getIcon(ICON_COMPUTEJUNCTIONS), this, MID_GNE_PROCESSING_COMPUTEJUNCTIONS_VOLATILE);
    new FXMenuCommand(myProcessingMenu,
                      "Clean Junctions\tF6\tRemoves solitary junctions.",
                      GUIIconSubSys::getIcon(ICON_CLEANJUNCTIONS), this, MID_GNE_PROCESSING_CLEANJUNCTIONS);
    new FXMenuCommand(myProcessingMenu,
                      "Join Selected Junctions\tF7\tJoins selected junctions into a single junction.",
                      GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), this, MID_GNE_PROCESSING_JOINJUNCTIONS);
    new FXMenuCommand(myProcessingMenu,
                      "Clean invalid crossings\tF8\tClear invalid crossings.",
                      GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), this, MID_GNE_PROCESSING_CLEANINVALIDCROSSINGS);
    new FXMenuCommand(myProcessingMenu,
                      "Options\tF10\t\tConfigure Processing Options.",
                      GUIIconSubSys::getIcon(ICON_OPTIONS), this, MID_GNE_PROCESSING_OPTIONS);
    // build settings menu
    /*
    mySettingsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Settings",0,mySettingsMenu);
    new FXMenuCheck(mySettingsMenu,
                    "Gaming Mode\t\tToggle gaming mode on/off.",
                    this,MID_GAMING);
    */
    // build Locate menu
    myLocatorMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Locate", NULL, myLocatorMenu);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Junctions\tShift+J\tOpen a Dialog for Locating a Junction.",
                      GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), this, MID_LOCATEJUNCTION);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Edges\tShift+E\tOpen a Dialog for Locating an Edge.",
                      GUIIconSubSys::getIcon(ICON_LOCATEEDGE), this, MID_LOCATEEDGE);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &TLS\tShift+T\tOpen a Dialog for Locating a Traffic Light.",
                      GUIIconSubSys::getIcon(ICON_LOCATETLS), this, MID_LOCATETLS);
    new FXMenuCommand(myLocatorMenu,
                      "Locate &Additional\tShift+A\tOpen a Dialog for Locating an Additional Structure.",
                      GUIIconSubSys::getIcon(ICON_LOCATEADD), this, MID_LOCATEADD);
    new FXMenuCommand(myLocatorMenu,
                      "Locate P&oI\tShift+O\tOpen a Dialog for Locating a Point of Intereset.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPOI), this, MID_LOCATEPOI);
    new FXMenuCommand(myLocatorMenu,
                      "Locate Po&lygon\tShift+L\tOpen a Dialog for Locating a Polygon.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPOLY), this, MID_LOCATEPOLY);
    // build windows menu
    myWindowsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Windows", 0, myWindowsMenu);
    new FXMenuCheck(myWindowsMenu,
                    "&Show Status Line\t\tToggle this Status Bar on/off.",
                    myStatusbar, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show &Message Window\t\tToggle the Message Window on/off.",
                    myMessageWindow, FXWindow::ID_TOGGLESHOWN);
    /*
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
    new FXMenuCommand(myWindowsMenu,"&Close",0,
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
    */
    new FXMenuCommand(myWindowsMenu,
                      "&Clear Message Window\t\tClear the message window.",
                      0, this, MID_CLEARMESSAGEWINDOW);

    // build help menu
    myHelpMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,
                    "&Help",
                    0, myHelpMenu);
    new FXMenuCommand(myHelpMenu,
                      "&Online Documentation\tF1\tOpen Online documentation.",
                      0, this, MID_HELP);
    new FXMenuCommand(myHelpMenu,
                      "&About\tF2\tAbout netedit.",
                      0, this, MID_ABOUT);
}


long
GNEApplicationWindow::onCmdQuit(FXObject*, FXSelector, void*) {
    if (continueWithUnsavedChanges()) {
        storeWindowSizeAndPos();
        getApp()->reg().writeStringEntry("SETTINGS", "basedir", gCurrentFolder.text());
        if (isMaximized()) {
            getApp()->reg().writeIntEntry("SETTINGS", "maximized", 1);
        } else {
            getApp()->reg().writeIntEntry("SETTINGS", "maximized", 0);
        }
        getApp()->exit(0);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEditChosen(FXObject*, FXSelector, void*) {
    GUIDialog_GLChosenEditor* chooser =
        new GUIDialog_GLChosenEditor(this, &gSelected);
    chooser->create();
    chooser->show();
    return 1;
}


long
GNEApplicationWindow::onCmdNewNetwork(FXObject*, FXSelector, void*) {
    // ask before we clobber options
    if (!continueWithUnsavedChanges()) {
        return 1;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    GNELoadThread::fillOptions(oc);
    GNELoadThread::setDefaultOptions(oc);
    loadConfigOrNet("", true, false, true, true);
    return 1;
}


long
GNEApplicationWindow::onCmdOpenConfiguration(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open Netconvert Configuration");
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
GNEApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
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
        // when a net is loaded, save additional and shapes are disabled
        mySaveAdditionalsMenuCommand->disable();
        mySaveShapesMenuCommand->disable();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenForeign(FXObject*, FXSelector, void*) {
    // ask before we clobber options
    if (!continueWithUnsavedChanges()) {
        return 1;
    }
    // get the new file name
    FXFileDialog opendialog(this, "Import Foreign Network");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    FXString osmPattern("OSM net (*.osm.xml,*.osm)");
    opendialog.setPatternText(0, osmPattern);
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();

        OptionsCont& oc = OptionsCont::getOptions();
        GNELoadThread::fillOptions(oc);
        if (osmPattern.contains(opendialog.getPattern())) {
            oc.set("osm-files", file);
            oc.set("ramps.guess", "true");
            oc.set("tls.guess", "true");
        } else {
            throw ProcessError("Attempted to import unknown file format '" + file + "'.");
        }

        GNEDialog_Wizard* wizard =
            new GNEDialog_Wizard(this, "Select Import Options", getWidth(), getHeight());

        if (wizard->execute()) {
            NIFrame::checkOptions(); // needed to set projection parameters
            loadConfigOrNet(file, false, false, false);
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenShapes(FXObject*, FXSelector, void*) {
    // get the shape file name
    FXFileDialog opendialog(this, "Open Shapes file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Shape files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        GNEShapeHandler handler(file, myNet);
        myUndoList->p_begin("Loading shapes from '" + file + "'");
        if (!XMLSubSys::runParser(handler, file, false)) {
            WRITE_MESSAGE("Loading of shapes failed.");
        }
        update();
        myUndoList->p_end();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenAdditionals(FXObject*, FXSelector, void*) {
    // get the shape file name
    FXFileDialog opendialog(this, "Open Additionals file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Additional files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // Start operation for undo/redo
        myUndoList->p_begin("load additionals");
        // Create additional handler
        GNEAdditionalHandler additionalHandler(file, myNet->getViewNet());
        // Run parser
        myUndoList->p_begin("Loading additionals from '" + file + "'");
        if (!XMLSubSys::runParser(additionalHandler, file, false)) {
            WRITE_MESSAGE("Loading of " + file + " failed.");
            // Abort undo/redo
            myUndoList->abort();
        } else {
            // commit undo/redo operation
            myUndoList->p_end();
            update();
        }
        myUndoList->p_end();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenRecent(FXObject* sender, FXSelector, void* fileData) {
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    }
    std::string file((const char*)fileData);
    loadConfigOrNet(file, sender == &myRecentNets);
    return 1;
}


long
GNEApplicationWindow::onCmdReload(FXObject*, FXSelector, void*) {
    // @note. If another network has been load during this session, it might not be desirable to set useStartupOptions
    loadConfigOrNet(OptionsCont::getOptions().getString("sumo-net-file"), true, true);
    return 1;
}


long
GNEApplicationWindow::onCmdClose(FXObject*, FXSelector, void*) {
    if (continueWithUnsavedChanges()) {
        closeAllWindows();
        // disable save additionals and shapes menu
        mySaveAdditionalsMenuCommand->disable();
        mySaveShapesMenuCommand->disable();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdLocate(FXObject*, FXSelector sel, void*) {
    if (myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            w->onCmdLocate(0, sel, 0);
        }
    }
    return 1;
}

long
GNEApplicationWindow::onUpdOpen(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myAmLoading ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), 0);
    return 1;
}


long
GNEApplicationWindow::onCmdClearMsgWindow(FXObject*, FXSelector, void*) {
    myMessageWindow->clear();
    return 1;
}


long
GNEApplicationWindow::onCmdAbout(FXObject*, FXSelector, void*) {
    // write warning if netedit is running in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Opening about dialog");
    }
    // create and open about dialog
    GNEDialog_About* about = new GNEDialog_About(this);
    about->create();
    about->show(PLACEMENT_OWNER);
    // write warning if netedit is running in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Closed about dialog");
    }
    return 1;
}


long GNEApplicationWindow::onClipboardRequest(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXString string = GUIUserIO::clipped.c_str();
    setDNDData(FROM_CLIPBOARD, event->target, string);
    return 1;
}


long
GNEApplicationWindow::onLoadThreadEvent(FXObject*, FXSelector, void*) {
    eventOccured();
    return 1;
}


void
GNEApplicationWindow::eventOccured() {
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
            case EVENT_SIMULATION_LOADED:
                handleEvent_NetworkLoaded(e);
                break;
            case EVENT_MESSAGE_OCCURED:
            case EVENT_WARNING_OCCURED:
            case EVENT_ERROR_OCCURED:
                handleEvent_Message(e);
                break;
            default:
                break;
        }
        delete e;
    }
}


void
GNEApplicationWindow::handleEvent_NetworkLoaded(GUIEvent* e) {
    myAmLoading = false;
    GNEEvent_NetworkLoaded* ec = static_cast<GNEEvent_NetworkLoaded*>(e);
    // check whether the loading was successfull
    if (ec->myNet == 0) {
        // report failure
        setStatusBarText("Loading of '" + ec->myFile + "' failed!");
    } else {
        myNet = ec->myNet;
        // report success
        setStatusBarText("'" + ec->myFile + "' loaded.");
        setWindowSizeAndPos();
        // initialise views
        myViewNumber = 0;
        GUISUMOAbstractView* view = openNewView();
        if (view && ec->mySettingsFile != "") {
            GUISettingsHandler settings(ec->mySettingsFile, true, true);
            std::string settingsName = settings.addSettings(view);
            view->addDecals(settings.getDecals());
            settings.applyViewport(view);
            settings.setSnapshots(view);
        }
        // set network name on the caption
        setTitle(MFXUtils::getTitleText(myTitlePrefix, ec->myFile.c_str()));
        getView()->setEditModeFromHotkey(MID_GNE_SETMODE_INSPECT);
        if (ec->myViewportFromRegistry) {
            Position off, p;
            off.set(getApp()->reg().readRealEntry("viewport", "x"), getApp()->reg().readRealEntry("viewport", "y"), getApp()->reg().readRealEntry("viewport", "z"));
            getView()->setViewportFromTo(off, p);
        }
    }
    getApp()->endWaitCursor();
    myMessageWindow->registerMsgHandlers();
    // check if additionals has to be loaded at start
    if (OptionsCont::getOptions().isSet("sumo-additionals-file") && myNet) {
        myAdditionalsFile = OptionsCont::getOptions().getString("sumo-additionals-file");
        WRITE_MESSAGE("Loading additionals from '" + myAdditionalsFile + "'");
        GNEAdditionalHandler additionalHandler(myAdditionalsFile, myNet->getViewNet());
        // Run parser
        myUndoList->p_begin("Loading additionals from '" + myAdditionalsFile + "'");
        if (!XMLSubSys::runParser(additionalHandler, myAdditionalsFile, false)) {
            WRITE_ERROR("Loading of " + myAdditionalsFile + " failed.");
        }
        myUndoList->p_end();
    }
    // check if shapes has to be loaded at start
    if (OptionsCont::getOptions().isSet("sumo-shapes-file") && myNet) {
        myShapesFile = OptionsCont::getOptions().getString("sumo-shapes-file");
        WRITE_MESSAGE("Loading shapes");
        GNEShapeHandler shapeHandler(myShapesFile, myNet);
        // Run parser
        myUndoList->p_begin("Loading shapes from '" + myShapesFile + "'");
        if (!XMLSubSys::runParser(shapeHandler, myShapesFile, false)) {
            WRITE_ERROR("Loading of shapes failed.");
        }
        myUndoList->p_end();
    }
    // check if additionals output must be changed
    if (OptionsCont::getOptions().isSet("additionals-output")) {
        myAdditionalsFile = OptionsCont::getOptions().getString("additionals-output");
    }
    // check if shapes output must be changed
    if (OptionsCont::getOptions().isSet("shapes-output")) {
        myShapesFile = OptionsCont::getOptions().getString("shapes-output");
    }

    update();
}


void
GNEApplicationWindow::handleEvent_Message(GUIEvent* e) {
    GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendMsg(ec->getOwnType(), ec->getMsg());
}


void
GNEApplicationWindow::loadConfigOrNet(const std::string file, bool isNet, bool isReload, bool useStartupOptions, bool newNet) {
    if (!continueWithUnsavedChanges()) {
        return;
    }
    storeWindowSizeAndPos();
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    if (isReload) {
        myLoadThread->start();
        setStatusBarText("Reloading.");
    } else {
        gSchemeStorage.saveViewport(0, 0, -1); // recenter view
        myLoadThread->loadConfigOrNet(file, isNet, useStartupOptions, newNet);
        setStatusBarText("Loading '" + file + "'.");
    }
    update();
}



GUISUMOAbstractView*
GNEApplicationWindow::openNewView() {
    std::string caption = "View #" + toString(myViewNumber++);
    FXuint opts = MDI_TRACKING;
    GNEViewParent* viewParent = new GNEViewParent(myMDIClient, myMDIMenu, FXString(caption.c_str()), this, getBuildGLCanvas(),
            myNet, myUndoList, NULL, opts, 10, 10, 300, 200);
    if (myMDIClient->numChildren() == 1) {
        viewParent->maximize();
    } else {
        myMDIClient->vertical(true);
    }
    myMDIClient->setActiveChild(viewParent);
    //v->grabKeyboard();
    return viewParent->getView();
}


FXGLCanvas*
GNEApplicationWindow::getBuildGLCanvas() const {
    if (myMDIClient->numChildren() == 0) {
        return 0;
    }
    GNEViewParent* share_tmp1 =
        static_cast<GNEViewParent*>(myMDIClient->childAtIndex(0));
    return share_tmp1->getBuildGLCanvas();
}


SUMOTime
GNEApplicationWindow::getCurrentSimTime() const {
    return 0;
}


double
GNEApplicationWindow::getTrackerInterval() const {
    return 1;
}


GNEUndoList*
GNEApplicationWindow::getUndoList() {
    return myUndoList;
}


void
GNEApplicationWindow::closeAllWindows() {
    myTrackerLock.lock();
    // remove trackers and other external windows
    for (int i = 0; i < (int)mySubWindows.size(); ++i) {
        mySubWindows[i]->destroy();
    }
    for (int i = 0; i < (int)myTrackerWindows.size(); ++i) {
        myTrackerWindows[i]->destroy();
    }
    // reset the caption
    setTitle(myTitlePrefix);
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

    myUndoList->p_clear();
    if (myNet != 0) {
        delete myNet;
        myNet = 0;
        GeoConvHelper::resetLoaded();
    }
    myMessageWindow->unregisterMsgHandlers();
    // Reset textures
    GUITextureSubSys::resetTextures();
    GLHelper::resetFont();
}


FXCursor*
GNEApplicationWindow::getDefaultCursor() {
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


void
GNEApplicationWindow::loadOptionOnStartup() {
    const OptionsCont& oc = OptionsCont::getOptions();
    loadConfigOrNet("", true, false, true, oc.getBool("new"));
}


void
GNEApplicationWindow::setStatusBarText(const std::string& statusBarText) {
    myStatusbar->getStatusLine()->setText(statusBarText.c_str());
    myStatusbar->getStatusLine()->setNormalText(statusBarText.c_str());
}


void
GNEApplicationWindow::setAdditionalsFile(const std::string& additionalsFile) {
    myAdditionalsFile = additionalsFile;
}


void
GNEApplicationWindow::setShapesFile(const std::string& shapesFile) {
    myShapesFile = shapesFile;
}


void
GNEApplicationWindow::enableSaveAdditionalsMenu() {
    mySaveAdditionalsMenuCommand->enable();
}


void
GNEApplicationWindow::enableSaveShapesMenu() {
    mySaveShapesMenuCommand->enable();
}


long
GNEApplicationWindow::onCmdSetMode(FXObject*, FXSelector sel, void*) {
    if (getView()) {
        getView()->setEditModeFromHotkey(FXSELID(sel));
    }
    return 1;
}


long
GNEApplicationWindow::onCmdAbort(FXObject*, FXSelector, void*) {
    if (getView()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Key ESC (abort) pressed");
        }
        // abort current operation
        getView()->abortOperation();
        getView()->update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdDel(FXObject*, FXSelector, void*) {
    if (getView()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Key DEL (delete) pressed");
        }
        getView()->hotkeyDel();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEnter(FXObject*, FXSelector, void*) {
    if (getView()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Key ENTER pressed");
        }
        getView()->hotkeyEnter();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdFocusFrame(FXObject*, FXSelector, void*) {
    if (getView()) {
        getView()->hotkeyFocusFrame();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEditViewport(FXObject*, FXSelector, void*) {
    if (getView()) {
        getView()->showViewportEditor();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdHelp(FXObject*, FXSelector, void*) {
    FXLinkLabel::fxexecute("http://sumo.dlr.de/wiki/NETEDIT");
    return 1;
}


long
GNEApplicationWindow::onCmdComputeJunctions(FXObject*, FXSelector, void*) {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Key F5 (Compute) pressed");
    }
    myNet->computeEverything(this, true, false);
    updateControls();
    return 1;
}


long
GNEApplicationWindow::onCmdComputeJunctionsVolatile(FXObject*, FXSelector, void*) {
    // declare variable to save FXMessageBox outputs.
    FXuint answer = 0;
    // declare string to save paths in wich additionals and shapes will be saved
    std::string additionalSavePath = myAdditionalsFile;
    std::string shapeSavePath = myShapesFile;
    // write warning if netedit is running in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Keys Shift + F5 (Compute with volatile options) pressed");
        WRITE_WARNING("Opening FXMessageBox 'Volatile Recomputing'");
    }
    // open question dialog box
    answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Recompute with volatile options",
                                    "Changes produced in the net due a recomputing with volatile options cannot be undone. Continue?");
    if (answer != 1) { //1:yes, 2:no, 4:esc
        // write warning if netedit is running in testing mode
        if (answer == 2 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox 'Volatile Recomputing' with 'No'");
        } else if (answer == 4 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox 'Volatile Recomputing' with 'ESC'");
        }
        // abort recompute with volatile options
        return 0;
    } else {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox 'Volatile Recomputing' with 'Yes'");
        }
        // Check if there are additionals in our net
        if (myNet->getNumberOfAdditionals() > 0) {
            // ask user if want to save additionals if weren't saved previously
            if (myAdditionalsFile == "") {
                // write warning if netedit is running in testing mode
                WRITE_WARNING("Opening FXMessageBox 'Save additionals before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save additionals before recomputing with volatile options",
                                                "Would you like to save additionals before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'Save additionals before recomputing' with 'No'");
                    } else if (answer == 4 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'Save additionals before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'Save additionals before recomputing' with 'Yes'");
                    }
                    // Open a dialog to set filename output
                    myAdditionalsFile = MFXUtils::getFilename2Write(this,
                                        "Select name of the additional file", ".xml",
                                        GUIIconSubSys::getIcon(ICON_EMPTY),
                                        gCurrentFolder).text();
                    // set obtanied filename output into additionalSavePath (can be "")
                    additionalSavePath = myAdditionalsFile;
                }
            }
            // Check if additional must be saved in a temporal directory, if user didn't define a directory for additionals
            if (myAdditionalsFile == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                additionalSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpAdditionalsNetedit.xml");
            }
            // Start saving additionals
            getApp()->beginWaitCursor();
            try {
                myNet->saveAdditionals(additionalSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Opening FXMessageBox 'Error saving additionals before recomputing'");
                }
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving additionals in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Closed FXMessageBox 'Error saving additionals before recomputing' with 'OK'");
                }
            }
            // end saving additionals
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
        } else {
            // clear additional path
            additionalSavePath = "";
        }
        // Check if there are shapes in our net
        if (myNet->getNumberOfShapes() > 0) {
            // ask user if want to save shapes if weren't saved previously
            if (myShapesFile == "") {
                // write warning if netedit is running in testing mode
                WRITE_WARNING("Opening FXMessageBox 'Save shapes before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save shapes before recomputing with volatile options",
                                                "Would you like to save shapes before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'Save shapes before recomputing' with 'No'");
                    } else if (answer == 4 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'Save shapes before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'Save shapes before recomputing' with 'Yes'");
                    }
                    // Open a dialog to set filename output
                    myShapesFile = MFXUtils::getFilename2Write(this,
                                   "Select name of the shape file", ".xml",
                                   GUIIconSubSys::getIcon(ICON_EMPTY),
                                   gCurrentFolder).text();
                    // set obtanied filename output into shapeSavePath (can be "")
                    shapeSavePath = myShapesFile;
                }
            }
            // Check if shape must be saved in a temporal directory, if user didn't define a directory for shapes
            if (myShapesFile == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                shapeSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpShapesNetedit.xml");
            }
            // Start saving shapes
            getApp()->beginWaitCursor();
            try {
                myNet->saveShapes(shapeSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Opening FXMessageBox 'Error saving shapes before recomputing'");
                }
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving shapes in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Closed FXMessageBox 'Error saving shapes before recomputing' with 'OK'");
                }
            }
            // end saving shapes
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
        } else {
            // clear save path
            shapeSavePath = "";
        }
        // compute with volatile options
        myNet->computeEverything(this, true, true, additionalSavePath, shapeSavePath);
        updateControls();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdCleanJunctions(FXObject*, FXSelector, void*) {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Key F6 (Clean junction) pressed");
    }
    myNet->removeSolitaryJunctions(myUndoList);
    return 1;
}


long
GNEApplicationWindow::onCmdJoinJunctions(FXObject*, FXSelector, void*) {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Key F7 (Join junctions) pressed");
    }
    myNet->joinSelectedJunctions(myUndoList);
    return 1;
}


long
GNEApplicationWindow::onCmdCleanInvalidCrossings(FXObject*, FXSelector, void*) {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Key F8 (Clean invalid crossings) pressed");
    }
    myNet->cleanInvalidCrossings(myUndoList);
    return 1;
}


long
GNEApplicationWindow::onCmdOptions(FXObject*, FXSelector, void*) {
    GNEDialog_Wizard* wizard =
        new GNEDialog_Wizard(this, "Configure Options", getWidth(), getHeight());

    if (wizard->execute()) {
        NIFrame::checkOptions(); // needed to set projection parameters
        NBFrame::checkOptions();
        NWFrame::checkOptions();
        SystemFrame::checkOptions(); // needed to set precision
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveAsNetwork(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save Network as", ".net.xml",
                    GUIIconSubSys::getIcon(ICON_EMPTY),
                    gCurrentFolder);
    if (file == "") {
        return 1;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    oc.resetWritable();
    oc.set("output-file", file.text());
    setTitle(MFXUtils::getTitleText(myTitlePrefix, file));
    onCmdSaveNetwork(0, 0, 0);
    return 1;
}


long
GNEApplicationWindow::onCmdSaveAsPlainXML(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the plain-xml edge-file (other names will be deduced from this)", "",
                    GUIIconSubSys::getIcon(ICON_EMPTY),
                    gCurrentFolder);
    if (file == "") {
        return 1;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    bool wasSet = oc.isSet("plain-output-prefix");
    std::string oldPrefix = oc.getString("plain-output-prefix");
    oc.resetWritable();
    std::string prefix = file.text();
    // if the name of an edg.xml file was given, remove the suffix
    if (StringUtils::endsWith(prefix, ".edg.xml")) {
        prefix = prefix.substr(0, prefix.size() - 8);
    }
    if (StringUtils::endsWith(prefix, ".")) {
        prefix = prefix.substr(0, prefix.size() - 1);
    }
    oc.set("plain-output-prefix", prefix);
    getApp()->beginWaitCursor();
    try {
        myNet->savePlain(oc);
        myUndoList->unmark();
        myUndoList->mark();
    } catch (IOError& e) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox 'Error saving plainXML'");
        }
        // open message box
        FXMessageBox::error(this, MBOX_OK, "Saving plain xml failed!", "%s", e.what());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox 'Error saving plainXML' with 'OK'");
        }
    }
    myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURED, "Plain XML saved with prefix '" + prefix + "'.\n");
    myMessageWindow->addSeparator();
    if (wasSet) {
        oc.resetWritable();
        oc.set("plain-output-prefix", oldPrefix);
    } else {
        oc.unSet("plain-output-prefix");
    }
    getApp()->endWaitCursor();
    return 1;
}


long
GNEApplicationWindow::onCmdSaveJoined(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the joined-junctions file", ".nod.xml",
                    GUIIconSubSys::getIcon(ICON_EMPTY),
                    gCurrentFolder);
    if (file == "") {
        return 1;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    bool wasSet = oc.isSet("junctions.join-output");
    std::string oldFile = oc.getString("junctions.join-output");
    oc.resetWritable();
    std::string filename = file.text();
    oc.set("junctions.join-output", filename);
    getApp()->beginWaitCursor();
    try {
        myNet->saveJoined(oc);
    } catch (IOError& e) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox 'error saving joined'");
        }
        // opening error message
        FXMessageBox::error(this, MBOX_OK, "Saving joined junctions failed!", "%s", e.what());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox 'error saving joined' with 'OK'");
        }
    }
    myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURED, "Joined junctions saved to '" + filename + "'.\n");
    myMessageWindow->addSeparator();
    if (wasSet) {
        oc.resetWritable();
        oc.set("junctions.join-output", oldFile);
    } else {
        oc.unSet("junctions.join-output");
    }
    getApp()->endWaitCursor();
    return 1;
}


long
GNEApplicationWindow::onCmdSaveShapes(FXObject*, FXSelector, void*) {
    // check if save shapes menu is enabled
    if (mySaveShapesMenuCommand->isEnabled()) {
        // Check if shapes file was already set at start of netedit or with a previous save
        if (myShapesFile == "") {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the shape file", ".xml",
                            GUIIconSubSys::getIcon(ICON_EMPTY),
                            gCurrentFolder);
            if (file == "") {
                // None shapes file was selected, then stop function
                return 0;
            } else {
                myShapesFile = file.text();
            }
        }
        getApp()->beginWaitCursor();
        try {
            myNet->saveShapes(myShapesFile);
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURED, "Shapes saved in " + myShapesFile + ".\n");
            mySaveShapesMenuCommand->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox 'Error saving shapes'");
            }
            // open error dialog box
            FXMessageBox::error(this, MBOX_OK, "Saving POIs failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Error saving shapes' with 'OK'");
            }
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        return 1;
    } else {
        return 0;
    }
}


long GNEApplicationWindow::onCmdSaveShapesAs(FXObject*, FXSelector, void*) {
    // Open window to select shape file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the shape file", ".xml",
                    GUIIconSubSys::getIcon(ICON_EMPTY),
                    gCurrentFolder);
    if (file != "") {
        // Set new shape file
        myShapesFile = file.text();
        // save shapes
        return onCmdSaveShapes(0, 0, 0);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onUpdNeedsNetwork(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myNet == 0 ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), 0);
    return 1;
}


long
GNEApplicationWindow::onUpdReload(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myNet == 0 || !OptionsCont::getOptions().isSet("sumo-net-file") ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), 0);
    return 1;
}


long
GNEApplicationWindow::onCmdSaveNetwork(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    // function onCmdSaveAsNetwork must be executed if this is the first save
    if (oc.getString("output-file") == "") {
        return onCmdSaveAsNetwork(0, 0, 0);
    } else {
        getApp()->beginWaitCursor();
        try {
            myNet->save(oc);
            myUndoList->unmark();
            myUndoList->mark();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox 'error saving network'");
            }
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving Network failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'error saving network' with 'OK'");
            }
        }
        myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURED, "Network saved in " + oc.getString("output-file") + ".\n");
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionals(FXObject*, FXSelector, void*) {
    // check if save additional menu is enabled
    if (mySaveAdditionalsMenuCommand->isEnabled()) {
        // Check if additionals file was already set at start of netedit or with a previous save
        if (myAdditionalsFile == "") {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the additional file", ".xml",
                            GUIIconSubSys::getIcon(ICON_EMPTY),
                            gCurrentFolder);
            if (file == "") {
                // None additionals file was selected, then stop function
                return 0;
            } else {
                myAdditionalsFile = file.text();
            }
        }
        // Start saving additionals
        getApp()->beginWaitCursor();
        try {
            myNet->saveAdditionals(myAdditionalsFile);
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURED, "Additionals saved in " + myAdditionalsFile + ".\n");
            mySaveAdditionalsMenuCommand->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox 'error saving additionals'");
            }
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving additionals failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'error saving additionals' with 'OK'");
            }
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionalsAs(FXObject*, FXSelector, void*) {
    // Open window to select additionasl file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the additional file", ".xml",
                    GUIIconSubSys::getIcon(ICON_EMPTY),
                    gCurrentFolder);
    if (file != "") {
        // Set new additional file
        myAdditionalsFile = file.text();
        // save additionals
        return onCmdSaveAdditionals(0, 0, 0);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onUpdSaveNetwork(FXObject* sender, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    bool enable = myNet != 0 && oc.isSet("output-file");
    sender->handle(this, FXSEL(SEL_COMMAND, enable ? ID_ENABLE : ID_DISABLE), 0);
    if (enable) {
        FXString caption = ("Save " + oc.getString("output-file")).c_str();
        sender->handle(this, FXSEL(SEL_COMMAND, FXMenuCaption::ID_SETSTRINGVALUE), (void*)&caption);
    }
    return 1;
}


GNEViewNet*
GNEApplicationWindow::getView() {
    if (mySubWindows.size() != 0) {
        return (GNEViewNet*)(((GUIGlChildWindow*)mySubWindows[0])->getView());
    } else {
        return 0;
    }
}


bool
GNEApplicationWindow::continueWithUnsavedChanges() {
    FXuint answer = 0;
    if (myUndoList->canUndo() && !myUndoList->marked()) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox 'Confirm closing network'");
        }
        // open question box
        answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                        "Confirm closing Network", "%s",
                                        "You have unsaved changes in the network. Do you wish to quit and discard all changes?");
        // restore focus to view net
        getView()->setFocus();
        // if user close dialog box, check additionasl and shapes
        if (answer == MBOX_CLICKED_QUIT) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Confirm closing network' with 'Quit'");
            }
            if (continueWithUnsavedAdditionalChanges() && continueWithUnsavedShapeChanges()) {
                // clear undo list and return true to continue with closing/reload
                myUndoList->p_clear();
                return true;
            } else {
                return false;
            }
        } else if (answer == MBOX_CLICKED_SAVE) {
            // save newtork
            onCmdSaveNetwork(0, 0, 0);
            if (!myUndoList->marked()) {
                // saving failed
                return false;
            }
            if (continueWithUnsavedAdditionalChanges() && continueWithUnsavedShapeChanges()) {
                // clear undo list and return true to continue with closing/reload
                myUndoList->p_clear();
                return true;
            } else {
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Confirm closing network' with 'No'");
            } else if (answer == 4 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Confirm closing network' with 'ESC'");
            }
            // return false to stop closing/reloading
            return false;
        }
    } else {
        if (continueWithUnsavedAdditionalChanges() && continueWithUnsavedShapeChanges()) {
            // clear undo list and return true to continue with closing/reload
            myUndoList->p_clear(); //only ask once
            return true;
        } else {
            // return false to stop closing/reloading
            return false;
        }
    }
}


bool
GNEApplicationWindow::continueWithUnsavedAdditionalChanges() {
    // Check if there are non saved additionals
    if (mySaveAdditionalsMenuCommand->isEnabled()) {
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox 'Save additionals before exit'");
        }
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               "Save additionals before exit", "%s",
                                               "You have unsaved additionals. Do you wish to quit and discard all changes?");
        // restore focus to view net
        getView()->setFocus();
        // if answer was affirmative, but there was an error during saving additional, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save additionals before exit' with 'Quit'");
            }
            // nothing to save, return true
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save additionals before exit' with 'Yes'");
            }
            if (onCmdSaveAdditionals(0, 0, 0) == 1) {
                // additionals sucesfully saved
                return true;
            } else {
                // error saving additionals, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save additionals before exit' with 'No'");
            } else if (answer == 4 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save additionals before exit' with 'ESC'");
            }
            // abort saving
            return false;
        }
    } else {
        // nothing to save, return true
        return true;
    }
}


bool
GNEApplicationWindow::continueWithUnsavedShapeChanges() {
    // Check if there are non saved additionals
    if (mySaveShapesMenuCommand->isEnabled()) {
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox 'Save shapes before exit'");
        }
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               "Save shapes before exit", "%s",
                                               "You have unsaved shapes. Do you wish to quit and discard all changes?");
        // restore focus to view net
        getView()->setFocus();
        // if answer was affirmative, but there was an error during saving additional, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save shapes before exit' with 'Quit'");
            }
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save shapes before exit' with 'Yes'");
            }
            if (onCmdSaveShapes(0, 0, 0) == 1) {
                // shapes sucesfully saved
                return true;
            } else {
                // error saving shapes, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save shapes before exit' with 'No'");
            } else if (answer == 4 && OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'Save shapes before exit' with 'ESC'");
            }
            // abort saving
            return false;
        }
    } else {
        // nothing to save, then return true
        return true;
    }
}


void
GNEApplicationWindow::updateControls() {
    GNEViewNet* view = getView();
    if (view != 0) {
        view->updateControls();
    }
}


long
GNEApplicationWindow::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    const long handled = FXMainWindow::onKeyPress(o, sel, eventData);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            w->onKeyPress(0, sel, eventData);
        }
    }
    return 0;
}


long
GNEApplicationWindow::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    const long handled = FXMainWindow::onKeyRelease(o, sel, eventData);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != 0) {
            w->onKeyRelease(0, sel, eventData);
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEApplicationWindow::GNEShapeHandler - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::GNEShapeHandler::GNEShapeHandler(const std::string& file, GNENet* net) :
    ShapeHandler(file, *net),
    myNet(net) {}


GNEApplicationWindow::GNEShapeHandler::~GNEShapeHandler() {}


Position
GNEApplicationWindow::GNEShapeHandler::getLanePos(const std::string& poiID, const std::string& laneID, double lanePos, double lanePosLat) {
    std::string edgeID;
    int laneIndex;
    NBHelpers::interpretLaneID(laneID, edgeID, laneIndex);
    NBEdge* edge = myNet->retrieveEdge(edgeID)->getNBEdge();
    if (edge == 0 || laneIndex < 0 || edge->getNumLanes() <= laneIndex) {
        WRITE_ERROR("Lane '" + laneID + "' to place poi '" + poiID + "' on is not known.");
        return Position::INVALID;
    }
    if (lanePos < 0) {
        lanePos = edge->getLength() + lanePos;
    }
    if (lanePos < 0 || lanePos > edge->getLength()) {
        WRITE_WARNING("lane position " + toString(lanePos) + " for poi '" + poiID + "' is not valid.");
    }
    return edge->getLanes()[laneIndex].shape.positionAtOffset(lanePos, -lanePosLat);
}

/****************************************************************************/
