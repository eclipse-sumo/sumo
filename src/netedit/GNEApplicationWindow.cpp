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
#include <netimport/NIFrame.h>
#include <netwrite/NWFrame.h>
#include <utils/common/SysUtils.h>
#include <utils/common/SystemFrame.h>
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/shortcuts/GUIShortcutsSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/events/GUIEvent_Message.h>
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
#include "GNEViewNet.h"
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
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Q_CLOSE,    GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_HOTKEY_CTRL_Q_CLOSE,    GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_F4_CLOSE,    GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,                 GNEApplicationWindow::onCmdQuit),

    // toolbar file
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_N_NEWNETWORK,                       GNEApplicationWindow::onCmdNewNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_N_NEWNETWORK,                       GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_NETWORK,                                   GNEApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,                                   GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,                                    GNEApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,                                    GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_OPENFOREIGN,                    GNEApplicationWindow::onCmdOpenForeign),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_OPENFOREIGN,                    GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,                                     GNEApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,                                     GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_R_RELOAD,                           GNEApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_R_RELOAD,                           GNEApplicationWindow::onUpdReload),
    // network
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,       GNEApplicationWindow::onCmdSaveNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,       GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORK_AS,             GNEApplicationWindow::onCmdSaveAsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORK_AS,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_L_SAVEASPLAINXML,                   GNEApplicationWindow::onCmdSaveAsPlainXML),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_L_SAVEASPLAINXML,                   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS,              GNEApplicationWindow::onCmdSaveJoined),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS,              GNEApplicationWindow::onUpdNeedsNetwork),
    // TLS
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,                  GNEApplicationWindow::onCmdOpenTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,                  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_K_SAVETLS,                    GNEApplicationWindow::onCmdSaveTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_K_SAVETLS,                    GNEApplicationWindow::onUpdNeedsNetwork),
    // additionals
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS,  GNEApplicationWindow::onCmdOpenAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS,            GNEApplicationWindow::onCmdSaveAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS,            GNEApplicationWindow::onUpdSaveAdditionals),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,             GNEApplicationWindow::onCmdSaveAdditionalsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,             GNEApplicationWindow::onUpdNeedsNetwork),
    // demand elements
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMAND,  GNEApplicationWindow::onCmdOpenDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMAND,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS,         GNEApplicationWindow::onCmdSaveDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS,         GNEApplicationWindow::onUpdSaveDemandElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,                  GNEApplicationWindow::onCmdSaveDemandElementsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,                  GNEApplicationWindow::onUpdNeedsNetwork),
    // other
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,             GNEApplicationWindow::onCmdSaveTLSProgramsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_W_CLOSESIMULATION,                  GNEApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_W_CLOSESIMULATION,                  GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar supermode
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F3_SUPERMODE_NETWORK,    GNEApplicationWindow::onCmdSetSuperMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F4_SUPERMODE_DEMAND,     GNEApplicationWindow::onCmdSetSuperMode),

    // Toolbar edit
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_E_EDGEMODE,                          GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_M_MOVEMODE,                          GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_D_DELETEMODE,                        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_I_INSPECTMODE,                       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_S_SELECTMODE,                        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE,        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_T_TLSMODE_VTYPEMODE,                 GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_A_ADDITIONALMODE_STOPMODE,           GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE,            GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_Z_TAZMODE,                           GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_P_POLYGONMODE_PERSONMODE,            GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_V_VEHICLEMODE,                       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE,    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWSCHEME,                             GNEApplicationWindow::onCmdEditViewScheme),
    FXMAPFUNC(SEL_UPDATE,   MID_EDITVIEWSCHEME,                             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITVIEWPORT,                               GNEApplicationWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_UPDATE,   MID_EDITVIEWPORT,                               GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Z_UNDO,                         GNEApplicationWindow::onCmdUndo),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_Z_UNDO,                         GNEApplicationWindow::onUpdUndo),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Y_REDO,                         GNEApplicationWindow::onCmdRedo),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_Y_REDO,                         GNEApplicationWindow::onUpdRedo),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID,        GNEApplicationWindow::onCmdToogleGrid),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,              GNEApplicationWindow::onCmdOpenSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,              GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar processing
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND,                       GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND,                       GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE,              GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE,              GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES,     GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES,     GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES,            GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES,            GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS,    GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HOTKEY_F10_OPTIONSMENU,                             GNEApplicationWindow::onCmdOptions),

    // Toolbar locate
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,     GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEJUNCTION,     GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEEDGE,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,      GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEVEHICLE,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEROUTE,        GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEROUTE,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATESTOP,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATESTOP,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATETLS,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEADD,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOI,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOLY,         GNEApplicationWindow::onUpdNeedsNetwork),

    // toolbar windows
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW,     GNEApplicationWindow::onCmdClearMsgWindow),

    // toolbar help
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F1_ONLINEDOCUMENTATION,  GNEApplicationWindow::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F2_ABOUT,                GNEApplicationWindow::onCmdAbout),

    // alt + <number>
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_0_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_0_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_1_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_1_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_2_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_2_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_3_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_3_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_4_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_4_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_5_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_5_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_6_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_6_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_7_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_7_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_8_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_8_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_9_TOOGLEEDITOPTION,      GNEApplicationWindow::onCmdToogleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_9_TOOGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),

    // key events
    FXMAPFUNC(SEL_KEYPRESS,     0,                          GNEApplicationWindow::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                          GNEApplicationWindow::onKeyRelease),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_HOTKEY_ESC,         GNEApplicationWindow::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_HOTKEY_DEL,         GNEApplicationWindow::onCmdDel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_HOTKEY_ENTER,       GNEApplicationWindow::onCmdEnter),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_HOTKEY_BACKSPACE,   GNEApplicationWindow::onCmdBackspace),

    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT,   ID_LOADTHREAD_EVENT,    GNEApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,         ID_LOADTHREAD_EVENT,    GNEApplicationWindow::onLoadThreadEvent),

    // Other
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST,    0,                                  GNEApplicationWindow::onClipboardRequest),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_F12_FOCUSUPPERELEMENT,   GNEApplicationWindow::onCmdFocusFrame),
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
    myEditMenuCommands(this),
    myProcessingMenuCommands(this),
    myLocateMenuCommands(this),
    mySupermodeCommands(this),
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
    // build NETEDIT Accelerators (hotkeys)
    GUIShortcutsSubSys::buildNETEDITAccelerators(this);
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
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
        OptionsCont& oc = OptionsCont::getOptions();
        GNELoadThread::fillOptions(oc);
        GNELoadThread::setDefaultOptions(oc);
        loadConfigOrNet("", true, false, true, true);
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenConfiguration(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
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
}


long
GNEApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
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
}


long
GNEApplicationWindow::onCmdOpenForeign(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
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
                // https://sumo.dlr.de/wiki/Networks/Import/OpenStreetMap#Recommended_NETCONVERT_Options
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
}


long
GNEApplicationWindow::onCmdOpenAdditionals(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open additional dialog");
    // get the Additional file name
    FXFileDialog opendialog(this, "Open Additionals file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Additional files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close additional dialog");
        // udpate current folder
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
            WRITE_ERROR("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->p_end();
        update();
        // restore validation for additionals
        XMLSubSys::setValidation("auto", "auto");
    } else {
        // write debug information
        WRITE_DEBUG("Cancel additional dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenTLSPrograms(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open TLSProgram dialog");
    // get the shape file name
    FXFileDialog opendialog(this, "Open TLSPrograms file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_MODETLS));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("TLSProgram files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close TLSProgram dialog");
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // Run parser
        myUndoList->p_begin("Loading TLS Programs from '" + file + "'");
        myNet->computeNetwork(this);
        if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->parseTLSPrograms(file) == false) {
            // Abort undo/redo
            myUndoList->abort();
        } else {
            // commit undo/redo operation
            myUndoList->p_end();
            update();
        }
    } else {
        // write debug information
        WRITE_DEBUG("Cancel TLSProgram dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenDemandElements(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open demand element dialog");
    // get the demand element file name
    FXFileDialog opendialog(this, "Open demand element file");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Demand element files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close demand element dialog");
        // udpate current folder
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
            WRITE_ERROR("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->p_end();
        update();
        // restore validation for demand
        XMLSubSys::setValidation("auto", "auto");
    } else {
        // write debug information
        WRITE_DEBUG("Cancel demand element dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenRecent(FXObject* sender, FXSelector, void* fileData) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    } else {
        std::string file((const char*)fileData);
        loadConfigOrNet(file, sender == &myMenuBarFile.myRecentNets);
        return 1;
    }
}


long
GNEApplicationWindow::onCmdReload(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
        // @note. If another network has been load during this session, it might not be desirable to set useStartupOptions
        loadConfigOrNet(OptionsCont::getOptions().getString("sumo-net-file"), true, true);
        return 1;
    }
}


long
GNEApplicationWindow::onCmdClose(FXObject*, FXSelector, void*) {
    if (continueWithUnsavedChanges()) {
        closeAllWindows();
        // disable save additionals and TLS menu
        disableSaveAdditionalsMenu();
        myFileMenuCommands.saveTLSPrograms->disable();
        // hide all Supermode, Network and demand commands
        mySupermodeCommands.hideSupermodeCommands();
        myEditMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myEditMenuCommands.demandMenuCommands.hideDemandMenuCommands();
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
        // obtain vector of additional files
        std::vector<std::string> additionalFiles = oc.getStringVector("additional-files");
        // begin undolist
        myUndoList->p_begin("Loading additionals and shapes from '" + toString(additionalFiles) + "'");
        // iterate over every additional file
        for (const auto& additionalFile : additionalFiles) {
            WRITE_MESSAGE("Loading additionals and shapes from '" + additionalFile + "'");
            GNEAdditionalHandler additionalHandler(additionalFile, myNet->getViewNet());
            // disable validation for additionals
            XMLSubSys::setValidation("never", "auto");
            // Run parser
            if (!XMLSubSys::runParser(additionalHandler, additionalFile, false)) {
                WRITE_ERROR("Loading of " + additionalFile + " failed.");
            }
            // disable validation for additionals
            XMLSubSys::setValidation("auto", "auto");
        }

        myUndoList->p_end();
    }
    // check if demand elements has to be loaded at start
    if (oc.isSet("route-files") && !oc.getString("route-files").empty() && myNet) {
        // obtain vector of route files
        std::vector<std::string> demandElementsFiles = oc.getStringVector("route-files");
        // begin undolist
        myUndoList->p_begin("Loading demand elements from '" + toString(demandElementsFiles) + "'");
        // iterate over every route file
        for (const auto& demandElementsFile : demandElementsFiles) {
            WRITE_MESSAGE("Loading demand elements from '" + demandElementsFile + "'");
            GNERouteHandler routeHandler(demandElementsFile, myNet->getViewNet());
            // disable validation for demand elements
            XMLSubSys::setValidation("never", "auto");
            if (!XMLSubSys::runParser(routeHandler, demandElementsFile, false)) {
                WRITE_ERROR("Loading of " + demandElementsFile + " failed.");
            }
            // disable validation for demand elements
            XMLSubSys::setValidation("auto", "auto");
        }

        myUndoList->p_end();
    }
    // check if additionals output must be changed
    if (oc.isSet("additionals-output")) {
        // overwrite "additional-files" with value "additionals-output"
        oc.resetWritable();
        oc.set("additional-files", oc.getString("additionals-output"));
    }
    // check if demand elements output must be changed
    if (oc.isSet("demandelements-output")) {
        // overwrite "route-files" with value "demandelements-output"
        oc.resetWritable();
        oc.set("route-files", oc.getString("demandelements-output"));
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
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK);
    new FXMenuCommand(fileMenu,
                      "Save Net&work As...\tCtrl+Shift+S\tSave the network in another file.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORK_AS);
    new FXMenuCommand(fileMenu,
                      "Save plain XM&L...\tCtrl+L\tSave plain xml representation the network.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_L_SAVEASPLAINXML);
    new FXMenuCommand(fileMenu,
                      "Save &joined junctions...\tCtrl+J\tSave log of joined junctions (allows reproduction of joins).",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS);
    // create Additionals menu options
    myGNEApp->myFileMenuAdditionals = new FXMenuPane(myGNEApp);
    new FXMenuCommand(myGNEApp->myFileMenuAdditionals,
                      "Load A&dditionals...\tCtrl+A\tLoad additionals and shapes.",
                      GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS);
    saveAdditionals = new FXMenuCommand(myGNEApp->myFileMenuAdditionals,
                                        "Save Additionals\tCtrl+Shift+A\tSave additionals and shapes.",
                                        GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS);
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
                      GUIIconSubSys::getIcon(ICON_OPEN_TLSPROGRAMS), myGNEApp, MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS);
    saveTLSPrograms = new FXMenuCommand(myGNEApp->myFileMenuTLS,
                                        "Save TLS Programs \tCtrl+Shift+K\tSave TLS Programs of all Traffic Lights of the current net.",
                                        GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_K_SAVETLS);
    saveTLSPrograms->disable();
    new FXMenuCommand(myGNEApp->myFileMenuTLS,
                      "Save TLS Programs As...\t\tSave TLS Programs of all Traffic Lights of the current net in another file.",
                      GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS);
    new FXMenuCascade(fileMenu, "Traffic Lights", GUIIconSubSys::getIcon(ICON_MODETLS), myGNEApp->myFileMenuTLS);
    // create DemandElements menu options
    myGNEApp->myFileMenuDemandElements = new FXMenuPane(myGNEApp);
    new FXMenuCommand(myGNEApp->myFileMenuDemandElements,
                      "Load demand elements...\tCtrl+D\tLoad demand elements.",
                      GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMAND);
    saveDemandElements = new FXMenuCommand(myGNEApp->myFileMenuDemandElements,
                                           "Save demand elements\tCtrl+Shift+D\tSave demand elements.",
                                           GUIIconSubSys::getIcon(ICON_SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS);
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
// GNEApplicationWindow::EditMenuCommands::NetworkMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::EditMenuCommands::NetworkMenuCommands::NetworkMenuCommands(const EditMenuCommands* editMenuCommandsParent) :
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
    myEditMenuCommandsParent(editMenuCommandsParent) {
}


void
GNEApplicationWindow::EditMenuCommands::NetworkMenuCommands::showNetworkMenuCommands() {
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
GNEApplicationWindow::EditMenuCommands::NetworkMenuCommands::hideNetworkMenuCommands() {
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
GNEApplicationWindow::EditMenuCommands::NetworkMenuCommands::buildNetworkMenuCommands(FXMenuPane* editMenu) {
    // build every FXMenuCommand giving it a shortcut
    createEdgeMode = new FXMenuCommand(editMenu,
                                       "&Edge mode\tE\tCreate junction and edges.",
                                       GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_E_EDGEMODE);
    moveMode = new FXMenuCommand(editMenu,
                                 "&Move mode\tM\tMove elements.",
                                 GUIIconSubSys::getIcon(ICON_MODEMOVE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_M_MOVEMODE);
    deleteMode = new FXMenuCommand(editMenu,
                                   "&Delete mode\tD\tDelete elements.",
                                   GUIIconSubSys::getIcon(ICON_MODEDELETE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_D_DELETEMODE);
    inspectMode = new FXMenuCommand(editMenu,
                                    "&Inspect mode\tI\tInspect elements and change their attributes.",
                                    GUIIconSubSys::getIcon(ICON_MODEINSPECT), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_I_INSPECTMODE);
    selectMode = new FXMenuCommand(editMenu,
                                   "&Select mode\tS\tSelect elements.",
                                   GUIIconSubSys::getIcon(ICON_MODESELECT), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_S_SELECTMODE);
    connectMode = new FXMenuCommand(editMenu,
                                    "&Connection mode\tC\tEdit connections between lanes.",
                                    GUIIconSubSys::getIcon(ICON_MODECONNECTION), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE);
    prohibitionMode = new FXMenuCommand(editMenu,
                                        "Pro&hibition mode\tW\tEdit connection prohibitions.",
                                        GUIIconSubSys::getIcon(ICON_MODEPROHIBITION), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE);
    TLSMode = new FXMenuCommand(editMenu,
                                "&Traffic light mode\tT\tEdit traffic lights over junctions.",
                                GUIIconSubSys::getIcon(ICON_MODETLS), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_T_TLSMODE_VTYPEMODE);
    additionalMode = new FXMenuCommand(editMenu,
                                       "&Additional mode\tA\tCreate additional elements.",
                                       GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_A_ADDITIONALMODE_STOPMODE);
    crossingMode = new FXMenuCommand(editMenu,
                                     "C&rossing mode\tR\tCreate crossings between edges.",
                                     GUIIconSubSys::getIcon(ICON_MODECROSSING), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE);
    TAZMode = new FXMenuCommand(editMenu,
                                "TA&Z mode\tZ\tCreate Traffic Assignment Zones.",
                                GUIIconSubSys::getIcon(ICON_MODETAZ), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_Z_TAZMODE);
    shapeMode = new FXMenuCommand(editMenu,
                                  "&POI-Poly mode\tP\tCreate Points-Of-Interest and polygons.",
                                  GUIIconSubSys::getIcon(ICON_MODEPOLYGON), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_P_POLYGONMODE_PERSONMODE);
    // build separator
    myHorizontalSeparator = new FXMenuSeparator(editMenu);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindow::EditMenuCommands::DemandMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::EditMenuCommands::DemandMenuCommands::DemandMenuCommands(const EditMenuCommands* editMenuCommandsParent) :
    routeMode(nullptr),
    vehicleMode(nullptr),
    vehicleTypeMode(nullptr),
    stopMode(nullptr),
    personTypeMode(nullptr),
    personMode(nullptr),
    personPlanMode(nullptr),
    myEditMenuCommandsParent(editMenuCommandsParent) {
}


void
GNEApplicationWindow::EditMenuCommands::DemandMenuCommands::showDemandMenuCommands() {
    routeMode->show();
    vehicleMode->show();
    vehicleTypeMode->show();
    stopMode->show();
    personTypeMode->show();
    personMode->show();
    personPlanMode->show();
    // also show separator
    myHorizontalSeparator->show();
}


void
GNEApplicationWindow::EditMenuCommands::DemandMenuCommands::hideDemandMenuCommands() {
    routeMode->hide();
    vehicleMode->hide();
    vehicleTypeMode->hide();
    stopMode->hide();
    personTypeMode->hide();
    personMode->hide();
    personPlanMode->hide();
    // also hide separator
    myHorizontalSeparator->hide();
}


void
GNEApplicationWindow::EditMenuCommands::DemandMenuCommands::buildDemandMenuCommands(FXMenuPane* editMenu) {
    // build every FXMenuCommand giving it a shortcut
    routeMode = new FXMenuCommand(editMenu,
                                  "Route mode\tR\tCreate Routes.",
                                  GUIIconSubSys::getIcon(ICON_MODEROUTE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE);
    vehicleMode = new FXMenuCommand(editMenu,
                                    "Vehicle mode\tV\tCreate vehicles.",
                                    GUIIconSubSys::getIcon(ICON_MODEVEHICLE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_V_VEHICLEMODE);
    vehicleTypeMode = new FXMenuCommand(editMenu,
                                        "Vehicle type mode\tT\tCreate vehicle types.",
                                        GUIIconSubSys::getIcon(ICON_MODEVEHICLETYPE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_T_TLSMODE_VTYPEMODE);
    stopMode = new FXMenuCommand(editMenu,
                                 "Stop mode\tA\tCreate stops.",
                                 GUIIconSubSys::getIcon(ICON_MODESTOP), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_A_ADDITIONALMODE_STOPMODE);
    personTypeMode = new FXMenuCommand(editMenu,
                                       "Person type mode\tW\tCreate person types.",
                                       GUIIconSubSys::getIcon(ICON_MODEPERSONTYPE), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE);
    personMode = new FXMenuCommand(editMenu,
                                   "Person mode\tP\tCreate persons.",
                                   GUIIconSubSys::getIcon(ICON_MODEPERSON), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_P_POLYGONMODE_PERSONMODE);
    personPlanMode = new FXMenuCommand(editMenu,
                                       "Person plan mode\tC\tCreate person plans.",
                                       GUIIconSubSys::getIcon(ICON_MODEPERSONPLAN), myEditMenuCommandsParent->myGNEApp, MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE);
    // build separator
    myHorizontalSeparator = new FXMenuSeparator(editMenu);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindow::EditMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::EditMenuCommands::EditMenuCommands(GNEApplicationWindow* GNEApp) :
    networkMenuCommands(this),
    demandMenuCommands(this),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::EditMenuCommands::buildEditMenuCommands(FXMenuPane* fileMenu) {
    // build undo/redo command
    undoLastChange = new FXMenuCommand(fileMenu,
                                       "&Undo\tCtrl+Z\tUndo the last change.",
                                       GUIIconSubSys::getIcon(ICON_UNDO), myGNEApp, MID_HOTKEY_CTRL_Z_UNDO);
    redoLastChange = new FXMenuCommand(fileMenu,
                                       "&Redo\tCtrl+Y\tRedo the last change.",
                                       GUIIconSubSys::getIcon(ICON_REDO), myGNEApp, MID_HOTKEY_CTRL_Y_REDO);
    // build separator
    new FXMenuSeparator(fileMenu);
    // build Supermode commands and hide it
    myGNEApp->mySupermodeCommands.buildSupermodeCommands(fileMenu);
    myGNEApp->mySupermodeCommands.hideSupermodeCommands();
    // build Network modes commands and hide it
    networkMenuCommands.buildNetworkMenuCommands(fileMenu);
    networkMenuCommands.hideNetworkMenuCommands();
    // build Demand Modes commands
    demandMenuCommands.buildDemandMenuCommands(fileMenu);
    demandMenuCommands.hideDemandMenuCommands();
    editViewScheme = new FXMenuCommand(fileMenu,
                                       "Edit Visualisation\tCtrl+V\tOpens a dialog for editing visualization settings.",
                                       nullptr, myGNEApp, MID_EDITVIEWSCHEME);
    editViewPort = new FXMenuCommand(fileMenu,
                                     "Edit Viewport\tCtrl+I\tOpens a dialog for editing viewing are, zoom and rotation.",
                                     nullptr, myGNEApp, MID_EDITVIEWPORT);
    toogleGrid = new FXMenuCommand(fileMenu,
                                   "Toggle Grid\tCtrl+G\tToggles background grid (and snap-to-grid functionality).",
                                   nullptr, myGNEApp, MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID);
    new FXMenuSeparator(fileMenu);
    openInSUMOGUI = new FXMenuCommand(fileMenu,
                                      "Open in SUMO GUI\tCtrl+T\tOpens the SUMO GUI application with the current network.",
                                      GUIIconSubSys::getIcon(ICON_SUMO_MINI), myGNEApp, MID_HOTKEY_CTRL_T_OPENSUMONETEDIT);
}

// ---------------------------------------------------------------------------
// GNEViewNet::ProcessingMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::ProcessingMenuCommands::ProcessingMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::ProcessingMenuCommands::buildProcessingMenuCommands(FXMenuPane* fileMenu) {
    // build network processing menu commands
    computeNetwork = new FXMenuCommand(fileMenu,
                                       "Compute Junctions\tF5\tComputes junction shape and logic.",
                                       GUIIconSubSys::getIcon(ICON_COMPUTEJUNCTIONS), myGNEApp, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    computeNetworkVolatile = new FXMenuCommand(fileMenu,
            "Compute Junctions with volatile options\tShift+F5\tComputes junction shape and logic using volatile junctions.",
            GUIIconSubSys::getIcon(ICON_COMPUTEJUNCTIONS), myGNEApp, MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE);
    cleanJunctions = new FXMenuCommand(fileMenu,
                                       "Clean Junctions\tF6\tRemoves solitary junctions.",
                                       GUIIconSubSys::getIcon(ICON_CLEANJUNCTIONS), myGNEApp, MID_GNE_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinJunctions = new FXMenuCommand(fileMenu,
                                      "Join Selected Junctions\tF7\tJoins selected junctions into a single junction.",
                                      GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), myGNEApp, MID_GNE_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    clearInvalidCrossings = new FXMenuCommand(fileMenu,
            "Clean invalid crossings\tF8\tClear invalid crossings.",
            GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), myGNEApp, MID_GNE_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // build demand  processing menu commands
    computeDemand = new FXMenuCommand(fileMenu,
                                      "Compute demand\tF5\tComputes demand elements.",
                                      GUIIconSubSys::getIcon(ICON_COMPUTEDEMAND), myGNEApp, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    cleanRoutes = new FXMenuCommand(fileMenu,
                                    "Clean routes\tF6\tRemoves routes without vehicles.",
                                    GUIIconSubSys::getIcon(ICON_CLEANROUTES), myGNEApp, MID_GNE_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinRoutes = new FXMenuCommand(fileMenu,
                                   "Join routes\tF7\tJoins routes with the same edges.",
                                   GUIIconSubSys::getIcon(ICON_JOINROUTES), myGNEApp, MID_GNE_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    clearInvalidDemandElements = new FXMenuCommand(fileMenu,
            "Clean invalid route elements\tF8\tClear elements with an invalid path (routes, Trips, Flows...).",
            GUIIconSubSys::getIcon(ICON_JOINJUNCTIONS), myGNEApp, MID_GNE_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // add separator (because optionsmenu is commmon for Network and Demand modes
    new FXMenuSeparator(fileMenu);
    optionMenus = new FXMenuCommand(fileMenu,
                                    "Options\tF10\t\tConfigure Processing Options.",
                                    GUIIconSubSys::getIcon(ICON_OPTIONS), myGNEApp, MID_GNE_HOTKEY_F10_OPTIONSMENU);
}


void
GNEApplicationWindow::ProcessingMenuCommands::showNetworkProcessingMenuCommands() {
    // first enable menu commands
    computeNetwork->enable();
    computeNetworkVolatile->enable();
    cleanJunctions->enable();
    joinJunctions->enable();
    clearInvalidCrossings->enable();
    // now show it
    computeNetwork->show();
    computeNetworkVolatile->show();
    cleanJunctions->show();
    joinJunctions->show();
    clearInvalidCrossings->show();
}


void
GNEApplicationWindow::ProcessingMenuCommands::hideNetworkProcessingMenuCommands() {
    // first disable menu commands
    computeNetwork->disable();
    computeNetworkVolatile->disable();
    cleanJunctions->disable();
    joinJunctions->disable();
    clearInvalidCrossings->disable();
    // now hide it
    computeNetwork->hide();
    computeNetworkVolatile->hide();
    cleanJunctions->hide();
    joinJunctions->hide();
    clearInvalidCrossings->hide();
}


void
GNEApplicationWindow::ProcessingMenuCommands::showDemandProcessingMenuCommands() {
    // first enable menu commands
    computeDemand->enable();
    cleanRoutes->enable();
    joinRoutes->enable();
    clearInvalidDemandElements->enable();
    // now show it
    computeDemand->show();
    cleanRoutes->show();
    joinRoutes->show();
    clearInvalidDemandElements->show();
}


void
GNEApplicationWindow::ProcessingMenuCommands::hideDemandProcessingMenuCommands() {
    // first disable menu commands
    computeDemand->disable();
    cleanRoutes->disable();
    joinRoutes->disable();
    clearInvalidDemandElements->disable();
    // now hide it
    computeDemand->hide();
    cleanRoutes->hide();
    joinRoutes->hide();
    clearInvalidDemandElements->hide();
}

// ---------------------------------------------------------------------------
// GNEViewNet::LocateMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::LocateMenuCommands::LocateMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::LocateMenuCommands::buildLocateMenuCommands(FXMenuPane* fileMenu) {
    // build locate menu commands
    new FXMenuCommand(fileMenu,
                      "Locate &Junctions\tShift+J\tOpen a dialog for locating a Junction.",
                      GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), myGNEApp, MID_LOCATEJUNCTION);
    new FXMenuCommand(fileMenu,
                      "Locate &Edges\tShift+E\tOpen a dialog for locating an Edge.",
                      GUIIconSubSys::getIcon(ICON_LOCATEEDGE), myGNEApp, MID_LOCATEEDGE);
    new FXMenuCommand(fileMenu,
                      "Locate &Vehicles\tShift+V\tOpen a dialog for locating a Vehicle.",
                      GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), myGNEApp, MID_LOCATEVEHICLE);
    new FXMenuCommand(fileMenu,
                      "Locate &Route\tShift+R\tOpen a dialog for locating a Route.",
                      GUIIconSubSys::getIcon(ICON_LOCATEROUTE), myGNEApp, MID_LOCATEROUTE);
    new FXMenuCommand(fileMenu,
                      "Locate &Stops\tShift+S\tOpen a dialog for locating a Stop.",
                      GUIIconSubSys::getIcon(ICON_LOCATESTOP), myGNEApp, MID_LOCATESTOP);
    new FXMenuCommand(fileMenu,
                      "Locate &TLS\tShift+T\tOpen a dialog for locating a Traffic Light.",
                      GUIIconSubSys::getIcon(ICON_LOCATETLS), myGNEApp, MID_LOCATETLS);
    new FXMenuCommand(fileMenu,
                      "Locate &Additional\tShift+A\tOpen a dialog for locating an Additional Structure.",
                      GUIIconSubSys::getIcon(ICON_LOCATEADD), myGNEApp, MID_LOCATEADD);
    new FXMenuCommand(fileMenu,
                      "Locate P&oI\tShift+O\tOpen a dialog for locating a Point of Interest.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPOI), myGNEApp, MID_LOCATEPOI);
    new FXMenuCommand(fileMenu,
                      "Locate Po&lygon\tShift+L\tOpen a dialog for locating a Polygon.",
                      GUIIconSubSys::getIcon(ICON_LOCATEPOLY), myGNEApp, MID_LOCATEPOLY);
}

// ---------------------------------------------------------------------------
// GNEViewNet::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::SupermodeCommands::SupermodeCommands(GNEApplicationWindow* GNEApp) :
    networkMode(nullptr),
    demandMode(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindow::SupermodeCommands::showSupermodeCommands() {
    networkMode->show();
    demandMode->show();
    // also show separator
    myHorizontalSeparator->show();
}


void
GNEApplicationWindow::SupermodeCommands::hideSupermodeCommands() {
    networkMode->hide();
    demandMode->hide();
    // also hide separator
    myHorizontalSeparator->hide();
}


void
GNEApplicationWindow::SupermodeCommands::buildSupermodeCommands(FXMenuPane* editMenu) {
    // build supermode menu commands
    networkMode = new FXMenuCommand(editMenu, "&Network mode\tF3\tSelect network mode.",
                                    GUIIconSubSys::getIcon(ICON_SUPERMODENETWORK), myGNEApp, MID_HOTKEY_F3_SUPERMODE_NETWORK);
    demandMode = new FXMenuCommand(editMenu, "&Demand mode\tF4\tSelect demand mode.",
                                   GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND), myGNEApp, MID_HOTKEY_F4_SUPERMODE_DEMAND);
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
    myEditMenuCommands.buildEditMenuCommands(myEditMenu);
    // build processing menu (trigger netbuild computations)
    myProcessingMenu = new FXMenuPane(this);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Processing", nullptr, myProcessingMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    myProcessingMenuCommands.buildProcessingMenuCommands(myProcessingMenu);
    // build locate menu
    myLocatorMenu = new FXMenuPane(this);
    menuTitle = new FXMenuTitle(myToolbarsGrip.menu, "&Locate", nullptr, myLocatorMenu, LAYOUT_FIX_HEIGHT);
    menuTitle->setHeight(23);
    myLocateMenuCommands.buildLocateMenuCommands(myLocatorMenu);
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
    storeWindowSizeAndPos();
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    if (isReload) {
        myLoadThread->start();
        setStatusBarText("Reloading.");
    } else {
        gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
        myLoadThread->loadConfigOrNet(file, isNet, useStartupOptions, newNet);
        setStatusBarText("Loading '" + file + "'.");
    }
    // show supermode commands menu
    mySupermodeCommands.showSupermodeCommands();
    // show Network command menus (because Network is the default supermode)
    myEditMenuCommands.networkMenuCommands.showNetworkMenuCommands();
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
    OptionsCont& oc = OptionsCont::getOptions();
    // Disable normalization preserve the given network as far as possible
    oc.set("offset.disable-normalization", "true");
    loadConfigOrNet("", true, false, true, oc.getBool("new"));
}


void
GNEApplicationWindow::setStatusBarText(const std::string& statusBarText) {
    myStatusbar->getStatusLine()->setText(statusBarText.c_str());
    myStatusbar->getStatusLine()->setNormalText(statusBarText.c_str());
}


long
GNEApplicationWindow::computeJunctionWithVolatileOptions() {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare variable to save FXMessageBox outputs.
    FXuint answer = 0;
    // declare string to save paths in wich additionals, shapes and demand will be saved
    std::string additionalsSavePath = oc.getString("additional-files");
    std::string demandElementsSavePath = oc.getString("route-files");
    // write warning if netedit is running in testing mode
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
            if (oc.getString("additional-files") == "") {
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
                    FXString file = MFXUtils::getFilename2Write(this,
                                    "Select name of the demand element file", ".xml",
                                    GUIIconSubSys::getIcon(ICON_MODETLS),
                                    gCurrentFolder).text();
                    // add xml extension
                    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
                    // check that file is valid
                    if (fileWithExtension != "") {
                        // update additional files
                        oc.resetWritable();
                        oc.set("additional-files", fileWithExtension);
                        // set obtanied filename output into additionalsSavePath (can be "")
                        additionalsSavePath = oc.getString("additional-files");
                    }
                }
            }
            // Check if additional must be saved in a temporal directory, if user didn't define a directory for additionals
            if (oc.getString("additional-files") == "") {
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
            if (oc.getString("route-files") == "") {
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
                    FXString file = MFXUtils::getFilename2Write(this,
                                    "Select name of the demand element file", ".xml",
                                    GUIIconSubSys::getIcon(ICON_MODETLS),
                                    gCurrentFolder).text();
                    // add xml extension
                    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
                    // check that file is valid
                    if (fileWithExtension != "") {
                        // update route files
                        oc.resetWritable();
                        oc.set("route-files", fileWithExtension);
                        // set obtanied filename output into demand elementSavePath (can be "")
                        demandElementsSavePath = oc.getString("route-files");
                    }
                }
            }
            // Check if demand element must be saved in a temporal directory, if user didn't define a directory for demand elements
            if (oc.getString("route-files") == "") {
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
        myNet->computeNetwork(this, true, true, additionalsSavePath, demandElementsSavePath);
        updateControls();
        return 1;
    }
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
GNEApplicationWindow::onCmdProcessButton(FXObject*, FXSelector sel, void*) {
    // first check if there is a view
    if (myViewNet) {
        // process depending of supermode
        if (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
            // check what FXMenuCommand was called
            switch (FXSELID(sel)) {
                case MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND:
                    // show extra information for tests
                    WRITE_DEBUG("Key F5 (Compute) pressed");
                    myNet->computeNetwork(this, true, false);
                    updateControls();
                    break;
                case MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE:
                    // show extra information for tests
                    WRITE_DEBUG("Keys Shift + F5 (Compute with volatile options) pressed");
                    computeJunctionWithVolatileOptions();
                    break;
                case MID_GNE_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F6 (Clean junction) pressed");
                    myNet->removeSolitaryJunctions(myUndoList);
                    break;
                case MID_GNE_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F7 (Join junctions) pressed");
                    myNet->joinSelectedJunctions(myUndoList);
                    break;
                case MID_GNE_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS:
                    // show extra information for tests
                    WRITE_DEBUG("Key F8 (Clean invalid crossings) pressed");
                    myNet->cleanInvalidCrossings(myUndoList);
                    break;
                default:
                    break;
            }
        } else {
            // check what FXMenuCommand was called
            switch (FXSELID(sel)) {
                case MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND:
                    // show extra information for tests
                    WRITE_DEBUG("Key F5 (Compute) pressed");
                    myNet->computeDemandElements(this);
                    updateControls();
                    break;
                case MID_GNE_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F6 (RemoveUnusedRoutes) pressed");
                    myNet->cleanUnusedRoutes(myUndoList);
                    break;
                case MID_GNE_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F7 (JoinRoutes) pressed");
                    myNet->joinRoutes(myUndoList);
                    break;
                case MID_GNE_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS:
                    // show extra information for tests
                    WRITE_DEBUG("Key F8 (CleanInvalidDemandElements) pressed");
                    myNet->cleanInvalidDemandElements(myUndoList);
                    break;
                default:
                    break;
            }
        }
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
        if (myViewNet->getViewParent()->getTAZFrame()->getTAZSelectionStatisticsModul()->getEdgeAndTAZChildrenSelected().size() > 0) {
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
GNEApplicationWindow::onCmdBackspace(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key BACKSPACE pressed");
        myViewNet->hotkeyBackSpace();
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
GNEApplicationWindow::onCmdToogleGrid(FXObject* obj, FXSelector sel, void* ptr) {
    // check that view exists
    if (myViewNet) {
        // Toogle getMenuCheckShowGrid of GNEViewNet
        if (myViewNet->getCommonViewOptions().menuCheckShowGrid->getCheck() == TRUE) {
            myViewNet->getCommonViewOptions().menuCheckShowGrid->setCheck(FALSE);
            // show extra information for tests
            WRITE_DEBUG("Disabled grid throught Ctrl+g hotkey");
        } else {
            myViewNet->getCommonViewOptions().menuCheckShowGrid->setCheck(TRUE);
            // show extra information for tests
            WRITE_WARNING("Enabled grid throught Ctrl+g hotkey");
        }
        // Call manually show grid function
        myViewNet->onCmdToogleShowGrid(obj, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToogleEditOptions(FXObject* obj, FXSelector sel, void* ptr) {
    // first check that we have a ViewNet
    if (myViewNet) {
        // first check what selector was called
        int numericalKeyPressed = sel - FXSEL(SEL_COMMAND, MID_HOTKEY_ALT_0_TOOGLEEDITOPTION) - 1;
        // check that numericalKeyPressed is valid
        if ((numericalKeyPressed < 0) || (numericalKeyPressed > 10)) {
            return 1;
        }
        // declare a vector in which save visible menu commands
        std::vector<FXMenuCheck*> visibleMenuCommands;
        // get common, network and demand visible menu commands
        myViewNet->getCommonViewOptions().getVisibleCommonMenuCommands(visibleMenuCommands);
        myViewNet->getNetworkViewOptions().getVisibleNetworkMenuCommands(visibleMenuCommands);
        myViewNet->getDemandViewOptions().getVisibleDemandMenuCommands(visibleMenuCommands);
        // now check that numericalKeyPressed isn't greather than visible view options
        if (numericalKeyPressed >= (int)visibleMenuCommands.size()) {
            return 1;
        }
        // finally function correspond to visibleMenuCommands[numericalKeyPressed]
        if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getCommonViewOptions().menuCheckShowGrid) {
            // Toogle menuCheckShowGrid
            if (myViewNet->getCommonViewOptions().menuCheckShowGrid->getCheck() == TRUE) {
                myViewNet->getCommonViewOptions().menuCheckShowGrid->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled toogle show grid throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getCommonViewOptions().menuCheckShowGrid->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled toogle show grid throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleShowGrid
            return myViewNet->onCmdToogleShowGrid(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckShowDemandElements) {
            // Toogle menuCheckShowDemandElements
            if (myViewNet->getNetworkViewOptions().menuCheckShowDemandElements->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckShowDemandElements->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled show demand elements throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckShowDemandElements->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled show demand elements throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleShowDemandElements
            return myViewNet->onCmdToogleShowDemandElements(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckSelectEdges) {
            // Toogle menuCheckSelectEdges
            if (myViewNet->getNetworkViewOptions().menuCheckSelectEdges->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckSelectEdges->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled select edges throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckSelectEdges->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled select edges throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleSelectEdges
            return myViewNet->onCmdToogleSelectEdges(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckShowConnections) {
            // Toogle menuCheckShowConnections
            if (myViewNet->getNetworkViewOptions().menuCheckShowConnections->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckShowConnections->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled show connections throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckShowConnections->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled show connections throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleShowConnections
            return myViewNet->onCmdToogleShowConnections(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckHideConnections) {
            // Toogle menuCheckHideConnections
            if (myViewNet->getNetworkViewOptions().menuCheckHideConnections->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckHideConnections->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled hide connections throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckHideConnections->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled hide connections throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleHideConnections
            return myViewNet->onCmdToogleHideConnections(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckExtendSelection) {
            // Toogle menuCheckExtendSelection
            if (myViewNet->getNetworkViewOptions().menuCheckExtendSelection->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckExtendSelection->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled extend selection throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckExtendSelection->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled extend selection throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleExtendSelection
            return myViewNet->onCmdToogleExtendSelection(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckChangeAllPhases) {
            // Toogle menuCheckChangeAllPhases
            if (myViewNet->getNetworkViewOptions().menuCheckChangeAllPhases->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckChangeAllPhases->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled change all phases throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckChangeAllPhases->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled change all phases throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleChangeAllPhases
            return myViewNet->onCmdToogleChangeAllPhases(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckWarnAboutMerge) {
            // Toogle menuCheckWarnAboutMerge
            if (myViewNet->getNetworkViewOptions().menuCheckWarnAboutMerge->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckWarnAboutMerge->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled warn about merge throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckWarnAboutMerge->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled warn about merge throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleWarnAboutMerge
            return myViewNet->onCmdToogleWarnAboutMerge(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckShowJunctionBubble) {
            // Toogle menuCheckShowJunctionBubble
            if (myViewNet->getNetworkViewOptions().menuCheckShowJunctionBubble->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckShowJunctionBubble->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled show junction as bubble throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckShowJunctionBubble->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled show junction as bubble throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleShowJunctionBubble
            return myViewNet->onCmdToogleShowJunctionBubbles(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckMoveElevation) {
            // Toogle menuCheckMoveElevation
            if (myViewNet->getNetworkViewOptions().menuCheckMoveElevation->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckMoveElevation->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled move elevation throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckMoveElevation->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled move elevation throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleMoveElevation
            return myViewNet->onCmdToogleMoveElevation(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckChainEdges) {
            // Toogle menuCheckChainEdges
            if (myViewNet->getNetworkViewOptions().menuCheckChainEdges->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckChainEdges->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled chain edges throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckChainEdges->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled chain edges throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleChainEdges
            return myViewNet->onCmdToogleChainEdges(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge) {
            // Toogle menuCheckAutoOppositeEdge
            if (myViewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge->getCheck() == TRUE) {
                myViewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled auto opposite edge throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled auto opposite edge throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleAutoOppositeEdge
            return myViewNet->onCmdToogleAutoOppositeEdge(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getDemandViewOptions().menuCheckHideShapes) {
            // Toogle menuCheckHideShapes
            if (myViewNet->getDemandViewOptions().menuCheckHideShapes->getCheck() == TRUE) {
                myViewNet->getDemandViewOptions().menuCheckHideShapes->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled hide shapes throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getDemandViewOptions().menuCheckHideShapes->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled hide shapes throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleHideNonInspecteDemandElements
            return myViewNet->onCmdToogleHideShapes(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements) {
            // Toogle menuCheckHideNonInspectedDemandElements
            if (myViewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements->getCheck() == TRUE) {
                myViewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled hide non inspected demand elements throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled hide non inspected demand elements throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleHideNonInspecteDemandElements
            return myViewNet->onCmdToogleHideNonInspecteDemandElements(obj, sel, ptr);
        } else if (visibleMenuCommands.at(numericalKeyPressed) == myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans) {
            // Toogle menuCheckShowAllPersonPlans
            if (myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->getCheck() == TRUE) {
                myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setCheck(FALSE);
                // show extra information for tests
                WRITE_DEBUG("Disabled show all person plans throught alt + " + toString(numericalKeyPressed + 1));
            } else {
                myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setCheck(TRUE);
                // show extra information for tests
                WRITE_DEBUG("Enabled show all person plans throught alt + " + toString(numericalKeyPressed + 1));
            }
            // Call manually onCmdToogleHideNonInspecteDemandElements
            return myViewNet->onCmdToogleHideNonInspecteDemandElements(obj, sel, ptr);
        } else {
            // nothing to call
            return 1;
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdHelp(FXObject*, FXSelector, void*) {
    FXLinkLabel::fxexecute("https://sumo.dlr.de/wiki/NETEDIT");
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
GNEApplicationWindow::onCmdUndo(FXObject*, FXSelector, void*) {
    // Undo needs a viewnet and a enabled undoLastChange menu command
    if (myViewNet && myEditMenuCommands.undoLastChange->isEnabled()) {
        myViewNet->getUndoList()->undo();
        // update current show frame after undo
        if (myViewNet->getViewParent()->getCurrentShownFrame()) {
            myViewNet->getViewParent()->getCurrentShownFrame()->updateFrameAfterUndoRedo();
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdRedo(FXObject*, FXSelector, void*) {
    // redo needs a viewnet and a enabled redoLastChange menu command
    if (myViewNet && myEditMenuCommands.redoLastChange->isEnabled()) {
        myViewNet->getUndoList()->redo();
        // update current show frame after redo
        if (myViewNet->getViewParent()->getCurrentShownFrame()) {
            myViewNet->getViewParent()->getCurrentShownFrame()->updateFrameAfterUndoRedo();
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveAsNetwork(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save Network as", ".net.xml",
                    GUIIconSubSys::getIcon(ICON_MODECREATEEDGE),
                    gCurrentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check that file with extension is valid
    if (fileWithExtension != "") {
        OptionsCont& oc = OptionsCont::getOptions();
        oc.resetWritable();
        oc.set("output-file", fileWithExtension);
        setTitle(MFXUtils::getTitleText(myTitlePrefix, fileWithExtension.c_str()));
        onCmdSaveNetwork(nullptr, 0, nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveAsPlainXML(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the plain-xml edge-file (other names will be deduced from this)", "",
                    GUIIconSubSys::getIcon(ICON_MODECREATEEDGE),
                    gCurrentFolder);
    // check that file is valid (note: in this case we don't need to use function FileHelpers::addExtension)
    if (file != "") {
        OptionsCont& oc = OptionsCont::getOptions();
        bool wasSet = oc.isSet("plain-output-prefix");
        std::string oldPrefix = oc.getString("plain-output-prefix");
        std::string prefix = file.text();
        // if the name of an edg.xml file was given, remove the suffix
        if (StringUtils::endsWith(prefix, ".edg.xml")) {
            prefix = prefix.substr(0, prefix.size() - 8);
        }
        if (StringUtils::endsWith(prefix, ".")) {
            prefix = prefix.substr(0, prefix.size() - 1);
        }
        oc.resetWritable();
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
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveJoined(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the joined-junctions file", ".nod.xml",
                    GUIIconSubSys::getIcon(ICON_MODECREATEEDGE),
                    gCurrentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check that file with extension is valid
    if (fileWithExtension != "") {
        OptionsCont& oc = OptionsCont::getOptions();
        bool wasSet = oc.isSet("junctions.join-output");
        std::string oldFile = oc.getString("junctions.join-output");
        oc.resetWritable();
        oc.set("junctions.join-output", fileWithExtension);
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
        myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Joined junctions saved to '" + fileWithExtension + "'.\n");
        myMessageWindow->addSeparator();
        if (wasSet) {
            oc.resetWritable();
            oc.set("junctions.join-output", oldFile);
        } else {
            oc.unSet("junctions.join-output");
        }
        getApp()->endWaitCursor();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdNeedsNetwork(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myNet == nullptr ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdReload(FXObject* sender, FXSelector, void*) {
    sender->handle(this, ((myNet == nullptr) || !OptionsCont::getOptions().isSet("sumo-net-file")) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}

long
GNEApplicationWindow::onUpdSaveAdditionals(FXObject* sender, FXSelector, void*) {
    sender->handle(this, ((myNet == nullptr) || myNet->isAdditionalsSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdSaveDemandElements(FXObject* sender, FXSelector, void*) {
    sender->handle(this, ((myNet == nullptr) || myNet->isDemandElementsSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdUndo(FXObject* obj, FXSelector sel, void* ptr) {
    return myUndoList->p_onUpdUndo(obj, sel, ptr);
}


long
GNEApplicationWindow::onUpdRedo(FXObject* obj, FXSelector sel, void* ptr) {
    return myUndoList->p_onUpdRedo(obj, sel, ptr);
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
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveAdditionals->isEnabled()) {
        // Check if additionals file was already set at start of netedit or with a previous save
        if (oc.getString("additional-files").empty()) {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the additional file", ".xml",
                            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL),
                            gCurrentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (fileWithExtension != "") {
                // change value of "additional-files"
                oc.resetWritable();
                oc.set("additional-files", fileWithExtension);
            } else {
                // None additionals file was selected, then stop function
                return 0;
            }
        }
        // Start saving additionals
        getApp()->beginWaitCursor();
        try {
            myNet->saveAdditionals(oc.getString("additional-files"));
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Additionals saved in " + oc.getString("additional-files") + ".\n");
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
    // Open window to select additional file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the additional file", ".xml",
                    GUIIconSubSys::getIcon(ICON_MODEADDITIONAL),
                    gCurrentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check tat file is valid
    if (fileWithExtension != "") {
        // reset writtable flag
        OptionsCont::getOptions().resetWritable();
        // change value of "additional-files"
        OptionsCont::getOptions().set("additional-files", fileWithExtension);
        // change flag of menu command for save additionals
        myFileMenuCommands.saveAdditionals->enable();
        // save additionals
        return onCmdSaveAdditionals(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveTLSPrograms(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveTLSPrograms->isEnabled()) {
        // Check if TLS Programs file was already set at start of netedit or with a previous save
        if (oc.getString("TLSPrograms-output").empty()) {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the TLS file", ".xml",
                            GUIIconSubSys::getIcon(ICON_MODETLS),
                            gCurrentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (file == "") {
                // None TLS Programs file was selected, then stop function
                return 0;
            } else {
                // change value of "TLSPrograms-output"
                oc.resetWritable();
                oc.set("TLSPrograms-output", fileWithExtension);
            }
        }
        // Start saving TLS Programs
        getApp()->beginWaitCursor();
        try {
            myNet->saveTLSPrograms(oc.getString("TLSPrograms-output"));
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "TLS Programs saved in " + oc.getString("TLSPrograms-output") + ".\n");
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
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check tat file is valid
    if (fileWithExtension != "") {
        // change value of "TLSPrograms-files"
        OptionsCont::getOptions().set("TLSPrograms-output", fileWithExtension);
        // save TLS Programs
        return onCmdSaveTLSPrograms(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElements(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save demand element menu is enabled
    if (myFileMenuCommands.saveDemandElements->isEnabled()) {
        // Check if demand elements file was already set at start of netedit or with a previous save
        if (oc.getString("route-files").empty()) {
            FXString file = MFXUtils::getFilename2Write(this,
                            "Select name of the demand element file", ".xml",
                            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL),
                            gCurrentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (fileWithExtension != "") {
                // change value of "route-files"
                oc.resetWritable();
                oc.set("route-files", fileWithExtension);
            } else {
                // None demand elements file was selected, then stop function
                return 0;
            }
        }
        // Start saving demand elements
        getApp()->beginWaitCursor();
        try {
            myNet->saveDemandElements(oc.getString("route-files"));
            myMessageWindow->appendMsg(EVENT_MESSAGE_OCCURRED, "Demand elements saved in " + oc.getString("route-files") + ".\n");
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
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check that file is correct
    if (fileWithExtension != "") {
        // reset writtable flag
        OptionsCont::getOptions().resetWritable();
        // change value of "route-files"
        OptionsCont::getOptions().set("route-files", fileWithExtension);
        // change flag of menu command for save demand elements
        myFileMenuCommands.saveDemandElements->enable();
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
            if (continueWithUnsavedAdditionalChanges() && continueWithUnsavedDemandElementChanges()) {
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
            if (continueWithUnsavedAdditionalChanges() && continueWithUnsavedDemandElementChanges()) {
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
        if (continueWithUnsavedAdditionalChanges() && continueWithUnsavedDemandElementChanges()) {
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


bool
GNEApplicationWindow::continueWithUnsavedDemandElementChanges() {
    // Check if there are non saved DemandElements
    if (myViewNet && myFileMenuCommands.saveDemandElements->isEnabled()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save demand elements before exit'");
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               "Save demand elements before exit", "%s",
                                               "You have unsaved demand elements. Do you wish to quit and discard all changes?");
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving DemandElement, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before exit' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before exit' with 'Yes'");
            if (onCmdSaveDemandElements(nullptr, 0, nullptr) == 1) {
                // DemandElements sucesfully saved
                return true;
            } else {
                // error saving DemandElements, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before exit' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before exit' with 'ESC'");
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
        myEditMenuCommands.networkMenuCommands.showNetworkMenuCommands();
        myEditMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myProcessingMenuCommands.showNetworkProcessingMenuCommands();
        myProcessingMenuCommands.hideDemandProcessingMenuCommands();
    } else if (currentSupermode == Supermode::GNE_SUPERMODE_DEMAND) {
        myEditMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myEditMenuCommands.demandMenuCommands.showDemandMenuCommands();
        myProcessingMenuCommands.hideNetworkProcessingMenuCommands();
        myProcessingMenuCommands.showDemandProcessingMenuCommands();
    } else {
        myEditMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myEditMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myProcessingMenuCommands.hideNetworkProcessingMenuCommands();
        myProcessingMenuCommands.hideDemandProcessingMenuCommands();
    }
}


void
GNEApplicationWindow::disableUndoRedo(const std::string& reason) {
    myUndoRedoListEnabled = reason;
}


void
GNEApplicationWindow::enableUndoRedo() {
    myUndoRedoListEnabled.clear();
}


const std::string&
GNEApplicationWindow::isUndoRedoEnabled() const {
    return myUndoRedoListEnabled;
}

// ---------------------------------------------------------------------------
// GNEApplicationWindow - protected methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::GNEApplicationWindow() :
    myToolbarsGrip(this),
    myMenuBarFile(this),
    myFileMenuCommands(this),
    myEditMenuCommands(this),
    myProcessingMenuCommands(this),
    myLocateMenuCommands(this),
    mySupermodeCommands(this) {
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
