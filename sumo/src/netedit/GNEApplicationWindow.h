/****************************************************************************/
/// @file    GNEApplicationWindow.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The main window of Netedit (adapted from GUIApplicationWindow)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
class GNEApplicationWindow :
    public GUIMainWindow, public MFXInterThreadEventClient {
    // FOX-declarations
    FXDECLARE(GNEApplicationWindow)

public:

    /** @brief Constructor
     * @param[in] a The FOX application
     * @param[in] configPattern The pattern used for loading configurations
     */
    GNEApplicationWindow(FXApp* a, const std::string& configPattern);


    /// @brief Destructor
    virtual ~GNEApplicationWindow();


    /// @brief Creates the main window (required by FOX)
    virtual void create();


    /// Detaches the tool/menu bar
    virtual void detach();


    void loadOnStartup();


    void dependentBuild();

    void setStatusBarText(const std::string&);


    /// @name Inter-thread event handling
    /// @{

    virtual void eventOccured();
    void handleEvent_NetworkLoaded(GUIEvent* e);
    void handleEvent_Message(GUIEvent* e);
    /// @}



    /// @name FOX-callbacks
    /// @{
    long onCmdNewNetwork(FXObject*, FXSelector, void*);
    long onCmdOpenConfiguration(FXObject*, FXSelector, void*);
    long onCmdOpenNetwork(FXObject*, FXSelector, void*);
    long onCmdOpenForeign(FXObject*, FXSelector, void*);
    long onCmdOpenShapes(FXObject*, FXSelector, void*);
    long onCmdReload(FXObject*, FXSelector, void*);
    long onCmdOpenRecent(FXObject*, FXSelector, void*);
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief Called on menu commands from the Locator menu
    long onCmdLocate(FXObject*, FXSelector, void*);

    long onCmdSaveNetwork(FXObject*, FXSelector, void*);
    long onUpdSaveNetwork(FXObject*, FXSelector, void*);
    long onCmdSaveAsNetwork(FXObject*, FXSelector, void*);
    long onUpdNeedsNetwork(FXObject*, FXSelector, void*);
    long onCmdSaveAsPlainXML(FXObject*, FXSelector, void*);
    long onCmdSaveJoined(FXObject*, FXSelector, void*);
    long onCmdSavePois(FXObject*, FXSelector, void*);

    /// @brief handle keys
    long onKeyPress(FXObject* o, FXSelector sel, void* data);
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);

    /** @brief Called by FOX if the application shall be closed
        Called either by FileMenu->Quit, the normal close-menu or SIGINT  */
    long onCmdQuit(FXObject*, FXSelector, void*);

    long onCmdEditChosen(FXObject*, FXSelector, void*);

    /// Opens the application settings menu
    long onCmdAppSettings(FXObject*, FXSelector, void*);
    /// Toggle gaming mode
    long onCmdGaming(FXObject*, FXSelector, void*);

    /// Shows the about dialog
    long onCmdAbout(FXObject*, FXSelector, void*);

    long onCmdNewView(FXObject*, FXSelector, void*);

    long onUpdOpen(FXObject*, FXSelector, void*);
    long onCmdClearMsgWindow(FXObject*, FXSelector, void*);

    long onLoadThreadEvent(FXObject*, FXSelector, void*);
    /// Somebody wants our clipped text
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

    FXGLCanvas* getBuildGLCanvas() const;

    /// pure virtual but we don't need it
    SUMOTime getCurrentSimTime() const {
        return 0;
    }

    FXCursor* getDefaultCursor();

    GNEUndoList* getUndoList() {
        return myUndoList;
    }

    /// @brief update control contents after undo/redo or recompute
    void updateControls();


protected:
    /// FOX needs this for static members
    GNEApplicationWindow() { }

    /// Builds the menu bar
    virtual void fillMenuBar();

    /** the name of the simulation */
    std::string myName;

    /** the thread that loads the network */
    GNELoadThread* myLoadThread;

    /// The current view number
    size_t myViewNumber;

    /// information whether the gui is currently loading and the load-options shall be greyed out
    bool myAmLoading;

    /// the submenus
    FXMenuPane* myFileMenu,
                *myEditMenu,
                *myLocatorMenu,
                *myProcessingMenu,
                *myWindowsMenu,
                *myHelpMenu;

    /// A window to display messages, warnings and error in
    GUIMessageWindow* myMessageWindow;

    /// The splitter that divides the main window into vies and the log window
    FXSplitter* myMainSplitter;

    /// for some menu detaching fun
    FXToolBarShell* myToolBarDrag1, *myToolBarDrag2, *myToolBarDrag3,
                    *myToolBarDrag4, *myToolBarDrag5,
                    *myMenuBarDrag;

    /// List of got requests
    MFXEventQue<GUIEvent*> myEvents;

    /// The menu used for the MDI-windows
    FXMDIMenu* myMDIMenu;

    /// The application menu bar
    FXMenuBar* myMenuBar;

    /// The application tool bar
    FXToolBar* myToolBar1, *myToolBar2, *myToolBar3, *myToolBar4, *myToolBar5;

    /// io-event with the load-thread
    FXEX::FXThreadEvent myLoadThreadEvent;

    /// List of recent config files
    FXRecentFiles myRecentConfigs;

    /// List of recent nets
    FXRecentFiles myRecentNets;

    /// Input file pattern
    std::string myConfigPattern;

    bool hadDependentBuild;

    /// we are responsible for the net
    GNENet* myNet;

    /// the one and only undo list
    GNEUndoList* myUndoList;

    /// the prefix for the window title
    const FXString myTitlePrefix;

    /**
     * @class GNEShapeHandler
     * @brief The XML-Handler for shapes loading network loading
     *
     * This subclasses ShapeHandler with netbuild specific function
     */
    class GNEShapeHandler : public ShapeHandler {
        public:
            GNEShapeHandler(const std::string& file, GNENet* net, ShapeContainer& sc) :
                ShapeHandler(file, sc),
                myNet(net) {}

            /// @brief Destructor
            virtual ~GNEShapeHandler() {}

            Position getLanePos(const std::string& poiID, const std::string& laneID, SUMOReal lanePos);

        private:
            GNENet* myNet;
    };

private:
    /** starts to load a netimport configuration or a network */
    void loadConfigOrNet(const std::string file, bool isNet, bool isReload = false, bool optionsReady = false, bool newNet = false);

    /** this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /** opens a new simulation display */
    GUISUMOAbstractView* openNewView();

    // @brief convenience method
    GNEViewNet* getView();

    /* @brief warns about unsaved changes and gives the user the option to abort
     */
    bool continueWithUnsavedChanges();
};


#endif

/****************************************************************************/

