/****************************************************************************/
/// @file    GUIApplicationWindow.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The main window of the SUMO-gui.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIApplicationWindow_h
#define GUIApplicationWindow_h


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
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/common/ValueRetriever.h>
#include <utils/common/ValueSource.h>
#include "GUISUMOViewParent.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUILoadThread;
class GUIRunThread;
class GUIMessageWindow;
class GUIEvent;
class GUIParameterTracker;
class GUIParameterTableWindow;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIApplicationWindow
 * @brief The main window of the SUMO-gui.
 *
 * Contains the file opening support and a canvas to display the simulation
 *  representations in.
 *
 * Beside views on the simulation, shown within a MDI-window, the main window
 * may also have some further views (children) assigned which are stored
 * within a separate list.
 */
class GUIApplicationWindow :
            public GUIMainWindow, public MFXInterThreadEventClient {
    // FOX-declarations
    FXDECLARE(GUIApplicationWindow)
public:

    /** @brief Constructor
     * @param[in] a The FOX application
     * @param[in] configPattern The pattern used for loading configurations
     */
    GUIApplicationWindow(FXApp* a, const std::string &configPattern);


    /// @brief Destructor
    virtual ~GUIApplicationWindow();


    /// @brief Creates the main window (required by FOX)
    virtual void create();


    /// Detaches the tool/menu bar
    virtual void detach();


    void loadOnStartup(const std::string &config, bool run);


    void dependentBuild(bool game);

    void setStatusBarText(const std::string &);


    /// @name Inter-thread event handling
    /// @{

    virtual void eventOccured();
    void handleEvent_SimulationLoaded(GUIEvent *e);
    void handleEvent_SimulationStep(GUIEvent *e);
    void handleEvent_Message(GUIEvent *e);
    void handleEvent_SimulationEnded(GUIEvent *e);
    /// @}



    /// @name FOX-callbacks
    /// @{
    long onCmdOpenConfiguration(FXObject*,FXSelector,void*);
    long onCmdOpenNetwork(FXObject*,FXSelector,void*);
    long onCmdReload(FXObject*,FXSelector,void*);
    long onCmdOpenRecent(FXObject*,FXSelector,void*);

    long onCmdClose(FXObject*,FXSelector,void*);

    /** @brief Called by FOX if the application shall be closed
        Called either by FileMenu->Quit, the normal close-menu or SIGINT  */
    long onCmdQuit(FXObject*,FXSelector,void*);

    long onCmdEditChosen(FXObject*,FXSelector,void*);
    long onCmdEditBreakpoints(FXObject*,FXSelector,void*);

    /// Opens the application settings menu
    long onCmdAppSettings(FXObject*,FXSelector,void*);
    /// Toggle gaming mode
    long onCmdGaming(FXObject*,FXSelector,void*);

    /// Shows the about dialog
    long onCmdAbout(FXObject*,FXSelector,void*);

    long onCmdStart(FXObject*,FXSelector,void*);
    long onCmdStop(FXObject*,FXSelector,void*);
    long onCmdStep(FXObject*,FXSelector,void*);

    long onCmdNewView(FXObject*,FXSelector,void*);

    long onUpdOpen(FXObject*,FXSelector,void*);
    long onUpdReload(FXObject*,FXSelector,void*);
    long onUpdOpenRecent(FXObject*,FXSelector,void*);
    long onUpdAddMicro(FXObject*,FXSelector,void*);
    virtual long onUpdStart(FXObject*,FXSelector,void*);
    long onUpdStop(FXObject*,FXSelector,void*);
    long onUpdStep(FXObject*,FXSelector,void*);
    long onUpdEditChosen(FXObject*sender,FXSelector,void*ptr);
    virtual long onUpdEditBreakpoints(FXObject*,FXSelector,void*);
    long onCmdClearMsgWindow(FXObject*,FXSelector,void*);

    long onLoadThreadEvent(FXObject*, FXSelector, void*);
    long onRunThreadEvent(FXObject*, FXSelector, void*);
    /// Somebody wants our clipped text
    long onClipboardRequest(FXObject* sender,FXSelector sel,void* ptr);
    /// @}

    FXGLCanvas *getBuildGLCanvas() const;
    SUMOTime getCurrentSimTime() const;

    FXCursor *getDefaultCursor();

    /** @brief Loads a selection from a given file
     * @param[in] file The file to load the selection from
     * @param[out] msg The error message
     * @return Whether the selection could be loaded (fills the error message if not)
     * @see GUIMainWindow::loadSelection
     */
    bool loadSelection(const std::string &file, std::string &msg) throw();

protected:
    virtual void addToWindowsMenu(FXMenuPane *) { }

private:
    /** starts to load a simulation */
    void load(const std::string &file, bool isNet, bool isReload=false);

    /** this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /** opens a new simulation display */
    GUISUMOAbstractView* openNewView();

protected:
    /// FOX needs this for static members
    GUIApplicationWindow() { }

protected:
    /// Builds the menu bar
    virtual void fillMenuBar();

    /// Builds the tool bar
    virtual void buildToolBars();

protected:
    /** the name of the simulation */
    std::string myName;

    /** the thread that loads simulations */
    GUILoadThread *myLoadThread;

    /** the thread that runs simulations */
    GUIRunThread *myRunThread;

    /** the information whether the simulation was started before */
    bool myWasStarted;

    /// The current view number
    size_t myViewNumber;

    /// information whether the gui is currently loading and the load-options shall be greyed out
    bool myAmLoading;

    /// the submenus
    FXMenuPane *myFileMenu, *myEditMenu, *mySettingsMenu,
    *myWindowsMenu, *myHelpMenu;

    /// A window to display messages, warnings and error in
    GUIMessageWindow *myMessageWindow;

    /// The splitter that divides the main window into vies and the log window
    FXSplitter *myMainSplitter;

    /// for some menu detaching fun
    FXToolBarShell *myToolBarDrag1, *myToolBarDrag2, *myToolBarDrag3,
    *myToolBarDrag4, *myToolBarDrag5,
    *myMenuBarDrag;

    ///
    FXRealSpinDial *mySimDelayTarget;

    /// The simulation delay
    FXdouble mySimDelay;

    /// List of got requests
    MFXEventQue myEvents;

    /// The menu used for the MDI-windows
    FXMDIMenu *myMDIMenu;

    /// The application menu bar
    FXMenuBar *myMenuBar;

    /// The application tool bar
    FXToolBar *myToolBar1, *myToolBar2, *myToolBar3, *myToolBar4, *myToolBar5;

    /// the simulation step display
    FXEX::FXLCDLabel *myLCDLabel;

    /// io-event with the load-thread
    FXEX::FXThreadEvent myLoadThreadEvent;

    /// io-event with the run-thread
    FXEX::FXThreadEvent myRunThreadEvent;

    /// List of recent config files
    FXRecentFiles myRecentConfigs;

    /// List of recent nets
    FXRecentFiles myRecentNets;

    /// Input file pattern
    std::string myConfigPattern;

    bool hadDependentBuild;


};


#endif

/****************************************************************************/

