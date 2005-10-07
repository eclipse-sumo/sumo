//---------------------------------------------------------------------------//
//                        GUIApplicationWindow.cpp
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.44  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.43  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.42  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.41  2005/07/12 12:08:23  dkrajzew
// patches only (unused features removed)
//
// Revision 1.40  2005/05/04 07:46:08  dkrajzew
// ported to fox1.4
//
// Revision 1.39  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.38  2004/12/16 12:12:58  dkrajzew
// first steps towards loading of selections between different applications
//
// Revision 1.37  2004/12/15 09:20:17  dkrajzew
// made guisim independent of giant/netedit
//
// Revision 1.36  2004/12/12 17:23:58  agaubatz
// Editor Tool Widgets included
//
// Revision 1.35  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.34  2004/08/02 11:44:31  dkrajzew
// ported to fox 1.2; patched missing unlock on unwished program termination
//
// Revision 1.33  2004/07/02 08:35:30  dkrajzew
// all 0.8.0.2 update steps
//
// Revision 1.32  2004/04/23 12:34:54  dkrajzew
// now, all tracker and tables are updated
//
// Revision 1.31  2004/04/02 11:08:35  dkrajzew
// storing settings in a registry added (window size and last folder used)
//
// Revision 1.30  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.29  2003/12/09 11:22:13  dkrajzew
// errors during simulation are now caught properly
//
// Revision 1.28  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane to come makes it necessary)
//
// Revision 1.27  2003/11/20 13:17:32  dkrajzew
// further work on aggregated views
//
// Revision 1.26  2003/11/12 14:06:32  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.25  2003/11/11 08:40:45  dkrajzew
// consequent position2D instead of two SUMOReals implemented; logging moved from
//  utils to microsim
//
// Revision 1.24  2003/10/27 10:47:03  dkrajzew
// added to possibility to close the application after a simulations end
//  without user interaction
//
// Revision 1.23  2003/09/22 14:54:22  dkrajzew
// some refactoring on GUILoadThread-usage
//
// Revision 1.22  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.21  2003/08/20 11:55:49  dkrajzew
// "Settings"-menu added
//
// Revision 1.20  2003/08/14 13:41:33  dkrajzew
// a lower priorised update-method is now used
//
// Revision 1.19  2003/07/30 12:50:22  dkrajzew
// subwindows do not close at main window closing-bug patched
//
// Revision 1.18  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.17  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.16  2003/07/16 15:16:26  dkrajzew
// unneeded uncommented files removed
//
// Revision 1.15  2003/07/07 08:08:33  dkrajzew
// The restart-button was removed and the play-button has now the function to
//  continue the simulation if it has been started before
//
// Revision 1.14  2003/06/24 14:28:53  dkrajzew
// first steps towards a settings manipulation applied
//
// Revision 1.13  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end
//  and be started with a simulation now;
//
// Revision 1.12  2003/06/18 11:04:22  dkrajzew
// new error processing adapted; new usage of fonts adapted
//
// Revision 1.11  2003/06/05 06:26:15  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.10  2003/05/21 15:15:40  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.9  2003/05/20 13:05:33  dkrajzew
// temporary font patch
//
// Revision 1.8  2003/05/20 09:23:53  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.7  2003/04/16 10:12:11  dkrajzew
// fontrendeder removed temporarily
//
// Revision 1.6  2003/04/16 09:50:03  dkrajzew
// centering of the network debugged; additional parameter of maximum display
//  size added
//
// Revision 1.5  2003/03/17 14:03:23  dkrajzew
// Dialog about simulation restart debugged
//
// Revision 1.4  2003/03/12 16:55:14  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <fx3d.h>
#include <string>
#include <sstream>
#include <algorithm>

#include <guisim/GUINet.h>

#include "GUISUMOViewParent.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"

#include <utils/common/ToString.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <sumo_version.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include "GUIEvent_SimulationLoaded.h"
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include "GUIGlobals.h"
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include "dialogs/GUIDialog_AboutSUMO.h"
#include "dialogs/GUIDialog_AppSettings.h"
#include "dialogs/GUIDialog_SimSettings.h"
#include "dialogs/GUIDialog_MicroViewSettings.h"
#include "dialogs/GUIDialog_EditAddWeights.h"
#include "dialogs/GUIDialog_Breakpoints.h"
#include "GUIThreadFactory.h"
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <guisim/GUINetWrapper.h>
#include <guisim/GUISelectionLoader.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#ifdef HAVE_MESOSIM
#include <microsim/MSGlobals.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;
using namespace FXEX;


/* =========================================================================
 * FOX-declarations
 * ======================================================================= */
FXDEFMAP(GUIApplicationWindow) GUIApplicationWindowMap[]=
{
    //________Message_Type____________ID________________________Message_Handler________
    FXMAPFUNC(SEL_COMMAND,  MID_QUIT,        GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_QUIT,        GUIApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,      GUIApplicationWindow::onCmdQuit),

    FXMAPFUNC(SEL_COMMAND,  MID_OPEN,              GUIApplicationWindow::onCmdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,        GUIApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_COMMAND,  MID_RELOAD,            GUIApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE,             GUIApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITCHOSEN,        GUIApplicationWindow::onCmdEditChosen),
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_ADD_WEIGHTS,  GUIApplicationWindow::onCmdEditAddWeights),
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_BREAKPOINTS,  GUIApplicationWindow::onCmdEditBreakpoints),

    FXMAPFUNC(SEL_COMMAND,  MID_APPSETTINGS,        GUIApplicationWindow::onCmdAppSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMSETTINGS,        GUIApplicationWindow::onCmdSimSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_ABOUT,              GUIApplicationWindow::onCmdAbout),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_MICROVIEW,      GUIApplicationWindow::onCmdNewMicro),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_LANEAVIEW,      GUIApplicationWindow::onCmdNewLaneA),
    FXMAPFUNC(SEL_COMMAND,  MID_START,              GUIApplicationWindow::onCmdStart),
    FXMAPFUNC(SEL_COMMAND,  MID_STOP,               GUIApplicationWindow::onCmdStop),
    FXMAPFUNC(SEL_COMMAND,  MID_STEP,               GUIApplicationWindow::onCmdStep),
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW, GUIApplicationWindow::onCmdClearMsgWindow),


    FXMAPFUNC(SEL_UPDATE,   MID_OPEN,              GUIApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_RELOAD,            GUIApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_MICROVIEW,     GUIApplicationWindow::onUpdAddMicro),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_LANEAVIEW,     GUIApplicationWindow::onUpdAddALane),
    FXMAPFUNC(SEL_UPDATE,   MID_START,             GUIApplicationWindow::onUpdStart),
    FXMAPFUNC(SEL_UPDATE,   MID_STOP,              GUIApplicationWindow::onUpdStop),
    FXMAPFUNC(SEL_UPDATE,   MID_STEP,              GUIApplicationWindow::onUpdStep),
    FXMAPFUNC(SEL_UPDATE,   MID_EDITCHOSEN,        GUIApplicationWindow::onUpdEditChosen),
    FXMAPFUNC(SEL_UPDATE,   MID_EDIT_ADD_WEIGHTS,  GUIApplicationWindow::onUpdEditAddWeights),
    FXMAPFUNC(SEL_UPDATE,   MID_EDIT_BREAKPOINTS,  GUIApplicationWindow::onUpdEditBreakpoints),
    FXMAPFUNC(SEL_UPDATE,   MID_SIMSETTINGS,       GUIApplicationWindow::onUpdSimSettings),

    FXMAPFUNC(SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT, GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(SEL_THREAD_EVENT, ID_RUNTHREAD_EVENT,  GUIApplicationWindow::onRunThreadEvent),
    FXMAPFUNC(SEL_THREAD, ID_LOADTHREAD_EVENT,       GUIApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(SEL_THREAD, ID_RUNTHREAD_EVENT,        GUIApplicationWindow::onRunThreadEvent),

//    FXMAPFUNC(SEL_COMMAND,  MID_CUTSWELL,               GUIApplicationWindow::onCmdCutSwell),

};

// Object implementation
FXIMPLEMENT(GUIApplicationWindow, FXMainWindow, GUIApplicationWindowMap, ARRAYNUMBER(GUIApplicationWindowMap))

/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIApplicationWindow::GUIApplicationWindow(FXApp* a,
                                           int glWidth, int glHeight,
                                           const std::string &configPattern)
    : GUIMainWindow(a, glWidth, glHeight),
    myLoadThread(0), myRunThread(0),
    myAmLoading(false),
    mySimDelay(50), myConfigPattern(configPattern), hadDependentBuild(false)
{
    GUIIconSubSys::init(a);
}


void
GUIApplicationWindow::dependentBuild(GUIThreadFactory &threadFactory)
{
    // do this not twice
    if(hadDependentBuild) {
        return;
    }
    hadDependentBuild = true;

    setTarget(this);
    setSelector(MID_WINDOW);
    GUITexturesHelper::init(getApp());

    // build menu bar
    myMenuBarDrag=new FXToolBarShell(this,FRAME_NORMAL);
    myMenuBar = new FXMenuBar(myTopDock, myMenuBarDrag,
        LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP,
        TOOLBARGRIP_DOUBLE);
    buildToolBars();
    // build the thread - io
    myLoadThreadEvent.setTarget(this),
    myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    myRunThreadEvent.setTarget(this),
    myRunThreadEvent.setSelector(ID_RUNTHREAD_EVENT);

    // build the status bar
    myStatusbar = new FXStatusBar(this,
        LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_RAISED);

    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this,
        SPLITTER_REVERSED|SPLITTER_VERTICAL|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_TRACKING|FRAME_RAISED|FRAME_THICK);
    myMDIClient = new FXMDIClient(myMainSplitter,
        LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK);
    myMDIMenu = new FXMDIMenu(this,myMDIClient);
    new FXMDIWindowButton(myMenuBar,myMDIMenu,myMDIClient,
        FXMDIClient::ID_MDI_MENUWINDOW,LAYOUT_LEFT);
    new FXMDIDeleteButton(myMenuBar,myMDIClient,
        FXMDIClient::ID_MDI_MENUCLOSE,FRAME_RAISED|LAYOUT_RIGHT);
    new FXMDIRestoreButton(myMenuBar,myMDIClient,
        FXMDIClient::ID_MDI_MENURESTORE,FRAME_RAISED|LAYOUT_RIGHT);
    new FXMDIMinimizeButton(myMenuBar,myMDIClient,
        FXMDIClient::ID_MDI_MENUMINIMIZE,FRAME_RAISED|LAYOUT_RIGHT);

    // build the message window
    myMessageWindow = new GUIMessageWindow(myMainSplitter);
    // fill menu and tool bar
    fillMenuBar();
    // build additional threads
    myLoadThread = threadFactory.buildLoadThread(this, myEvents, myLoadThreadEvent);
    myRunThread = threadFactory.buildRunThread(this, *mySimDelayTarget, myEvents,
        myRunThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");
    /*
    myProgressBar =
        new FXProgressBar(myStatusbar, 0, 0, PROGRESSBAR_NORMAL|LAYOUT_FILL_X, 200);
*/
    // set the caption
    string caption = string("SUMO ") + string(version)
        + string(" - no simulation loaded");
    setTitle(caption.c_str());

    // start the simulation-thread
    //  (it will loop until the application ends deciding by itself whether
    //        to perform a step or not)
    myRunThread->start();
    //}
    setIcon( GUIIconSubSys::getIcon(ICON_APP) );
}


void
GUIApplicationWindow::create()
{
    if(getApp()->reg().readIntEntry("SETTINGS","maximized", 0)==0) {
        setX(getApp()->reg().readIntEntry("SETTINGS","x",150));
        setY(getApp()->reg().readIntEntry("SETTINGS","y",150));
        setWidth(getApp()->reg().readIntEntry("SETTINGS","width",600));
        setHeight(getApp()->reg().readIntEntry("SETTINGS","height",400));
    }
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS","basedir", "");
    FXMainWindow::create();
    myMenuBarDrag->create();
    myToolBarDrag1->create();
    myToolBarDrag2->create();
    myFileMenu->create();
    myEditMenu->create();
    mySettingsMenu->create();
    myWindowsMenu->create();
    myHelpMenu->create();
    show(PLACEMENT_SCREEN);
    if(getApp()->reg().readIntEntry("SETTINGS","maximized", 0)==1) {
        maximize();
    }
    // recheck the maximum sizes

//!!!!    FXWindow *root = getApp()->getRootWindow();
//!!!!    myGLWidth = myGLWidth < root->getWidth() ? myGLWidth : root->getWidth();
//!!!!    myGLHeight = myGLHeight < root->getHeight() ? myGLHeight : root->getHeight();
}





GUIApplicationWindow::~GUIApplicationWindow()
{
    myRunThread->prepareDestruction();
    closeAllWindows();
    //
    GUIIconSubSys::close();
    GUITexturesHelper::close();
    delete myGLVisual;
    // delete some non-parented windows
    delete myToolBarDrag1;
    //
//!!!!    myRunThread->yield();
    delete myRunThread;
    delete myFileMenu;
    delete myEditMenu;
    delete mySettingsMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

    delete myLoadThread;

    delete gGradients;
}


void
GUIApplicationWindow::detach()
{
    FXMainWindow::detach();
    myMenuBarDrag->detach();
    myToolBarDrag1->detach();
}


void
GUIApplicationWindow::fillMenuBar()
{
    // build file menu
    myFileMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&File",NULL,myFileMenu);
    new FXMenuCommand(myFileMenu,
        "&Open Simulation...\tCtl-O\tOpen a Simulation (Configuration File).",
        GUIIconSubSys::getIcon(ICON_OPEN),this,MID_OPEN);
    new FXMenuCommand(myFileMenu,
        "&Reload Simulation\tCtl-R\tReloads the Simulation (Configuration File).",
        GUIIconSubSys::getIcon(ICON_RELOAD),this,MID_RELOAD);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
        "&Close\tCtl-C\tClose the Simulation.",
        GUIIconSubSys::getIcon(ICON_CLOSE),this,MID_CLOSE);
        // Recent files
    FXMenuSeparator* sep1=new FXMenuSeparator(myFileMenu);
    sep1->setTarget(&myRecentFiles);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_1);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_2);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_3);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_4);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_5);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_6);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_7);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_8);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_9);
    new FXMenuCommand(myFileMenu,NULL,NULL,&myRecentFiles,FXRecentFiles::ID_FILE_10);
    new FXMenuCommand(myFileMenu,"&Clear Recent Files",NULL,&myRecentFiles,FXRecentFiles::ID_CLEAR);
    myRecentFiles.setTarget(this);
    myRecentFiles.setSelector(MID_RECENTFILE);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
        "&Quit\tCtl-Q\tQuit the Application.",
        0, this, MID_QUIT, 0);

    // build edit menu
    myEditMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Edit",NULL,myEditMenu);
    new FXMenuCommand(myEditMenu,
        "Edit Chosen...\t\tOpens a Dialog for editing the List of chosen Items.",
        GUIIconSubSys::getIcon(ICON_FLAG), this, MID_EDITCHOSEN);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
        "Edit Additional Weights...\t\tOpens a Dialog for editing additional Weights.",
        0, this, MID_EDIT_ADD_WEIGHTS);
    new FXMenuSeparator(myEditMenu);
    new FXMenuCommand(myEditMenu,
        "Edit Breakpoints...\t\tOpens a Dialog for editing breakpoints.",
        0, this, MID_EDIT_BREAKPOINTS);

    // build settings menu
    mySettingsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Settings",NULL,mySettingsMenu);
    new FXMenuCommand(mySettingsMenu,
        "Application Settings...\t\tOpen a Dialog for Application Settings editing.",
        NULL,this,MID_APPSETTINGS);
    new FXMenuCommand(mySettingsMenu,
        "Simulation Settings...\t\tOpen a Dialog for Simulation Settings editing.",
        NULL,this,MID_SIMSETTINGS);

    // build windows menu
    myWindowsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Windows",NULL,myWindowsMenu);
    new FXMenuCheck(myWindowsMenu,
        "Show Status Line\t\tToggle this Status Bar on/off.",
        myStatusbar,FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
        "Show Message Window\t\tToggle the Message Window on/off.",
        myMessageWindow,FXWindow::ID_TOGGLESHOWN);
    /*
    new FXMenuCheck(myWindowsMenu,
        "Show Toolbar\t\tToggle the Toolbar on/off.",
        myToolBar1, FXWindow::ID_TOGGLESHOWN);
        */
    new FXMenuCheck(myWindowsMenu,
        "Show Simulation Time\t\tToggle the Simulation Time on/off.",
        myToolBar3, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
        "Show Simulation Delay\t\tToggle the Simulation Delay Entry on/off.",
        myToolBar4, FXWindow::ID_TOGGLESHOWN);
    addToWindowsMenu(myWindowsMenu);
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
    new FXMenuCommand(myWindowsMenu,"&Close",NULL,
        myMDIClient,FXMDIClient::ID_MDI_CLOSE);
    sep1=new FXMenuSeparator(myWindowsMenu);
    sep1->setTarget(myMDIClient);
    sep1->setSelector(FXMDIClient::ID_MDI_ANY);
    new FXMenuCommand(myWindowsMenu,NULL,NULL,
        myMDIClient,FXMDIClient::ID_MDI_1);
    new FXMenuCommand(myWindowsMenu,NULL,NULL,
        myMDIClient,FXMDIClient::ID_MDI_2);
    new FXMenuCommand(myWindowsMenu,NULL,NULL,
        myMDIClient,FXMDIClient::ID_MDI_3);
    new FXMenuCommand(myWindowsMenu,NULL,NULL,
        myMDIClient,FXMDIClient::ID_MDI_4);
    new FXMenuCommand(myWindowsMenu,"&Others...",NULL,
        myMDIClient,FXMDIClient::ID_MDI_OVER_5);
    new FXMenuSeparator(myWindowsMenu);
    new FXMenuCommand(myWindowsMenu,
        "Clear Message Window\t\tClear the message window.",
        0, this, MID_CLEARMESSAGEWINDOW);

    // build help menu
    myHelpMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Help",NULL,myHelpMenu);
    new FXMenuCommand(myHelpMenu,"&About", GUIIconSubSys::getIcon(ICON_APP),
        this, MID_ABOUT);
}


void
GUIApplicationWindow::buildToolBars()
{
    // build tool bars
    {
        // file and simulation tool bar
        myToolBarDrag1=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar1 = new FXToolBar(myTopDock,myToolBarDrag1,
            LAYOUT_DOCK_NEXT|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar1, myToolBar1, FXToolBar::ID_TOOLBARGRIP,
            TOOLBARGRIP_DOUBLE);
        // build file tools
        new FXButton(myToolBar1,"\t\tOpen a Simulation (Configuration File).",
            GUIIconSubSys::getIcon(ICON_OPEN), this, MID_OPEN,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar1,"\t\tReload the Simulation (Configuration File).",
            GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
    {
        // build simulation tools
        myToolBarDrag2=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar2 = new FXToolBar(myTopDock,myToolBarDrag2,
            LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar2, myToolBar2, FXToolBar::ID_TOOLBARGRIP,
            TOOLBARGRIP_DOUBLE);
        new FXButton(myToolBar2,"\t\tStart the loaded Simulation.",
            GUIIconSubSys::getIcon(ICON_START), this, MID_START,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar2,"\t\tStop the running Simulation.",
            GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar2,"\t\tPerform a single Simulation Step.",
            GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
/*
        new FXButton(myToolBar2,"\t\tCompute strategies.",
            GUIIconSubSys::getIcon(ICON_CUT_SWELL), this, MID_CUTSWELL,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
*/

    }
    {
        // Simulation Step Display
        myToolBarDrag3=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar3 = new FXToolBar(myTopDock,myToolBarDrag3,
            LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar3, myToolBar3, FXToolBar::ID_TOOLBARGRIP,
            TOOLBARGRIP_DOUBLE);
        new FXLabel(myToolBar3, "Current Step:", 0, LAYOUT_CENTER_X);
        myLCDLabel = new FXLCDLabel(myToolBar3, 6, 0, 0,
            LCDLABEL_LEADING_ZEROS);
        myLCDLabel->setHorizontal(2);
        myLCDLabel->setVertical(2);
        myLCDLabel->setThickness(2);
        myLCDLabel->setGroove(2);
        myLCDLabel->setText("-----------");
    }
    {
        // Simulation Delay
        myToolBarDrag4=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar4 = new FXToolBar(myTopDock,myToolBarDrag4,
            LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar4, myToolBar4, FXToolBar::ID_TOOLBARGRIP,
            TOOLBARGRIP_DOUBLE);
        new FXLabel(myToolBar4, "Delay:", 0, LAYOUT_CENTER_X);
        mySimDelayTarget =
            new FXRealSpinDial(myToolBar4, 10, 0, MID_SIMDELAY,
            LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        mySimDelayTarget->setFormatString("%.0fms");
        mySimDelayTarget->setIncrements(1,10,10);
        mySimDelayTarget->setRange(0,1000);
        mySimDelayTarget->setValue(0);
    }
    {
        // Views
        myToolBarDrag5=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar5 = new FXToolBar(myTopDock,myToolBarDrag5,
            LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar5, myToolBar5, FXToolBar::ID_TOOLBARGRIP,
            TOOLBARGRIP_DOUBLE);
        // build view tools
        new FXButton(myToolBar5,"\t\tOpen a new microscopic View.",
            GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_MICROVIEW,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar5,
            "\t\tOpen a new lane aggregated View.",
            GUIIconSubSys::getIcon(ICON_LAGGRVIEW), this, MID_NEW_LANEAVIEW,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
}


long
GUIApplicationWindow::onCmdQuit(FXObject*,FXSelector,void*)
{
    getApp()->reg().writeIntEntry("SETTINGS","x",getX());
    getApp()->reg().writeIntEntry("SETTINGS","y",getY());
    getApp()->reg().writeIntEntry("SETTINGS","width",getWidth());
    getApp()->reg().writeIntEntry("SETTINGS","height",getHeight());
    getApp()->reg().writeStringEntry("SETTINGS","basedir", gCurrentFolder.c_str());
    if(isMaximized()) {
        getApp()->reg().writeIntEntry("SETTINGS","maximized", 1);
    } else {
        getApp()->reg().writeIntEntry("SETTINGS","maximized", 0);
    }
    getApp()->exit(0);
    return 1;
}


long
GUIApplicationWindow::onCmdEditChosen(FXObject*,FXSelector,void*)
{
    GUIDialog_GLChosenEditor *chooser =
        new GUIDialog_GLChosenEditor(this, &gSelected);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdEditBreakpoints(FXObject*,FXSelector,void*)
{
    GUIDialog_Breakpoints *chooser =
        new GUIDialog_Breakpoints(this);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdEditAddWeights(FXObject*,FXSelector,void*)
{
    GUIDialog_EditAddWeights *chooser =
        new GUIDialog_EditAddWeights(this);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUIApplicationWindow::onCmdOpen(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Open Simulation Configuration");
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(myConfigPattern.c_str());
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        load(file);
        myRecentFiles.appendFile(file.c_str());
    }
    return 1;
}


long
GUIApplicationWindow::onCmdReload(FXObject*,FXSelector,void*)
{
    load(myLoadThread->getFileName());
    return 1;
}


long
GUIApplicationWindow::onCmdOpenRecent(FXObject*,FXSelector,void *data)
{
    string file = string((const char*)data);
    load(file);
    return 1;
}


long
GUIApplicationWindow::onCmdClose(FXObject*,FXSelector,void*)
{
    closeAllWindows();
    return 1;
}


long
GUIApplicationWindow::onUpdOpen(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myAmLoading?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdReload(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myAmLoading||myLoadThread->getFileName()==""
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdAddMicro(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myAmLoading||!myRunThread->simulationAvailable()
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdStart(FXObject*,FXSelector,void*)
{
    // check whether a net was loaded successfully
    if(!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 1;
    }
    // check whether it was started before and pasued;
    //  when yes, prompt the user for acknowledge
    if(_wasStarted) {
        myRunThread->resume();
        return 1;
    }
    _wasStarted = true;
    myRunThread->begin();
    return 1;
}


long
GUIApplicationWindow::onCmdStop(FXObject*,FXSelector,void*)
{
    myRunThread->stop();
    return 1;
}


long
GUIApplicationWindow::onCmdStep(FXObject*,FXSelector,void*)
{
    myRunThread->singleStep();
    return 1;
}


long
GUIApplicationWindow::onCmdClearMsgWindow(FXObject*,FXSelector,void*)
{
    myMessageWindow->clear();
    return 1;
}


long
GUIApplicationWindow::onUpdAddALane(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myAmLoading||!myRunThread->simulationAvailable()||!gAllowAggregated
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStart(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        !myRunThread->simulationIsStartable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStop(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        !myRunThread->simulationIsStopable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdStep(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        !myRunThread->simulationIsStepable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdSimSettings(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        !myRunThread->simulationAvailable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdEditChosen(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        !myRunThread->simulationAvailable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdEditAddWeights(FXObject *sender,FXSelector,void *ptr)
{
    sender->handle(this,
        !myRunThread->simulationAvailable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onUpdEditBreakpoints(FXObject *sender,FXSelector,void *ptr)
{
    sender->handle(this,
        !myRunThread->simulationAvailable()||myAmLoading
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    return 1;
}


long
GUIApplicationWindow::onCmdAppSettings(FXObject*,FXSelector,void*)
{
    GUIDialog_AppSettings *d = new GUIDialog_AppSettings(this);
    d->create();
    d->show(PLACEMENT_OWNER);
    return 1;
}


long
GUIApplicationWindow::onCmdSimSettings(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GUIApplicationWindow::onCmdNewMicro(FXObject*,FXSelector,void*)
{
    openNewView(GUISUMOViewParent::MICROSCOPIC_VIEW);
    return 1;
}


long
GUIApplicationWindow::onCmdNewLaneA(FXObject*,FXSelector,void*)
{
    openNewView(GUISUMOViewParent::LANE_AGGREGATED_VIEW);
    return 1;
}


long
GUIApplicationWindow::onCmdAbout(FXObject*,FXSelector,void*)
{
    GUIDialog_AboutSUMO *about =
        new GUIDialog_AboutSUMO(this, "About SUMO", 0, 0);
    about->create();
    about->show(PLACEMENT_OWNER);
    return 1;
}


long
GUIApplicationWindow::onLoadThreadEvent(FXObject*, FXSelector, void*)
{
    eventOccured();
    return 1;
}


long
GUIApplicationWindow::onRunThreadEvent(FXObject*, FXSelector, void*)
{
    eventOccured();
    return 1;
}


void
GUIApplicationWindow::eventOccured()
{
    while(!myEvents.empty()) {
        // get the next event
        GUIEvent *e = static_cast<GUIEvent*>(myEvents.top());
        myEvents.pop();
        // process
        switch(e->getOwnType()) {
        case EVENT_SIMULATION_LOADED:
            handleEvent_SimulationLoaded(e);
            break;
        case EVENT_SIMULATION_STEP:
            handleEvent_SimulationStep(e);
            break;
        case EVENT_MESSAGE_OCCURED:
        case EVENT_WARNING_OCCURED:
        case EVENT_ERROR_OCCURED:
            handleEvent_Message(e);
            break;
        case EVENT_SIMULATION_ENDED:
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
GUIApplicationWindow::handleEvent_SimulationLoaded(GUIEvent *e)
{
    GUITexturesHelper::init(getApp());
    myAmLoading = false;
    GUIEvent_SimulationLoaded *ec =
        static_cast<GUIEvent_SimulationLoaded*>(e);
    // check whether the loading was successfull
    if(ec->_net==0) {
        // report failure
        string text = string("Loading of '") + ec->_file + string("' failed!");
        myStatusbar->getStatusLine()->setText(text.c_str());
        myStatusbar->getStatusLine()->setNormalText(text.c_str());
    } else {
        // initialise global information
        gSimInfo = new GUISimInfo(*(ec->_net));
        gNetWrapper = ec->_net->getWrapper();
        // report success
        string text = string("'") + ec->_file + string("' loaded.");
        myStatusbar->getStatusLine()->setText(text.c_str());
        myStatusbar->getStatusLine()->setNormalText(text.c_str());
        // initialise simulation thread
        myRunThread->init(ec->_net, ec->_begin, ec->_end);
        _wasStarted = false;
        // initialise views
        myViewNumber = 0;
        openNewView(GUISUMOViewParent::MICROSCOPIC_VIEW);
        // set simulation name on the caption
        string caption = string("SUMO ") + string(version)
            + string(" - ") + ec->_file;
        setTitle( caption.c_str());
        ostringstream str;
        // set simulation step begin information
        str << myRunThread->getCurrentTimeStep();
        myLCDLabel->setText(str.str().c_str());
    }
    getApp()->endWaitCursor();
    // start if wished
    if(myRunAtBegin&&ec->_net!=0&&myRunThread->simulationIsStartable()) {
        onCmdStart(0, 0, 0);
    }
    update();
}


void
GUIApplicationWindow::handleEvent_SimulationStep(GUIEvent *e)
{
    updateChildren();
    ostringstream str;
    str << myRunThread->getCurrentTimeStep();
    myLCDLabel->setText(str.str().c_str());
    update();
}


void
GUIApplicationWindow::handleEvent_Message(GUIEvent *e)
{
    GUIEvent_Message *ec =
        static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendText(ec->getOwnType(), ec->getMsg());
}


void
GUIApplicationWindow::handleEvent_SimulationEnded(GUIEvent *e)
{
    GUIEvent_SimulationEnded *ec =
        static_cast<GUIEvent_SimulationEnded*>(e);
    if(!gSuppressEndInfo) {
        // build the text
        stringstream text;
        text << "The simulation has ended at time step "
            << ec->getTimeStep() << "." << endl;
        switch(ec->getReason()) {
        case GUIEvent_SimulationEnded::ER_NO_VEHICLES:
            text << "Reason: All vehicles have left the simulation.";
            break;
        case GUIEvent_SimulationEnded::ER_END_STEP_REACHED:
            text << "Reason: The final simulation step has been reached.";
            break;
        case GUIEvent_SimulationEnded::ER_ERROR_IN_SIM:
            text << "Reason: An error occured (see log).";
            break;
        case GUIEvent_SimulationEnded::ER_FORCED:
            gQuitOnEnd = true;
            break;
        default:
            throw 1;
        }
        //
        onCmdStop(0, 0, 0);
        string tstr = text.str();
        if(ec->getReason()!=GUIEvent_SimulationEnded::ER_FORCED) {
            FXMessageBox::warning(this, MBOX_OK, "Simulation Ended",
                tstr.c_str());
        }
    } else {
        onCmdStop(0, 0, 0);
    }
    if(gQuitOnEnd) {
        closeAllWindows();
        getApp()->exit(0);
    }
}



void
GUIApplicationWindow::load(const std::string &file)
{
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    myLoadThread->load(file);
    string text = string("Loading '") + file + string("'.");
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
    update();
}



void
GUIApplicationWindow::openNewView(GUISUMOViewParent::ViewType type)
{
    if(!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return;
    }
    string caption = string("View #") + toString(myViewNumber++);
    FXuint opts = MDI_TRACKING;
    GUISUMOViewParent* w = 0;
    if(myMDIClient->numChildren()==0) {
        w = new GUISUMOViewParent( myMDIClient, 0,
            myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
            this, type, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
        w->init(type, 0, myRunThread->getNet());
    } else {
        w = new GUISUMOViewParent( myMDIClient, getBuildGLCanvas(),
            myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
            this, type, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
        w->init(type, getBuildGLCanvas(), myRunThread->getNet());
    }
    w->create();
    if(myMDIClient->numChildren()==1) {
        w->maximize();
    } else {
        myMDIClient->vertical(true);
    }
    myMDIClient->setActiveChild(w);
}


FXGLCanvas *
GUIApplicationWindow::getBuildGLCanvas() const
{
    if(myMDIClient->numChildren()==0) {
        return 0;
    }
    GUISUMOViewParent *share_tmp1 =
        static_cast<GUISUMOViewParent*>(myMDIClient->childAtIndex(0));
    return share_tmp1->getBuildGLCanvas();
}


void
GUIApplicationWindow::closeAllWindows()
{
    myTrackerLock.lock();
    myLCDLabel->setText("-----------");
    // remove trackers and other external windows
    size_t i;
    for(i=0; i<mySubWindows.size(); i++) {
        mySubWindows[i]->destroy();
    }
    for(i=0; i<myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->destroy();
    }
    // delete the simulation
    myRunThread->deleteSim();
    delete gSimInfo;
    gSimInfo = 0;
    // reset the caption
    string caption = string("SUMO ") + string(version)
        + string(" - no simulation loaded");
    setTitle( caption.c_str());
    // delete other children
    while(mySubWindows.size()!=0) {
        delete mySubWindows[0];
    }
    while(myTrackerWindows.size()!=0) {
        delete myTrackerWindows[0];
    }
    mySubWindows.clear();
    // add a separator to the log
    myMessageWindow->addSeparator();
    myTrackerLock.unlock();
    //
    update();
}


FXCursor *
GUIApplicationWindow::getDefaultCursor()
{
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


void
GUIApplicationWindow::addTimeout(FXObject *tgt, FXSelector sel,
                                 FXuint ms, void *ptr)
{
    getApp()->addTimeout(tgt, sel, ms, ptr);
}


void
GUIApplicationWindow::removeTimeout(FXObject *tgt, FXSelector sel)
{
    getApp()->removeTimeout(tgt, sel);
}


size_t
GUIApplicationWindow::getCurrentSimTime() const
{
    return myRunThread->getCurrentTimeStep();
}


void
GUIApplicationWindow::loadSelection(const std::string &file) const
{
    GUISelectionLoader::loadSelection(file);
}


void
GUIApplicationWindow::loadOnStartup(const std::string &config, bool run)
{
    myRunAtBegin = run;
    load(config);
}
/*
long
GUIApplicationWindow::onCmdCutSwell(FXObject*, FXSelector, void*)
{
    /*
    GUIDialog_CutSwell *about =
        new GUIDialog_CutSwell(this, "Simulating...", 0, 0);
    about->create();
    about->show(PLACEMENT_OWNER);
    /
    string prev = myStatusbar->getStatusLine()->getText().text();
    string text = string("Computing strategies.");
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
    for(int i=0; i<100; i++) {
        fxsleep(3000);
        myProgressBar->setProgress(i);
    }
//    delete about;
    myProgressBar->setProgress(0);
    myStatusbar->getStatusLine()->setText(prev.c_str());
    myStatusbar->getStatusLine()->setNormalText(prev.c_str());
    return 1;
}
*/
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


