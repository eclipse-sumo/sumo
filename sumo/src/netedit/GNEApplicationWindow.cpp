/****************************************************************************/
/// @file    GNEApplicationWindow.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 22. Nov 2004
/// @version $Id$
///
// Class for the main gui window
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <fx3d.h>
#include <string>
#include <sstream>
#include <algorithm>

#include <guisim/GUINet.h>

#include <gui/GUILoadThread.h>
#include <gui/GUIRunThread.h>
#include "GNEApplicationWindow.h"
#include <gui/GUISUMOViewParent.h>
#include <utils/common/ToString.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include <gui/GUIEvent_SimulationLoaded.h>
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <gui/dialogs/GUIDialog_AboutSUMO.h>
#include <gui/dialogs/GUIDialog_AppSettings.h>
#include <gui/dialogs/GUIDialog_SimSettings.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <gui/dialogs/GUIDialog_EditAddWeights.h>
#include <gui/dialogs/GUIDialog_Breakpoints.h>
#include <gui/GUIThreadFactory.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <guisim/GUINetWrapper.h>
#include "GNEViewParent.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/foxtools/MFXUtils.h>

#include <netbuild/NBEdge.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>

#include "Image.h"
#include "GNEImageProcWindow.h"
#include <microsim/MSGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace FXEX;


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXDEFMAP(GNEApplicationWindow) GNEApplicationWindowMap[]=
    {
        //________Message_Type____________ID________________________Message_Handler________
        FXMAPFUNC(SEL_COMMAND,  MID_QUIT,        GNEApplicationWindow::onCmdQuit),
        FXMAPFUNC(SEL_SIGNAL,   MID_QUIT,        GNEApplicationWindow::onCmdQuit),
        FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,      GNEApplicationWindow::onCmdQuit),

        FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,       GNEApplicationWindow::onCmdOpen),
        FXMAPFUNC(SEL_COMMAND,  MID_IMPORT_NET,        GNEApplicationWindow::onCmdImportNet),
        FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,        GNEApplicationWindow::onCmdOpenRecent),
        FXMAPFUNC(SEL_COMMAND,  MID_RELOAD,            GNEApplicationWindow::onCmdReload),
        FXMAPFUNC(SEL_COMMAND,  MID_CLOSE,             GNEApplicationWindow::onCmdClose),
        FXMAPFUNC(SEL_COMMAND,  MID_LOAD_IMAGE,        GNEApplicationWindow::onCmdLoadImage),
        FXMAPFUNC(SEL_COMMAND,  MID_SAVE_IMAGE,        GNEApplicationWindow::onCmdSaveImage),
        FXMAPFUNC(SEL_COMMAND,  MID_SAVE_EDGES_NODES,  GNEApplicationWindow::onCmdSaveEdgesNodes),
        FXMAPFUNC(SEL_COMMAND,  MID_SAVE_NET,          GNEApplicationWindow::onCmdSaveNet),

        FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,       GNEApplicationWindow::onUpdOpen),
        FXMAPFUNC(SEL_UPDATE,   MID_RELOAD,            GNEApplicationWindow::onUpdReload),
        FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,        GNEApplicationWindow::onUpdOpenRecent),
        FXMAPFUNC(SEL_UPDATE,   MID_SAVE_IMAGE,        GNEApplicationWindow::onUpdSaveImage),
        FXMAPFUNC(SEL_UPDATE,   MID_SAVE_EDGES_NODES,  GNEApplicationWindow::onUpdSaveEdgesNodes),
        FXMAPFUNC(SEL_UPDATE,   MID_SAVE_NET,          GNEApplicationWindow::onUpdSaveNet),



        FXMAPFUNC(SEL_COMMAND,  MID_EDITCHOSEN,        GNEApplicationWindow::onCmdEditChosen),
        FXMAPFUNC(SEL_COMMAND,  MID_EDIT_BREAKPOINTS,  GNEApplicationWindow::onCmdEditBreakpoints),

        FXMAPFUNCS(SEL_UPDATE,  MID_EXTRACT_STREETS, MID_OPEN_BMP_DIALOG, GNEApplicationWindow::onUpdPictureMenu),
        FXMAPFUNCS(SEL_COMMAND,  MID_EXTRACT_STREETS, MID_OPEN_BMP_DIALOG, GNEApplicationWindow::onCmdPictureMenu),
        FXMAPFUNCS(SEL_UPDATE,  MID_SHOW_GRAPH_ON_EMPTY_BITMAP, MID_EXPORT_EDGES_XML, GNEApplicationWindow::onUpdGraphMenu),
        FXMAPFUNCS(SEL_COMMAND,  MID_SHOW_GRAPH_ON_EMPTY_BITMAP, MID_EXPORT_EDGES_XML, GNEApplicationWindow::onCmdGraphMenu),

        FXMAPFUNC(SEL_COMMAND,           ID_CLEAR,    GNEApplicationWindow::onCmdClear),
        FXMAPFUNC(SEL_UPDATE,            ID_CLEAR,    GNEApplicationWindow::onUpdClear),

        FXMAPFUNC(SEL_COMMAND,  MID_APPSETTINGS,   GNEApplicationWindow::onCmdAppSettings),
        FXMAPFUNC(SEL_COMMAND,  MID_SIMSETTINGS,   GNEApplicationWindow::onCmdSimSettings),
        FXMAPFUNC(SEL_COMMAND,  MID_ABOUT,         GNEApplicationWindow::onCmdAbout),
        FXMAPFUNC(SEL_COMMAND,  MID_NEW_MICROVIEW, GNEApplicationWindow::onCmdNewMicro),
        FXMAPFUNC(SEL_COMMAND,  MID_START,         GNEApplicationWindow::onCmdStart),
        FXMAPFUNC(SEL_COMMAND,  MID_STOP,          GNEApplicationWindow::onCmdStop),
        FXMAPFUNC(SEL_COMMAND,  MID_STEP,          GNEApplicationWindow::onCmdStep),



        FXMAPFUNC(SEL_UPDATE,   MID_NEW_MICROVIEW,     GNEApplicationWindow::onUpdAddMicro),
        FXMAPFUNC(SEL_UPDATE,   MID_START,             GNEApplicationWindow::onUpdStart),
        FXMAPFUNC(SEL_UPDATE,   MID_STOP,              GNEApplicationWindow::onUpdStop),
        FXMAPFUNC(SEL_UPDATE,   MID_STEP,              GNEApplicationWindow::onUpdStep),
        FXMAPFUNC(SEL_UPDATE,   MID_EDITCHOSEN,        GNEApplicationWindow::onUpdEditChosen),
        FXMAPFUNC(SEL_UPDATE,   MID_EDIT_ADD_WEIGHTS,  GNEApplicationWindow::onUpdEditAddWeights),
        FXMAPFUNC(SEL_UPDATE,   MID_EDIT_BREAKPOINTS,  GNEApplicationWindow::onUpdEditBreakpoints),
        FXMAPFUNC(SEL_UPDATE,   MID_SIMSETTINGS,       GNEApplicationWindow::onUpdSimSettings),

        FXMAPFUNC(SEL_THREAD_EVENT, ID_LOADTHREAD_EVENT, GNEApplicationWindow::onLoadThreadEvent),
        FXMAPFUNC(SEL_THREAD_EVENT, ID_RUNTHREAD_EVENT,  GNEApplicationWindow::onRunThreadEvent),
        FXMAPFUNC(SEL_THREAD, ID_LOADTHREAD_EVENT,       GNEApplicationWindow::onLoadThreadEvent),
        FXMAPFUNC(SEL_THREAD, ID_RUNTHREAD_EVENT,        GNEApplicationWindow::onRunThreadEvent),
    };

// Object implementation
FXIMPLEMENT(GNEApplicationWindow, FXMainWindow, GNEApplicationWindowMap, ARRAYNUMBER(GNEApplicationWindowMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GNEApplicationWindow::GNEApplicationWindow(FXApp* a,
        GUIThreadFactory &threadFactory,
        int glWidth, int glHeight,
        const std::string &config)
        : GUIMainWindow(a, glWidth, glHeight),
        myLoadThread(0), myRunThread(0),
        myAmLoading(false),
        mySimDelay(50)
{
    setTarget(this);
    setSelector(MID_WINDOW);
    GUIIconSubSys::init(a);
    GUITexturesHelper::init(getApp());
    // build menu bar
    myMenuBarDrag=new FXToolBarShell(this,FRAME_NORMAL);
    myMenuBar = new FXMenuBar(this, myMenuBarDrag,
                              LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myMenuBar, myMenuBar, FXMenuBar::ID_TOOLBARGRIP,
                      TOOLBARGRIP_DOUBLE);
    // build tool bars
    buildToolBars();

    // build the thread - io
    myLoadThreadEvent.setTarget(this),
    myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    myRunThreadEvent.setTarget(this),
    myRunThreadEvent.setSelector(ID_RUNTHREAD_EVENT);

    // build the status bar
    myStatusbar = new FXStatusBar(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_RAISED);

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

    // set the caption
    string caption = "SUMO " + string(VERSION) + " - no simulation loaded";
    setTitle(caption.c_str());

    // start the simulation-thread
    //  (it will loop until the application ends deciding by itself whether
    //        to perform a step or not)
    myRunThread->start();
    setIcon(GUIIconSubSys::getIcon(ICON_APP));
}


void
GNEApplicationWindow::create()
{
    if (getApp()->reg().readIntEntry("SETTINGS","maximized", 0)==0) {
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
    myToolBarDrag3->create();
    myToolBarDrag4->create();
    myToolBarDrag5->create();
    myFileMenu->create();
    myEditMenu->create();
    mySettingsMenu->create();
    myWindowsMenu->create();
    myHelpMenu->create();
    show(PLACEMENT_SCREEN);
    if (getApp()->reg().readIntEntry("SETTINGS","maximized", 0)==1) {
        maximize();
    }
}


GNEApplicationWindow::~GNEApplicationWindow()
{
    myRunThread->prepareDestruction();
    closeAllWindows();
    //
    GUIIconSubSys::close();
    GUITexturesHelper::close();
    // delete some non-parented windows
    delete myToolBarDrag1;
    delete myToolBarDrag2;
    delete myToolBarDrag3;
    delete myToolBarDrag4;
    delete myToolBarDrag5;
    //
    delete myRunThread;
    delete myFileMenu;
    delete myEditMenu;
    delete myImageMenu;
    delete myGraphMenu;
    delete mySettingsMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

    delete gGradients;
}


void
GNEApplicationWindow::detach()
{
    FXMainWindow::detach();
    myMenuBarDrag->detach();
    myToolBarDrag1->detach();
    myToolBarDrag2->detach();
    myToolBarDrag3->detach();
    myToolBarDrag4->detach();
    myToolBarDrag5->detach();
}


void
GNEApplicationWindow::fillMenuBar()
{
    // build file menu
    myFileMenu = new FXMenuPane(this);
    //mySubFileMenu1 = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&File",NULL,myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Open Simulation...\tCtl-O\tOpen a simulation (Configuration file).",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG),this,MID_OPEN_CONFIG);
    new FXMenuCommand(myFileMenu,
                      "&Reload\tCtl-R\tReloads the simulation / the network.",
                      GUIIconSubSys::getIcon(ICON_RELOAD),this,MID_RELOAD);
    new FXMenuSeparator(myFileMenu);
    new FXMenuCommand(myFileMenu,
                      "&Close\tCtl-C\tClose the simulation.",
                      GUIIconSubSys::getIcon(ICON_CLOSE),this,MID_CLOSE);
    new FXMenuSeparator(myFileMenu);
//    new FXMenuCascade(myFileMenu,"Import",NULL,mySubFileMenu1);
    new FXMenuCommand(myFileMenu,
                      "&Import...\t\tImports from a supported network.",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG),this,MID_IMPORT_NET);
    new FXMenuCommand(myFileMenu,
                      "&Load Bitmap\t\tOpens a bitmap file.",
                      GUIIconSubSys::getIcon(ICON_OPEN_CONFIG),this,MID_LOAD_IMAGE);
    new FXMenuCommand(myFileMenu,
                      "&Save Bitmap\t\tSaves the current bitmap file.",
                      GUIIconSubSys::getIcon(ICON_CLOSE),this,MID_SAVE_IMAGE);
    new FXMenuCommand(myFileMenu,
                      "&Save Edges and Nodes...\t\tSaves the extracted edges and nodes.",
                      GUIIconSubSys::getIcon(ICON_SAVE),this,MID_SAVE_EDGES_NODES);
    new FXMenuCommand(myFileMenu,
                      "&Save Network...\t\tSaves the build network.",
                      GUIIconSubSys::getIcon(ICON_SAVE),this,MID_SAVE_NET);
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

    // build image menu (Andreas)
    myImageMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Bild",NULL,myImageMenu);
    new FXMenuCommand(myImageMenu,
                      "Extract Streets\t\tExtracts streets (areas with one of the chosen colours).",
                      GUIIconSubSys::getIcon(ICON_EXTRACT),this,MID_EXTRACT_STREETS);
    new FXMenuCommand(myImageMenu,
                      "Dilatation\t\tPerforms a dilatation on the image.",
                      GUIIconSubSys::getIcon(ICON_DILATE),this,MID_DILATION);
    new FXMenuCommand(myImageMenu,
                      "Erosion...\t\tPerforms an erosion on the image.",
                      GUIIconSubSys::getIcon(ICON_ERODE),this,MID_EROSION);
    new FXMenuCommand(myImageMenu,
                      "Open morphologically\t\tOpens morphologically (erode first, then dilate).",
                      GUIIconSubSys::getIcon(ICON_OPENING),this,MID_OPENING);
    new FXMenuCommand(myImageMenu,
                      "Close morphologically\t\tCloses morphologically (dilate first, then close).",
                      GUIIconSubSys::getIcon(ICON_CLOSING),this,MID_CLOSING);
    new FXMenuCommand(myImageMenu,
                      "Close Gaps\t\tMinimizes small white spots in black areas.",
                      GUIIconSubSys::getIcon(ICON_CLOSE_GAPS),this,MID_CLOSE_GAPS);
    new FXMenuCommand(myImageMenu,
                      "Remove Unconnected\t\tErases black ´noise´.",
                      GUIIconSubSys::getIcon(ICON_ERASE_STAINS),this,MID_ERASE_STAINS);
    new FXMenuCommand(myImageMenu,
                      "Skeletonize\t\tCreates a street´s skeleton (thin black lines in the middle of the street.",
                      GUIIconSubSys::getIcon(ICON_SKELETONIZE),this,MID_SKELETONIZE);
    new FXMenuCommand(myImageMenu,
                      "Skelett Maximal Verdünnen\t\tDas Strassenskelett wird maximal verdünnt. Löscht man nach dieser Operation noch Pixel zerfällt das Skelett.",
                      NULL,this,MID_RARIFY);
    new FXMenuSeparator(myImageMenu);
    new FXMenuCommand(myImageMenu,
                      "Generate Graph\t\tCreates a graph from the skeleton.",
                      GUIIconSubSys::getIcon(ICON_CREATE_GRAPH),this,MID_CREATE_GRAPH);

    // build graph menu (Miguel)
    myGraphMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar, "&Graph",NULL,myGraphMenu);
    new FXMenuCommand(myGraphMenu,
                      "Graph anzeigen auf leerem Bitmap...\t\tZeigt den Graphen auf einem leeren Bitmap an.",
                      NULL,this,MID_SHOW_GRAPH_ON_EMPTY_BITMAP);
    new FXMenuCommand(myGraphMenu,
                      "Graph anzeigen auf aktuellem Bitmap...\t\tZeigt den Graphen auf dem aktuellen Bitmap an",
                      NULL,this,MID_SHOW_GRAPH_ON_ACTUAL_BITMAP);
    new FXMenuCommand(myGraphMenu,
                      "Minimiere Knoten...\t\tEntfernt Knoten, deren ausgehende Kanten nahezu kolinear sind.",
                      NULL,this,MID_REDUCE_VERTEXES);
    new FXMenuCommand(myGraphMenu,
                      "Minimiere Knoten (+)...\t\tEntfernt Knoten, deren ausgehende Kanten nahezu kolinear sind,besser.",
                      NULL,this,MID_REDUCE_VERTEXES_PLUS);
    new FXMenuCommand(myGraphMenu,
                      "Minimiere Kanten\t\tEntfernt Kanten, die Schleifen darstellen.",
                      NULL,this,MID_REDUCE_EDGES);
    new FXMenuCommand(myGraphMenu,
                      "Vereine nahe Knoten\t\tVerschmelzt nahe beieinander liegende Knoten.",
                      NULL,this,MID_MERGE_VERTEXES);
    new FXMenuCommand(myGraphMenu,
                      "Exportiere Knotendatei(XML)\t\tExportiert das Knotenarray in eine XML-Datei in SUMO-Norm.",
                      NULL,this,MID_EXPORT_VERTEXES_XML);
    new FXMenuCommand(myGraphMenu,
                      "Exportiere Kantendatei(XML)\t\tExportiert das Kantenarray in eine XML-Datei in SUMO-Norm.",
                      NULL,this,MID_EXPORT_EDGES_XML);


    // build windows menu
    myWindowsMenu = new FXMenuPane(this);
    new FXMenuTitle(myMenuBar,"&Windows",NULL,myWindowsMenu);
    new FXMenuCheck(myWindowsMenu,
                    "Show Status Line\t\tToggle this Status Bar on/off.",
                    myStatusbar,FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Message Window\t\tToggle the Message Window on/off.",
                    myMessageWindow,FXWindow::ID_TOGGLESHOWN);
    /*    new FXMenuCheck(myWindowsMenu,
            "Show Simulation Toolbar\t\tToggle the Toolbar on/off.",
            myToolBar, FXWindow::ID_TOGGLESHOWN);*/
    new FXMenuCheck(myWindowsMenu,
                    "Show Simulation Time\t\tToggle the Simulation Time on/off.",
                    myToolBar3, FXWindow::ID_TOGGLESHOWN);
    new FXMenuCheck(myWindowsMenu,
                    "Show Simulation Delay\t\tToggle the Simulation Delay Entry on/off.",
                    myToolBar4, FXWindow::ID_TOGGLESHOWN);
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
GNEApplicationWindow::buildToolBars()
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
        new FXButton(myToolBar1,"\t\tOpen a simulation (Configuration file).",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_OPEN_CONFIG,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar1,"\t\tReloads the simulation / the network.",
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
        new FXButton(myToolBar2,"\t\tStart the loaded simulation.",
                     GUIIconSubSys::getIcon(ICON_START), this, MID_START,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar2,"\t\tStop the running simulation.",
                     GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(myToolBar2,"\t\tPerform a single simulation step..",
                     GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
    {
        // Simulation Step Display
        myToolBarDrag3=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar3 = new FXToolBar(myTopDock,myToolBarDrag3,
                                   LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED);
        new FXToolBarGrip(myToolBar3, myToolBar3, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXLabel(myToolBar3, "Next Step:", 0, LAYOUT_CENTER_Y);
        myLCDLabel = new FXLCDLabel(myToolBar3, 6, 0, 0,
                                    LCDLABEL_LEADING_ZEROS);
        myLCDLabel->setHorizontal(2);
        myLCDLabel->setVertical(6);
        myLCDLabel->setThickness(2);
        myLCDLabel->setGroove(2);
        myLCDLabel->setText("-----------");
    }
    {
        // Simulation Delay
        myToolBarDrag4=new FXToolBarShell(this,FRAME_NORMAL);
        myToolBar4 = new FXToolBar(myTopDock,myToolBarDrag4,
                                   LAYOUT_DOCK_SAME|LAYOUT_SIDE_TOP|FRAME_RAISED|LAYOUT_FILL_Y);
        new FXToolBarGrip(myToolBar4, myToolBar4, FXToolBar::ID_TOOLBARGRIP,
                          TOOLBARGRIP_DOUBLE);
        new FXLabel(myToolBar4, "Delay (ms):", 0, LAYOUT_CENTER_Y);
        mySimDelayTarget =
            new FXRealSpinDial(myToolBar4, 7, 0, MID_SIMDELAY,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_Y);
        mySimDelayTarget->setNumberFormat(0);
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
        new FXButton(myToolBar5,"\t\tOpen a new microscopic view.",
                     GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_MICROVIEW,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    }
}

// Handle the clear message
long GNEApplicationWindow::onCmdClear(FXObject*,FXSelector,void*)
{
    /*
      FXDCWindow dc(canvas);
    dc.setForeground(canvas->getBackColor());
    dc.fillRectangle(0,0,canvas->getWidth(),canvas->getHeight());
    dirty=0;
    */
    return 1;
}

long GNEApplicationWindow::onUpdClear(FXObject* sender,FXSelector,void*)
{
    /*!!!
      if(dirty)
        sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
      else
        sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);
    */
    return 1;
}


long
GNEApplicationWindow::onCmdQuit(FXObject*,FXSelector,void*)
{
    getApp()->reg().writeIntEntry("SETTINGS","x",getX());
    getApp()->reg().writeIntEntry("SETTINGS","y",getY());
    getApp()->reg().writeIntEntry("SETTINGS","width",getWidth());
    getApp()->reg().writeIntEntry("SETTINGS","height",getHeight());
    getApp()->reg().writeStringEntry("SETTINGS","basedir", gCurrentFolder.c_str());
    if (isMaximized()) {
        getApp()->reg().writeIntEntry("SETTINGS","maximized", 1);
    } else {
        getApp()->reg().writeIntEntry("SETTINGS","maximized", 0);
    }
    getApp()->exit(0);
    return 1;
}

long
GNEApplicationWindow::onCmdEditChosen(FXObject*,FXSelector,void*)
{
    GUIDialog_GLChosenEditor *chooser =
        new GUIDialog_GLChosenEditor(this, &gSelected);
    chooser->create();
    chooser->show();
    return 1;
}

long
GNEApplicationWindow::onCmdEditBreakpoints(FXObject*,FXSelector,void*)
{
    GUIDialog_Breakpoints *chooser =
        new GUIDialog_Breakpoints(this);
    chooser->create();
    chooser->show();
    return 1;
}


long
GNEApplicationWindow::onCmdEditAddWeights(FXObject*,FXSelector,void*)
{
    GUIDialog_EditAddWeights *chooser =
        new GUIDialog_EditAddWeights(this);
    chooser->create();
    chooser->show();
    return 1;
}


long
GNEApplicationWindow::onCmdOpen(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Open Simulation Configuration...");
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.sumo.cfg");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory().text();
        string file = opendialog.getFilename().text();
        load(file);
        myRecentFiles.appendFile(file.c_str());
    }
    return 1;
}


long
GNEApplicationWindow::onCmdImportNet(FXObject*,FXSelector,void*)
{
    /*
    // get the new file name
    FXFileDialog opendialog(this,"Open Simulation Configuration");
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.sumo.cfg");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        load(file);
        myRecentFiles.appendFile(file.c_str());
    }
    */
    return 1;
}


long
GNEApplicationWindow::onCmdReload(FXObject*,FXSelector,void*)
{
    load(myLoadThread->getFileName());
    return 1;
}


long
GNEApplicationWindow::onCmdOpenRecent(FXObject*,FXSelector,void *data)
{
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    }
    string file = (const char*)data;
    load(file);
    return 1;
}


long
GNEApplicationWindow::onCmdClose(FXObject*,FXSelector,void*)
{
    closeAllWindows();
    return 1;
}

long
GNEApplicationWindow::onCmdLoadImage(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Öffne BitMap");
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.bmp");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory().text();
        FXImage *fximg=new FXBMPImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
        if (fximg) {
            FXFileStream stream;
            if (stream.open(opendialog.getFilename().text(),FXStreamLoad)) {
                fximg->loadPixels(stream);
                stream.close();
                fximg->create();
            }
        }
        Image *img = new Image(fximg,getApp());
        bool extrFlag = true;

        //Sets the Flag for Extract Streets if a sceletton is loaded
        FXint wid = img->GetFXImage()->getWidth();
        FXint hei = img->GetFXImage()->getHeight();

        for (FXint i=0 ; i<wid ; ++i) {
            for (FXint j=0; j<hei ; ++j) {

                FXColor col=img->GetFXImage()->getPixel(i,j);

                // prooves if a pixel is coloured
                if (
                    //not white
                    (col!=FXRGB(255,255,255))&&
                    //not black
                    (col!=FXRGB(0,0,0))

                ) {

                    extrFlag=false;
                }
            }
        }
        GNEImageProcWindow *nWindow =
            new GNEImageProcWindow(this, &myNetBuilder, myMDIClient, myMDIMenu,
                                   img, extrFlag, FXString("Hallo!!!"));
        nWindow->create();
        if (myMDIClient->numChildren()==1) {
            nWindow->maximize();
        } else {
            myMDIClient->vertical(true);
        }
        myMDIClient->setActiveChild(nWindow);
    }
    return 1;

}


long
GNEApplicationWindow::onUpdSaveImage(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GNEApplicationWindow::onCmdSaveImage(FXObject*,FXSelector,void*)
{
    /*
    if(m_img)
    {
    FXFileDialog savedialog(this,"Save Document");
    FXString file=imgfilename;
    savedialog.setSelectMode(SELECTFILE_ANY);
    savedialog.setPatternList("*.bmp");
    savedialog.setCurrentPattern(0);
    savedialog.setFilename(file);
    if(gCurrentFolder.length()!=0)
    savedialog.setDirectory(gCurrentFolder.c_str());
    if(savedialog.execute())
    {
    //setCurrentPattern(savedialog.getCurrentPattern());
    file=savedialog.getFilename();
    if(FXFile::exists(file))
    {
    if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Overwrite Document","Overwrite existing document: %s?",file.text())) return 1;
    }
    FXFileStream stream;
    FXString s=".bmp";
    FXString a=savedialog.getFilename().text();
    if(a.right(4)==".bmp")
    {
    if(stream.open(savedialog.getFilename().text(),FXStreamSave))
    	{
    		m_img->GetFXImage()->savePixels(stream);
    		stream.close();
    	}
    }
    else
    if(stream.open(savedialog.getFilename().text()+s,FXStreamSave))
    	{
    		m_img->GetFXImage()->savePixels(stream);
    		stream.close();
    	}
    }
    }
    return 1;
    */
    return 1;
}


long
GNEApplicationWindow::onCmdSaveEdgesNodes(FXObject*,FXSelector,void*)
{
    FXFileDialog savedialog(this,"Save Nodes and Edges...");
    savedialog.setSelectMode(SELECTFILE_ANY);
    savedialog.setPatternList("*");
    savedialog.setCurrentPattern(0);
    if (gCurrentFolder.length()!=0) {
        savedialog.setDirectory(gCurrentFolder.c_str());
    }
    if (!savedialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, savedialog.getFilename())) {
        return 1;
    }
    //setCurrentPattern(savedialog.getCurrentPattern());
    FXString file = savedialog.getFilename();
    string filestr = file.text();

    // prune the postfix
    if (filestr.rfind(".nod.xml")==filestr.length()-8
            ||
            filestr.rfind(".edg.xml")==filestr.length()-8) {

        filestr = filestr.substr(0, filestr.length()-8);
        file = filestr.c_str();
    }
    bool saveNodes = true;
    bool saveEdges = true;
    if (FXFile::exists(file+".nod.xml")) {
        if (MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Overwrite Document","Overwrite existing document: %s?",
                (file+".nod.xml").text())) {
            saveNodes = false;
        }
    }
    if (FXFile::exists(file+".edg.xml")) {
        if (MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Overwrite Document","Overwrite existing document: %s?",
                (file+".edg.xml").text())) {
            saveEdges = false;
        }
    }
    if (saveNodes) {
        myNetBuilder.getNodeCont().savePlain(filestr + ".nod.xml");
    }
    if (saveEdges) {
        myNetBuilder.getEdgeCont().savePlain(filestr + ".edg.xml");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveNet(FXObject*,FXSelector,void*)
{
    FXFileDialog savedialog(this,"Save Build Net...");
    savedialog.setSelectMode(SELECTFILE_ANY);
    savedialog.setPatternList("SUMO Networks (*.net.xml)");
    savedialog.setCurrentPattern(0);
    if (gCurrentFolder.length()!=0) {
        savedialog.setDirectory(gCurrentFolder.c_str());
    }
    if (!savedialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, savedialog.getFilename())) {
        return 1;
    }
    //setCurrentPattern(savedialog.getCurrentPattern());
    FXString file = savedialog.getFilename();
    string filestr = file.text();

    // prune the postfix
    if (filestr.rfind(".net.xml")==filestr.length()-8) {
        filestr = filestr.substr(0, filestr.length()-8);
        file = filestr.c_str();
    }
    if (FXFile::exists(file+".net.xml")) {
        if (MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Overwrite Document","Overwrite existing document: %s?",
                (file+".net.xml").text())) {
            return 1;
        }
    }
    ofstream out((filestr + ".net.xml").c_str());
    OptionsCont &oc = OptionsSubSys::getOptions();
    oc.clear();
    myNetBuilder.insertNetBuildOptions(oc);
    myNetBuilder.save(out, oc);
    return 1;
}


long
GNEApplicationWindow::onUpdSaveEdgesNodes(FXObject *sender,FXSelector,void*ptr)
{
    /*
    GUIGlChildWindow *child =
        static_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    bool allow = false;
    if(child!=0) {
        allow = child->getBuildGLCanvas()==0;
        allow =
            allow & static_cast<GNEImageProcWindow*>(child)->haveBuild
    }
    */
    bool allow = myNetBuilder.getNodeCont().size()!=0;
    sender->handle(this,
                   allow?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdSaveNet(FXObject *sender,FXSelector,void*ptr)
{
    /*
    GUIGlChildWindow *child =
        static_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    bool allow = false;
    if(child!=0) {
        allow = child->getBuildGLCanvas()==0;
        allow =
            allow & static_cast<GNEImageProcWindow*>(child)->haveBuild
    }
    */
    bool allow = myNetBuilder.getNodeCont().size()!=0 && myNetBuilder.netBuild();
    sender->handle(this,
                   allow?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdOpen(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   myAmLoading?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdReload(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   myAmLoading||myLoadThread->getFileName()==""
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdOpenRecent(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   myAmLoading?FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdAddMicro(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   myAmLoading||!myRunThread->simulationAvailable()
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onCmdStart(FXObject*,FXSelector,void*)
{
    // check whether a net was loaded successfully
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 1;
    }
    // check whether it was started before and paused;
    //  when yes, prompt the user for acknowledge
    if (_wasStarted) {
        myRunThread->resume();
        return 1;
    }
    _wasStarted = true;
    myRunThread->begin();
    return 1;
}


long
GNEApplicationWindow::onCmdStop(FXObject*,FXSelector,void*)
{
    myRunThread->stop();
    return 1;
}


long
GNEApplicationWindow::onCmdStep(FXObject*,FXSelector,void*)
{
    myRunThread->singleStep();
    return 1;
}


long
GNEApplicationWindow::onCmdClearMsgWindow(FXObject*,FXSelector,void*)
{
    myMessageWindow->clear();
    return 1;
}


long
GNEApplicationWindow::onUpdStart(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   !myRunThread->simulationIsStartable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdStop(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   !myRunThread->simulationIsStopable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdStep(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   !myRunThread->simulationIsStepable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdSimSettings(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   !myRunThread->simulationAvailable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdEditChosen(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   !myRunThread->simulationAvailable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdEditAddWeights(FXObject *sender,FXSelector,void *ptr)
{
    sender->handle(this,
                   !myRunThread->simulationAvailable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onUpdEditBreakpoints(FXObject *sender,FXSelector,void *ptr)
{
    sender->handle(this,
                   !myRunThread->simulationAvailable()||myAmLoading
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onCmdAppSettings(FXObject*,FXSelector,void*)
{
    GUIDialog_AppSettings *d = new GUIDialog_AppSettings(this);
    d->create();
    d->show(PLACEMENT_OWNER);
    return 1;
}


long
GNEApplicationWindow::onCmdSimSettings(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GNEApplicationWindow::onCmdNewMicro(FXObject*,FXSelector,void*)
{
    openNewView(GUISUMOViewParent::MICROSCOPIC_VIEW);
    return 1;
}


long
GNEApplicationWindow::onCmdAbout(FXObject*,FXSelector,void*)
{
    GUIDialog_AboutSUMO *about =
        new GUIDialog_AboutSUMO(this, "About SUMO", 0, 0);
    about->create();
    about->show(PLACEMENT_OWNER);
    return 1;
}


long
GNEApplicationWindow::onLoadThreadEvent(FXObject*, FXSelector, void*)
{
    eventOccured();
    return 1;
}


long
GNEApplicationWindow::onRunThreadEvent(FXObject*, FXSelector, void*)
{
    eventOccured();
    return 1;
}


void
GNEApplicationWindow::eventOccured()
{
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent *e = static_cast<GUIEvent*>(myEvents.top());
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
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
        }
        delete e;
    }
    myToolBar3->forceRefresh();
}


void
GNEApplicationWindow::handleEvent_SimulationLoaded(GUIEvent *e)
{
    GUITexturesHelper::init(getApp());
    myAmLoading = false;
    GUIEvent_SimulationLoaded *ec =
        static_cast<GUIEvent_SimulationLoaded*>(e);
    // check whether the loading was successfull
    if (ec->_net==0) {
        // report failure
        string text = "Loading of '" + ec->_file + "' failed!";
        myStatusbar->getStatusLine()->setText(text.c_str());
        myStatusbar->getStatusLine()->setNormalText(text.c_str());
    } else {
        // initialise global information
        gSimInfo = new GUISimInfo(*(ec->_net));
        gNetWrapper = ec->_net->getWrapper();
        // report success
        string text = "'" + ec->_file + "' loaded.";
        myStatusbar->getStatusLine()->setText(text.c_str());
        myStatusbar->getStatusLine()->setNormalText(text.c_str());
        // initialise simulation thread
        myRunThread->init(ec->_net, ec->_begin, ec->_end);
        _wasStarted = false;
        // initialise views
        myViewNumber = 0;
        openNewView(GUISUMOViewParent::MICROSCOPIC_VIEW);
        // set simulation name on the caption
        string caption = "SUMO " + string(VERSION) + " - " + ec->_file;
        setTitle(caption.c_str());
        ostringstream str;
        // set simulation step begin information
        str << myRunThread->getCurrentTimeStep();
        myLCDLabel->setText(str.str().c_str());
    }
    getApp()->endWaitCursor();
    // start if wished
    if (myRunAtBegin&&ec->_net!=0) {
        onCmdStart(0, 0, 0);
    }
    update();
}


void
GNEApplicationWindow::handleEvent_SimulationStep(GUIEvent *e)
{
    updateChildren();
    ostringstream str;
    str << myRunThread->getCurrentTimeStep();
    myLCDLabel->setText(str.str().c_str());
    update();
}


void
GNEApplicationWindow::handleEvent_Message(GUIEvent *e)
{
    GUIEvent_Message *ec =
        static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendText(ec->getOwnType(), ec->getMsg());
}


void
GNEApplicationWindow::handleEvent_SimulationEnded(GUIEvent *e)
{
    GUIEvent_SimulationEnded *ec =
        static_cast<GUIEvent_SimulationEnded*>(e);
    if (!gSuppressEndInfo) {
        // build the text
        stringstream text;
        text << "The simulation has ended at time step "
        << ec->getTimeStep() << "." << endl;
        switch (ec->getReason()) {
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
        case GUIEvent_SimulationEnded::ER_TOO_SLOW:
            text << "Reason: The simulation got too slow.";
            break;
        default:
            throw 1;
        }
        //
        onCmdStop(0, 0, 0);
        string tstr = text.str();
        if (ec->getReason()!=GUIEvent_SimulationEnded::ER_FORCED) {
            FXMessageBox::warning(this, MBOX_OK, "Simulation Ended",
                                  tstr.c_str());
        }
    } else {
        onCmdStop(0, 0, 0);
    }
    if (gQuitOnEnd) {
        closeAllWindows();
        getApp()->exit(0);
    }
}



void
GNEApplicationWindow::load(const std::string &file)
{
    getApp()->beginWaitCursor();
    myAmLoading = true;
    closeAllWindows();
    if (FXFile::match("*.net.xml", FXFile::name(file.c_str()))) {
        myLoadThread->load(file, true);
    } else {
        myLoadThread->load(file, false);
    }
    string text = "Loading '" + file + "'.";
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
    update();
}



void
GNEApplicationWindow::openNewView(GUISUMOViewParent::ViewType type)
{
    if (!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return;
    }
    string caption = "View #" + toString(myViewNumber++);
    FXuint opts = MDI_TRACKING;
    GUISUMOViewParent* w = 0;
    if (myMDIClient->numChildren()==0) {
        w = new GNEViewParent(myMDIClient, 0,
                              myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
                              this, type, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
        w->init(type, 0, myRunThread->getNet());
    } else {
        w = new GNEViewParent(myMDIClient, getBuildGLCanvas(),
                              myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
                              this, type, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
        w->init(type, getBuildGLCanvas(), myRunThread->getNet());
    }
    w->create();
    if (myMDIClient->numChildren()==1) {
        w->maximize();
    } else {
        myMDIClient->vertical(true);
    }
    myMDIClient->setActiveChild(w);
}


FXGLCanvas *
GNEApplicationWindow::getBuildGLCanvas() const
{
    if (myMDIClient->numChildren()==0) {
        return 0;
    }
    GUISUMOViewParent *share_tmp1 =
        static_cast<GUISUMOViewParent*>(myMDIClient->childAtIndex(0));
    return share_tmp1->getBuildGLCanvas();
}


void
GNEApplicationWindow::closeAllWindows()
{
    myTrackerLock.lock();
    myLCDLabel->setText("-----------");
    // remove trackers and other external windows
    size_t i;
    for (i=0; i<mySubWindows.size(); i++) {
        mySubWindows[i]->destroy();
    }
    for (i=0; i<myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->destroy();
    }
    // delete the simulation
    myRunThread->deleteSim();
    delete gSimInfo;
    gSimInfo = 0;
    // reset the caption
    string caption = "SUMO " + string(VERSION) + " - no simulation loaded";
    setTitle(caption.c_str());
    // delete other children
    while (mySubWindows.size()!=0) {
        delete mySubWindows[0];
    }
    while (myTrackerWindows.size()!=0) {
        delete myTrackerWindows[0];
    }
    mySubWindows.clear();
    // clear selected items
    gSelected.clear();
    // add a separator to the log
    myMessageWindow->addSeparator();
    myTrackerLock.unlock();
    //
    update();
}


FXCursor *
GNEApplicationWindow::getDefaultCursor()
{
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


SUMOTime
GNEApplicationWindow::getCurrentSimTime() const
{
    return myRunThread->getCurrentTimeStep();
}



long
GNEApplicationWindow::onUpdPictureMenu(FXObject *sender,FXSelector,void*ptr)
{
    GUIGlChildWindow *child =
        static_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    bool allow = false;
    if (child!=0) {
        allow = child->getBuildGLCanvas()==0;
    }
    sender->handle(this,
                   allow?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onCmdPictureMenu(FXObject *sender,FXSelector sel,void*ptr)
{
    GUIGlChildWindow *child =
        static_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    bool allow = false;
    if (child!=0) {
        if (child->getBuildGLCanvas()==0) {
            child->handle(sender, sel, ptr);
        }
    }
    return 1;
}


long
GNEApplicationWindow::onUpdGraphMenu(FXObject *sender,FXSelector,void*ptr)
{
    GUIGlChildWindow *child =
        static_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    bool allow = false;
    if (child!=0) {
        allow = child->getBuildGLCanvas()==0;
    }
    sender->handle(this,
                   allow?FXSEL(SEL_COMMAND,ID_ENABLE):FXSEL(SEL_COMMAND,ID_DISABLE),
                   ptr);
    return 1;
}


long
GNEApplicationWindow::onCmdGraphMenu(FXObject *sender,FXSelector sel,void*ptr)
{
    GUIGlChildWindow *child =
        static_cast<GUIGlChildWindow*>(myMDIClient->getActiveChild());
    bool allow = false;
    if (child!=0) {
        if (child->getBuildGLCanvas()==0) {
            child->handle(sender, sel, ptr);
        }
    }
    return 1;
}



/****************************************************************************/

