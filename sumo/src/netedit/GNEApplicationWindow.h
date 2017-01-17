/****************************************************************************/
/// @file    GNEApplicationWindow.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The main window of Netedit (adapted from GUIApplicationWindow)
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
#ifndef GNEApplicationWindow_h
#define GNEApplicationWindow_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <fx.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>
#include <utils/shapes/ShapeHandler.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIMessageWindow.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNELoadThread;
class GNEUndoList;
class GNENet;
class GNEViewNet;
class GUIEvent;
class GUIParameterTracker;
class GUIParameterTableWindow;


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

    /**@brief Constructor
     * @param[in] a The FOX application
     * @param[in] configPattern The pattern used for loading configurations
     */
    GNEApplicationWindow(FXApp* a, const std::string& configPattern);

    /// @brief Destructor
    virtual ~GNEApplicationWindow();

    /// @brief Creates the main window (required by FOX)
    virtual void create();

    /// @brief detaches the tool/menu bar
    virtual void detach();

    /// @brief load net on startup
    void loadOptionOnStartup();

    /// @brief build dependent
    void dependentBuild();

    /// @brief set text of the statusBar
    void setStatusBarText(const std::string& statusBarText);

    /// @brief set additionals file
    void setAdditionalsFile(const std::string& additionalsFile);

    /// @name Inter-thread event handling
    /// @{
    /// @brief called when an event occured
    virtual void eventOccured();

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

    /// @brief called when the command/FXCall open shapes is executed
    long onCmdOpenShapes(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open additionals is executed
    long onCmdOpenAdditionals(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall reload is executed
    long onCmdReload(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall open recent is executed
    long onCmdOpenRecent(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall close is executed
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall locate is executed
    long onCmdLocate(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save network is executed
    long onCmdSaveNetwork(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save additionals is executed
    long onCmdSaveAdditionals(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall save additionals as is executed
    long onCmdSaveAdditionalsAs(FXObject*, FXSelector, void*);

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

    /// @brief called when the command/FXCall save pois is executed
    long onCmdSavePois(FXObject*, FXSelector, void*);

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

    /// @brief Opens the application settings menu
    long onCmdAppSettings(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall toogle gaming mode is executed
    long onCmdGaming(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall show about dialog is executed
    long onCmdAbout(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall new view is executed
    long onCmdNewView(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall on update open executed
    long onUpdOpen(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall clear message windows is executed
    long onCmdClearMsgWindow(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall load thread is executed
    long onLoadThreadEvent(FXObject*, FXSelector, void*);

    /// @brief called when the command/FXCall clipboard request is executed
    long onClipboardRequest(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits an edit-mode hotkey
    long onCmdSetMode(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits esc
    long onCmdAbort(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits del
    long onCmdDel(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user hits enter
    long onCmdEnter(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user selects help->Documentation
    long onCmdHelp(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief called if the user selects Processing->compute junctions
    long onCmdComputeJunctions(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->clean junctions
    long onCmdCleanJunctions(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->join junctions
    long onCmdJoinJunctions(FXObject*, FXSelector, void*);

    /// @brief called if the user selects Processing->Configure Options
    long onCmdOptions(FXObject*, FXSelector, void*);
    /// @}

    /// @brief get build OpenGL Canvas
    FXGLCanvas* getBuildGLCanvas() const;

    /// @brief get current simulation time (pure virtual but we don't need it)
    SUMOTime getCurrentSimTime() const;

    /// @brief get default cursor
    FXCursor* getDefaultCursor();

    /// @brief get pointer to undoList
    GNEUndoList* getUndoList();

    /// @brief update control contents after undo/redo or recompute
    void updateControls();

protected:
    /// @brief FOX needs this for static members
    GNEApplicationWindow() {}

    /// @brief Builds the menu bar
    virtual void fillMenuBar();

    /// @brief the name of the simulation
    std::string myName;

    /// @brief the thread that loads the network
    GNELoadThread* myLoadThread;

    /// @brief The current view number
    int myViewNumber;

    /// @brief information whether the gui is currently loading and the load-options shall be greyed out
    bool myAmLoading;

    /// @brief the submenus
    FXMenuPane* myFileMenu,
                *myEditMenu,
                *myProcessingMenu,
                *myLocatorMenu,
                *myWindowsMenu,
                *myHelpMenu;

    /// @brief A window to display messages, warnings and error in
    GUIMessageWindow* myMessageWindow;

    /// @brief The splitter that divides the main window into view and the log window
    FXSplitter* myMainSplitter;

    /// @brief for some menu detaching fun
    FXToolBarShell* myToolBarDrag1, *myToolBarDrag2, *myToolBarDrag3,
                    *myToolBarDrag4, *myToolBarDrag5,
                    *myMenuBarDrag;

    /// @brief List of got requests
    MFXEventQue<GUIEvent*> myEvents;

    /// @brief The menu used for the MDI-windows
    FXMDIMenu* myMDIMenu;

    /// @brief The application menu bar
    FXMenuBar* myMenuBar;

    /// @brief The application tool bar
    FXToolBar* myToolBar1, *myToolBar2, *myToolBar3, *myToolBar4, *myToolBar5;

    /// @brief io-event with the load-thread
    FXEX::FXThreadEvent myLoadThreadEvent;

    /// @brief List of recent config files
    FXRecentFiles myRecentConfigs;

    /// @brief List of recent nets
    FXRecentFiles myRecentNets;

    /// @brief Input file pattern
    std::string myConfigPattern;

    /// @brief check if had dependent build
    bool hadDependentBuild;

    /// @brief we are responsible for the net
    GNENet* myNet;

    /// @brief the one and only undo list
    GNEUndoList* myUndoList;

    /// @brief the prefix for the window title
    const FXString myTitlePrefix;

    /// @brief filename for load/save additionals
    std::string myAdditionalsFile;

    /**
     * @class GNEShapeHandler
     * @brief The XML-Handler for shapes loading network loading
     *
     * This subclasses ShapeHandler with netbuild specific function
     */
    class GNEShapeHandler : public ShapeHandler {
    public:
        /**@brief Constructor
         * @param[in] file file with the shapes
         * @param[in] net network in which add shapes
         * @param[in] ShapeContainer reference to ShapeContainer
         */
        GNEShapeHandler(const std::string& file, GNENet* net, ShapeContainer& sc);

        /// @brief Destructor
        virtual ~GNEShapeHandler();

        /**@brief get lane position
         * @param[in] poi poi ID
         * @param[in] laneID lane ID
         * @param[in] SlanePos position in the lane
         */
        Position getLanePos(const std::string& poiID, const std::string& laneID, SUMOReal lanePos);

    private:
        /// @brief pointer of the net
        GNENet* myNet;
    };

private:
    /// @brief starts to load a netimport configuration or a network */
    void loadConfigOrNet(const std::string file, bool isNet, bool isReload = false, bool useStartupOptions = false, bool newNet = false);

    /// @brief this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /// @brief opens a new simulation display
    GUISUMOAbstractView* openNewView();

    /// @brief convenience method
    GNEViewNet* getView();

    /// @brief warns about unsaved changes and gives the user the option to abort
    bool continueWithUnsavedChanges();
};


#endif

/****************************************************************************/

