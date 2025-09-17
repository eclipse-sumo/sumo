/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GUIApplicationWindow.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @author  Mirko Barthauer
/// @date    Sept 2002
///
// The main window of the SUMO-gui.
/****************************************************************************/
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <fxkeys.h>

#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/devices/MSRoutingEngine.h>
#include <netload/NLHandler.h>
#include <traci-server/TraCIServer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/foxtools/MFXLCDLabel.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/foxtools/MFXRealSpinner.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/events/GUIEvent_AddView.h>
#include <utils/gui/events/GUIEvent_CloseView.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/shortcuts/GUIShortcutsSubSys.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/xml/XMLSubSys.h>

#include "GUIApplicationWindow.h"
#include "GUIEvent_SimulationEnded.h"
#include "GUIEvent_SimulationLoaded.h"
#include "GUIGlobals.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "dialogs/GUIDialog_AboutSUMO.h"
#include "dialogs/GUIDialog_Feedback.h"
#include "dialogs/GUIDialog_AppSettings.h"
#include "dialogs/GUIDialog_Breakpoints.h"
#include "dialogs/GUIDialog_HallOfFame.h"


#define MIN_DRAW_DELAY 20
//#define HAVE_DANGEROUS_SOUNDS

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GUIApplicationWindow) GUIApplicationWindowMap[] = {
    // close
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Q_CLOSE,    GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_HOTKEY_CTRL_Q_CLOSE,    GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_F4_CLOSE,    GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,                 GUIApplicationWindow::onCmdQuit),
    // toolbar
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW,                  GUIApplicationWindow::onCmdNewWindow),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK,       GUIApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK,           GUIApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_P_OPENSHAPES,                       GUIApplicationWindow::onCmdOpenShapes),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_U_OPENEDGEDATA,                     GUIApplicationWindow::onCmdOpenEdgeData),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,                                     GUIApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_R_RELOAD,                           GUIApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_QUICK_RELOAD,                       GUIApplicationWindow::onCmdQuickReload),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG,             GUIApplicationWindow::onCmdSaveConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_W_CLOSESIMULATION,                  GUIApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG,  GUIApplicationWindow::onCmdEditChosen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS,  GUIApplicationWindow::onCmdEditBreakpoints),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F9_EDIT_VIEWSCHEME,                      GUIApplicationWindow::onCmdEditViewScheme),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_I_EDITVIEWPORT,                     GUIApplicationWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO,             GUIApplicationWindow::onCmdOpenInNetedit),
    // gaming
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_H_APPSETTINGS_OPENEDGETYPES,    GUIApplicationWindow::onCmdAppSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID,        GUIApplicationWindow::onCmdGaming),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_J_TOGGLEDRAWJUNCTIONSHAPE,      GUIApplicationWindow::onCmdToggleDrawJunctionShape),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,              GUIApplicationWindow::onCmdToggleSecondaryShape),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_F_FULSCREENMODE,                GUIApplicationWindow::onCmdFullScreen),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTINTERNAL,                               GUIApplicationWindow::onCmdListInternal),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTPARKING,                                GUIApplicationWindow::onCmdListParking),
    FXMAPFUNC(SEL_COMMAND,  MID_LISTTELEPORTING,                            GUIApplicationWindow::onCmdListTeleporting),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_MICROVIEW,                              GUIApplicationWindow::onCmdNewView),
    // OSG
#ifdef HAVE_OSG
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_OSGVIEW,    GUIApplicationWindow::onCmdNewOSG),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_OSGVIEW,    GUIApplicationWindow::onUpdAddView),

#endif
    // Time
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS,        GUIApplicationWindow::onCmdStart),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALELEMENTS,   GUIApplicationWindow::onCmdStart),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT,                    GUIApplicationWindow::onCmdStop),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,               GUIApplicationWindow::onCmdStop),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE,              GUIApplicationWindow::onCmdStep),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS,  GUIApplicationWindow::onCmdStep),
    FXMAPFUNC(SEL_COMMAND,  MID_DELAY_INC,                                              GUIApplicationWindow::onCmdDelayInc),
    FXMAPFUNC(SEL_COMMAND,  MID_DELAY_DEC,                                              GUIApplicationWindow::onCmdDelayDec),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMSAVE,                                                GUIApplicationWindow::onCmdSaveState),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMLOAD,                                                GUIApplicationWindow::onCmdLoadState),
    FXMAPFUNC(SEL_COMMAND,  MID_TIME_TOGGLE,                                            GUIApplicationWindow::onCmdTimeToggle),
    FXMAPFUNC(SEL_COMMAND,  MID_DELAY_TOGGLE,                                           GUIApplicationWindow::onCmdDelayToggle),
    FXMAPFUNC(SEL_COMMAND,  MID_DEMAND_SCALE,                                           GUIApplicationWindow::onCmdDemandScale),
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW,                                     GUIApplicationWindow::onCmdClearMsgWindow),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_B_BREAKPOINT,                                    GUIApplicationWindow::onCmdBreakpoint),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_B_BREAKPOINT_EARLY,                          GUIApplicationWindow::onCmdBreakpointEarly),
    // Stats
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWNETSTATS,       GUIApplicationWindow::onCmdShowStats),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWVEHSTATS,       GUIApplicationWindow::onCmdShowStats),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPERSONSTATS,    GUIApplicationWindow::onCmdShowStats),
    // these functions do not assign shortcut keys to commands, but rather affect the button enable status upon other events (e.g. simulation loaded)
    // since those events are invoked through pseudo key events (?), the same key shortcuts as in cmd must be supplied as well
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK,               GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK,                   GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_P_OPENSHAPES,                               GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_U_OPENEDGEDATA,                             GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_R_RELOAD,                                   GUIApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_QUICK_RELOAD,                               GUIApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,                                             GUIApplicationWindow::onUpdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_MICROVIEW,                                          GUIApplicationWindow::onUpdAddView),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALELEMENTS,   GUIApplicationWindow::onUpdStart),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,               GUIApplicationWindow::onUpdStop),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS,  GUIApplicationWindow::onUpdStep),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMSAVE,                                                GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMLOAD,                                                GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG,          GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS,          GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_B_BREAKPOINT,                                    GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_B_BREAKPOINT_EARLY,                          GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F9_EDIT_VIEWSCHEME,                              GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_I_EDITVIEWPORT,                             GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO,                     GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_TOOLBAREDIT_LOADADDITIONALS,                            GUIApplicationWindow::onUpdNeedsSumoConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_TOOLBAREDIT_LOADDEMAND,                                 GUIApplicationWindow::onUpdNeedsSumoConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_DEMAND_SCALE,                                           GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID,                    GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_F_FULSCREENMODE,                            GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_TRACI_STATUS,                                           GUIApplicationWindow::onUpdTraCIStatus),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F1_ONLINEDOCUMENTATION,                          GUIApplicationWindow::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_CHANGELOG,                                              GUIApplicationWindow::onCmdChangelog),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEYS,                                                GUIApplicationWindow::onCmdHotkeys),
    FXMAPFUNC(SEL_COMMAND,  MID_TUTORIAL,                                               GUIApplicationWindow::onCmdTutorial),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F11_HALLOFFAME,                            GUIApplicationWindow::onCmdHallOfFame),
    FXMAPFUNC(SEL_COMMAND,  MID_FEEDBACK,                                               GUIApplicationWindow::onCmdFeedback),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F12_ABOUT,                                       GUIApplicationWindow::onCmdAbout),
    // forward requests to the active view
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,      GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_E_LOCATEEDGE,          GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,       GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_P_LOCATEPERSON,        GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_C_LOCATECONTAINER,     GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_T_LOCATETLS,           GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,    GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_O_LOCATEPOI,           GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_L_LOCATEPOLY,          GUIApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,      GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_E_LOCATEEDGE,          GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,       GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_P_LOCATEPERSON,        GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_C_LOCATECONTAINER,     GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_T_LOCATETLS,           GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,    GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_O_LOCATEPOI,           GUIApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_L_LOCATEPOLY,          GUIApplicationWindow::onUpdNeedsNetwork),
    // languages
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_EN,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_EN,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_DE,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_DE,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_ES,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_ES,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_FR,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_FR,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_IT,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_IT,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_ZH,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_ZH,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_ZHT,   GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_ZHT,   GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_TR,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_TR,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_HU,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_HU,    GUIApplicationWindow::onUpdChangeLanguage),
    FXMAPFUNC(SEL_COMMAND,  MID_LANGUAGE_JA,    GUIApplicationWindow::onCmdChangeLanguage),
    FXMAPFUNC(SEL_UPDATE,   MID_LANGUAGE_JA,    GUIApplicationWindow::onUpdChangeLanguage),
    // keys
    FXMAPFUNC(SEL_KEYPRESS,              0,     GUIApplicationWindow::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,            0,     GUIApplicationWindow::onKeyRelease),
    // clipboard
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0, GUIApplicationWindow::onClipboardRequest),
    // events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT,  GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT, ID_RUNTHREAD_EVENT,   GUIApplicationWindow::onRunThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,       ID_LOADTHREAD_EVENT,  GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,       ID_RUNTHREAD_EVENT,   GUIApplicationWindow::onRunThreadEvent),
};

// Object implementation
FXIMPLEMENT(GUIApplicationWindow, FXMainWindow, GUIApplicationWindowMap, ARRAYNUMBER(GUIApplicationWindowMap))

// ===========================================================================
// static members
// ===========================================================================
std::mt19937 GUIApplicationWindow::myGamingRNG;


// ===========================================================================
// member method definitions
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GUIApplicationWindow::GUIApplicationWindow(FXApp* a) :
    GUIMainWindow(a),
    myFileMenuRecentNetworks(new FXMenuPane(this)),
    myFileMenuRecentConfigs(new FXMenuPane(this)),
    myRecentNetworks(a, "networks"),
    myRecentConfigs(a, "configs"),
    myLastStepEventMillis(SysUtils::getCurrentMillis() - MIN_DRAW_DELAY) {
    // init icons
    GUIIconSubSys::initIcons(a);
    // init Textures
    GUITextureSubSys::initTextures(a);
    // init cursors
    GUICursorSubSys::initCursors(a);
    // disable tooltips
    a->setTooltipTime(1000000000);
    a->setTooltipPause(1000000000);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


GUIRunThread*
GUIApplicationWindow::getRunner() {
    return myRunThread;
}


void
GUIApplicationWindow::dependentBuild(const bool isLibsumo) {
    // don't do this twice
    if (hadDependentBuild) {
        return;
    }
    hadDependentBuild = true;
    setTarget(this);
    setSelector(MID_WINDOW);
    // build menu bar
    myMenuBarDrag = new FXToolBarShell(this, GUIDesignToolBar);
    myMenuBar = new FXMenuBar(myTopDock, myMenuBarDrag, GUIDesignToolbarMenuBar);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    buildToolBars();
    // build the thread - io
    myLoadThreadEvent.setTarget(this);
    myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    myRunThreadEvent.setTarget(this);
    myRunThreadEvent.setSelector(ID_RUNTHREAD_EVENT);
    // build the status bar
    myStatusbar = new FXStatusBar(this, GUIDesignStatusBar);
    {
        // build TraCi info
        myTraCiFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        auto button = GUIDesigns::buildFXButton(myTraCiFrame, "TraCI", "", "", nullptr, this, MID_TRACI_STATUS, GUIDesignButtonStatusBarFixed);
        button->setBackColor(FXRGBA(253, 255, 206, 255));
        if (TraCIServer::getInstance() == nullptr) {
            myTraCiFrame->hide();
        }
        // build geo coordiantes
        myGeoFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myGeoCoordinate = GUIDesigns::buildFXLabel(myGeoFrame, TL("N/A"), "", TL("Original coordinate (before coordinate transformation in netconvert)"), nullptr, LAYOUT_CENTER_Y);
        // build cartesian coordinates
        myCartesianFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myCartesianCoordinate = GUIDesigns::buildFXLabel(myCartesianFrame, TL("N/A"), "", TL("Network coordinate"), nullptr, LAYOUT_CENTER_Y);
        // build buttons
        myStatButtons.push_back(GUIDesigns::buildFXButton(myStatusbar, "-", "", "", GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE), this, MID_SHOWVEHSTATS));
        myStatButtons.push_back(GUIDesigns::buildFXButton(myStatusbar, "-", "", "", GUIIconSubSys::getIcon(GUIIcon::GREENPERSON), this, MID_SHOWPERSONSTATS));
        myStatButtons.back()->hide();
        myStatButtons.push_back(GUIDesigns::buildFXButton(myStatusbar, "-", "", "", GUIIconSubSys::getIcon(GUIIcon::GREENCONTAINER), this, MID_SHOWVEHSTATS));
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
    myMessageWindow = new GUIMessageWindow(myMainSplitter, this);
    // fill menu and tool bar
    fillMenuBar();
    myToolBar6->hide();
    myToolBar7->hide();
    myToolBar9->hide();
    myToolBar10->hide();
    // build additional threads
    myLoadThread = new GUILoadThread(getApp(), this, myEvents, myLoadThreadEvent, isLibsumo);
    myRunThread = new GUIRunThread(getApp(), this, mySimDelay, myEvents, myRunThreadEvent);
    // set the status bar
    setStatusBarText(TL("Ready."));
    // set the caption
    setTitle(MFXUtils::getTitleText("SUMO " VERSION_STRING));
    // start the simulation-thread (it will loop until the application ends deciding by itself whether to perform a step or not)
    myRunThread->start();
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO));
    setMiniIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI));
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
    myWindowMenu->create();
    myLanguageMenu->create();
    myHelpMenu->create();
    FXint textWidth = getApp()->getNormalFont()->getTextWidth("8", 1) * 24;
    myCartesianFrame->setWidth(textWidth);
    myGeoFrame->setWidth(textWidth);
    if (myTestFrame) {
        myTestFrame->setWidth(textWidth);
    }

    show(PLACEMENT_DEFAULT);
    if (!OptionsCont::getOptions().isSet("window-size")) {
        if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 1) {
            maximize();
        }
    }
    myShowTimeAsHMS = (getApp()->reg().readIntEntry("gui", "timeasHMS", 0) == 1);
    myAlternateSimDelay = getApp()->reg().readIntEntry("gui", "alternateSimDelay", 100);
    const std::string& onlineMaps = getApp()->reg().readStringEntry("gui", "onlineMaps", "");
    for (const std::string& entry : StringTokenizer(onlineMaps, "\n").getVector()) {
        const std::vector<std::string> split = StringTokenizer(entry, "\t").getVector();
        if (split.size() == 2) {
            myOnlineMaps[split[0]] = split[1];
        }
    }
    if (myOnlineMaps.empty()) {
        myOnlineMaps["GeoHack"] = "https://geohack.toolforge.org/geohack.php?params=%lat;%lon_scale:1000";
        myOnlineMaps["Google Maps"] = "https://www.google.com/maps?ll=%lat,%lon&t=h&z=18";
        myOnlineMaps["OSM"] = "https://www.openstreetmap.org/?mlat=%lat&mlon=%lon&zoom=18&layers=M";
    }
    updateTimeLCDTooltip();
}


GUIApplicationWindow::~GUIApplicationWindow() {
    myRunThread->prepareDestruction();
    myRunThread->join();
    closeAllWindows();
    // close icons
    GUIIconSubSys::close();
    GUICursorSubSys::close();
    // delete visual
    delete myGLVisual;
    // delete some non-parented windows
    delete myToolBarDrag1;
    delete mySimDelayTarget;
    // delete rest of elements
    delete myFileMenuRecentNetworks;
    delete myFileMenuRecentConfigs;
    delete myRunThread;
    delete myFileMenu;
    delete myEditMenu;
    delete mySelectByPermissions;
    delete mySettingsMenu;
    delete myLocatorMenu;
    delete myControlMenu;
    delete myLanguageMenu;
    delete myWindowMenu;
    delete myHelpMenu;
    delete myLoadThread;

    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        delete e;
    }
    for (auto item : myHotkeyPress) {
        delete item.second;
    }
    for (auto item : myHotkeyRelease) {
        delete item.second;
    }
}


void
GUIApplicationWindow::detach() {
    FXMainWindow::detach();
    myMenuBarDrag->detach();
    myToolBarDrag1->detach();
}


void
GUIApplicationWindow::addToWindowsMenu(FXMenuPane* /*menuPane*/) {
    // unused, implement in children
}


void
GUIApplicationWindow::fillMenuBar() {
    // build file menu
    myFileMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar, TL("&File"), nullptr, myFileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("New Window"), "Ctrl+Shift+N", TL("Open a new sumo-gui window."),
                                           nullptr, this, MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW);
    new FXMenuSeparator(myFileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("&Open Simulation..."), "Ctrl+O", TL("Open a simulation (Configuration file)."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_SUMOCONFIG), this, MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("Open &Network..."), "Ctrl+N", TL("Open a network."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("Open Shapes "), "Ctrl+P", TL("Load POIs and Polygons for visualization."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_SHAPES), this, MID_HOTKEY_CTRL_P_OPENSHAPES);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("Open EdgeData "), "Ctrl+U", TL("Load edge related data for visualization."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_HOTKEY_CTRL_U_OPENEDGEDATA);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("&Reload"), "Ctrl+R", TL("Reloads the simulation / the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_HOTKEY_CTRL_R_RELOAD);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("Quick-Reload"), "Ctrl+0", TL("Reloads the simulation (but not network)."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_HOTKEY_CTRL_QUICK_RELOAD);
    new FXMenuSeparator(myFileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("Save Configuration"), "Ctrl+Shift+S", TL("Save current options as a configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_SUMOCONFIG), this, MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("Close"), "Ctrl+W", TL("Close the simulation."),
                                           GUIIconSubSys::getIcon(GUIIcon::CLOSE), this, MID_HOTKEY_CTRL_W_CLOSESIMULATION);
    new FXMenuSeparator(myFileMenu);
    // build recent files
    buildRecentNetworks(myFileMenu, myFileMenuRecentNetworks);
    buildRecentConfigs(myFileMenu, myFileMenuRecentConfigs);
    new FXMenuSeparator(myFileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("&Quit"), "Ctrl+Q", TL("Quit the Application."),
                                           nullptr, this, MID_HOTKEY_CTRL_Q_CLOSE);
    // build edit menu
    mySelectByPermissions = new FXMenuPane(this);
    std::vector<std::string> vehicleClasses = SumoVehicleClassStrings.getStrings();
    for (const auto& vehicleClass : vehicleClasses) {
        GUIDesigns::buildFXMenuCommand(mySelectByPermissions, vehicleClass, VClassIcons::getVClassIcon(SumoVehicleClassStrings.get(vehicleClass)), this, MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG);
    }
    myEditMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar, TL("&Edit"), nullptr, myEditMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                                           TL("Edit Selected..."), "Ctrl+E", TL("Opens a dialog for editing the list of selected items."),
                                           GUIIconSubSys::getIcon(GUIIcon::FLAG), this, MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG);
    mySelectLanesMenuCascade = new FXMenuCascade(myEditMenu, TL("Select lanes which allow..."), GUIIconSubSys::getIcon(GUIIcon::FLAG), mySelectByPermissions);
    mySelectLanesMenuCascade->setHelpText(TL("Opens a menu for selecting a vehicle class by which to selected lanes."));
    new FXMenuSeparator(myEditMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                                           TL("Set Breakpoint"), "B", TL("Sets a breakpoint at the current simulation step"),
                                           GUIIconSubSys::getIcon(GUIIcon::APP_BREAKPOINTS), this, MID_HOTKEY_B_BREAKPOINT);
    GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                                           TL("Set Breakpoint with offset"), "Alt+B", TL("Sets a breakpoint at the current simulation step + offset configured in application settings"),
                                           GUIIconSubSys::getIcon(GUIIcon::APP_BREAKPOINTS), this, MID_HOTKEY_ALT_B_BREAKPOINT_EARLY);
    GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                                           TL("Edit Breakpoints"), "Ctrl+B", TL("Opens a dialog for editing breakpoints."),
                                           GUIIconSubSys::getIcon(GUIIcon::APP_BREAKPOINTS), this, MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS);
    GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                                           TL("Edit Visualisation"), "F9", TL("Opens a dialog for editing visualization settings."),
                                           GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL), this, MID_HOTKEY_F9_EDIT_VIEWSCHEME);
    GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                                           TL("Edit Viewport"), "Ctrl+I", TL("Opens a dialog for editing viewing area, zoom and rotation."),
                                           GUIIconSubSys::getIcon(GUIIcon::EDITVIEWPORT), this, MID_HOTKEY_CTRL_I_EDITVIEWPORT);
    new FXMenuSeparator(myEditMenu);
    // add open in sumo options
    myLoadAdditionalsInNetedit = GUIDesigns::buildFXMenuCheckbox(myEditMenu, TL("Load additionals in netedit"), TL("Load additionals in netedit."), this, MID_TOOLBAREDIT_LOADADDITIONALS);
    myLoadAdditionalsInNetedit->setCheck(TRUE);
    myLoadDemandInNetedit = GUIDesigns::buildFXMenuCheckbox(myEditMenu, TL("Load demand in netedit"), TL("Load demand in netedit."), this, MID_TOOLBAREDIT_LOADDEMAND);
    myLoadDemandInNetedit->setCheck(FALSE);
    myOpenInNetedit = GUIDesigns::buildFXMenuCommandShortcut(myEditMenu,
                      TL("Open in netedit"), "Ctrl+T", TL("Opens current simulation in NETEDIT."),
                      GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI), this, MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO);
    // build settings menu
    mySettingsMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar, TL("&Settings"), nullptr, mySettingsMenu);
    GUIDesigns::buildFXMenuCommandShortcut(mySettingsMenu,
                                           TL("Application Settings"), "Ctrl+H", TL("Open a Dialog for Application Settings editing."),
                                           nullptr, this, MID_HOTKEY_CTRL_H_APPSETTINGS_OPENEDGETYPES);
    myGamingModeCheckbox = new FXMenuCheck(mySettingsMenu,
                                           TL("Gaming Mode\tCtrl+G\tToggle gaming mode on/off."),
                                           this, MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID);
    GUIDesigns::buildFXMenuCommandShortcut(mySettingsMenu,
                                           TL("Full Screen Mode"), "Ctrl+F", TL("Toggle full screen mode on/off."),
                                           GUIIconSubSys::getIcon(GUIIcon::FULL_SCREEN), this, MID_HOTKEY_CTRL_F_FULSCREENMODE);
    // build Locate menu
    myLocatorMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar, TL("&Locate"), nullptr, myLocatorMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&Junctions"), "Shift+J", TL("Open a dialog for locating a Junction."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), this, MID_HOTKEY_SHIFT_J_LOCATEJUNCTION);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&Edges"), "Shift+E", TL("Open a dialog for locating an Edge."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), this, MID_HOTKEY_SHIFT_E_LOCATEEDGE);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&Vehicles"), "Shift+V", TL("Open a dialog for locating a Vehicle."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), this, MID_HOTKEY_SHIFT_V_LOCATEVEHICLE);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&Persons"), "Shift+P", TL("Open a dialog for locating a Person."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), this, MID_HOTKEY_SHIFT_P_LOCATEPERSON);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&Container"), "Shift+C", TL("Open a dialog for locating a Container."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATECONTAINER), this, MID_HOTKEY_SHIFT_C_LOCATECONTAINER);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&TLS"), "Shift+T", TL("Open a dialog for locating a Traffic Light."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), this, MID_HOTKEY_SHIFT_T_LOCATETLS);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("&Additional"), "Shift+A", TL("Open a dialog for locating an Additional Structure."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), this, MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("P&oI"), "Shift+O", TL("Open a dialog for locating a Point of Interest."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), this, MID_HOTKEY_SHIFT_O_LOCATEPOI);
    GUIDesigns::buildFXMenuCommandShortcut(myLocatorMenu,
                                           TL("Po&lygon"), "Shift+L", TL("Open a dialog for locating a Polygon."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), this, MID_HOTKEY_SHIFT_L_LOCATEPOLY);
    new FXMenuSeparator(myLocatorMenu);
    GUIDesigns::buildFXMenuCheckbox(myLocatorMenu, TL("Show Internal Structures"), TL("Show internal junctions and streets in locator dialog."), this, MID_LISTINTERNAL);
    FXMenuCheck* listParking = GUIDesigns::buildFXMenuCheckbox(myLocatorMenu, TL("Show Parking Vehicles"), TL("Show parking vehicles in locator dialog."), this, MID_LISTPARKING);
    listParking->setCheck(myListParking);
    GUIDesigns::buildFXMenuCheckbox(myLocatorMenu, TL("Show vehicles outside the road network"), TL("Show vehicles that are teleporting or driving remote-controlled outside the road network in locator dialog."), this, MID_LISTTELEPORTING);
    // build control menu
    // the shortcut designator is not only at text in the submenu but also defines the real shortcut key assigned with it!
    // secondary shortcuts (ctrl+A, ctrl+S, ctrl+D) are defined in GUIShortcutsSubSys::buildSUMOAccelerators
    myControlMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar, TL("Simulation"), nullptr, myControlMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TL("Run"), "A,space", TL("Start/ Resume the simulation."),
                                           GUIIconSubSys::getIcon(GUIIcon::START), this, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALELEMENTS);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TLC("Simulation", "Stop"), "S,space", TL("Halt the simulation."),
                                           GUIIconSubSys::getIcon(GUIIcon::STOP), this, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TL("Step"), "D", TL("Perform one simulation step."),
                                           GUIIconSubSys::getIcon(GUIIcon::STEP), this, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TL("Delay+"), "PgUp", TL("Increase simulation step delay."), nullptr, this, MID_DELAY_INC);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TL("Delay-"), "PgDn", TL("Decrease simulation step delay."), nullptr, this, MID_DELAY_DEC);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TL("Save"), "", TL("Save the current simulation state to a file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_SIMSAVE);
    GUIDesigns::buildFXMenuCommandShortcut(myControlMenu,
                                           TL("Load"), "", TL("Load simulation state for the current network from file."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_SIMLOAD);
    // build windows menu
    myWindowMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar, TL("&Window"), nullptr, myWindowMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu, TL("Open new view"), "", TL("Open a new microscopic view."), GUIIconSubSys::getIcon(GUIIcon::MICROVIEW), this, MID_NEW_MICROVIEW);
#ifdef HAVE_OSG
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu, TL("Open new 3D view"), "", TL("Open a new 3D view."), GUIIconSubSys::getIcon(GUIIcon::OSGVIEW), this, MID_NEW_OSGVIEW);
#endif
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu,
                                           TL("Tile &Horizontally"), "", TL("Tile the views horizontally."),
                                           GUIIconSubSys::getIcon(GUIIcon::WINDOWS_TILE_HORI), myMDIClient, FXMDIClient::ID_MDI_TILEHORIZONTAL);
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu,
                                           TL("Tile &Vertically"), "", TL("Tile the views vertically."),
                                           GUIIconSubSys::getIcon(GUIIcon::WINDOWS_TILE_VERT), myMDIClient, FXMDIClient::ID_MDI_TILEVERTICAL);
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu,
                                           TL("Cascade"), "", TL("Cascade the views."),
                                           GUIIconSubSys::getIcon(GUIIcon::WINDOWS_CASCADE),
                                           myMDIClient, FXMDIClient::ID_MDI_CASCADE);
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu,
                                           TL("&Close"), "", TL("Close the selected view."),
                                           nullptr, myMDIClient, FXMDIClient::ID_MDI_CLOSE);
    FXMenuSeparator* sep2 = new FXMenuSeparator(myWindowMenu);
    sep2->setTarget(myMDIClient);
    sep2->setSelector(FXMDIClient::ID_MDI_ANY);
    // for whatever reason, sonar complains in the next line that sep2 may leak, but fox does the cleanup
    GUIDesigns::buildFXMenuCommand(myWindowMenu, "", nullptr, myMDIClient, FXMDIClient::ID_MDI_1);  // NOSONAR
    GUIDesigns::buildFXMenuCommand(myWindowMenu, "", nullptr, myMDIClient, FXMDIClient::ID_MDI_2);
    GUIDesigns::buildFXMenuCommand(myWindowMenu, "", nullptr, myMDIClient, FXMDIClient::ID_MDI_3);
    GUIDesigns::buildFXMenuCommand(myWindowMenu, "", nullptr, myMDIClient, FXMDIClient::ID_MDI_4);
    GUIDesigns::buildFXMenuCommand(myWindowMenu, TL("&Others..."), nullptr, myMDIClient, FXMDIClient::ID_MDI_OVER_5);

    new FXMenuSeparator(myWindowMenu);
    GUIDesigns::buildFXMenuCheckbox(myWindowMenu, TL("Show Status Line"), TL("Toggle the Status Bar on/off."), myStatusbar, FXWindow::ID_TOGGLESHOWN);
    GUIDesigns::buildFXMenuCheckbox(myWindowMenu, TL("Show Message Window"), TL("Toggle the Message Window on/off."), myMessageWindow, FXWindow::ID_TOGGLESHOWN);
    GUIDesigns::buildFXMenuCheckbox(myWindowMenu, TL("Show Simulation Time"), TL("Toggle the Simulation Time on/off."), myToolBar3, FXWindow::ID_TOGGLESHOWN);
    GUIDesigns::buildFXMenuCheckbox(myWindowMenu, TL("Show Simulation Delay"), TL("Toggle the Simulation Delay Entry on/off."), myToolBar4, FXWindow::ID_TOGGLESHOWN);
    addToWindowsMenu(myWindowMenu);

    new FXMenuSeparator(myWindowMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myWindowMenu,
                                           TL("Clear Message Window"), "", TL("Clear the message window."),
                                           GUIIconSubSys::getIcon(GUIIcon::CLEARMESSAGEWINDOW), this, MID_CLEARMESSAGEWINDOW);
    // build windows menu
    buildLanguageMenu(myMenuBar);
    // build help menu
    myHelpMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myMenuBar,
                                 TL("&Help"),
                                 nullptr, myHelpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Online Documentation"), "F1", TL("Open Online documentation."),
                                           nullptr, this, MID_HOTKEY_F1_ONLINEDOCUMENTATION);
    new FXMenuSeparator(myHelpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Changelog"), "", TL("Open Changelog."),
                                           nullptr, this, MID_CHANGELOG);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Hotkeys"), "", TL("Open Hotkeys."),
                                           nullptr, this, MID_HOTKEYS);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Tutorial"), "", TL("Open Tutorial."),
                                           nullptr, this, MID_TUTORIAL);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Feedback"), "", TL("Open feedback dialog."),
                                           nullptr, this, MID_FEEDBACK);
    new FXMenuSeparator(myHelpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&About"), "F12", TL("About sumo-gui."),
                                           GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), this, MID_HOTKEY_F12_ABOUT);
    // build SUMO Accelerators (hotkeys)
    GUIShortcutsSubSys::buildAccelerators(getAccelTable(), this, true);
}


void
GUIApplicationWindow::buildToolBars() {
    // file and simulation tool bar
    {
        myToolBarDrag1 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar1 = new FXToolBar(myTopDock, myToolBarDrag1, GUIDesignToolBarRaisedNextTop);
        new FXToolBarGrip(myToolBar1, myToolBar1, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        // build file tools
        new MFXButtonTooltip(myToolBar1, myStaticTooltipMenu, TL("\tOpen simulation\tOpen a simulation (Configuration file)."),
                             GUIIconSubSys::getIcon(GUIIcon::OPEN_SUMOCONFIG), this, MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK, GUIDesignButtonToolbar);
        new MFXButtonTooltip(myToolBar1, myStaticTooltipMenu, TL("\tOpen network\tOpen a network."),
                             GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK, GUIDesignButtonToolbar);
        new MFXButtonTooltip(myToolBar1, myStaticTooltipMenu, TL("\tReload\tReloads the simulation / the network."),
                             GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_HOTKEY_CTRL_R_RELOAD, GUIDesignButtonToolbar);
    }
    // simulation toolbar
    {
        myToolBarDrag2 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar2 = new FXToolBar(myTopDock, myToolBarDrag2, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar2, myToolBar2, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        // build simulation tools
        new MFXButtonTooltip(myToolBar2, myStaticTooltipMenu, TL("\tRun\tStart/Resume the loaded simulation."),
                             GUIIconSubSys::getIcon(GUIIcon::START), this, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALELEMENTS, GUIDesignButtonToolbar);
        new MFXButtonTooltip(myToolBar2, myStaticTooltipMenu, TL("\tStop\tHalt the running simulation."),
                             GUIIconSubSys::getIcon(GUIIcon::STOP), this, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK, GUIDesignButtonToolbar);
        new MFXButtonTooltip(myToolBar2, myStaticTooltipMenu, TL("\tStep\tPerform a single simulation step."),
                             GUIIconSubSys::getIcon(GUIIcon::STEP), this, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS, GUIDesignButtonToolbar);
    }
    // Simulation Step Display
    {
        myToolBarDrag3 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar3 = new FXToolBar(myTopDock, myToolBarDrag3, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar3, myToolBar3, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        new MFXButtonTooltip(myToolBar3, myStaticTooltipMenu, TL("Time:\tToggle between time formats\tToggle between seconds and hour:minute:seconds display."), nullptr, this, MID_TIME_TOGGLE, GUIDesignButtonToolbarText);

        myLCDLabel = new MFXLCDLabel(myToolBar3, myStaticTooltipMenu, 16, nullptr, 0, JUSTIFY_RIGHT);
        myLCDLabel->setHorizontal(2);
        myLCDLabel->setVertical(6);
        myLCDLabel->setThickness(2);
        myLCDLabel->setGroove(2);
        myLCDLabel->setText("----------------");
    }
    // Simulation Delay
    {
        myToolBarDrag4 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar4 = new FXToolBar(myTopDock, myToolBarDrag4, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar4, myToolBar4, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        new MFXButtonTooltip(myToolBar4, myStaticTooltipMenu, TL("Delay (ms):\tDelay per simulated second\tDelay per simulated second. Click to toggle between the last two delay values."), nullptr, this, MID_DELAY_TOGGLE, GUIDesignButtonToolbarText);
        // create spinner for delay
        mySimDelay = 0;
        mySimDelayTarget = new FXDataTarget(mySimDelay);
        mySimDelaySpinner = new MFXRealSpinner(myToolBar4, 7, mySimDelayTarget, FXDataTarget::ID_VALUE, GUIDesignSpinDial);
        // create slider
        mySimDelaySlider = new FXSlider(myToolBar4, mySimDelayTarget, FXDataTarget::ID_VALUE, GUIDesignSlider);
        mySimDelaySlider->setRange(0, 1000);
        mySimDelaySlider->setHeadSize(10);
        mySimDelaySlider->setIncrement(50);
        mySimDelaySlider->setTickDelta(100);
        mySimDelaySlider->setValue((int)mySimDelay);
        //mySimDelayTarget->setNumberFormat(0);
        //mySimDelayTarget->setIncrements(1, 10, 10);
        mySimDelaySpinner->setIncrement(10);
        mySimDelaySpinner->setRange(0, 10000);
        mySimDelaySpinner->setValue(mySimDelay);
    }
    // Scale traffic (flows and incrementally loaded vehicles)
    {
        myToolBarDrag8 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar8 = new FXToolBar(myTopDock, myToolBarDrag8, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar8, myToolBar8, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        myScaleTrafficTooltip = new MFXLabelTooltip(myToolBar8, myStaticTooltipMenu, TL("Scale Traffic:"), nullptr, LAYOUT_TOP | LAYOUT_LEFT);
        myScaleTrafficTooltip->setHelpText(TL("Scale traffic volume from running flows and from vehicles that are loaded incrementally from route files."));
        myDemandScaleSpinner = new MFXRealSpinner(myToolBar8, 7, this, MID_DEMAND_SCALE, GUIDesignSpinDial);
        myDemandScaleSpinner->setIncrement(0.5);
        myDemandScaleSpinner->setRange(0, 1000);
        myDemandScaleSpinner->setValue(1);
    }
    // Views
    {
        myToolBarDrag5 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar5 = new FXToolBar(myTopDock, myToolBarDrag5, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar5, myToolBar5, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        // build view tools
        new MFXButtonTooltip(myToolBar5, myStaticTooltipMenu, (std::string("\t") + TL("Open new view") + std::string("\t") + TL("Open a new microscopic view.")).c_str(),
                             GUIIconSubSys::getIcon(GUIIcon::MICROVIEW), this, MID_NEW_MICROVIEW, GUIDesignButtonToolbar);
#ifdef HAVE_OSG
        new MFXButtonTooltip(myToolBar5, myStaticTooltipMenu, (std::string("\t") + TL("Open new 3D view") + std::string("\t") + TL("Open a new 3D view.")).c_str(),
                             GUIIconSubSys::getIcon(GUIIcon::OSGVIEW), this, MID_NEW_OSGVIEW, GUIDesignButtonToolbar);
#endif
    }
    /// game specific stuff
    {
        // total waitingTime
        myToolBarDrag6 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar6 = new FXToolBar(myTopDock, myToolBarDrag6, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar6, myToolBar6, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        GUIDesigns::buildFXLabel(myToolBar6, TL("Waiting Time:"), "", TL("Time spent waiting accumulated for all vehicles"), nullptr, LAYOUT_TOP | LAYOUT_LEFT);
        myWaitingTimeLabel = new MFXLCDLabel(myToolBar6, myStaticTooltipMenu, 13, nullptr, 0, JUSTIFY_RIGHT);
        myWaitingTimeLabel->setHorizontal(2);
        myWaitingTimeLabel->setVertical(6);
        myWaitingTimeLabel->setThickness(2);
        myWaitingTimeLabel->setGroove(2);
        myWaitingTimeLabel->setText("-------------");
        // idealistic time loss
        myToolBarDrag7 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar7 = new FXToolBar(myTopDock, myToolBarDrag7, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar7, myToolBar7, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        GUIDesigns::buildFXLabel(myToolBar7, TL("Time Loss:"), "", TL("Time lost due to being unable to drive with maximum speed for all vehicles"), nullptr, LAYOUT_TOP | LAYOUT_LEFT);
        myTimeLossLabel = new MFXLCDLabel(myToolBar7, myStaticTooltipMenu, 13, nullptr, 0, JUSTIFY_RIGHT);
        myTimeLossLabel->setHorizontal(2);
        myTimeLossLabel->setVertical(6);
        myTimeLossLabel->setThickness(2);
        myTimeLossLabel->setGroove(2);
        myTimeLossLabel->setText("-------------");
        // total driving distance
        myToolBarDrag9 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar9 = new FXToolBar(myTopDock, myToolBarDrag9, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar9, myToolBar9, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        GUIDesigns::buildFXLabel(myToolBar9, TL("Distance (km):"), "", TL("Total distance driven by DRT vehicles"), nullptr, LAYOUT_TOP | LAYOUT_LEFT);
        myTotalDistanceLabel = new MFXLCDLabel(myToolBar9, myStaticTooltipMenu, 13, nullptr, 0, JUSTIFY_RIGHT);
        myTotalDistanceLabel->setHorizontal(2);
        myTotalDistanceLabel->setVertical(6);
        myTotalDistanceLabel->setThickness(2);
        myTotalDistanceLabel->setGroove(2);
        myTotalDistanceLabel->setText("-------------");
        // emergency vehicle counts
        myToolBarDrag10 = new FXToolBarShell(this, GUIDesignToolBar);
        myToolBar10 = new FXToolBar(myTopDock, myToolBarDrag10, GUIDesignToolBarRaisedSameTop);
        new FXToolBarGrip(myToolBar10, myToolBar10, FXToolBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
        GUIDesigns::buildFXLabel(myToolBar10, TL("Emergency Vehicle waiting time:"), "", TL("Time spent waiting accumulated for emergency vehicles"), nullptr, LAYOUT_TOP | LAYOUT_LEFT);
        myEmergencyVehicleLabel = new MFXLCDLabel(myToolBar10, myStaticTooltipMenu, 13, nullptr, 0, JUSTIFY_RIGHT);
        myEmergencyVehicleLabel->setHorizontal(2);
        myEmergencyVehicleLabel->setVertical(6);
        myEmergencyVehicleLabel->setThickness(2);
        myEmergencyVehicleLabel->setGroove(2);
        myEmergencyVehicleLabel->setText("-------------");
    }
}


void
GUIApplicationWindow::buildRecentNetworks(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentNetworks) {
    // for whatever reason, sonar complains in the next line that sep1 may leak, but fox does the cleanup
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_1);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_2);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_3);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_4);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_5);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_6);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_7);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_8);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_9);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworks, "", &myRecentNetworks, FXRecentFiles::ID_FILE_10);
    new FXMenuSeparator(fileMenuRecentNetworks);  // NOSONAR, Fox does the cleanup
    GUIDesigns::buildFXMenuCommand(fileMenuRecentNetworks, TL("Cl&ear Recent Networks"), nullptr, &myRecentNetworks, FXRecentFiles::ID_CLEAR);
    GUIDesigns::buildFXMenuCommand(fileMenuRecentNetworks, TL("No Recent Networks"), nullptr, &myRecentNetworks, MFXRecentNetworks::ID_NOFILES);
    // set target
    myRecentNetworks.setTarget(this);
    myRecentNetworks.setSelector(MID_RECENTFILE);
    new FXMenuCascade(fileMenu, TL("Recent Networks"), nullptr, fileMenuRecentNetworks);
}


void
GUIApplicationWindow::buildRecentConfigs(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentConfigs) {
    // for whatever reason, sonar complains in the next line that sep1 may leak, but fox does the cleanup
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_1);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_2);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_3);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_4);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_5);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_6);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_7);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_8);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_9);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigs, "", &myRecentConfigs, FXRecentFiles::ID_FILE_10);
    new FXMenuSeparator(fileMenuRecentConfigs);  // NOSONAR, Fox does the cleanup
    GUIDesigns::buildFXMenuCommand(fileMenuRecentConfigs, TL("Cl&ear Recent Configs"), nullptr, &myRecentConfigs, FXRecentFiles::ID_CLEAR);
    GUIDesigns::buildFXMenuCommand(fileMenuRecentConfigs, TL("No Recent Configs"), nullptr, &myRecentConfigs, MFXRecentNetworks::ID_NOFILES);
    // set target
    myRecentConfigs.setTarget(this);
    myRecentConfigs.setSelector(MID_RECENTFILE);
    new FXMenuCascade(fileMenu, TL("Recent Configs"), nullptr, fileMenuRecentConfigs);
}


long
GUIApplicationWindow::onCmdQuit(FXObject*, FXSelector, void*) {
    storeWindowSizeAndPos();
    getApp()->reg().writeStringEntry("SETTINGS", "basedir", gCurrentFolder.text());
    getApp()->reg().writeIntEntry("SETTINGS", "maximized", isMaximized() ? 1 : 0);
    getApp()->reg().writeIntEntry("gui", "timeasHMS", myShowTimeAsHMS ? 1 : 0);
    getApp()->reg().writeIntEntry("gui", "alternateSimDelay", (int)myAlternateSimDelay);
    closeAllWindows();
    getApp()->exit(0);
    return 1;
}


long
GUIApplicationWindow::onCmdEditChosen(FXObject* menu, FXSelector, void*) {
    FXMenuCommand* mc = dynamic_cast<FXMenuCommand*>(menu);
    if (mc->getText() == StringUtils::replace(TL("Edit Selected..."), "&", "").c_str()) {
        GUIDialog_GLChosenEditor* chooser = new GUIDialog_GLChosenEditor(this, &gSelected);
        chooser->create();
        chooser->show();
    } else {
        if (!myAmLoading && myRunThread->networkAvailable()) {
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
                if (w != nullptr) {
                    // color by selection
                    w->getView()->editVisualisationSettings()->laneColorer.setActive(1);
                }
            }
        }
        updateChildren();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdEditBreakpoints(FXObject*, FXSelector, void*) {
    if (myBreakpointDialog == nullptr) {
        myBreakpointDialog = new GUIDialog_Breakpoints(this, myRunThread->getBreakpoints(), myRunThread->getBreakpointLock(), myRunThread->getSimBegin());
    } else {
        myBreakpointDialog->restore();
        myBreakpointDialog->setFocus();
        myBreakpointDialog->raise();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdEditViewport(FXObject*, FXSelector, void*) {
    if (!myGLWindows.empty()) {
        myGLWindows[0]->getView()->showViewportEditor();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdEditViewScheme(FXObject*, FXSelector, void*) {
    if (!myGLWindows.empty()) {
        myGLWindows[0]->getView()->showViewschemeEditor();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdHelp(FXObject*, FXSelector, void*) {
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/sumo-gui.html");
    return 1;
}


long
GUIApplicationWindow::onCmdChangelog(FXObject*, FXSelector, void*) {
    // update in every version
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/ChangeLog.html");
    return 1;
}


long
GUIApplicationWindow::onCmdHotkeys(FXObject*, FXSelector, void*) {
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/sumo-gui.html#keyboard_shortcuts");
    return 1;
}


long
GUIApplicationWindow::onCmdTutorial(FXObject*, FXSelector, void*) {
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/Tutorials/index.html");
    return 1;
}


long
GUIApplicationWindow::onCmdOpenInNetedit(FXObject*, FXSelector, void*) {
    if (myGLWindows.empty()) {
        return 1;
    }
    FXRegistry reg("SUMO netedit", "netedit");
    reg.read();
    const GUISUMOAbstractView* const v = myGLWindows[0]->getView();
    reg.writeRealEntry("viewport", "x", v->getChanger().getXPos());
    reg.writeRealEntry("viewport", "y", v->getChanger().getYPos());
    reg.writeRealEntry("viewport", "z", v->getChanger().getZPos());
    reg.write();
    std::string netedit = "netedit";
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath != nullptr) {
        std::string newPath = std::string(sumoPath) + "/bin/netedit";
        if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
            netedit = "\"" + newPath + "\"";
        }
    }
    // declare command for calling netedit using the viewport saved in registry
    std::string cmd = netedit + " --registry-viewport";
    // continue depending if we're loading only a network or the entire sumo config
    if (myLoadAdditionalsInNetedit->shown()) {
        cmd += " --sumocfg-file \"" + OptionsCont::getOptions().getString("configuration-file") + "\"";
        // check if ignore additional or demand elements
        if (myLoadAdditionalsInNetedit->getCheck() == FALSE) {
            cmd += " --ignore.additionalelements";
        }
        if (myLoadDemandInNetedit->getCheck() == FALSE) {
            cmd += " --ignore.routeelements";
        }
    } else {
        cmd += " -s \"" + OptionsCont::getOptions().getString("net-file") + "\"";
    }
    // start in background
#ifndef WIN32
    cmd = cmd + " &";
#else
    // see "help start" for the parameters
    cmd = "start /B \"\" " + cmd;
#endif
    WRITE_MESSAGEF(TL("Running %."), cmd);
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(cmd);
    return 1;
}


long
GUIApplicationWindow::onCmdNewWindow(FXObject*, FXSelector, void*) {
    FXRegistry reg("SUMO sumo-gui", "sumo-gui");
    std::string sumo_gui = "sumo-gui";
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath != nullptr) {
        std::string newPath = std::string(sumoPath) + "/bin/sumo_gui";
        if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
            sumo_gui = "\"" + newPath + "\"";
        }
    }
    std::string cmd = sumo_gui;
    // start in background
#ifndef WIN32
    cmd = cmd + " &";
#else
    // see "help start" for the parameters
    cmd = "start /B \"\" " + cmd;
#endif
    WRITE_MESSAGEF(TL("Running %."), cmd);
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(cmd);
    return 1;
}


long
GUIApplicationWindow::onCmdOpenConfiguration(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, TL("Open Simulation Configuration"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_SUMOCONFIG));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(SUMOXMLDefinitions::SumoConfigFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        loadConfigOrNet(file);
        myRecentConfigs.appendFile(file.c_str());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, TL("Open Network"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_NET));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(SUMOXMLDefinitions::NetFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        loadConfigOrNet(file);
        myRecentNetworks.appendFile(file.c_str());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdOpenShapes(FXObject*, FXSelector, void*) {
    // get the shape file name
    FXFileDialog opendialog(this, TL("Open Shapes"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_SHAPES));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(SUMOXMLDefinitions::ShapesFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();

        dynamic_cast<GUIShapeContainer&>(myRunThread->getNet().getShapeContainer()).allowReplacement();
        NLShapeHandler handler(file, myRunThread->getNet().getShapeContainer());
        if (!XMLSubSys::runParser(handler, file, false)) {
            WRITE_MESSAGEF(TL("Loading of % failed."), file);
        }
        update();
        if (myMDIClient->numChildren() > 0) {
            GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
            if (w != nullptr) {
                w->getView()->update();
            }
        }
    }
    return 1;
}


long
GUIApplicationWindow::onCmdOpenEdgeData(FXObject*, FXSelector, void*) {
    // get the shape file name
    FXFileDialog opendialog(this, TL("Open EdgeData"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_NET));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(SUMOXMLDefinitions::EdgeDataFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        if (!GUINet::getGUIInstance()->loadEdgeData(file)) {
            WRITE_MESSAGEF(TL("Loading of % failed."), file);
        }
        update();
        if (myMDIClient->numChildren() > 0) {
            GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
            if (w != nullptr) {
                w->getView()->update();
            }
        }
    }
    return 1;
}


long
GUIApplicationWindow::onCmdReload(FXObject* sender, FXSelector sel, void*) {
    if (!myAmLoading && (sender == nullptr || TraCIServer::getInstance() == nullptr)) {
        storeWindowSizeAndPos();
        getApp()->beginWaitCursor();
        myAmLoading = true;
        myIsReload = sender != nullptr || sel == 1;
        closeAllWindows();
        myLoadThread->start();
        if (sender == nullptr) {
            setStatusBarText(sel == 1 ? TL("Auto-Reloading.") : TL("TraCI-Loading."));
        } else {
            setStatusBarText(TL("Reloading."));
        }
        update();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdQuickReload(FXObject*, FXSelector, void*) {
    if (!myAmLoading) {
        setStatusBarText(TL("Quick-Reloading."));
        MSNet::getInstance()->quickReload();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdOpenRecent(FXObject* /* sender */, FXSelector, void* ptr) {
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText(TL("Already loading!"));
        return 1;
    }
    std::string file((const char*)ptr);
    loadConfigOrNet(file);
    return 1;
}


long
GUIApplicationWindow::onCmdSaveConfig(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, TL("Save SUMO Configuration"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::SAVE));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList(SUMOXMLDefinitions::SumoConfigFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    const std::string file = MFXUtils::assureExtension(opendialog).text();
    std::ofstream out(StringUtils::transcodeToLocal(file));
    if (out.good()) {
        OptionsCont::getOptions().writeConfiguration(out, true, false, false, file, true);
        setStatusBarText(TLF("Configuration saved to %.", file));
    } else {
        setStatusBarText(TLF("Could not save configuration to %.", file));
    }
    out.close();
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
                   myAmLoading || myLoadThread->getFileName() == "" || TraCIServer::getInstance() != nullptr
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
                   myAmLoading || !myRunThread->networkAvailable()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdStart(FXObject*, FXSelector, void*) {
    // check whether a net was loaded successfully
    if (!myRunThread->networkAvailable()) {
        myStatusbar->getStatusLine()->setText(TL("No simulation loaded!"));
        return 1;
    }
    // check whether it was started before and paused;
    if (!myWasStarted) {
        myRunThread->begin();
        myWasStarted = true;
    }
    myRunThread->resume();
    getApp()->forceRefresh(); // only calling myToolBar2->forceRefresh somehow looses keyboard focus
    return 1;
}


long
GUIApplicationWindow::onCmdStop(FXObject*, FXSelector, void*) {
    myRunThread->stop();
    getApp()->forceRefresh(); // only calling myToolBar2->forceRefresh somehow looses keyboard focus
    return 1;
}


long
GUIApplicationWindow::onCmdStep(FXObject*, FXSelector, void*) {
    // check whether a net was loaded successfully
    if (!myRunThread->networkAvailable()) {
        myStatusbar->getStatusLine()->setText(TL("No simulation loaded!"));
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
GUIApplicationWindow::onCmdSaveState(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, TL("Save Simulation State"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::SAVE));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList(SUMOXMLDefinitions::StateFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    const std::string file = MFXUtils::assureExtension(opendialog).text();
    MSStateHandler::saveState(file, MSNet::getInstance()->getCurrentTimeStep(), false);
    setStatusBarText(TLF("Simulation state saved to '%'.", file));
    return 1;
}


long
GUIApplicationWindow::onCmdLoadState(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, TL("Load Simulation State"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList(SUMOXMLDefinitions::StateFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute() && FXStat::exists(opendialog.getFilename())) {
        gCurrentFolder = opendialog.getDirectory();
        const std::string file = opendialog.getFilename().text();
        try {
            MSNet::getInstance()->loadState(file, true);
            setStatusBarText(TLF("State loaded from '%'.", file));
        } catch (ProcessError& e) {
            setStatusBarText(TLF("Failed to load state from '%' (%).", file, e.what()));
        }
    }
    return 1;
}


long
GUIApplicationWindow::onCmdTimeToggle(FXObject*, FXSelector, void*) {
    // toggle show time as HMS
    myShowTimeAsHMS = !myShowTimeAsHMS;
    updateTimeLCDTooltip();
    if (myRunThread->networkAvailable()) {
        updateTimeLCD(myRunThread->getNet().getCurrentTimeStep());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdDelayInc(FXObject*, FXSelector, void*) {
    if (mySimDelay < 10) {
        mySimDelay = 10;
    } else if (mySimDelay >= 20 && mySimDelay < 50) {
        mySimDelay = 50;
    } else if (mySimDelay >= 200 && mySimDelay < 500) {
        mySimDelay = 500;
    } else {
        mySimDelay *= 2;
    }
    if (mySimDelay > 1000) {
        // setting high delay by pressing the key too often is hard to recover from
        mySimDelay = 1000;
    }
    mySimDelaySlider->setValue((int)mySimDelay);
    mySimDelaySpinner->setValue(mySimDelay);
    return 1;
}


long
GUIApplicationWindow::onCmdDelayDec(FXObject*, FXSelector, void*) {
    if (mySimDelay <= 10) {
        mySimDelay = 0;
    } else if (mySimDelay > 20 && mySimDelay <= 50) {
        mySimDelay = 20;
    } else if (mySimDelay > 200 && mySimDelay <= 500) {
        mySimDelay = 200;
    } else {
        mySimDelay /= 2;
    }
    mySimDelaySlider->setValue((int)mySimDelay);
    mySimDelaySpinner->setValue(mySimDelay);
    return 1;
}


long
GUIApplicationWindow::onCmdDelayToggle(FXObject*, FXSelector, void*) {
    const double tmp = myAlternateSimDelay;
    myAlternateSimDelay = mySimDelay;
    mySimDelay = tmp;
    return 1;
}


long
GUIApplicationWindow::onCmdDemandScale(FXObject*, FXSelector, void*) {
    if (myRunThread->networkAvailable()) {
        myRunThread->getNet().getVehicleControl().setScale(myDemandScaleSpinner->getValue());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdClearMsgWindow(FXObject*, FXSelector, void*) {
    myMessageWindow->clear();
    return 1;
}


long
GUIApplicationWindow::onCmdBreakpoint(FXObject*, FXSelector, void*) {
    // see updateTimeLCD for the DELTA_T
    if (myRunThread->networkAvailable()) {
        addBreakpoint(SIMSTEP - DELTA_T);
    }
    return 1;
}


long
GUIApplicationWindow::onCmdBreakpointEarly(FXObject*, FXSelector, void*) {
    // see updateTimeLCD for the DELTA_T
    if (myRunThread->networkAvailable()) {
        addBreakpoint(SIMSTEP - DELTA_T + GUIMessageWindow::getBreakPointOffset());
    }
    return 1;
}


long
GUIApplicationWindow::onUpdStart(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStartable() || myAmLoading
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    if (myRunThread->simulationIsStartable() && !myAmLoading) {
        // bind start simulation with space key
        GUIShortcutsSubSys::changeAccelerator(getAccelTable(), this, KEY_SPACE, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALELEMENTS);
    }
    return 1;
}


long
GUIApplicationWindow::onUpdStop(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   !myRunThread->simulationIsStopable() || myAmLoading
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    if (myRunThread->simulationIsStopable() && !myAmLoading) {
        // bind stop simulation with space key
        GUIShortcutsSubSys::changeAccelerator(getAccelTable(), this, KEY_SPACE, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK);
    }
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
GUIApplicationWindow::onUpdNeedsNetwork(FXObject* sender, FXSelector, void* ptr) {
    // check if there is a loaded network and gui isn't loading
    if (myRunThread->networkAvailable() && !myAmLoading) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), ptr);
        // enable certain elements manually
        mySelectLanesMenuCascade->enable();
        myScaleTrafficTooltip->setTipText(TL("Scale number of vehicles in simulation"));
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), ptr);
        // disable certain elements manually
        mySelectLanesMenuCascade->disable();
        myScaleTrafficTooltip->setTipText("");
    }
    return 1;
}


long
GUIApplicationWindow::onUpdNeedsSumoConfig(FXObject* sender, FXSelector, void* ptr) {
    // check if there is a loaded network and gui isn't loading
    if (myRunThread->networkAvailable() && !myAmLoading && OptionsCont::getOptions().isSet("configuration-file")) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), ptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SHOW), ptr);
        myOpenInNetedit->setText(TL("Open sumo config in netedit"));
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), ptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_HIDE), ptr);
        myOpenInNetedit->setText(TL("Open network in netedit"));
    }
    return 1;
}


long
GUIApplicationWindow::onUpdTraCIStatus(FXObject* /*sender*/, FXSelector, void* /*ptr*/) {
    if (TraCIServer::getInstance()) {
        myTraCiFrame->show();
    } else {
        myTraCiFrame->hide();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdLocate(FXObject*, FXSelector sel, void*) {
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onCmdLocate(nullptr, sel, nullptr);
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
    if (myGLWindows.empty()) {
        return 1;
    }
    myAmGaming = !myAmGaming;
    myGLWindows[0]->getView()->editVisualisationSettings()->gaming = myAmGaming;
    if (myAmGaming) {
        myGamingModeCheckbox->setCheck(TRUE);
        myMenuBar->hide();
        myStatusbar->hide();
        myToolBar1->hide();
        myToolBar2->hide();
        myToolBar4->hide();
        myToolBar5->hide();
        myToolBar6->show();
        myToolBar8->hide();
        myToolBar10->show();
        if (myTLSGame) {
            myToolBar7->show();
        } else {
            myToolBar9->show();
        }
        myMessageWindow->hide();
        myLCDLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        myWaitingTimeLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        myTimeLossLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        myEmergencyVehicleLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
        myTotalDistanceLabel->setFgColor(MFXUtils::getFXColor(RGBColor::RED));
    } else {
        myGamingModeCheckbox->setCheck(FALSE);
        myMenuBar->show();
        myStatusbar->show();
        myToolBar1->show();
        myToolBar2->show();
        myToolBar4->show();
        myToolBar5->show();
        myToolBar6->hide();
        myToolBar7->hide();
        myToolBar8->show();
        myToolBar9->hide();
        myToolBar10->hide();
        myMessageWindow->show();
        myLCDLabel->setFgColor(MFXUtils::getFXColor(RGBColor::GREEN));
    }
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->setToolBarVisibility(!myAmGaming && !myAmFullScreen);
        }
    }
    update();
    return 1;
}


long
GUIApplicationWindow::onCmdToggleDrawJunctionShape(FXObject*, FXSelector, void*) {
    GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
    if (w != nullptr) {
        // show or hide grid depending of myNetworkViewOptions.menuCheckToggleGrid
        if (w->getView()->getVisualisationSettings().drawJunctionShape) {
            w->getView()->editVisualisationSettings()->drawJunctionShape = false;
        } else {
            w->getView()->editVisualisationSettings()->drawJunctionShape = true;
        }
        w->getView()->update();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdToggleSecondaryShape(FXObject*, FXSelector, void*) {
    GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
    if (w != nullptr) {
        // toggle secondary shape visualization
        w->getView()->editVisualisationSettings()->secondaryShape = !w->getView()->getVisualisationSettings().secondaryShape;
        w->getView()->update();
    }
    return 1;
}


long
GUIApplicationWindow::onCmdFullScreen(FXObject*, FXSelector, void*) {
    if (myGLWindows.empty()) {
        return 1;
    }
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
        myToolBar8->hide();
        myMessageWindow->hide();
        if (myMDIClient->numChildren() > 0) {
            GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
            if (w != nullptr) {
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
        onCmdGaming(nullptr, 0, nullptr);
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
GUIApplicationWindow::onCmdFeedback(FXObject*, FXSelector, void*) {
    // create and open feedback dialog
    GUIDialog_Feedback* feedback = new GUIDialog_Feedback(this);
    feedback->create();
    feedback->show(PLACEMENT_OWNER);
    return 1;
}


long
GUIApplicationWindow::onCmdAbout(FXObject*, FXSelector, void*) {
    GUIDialog_AboutSUMO* about = new GUIDialog_AboutSUMO(this);
    about->create();
    about->show(PLACEMENT_OWNER);
    return 1;
}


long
GUIApplicationWindow::onCmdHallOfFame(FXObject*, FXSelector, void*) {
    GUIDialog_HallOfFame* hall = new GUIDialog_HallOfFame(this);
    hall->create();
    hall->show(PLACEMENT_OWNER);
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
    eventOccurred();
    return 1;
}


long
GUIApplicationWindow::onRunThreadEvent(FXObject*, FXSelector, void*) {
    eventOccurred();
    return 1;
}


void
GUIApplicationWindow::eventOccurred() {
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
            case GUIEventType::SIMULATION_LOADED:
                handleEvent_SimulationLoaded(e);
                setFocus();
                break;
            case GUIEventType::SIMULATION_STEP:
                if (myRunThread->networkAvailable()) { // avoid race-condition related crash if reload was pressed
                    handleEvent_SimulationStep(e);
                }
                break;
            case GUIEventType::MESSAGE_OCCURRED:
            case GUIEventType::WARNING_OCCURRED:
            case GUIEventType::ERROR_OCCURRED:
            case GUIEventType::DEBUG_OCCURRED:
            case GUIEventType::GLDEBUG_OCCURRED:
            case GUIEventType::STATUS_OCCURRED:
                handleEvent_Message(e);
                break;
            case GUIEventType::ADD_VIEW: {
                GUIEvent_AddView* ave = dynamic_cast<GUIEvent_AddView*>(e);
                auto v = openNewView(ave->in3D() ? GUISUMOViewParent::VIEW_3D_OSG : GUISUMOViewParent::VIEW_2D_OPENGL, ave->getCaption());
                if (ave->getSchemeName() != "") {
                    MFXComboBoxIcon* sCombo = v->getColoringSchemesCombo();
                    int index = sCombo->findItem(ave->getSchemeName().c_str());
                    if (index >= 0) {
                        sCombo->setCurrentItem(index);
                    }
                    v->setColorScheme(ave->getSchemeName());
                }
                break;
            }
            case GUIEventType::CLOSE_VIEW: {
                GUIEvent_CloseView* ave = dynamic_cast<GUIEvent_CloseView*>(e);
                removeViewByID(ave->getCaption());
                break;
            }
            case GUIEventType::SIMULATION_ENDED:
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
    // check whether the loading was successful
    if (ec->myNet == nullptr) {
        // report failure
        setStatusBarText(TLF("Loading of '%' failed!", ec->myFile));
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
            setStatusBarText(TLF("'%' loaded.", ec->myFile));
            setWindowSizeAndPos();
            myWasStarted = false;
            myHaveNotifiedAboutSimEnd = false;
            // initialise views
            myViewNumber = 0;
            const GUISUMOViewParent::ViewType defaultType = ec->myOsgView ? GUISUMOViewParent::VIEW_3D_OSG : GUISUMOViewParent::VIEW_2D_OPENGL;
            // check/record settings file modification time
            long long mTime = myGuiSettingsFileMTime;
            if (ec->mySettingsFiles.size() > 0) {
                for (std::string fname : ec->mySettingsFiles) {
                    mTime = MAX2(mTime, SysUtils::getModifiedTime(fname));
                }
            }
            // always reload if settings were modified or to restore multiple views
            if (!myIsReload) {
                gSchemeStorage.clearDecals();
            }
            if (ec->mySettingsFiles.size() > 0 && (!myIsReload || myGuiSettingsFileMTime < mTime || ec->mySettingsFiles.size() > 1)) {
                // open a view for each file and apply settings
                for (std::string fname : ec->mySettingsFiles) {
                    GUISettingsHandler settings(fname);
                    GUISUMOViewParent::ViewType vt = defaultType;
                    if (settings.getViewType() == "osg" || settings.getViewType() == "3d") {
                        vt = GUISUMOViewParent::VIEW_3D_OSG;
                    }
                    if (settings.getViewType() == "opengl" || settings.getViewType() == "2d") {
                        vt = GUISUMOViewParent::VIEW_2D_OPENGL;
                    }
                    GUISUMOAbstractView* view = openNewView(vt);
                    if (view == nullptr) {
                        break;
                    }
                    if (settings.getSettingName() != "") {
                        view->setColorScheme(settings.getSettingName());
                        MFXComboBoxIcon* sCombo = view->getColoringSchemesCombo();
                        int index = sCombo->findItem(settings.getSettingName().c_str());
                        if (index >= 0) {
                            sCombo->setCurrentItem(index);
                        }
                    }
                    view->addDecals(settings.getDecals());
                    settings.applyViewport(view);
                    settings.setSnapshots(view);
                    if (settings.getDelay() > 0.) {
                        mySimDelay = settings.getDelay();
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
            myGuiSettingsFileMTime = mTime;
            if (!OptionsCont::getOptions().isDefault("delay")) {
                setDelay(OptionsCont::getOptions().getFloat("delay"));
                mySimDelaySlider->setValue((int)mySimDelay);
                mySimDelaySpinner->setValue(mySimDelay);
            }
            if (!OptionsCont::getOptions().isDefault("breakpoints") && !myIsReload) {
                std::vector<SUMOTime> breakpoints;
                for (const std::string& val : OptionsCont::getOptions().getStringVector("breakpoints")) {
                    SUMOTime t = string2time(val);
                    // round down to nearest reachable time step
                    t -= t % DELTA_T;
                    breakpoints.push_back(t);
                }
                std::sort(breakpoints.begin(), breakpoints.end());
                myRunThread->getBreakpointLock().lock();
                myRunThread->getBreakpoints().assign(breakpoints.begin(), breakpoints.end());
                myRunThread->getBreakpointLock().unlock();
            }
            if (!OptionsCont::getOptions().isDefault("selection-file")) {
                delete myDynamicSelection;
                myDynamicSelection = new std::stringstream();
                std::string msg = gSelected.load(OptionsCont::getOptions().getString("selection-file"), GLO_MAX, myDynamicSelection);
                if (msg != "") {
                    WRITE_ERRORF("Errors while loading selection: %", msg.c_str());
                }
                if (!myDynamicSelection->str().empty()) {
                    std::string dummy;
                    int numNotFound = 0;
                    while (myDynamicSelection->good()) {
                        (*myDynamicSelection) >> dummy;
                        numNotFound++;
                    }
                    myDynamicSelection->clear(); // first clear error state before seek works
                    myDynamicSelection->seekg(0);
                    // @note for some reason the last line is read twice
                    WRITE_MESSAGEF("% dynamic objects not present while loading selection", numNotFound - 1);
                }
            }
            myTLSGame = OptionsCont::getOptions().getString("game.mode") == "tls";
            if (OptionsCont::getOptions().getBool("game")) {
                if (myTLSGame) {
                    setTitle(TL("SUMO Interactive Traffic Light"));
                } else {
                    setTitle(TL("SUMO Interactive Demand-Responsive-Transport"));
                }
                onCmdGaming(nullptr, 0, nullptr);
            } else {
                // set simulation name on the caption
                setTitle(MFXUtils::getTitleText("SUMO " VERSION_STRING, ec->myFile.c_str()));
            }
            if (ec->myViewportFromRegistry) {
                Position off;
                off.set(getApp()->reg().readRealEntry("viewport", "x"),
                        getApp()->reg().readRealEntry("viewport", "y"),
                        getApp()->reg().readRealEntry("viewport", "z"));
                Position p(off.x(), off.y(), 0);
                GUISUMOAbstractView* view = myGLWindows[0]->getView();
                view->setViewportFromToRot(off, p, 0);
            }
            // set simulation step begin information
            myLCDLabel->setText("----------------");
            for (std::vector<FXButton*>::const_iterator it = myStatButtons.begin(); it != myStatButtons.end(); ++it) {
                (*it)->setText("-");
            }
            // initialize scale from options unless already set in the UI
            if (myDemandScaleSpinner->getValue() == 1 || !OptionsCont::getOptions().isDefault("scale")) {
                myDemandScaleSpinner->setValue(OptionsCont::getOptions().getFloat("scale"));
            }
            myRunThread->getNet().getVehicleControl().setScale(myDemandScaleSpinner->getValue());
            MSRoutingEngine::initGUIThreadRNG();
        }
    }
    getApp()->endWaitCursor();
    // start if wished
    if (GUIGlobals::gRunAfterLoad && ec->myNet != nullptr && myRunThread->simulationIsStartable()) {
        onCmdStart(nullptr, 0, nullptr);
    }
    update();
}


void
GUIApplicationWindow::handleEvent_SimulationStep(GUIEvent*) {
#ifdef WIN32
    long t = SysUtils::getCurrentMillis();
    // only skip if the simulation is running
    if (t - myLastStepEventMillis < MIN_DRAW_DELAY && myRunThread->simulationIsStopable()) {
        // do not try to redraw with more than 50FPS (#6371)
        return;
    }
    myLastStepEventMillis = t;
#endif
    updateTimeLCD(myRunThread->getNet().getCurrentTimeStep());
    const int running = myRunThread->getNet().getVehicleControl().getRunningVehicleNo();
    const int backlog = myRunThread->getNet().getInsertionControl().getWaitingVehicleNo();
    if (backlog > running) {
        if (myStatButtons.front()->getIcon() == GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE)) {
            myStatButtons.front()->setIcon(GUIIconSubSys::getIcon(GUIIcon::YELLOWVEHICLE));
        }
    } else {
        if (myStatButtons.front()->getIcon() == GUIIconSubSys::getIcon(GUIIcon::YELLOWVEHICLE)) {
            myStatButtons.front()->setIcon(GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE));
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
        if (myTLSGame) {
            checkGamingEvents();
        } else {
            checkGamingEventsDRT();
        }
    }
    if (myRunThread->simulationIsStartable()) {
        getApp()->forceRefresh(); // restores keyboard focus
    }
    // try to load dynamic selection
    if (myDynamicSelection != nullptr) {
        std::stringstream tmp;
        gSelected.load(*myDynamicSelection, GLO_MAX, &tmp);
        if (tmp.str().empty()) {
            delete myDynamicSelection;
            myDynamicSelection = nullptr;
        } else {
            myDynamicSelection->str(tmp.str());
            myDynamicSelection->clear(); // first clear error state before seek works
            myDynamicSelection->seekg(0);
        }
    }
    updateChildren();
    update();
}


void
GUIApplicationWindow::handleEvent_Message(GUIEvent* e) {
    GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
    if (ec->getOwnType() == GUIEventType::STATUS_OCCURRED) {
        setStatusBarText(ec->getMsg());
    } else {
        myMessageWindow->appendMsg(ec->getOwnType(), ec->getMsg());
    }
}


void
GUIApplicationWindow::handleEvent_SimulationEnded(GUIEvent* e) {
    GUIEvent_SimulationEnded* ec = static_cast<GUIEvent_SimulationEnded*>(e);
    onCmdStop(nullptr, 0, nullptr);
    if (ec->getReason() == MSNet::SIMSTATE_LOADING) {
        onCmdReload(nullptr, 0, nullptr);
    } else if (GUIGlobals::gQuitOnEnd) {
        closeAllWindows();
        getApp()->exit(ec->getReason() == MSNet::SIMSTATE_ERROR_IN_SIM);
    } else if (GUIGlobals::gDemoAutoReload) {
        onCmdReload(nullptr, 1, nullptr);
    } else if (!myHaveNotifiedAboutSimEnd) {
        // GUIRunThread::deleteSim() triggers the final message to the log file
        // (this will never reach the GUI but we cannot use WRITE_MESSAGE here
        // to avoid a duplicate log entry)
        myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED,
                                   TLF("Simulation ended at time: %. (%)",
                                       time2string(ec->getTimeStep()), MSNet::getStateMessage(ec->getReason())) + "\n");
        // build the text
        const std::string text = TLF("Simulation ended at time: %.", time2string(ec->getTimeStep())) + "\n" +
                                 TL("Reason:") + MSNet::getStateMessage(ec->getReason()) + "\n" +
                                 TL("Do you want to close all open files and views?");
        FXuint answer = FXMessageBox::question(this, MBOX_YES_NO, TL("Simulation ended"), "%s", text.c_str());
        if (answer == 1) { //1:yes, 2:no, 4:esc
            closeAllWindows();
        } else {
            GUINet::getGUIInstance()->flushOutputsAtEnd();
            updateChildren();
            update();
        }
        myHaveNotifiedAboutSimEnd = true;
    }
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
        if (veh->isOnRoad() && !veh->isStopped()) {
            const double vmax = veh->getLane()->getVehicleMaxSpeed(veh);
            if (veh->getSpeed() < SUMO_const_haltingSpeed) {
                myWaitingTime += DELTA_T;
                if (veh->getVClass() == SVC_EMERGENCY) {
                    myEmergencyVehicleCount += DELTA_T;
                }
            }
            myTimeLoss += TIME2STEPS(TS * (vmax - veh->getSpeed()) / vmax); // may be negative with speedFactor > 1
        }

    }
    myWaitingTimeLabel->setText(time2string(myWaitingTime, myShowTimeAsHMS).c_str());
    myTimeLossLabel->setText(time2string(myTimeLoss, myShowTimeAsHMS).c_str());
    myEmergencyVehicleLabel->setText(time2string(myEmergencyVehicleCount, myShowTimeAsHMS).c_str());
}


void
GUIApplicationWindow::checkGamingEventsDRT() {
    // update performance indicators
    MSTransportableControl& pc = myRunThread->getNet().getPersonControl();
    myWaitingTime += pc.getWaitingForVehicleNumber() * DELTA_T;
    myWaitingTimeLabel->setText(time2string(myWaitingTime).c_str());

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();
    for (auto it = vc.loadedVehBegin(); it != end; ++it) {
        const MSVehicle* veh = dynamic_cast<MSVehicle*>(it->second);
        assert(veh != 0);
        if (veh->isOnRoad() && !veh->isStopped()) {
            myTotalDistance += SPEED2DIST(veh->getSpeed());
        }
    }
    myTotalDistanceLabel->setText(toString(myTotalDistance / 100).c_str());
}


void
GUIApplicationWindow::loadConfigOrNet(const std::string& file) {
    if (!myAmLoading) {
        storeWindowSizeAndPos();
        getApp()->beginWaitCursor();
        myAmLoading = true;
        myIsReload = false;
        closeAllWindows();
        gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
        myLoadThread->loadConfigOrNet(file);
        setStatusBarText(TLF("Loading '%'.", file));
        update();
    }
}


GUISUMOAbstractView*
GUIApplicationWindow::openNewView(GUISUMOViewParent::ViewType vt, std::string caption) {
    if (!myRunThread->networkAvailable()) {
        myStatusbar->getStatusLine()->setText(TL("No simulation loaded!"));
        return nullptr;
    }
    GUISUMOAbstractView* oldView = nullptr;
    if (myMDIClient->numChildren() > 0) {
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            oldView = w->getView();
        }
    }
    if (caption == "") {
        caption = "View #" + toString(myViewNumber++);
    }
    FXuint opts = MDI_TRACKING;
    GUISUMOViewParent* w = new GUISUMOViewParent(myMDIClient, myMDIMenu, FXString(caption.c_str()),
            this, GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), opts, 10, 10, 200, 100);
    GUISUMOAbstractView* v = w->init(getBuildGLCanvas(), myRunThread->getNet(), vt);
    if (oldView != nullptr) {
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
        return nullptr;
    }
    GUISUMOViewParent* share_tmp1 =
        static_cast<GUISUMOViewParent*>(myMDIClient->childAtIndex(0));
    return share_tmp1->getBuildGLCanvas();
}


void
GUIApplicationWindow::closeAllWindows() {
    myTrackerLock.lock();
    myLCDLabel->setText("----------------");
    for (std::vector<FXButton*>::const_iterator it = myStatButtons.begin(); it != myStatButtons.end(); ++it) {
        (*it)->setText("-");
        if (it != myStatButtons.begin()) {
            (*it)->hide();
        }
    }
    // delete the simulation
    myRunThread->deleteSim();
    // reset the caption
    setTitle(MFXUtils::getTitleText("SUMO " VERSION_STRING));
    // remove trackers and other external windows (must be delayed until deleteSim)
    while (!myGLWindows.empty()) {
        delete myGLWindows.front();
    }
    // make a copy because deleting modifyes the vector;
    std::vector<FXMainWindow*> trackerWindows = myTrackerWindows;
    for (FXMainWindow* const window : trackerWindows) {
        delete window;
    }
    myTrackerWindows.clear();
    // clear selected items
    gSelected.clear();
    // add a separator to the log
    myMessageWindow->addSeparator();
    myTrackerLock.unlock();
    // remove coordinate information
    myGeoCoordinate->setText(TL("N/A"));
    myCartesianCoordinate->setText(TL("N/A"));
    if (myTestCoordinate) {
        myTestCoordinate->setText(TL("N/A"));
    }
    //
    GUITexturesHelper::clearTextures();
    GLHelper::resetFont();
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


double
GUIApplicationWindow::getTrackerInterval() const {
    return GUIGlobals::gTrackerInterval;
}


void
GUIApplicationWindow::loadOnStartup(const bool wait) {
    loadConfigOrNet("");
    if (wait) {
        while (myAmLoading) {
            myRunThread->sleep(50);
        }
    }
}


void
GUIApplicationWindow::setStatusBarText(const std::string& text) {
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
}


void
GUIApplicationWindow::addRecentNetwork(const FX::FXString& f) {
    myRecentNetworks.appendFile(f);
}


void
GUIApplicationWindow::addRecentConfig(const FX::FXString& f) {
    myRecentConfigs.appendFile(f);
}


void
GUIApplicationWindow::updateTimeLCDTooltip() {
    if (myShowTimeAsHMS) {
        myLCDLabel->setToolTipText("HH:MM:SS");
        if (myAmGaming) {
            myWaitingTimeLabel->setToolTipText("HH:MM:SS");
            myTimeLossLabel->setToolTipText("HH:MM:SS");
            myEmergencyVehicleLabel->setToolTipText("HH:MM:SS");
        }
    } else {
        myLCDLabel->setToolTipText(TL("seconds"));
        if (myAmGaming) {
            myWaitingTimeLabel->setToolTipText(TL("seconds"));
            myTimeLossLabel->setToolTipText(TL("seconds"));
            myEmergencyVehicleLabel->setToolTipText(TL("seconds"));
        }
    }
}


void
GUIApplicationWindow::updateTimeLCD(SUMOTime time) {
    time -= DELTA_T; // synchronize displayed time with netstate output
    if (time < 0) {
        myLCDLabel->setText("----------------");
        return;
    }
    if (myAmGaming) {
        // show time counting backwards
        time = myRunThread->getSimEndTime() - time;
    }
    std::ostringstream str;
    str << std::setfill('0');
    const bool hideFraction = myAmGaming || DELTA_T % 1000 == 0;
    if (myShowTimeAsHMS) {
        SUMOTime day = time / 86400000;
        if (day > 0) {
            str << day << '-';
            time %= 86400000;
        }
        str << std::setw(2);
        str << time / 3600000 << '-';
        time %= 3600000;
        str << std::setw(2) << time / 60000 << '-';
        time %= 60000;
    }
    str << std::setw(2) << time / 1000;
    if (!hideFraction) {
        str << '.' << std::setw(3) << time % 1000;
    }
    myLCDLabel->setText(str.str().c_str());
}


void
GUIApplicationWindow::addHotkey(int key, Command* press, Command* release) {
    if (press != nullptr) {
        myHotkeyPress[key] = press;
    }
    if (release != nullptr) {
        myHotkeyRelease[key] = release;
    }
}


long
GUIApplicationWindow::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    FXEvent* e = (FXEvent*) ptr;
    // PgUp and PgDown switch between widgets by default and binding them via menu shortcuts does not work reliably
    // so we must intercept them before FXMainWindow can handle it
    if (e->code == FX::KEY_Page_Up) {
        onCmdDelayInc(nullptr, 0, nullptr);
    } else if (e->code == FX::KEY_Page_Down) {
        onCmdDelayDec(nullptr, 0, nullptr);
    } else {
        // disable hotkeys without modifiers for the game
        const bool ignoreSimple = myAmGaming && (e->state & (CONTROLMASK | SHIFTMASK | ALTMASK)) == 0;
        const long handled = ignoreSimple ? 0 : FXMainWindow::onKeyPress(o, sel, ptr);
        if (handled == 0 && myMDIClient->numChildren() > 0) {
            auto it = myHotkeyPress.find(e->code);
            if (it != myHotkeyPress.end()) {
                it->second->execute(SIMSTEP);
            }
            if (!ignoreSimple) {
                GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
                if (w != nullptr) {
                    w->onKeyPress(nullptr, sel, ptr);
                }
            }
        }
    }
    return 0;
}


long
GUIApplicationWindow::onKeyRelease(FXObject* o, FXSelector sel, void* ptr) {
    const long handled = FXMainWindow::onKeyRelease(o, sel, ptr);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        FXEvent* e = (FXEvent*) ptr;
        auto it = myHotkeyRelease.find(e->code);
        if (it != myHotkeyRelease.end()) {
            it->second->execute(SIMSTEP);
        }
        GUISUMOViewParent* w = dynamic_cast<GUISUMOViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onKeyRelease(nullptr, sel, ptr);
        }
    }
    return 0;
}


double
GUIApplicationWindow::getDelay() const {
    return mySimDelay;
}


void
GUIApplicationWindow::setDelay(double delay) {
    mySimDelay = delay;
}


void
GUIApplicationWindow::sendBlockingEvent(GUIEvent* event) {
    myEventMutex.lock();
    myEvents.push_back(event);
    myRunThreadEvent.signal();
    //myEventCondition.wait(myEventMutex);
    myEventMutex.unlock();
}


void
GUIApplicationWindow::setBreakpoints(const std::vector<SUMOTime>& breakpoints) {
    if (myRunThread != nullptr) {
        myRunThread->getBreakpointLock().lock();
        myRunThread->getBreakpoints().assign(breakpoints.begin(), breakpoints.end());
        myRunThread->getBreakpointLock().unlock();
        updateChildren(MID_TIMELINK_BREAKPOINT);
    }
}


void
GUIApplicationWindow::addBreakpoint(SUMOTime time) {
    const SUMOTime begin = string2time(OptionsCont::getOptions().getString("begin"));
    if (time >= begin) {
        // ensure breakpoint is valid
        time -= (time - begin) % DELTA_T;
        std::vector<SUMOTime> breakpoints = retrieveBreakpoints();
        if (std::find(breakpoints.begin(), breakpoints.end(), time) == breakpoints.end()) {
            breakpoints.push_back(time);
            std::sort(breakpoints.begin(), breakpoints.end());
            setBreakpoints(breakpoints);
            setStatusBarText(TLF("Set breakpoint at %", time2string(time)));
        }
    }
}


const std::vector<SUMOTime>
GUIApplicationWindow::retrieveBreakpoints() const {
    myRunThread->getBreakpointLock().lock();
    std::vector<SUMOTime> result = myRunThread->getBreakpoints();
    myRunThread->getBreakpointLock().unlock();
    return result;
}


void
GUIApplicationWindow::eraseBreakpointDialog() {
    myBreakpointDialog = nullptr;
}

/****************************************************************************/
