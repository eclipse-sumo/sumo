/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIApplicationWindow.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// The main window of the SUMO-gui.
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXRecentNetworks.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>
#include <utils/gui/windows/GUIMainWindow.h>

#include "GUISUMOViewParent.h"


// ===========================================================================
// class declarations
// ===========================================================================

class Command;
class GUILoadThread;
class GUIRunThread;
class GUIMessageWindow;
class GUIEvent;
class GUIParameterTracker;
class GUIParameterTableWindow;
class GUIDialog_Breakpoints;
class MFXLCDLabel;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIApplicationWindow
 * @brief The main window of the SUMO-gui.
 *
 * Beside views on the simulation, shown within a MDI-window, the main window
 * may also have some further views (children) assigned which are stored
 * within a separate list.
 */
class GUIApplicationWindow : public GUIMainWindow, public MFXInterThreadEventClient {
    // FOX-declarations
    FXDECLARE(GUIApplicationWindow)

public:
    /** @brief Constructor
     * @param[in] a The FOX application
     * @param[in] configPattern The pattern used for loading configurations
     */
    GUIApplicationWindow(FXApp* a, const std::string& configPattern);

    /// @brief Destructor
    virtual ~GUIApplicationWindow();

    /// @name FOX-interactions
    /// {

    /// @brief Creates the main window (required by FOX)
    virtual void create();

    /// @brief Detaches the tool/menu bar
    virtual void detach();

    /// @}

    /// @brief config or net on startup
    void loadOnStartup(const bool wait = false);

    /// @brief get run thread
    GUIRunThread* getRunner();

    /// @brief build dependt
    void dependentBuild(const bool isLibsumo);

    /// @brief set status bar text
    void setStatusBarText(const std::string& text);

    /// @brief add recent network to recent file list
    void addRecentNetwork(const FX::FXString& f);

    /// @brief add recent config to recent file list
    void addRecentConfig(const FX::FXString& f);

    /// @brief get build GLCanvas
    FXGLCanvas* getBuildGLCanvas() const;

    /// @brief get current simulation time
    SUMOTime getCurrentSimTime() const;

    /// @brief get tracker interval
    double getTrackerInterval() const;

    /// @brief get default cursor
    FXCursor* getDefaultCursor();

    /// @name Inter-thread event handling
    /// @{

    /// @brief a certaint event ocurred
    virtual void eventOccurred();

    /// @brief called when event "simulation loaded" ocurred
    void handleEvent_SimulationLoaded(GUIEvent* e);

    /// @brief called when event "simulation step" ocurred
    void handleEvent_SimulationStep(GUIEvent* e);

    /// @brief called when event "message" ocurred
    void handleEvent_Message(GUIEvent* e);

    /// @brief called when event "simulation ended" ocurred
    void handleEvent_SimulationEnded(GUIEvent* e);

    /// @}

    /// @name FOX-callbacks
    /// @{

    /// @brief Called on menu File->New Window
    long onCmdNewWindow(FXObject*, FXSelector, void*);

    /// @brief Called on menu File->Open Configuration
    long onCmdOpenConfiguration(FXObject*, FXSelector, void*);

    /// @brief Called on menu File->Open Network
    long onCmdOpenNetwork(FXObject*, FXSelector, void*);

    /// @brief Called on menu File->Load Shapes
    long onCmdOpenShapes(FXObject*, FXSelector, void*);

    /// @brief Called on menu File->Load EdgeData
    long onCmdOpenEdgeData(FXObject*, FXSelector, void*);

    /// @brief Called on reload
    long onCmdReload(FXObject*, FXSelector, void*);

    /// @brief Called on quick-reload
    long onCmdQuickReload(FXObject*, FXSelector, void*);

    /// @brief Called on opening a recent file
    long onCmdOpenRecent(FXObject*, FXSelector, void*);

    /// @brief Called on menu File->Close
    long onCmdSaveConfig(FXObject*, FXSelector, void*);

    /// @brief Called on menu File->Close
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief Called by FOX if the application shall be closed (Called either by FileMenu->Quit, the normal close-menu or SIGINT)
    long onCmdQuit(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Edit Chosen
    long onCmdEditChosen(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Edit Breakpoints
    long onCmdEditBreakpoints(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Visualization
    long onCmdEditViewScheme(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Viewport
    long onCmdEditViewport(FXObject*, FXSelector, void*);

    /// @brief called if the user selects help->Documentation
    long onCmdHelp(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user selects help->Changelog
    long onCmdChangelog(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user selects help->Hotkeys
    long onCmdHotkeys(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user selects help->Tutorial
    long onCmdTutorial(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief Called on menu Edit->Netedit (network)
    long onCmdNeteditNetwork(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Netedit (SUMOCfg)
    long onCmdNeteditSUMOConfig(FXObject*, FXSelector, void*);

    /// @brief Enable or disable open SUMOConfig in netedit
    long onUpdNeteditSUMOConfig(FXObject*, FXSelector, void*);

    /// @brief Opens the application settings menu (Settings->Application Settings...)
    long onCmdAppSettings(FXObject*, FXSelector, void*);

    /// @brief Toggle gaming mode
    long onCmdGaming(FXObject*, FXSelector, void*);

    /// @brief Toggle draw junction shape
    long onCmdToggleDrawJunctionShape(FXObject*, FXSelector, void*);

    /// @brief Toggle draw junction shape
    long onCmdToggleSecondaryShape(FXObject*, FXSelector, void*);

    /// @brief Toggle full screen mode
    long onCmdFullScreen(FXObject*, FXSelector, void*);

    /// @brief Toggle listing of internal structures
    long onCmdListInternal(FXObject*, FXSelector, void*);

    /// @brief Toggle listing of parking vehicles
    long onCmdListParking(FXObject*, FXSelector, void*);

    /// @brief Toggle listing of teleporting vehicles
    long onCmdListTeleporting(FXObject*, FXSelector, void*);

    /// @brief Shows the feedback dialog
    long onCmdFeedback(FXObject*, FXSelector, void*);

    /// @brief Shows the about dialog
    long onCmdAbout(FXObject*, FXSelector, void*);

    /// @brief Shows the Hall of Fame dialog
    long onCmdHallOfFame(FXObject*, FXSelector, void*);

    /// @brief Called on "play"
    long onCmdStart(FXObject*, FXSelector, void*);

    /// @brief Called on "stop"
    long onCmdStop(FXObject*, FXSelector, void*);

    /// @brief Called on "step"
    long onCmdStep(FXObject*, FXSelector, void*);

    /// @brief Called on "save state"
    long onCmdSaveState(FXObject*, FXSelector, void*);

    /// @brief Called on "save state"
    long onCmdLoadState(FXObject*, FXSelector, void*);

    /// @brief Called on "time toggle"
    long onCmdTimeToggle(FXObject*, FXSelector, void*);

    /// @brief Called on "delay inc"
    long onCmdDelayInc(FXObject*, FXSelector, void*);

    /// @brief Called on "delay dec"
    long onCmdDelayDec(FXObject*, FXSelector, void*);

    /// @brief Called on "delay toggle"
    long onCmdDelayToggle(FXObject*, FXSelector, void*);

    /// @brief Called on "demand scale"
    long onCmdDemandScale(FXObject*, FXSelector, void*);

    /// @brief Called if a new view shall be opened (2D view)
    long onCmdNewView(FXObject*, FXSelector, void*);

#ifdef HAVE_OSG
    /// @brief Called if a new 3D view shall be opened
    long onCmdNewOSG(FXObject*, FXSelector, void*);
#endif

    /// @brief Determines whether opening is enabled
    long onUpdOpen(FXObject*, FXSelector, void*);

    /// @brief Determines whether reloading is enabled
    long onUpdReload(FXObject*, FXSelector, void*);

    /// @brief Determines whether opening a recent file is enabled
    long onUpdOpenRecent(FXObject*, FXSelector, void*);

    /// @brief Determines whether adding a view is enabled
    long onUpdAddView(FXObject*, FXSelector, void*);

    /// @brief Determines whether "play" is enabled
    long onUpdStart(FXObject* sender, FXSelector, void* ptr);

    /// @brief Determines whether "stop" is enabled
    long onUpdStop(FXObject*, FXSelector, void*);

    /// @brief Determines whether "step" is enabled
    long onUpdStep(FXObject*, FXSelector, void*);

    /// @brief Determines whether some buttons which require an active simulation may be shown
    long onUpdNeedsSimulation(FXObject*, FXSelector, void*);

    /// @brief Determines whether traci is active
    long onUpdTraCIStatus(FXObject*, FXSelector, void*);

    /// @brief Called if the message window shall be cleared
    long onCmdClearMsgWindow(FXObject*, FXSelector, void*);

    /// @brief Called on menu commands from the Locator menu
    long onCmdLocate(FXObject*, FXSelector, void*);

    /// @brief Called on commands from the statistic buttons
    long onCmdShowStats(FXObject*, FXSelector, void*);

    /// @brief Called on an event from the loading thread
    long onLoadThreadEvent(FXObject*, FXSelector, void*);

    /// @brief Called on an event from the simulation thread
    long onRunThreadEvent(FXObject*, FXSelector, void*);

    /// @brief Somebody wants our clipped text
    long onClipboardRequest(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called when a key is pressed
    long onKeyPress(FXObject* o, FXSelector sel, void* data);

    /// @brief called when a key is released
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);

    /// @}

    /// @brief Returns the simulation delay in miliseconds
    virtual double getDelay() const;

    /// @brief Sets the delay of the parent application in milliseconds
    virtual void setDelay(double delay);

    /// @brief Sets the breakpoints of the parent application
    virtual void setBreakpoints(const std::vector<SUMOTime>& breakpoints);

    /// @brief Sends an event from the application thread to the GUI and waits until it is handled
    virtual void sendBlockingEvent(GUIEvent* event);

    /// @brief retrieve list of breakpoints
    const std::vector<SUMOTime> retrieveBreakpoints() const;

    /// @brief erase current breakpoint dialog
    void eraseBreakpointDialog();

    /// @brief register custom hotkey action
    void addHotkey(int key, Command* press, Command* release);

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GUIApplicationWindow)

    /// @brief add the given menuPane to windows Menu
    virtual void addToWindowsMenu(FXMenuPane* menuPane);

    /// Builds the menu bar
    virtual void fillMenuBar();

    /// Builds the tool bar
    virtual void buildToolBars();

    /// @brief build recent networks
    void buildRecentNetworks(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentNetworks);

    /// @brief build recent configs
    void buildRecentConfigs(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentConfigs);

    /// @brief  the name of the simulation
    std::string myName;

    /// @brief  the thread that loads simulations
    GUILoadThread* myLoadThread = nullptr;

    /// @brief  the thread that runs simulations
    GUIRunThread* myRunThread = nullptr;

    /// @brief  the information whether the simulation was started before
    bool myWasStarted = false;

    /// @brief The current view number
    int myViewNumber;

    /// @brief information whether the gui is currently loading and the load-options shall be greyed out
    bool myAmLoading = false;

    /// @brief whether we are reloading the simulation
    bool myIsReload = false;

    /// @brief last modification time of the gui setting file
    long long  myGuiSettingsFileMTime = -2;

    /// @brief the submenus
    FXMenuPane *myFileMenu = nullptr,
               *myEditMenu = nullptr,
               *mySelectByPermissions = nullptr,
               *mySettingsMenu = nullptr,
               *myLocatorMenu = nullptr,
               *myControlMenu = nullptr,
               *myWindowMenu = nullptr,
               *myHelpMenu = nullptr;

    /// @brief FXMenu pane for recent networks
    FXMenuPane* myFileMenuRecentNetworks = nullptr;

    /// @brief FXMenu pane for recent configs
    FXMenuPane* myFileMenuRecentConfigs = nullptr;

    /// @brief the menu cascades
    FXMenuCascade* mySelectLanesMenuCascade = nullptr;

    /// @brief Buttons showing and running values and triggering statistic windows
    std::vector<FXButton*> myStatButtons;

    /// @brief A window to display messages, warnings and error in
    GUIMessageWindow* myMessageWindow = nullptr;

    /// @brief The splitter that divides the main window into views and the log window
    FXSplitter* myMainSplitter = nullptr;

    /// @brief for some menu detaching fun
    FXToolBarShell* myToolBarDrag1 = nullptr,
                    *myToolBarDrag2 = nullptr,
                     *myToolBarDrag3 = nullptr,
                      *myToolBarDrag4 = nullptr,
                       *myToolBarDrag5 = nullptr,
                        *myMenuBarDrag = nullptr,
                         *myToolBarDrag8 = nullptr;

    /// @brief the simulation delay in milliseconds
    double mySimDelay = 0.;

    /// @brief Simulation delay target
    FXDataTarget* mySimDelayTarget = nullptr;

    /// @brief Simulation delay spinner
    FXRealSpinner* mySimDelaySpinner = nullptr;

    /// @brief Simulation delay slider
    FXSlider* mySimDelaySlider = nullptr;

    /// @brief the demand scale
    FXRealSpinner* myDemandScaleSpinner = nullptr;

    /// @brief The alternate simulation delay in milliseconds for toggling
    double myAlternateSimDelay = 0;

    /// @brief List of got requests
    MFXSynchQue<GUIEvent*> myEvents;

    /// @brief The menu used for the MDI-windows
    FXMDIMenu* myMDIMenu = nullptr;

    /// @brief The application menu bar
    FXMenuBar* myMenuBar = nullptr;

    /// @brief The application tool bar
    FXToolBar* myToolBar1 = nullptr,
               *myToolBar2 = nullptr,
                *myToolBar3 = nullptr,
                 *myToolBar4 = nullptr,
                  *myToolBar5 = nullptr,
                   *myToolBar8 = nullptr;

    /// @brief the simulation step display
    MFXLCDLabel* myLCDLabel = nullptr;

    /// @brief io-event with the load-thread
    FXEX::MFXThreadEvent myLoadThreadEvent;

    /// @brief io-event with the run-thread
    FXEX::MFXThreadEvent myRunThreadEvent;

    /// @brief List of recent networks
    MFXRecentNetworks myRecentNetworks;

    /// @brief List of recent configs
    MFXRecentNetworks myRecentConfigs;

    /// @brief Input file pattern
    std::string myConfigPattern;

    /// @brief flag to mark if GUIApplicationWIndow has depend build
    bool hadDependentBuild = false;

    /// @brief whether to show time as hour:minute:second
    bool myShowTimeAsHMS = false;

    /// @brief whether the simulation end was already announced
    bool myHaveNotifiedAboutSimEnd = false;

    /// @brief the mutex for the waiting semaphore
    FXMutex myEventMutex;

    /// @brief the semaphore when waiting for event completion
    FXCondition myEventCondition;

    /// @brief menu checkbox to activate game mode
    FXMenuCheck* myGamingModeCheckbox;

    /// @name game related things
    /// {

    /// @brief random list of jam sounds
    RandomDistributor<std::string> myJamSounds;

    /// @brief random list of collision sounds
    RandomDistributor<std::string> myCollisionSounds;

    /// @brief waiting time after which vehicles trigger jam sounds
    double myJamSoundTime = 60;

    /// @brief A random number generator used to choose a gaming sound
    static std::mt19937 myGamingRNG;

    /// @brief previous collision number
    int myPreviousCollisionNumber = 0;

    /// @brief flag for enable TLS gameMode
    bool myTLSGame = false;

    /// @brief waiting time label
    MFXLCDLabel* myWaitingTimeLabel = nullptr;

    /// @brief waiting time
    SUMOTime myWaitingTime = 0;

    /// @brief time loss label
    MFXLCDLabel* myTimeLossLabel = nullptr;

    /// @brief time loss
    SUMOTime myTimeLoss = 0;

    /// @brief total distance label
    MFXLCDLabel* myTotalDistanceLabel = nullptr;

    /// @brief total distance
    double myTotalDistance = 0;

    /// @brief emergency vehicle label
    MFXLCDLabel* myEmergencyVehicleLabel = nullptr;

    /// @brief emergency vehicle count
    SUMOTime myEmergencyVehicleCount = 0;

    /// @brief toolbars used in game
    FXToolBar* myToolBar6 = nullptr,
               *myToolBar7 = nullptr,
                *myToolBar9 = nullptr,
                 *myToolBar10 = nullptr;

    /// @brief toolbars shell used in game
    FXToolBarShell* myToolBarDrag6 = nullptr,
                    *myToolBarDrag7 = nullptr,
                     *myToolBarDrag9 = nullptr,
                      *myToolBarDrag10 = nullptr;
    ////}

    /// @brief last time the simulation view was redrawn due to a simStep
    long myLastStepEventMillis;

    /// @brief custom hotkeys pressed
    std::map<int, Command*> myHotkeyPress;

    /// @brief custom hotkeys released
    std::map<int, Command*> myHotkeyRelease;

    /// @brief breakpoint dialog
    GUIDialog_Breakpoints* myBreakpointDialog = nullptr;

private:
    /// @brief starts to load a simulation
    void loadConfigOrNet(const std::string& file);

    /// @brief this method closes all windows and deletes the current simulation
    void closeAllWindows();

    /// @brief updates the simulation time display
    void updateTimeLCD(SUMOTime time);

    /// @brief update LCD timer tooltip
    void updateTimeLCDTooltip();

    /// @brief opens a new simulation display
    GUISUMOAbstractView* openNewView(GUISUMOViewParent::ViewType vt = GUISUMOViewParent::VIEW_2D_OPENGL, std::string caption = "");

    /// @brief handles additional game-related events
    void checkGamingEvents();

    /// @brief handles additional game-related events (DRT)
    void checkGamingEventsDRT();

    /// @brief invalidate copy constructor
    GUIApplicationWindow(const GUIApplicationWindow& s) = delete;

    /// @brief invalidate assignment operator
    GUIApplicationWindow& operator=(const GUIApplicationWindow& s) = delete;
};
