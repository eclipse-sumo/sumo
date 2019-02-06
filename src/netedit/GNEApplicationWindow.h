/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEApplicationWindow.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The main window of Netedit (adapted from GUIApplicationWindow)
/****************************************************************************/
#ifndef GNEApplicationWindow_h
#define GNEApplicationWindow_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/foxtools/FXSynchQue.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>
#include <utils/geom/Position.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/shapes/ShapeHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNELoadThread;
class GNEUndoList;
class GNENet;
class GNEViewNet;
class GUIEvent;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GNEApplicationWindow
 * @brief The main window of the Netedit.
 *
 * Contains the file opening support and a canvas to display the network in.
 *
 * Beside views on the simulation, shown within a MDI-window, the main window
 * may also have some further views (children) assigned which are stored
 * within a separate list.
 */
class GNEApplicationWindow : public GUIMainWindow, public MFXInterThreadEventClient {
    /// @brief FOX-declaration
    FXDECLARE(GNEApplicationWindow)

public:
    /// @brief struct for griped toolbars
    struct ToolbarsGrip {

        /// @brief constructor
        ToolbarsGrip(GNEApplicationWindow* GNEApp);

        /// @brief build menu toolbar grips
        void buildMenuToolbarsGrip();

        /// @brief build toolbars grips
        void buildViewParentToolbarsGrips();

        /// @brief build toolbars grips
        void destroyParentToolbarsGrips();

        /// @brief The application menu bar (for file, edit, processing...)
        FXMenuBar* menu;

        /// @brief The application menu bar for supermodes (network and demand)
        FXMenuBar* superModes;

        /// @brief The application menu bar for navigation (zoom, coloring...)
        FXMenuBar* navigation;

        /// @brief The application menu bar (for select, inspect...)
        FXMenuBar* modes;

        /// @brief The application menu bar for mode options (show connections, select edges...)
        FXMenuBar* modeOptions;

    private:
        /// @brief pointer to current GNEApplicationWindow
        GNEApplicationWindow* myGNEApp;

        /// @brief menu bar drag (for file, edit, processing...)
        FXToolBarShell* myToolBarShellMenu;

        /// @brief menu bar drag for modes (network and demand)
        FXToolBarShell* myToolBarShellSuperModes;

        /// @brief menu bar drag for navigation (Zoom, coloring...)
        FXToolBarShell* myToolBarShellNavigation;

        /// @brief menu bar drag for modes (select, inspect, delete...)
        FXToolBarShell* myToolBarShellModes;

        /// @brief menu bar drag for mode options(show connections, select edges...)
        FXToolBarShell* myToolBarShellModeOptions;
    };

    /**@brief Constructor
     * @param[in] a The FOX application
     * @param[in] configPattern The pattern used for loading configurations
     */
    GNEApplicationWindow(FXApp* a, const std::string& configPattern);

    /// @brief Destructor
    ~GNEApplicationWindow();

    /// @brief Creates the main window (required by FOX)
    void create();

    /// @brief load net on startup
    void loadOptionOnStartup();

    /// @brief build dependent
    void dependentBuild();

    /// @brief set text of the statusBar
    void setStatusBarText(const std::string& statusBarText);

    /// @brief enable save additionals
    void enableSaveAdditionalsMenu();

    /// @brief disable save additionals
    void disableSaveAdditionalsMenu();

    /// @brief enable save TLS Programs
    void enableSaveTLSProgramsMenu();

    /// @brief enable save demand elements
    void enableSaveDemandElementsMenu();

    /// @brief disable save demand elements
    void disableSaveDemandElementsMenu();

    /// @name Inter-thread event handling
    /// @{
    /// @brief called when an event occurred
    void eventOccurred();

    /// @brief handle event of type Network loaded
    void handleEvent_NetworkLoaded(GUIEvent* e);

    /// @brief hanlde event of type message
    void handleEvent_Message(GUIEvent* e);
    /// @}

    /// @name FOX-callbacks
    /// @{
    /// @brief called when the command/FXCall new network is executed
    long onCmdNewNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open configuration is executed
    long onCmdOpenConfiguration(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open network is executed
    long onCmdOpenNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open foreign is executed
    long onCmdOpenForeign(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open additionals is executed
    long onCmdOpenAdditionals(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open additionals is executed
    long onCmdOpenTLSPrograms(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open demand is executed
    long onCmdOpenDemandElements(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall reload is executed
    long onCmdReload(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open recent is executed
    long onCmdOpenRecent(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall close is executed
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Visualization
    long onCmdEditViewScheme(FXObject*, FXSelector, void*);

    /// @brief Called on menu Edit->Viewport
    long onCmdEditViewport(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall locate is executed
    long onCmdLocate(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save network is executed
    long onCmdSaveNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save additionals is executed
    long onCmdSaveAdditionals(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save additionals as is executed
    long onCmdSaveAdditionalsAs(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save TLSPrograms is executed
    long onCmdSaveTLSPrograms(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save TLSPrograms as is executed
    long onCmdSaveTLSProgramsAs(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save demand elements is executed
    long onCmdSaveDemandElements(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save demand elements as is executed
    long onCmdSaveDemandElementsAs(FXObject*, FXSelector, void*);

    /// @brief called when the update/FXCall save network is executed
    long onUpdSaveNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save network as is executed
    long onCmdSaveAsNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the upadte/FXCall needs network is executed
    long onUpdNeedsNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the update/FXCall reload is executed
    long onUpdReload(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save as plain xml is executed
    long onCmdSaveAsPlainXML(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save joined is executed
    long onCmdSaveJoined(FXObject*, FXSelector, void*);

    /// @brief called when a key is pressed
    long onKeyPress(FXObject* o, FXSelector sel, void* data);

    /// @brief called when a key is released
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);

    /**@brief Called by FOX if the application shall be closed
     * @note Called either by FileMenu->Quit, the normal close-menu or SIGINT
     */
    long onCmdQuit(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall edit chosen is executed
    long onCmdEditChosen(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall show about dialog is executed
    long onCmdAbout(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall on update open executed
    long onUpdOpen(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall clear message windows is executed
    long onCmdClearMsgWindow(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall load thread is executed
    long onLoadThreadEvent(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall clipboard request is executed
    long onClipboardRequest(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits an edit-supermode hotkey
    long onCmdSetSuperMode(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits an edit-mode hotkey
    long onCmdSetMode(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hints ctrl + T
    long onCmdOpenSUMOGUI(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits esc
    long onCmdAbort(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits del
    long onCmdDel(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits enter
    long onCmdEnter(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits f
    long onCmdFocusFrame(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user press key combination Ctrl + G to toogle grid
    long onCmdToogleGrid(FXObject*, FXSelector, void*);

    /// @brief called if the user press key combination Ctrl + Shift + C to toogle show connections
    long onCmdToogleShowConnections(FXObject*, FXSelector, void*);

    /// @brief called if the user press key combination Ctrl + Shift + H to toogle elevation
    long onCmdToogleElevation(FXObject*, FXSelector, void*);

    /// @brief called if the user press key combination Ctrl + Shift + I to toogle select edges
    long onCmdToogleSelectEdges(FXObject*, FXSelector, void*);

    /// @brief called if the user press key combination Ctrl + Shift + J to toogle chain mode
    long onCmdToogleChain(FXObject*, FXSelector, void*);

    /// @brief called if the user press key combination Ctrl + Shift + K to toogle two way
    long onCmdToogleTwoWay(FXObject*, FXSelector, void*);

    /// @brief called if the user selects help->Documentation
    long onCmdHelp(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user selects Processing->compute junctions
    long onCmdComputeJunctions(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->compute junctions with volatile options
    long onCmdComputeJunctionsVolatile(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->clean junctions
    long onCmdCleanJunctions(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->join junctions
    long onCmdJoinJunctions(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->clear invalid crossings
    long onCmdCleanInvalidCrossings(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->Configure Options
    long onCmdOptions(FXObject*, FXSelector, void*);

    /// @brief called when user press Ctrl+Z
    long onCmdUndo(FXObject*, FXSelector, void*);

    // @brief called when user press Ctrl+Y
    long onCmdRedo(FXObject*, FXSelector, void*);
    /// @}

    /// @name inherited from GUIMainWindow
    /// @{
    /// @brief get build OpenGL Canvas (due NETEDIT only uses a single View, it always return nullptr)
    FXGLCanvas* getBuildGLCanvas() const;

    /// @brief get current simulation time (pure virtual but we don't need it)
    SUMOTime getCurrentSimTime() const;

    /// @brief get current tracker interval (pure virtual but we don't need it)
    double getTrackerInterval() const;
    /// @}

    /// @brief get default cursor
    FXCursor* getDefaultCursor();

    /// @brief get pointer to undoList
    GNEUndoList* getUndoList();

    /// @brief get ToolbarsGrip
    ToolbarsGrip& getToolbarsGrip();

    /// @brief update control contents after undo/redo or recompute
    void updateControls();

    /// @brief update FXMenuCommands
    void updateSuperModeMenuCommands(int supermode);

protected:
    /// @brief FOX needs this for static members
    GNEApplicationWindow();

    /// @brief the thread that loads the network
    GNELoadThread* myLoadThread;

    /// @brief information whether the gui is currently loading and the load-options shall be greyed out
    bool myAmLoading;

    /// @brief the submenus
    FXMenuPane* myFileMenu,
                *myFileMenuAdditionals,
                *myFileMenuTLS,
                *myFileMenuDemandElements,
                *myEditMenu,
                *myProcessingMenu,
                *myLocatorMenu,
                *myWindowsMenu,
                *myHelpMenu;

    /// @brief A window to display messages, warnings and error in
    GUIMessageWindow* myMessageWindow;

    /// @brief The splitter that divides the main window into view and the log window
    FXSplitter* myMainSplitter;

    /// @brief List of got requests
    FXSynchQue<GUIEvent*> myEvents;

    /// @brief io-event with the load-thread
    FXEX::FXThreadEvent myLoadThreadEvent;

    /// @brief check if had dependent build
    bool hadDependentBuild;

    /// @brief we are responsible for the net
    GNENet* myNet;

    /// @brief the one and only undo list
    GNEUndoList* myUndoList;

    /// @brief Input file pattern
    std::string myConfigPattern;

private:
    /// @brief struct for menu bar file
    struct MenuBarFile {

        /// @brief constructor
        MenuBarFile(GNEApplicationWindow* GNEApp);

        /// @brief build recent files
        void buildRecentFiles(FXMenuPane* fileMenu);

        /// @brief filename for load/save additionals
        std::string myAdditionalsFile;

        /// @brief filename for load/save TLS Programs
        std::string myTLSProgramsFile;

        /// @brief filename for load/save demand elemetns
        std::string myDemandElementsFile;

        /// @brief List of recent config files
        FXRecentFiles myRecentConfigs;

        /// @brief List of recent nets
        FXRecentFiles myRecentNets;

    private:
        /// @brief pointer to current GNEApplicationWindow
        GNEApplicationWindow* myGNEApp;
    };

    /// @brief struct for File menu commands
    struct FileMenuCommands {

        /// @brief constructor
        FileMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildFileMenuCommands(FXMenuPane* editMenu);

        /// @brief FXMenuCommand for enable or disable save additionals
        FXMenuCommand* saveAdditionals;

        /// @brief FXMenuCommand for enable or disable save additionals As
        FXMenuCommand* saveAdditionalsAs;

        /// @brief FXMenuCommand for enable or disable save additionals
        FXMenuCommand* saveTLSPrograms;

        /// @brief FXMenuCommand for enable or disable save demand elements
        FXMenuCommand* saveDemandElements;

        /// @brief FXMenuCommand for enable or disable save demand elements as
        FXMenuCommand* saveDemandElementsAs;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;
    };

    /// @brief struct for network menu commands
    struct NetworkMenuCommands {

        /// @brief constructor
        NetworkMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildNetworkMenuCommands(FXMenuPane* editMenu);

        /// @brief show all menu commands
        void showNetworkMenuCommands();

        /// @brief hide all menu commands
        void hideNetworkMenuCommands();

        /// @brief menu command for create edge
        FXMenuCommand* createEdgeMode;

        /// @brief menu command for move mode
        FXMenuCommand* moveMode;

        /// @brief menu command for delete mode
        FXMenuCommand* deleteMode;

        /// @brief menu command for inspect mode
        FXMenuCommand* inspectMode;

        /// @brief menu command for select mode
        FXMenuCommand* selectMode;

        /// @brief menu command for connect mode
        FXMenuCommand* connectMode;

        /// @brief menu command for prohibition mode
        FXMenuCommand* prohibitionMode;

        /// @brief menu command for TLS Mode
        FXMenuCommand* TLSMode;

        /// @brief menu command for additional mode
        FXMenuCommand* additionalMode;

        /// @brief menu command for crossing mode
        FXMenuCommand* crossingMode;

        /// @brief menu command for TAZ mode
        FXMenuCommand* TAZMode;

        /// @brief menu command for shape mode
        FXMenuCommand* shapeMode;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief separator between sets of FXMenuCommand
        FXMenuSeparator* myHorizontalSeparator;
    };

    /// @brief struct for Demand menu commands
    struct DemandMenuCommands {

        /// @brief constructor
        DemandMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildDemandMenuCommands(FXMenuPane* editMenu);

        /// @brief show all menu commands
        void showDemandMenuCommands();

        /// @brief hide all menu commands
        void hideDemandMenuCommands();

        /// @brief menu command for route mode
        FXMenuCommand* routeMode;

        /// @brief menu command for vehicle mode
        FXMenuCommand* vehicleMode;

        /// @brief menu command for vehicle type mode
        FXMenuCommand* vehicleTypeMode;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief separator between sets of FXMenuCommand
        FXMenuSeparator* myHorizontalSeparator;
    };

    /// @brief Toolbars Grip
    ToolbarsGrip myToolbarsGrip;

    /// @brief MenuBarFile
    MenuBarFile myMenuBarFile;

    /// @brief File Menu Commands
    FileMenuCommands myFileMenuCommands;

    /// @brief Network Menu Commands
    NetworkMenuCommands myNetworkMenuCommands;

    /// @brief Demand Menu Commands
    DemandMenuCommands myDemandMenuCommands;

    /// @brief pointer to current view net
    GNEViewNet* myViewNet;

    /// @brief the prefix for the window title
    const FXString myTitlePrefix;

    /// @brief The menu used for the MDI-windows
    FXMDIMenu* myMDIMenu;

    /// @brief Builds the menu bar
    void fillMenuBar();

    /// @brief starts to load a netimport configuration or a network */
    void loadConfigOrNet(const std::string file, bool isNet, bool isReload = false, bool useStartupOptions = false, bool newNet = false);

    /// @brief this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /// @brief warns about unsaved changes and gives the user the option to abort
    bool continueWithUnsavedChanges();

    /// @brief warns about unsaved changes in additionals and gives the user the option to abort
    bool continueWithUnsavedAdditionalChanges();
};


#endif

/****************************************************************************/

