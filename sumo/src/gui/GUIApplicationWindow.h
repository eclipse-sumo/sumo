#ifndef GUIApplicationWindow_h
#define GUIApplicationWindow_h
//---------------------------------------------------------------------------//
//                        GUIApplicationWindow.h -
//  Class for the main gui window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.22  2004/07/02 08:35:30  dkrajzew
// all 0.8.0.2 update steps
//
// Revision 1.21  2004/04/23 12:34:54  dkrajzew
// now, all tracker and tables are updated
//
// Revision 1.20  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.19  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane to come makes it necessary)
//
// Revision 1.18  2003/11/12 14:06:32  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.17  2003/11/11 08:40:45  dkrajzew
// consequent position2D instead of two doubles implemented; logging moved from utils to microsim
//
// Revision 1.16  2003/10/27 10:47:03  dkrajzew
// added to possibility to close the application after a simulations end without user interaction
//
// Revision 1.15  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.14  2003/08/20 11:55:49  dkrajzew
// "Settings"-menu added
//
// Revision 1.13  2003/07/16 15:16:26  dkrajzew
// unneeded uncommented files removed
//
// Revision 1.12  2003/07/07 08:08:33  dkrajzew
// The restart-button was removed and the play-button has now the function to continue the simulation if it has been started before
//
// Revision 1.11  2003/06/24 14:28:53  dkrajzew
// first steps towards a settings manipulation applied
//
// Revision 1.10  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end and be started with a simulation now;
//
// Revision 1.9  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
//
// Revision 1.8  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.7  2003/04/16 10:12:12  dkrajzew
// fontrendeder removed temporarily
//
// Revision 1.6  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.5  2003/04/04 15:04:53  roessel
// Added #include <qtoolbutton.h>
//
// Revision 1.4  2003/03/17 14:03:24  dkrajzew
// Dialog about simulation restart debugged
//
// Revision 1.3  2003/03/12 16:55:16  dkrajzew
// centering of objects debugged
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <fx.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <helpers/ValueRetriever.h>
#include <helpers/ValueSource.h>
#include "GUISUMOViewParent.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILoadThread;
class GUIRunThread;
class GUIMessageWindow;
class GUIEvent;
class GUIParameterTracker;
class GUIParameterTableWindow;
class GUIThreadFactory;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * @class GUIApplicationWindow
 * The main window of the SUMO-gui. Contains the file opening support and
 * a canvas to display the simulation representations in.
 *
 * Beside views on the simulation, showed within a MDI-window, the main window
 * may also have some further views (children) assigned which are stored
 * within a separate list.
 *
 * As the aggregated visualisation is quite memory consuming and needs also
 * some cpu-time, it may be swichted of on gui-startup. The information
 * whether aggregated views are allowed is stored within this class, too.
 */
class GUIApplicationWindow :
        public FXMainWindow, public MFXInterThreadEventClient
{
    // FOX-declarations
    FXDECLARE(GUIApplicationWindow)
public:

    /** constructor */
    GUIApplicationWindow(FXApp* a, GUIThreadFactory &threadFactory,
        int glWidth, int glHeight, const std::string &config);

    /** destructor */
    virtual ~GUIApplicationWindow();

    /** @brief Creates the main window
        (required by FOX) */
    void create();

    /// Detaches the tool/menu bar
    void detach();

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    /// Adds a further child window to the list
    void addChild(FXMDIChild *child, bool updateOnSimStep=true);
    void addChild(FXMainWindow *child, bool updateOnSimStep=true);

    /// removes the given child window from the list
    void removeChild(FXMDIChild *child);
    void removeChild(FXMainWindow  *child);

    FXCursor *getDefaultCursor();

    FXGLVisual *getGLVisual() const;
    FXGLCanvas *getBuildGLCanvas() const;


    FXFont *getBoldFont();

public:
    /// Closes the log window
    void showLog();

public:
    void eventOccured();
    void handleEvent_SimulationLoaded(GUIEvent *e);
    void handleEvent_SimulationStep(GUIEvent *e);
    void handleEvent_Message(GUIEvent *e);
    void handleEvent_SimulationEnded(GUIEvent *e);


public:
    /** @brief Called by FOX if a simulation shall be opened
        Called either by FileMenu->Open or the Toolbar Open-Button */
    long onCmdOpen(FXObject*,FXSelector,void*);
    long onCmdReload(FXObject*,FXSelector,void*);
    long onCmdOpenRecent(FXObject*,FXSelector,void*);

    long onCmdClose(FXObject*,FXSelector,void*);

    /** @brief Called by FOX if the application shall be closed
        Called either by FileMenu->Quit, the normal close-menu or SIGINT  */
    long onCmdQuit(FXObject*,FXSelector,void*);

    long onCmdEditChosen(FXObject*,FXSelector,void*);
    virtual long onCmdEditAddWeights(FXObject*,FXSelector,void*);
    long onCmdEditBreakpoints(FXObject*,FXSelector,void*);

    /// Opens the application settings menu
    long onCmdAppSettings(FXObject*,FXSelector,void*);

    /// Opens the simulation settings menu
    long onCmdSimSettings(FXObject*,FXSelector,void*);

    /// Shows the about dialog
    long onCmdAbout(FXObject*,FXSelector,void*);

    long onCmdStart(FXObject*,FXSelector,void*);
    long onCmdStop(FXObject*,FXSelector,void*);
    long onCmdStep(FXObject*,FXSelector,void*);

    long onCmdNewMicro(FXObject*,FXSelector,void*);
    long onCmdNewLaneA(FXObject*,FXSelector,void*);

    long onUpdOpen(FXObject*,FXSelector,void*);
    long onUpdReload(FXObject*,FXSelector,void*);
    long onUpdAddMicro(FXObject*,FXSelector,void*);
    long onUpdAddALane(FXObject*,FXSelector,void*);
    long onUpdStart(FXObject*,FXSelector,void*);
    long onUpdStop(FXObject*,FXSelector,void*);
    long onUpdStep(FXObject*,FXSelector,void*);
    long onUpdEditChosen(FXObject*sender,FXSelector,void*ptr);
    virtual long onUpdEditAddWeights(FXObject*,FXSelector,void*);
    virtual long onUpdEditBreakpoints(FXObject*,FXSelector,void*);
    long onUpdSimSettings(FXObject*sender,FXSelector,void*ptr);

    long onLoadThreadEvent(FXObject*, FXSelector, void*);
    long onRunThreadEvent(FXObject*, FXSelector, void*);
/*
    long onLeftBtnRelease(FXObject*sender,FXSelector,void*ptr);
    long onRightBtnRelease(FXObject*sender,FXSelector,void*ptr);
    long onMouseMove(FXObject*sender,FXSelector,void*ptr);
*/

    FXTimer *addTimeout(FXObject *tgt, FXSelector sel,
        FXuint ms=1000, void *ptr=NULL);
    FXTimer *removeTimeout(FXObject *tgt, FXSelector sel);

    void updateChildren();

private:
    /** starts to load a simulation */
    void load(const std::string &file);

    /** this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /** opens a new simulation display */
    void openNewView(GUISUMOViewParent::ViewType type);

protected:
    /// FOX needs this for static members
    GUIApplicationWindow() { }
    /** called when an event occures */
//    bool event(QEvent *e);

private:
    /// Builds the menu bar
    void fillMenuBar();

    /// Builds the tool bar
    void fillToolBar();

protected:
    /** the name of the simulation */
    std::string _name;

    /** the thread that loads simulations */
    GUILoadThread *myLoadThread;

    /** the thread that runs simulations */
    GUIRunThread *myRunThread;

    /** the information whether the simulation was started before */
    bool _wasStarted;

    /// The current view number
    size_t myViewNumber;

    /// information whether the gui is currently loading and the load-options shall be greyed out
    bool myAmLoading;

    /// the submenus
    FXMenuPane *myFileMenu, *myEditMenu, *mySettingsMenu,
        *myWindowsMenu, *myHelpMenu;

    /// The openGL-maximum screen sizes
    int myGLWidth, myGLHeight;

    std::vector<FXMDIChild*> mySubWindows;
    std::vector<FXMainWindow*> myTrackerWindows;

    /// A window to display messages, warnings and error in
    GUIMessageWindow *myMessageWindow;

    /// The splitter that divides the main window into vies and the log window
    FXSplitter *myMainSplitter;

    /// The multi view panel
    FXMDIClient *myMDIClient;

    /// The status bar
    FXStatusBar *myStatusbar;

    /// for some menu detaching fun
    FXToolBarShell *myToolBarDrag, *myMenuBarDrag;

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
    FXToolBar *myToolBar;

    /// The gl-visual used
    FXGLVisual *myGLVisual;

    /// the simulation step display
    FXEX::FXLCDLabel *myLCDLabel;

    /// io-event with the load-thread
    FXEX::FXThreadEvent myLoadThreadEvent;

    /// io-event with the run-thread
    FXEX::FXThreadEvent myRunThreadEvent;

    /// Font used for popup-menu titles
    FXFont *myBoldFont;

    /// List of recent files
    FXRecentFiles myRecentFiles;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

