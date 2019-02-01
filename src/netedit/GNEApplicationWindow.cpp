/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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

#include <netbuild/NBFrame.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/dialogs/GNEDialog_About.h>
#include <netedit/frames/GNETAZFrame.h>
#include <netedit/frames/GNETLSEditorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netimport/NIFrame.h>
#include <netwrite/NWFrame.h>
#include <utils/common/SysUtils.h>
#include <utils/common/SystemFrame.h>
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_Options.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEApplicationWindow.h"
#include "GNEEvent_NetworkLoaded.h"
#include "GNELoadThread.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEViewParent.h"

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GNEApplicationWindow) GNEApplicationWindowMap[] = {
    // quit calls
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Q_CLOSE,                        GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_HOTKEY_CTRL_Q_CLOSE,                        GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,                                     GNEApplicationWindow::onCmdQuit),

    // toolbar file
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_N_NEWNETWORK,                   GNEApplicationWindow::onCmdNewNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_N_NEWNETWORK,                   GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_NETWORK,                               GNEApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,                               GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,                                GNEApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,                                GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_OPENFOREIGN,                GNEApplicationWindow::onCmdOpenForeign),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_OPENFOREIGN,                GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_ADDITIONALS,                           GNEApplicationWindow::onCmdOpenAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_ADDITIONALS,                           GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_TLSPROGRAMS,                           GNEApplicationWindow::onCmdOpenTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_TLSPROGRAMS,                           GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_LOADDEMAND,                 GNEApplicationWindow::onCmdOpenDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_LOADDEMAND,                 GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,                                 GNEApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,                                 GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_R_RELOAD,                       GNEApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_R_RELOAD,                       GNEApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_S_SAVENETWORK_STOPSIMULATION,   GNEApplicationWindow::onCmdSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_S_SAVENETWORK_STOPSIMULATION,   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORKAS,          GNEApplicationWindow::onCmdSaveAsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORKAS,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_L_SAVEASPLAINXML,               GNEApplicationWindow::onCmdSaveAsPlainXML),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_L_SAVEASPLAINXML,               GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS,          GNEApplicationWindow::onCmdSaveJoined),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_D_SAVEADDITIONAL,         GNEApplicationWindow::onCmdSaveAdditionals),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,         GNEApplicationWindow::onCmdSaveAdditionalsAs),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_CTRL_SHIFT_K,                    GNEApplicationWindow::onCmdSaveTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_CTRL_SHIFT_K,                    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,         GNEApplicationWindow::onCmdSaveTLSProgramsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_W_CLOSESIMULATION,              GNEApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_W_CLOSESIMULATION,              GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEDEMAND,                 GNEApplicationWindow::onCmdSaveDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEDEMAND,                 GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,              GNEApplicationWindow::onCmdSaveDemandElementsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,              GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar supermode
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F3_SUPERMODE_NETWORK,        GNEApplicationWindow::onCmdSetSuperMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F4_SUPERMODE_DEMAND,         GNEApplicationWindow::onCmdSetSuperMode),

    // Toolbar edit
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_E_EDGEMODE,                      GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_M_MOVEMODE,                      GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_D_DELETEMODE,                    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_I_INSPECTMODE,                   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_S_SELECTMODE,                    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_C_CONNECTMODE,                   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_T_TLSMODE_VTYPEMODE,             GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_A_ADDITIONALMODE,                GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE,        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_Z_TAZMODE,                       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_P_POLYGONMODE,                   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_V_VEHICLEMODE,                   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_W_PROHIBITIONMODE,               GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWSCHEME,                         GNEApplicationWindow::onCmdEditViewScheme),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWPORT,                           GNEApplicationWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID,    GNEApplicationWindow::onCmdToogleGrid),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_C_SHOWCONNECTIONS,    GNEApplicationWindow::onCmdToogleShowConnections),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_C_SHOWCONNECTIONS,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_I_SELECTEDGES,        GNEApplicationWindow::onCmdToogleSelectEdges),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_I_SELECTEDGES,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,          GNEApplicationWindow::onCmdOpenSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,          GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar processing
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F5_RECOMPUTE,                GNEApplicationWindow::onCmdComputeJunctions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F5_RECOMPUTE,                GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F5_RECOMPUTEVOLATILE,  GNEApplicationWindow::onCmdComputeJunctionsVolatile),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_F5_RECOMPUTEVOLATILE,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F6,                      GNEApplicationWindow::onCmdCleanJunctions),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_F6,                      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F7,                      GNEApplicationWindow::onCmdJoinJunctions),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_F7,                      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F8,                      GNEApplicationWindow::onCmdCleanInvalidCrossings),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_F8,                      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F10,                     GNEApplicationWindow::onCmdOptions),

    // Toolbar locate
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,                     GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEJUNCTION,                     GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,                         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEEDGE,                         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,                          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATETLS,                          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,                          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEADD,                          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,                          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOI,                          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,                         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOLY,                         GNEApplicationWindow::onUpdNeedsNetwork),

    // toolbar windows
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW,                 GNEApplicationWindow::onCmdClearMsgWindow),

    // toolbar help
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F1_ONLINEDOCUMENTATION,      GNEApplicationWindow::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F2_ABOUT,                    GNEApplicationWindow::onCmdAbout),

    // key events
    FXMAPFUNC(SEL_KEYPRESS,     0,                                  GNEApplicationWindow::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                                  GNEApplicationWindow::onKeyRelease),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_ESC,                     GNEApplicationWindow::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_DEL,                     GNEApplicationWindow::onCmdDel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_ENTER,                   GNEApplicationWindow::onCmdEnter),

    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT,          GNEApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,       ID_LOADTHREAD_EVENT,          GNEApplicationWindow::onLoadThreadEvent),

    // Other
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0,                             GNEApplicationWindow::onClipboardRequest),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F12,                     GNEApplicationWindow::onCmdFocusFrame),
};

// Object implementation
FXIMPLEMENT(GNEApplicationWindow, FXMainWindow, GNEApplicationWindowMap, ARRAYNUMBER(GNEApplicationWindowMap))


// ===========================================================================
// GNEApplicationWindow::ToolbarsGrip method definitions
// ===========================================================================

GNEApplicationWindow::ToolbarsGrip::ToolbarsGrip(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::ToolbarsGrip::buildMenuToolbarsGrip() {
    // build menu bar (for File, edit, processing...) using specify design
    myToolBarShellMenu = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    menu = new FXMenuBar(myGNEApp->myTopDock, myToolBarShellMenu, GUIDesignToolbarMenuBarNetedit);
    // declare toolbar grip for menu bar
    new FXToolBarGrip(menu, menu, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
}


void
GNEApplicationWindow::ToolbarsGrip::buildViewParentToolbarsGrips() {
    // build menu bar for supermodes (next to menu bar)
    myToolBarShellSuperModes = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    superModes = new FXMenuBar(myGNEApp->myTopDock, myToolBarShellSuperModes, GUIDesignToolBarRaisedSame);
    // declare toolbar grip for menu bar Supermodes
    new FXToolBarGrip(superModes, superModes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for navigation (bot to menu bar)
    myToolBarShellNavigation = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    navigation = new FXMenuBar(myGNEApp->myTopDock, myToolBarShellNavigation, GUIDesignToolBarRaisedNext);
    // declare toolbar grip for menu bar Navigation
    new FXToolBarGrip(navigation, navigation, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for modes
    myToolBarShellModes = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    modes = new FXMenuBar(myGNEApp->myTopDock, myToolBarShellModes, GUIDesignToolBarRaisedSame);
    // declare toolbar grip for menu bar modes
    new FXToolBarGrip(modes, modes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for mode Options
    myToolBarShellModeOptions = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    modeOptions = new FXMenuBar(myGNEApp->myTopDock, myToolBarShellModeOptions, GUIDesignToolBarRaisedSame);
    // declare toolbar grip for menu bar modes
    new FXToolBarGrip(modeOptions, modeOptions, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // create menu bars
    superModes->create();
    navigation->create();
    modes->create();
    modeOptions->create();
    // create shell supermodes
    myToolBarShellSuperModes->create();
    myToolBarShellNavigation->create();
    myToolBarShellModes->create();
    myToolBarShellModeOptions->create();
    // recalc top dop after creating elements
    myGNEApp->myTopDock->recalc();
}


void
GNEApplicationWindow::ToolbarsGrip::destroyParentToolbarsGrips() {
    // delete Menu bars
    delete superModes;
    delete navigation;
    delete modes;
    delete modeOptions;
    // also delete toolbar shells to avoid floating windows
    delete myToolBarShellSuperModes;
    delete myToolBarShellNavigation;
    delete myToolBarShellModes;
    delete myToolBarShellModeOptions;
    // recalc top dop after deleting elements
    myGNEApp->myTopDock->recalc();
}

// ===========================================================================
// GNEApplicationWindow method definitions
// ===========================================================================

GNEApplicationWindow::GNEApplicationWindow(FXApp* a, const std::string& configPattern) :
    GUIMainWindow(a),
    myLoadThread(nullptr),
    myAmLoading(false),
    hadDependentBuild(false),
    myNet(nullptr),
    myUndoList(new GNEUndoList(this)),
    myConfigPattern(configPattern),
    myToolbarsGrip(this),
    myMenuBarFile(this),
    myFileMenuCommands(this),
    myNetworkMenuCommands(this),
    myDemandMenuCommands(this),
    myViewNet(nullptr),
    myTitlePrefix("NETEDIT " VERSION_STRING) {
    // init icons
    GUIIconSubSys::initIcons(a);
    // init Textures
    GUITextureSubSys::initTextures(a);
    // init cursors
    GUICursorSubSys::initCursors(a);
}


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
    // build toolbar menu
    myToolbarsGrip.buildMenuToolbarsGrip();
    // build the thread - io
    myLoadThreadEvent.setTarget(this),  myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    // build the status bar
    myStatusbar = new FXStatusBar(this, GUIDesignStatusBar);
    {
        myGeoFrame =
            new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myGeoCoordinate = new FXLabel(myGeoFrame, "N/A\t\tOriginal coordinate (before coordinate transformation in NETCONVERT)", nullptr, LAYOUT_CENTER_Y);
        myCartesianFrame =
            new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myCartesianCoordinate = new FXLabel(myCartesianFrame, "N/A\t\tNetwork coordinate", nullptr, LAYOUT_CENTER_Y);
    }
    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this, GUIDesignSplitter | SPLITTER_VERTICAL | SPLITTER_REVERSED);
    myMDIClient = new FXMDIClient(myMainSplitter, GUIDesignSplitterMDI);
    myMDIMenu = new FXMDIMenu(this, myMDIClient);
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
    // set Netedit ICON
    setIcon(GUIIconSubSys::getIcon(ICON_NETEDIT));
    // initialize single hotkeys using decimal code (to avoid problems in Linux)
    getAccelTable()->addAccel(101, this, FXSEL(SEL_COMMAND, MID_HOTKEY_E_EDGEMODE));   // e
    getAccelTable()->addAccel(69,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_E_EDGEMODE));   // E
    getAccelTable()->addAccel(109, this, FXSEL(SEL_COMMAND, MID_HOTKEY_M_MOVEMODE));   // m
    getAccelTable()->addAccel(77,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_M_MOVEMODE));   // M
    getAccelTable()->addAccel(100, this, FXSEL(SEL_COMMAND, MID_HOTKEY_D_DELETEMODE));   // d
    getAccelTable()->addAccel(68,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_D_DELETEMODE));   // D
    getAccelTable()->addAccel(105, this, FXSEL(SEL_COMMAND, MID_HOTKEY_I_INSPECTMODE));   // i
    getAccelTable()->addAccel(73,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_I_INSPECTMODE));   // I
    getAccelTable()->addAccel(115, this, FXSEL(SEL_COMMAND, MID_HOTKEY_S_SELECTMODE));   // s
    getAccelTable()->addAccel(83,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_S_SELECTMODE));   // S
    getAccelTable()->addAccel(99,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_C_CONNECTMODE));   // c
    getAccelTable()->addAccel(67,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_C_CONNECTMODE));   // C
    getAccelTable()->addAccel(119, this, FXSEL(SEL_COMMAND, MID_HOTKEY_W_PROHIBITIONMODE));   // w
    getAccelTable()->addAccel(87,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_W_PROHIBITIONMODE));   // W
    getAccelTable()->addAccel(116, this, FXSEL(SEL_COMMAND, MID_HOTKEY_T_TLSMODE_VTYPEMODE));   // t
    getAccelTable()->addAccel(94,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_T_TLSMODE_VTYPEMODE));   // T
    getAccelTable()->addAccel(97,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_A_ADDITIONALMODE));   // a
    getAccelTable()->addAccel(65,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_A_ADDITIONALMODE));   // A
    getAccelTable()->addAccel(114, this, FXSEL(SEL_COMMAND, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE));   // r
    getAccelTable()->addAccel(82,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE));   // R
    getAccelTable()->addAccel(122, this, FXSEL(SEL_COMMAND, MID_HOTKEY_Z_TAZMODE));   // z
    getAccelTable()->addAccel(90,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_Z_TAZMODE));   // Z
    getAccelTable()->addAccel(112, this, FXSEL(SEL_COMMAND, MID_HOTKEY_P_POLYGONMODE));   // p
    getAccelTable()->addAccel(80,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_P_POLYGONMODE));   // P
    getAccelTable()->addAccel(118, this, FXSEL(SEL_COMMAND, MID_HOTKEY_V_VEHICLEMODE));     // v
    getAccelTable()->addAccel(86,  this, FXSEL(SEL_COMMAND, MID_HOTKEY_V_VEHICLEMODE));     // V
    // initialize Ctrl hotkeys with Caps Lock enabled using decimal code (to avoid problems in Linux)
    getAccelTable()->addAccel(262222, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_N_NEWNETWORK));         // Ctrl + N
    getAccelTable()->addAccel(262223, this, FXSEL(SEL_COMMAND, MID_OPEN_NETWORK));              // Ctrl + O
    getAccelTable()->addAccel(327691, this, FXSEL(SEL_COMMAND, MID_OPEN_CONFIG));               // Ctrl + Shift + O
    getAccelTable()->addAccel(262226, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_R_RELOAD));             // Ctrl + R
    getAccelTable()->addAccel(262227, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_S_SAVENETWORK_STOPSIMULATION));         // Ctrl + S
    getAccelTable()->addAccel(327695, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORKAS));   // Ctrl + Shift + S
    getAccelTable()->addAccel(262220, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_L_SAVEASPLAINXML));         // Ctrl + L
    getAccelTable()->addAccel(262218, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS));         // Ctrl + J
    /*
    getAccelTable()->addAccel(262224, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_P));             // Ctrl + P
    getAccelTable()->addAccel(327692, this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_CTRL_SHIFT_P));   // Ctrl + Shift + P
    */
    getAccelTable()->addAccel(262212, this, FXSEL(SEL_COMMAND, MID_OPEN_ADDITIONALS));                      // Ctrl + D
    getAccelTable()->addAccel(327780, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_D_SAVEADDITIONAL));    // Ctrl + Shift + D
    getAccelTable()->addAccel(262219, this, FXSEL(SEL_COMMAND, MID_OPEN_TLSPROGRAMS));                      // Ctrl + K
    getAccelTable()->addAccel(327787, this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_CTRL_SHIFT_K));               // Ctrl + Shift + K
    getAccelTable()->addAccel(262230, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_W_CLOSESIMULATION));         // Ctrl + W
    getAccelTable()->addAccel(262225, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_Q_CLOSE));                   // Ctrl + Q
    getAccelTable()->addAccel(262234, this, FXSEL(SEL_COMMAND, FXUndoList::ID_UNDO));                       // Ctrl + Z
    getAccelTable()->addAccel(262233, this, FXSEL(SEL_COMMAND, FXUndoList::ID_REDO));                       // Ctrl + Y
    getAccelTable()->addAccel(262230, this, FXSEL(SEL_COMMAND, MID_EDITVIEWSCHEME));                        // Ctrl + V
    getAccelTable()->addAccel(262217, this, FXSEL(SEL_COMMAND, MID_EDITVIEWPORT));                          // Ctrl + I
    getAccelTable()->addAccel(262215, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID));   // Ctrl + G
    getAccelTable()->addAccel(262228, this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_T_OPENSUMONETEDIT));         // Ctrl + T
    getAccelTable()->addAccel(parseAccel("ctrl+shift+c"), this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_C_SHOWCONNECTIONS));   // Ctrl + Shift + C
    getAccelTable()->addAccel(parseAccel("ctrl+shift+i"), this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_I_SELECTEDGES));       // Ctrl + Shift + I
    // initialize Shift hotkeys with Caps Lock enabled using decimal code (to avoid problems in Linux)
    getAccelTable()->addAccel(65642, this, FXSEL(SEL_COMMAND, MID_LOCATEJUNCTION)); // Shift + J
    getAccelTable()->addAccel(65637, this, FXSEL(SEL_COMMAND, MID_LOCATEEDGE));     // Shift + E
    getAccelTable()->addAccel(65652, this, FXSEL(SEL_COMMAND, MID_LOCATETLS));      // Shift + T
    getAccelTable()->addAccel(65633, this, FXSEL(SEL_COMMAND, MID_LOCATEADD));      // Shift + A
    getAccelTable()->addAccel(65647, this, FXSEL(SEL_COMMAND, MID_LOCATEPOI));      // Shift + O
    getAccelTable()->addAccel(65644, this, FXSEL(SEL_COMMAND, MID_LOCATEPOLY));     // Shift + L
    // initialize rest of hotkeys
    getAccelTable()->addAccel(parseAccel("F3"), this, FXSEL(SEL_COMMAND, MID_HOTKEY_F3_SUPERMODE_NETWORK));
    getAccelTable()->addAccel(parseAccel("F4"), this, FXSEL(SEL_COMMAND, MID_HOTKEY_F4_SUPERMODE_DEMAND));
    getAccelTable()->addAccel(parseAccel("Esc"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_ESC));
    getAccelTable()->addAccel(parseAccel("Del"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_DEL));
    getAccelTable()->addAccel(parseAccel("Enter"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_ENTER));
    getAccelTable()->addAccel(parseAccel("F12"), this, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_F12));
}


void
GNEApplicationWindow::create() {
    setWindowSizeAndPos();
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS", "basedir", "");
    FXMainWindow::create();
    myFileMenu->create();
    myEditMenu->create();
    myFileMenuAdditionals->create();
    myFileMenuTLS->create();
    myFileMenuDemandElements->create();
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
    // delete visuals
    delete myGLVisual;
    // must delete menus to avoid segfault on removing accelerators
    // (http://www.fox-toolkit.net/faq#TOC-What-happens-when-the-application-s)
    delete myFileMenuAdditionals,
           delete myFileMenuTLS,
           delete myFileMenuDemandElements,
           delete myFileMenu;
    delete myEditMenu;
    delete myLocatorMenu;
    delete myProcessingMenu;
    delete myWindowsMenu;
    delete myHelpMenu;
    // Delete load thread
    delete myLoadThread;
    // drop all events
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        delete e;
    }
    // delte undo list
    delete myUndoList;
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
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODECREATEEDGE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(myConfigPattern.c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        loadConfigOrNet(file, false);
        // add it into recent configs
        myMenuBarFile.myRecentConfigs.appendFile(file.c_str());
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open Network");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODECREATEEDGE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("SUMO nets (*.net.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        loadConfigOrNet(file, true);
        // add it into recent nets
        myMenuBarFile.myRecentNets.appendFile(file.c_str());
        // when a net is loaded, save additionals and TLSPrograms are disabled
        disableSaveAdditionalsMenu();
        myFileMenuCommands.saveTLSPrograms->disable();
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
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODECREATEEDGE));
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
            // recommended osm options
            // http://sumo.dlr.de/wiki/Networks/Import/OpenStreetMap#Recommended_NETCONVERT_Options
            oc.set("osm-files", file);
            oc.set("geometry.remove", "true");
            oc.set("ramps.guess", "true");
            oc.set("junctions.join", "true");
            oc.set("tls.guess-signals", "true");
            oc.set("tls.discard-simple", "true");
        } else {
            throw ProcessError("Attempted to import unknown file format '" + file + "'.");
        }

        GUIDialog_Options* wizard =
            new GUIDialog_Options(this, "Select Import Options", getWidth(), getHeight());

        if (wizard->execute()) {
            NIFrame::checkOptions(); // needed to set projection parameters
            loadConfigOrNet("", false, false, false);
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenAdditionals(FXObject*, FXSelector, void*) {
    // get the Additional file name
    FXFileDialog opendialog(this, "Open Additionals file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Additional files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto");
        // Create additional handler
        GNEAdditionalHandler additionalHandler(file, myNet->getViewNet());
        // begin undoList operation
        myUndoList->p_begin("Loading additionals from '" + file + "'");
        // Run parser for additionals
        if (!XMLSubSys::runParser(additionalHandler, file, false)) {
            WRITE_MESSAGE("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->p_end();
        update();
        // restore validation for additionals
        XMLSubSys::setValidation("auto", "auto");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenTLSPrograms(FXObject*, FXSelector, void*) {
    // get the shape file name
    FXFileDialog opendialog(this, "Open TLSPrograms file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODETLS));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("TLSProgram files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // Run parser
        myUndoList->p_begin("Loading TLS Programs from '" + file + "'");
        myNet->computeEverything(this);
        if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->parseTLSPrograms(file) == false) {
            // Abort undo/redo
            myUndoList->abort();
        } else {
            // commit undo/redo operation
            myUndoList->p_end();
            update();
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenDemandElements(FXObject*, FXSelector, void*) {
    // get the demand element file name
    FXFileDialog opendialog(this, "Open demand element file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Demand element files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto");
        // Create additional handler
        GNERouteHandler demandHandler(file, myNet->getViewNet());
        // begin undoList operation
        myUndoList->p_begin("Loading demand elements from '" + file + "'");
        // Run parser for additionals
        if (!XMLSubSys::runParser(demandHandler, file, false)) {
            WRITE_MESSAGE("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->p_end();
        update();
        // restore validation for demand
        XMLSubSys::setValidation("auto", "auto");
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
    loadConfigOrNet(file, sender == &myMenuBarFile.myRecentNets);
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
        // disable save additionals and TLS menu
        disableSaveAdditionalsMenu();
        myFileMenuCommands.saveTLSPrograms->disable();
        // hide all Network and demand commands
        myNetworkMenuCommands.hideNetworkMenuCommands();
        myDemandMenuCommands.hideDemandMenuCommands();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdLocate(FXObject*, FXSelector sel, void*) {
    if (myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onCmdLocate(nullptr, sel, nullptr);
        }
    }
    return 1;
}

long
GNEApplicationWindow::onUpdOpen(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myAmLoading ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
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
    WRITE_DEBUG("Opening about dialog");
    // create and open about dialog
    GNEDialog_About* about = new GNEDialog_About(this);
    about->create();
    about->show(PLACEMENT_OWNER);
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Closed about dialog");
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
    eventOccurred();
    return 1;
}


void
GNEApplicationWindow::eventOccurred() {
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
            case EVENT_SIMULATION_LOADED:
                handleEvent_NetworkLoaded(e);
                break;
            case EVENT_MESSAGE_OCCURRED:
            case EVENT_WARNING_OCCURRED:
            case EVENT_ERROR_OCCURRED:
            case EVENT_DEBUG_OCCURRED:
            case EVENT_GLDEBUG_OCCURRED:
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
    OptionsCont& oc = OptionsCont::getOptions();
    myAmLoading = false;
    GNEEvent_NetworkLoaded* ec = static_cast<GNEEvent_NetworkLoaded*>(e);
    // check whether the loading was successfull
    if (ec->myNet == nullptr) {
        // report failure
        setStatusBarText("Loading of '" + ec->myFile + "' failed!");
    } else {
        // set new Net
        myNet = ec->myNet;
        // report success
        setStatusBarText("'" + ec->myFile + "' loaded.");
        setWindowSizeAndPos();
        // build viewparent toolbar grips before creating view parent
        myToolbarsGrip.buildViewParentToolbarsGrips();
        // initialise NETEDIT View
        GNEViewParent* viewParent = new GNEViewParent(myMDIClient, myMDIMenu, "NETEDIT VIEW", this, nullptr, myNet, myUndoList, nullptr, MDI_TRACKING, 10, 10, 300, 200);
        // create it maximized
        viewParent->maximize();
        // mark it as Active child
        myMDIClient->setActiveChild(viewParent);
        // cast pointer myViewNet
        myViewNet = dynamic_cast<GNEViewNet*>(viewParent->getView());
        // set settings in view
        if (viewParent->getView() && ec->mySettingsFile != "") {
            GUISettingsHandler settings(ec->mySettingsFile, true, true);
            std::string settingsName = settings.addSettings(viewParent->getView());
            viewParent->getView()->addDecals(settings.getDecals());
            settings.applyViewport(viewParent->getView());
            settings.setSnapshots(viewParent->getView());
        }
        // set network name on the caption
        setTitle(MFXUtils::getTitleText(myTitlePrefix, ec->myFile.c_str()));
        // set supermode network
        if (myViewNet) {
            myViewNet->onCmdSetSupermode(0, MID_HOTKEY_F3_SUPERMODE_NETWORK, 0);
        }
        if (myViewNet && ec->myViewportFromRegistry) {
            Position off;
            off.set(getApp()->reg().readRealEntry("viewport", "x"), getApp()->reg().readRealEntry("viewport", "y"), getApp()->reg().readRealEntry("viewport", "z"));
            Position p(off.x(), off.y(), 0);
            myViewNet->setViewportFromToRot(off, p, 0);
        }
    }
    getApp()->endWaitCursor();
    myMessageWindow->registerMsgHandlers();
    // check if additionals/shapes has to be loaded at start
    if (oc.isSet("additional-files") && !oc.getString("additional-files").empty() && myNet) {
        myMenuBarFile.myAdditionalsFile = oc.getString("additional-files");
        WRITE_MESSAGE("Loading additionals and shpes from '" + myMenuBarFile.myAdditionalsFile + "'");
        GNEAdditionalHandler additionalHandler(myMenuBarFile.myAdditionalsFile, myNet->getViewNet());
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto");
        // Run parser
        myUndoList->p_begin("Loading additionals and shapes from '" + myMenuBarFile.myAdditionalsFile + "'");
        if (!XMLSubSys::runParser(additionalHandler, myMenuBarFile.myAdditionalsFile, false)) {
            WRITE_ERROR("Loading of " + myMenuBarFile.myAdditionalsFile + " failed.");
        }
        // disable validation for additionals
        XMLSubSys::setValidation("auto", "auto");
        myUndoList->p_end();
    }
    // check if demand elements has to be loaded at start
    if (oc.isSet("route-files") && !oc.getString("route-files").empty() && myNet) {
        myMenuBarFile.myDemandElementsFile = oc.getString("route-files");
        WRITE_MESSAGE("Loading demand elements from '" + myMenuBarFile.myDemandElementsFile + "'");
        GNERouteHandler routeHandler(myMenuBarFile.myDemandElementsFile, myNet->getViewNet());
        // disable validation for demand elements
        XMLSubSys::setValidation("never", "auto");
        // Run parser
        myUndoList->p_begin("Loading demand elements from '" + myMenuBarFile.myDemandElementsFile + "'");
        if (!XMLSubSys::runParser(routeHandler, myMenuBarFile.myDemandElementsFile, false)) {
            WRITE_ERROR("Loading of " + myMenuBarFile.myDemandElementsFile + " failed.");
        }
        // disable validation for demand elements
        XMLSubSys::setValidation("auto", "auto");
        myUndoList->p_end();
    }
    // check if additionals output must be changed
    if (oc.isSet("additionals-output")) {
        myMenuBarFile.myAdditionalsFile = oc.getString("additionals-output");
    }
    // check if TLSPrograms output must be changed
    if (oc.isSet("TLSPrograms-output")) {
        myMenuBarFile.myTLSProgramsFile = oc.getString("TLSPrograms-output");
    }
    // check if demand elements output must be changed
    if (oc.isSet("demandelements-output")) {
        myMenuBarFile.myDemandElementsFile = oc.getString("demandelements-output");
    }
    // after loading net shouldn't be saved
    if (myNet) {
        myNet->requiereSaveNet(false);
    }
    // update app
    update();
}


void
GNEApplicationWindow::handleEvent_Message(GUIEvent* e) {
    GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendMsg(ec->getOwnType(), ec->getMsg());
}

// ===========================================================================
// GNEApplicationWindow::MenuBarFile method definitions
// ===========================================================================

GNEApplicationWindow::MenuBarFile::MenuBarFile(GNEApplicationWindow* GNEApp) :
    myRecentNets(GNEApp->getApp(), "nets"),
    myGNEApp(GNEApp)
{ }


void
GNEApplicationWindow::MenuBarFile::buildRecentFiles(FXMenuPane* fileMenu) {
    FXMenuSeparator* sep1 = new FXMenuSeparator(fileMenu);
    sep1->setTarget(&myRecentConfigs);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentConfigs, FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(fileMenu, "Clear Recent Configurat&ions", nullptr, &myRecentConfigs, FXRecentFiles::ID_CLEAR);
    myRecentConfigs.setTarget(myGNEApp);
    myRecentConfigs.setSelector(MID_RECENTFILE);
    FXMenuSeparator* sep2 = new FXMenuSeparator(fileMenu);
    sep2->setTarget(&myRecentNets);
    sep2->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(fileMenu, "", nullptr, &myRecentNets, FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(fileMenu, "Cl&ear Recent Networks", nullptr, &myRecentNets, FXRecentFiles::ID_CLEAR);
    myRecentNets.setTarget(myGNEApp);
    myRecentNets.setSelector(MID_RECENTFILE);
}

// ---------------------------------------------------------------------------
// GNEViewNet::FileMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::FileMenuCommands::FileMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::FileMenuCommands::buildFileMenuCommands(FXMenuPane* fileMenu) {
    new FXMenuCommand(fileMenu,
                      "&New Network...\tCtrl+N\tCreate a new network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), myGNEApp, MID_HOTKEY_CTRL_N_NEWNETWORK);
    new FXMenuCommand(fileMenu,
                      "&Open Network...\tCtrl+O\tOpen a SUMO network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), myGNEApp, MID_OPEN_NETWORK);
    new FXMenuCommand(fileMenu,
                      "Open Netconvert Configura&tion...\tCtrl+Shift+O\tOpen a configuration file with NETCONVERT options.",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), myGNEApp, MID_OPEN_CONFIG);
    new FXMenuCommand(fileMenu,
                      "Import &Foreign Network...\t\tImport a foreign network such as OSM.",
                      GUIIconSubSys::getIcon(ICON_OPEN_NET), myGNEApp, MID_GNE_TOOLBARFILE_OPENFOREIGN);
    new FXMenuCommand(fileMenu,
                      "&Reload\tCtrl+R\tReloads the network.",
                      GUIIconSubSys::getIcon(ICON_RELOAD), myGNEApp, MID_HOTKEY_CTRL_R_RELOAD);
    new FXMenuCommand(fileMenu,
                      "&Save Network...\tCtrl+S\tSave the network.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_S_SAVENETWORK_STOPSIMULATION);
    new FXMenuCommand(fileMenu,
                      "Save Net&work As...\tCtrl+Shift+S\tSave the network in another file.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORKAS);
    new FXMenuCommand(fileMenu,
                      "Save plain XM&L...\tCtrl+L\tSave plain xml representation the network.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_L_SAVEASPLAINXML);
    new FXMenuCommand(fileMenu,
                      "Save &joined junctions...\tCtrl+J\tSave log of joined junctions (allows reproduction of joins).",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS);
    // create Additionals menu options
    myGNEApp->myFileMenuAdditionals = new FXMenuPane(myGNEApp);
    new FXMenuCommand(myGNEApp->myFileMenuAdditionals,
                      "Load A&dditionals...\tCtrl+D\tLoad additional and shapes elements.",
                      GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), myGNEApp, MID_OPEN_ADDITIONALS);
    saveAdditionals = new FXMenuCommand(myGNEApp->myFileMenuAdditionals,
                                        "Save Additionals\tCtrl+Shift+D\tSave additional and shapes elements.",
                                        GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_D_SAVEADDITIONAL);
    saveAdditionals->disable();
    saveAdditionalsAs = new FXMenuCommand(myGNEApp->myFileMenuAdditionals,
                                          "Save Additionals As...\t\tSave additional elements in another file.",
                                          GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS);
    saveAdditionalsAs->disable();
    new FXMenuCascade(fileMenu, "Additionals and shapes", GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), myGNEApp->myFileMenuAdditionals);
    // create TLS menu options
    myGNEApp->myFileMenuTLS = new FXMenuPane(myGNEApp);
    new FXMenuCommand(myGNEApp->myFileMenuTLS,
                      "load TLS Programs...\tCtrl+K\tload TLS Programs in all Traffic Lights of the net.",
                      GUIIconSubSys::getIcon(ICON_OPEN_TLSPROGRAMS), myGNEApp, MID_OPEN_TLSPROGRAMS);
    saveTLSPrograms = new FXMenuCommand(myGNEApp->myFileMenuTLS,
                                        "Save TLS Programs \tCtrl+Shift+K\tSave TLS Programs of all Traffic Lights of the current net.",
                                        GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_GNE_HOTKEY_CTRL_SHIFT_K);
    saveTLSPrograms->disable();
    new FXMenuCommand(myGNEApp->myFileMenuTLS,
                      "Save TLS Programs As...\t\tSave TLS Programs of all Traffic Lights of the current net in another file.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS);
    new FXMenuCascade(fileMenu, "Traffic Lights", GUIIconSubSys::getIcon(ICON_MODETLS), myGNEApp->myFileMenuTLS);
    // create DemandElements menu options
    myGNEApp->myFileMenuDemandElements = new FXMenuPane(myGNEApp);
    new FXMenuCommand(myGNEApp->myFileMenuDemandElements,
                      "Load demand elements...\t\tLoad demand elements.",
                      GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), myGNEApp, MID_GNE_TOOLBARFILE_LOADDEMAND);
    saveDemandElements = new FXMenuCommand(myGNEApp->myFileMenuDemandElements,
                                           "Save demand elements\t\tSave demand elements.",
                                           GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVEDEMAND);
    saveDemandElements->disable();
    saveDemandElementsAs = new FXMenuCommand(myGNEApp->myFileMenuDemandElements,
            "Save demand elements as...\t\tSave demand elements in another file.",
            GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVEDEMAND_AS);
    saveDemandElementsAs->disable();
    new FXMenuCascade(fileMenu, "Demand elements", GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND), myGNEApp->myFileMenuDemandElements);
    // close network
    new FXMenuSeparator(fileMenu);
    new FXMenuCommand(fileMenu,
                      "Close\tCtrl+W\tClose the net&work.",
                      GUIIconSubSys::getIcon(ICON_CLOSE), myGNEApp, MID_HOTKEY_CTRL_W_CLOSESIMULATION);
    // build recent files
    myGNEApp->myMenuBarFile.buildRecentFiles(fileMenu);
    new FXMenuSeparator(fileMenu);
    new FXMenuCommand(fileMenu, "&Quit\tCtrl+Q\tQuit the Application.", nullptr, myGNEApp, MID_HOTKEY_CTRL_Q_CLOSE, 0);

}

// ---------------------------------------------------------------------------
// GNEViewNet::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::NetworkMenuCommands::NetworkMenuCommands(GNEApplicationWindow* GNEApp) :
    createEdgeMode(nullptr),
    moveMode(nullptr),
    deleteMode(nullptr),
    inspectMode(nullptr),
    selectMode(nullptr),
    connectMode(nullptr),
    prohibitionMode(nullptr),
    TLSMode(nullptr),
    additionalMode(nullptr),
    crossingMode(nullptr),
    TAZMode(nullptr),
    shapeMode(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::NetworkMenuCommands::showNetworkMenuCommands() {
    createEdgeMode->show();
    moveMode->show();
    deleteMode->show();
    inspectMode->show();
    selectMode->show();
    connectMode->show();
    prohibitionMode->show();
    TLSMode->show();
    additionalMode->show();
    crossingMode->show();
    TAZMode->show();
    shapeMode->show();
    // also show separator
    myHorizontalSeparator->show();
}


void
GNEApplicationWindow::NetworkMenuCommands::hideNetworkMenuCommands() {
    createEdgeMode->hide();
    moveMode->hide();
    deleteMode->hide();
    inspectMode->hide();
    selectMode->hide();
    connectMode->hide();
    prohibitionMode->hide();
    TLSMode->hide();
    additionalMode->hide();
    crossingMode->hide();
    TAZMode->hide();
    shapeMode->hide();
    // also hide separator
    myHorizontalSeparator->hide();
}


void
GNEApplicationWindow::NetworkMenuCommands::buildNetworkMenuCommands(FXMenuPane* editMenu) {
    // build every FXMenuCommand giving it a shortcut
    createEdgeMode = new FXMenuCommand(editMenu,
        "&Edge mode\tE\tCreate junction and edges.",
        GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), myGNEApp, MID_HOTKEY_E_EDGEMODE);
    moveMode = new FXMenuCommand(editMenu,
        "&Move mode\tM\tMove elements.",
        GUIIconSubSys::getIcon(ICON_MODEMOVE), myGNEApp, MID_HOTKEY_M_MOVEMODE);
    deleteMode = new FXMenuCommand(editMenu,
        "&Delete mode\tD\tDelete elements.",
        GUIIconSubSys::getIcon(ICON_MODEDELETE), myGNEApp, MID_HOTKEY_D_DELETEMODE);
    inspectMode = new FXMenuCommand(editMenu,
        "&Inspect mode\tI\tInspect elements and change their attributes.",
        GUIIconSubSys::getIcon(ICON_MODEINSPECT), myGNEApp, MID_HOTKEY_I_INSPECTMODE);
    selectMode = new FXMenuCommand(editMenu,
        "&Select mode\tS\tSelect elements.",
        GUIIconSubSys::getIcon(ICON_MODESELECT), myGNEApp, MID_HOTKEY_S_SELECTMODE);
    connectMode = new FXMenuCommand(editMenu,
        "&Connection mode\tC\tEdit connections between lanes.",
        GUIIconSubSys::getIcon(ICON_MODECONNECTION), myGNEApp, MID_HOTKEY_C_CONNECTMODE);
    prohibitionMode = new FXMenuCommand(editMenu,
        "Pro&hibition mode\tW\tEdit connection prohibitions.",
        GUIIconSubSys::getIcon(ICON_MODEPROHIBITION), myGNEApp, MID_HOTKEY_W_PROHIBITIONMODE);
    TLSMode = new FXMenuCommand(editMenu,
        "&Traffic light mode\tT\tEdit traffic lights over junctions.",
        GUIIconSubSys::getIcon(ICON_MODETLS), myGNEApp, MID_HOTKEY_T_TLSMODE_VTYPEMODE);
    additionalMode = new FXMenuCommand(editMenu,
        "&Additional mode\tA\tCreate additional elements.",
        GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), myGNEApp, MID_HOTKEY_A_ADDITIONALMODE);
    crossingMode = new FXMenuCommand(editMenu,
        "C&rossing mode\tR\tCreate crossings between edges.",
        GUIIconSubSys::getIcon(ICON_MODECROSSING), myGNEApp, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE);
    TAZMode = new FXMenuCommand(editMenu,
        "TA&Z mode\tZ\tCreate Traffic Assignment Zones.",
        GUIIconSubSys::getIcon(ICON_MODETAZ), myGNEApp, MID_HOTKEY_Z_TAZMODE);
    shapeMode = new FXMenuCommand(editMenu,
        "&POI-Poly mode\tP\tCreate Points-Of-Interest and polygons.",
        GUIIconSubSys::getIcon(ICON_MODEPOLYGON), myGNEApp, MID_HOTKEY_P_POLYGONMODE);
    // build separator
    myHorizontalSeparator = new FXMenuSeparator(editMenu);
}

// ---------------------------------------------------------------------------
// GNEViewNet::DemandCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::DemandMenuCommands::DemandMenuCommands(GNEApplicationWindow* GNEApp) :
    routeMode(nullptr),
    vehicleMode(nullptr),
    vehicleTypeMode(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::DemandMenuCommands::showDemandMenuCommands() {
    routeMode->show();
    vehicleMode->show();
    vehicleTypeMode->show();
    // also show separator
    myHorizontalSeparator->show();
}


void
GNEApplicationWindow::DemandMenuCommands::hideDemandMenuCommands() {
    routeMode->hide();
    vehicleMode->hide();
    vehicleTypeMode->hide();
    // also hide separator
    myHorizontalSeparator->hide();
}


void
GNEApplicationWindow::DemandMenuCommands::buildDemandMenuCommands(FXMenuPane* editMenu) {
    // build every FXMenuCommand giving it a shortcut
    routeMode = new FXMenuCommand(editMenu,
        "&Route mode\tR\tCreate Routes.",
        GUIIconSubSys::getIcon(ICON_MODEROUTE), myGNEApp, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE);
    vehicleMode = new FXMenuCommand(editMenu,
        "&Vehicle mode\tV\tCreate vehicles.",
        GUIIconSubSys::getIcon(ICON_MODEVEHICLE), myGNEApp, MID_HOTKEY_V_VEHICLEMODE);
    vehicleTypeMode = new FXMenuCommand(editMenu,
        "&Vehicle type mode\tT\tCreate vehicle types.",
        GUIIconSubSys::getIcon(ICON_MODEVEHICLETYPE), myGNEApp, MID_HOTKEY_T_TLSMODE_VTYPEMODE);
    // build separator
    myHorizontalSeparator = new FXMenuSeparator(editMenu);
}

// ---------------------------------------------------------------------------
// private methods
// ---------------------------------------------------------------------------

void
GNEApplicationWindow::fillMenuBar() {
    // declare a FXMenuTitle needed to set height in all menu titles
    FXMenuTitle* menuTitle;
    // build file menu
    myFileMenu = new FXMenuPane(this, LAYOUT_FIX_HEIGHT);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&File", nullptr, myFileMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    myFileMenuCommands.buildFileMenuCommands(myFileMenu);
    // build edit menu
    myEditMenu = new FXMenuPane(this);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Edit", nullptr, myEditMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    // build undo/redo command
    new FXMenuCommand(myEditMenu,
                      "&Undo\tCtrl+Z\tUndo the last change.",
                      GUIIconSubSys::getIcon(ICON_UNDO), myUndoList, FXUndoList::ID_UNDO);
    new FXMenuCommand(myEditMenu,
                      "&Redo\tCtrl+Y\tRedo the last change.",
                      GUIIconSubSys::getIcon(ICON_REDO), myUndoList, FXUndoList::ID_REDO);
    // build separator
    new FXMenuSeparator(myEditMenu);
    // build Network modes commands and hide it
    myNetworkMenuCommands.buildNetworkMenuCommands(myEditMenu);
    myNetworkMenuCommands.hideNetworkMenuCommands();
    // build Demand Modes commands
    myDemandMenuCommands.buildDemandMenuCommands(myEditMenu);
    myDemandMenuCommands.hideDemandMenuCommands();
    new FXMenuCommand(myEditMenu,
                      "Edit Visualisation ...\tCtrl+V\tOpens a dialog for editing visualization settings.",
                      nullptr, this, MID_EDITVIEWSCHEME);
    new FXMenuCommand(myEditMenu,
                      "Edit Viewport...\tCtrl+I\tOpens a dialog for editing viewing are, zoom and rotation.",
                      nullptr, this, MID_EDITVIEWPORT);
    new FXMenuCommand(myEditMenu,
                      "Toggle Grid...\tCtrl+G\tToggles background grid (and snap-to-grid functionality).",
                      nullptr, this, MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
                      "Open in SUMO GUI...\tCtrl+T\tOpens the SUMO GUI application with the current network.",
                      GUIIconSubSys::getIcon(ICON_SUMO_MINI), this, MID_HOTKEY_CTRL_T_OPENSUMONETEDIT);
    // processing menu (trigger netbuild computations)
    myProcessingMenu = new FXMenuPane(this);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Processing", nullptr, myProcessingMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    // build processing menu commands
    new FXMenuCommand(myProcessingMenu,
                      "Compute Junctions\tF5\tComputes junction shape and logic.",
                      GUIIconSubSys::getIcon(ICON_COMPUTEJUNCTIONS), this, MID_HOTKEY_F5_RECOMPUTE);
    new FXMenuCommand(myProcessingMenu,
                      "Compute Junctions with volatile options\tShift+F5\tComputes junction shape and logic using volatile junctions.",
                      GUIIconSubSys::getIcon(ICON_COMPUTEJUNCTIONS), this, MID_HOTKEY_SHIFT_F5_RECOMPUTEVOLATILE);
    new FXMenuCommand(myProcessingMenu,
                      "Clean Junctions\tF6\tRemoves solitary junctions.",
                      GUIIconSubSys::getIcon(ICON_CLEANJUNCTIONS), this, MID_GNE_HOTKEY_F6);
    new FXMenuCommand(myProcessingMenu,
                      "Join Selected Junctions\tF7\tJoins selected junctions into a single junction.",
                      GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), this, MID_GNE_HOTKEY_F7);
    new FXMenuCommand(myProcessingMenu,
                      "Clean invalid crossings\tF8\tClear invalid crossings.",
                      GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), this, MID_GNE_HOTKEY_F8);
    new FXMenuCommand(myProcessingMenu,
                      "Options\tF10\t\tConfigure Processing Options.",
                      GUIIconSubSys::getIcon(ICON_OPTIONS), this, MID_GNE_HOTKEY_F10);
    // build locate menu
    myLocatorMenu = new FXMenuPane(this);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Locate", nullptr, myLocatorMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    // build locate menu commands
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
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Windows", nullptr, myWindowsMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    // build windows menu commands
    new FXMenuCheck(myWindowsMenu,
                    "&Show Status Line\t\tToggle this Status Bar on/off.",
                    myStatusbar, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show &Message Window\t\tToggle the Message Window on/off.",
                    myMessageWindow, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCommand(myWindowsMenu,
                      "&Clear Message Window\t\tClear the message window.",
                      nullptr, this, MID_CLEARMESSAGEWINDOW);
    // build help menu
    myHelpMenu = new FXMenuPane(this);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Help", nullptr, myHelpMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    // build help menu commands
    new FXMenuCommand(myHelpMenu,
                      "&Online Documentation\tF1\tOpen Online documentation.",
                      nullptr, this, MID_HOTKEY_F1_ONLINEDOCUMENTATION);
    new FXMenuCommand(myHelpMenu,
                      "&About\tF2\tAbout netedit.",
                      nullptr, this, MID_HOTKEY_F2_ABOUT);
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
    // show Network command menus
    myNetworkMenuCommands.showNetworkMenuCommands();
    // update window
    update();
}


FXGLCanvas*
GNEApplicationWindow::getBuildGLCanvas() const {
    // NETEDIT uses only a single View, then return nullptr
    return nullptr;
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


GNEApplicationWindow::ToolbarsGrip&
GNEApplicationWindow::getToolbarsGrip() {
    return myToolbarsGrip;
}


void
GNEApplicationWindow::closeAllWindows() {
    myTrackerLock.lock();
    // remove trackers and other external windows
    while (!myGLWindows.empty()) {
        delete myGLWindows.front();
    }
    myViewNet = nullptr;
    for (FXMainWindow* const window : myTrackerWindows) {
        window->destroy();
        delete window;
    }
    myTrackerWindows.clear();
    // reset the caption
    setTitle(myTitlePrefix);
    // add a separator to the log
    myMessageWindow->addSeparator();
    myTrackerLock.unlock();
    // remove coordinate information
    myGeoCoordinate->setText("N/A");
    myCartesianCoordinate->setText("N/A");

    myUndoList->p_clear();
    // check if net can be deleted
    if (myNet != nullptr) {
        delete myNet;
        myNet = nullptr;
        GeoConvHelper::resetLoaded();
    }
    myMessageWindow->unregisterMsgHandlers();
    // Reset textures
    GUITextureSubSys::resetTextures();
    // reset fonts
    GLHelper::resetFont();
    // disable saving commmand
    disableSaveAdditionalsMenu();
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
GNEApplicationWindow::enableSaveAdditionalsMenu() {
    myFileMenuCommands.saveAdditionals->enable();
    myFileMenuCommands.saveAdditionalsAs->enable();
}


void
GNEApplicationWindow::disableSaveAdditionalsMenu() {
    myFileMenuCommands.saveAdditionals->disable();
    myFileMenuCommands.saveAdditionalsAs->disable();
}


void
GNEApplicationWindow::enableSaveTLSProgramsMenu() {
    myFileMenuCommands.saveTLSPrograms->enable();
}


void
GNEApplicationWindow::enableSaveDemandElementsMenu() {
    myFileMenuCommands.saveDemandElements->disable();
    myFileMenuCommands.saveDemandElementsAs->disable();
}


void
GNEApplicationWindow::disableSaveDemandElementsMenu() {
    myFileMenuCommands.saveDemandElements->disable();
    myFileMenuCommands.saveDemandElementsAs->disable();
}


long
GNEApplicationWindow::onCmdSetSuperMode(FXObject* sender, FXSelector sel, void* ptr) {
    // check that currently there is a View
    if (myViewNet) {
        myViewNet->onCmdSetSupermode(sender, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSetMode(FXObject* sender, FXSelector sel, void* ptr) {
    // check that currently there is a View
    if (myViewNet) {
        myViewNet->onCmdSetMode(sender, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenSUMOGUI(FXObject*, FXSelector, void*) {
    // check that currently there is a View
    if (myViewNet) {
        FXRegistry reg("SUMO GUI", "Eclipse");
        reg.read();
        reg.writeRealEntry("viewport", "x", myViewNet->getChanger().getXPos());
        reg.writeRealEntry("viewport", "y", myViewNet->getChanger().getYPos());
        reg.writeRealEntry("viewport", "z", myViewNet->getChanger().getZPos());
        reg.write();
        std::string sumogui = "sumo-gui";
        const char* sumoPath = getenv("SUMO_HOME");
        if (sumoPath != nullptr) {
            std::string newPath = std::string(sumoPath) + "/bin/sumo-gui";
            if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
                sumogui = "\"" + newPath + "\"";
            }
        }
        std::string cmd = sumogui + " --registry-viewport" + " -n "  + OptionsCont::getOptions().getString("output-file");
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
    }
    return 1;
}


long
GNEApplicationWindow::onCmdAbort(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key ESC (abort) pressed");
        // first check if we're selecting a subset of edges in TAZ Frame
        if (myViewNet->getViewParent()->getTAZFrame()->getTAZSelectionStatisticsModul()->getEdgeAndTAZChildsSelected().size() > 0) {
            // show extra information for tests
            WRITE_DEBUG("Cleaning current selected edges");
            // clear current selection
            myViewNet->getViewParent()->getTAZFrame()->getTAZSelectionStatisticsModul()->clearSelectedEdges();
        } else {
            // abort current operation
            myViewNet->abortOperation();
            myViewNet->update();
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdDel(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key DEL (delete) pressed");
        myViewNet->hotkeyDel();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEnter(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key ENTER pressed");
        myViewNet->hotkeyEnter();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdFocusFrame(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        myViewNet->hotkeyFocusFrame();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEditViewport(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        myViewNet->showViewportEditor();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEditViewScheme(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        myViewNet->showViewschemeEditor();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToogleGrid(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // Toogle getMenuCheckShowGrid of GNEViewNet
        if (myViewNet->getViewOptions().menuCheckShowGrid->getCheck() == TRUE) {
            myViewNet->getViewOptions().menuCheckShowGrid->setCheck(FALSE);
            // show extra information for tests
            WRITE_DEBUG("Disabled grid throught Ctrl+g hotkey");
        } else {
            myViewNet->getViewOptions().menuCheckShowGrid->setCheck(TRUE);
            // show extra information for tests
            WRITE_WARNING("Enabled grid throught Ctrl+g hotkey");
        }
        // Call manually show grid function
        myViewNet->onCmdShowGrid(nullptr, 0, nullptr);
    }
    return 1;
}


long 
GNEApplicationWindow::onCmdToogleShowConnections(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // Toogle menuCheckShowConnections of GNEViewNet
        if (myViewNet->getViewOptions().menuCheckShowConnections->getCheck() == TRUE) {
            myViewNet->getViewOptions().menuCheckShowConnections->setCheck(FALSE);
            // show extra information for tests
            WRITE_DEBUG("Disabled show connections throught Ctrl+shift+c hotkey");
        } else {
            myViewNet->getViewOptions().menuCheckShowConnections->setCheck(TRUE);
            // show extra information for tests
            WRITE_WARNING("Disabled show connections throught Ctrl+shift+c hotkey");
        }
        // Call manually toogle show connection function
        myViewNet->onCmdToogleShowConnection(nullptr, 0, nullptr);

    }
    return 1;
}


long 
GNEApplicationWindow::onCmdToogleSelectEdges(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // Toogle selectEdges of GNEViewNet
        if (myViewNet->getViewOptions().menuCheckSelectEdges->getCheck() == TRUE) {
            myViewNet->getViewOptions().menuCheckSelectEdges->setCheck(FALSE);
            // show extra information for tests
            WRITE_DEBUG("Disabled select edges throught Ctrl+shift+i hotkey");
        } else {
            myViewNet->getViewOptions().menuCheckSelectEdges->setCheck(TRUE);
            // show extra information for tests
            WRITE_WARNING("Disabled select edges throught Ctrl+shift+i hotkey");
        }
        // update view
        myViewNet->update();
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
    WRITE_DEBUG("Key F5 (Compute) pressed");
    myNet->computeEverything(this, true, false);
    updateControls();
    return 1;
}


long
GNEApplicationWindow::onCmdComputeJunctionsVolatile(FXObject*, FXSelector, void*) {
    // declare variable to save FXMessageBox outputs.
    FXuint answer = 0;
    // declare string to save paths in wich additionals, shapes and demand will be saved
    std::string additionalsSavePath = myMenuBarFile.myAdditionalsFile;
    std::string demandElementsSavePath = myMenuBarFile.myDemandElementsFile;
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Keys Shift + F5 (Compute with volatile options) pressed");
    WRITE_DEBUG("Opening FXMessageBox 'Volatile Recomputing'");
    // open question dialog box
    answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Recompute with volatile options",
                                    "Changes produced in the net due a recomputing with volatile options cannot be undone. Continue?");
    if (answer != 1) { //1:yes, 2:no, 4:esc
        // write warning if netedit is running in testing mode
        if (answer == 2) {
            WRITE_DEBUG("Closed FXMessageBox 'Volatile Recomputing' with 'No'");
        } else if (answer == 4) {
            WRITE_DEBUG("Closed FXMessageBox 'Volatile Recomputing' with 'ESC'");
        }
        // abort recompute with volatile options
        return 0;
    } else {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'Volatile Recomputing' with 'Yes'");
        // Check if there are additionals in our net
        if (myNet->getNumberOfAdditionals() > 0) {
            // ask user if want to save additionals if weren't saved previously
            if (myMenuBarFile.myAdditionalsFile == "") {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Save additionals before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save additionals before recomputing with volatile options",
                                                "Would you like to save additionals before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save additionals before recomputing' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save additionals before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'Save additionals before recomputing' with 'Yes'");
                    // Open a dialog to set filename output
                    myMenuBarFile.myAdditionalsFile = MFXUtils::getFilename2Write(this,
                                                      "Select name of the additional file", ".xml",
                                                      GUIIconSubSys::getIcon(ICON_MODETLS),
                                                      gCurrentFolder).text();
                    // set obtanied filename output into additionalsSavePath (can be "")
                    additionalsSavePath = myMenuBarFile.myAdditionalsFile;
                }
            }
            // Check if additional must be saved in a temporal directory, if user didn't define a directory for additionals
            if (myMenuBarFile.myAdditionalsFile == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                additionalsSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpAdditionalsNetedit.xml");
            }
            // Start saving additionals
            getApp()->beginWaitCursor();
            try {
                myNet->saveAdditionals(additionalsSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Error saving additionals before recomputing'");
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving additionals in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'Error saving additionals before recomputing' with 'OK'");
            }
            // end saving additionals
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
        } else {
            // clear additional path
            additionalsSavePath = "";
        }
        // Check if there are demand elements in our net
        if (myNet->getNumberOfDemandElements() > 0) {
            // ask user if want to save demand elements if weren't saved previously
            if (myMenuBarFile.myDemandElementsFile == "") {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Save demand elements before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save demand elements before recomputing with volatile options",
                                                "Would you like to save demand elements before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before recomputing' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before recomputing' with 'Yes'");
                    // Open a dialog to set filename output
                    myMenuBarFile.myDemandElementsFile = MFXUtils::getFilename2Write(this,
                                                         "Select name of the demand element file", ".xml",
                                                         GUIIconSubSys::getIcon(ICON_MODETLS),
                                                         gCurrentFolder).text();
                    // set obtanied filename output into demand elementSavePath (can be "")
                    demandElementsSavePath = myMenuBarFile.myDemandElementsFile;
                }
            }
            // Check if demand element must be saved in a temporal directory, if user didn't define a directory for demand elements
            if (myMenuBarFile.myDemandElementsFile == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                demandElementsSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpDemandElementsNetedit.xml");
            }
            // Start saving demand elements
            getApp()->beginWaitCursor();
            try {
                myNet->saveDemandElements(demandElementsSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Error saving demand elements before recomputing'");
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving demand elements in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'Error saving demand elements before recomputing' with 'OK'");
            }
            // end saving demand elements
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
        } else {
            // clear demand element path
            demandElementsSavePath = "";
        }
        // compute with volatile options
        myNet->computeEverything(this, true, true, additionalsSavePath);
        updateControls();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdCleanJunctions(FXObject*, FXSelector, void*) {
    // show extra information for tests
    WRITE_DEBUG("Key F6 (Clean junction) pressed");
    myNet->removeSolitaryJunctions(myUndoList);
    return 1;
}


long
GNEApplicationWindow::onCmdJoinJunctions(FXObject*, FXSelector, void*) {
    // show extra information for tests
    WRITE_DEBUG("Key F7 (Join junctions) pressed");
    myNet->joinSelectedJunctions(myUndoList);
    return 1;
}


long
GNEApplicationWindow::onCmdCleanInvalidCrossings(FXObject*, FXSelector, void*) {
    // show extra information for tests
    WRITE_DEBUG("Key F8 (Clean invalid crossings) pressed");
    myNet->cleanInvalidCrossings(myUndoList);
    return 1;
}


long
GNEApplicationWindow::onCmdOptions(FXObject*, FXSelector, void*) {
    GUIDialog_Options* wizard =
        new GUIDialog_Options(this, "Configure Options", getWidth(), getHeight());

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
                    GUIIconSubSys::getIcon(ICON_MODECREATEEDGE),
                    gCurrentFolder);
    if (file == "") {
        return 1;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    oc.resetWritable();
    oc.set("output-file", file.text());
    setTitle(MFXUtils::getTitleText(myTitlePrefix, file));
    onCmdSaveNetwork(nullptr, 0, nullptr);
    return 1;
}


long
GNEApplicationWindow::onCmdSaveAsPlainXML(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the plain-xml edge-file (other names will be deduced from this)", "",
                    GUIIconSubSys::getIcon(ICON_MODECREATEEDGE),
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
        WRITE_DEBUG("Opening FXMessageBox 'Error saving plainXML'");
        // open message box
        FXMessageBox::error(this, MBOX_OK, "Saving plain xml failed!", "%s", e.what());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'Error saving plainXML' with 'OK'");
    }
    myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Plain XML saved with prefix '" + prefix + "'.\n");
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
                    GUIIconSubSys::getIcon(ICON_MODECREATEEDGE),
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
        WRITE_DEBUG("Opening FXMessageBox 'error saving joined'");
        // opening error message
        FXMessageBox::error(this, MBOX_OK, "Saving joined junctions failed!", "%s", e.what());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'error saving joined' with 'OK'");
    }
    myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Joined junctions saved to '" + filename + "'.\n");
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
GNEApplicationWindow::onUpdNeedsNetwork(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myNet == nullptr ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdReload(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myNet == nullptr || !OptionsCont::getOptions().isSet("sumo-net-file") ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onCmdSaveNetwork(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    // function onCmdSaveAsNetwork must be executed if this is the first save
    if (oc.getString("output-file") == "") {
        return onCmdSaveAsNetwork(nullptr, 0, nullptr);
    } else {
        getApp()->beginWaitCursor();
        try {
            myNet->save(oc);
            myUndoList->unmark();
            myUndoList->mark();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving network'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving Network failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving network' with 'OK'");
        }
        myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Network saved in " + oc.getString("output-file") + ".\n");
        // After saveing a net sucesfully, add it into Recent Nets list.
        myMenuBarFile.myRecentNets.appendFile(oc.getString("output-file").c_str());
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionals(FXObject*, FXSelector, void*) {
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveAdditionals->isEnabled()) {
        // Check if additionals file was already set at start of netedit or with a previous save
        if (myMenuBarFile.myAdditionalsFile == "") {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the additional file", ".xml",
                            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL),
                            gCurrentFolder);
            if (file == "") {
                // None additionals file was selected, then stop function
                return 0;
            } else {
                myMenuBarFile.myAdditionalsFile = file.text();
            }
        }
        // Start saving additionals
        getApp()->beginWaitCursor();
        try {
            myNet->saveAdditionals(myMenuBarFile.myAdditionalsFile);
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Additionals saved in " + myMenuBarFile.myAdditionalsFile + ".\n");
            myFileMenuCommands.saveAdditionals->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving additionals'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving additionals failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving additionals' with 'OK'");
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
                    GUIIconSubSys::getIcon(ICON_MODEADDITIONAL),
                    gCurrentFolder);
    if (file != "") {
        // Set new additional file
        myMenuBarFile.myAdditionalsFile = file.text();
        // save additionals
        return onCmdSaveAdditionals(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveTLSPrograms(FXObject*, FXSelector, void*) {
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveTLSPrograms->isEnabled()) {
        // Check if TLS Programs file was already set at start of netedit or with a previous save
        if (myMenuBarFile.myTLSProgramsFile == "") {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the additional file", ".xml",
                            GUIIconSubSys::getIcon(ICON_MODETLS),
                            gCurrentFolder);
            if (file == "") {
                // None TLS Programs file was selected, then stop function
                return 0;
            } else {
                myMenuBarFile.myTLSProgramsFile = file.text();
            }
        }
        // Start saving TLS Programs
        getApp()->beginWaitCursor();
        try {
            myNet->saveTLSPrograms(myMenuBarFile.myTLSProgramsFile);
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "TLS Programs saved in " + myMenuBarFile.myTLSProgramsFile + ".\n");
            myFileMenuCommands.saveTLSPrograms->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving TLS Programs'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving TLS Programs failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving TLS Programs' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdSaveTLSProgramsAs(FXObject*, FXSelector, void*) {
    // Open window to select TLS Programs file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the TLS Progarm file", ".xml",
                    GUIIconSubSys::getIcon(ICON_MODETLS),
                    gCurrentFolder);
    if (file != "") {
        // Set new TLS Program file
        myMenuBarFile.myTLSProgramsFile = file.text();
        // save TLS Programs
        return onCmdSaveTLSPrograms(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElements(FXObject*, FXSelector, void*) {
    // check if save demand element menu is enabled
    if (myFileMenuCommands.saveDemandElements->isEnabled()) {
        // Check if demand elements file was already set at start of netedit or with a previous save
        if (myMenuBarFile.myDemandElementsFile == "") {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the demand element file", ".xml",
                            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL),
                            gCurrentFolder);
            if (file == "") {
                // None demand elements file was selected, then stop function
                return 0;
            } else {
                myMenuBarFile.myDemandElementsFile = file.text();
            }
        }
        // Start saving demand elements
        getApp()->beginWaitCursor();
        try {
            myNet->saveDemandElements(myMenuBarFile.myDemandElementsFile);
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Demand elements saved in " + myMenuBarFile.myDemandElementsFile + ".\n");
            myFileMenuCommands.saveDemandElements->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving demand elements'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving demand elements failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving demand elements' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElementsAs(FXObject*, FXSelector, void*) {
    // Open window to select additionasl file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the demand element file", ".xml",
                    GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND),
                    gCurrentFolder);
    if (file != "") {
        // Set new demand element file
        myMenuBarFile.myDemandElementsFile = file.text();
        // save demand elements
        return onCmdSaveDemandElements(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onUpdSaveNetwork(FXObject* sender, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    bool enable = myNet != nullptr && oc.isSet("output-file");
    sender->handle(this, FXSEL(SEL_COMMAND, enable ? ID_ENABLE : ID_DISABLE), nullptr);
    if (enable) {
        FXString caption = ("Save " + oc.getString("output-file")).c_str();
        sender->handle(this, FXSEL(SEL_COMMAND, FXMenuCaption::ID_SETSTRINGVALUE), (void*)&caption);
    }
    return 1;
}


bool
GNEApplicationWindow::continueWithUnsavedChanges() {
    FXuint answer = 0;
    if (myViewNet && myNet && !myNet->isNetSaved()) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'Confirm closing network'");
        // open question box
        answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                        "Confirm closing Network", "%s",
                                        "You have unsaved changes in the network. Do you wish to quit and discard all changes?");
        // restore focus to view net
        myViewNet->setFocus();
        // if user close dialog box, check additionasl and shapes
        if (answer == MBOX_CLICKED_QUIT) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Confirm closing network' with 'Quit'");
            if (continueWithUnsavedAdditionalChanges()) {
                // clear undo list and return true to continue with closing/reload
                myUndoList->p_clear();
                return true;
            } else {
                return false;
            }
        } else if (answer == MBOX_CLICKED_SAVE) {
            // save newtork
            onCmdSaveNetwork(nullptr, 0, nullptr);
            if (!myUndoList->marked()) {
                // saving failed
                return false;
            }
            if (continueWithUnsavedAdditionalChanges()) {
                // clear undo list and return true to continue with closing/reload
                myUndoList->p_clear();
                return true;
            } else {
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Confirm closing network' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Confirm closing network' with 'ESC'");
            }
            // return false to stop closing/reloading
            return false;
        }
    } else {
        if (continueWithUnsavedAdditionalChanges()) {
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
    if (myViewNet && myFileMenuCommands.saveAdditionals->isEnabled()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save additionals before exit'");
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               "Save additionals before exit", "%s",
                                               "You have unsaved additionals. Do you wish to quit and discard all changes?");
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving additional, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            WRITE_DEBUG("Closed FXMessageBox 'Save additionals before exit' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Save additionals before exit' with 'Yes'");
            if (onCmdSaveAdditionals(nullptr, 0, nullptr) == 1) {
                // additionals sucesfully saved
                return true;
            } else {
                // error saving additionals, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Save additionals before exit' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Save additionals before exit' with 'ESC'");
            }
            // abort saving
            return false;
        }
    } else {
        // nothing to save, return true
        return true;
    }
}


void
GNEApplicationWindow::updateControls() {
    // check that view exists
    if (myViewNet) {
        myViewNet->updateControls();
    }
}


void
GNEApplicationWindow::updateSuperModeMenuCommands(int supermode) {
    // cast supermode
    Supermode currentSupermode = static_cast<Supermode>(supermode);
    if (currentSupermode == Supermode::GNE_SUPERMODE_NETWORK) {
        myNetworkMenuCommands.showNetworkMenuCommands();
        myDemandMenuCommands.hideDemandMenuCommands();
    } else if (currentSupermode == Supermode::GNE_SUPERMODE_DEMAND) {
        myNetworkMenuCommands.hideNetworkMenuCommands();
        myDemandMenuCommands.showDemandMenuCommands();
    } else {
        myNetworkMenuCommands.hideNetworkMenuCommands();
        myDemandMenuCommands.hideDemandMenuCommands();
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindow - protected methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::GNEApplicationWindow() :
    myToolbarsGrip(this),
    myMenuBarFile(this),
    myFileMenuCommands(this),
    myNetworkMenuCommands(this),
    myDemandMenuCommands(this) {
}


long
GNEApplicationWindow::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    const long handled = FXMainWindow::onKeyPress(o, sel, eventData);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onKeyPress(nullptr, sel, eventData);
        }
    }
    return 0;
}


long
GNEApplicationWindow::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    const long handled = FXMainWindow::onKeyRelease(o, sel, eventData);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onKeyRelease(nullptr, sel, eventData);
        }
    }
    return 0;
}

/****************************************************************************/
