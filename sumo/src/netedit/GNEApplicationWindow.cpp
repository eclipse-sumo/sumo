//---------------------------------------------------------------------------//
//                        GNEApplicationWindow.cpp
//  Class for the main gui window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 22. Nov 2004
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
// Revision 1.1  2004/11/23 10:45:06  dkrajzew
// netedit by A. Gaubatz added
//
// Revision 1.1  2004/11/22 12:45:40  dksumo
// added 'netedit' classes
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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
#include <utils/convert/ToString.h>
#include <utils/foxtools/FXLCDLabel.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <sumo_version.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include <gui/GUIEvent_SimulationLoaded.h>
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvents.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <gui/dialogs/GUIDialog_AboutSUMO.h>
#include <gui/dialogs/GUIDialog_AppSettings.h>
#include <gui/dialogs/GUIDialog_SimSettings.h>
#include <gui/dialogs/GUIDialog_MicroViewSettings.h>
#include <gui/dialogs/GUIDialog_GLChosenEditor.h>
#include <gui/dialogs/GUIDialog_EditAddWeights.h>
#include <gui/dialogs/GUIDialog_Breakpoints.h>
#include <gui/GUIThreadFactory.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <guisim/GUINetWrapper.h>

#include "Image.h"
#include "ConfigDialog.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;
using namespace FXEX;


/* =========================================================================
 * FOX-declarations
 * ======================================================================= */
FXDEFMAP(GNEApplicationWindow) GNEApplicationWindowMap[]=
{
    //________Message_Type____________ID________________________Message_Handler________
    FXMAPFUNC(SEL_COMMAND,  MID_QUIT,        GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_QUIT,        GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,      GNEApplicationWindow::onCmdQuit),

    FXMAPFUNC(SEL_COMMAND,  MID_OPEN,              GNEApplicationWindow::onCmdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,        GNEApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_COMMAND,  MID_RELOAD,            GNEApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE,             GNEApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_IMPORT,            GNEApplicationWindow::onCmdImport),
    FXMAPFUNC(SEL_COMMAND,  MID_EXPORT,            GNEApplicationWindow::onCmdExport),
    FXMAPFUNC(SEL_COMMAND,  MID_EDITCHOSEN,        GNEApplicationWindow::onCmdEditChosen),
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_ADD_WEIGHTS,  GNEApplicationWindow::onCmdEditAddWeights),
    FXMAPFUNC(SEL_COMMAND,  MID_EDIT_BREAKPOINTS,  GNEApplicationWindow::onCmdEditBreakpoints),

//new Andreas (Anfang)
    FXMAPFUNC(SEL_COMMAND,  MID_EXTRACT_STREETS,  GNEApplicationWindow::onCmdExtractStreets),
    FXMAPFUNC(SEL_COMMAND,  MID_DILATION,  GNEApplicationWindow::onCmdDilate),
    FXMAPFUNC(SEL_COMMAND,  MID_EROSION,  GNEApplicationWindow::onCmdErode),
    FXMAPFUNC(SEL_COMMAND,  MID_OPENING,  GNEApplicationWindow::onCmdOpening),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSING,  GNEApplicationWindow::onCmdClosing),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE_GAPS,  GNEApplicationWindow::onCmdCloseGaps),
    FXMAPFUNC(SEL_COMMAND,  MID_SKELETONIZE,  GNEApplicationWindow::onCmdSkeletonize),
    FXMAPFUNC(SEL_COMMAND,  MID_RARIFY,  GNEApplicationWindow::onCmdRarify),
    FXMAPFUNC(SEL_COMMAND,  MID_CREATE_GRAPH,  GNEApplicationWindow::onCmdCreateGraph),
    FXMAPFUNC(SEL_COMMAND,  MID_BLACK_CORPSES,  GNEApplicationWindow::onCmdBlackCorpses),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_BMP_DIALOG,  GNEApplicationWindow::onCmdShowBMPDialog),
//new Andreas (End)

//new Miguel (Anfang)
    FXMAPFUNC(SEL_COMMAND,  MID_SHOW_GRAPH_ON_EMPTY_BITMAP,  GNEApplicationWindow::onCmdShowGraphOnEmptyBitmap),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOW_GRAPH_ON_ACTUAL_BITMAP,  GNEApplicationWindow::onCmdShowGraphOnActualBitmap),
    FXMAPFUNC(SEL_COMMAND,  MID_REDUCE_VERTEXES,  GNEApplicationWindow::onCmdReduceVertexes),
    FXMAPFUNC(SEL_COMMAND,  MID_REDUCE_VERTEXES_PLUS,  GNEApplicationWindow::onCmdReduceVertexesPlus),
    FXMAPFUNC(SEL_COMMAND,  MID_REDUCE_EDGES,  GNEApplicationWindow::onCmdReduceEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_MERGE_VERTEXES,  GNEApplicationWindow::onCmdMergeVertexes),
    FXMAPFUNC(SEL_COMMAND,  MID_EXPORT_VERTEXES_XML,  GNEApplicationWindow::onCmdExportVertexesXML),
    FXMAPFUNC(SEL_COMMAND,  MID_EXPORT_EDGES_XML,  GNEApplicationWindow::onCmdExportEdgesXML),
//new Miguel (End)

    FXMAPFUNC(SEL_PAINT,             ID_CANVAS,   GNEApplicationWindow::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,   ID_CANVAS,   GNEApplicationWindow::onMouseDown),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE, ID_CANVAS,   GNEApplicationWindow::onMouseUp),
    FXMAPFUNC(SEL_MOTION,            ID_CANVAS,   GNEApplicationWindow::onMouseMove),
    FXMAPFUNC(SEL_COMMAND,           ID_CLEAR,    GNEApplicationWindow::onCmdClear),
    FXMAPFUNC(SEL_UPDATE,            ID_CLEAR,    GNEApplicationWindow::onUpdClear),

    FXMAPFUNC(SEL_COMMAND,  MID_APPSETTINGS,   GNEApplicationWindow::onCmdAppSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMSETTINGS,   GNEApplicationWindow::onCmdSimSettings),
    FXMAPFUNC(SEL_COMMAND,  MID_ABOUT,         GNEApplicationWindow::onCmdAbout),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_MICROVIEW, GNEApplicationWindow::onCmdNewMicro),
    FXMAPFUNC(SEL_COMMAND,  MID_NEW_LANEAVIEW, GNEApplicationWindow::onCmdNewLaneA),
    FXMAPFUNC(SEL_COMMAND,  MID_START,         GNEApplicationWindow::onCmdStart),
    FXMAPFUNC(SEL_COMMAND,  MID_STOP,          GNEApplicationWindow::onCmdStop),
    FXMAPFUNC(SEL_COMMAND,  MID_STEP,          GNEApplicationWindow::onCmdStep),

    FXMAPFUNC(SEL_UPDATE,   MID_OPEN,              GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_UPDATE,   MID_RELOAD,            GNEApplicationWindow::onUpdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_MICROVIEW,     GNEApplicationWindow::onUpdAddMicro),
    FXMAPFUNC(SEL_UPDATE,   MID_NEW_LANEAVIEW,     GNEApplicationWindow::onUpdAddALane),
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

/* =========================================================================
 * member method definitions
 * ======================================================================= */
GNEApplicationWindow::GNEApplicationWindow(FXApp* a,
                                           GUIThreadFactory &threadFactory,
                                           int glWidth, int glHeight,
                                           const std::string &config)
    : GUIMainWindow(a, glWidth, glHeight),
    myLoadThread(0), myRunThread(0),
    myAmLoading(false),
    mySimDelay(50)
{
    // No image loaded, yet.
    m_img=NULL;
    //dialog=new FXTestDialog(this);

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
    myToolBarDrag=new FXToolBarShell(this,FRAME_NORMAL);
    myToolBar = new FXToolBar(this,myToolBarDrag,
        LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myToolBar, myToolBar, FXToolBar::ID_TOOLBARGRIP,
        TOOLBARGRIP_DOUBLE);

    myIMGToolBarDrag=new FXToolBarShell(this,FRAME_NORMAL);
    myIMGToolBar = new FXToolBar(this,myIMGToolBarDrag,
        LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myIMGToolBar, myIMGToolBar, FXToolBar::ID_TOOLBARGRIP,
        TOOLBARGRIP_DOUBLE);

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
    scrollBox=new FXScrollWindow(myMainSplitter,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0);

    // Drawing canvas
    myCanvas=new FXCanvas(scrollBox,this,ID_CANVAS,FRAME_SUNKEN|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,0,0);

    //delete
    drawColor=FXRGB(255,0,0);
    //delete

    // fill menu and tool bar
    fillMenuBar();
    fillToolBar();
    // build additional threads
    myLoadThread = threadFactory.buildLoadThread(this, myEvents, myLoadThreadEvent);
    myRunThread = threadFactory.buildRunThread(this, *mySimDelayTarget, myEvents,
        myRunThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");

    // set the caption
    string caption = string("SUMO ") + string(version)
        + string(" - no simulation loaded");
    setTitle(caption.c_str());

    // start the simulation-thread
    //  (it will loop until the application ends deciding by itself whether
    //        to perform a step or not)
    myRunThread->start();
    // check whether a simulation shall be started on begin
    if(config!="") {
        gStartAtBegin = true;
        load(config);
    } else {
        gStartAtBegin = false;
    }
    setIcon( GUIIconSubSys::getIcon(ICON_APP) );

    FXFontDesc fdesc;
    getApp()->getNormalFont()->getFontDesc(fdesc);
    fdesc.weight = FONTWEIGHT_BOLD;
    myBoldFont = new FXFont(getApp(), fdesc);
}


void
GNEApplicationWindow::create()
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
    myToolBarDrag->create();
    myIMGToolBarDrag->create();
    myFileMenu->create();
//  mySubFileMenu1->create();
    myEditMenu->create();
    myImageMenu->create();
    myGraphMenu->create();
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





GNEApplicationWindow::~GNEApplicationWindow()
{
    myRunThread->prepareDestruction();
    closeAllWindows();
    //
    GUIIconSubSys::close();
    GUITexturesHelper::close();
    delete myGLVisual;
    // delete some non-parented windows
    delete myToolBarDrag;
    //
//!!!!    myRunThread->yield();
    delete myRunThread;
    delete myFileMenu;
    delete myEditMenu;
    delete myImageMenu;
    delete myGraphMenu;
    delete mySettingsMenu;
    delete myWindowsMenu;
    delete myHelpMenu;

}


void
GNEApplicationWindow::detach()
{
    FXMainWindow::detach();
    myMenuBarDrag->detach();
    myToolBarDrag->detach();
    myIMGToolBarDrag->detach();
}


void
GNEApplicationWindow::fillMenuBar()
{
    // build file menu
    myFileMenu = new FXMenuPane(this);
    //mySubFileMenu1 = new FXMenuPane(this);
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
    new FXMenuSeparator(myFileMenu);
//  new FXMenuCascade(myFileMenu,"Import",NULL,mySubFileMenu1);
    new FXMenuCommand(myFileMenu,
        "&Load Bitmap\t\tOpens a bitmap file.",
        GUIIconSubSys::getIcon(ICON_CLOSE),this,MID_IMPORT);
    //new
    new FXMenuCommand(myFileMenu,
        "&Save Bitmap\t\tSaves the current bitmap file.",
        GUIIconSubSys::getIcon(ICON_CLOSE),this,MID_EXPORT);
    //new
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
        "Strassenflächen Extrahieren...\t\tÖffnet einen Dialog für die Extraktion der Strassenflächen. Wählen Sie die Farben, die zu den Strassen gehören.",
        NULL,this,MID_EXTRACT_STREETS);
    new FXMenuCommand(myImageMenu,
        "Schwarze Leichen...\t\tLöscht isolierte schwarze Flächen aus dem Bild.",
        NULL,this,MID_BLACK_CORPSES);
    new FXMenuCommand(myImageMenu,
        "Erosion...\t\tÖffnet ein Dialogfenster, in welchem Sie die Anzahl der Erosionen bestimmen können.",
        NULL,this,MID_EROSION);
    new FXMenuCommand(myImageMenu,
        "Dilatation...\t\tÖffnet ein Dialogfenster, in welchem Sie die Anzahl der Dilatationen bestimmen können.",
        NULL,this,MID_DILATION);
    new FXMenuCommand(myImageMenu,
        "Morphologisch Öffnen\t\tÖffnet das Bild morphologisch. Dies entspricht einer Erosion gefolgt von einer Dilatation.",
        NULL,this,MID_OPENING);
    new FXMenuCommand(myImageMenu,
        "Morphologisch Schließen\t\tSchließt das Bild morphologisch. Dies entspricht einer Dilatation gefolgt von einer Erosion.",
        NULL,this,MID_CLOSING);
    new FXMenuCommand(myImageMenu,
        "Lücken Schließen\t\tSchließt kleine Löcher und Lücken des Bildes.",
        NULL,this,MID_CLOSE_GAPS);
    new FXMenuCommand(myImageMenu,
        "Skelettieren\t\tSkelettiert das Bild. Die Strassen werden dann als dünne Strassenmittellinie repräsentiert.",
        NULL,this,MID_SKELETONIZE);
    new FXMenuCommand(myImageMenu,
        "Skelett Maximal Verdünnen\t\tDas Strassenskelett wird maximal verdünnt. Löscht man nach dieser Operation noch Pixel zerfällt das Skelett.",
        NULL,this,MID_RARIFY);
    new FXMenuCommand(myImageMenu,
        "Erzeuge Strassengraphen\t\tErzeugt auis dem generierten Skelett einen Graphen.",
        NULL,this,MID_CREATE_GRAPH);

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
    new FXMenuCheck(myWindowsMenu,
        "Show Toolbar\t\tToggle the Toolbar on/off.",
        myToolBar, FXWindow::ID_TOGGLESHOWN);
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
GNEApplicationWindow::fillToolBar()
{
    //The Simulation Toolbar

    // build file tools
    new FXButton(myToolBar,"\t\tOpen a Simulation (Configuration File).",
        GUIIconSubSys::getIcon(ICON_OPEN), this, MID_OPEN,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myToolBar,"\t\tReload the Simulation (Configuration File).",
        GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_RELOAD,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXToolBarGrip(myToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);

    // build simulation tools
    new FXButton(myToolBar,"\t\tStart the loaded Simulation.",
        GUIIconSubSys::getIcon(ICON_START), this, MID_START,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myToolBar,"\t\tStop the running Simulation.",
        GUIIconSubSys::getIcon(ICON_STOP), this, MID_STOP,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myToolBar,"\t\tPerform a single Simulation Step..",
        GUIIconSubSys::getIcon(ICON_STEP), this, MID_STEP,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXLabel(myToolBar, "Current Step:");

    myLCDLabel = new FXLCDLabel(myToolBar, 10, 0, 0,
        LCDLABEL_LEADING_ZEROS);
    myLCDLabel->setHorizontal(2);
    myLCDLabel->setVertical(2);
    myLCDLabel->setThickness(2);
    myLCDLabel->setGroove(2);
    myLCDLabel->setText("-----------");

    new FXToolBarGrip(myToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);

    new FXLabel(myToolBar, "Delay:");
    mySimDelayTarget =
        new FXRealSpinDial(myToolBar, 10, 0, MID_SIMDELAY,
        LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
    mySimDelayTarget->setFormatString("%.0fms");
    mySimDelayTarget->setIncrements(1,10,10);
    mySimDelayTarget->setRange(0,1000);
    mySimDelayTarget->setValue(50);

    new FXToolBarGrip(myToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);

    // build view tools
    new FXButton(myToolBar,"\t\tOpen a new microscopic View.",
        GUIIconSubSys::getIcon(ICON_MICROVIEW), this, MID_NEW_MICROVIEW,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myToolBar,
        "\t\tOpen a new Lane aggregated View.",
        GUIIconSubSys::getIcon(ICON_LAGGRVIEW), this, MID_NEW_LANEAVIEW,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

    //The Image Toolbar

/*
new FXMenuCommand(myImageMenu,
        "Strassenflächen Extrahieren...\t\tÖffnet einen Dialog für die Extraktion der Strassenflächen. Wählen Sie die Farben, die zu den Strassen gehören.",
        NULL,this,MID_EXTRACT_STREETS);
    new FXMenuCommand(myImageMenu,
        "Schwarze Leichen...\t\tLöscht isolierte schwarze Flächen aus dem Bild.",
        NULL,this,MID_BLACK_CORPSES);
    new FXMenuCommand(myImageMenu,
        "Erosion...\t\tÖffnet ein Dialogfenster, in welchem Sie die Anzahl der Erosionen bestimmen können.",
        NULL,this,MID_EROSION);
    new FXMenuCommand(myImageMenu,
        "Dilatation...\t\tÖffnet ein Dialogfenster, in welchem Sie die Anzahl der Dilatationen bestimmen können.",
        NULL,this,MID_DILATION);
    new FXMenuCommand(myImageMenu,
        "Morphologisch Öffnen\t\tÖffnet das Bild morphologisch. Dies entspricht einer Erosion gefolgt von einer Dilatation.",
        NULL,this,MID_OPENING);
    new FXMenuCommand(myImageMenu,
        "Morphologisch Schließen\t\tSchließt das Bild morphologisch. Dies entspricht einer Dilatation gefolgt von einer Erosion.",
        NULL,this,MID_CLOSING);
    new FXMenuCommand(myImageMenu,
        "Lücken Schließen\t\tSchließt kleine Löcher und Lücken des Bildes.",
        NULL,this,MID_CLOSE_GAPS);
    new FXMenuCommand(myImageMenu,
        "Skelettieren\t\tSkelettiert das Bild. Die Strassen werden dann als dünne Strassenmittellinie repräsentiert.",
        NULL,this,MID_SKELETONIZE);
    new FXMenuCommand(myImageMenu,
        "Skelett Maximal Verdünnen\t\tDas Strassenskelett wird maximal verdünnt. Löscht man nach dieser Operation noch Pixel zerfällt das Skelett.",
        NULL,this,MID_RARIFY);
    new FXMenuCommand(myImageMenu,
        "Erzeuge Strassengraphen\t\tErzeugt auis dem generierten Skelett einen Graphen.",
        NULL,this,MID_CREATE_GRAPH);
*/

    new FXButton(myIMGToolBar,"\t\tExtract Street Colors.",
        GUIIconSubSys::getIcon(ICON_EXTRACT), this, MID_EXTRACT_STREETS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tDilatation",
        GUIIconSubSys::getIcon(ICON_DILATE), this, MID_DILATION,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tErosion.",
        GUIIconSubSys::getIcon(ICON_ERODE), this, MID_EROSION,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tOpening. (Erosion followed by a dilatation).",
        GUIIconSubSys::getIcon(ICON_OPENING), this, MID_OPENING,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tClosing (Dilatation followed by erosion).",
        GUIIconSubSys::getIcon(ICON_CLOSING), this, MID_CLOSING,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tMinimizes small white spots in black areas.",
        GUIIconSubSys::getIcon(ICON_CLOSE_GAPS), this, MID_CLOSE_GAPS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tErases black ´noise´.",
        GUIIconSubSys::getIcon(ICON_BLACK_CORPSES), this, MID_BLACK_CORPSES,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tCreates a street´s skeleton (thin black lines in the middle of the street.",
        GUIIconSubSys::getIcon(ICON_SKELETONIZE), this, MID_SKELETONIZE,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tReduces the skeleton to minimum.",
        GUIIconSubSys::getIcon(ICON_RARIFY), this, MID_RARIFY,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXButton(myIMGToolBar,"\t\tCreates a graph from the skeleton.",
        GUIIconSubSys::getIcon(ICON_CREATE_GRAPH), this, MID_CREATE_GRAPH,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);
    new FXButton(myIMGToolBar,"\t\tOpen Bitmap Configuration Dialog.",
        GUIIconSubSys::getIcon(ICON_OPEN_BMP_DIALOG), this, MID_OPEN_BMP_DIALOG,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

}

// Mouse button was pressed somewhere
long GNEApplicationWindow::onMouseDown(FXObject*,FXSelector,void*)
{
  myCanvas->grab();

  // While the mouse is down, we'll draw lines
  mdflag=1;

  return 1;
}

// The mouse has moved, draw a line
long GNEApplicationWindow::onMouseMove(FXObject*, FXSelector, void* ptr)
{
  FXEvent *ev=(FXEvent*)ptr;

  // Draw
  if(mdflag==1)
  {

    // Get DC for the canvas
    FXDCWindow dc(myCanvas);

    // Draw line
    dc.setLineWidth (1);
    dc.drawLine(ev->last_x, ev->last_y, ev->win_x, ev->win_y);
    m_img->DrawLine(ev->last_x,ev->last_y, ev->win_x, ev->win_y);

    dirty=1;
  }
  return 1;
}

// The mouse button was released again
long GNEApplicationWindow::onMouseUp(FXObject*,FXSelector,void* ptr)
{
  FXEvent *ev=(FXEvent*) ptr;
  myCanvas->ungrab();
  if(mdflag){
    FXDCWindow dc(myCanvas);
    dc.setForeground(drawColor);
    dc.drawLine(ev->last_x, ev->last_y, ev->win_x, ev->win_y);
    m_img->DrawLine(ev->last_x,ev->last_y, ev->win_x, ev->win_y);

    // We have drawn something, so now the canvas is dirty
    dirty=1;

    // Mouse no longer down
    mdflag=0;
    }
  return 1;
}

// Paint the canvas
long GNEApplicationWindow::onPaint(FXObject*,FXSelector,void* ptr)
{
  FXDCWindow dc(myCanvas);
  if(m_img)
  {
      m_img->GetFXImage()->render();
      dc.drawImage(m_img->GetFXImage(),0,0);
  }
  return 1;
  }

// Handle the clear message
long GNEApplicationWindow::onCmdClear(FXObject*,FXSelector,void*){
  /*
    FXDCWindow dc(canvas);
  dc.setForeground(canvas->getBackColor());
  dc.fillRectangle(0,0,canvas->getWidth(),canvas->getHeight());
  dirty=0;
  */
  return 1;
  }

long GNEApplicationWindow::onUpdClear(FXObject* sender,FXSelector,void*){

  if(dirty)
    sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE),NULL);
  else
    sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE),NULL);

  return 1;
  }

//new

/////////////////////////new Andeas (Anfang)
long
GNEApplicationWindow::onCmdExtractStreets(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->ExtractStreets();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdErode(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Erode();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdDilate(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Dilate();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdOpening(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Opening();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdClosing(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Closing();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdCloseGaps(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->CloseGaps();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdSkeletonize(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->CreateSkeleton();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdBlackCorpses(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->BlackCorpses(15);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdRarify(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->RarifySkeleton();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdCreateGraph(FXObject*,FXSelector,void*)
{
    Graph leergraph;
    graph=leergraph;
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        graph=m_img->Tracking(graph);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdShowBMPDialog(FXObject*,FXSelector,void*)
{
    dialog->show();
    return 1;

}
/////////////////////////new Andreas (Ende)

//Noch zu ändern!!!!!!!!!!!!!!!!!!
/////////////////////////new Miguel (Anfang)
long
GNEApplicationWindow::onCmdShowGraphOnEmptyBitmap(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->EmptyImage();
        m_img->DrawGraph(graph);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdShowGraphOnActualBitmap(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->DrawGraph(graph);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdReduceVertexes(FXObject*,FXSelector,void*)
{
    /*
    graph.Reduce();
    */
    return 1;

}

long
GNEApplicationWindow::onCmdReduceVertexesPlus(FXObject*,FXSelector,void*)
{
    graph.Reduce_plus();
    return 1;
}

long
GNEApplicationWindow::onCmdReduceEdges(FXObject*,FXSelector,void*)
{
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        graph.Reduce_Edges();
        m_img->DrawGraph(graph);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;
}

long
GNEApplicationWindow::onCmdMergeVertexes(FXObject*,FXSelector,void*)
{
    // Noch einzufügen
    return 1;
}

long
GNEApplicationWindow::onCmdExportVertexesXML(FXObject*,FXSelector,void*)
{
    graph.Export_Vertexes_XML();
    return 1;
}

long
GNEApplicationWindow::onCmdExportEdgesXML(FXObject*,FXSelector,void*)
{
    graph.Export_Edges_XML();
    return 1;
}

/////////////////////////new Miguel (Ende)



long
GNEApplicationWindow::onCmdQuit(FXObject*,FXSelector,void*)
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
GNEApplicationWindow::onCmdEditChosen(FXObject*,FXSelector,void*)
{
    GUIDialog_GLChosenEditor *chooser =
        new GUIDialog_GLChosenEditor(this);
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
    FXFileDialog opendialog(this,"Open Document");
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
    string file = string((const char*)data);
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
GNEApplicationWindow::onCmdImport(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Öffne BitMap");
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.bmp");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        FXImage *img=new FXBMPImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
        if(img)
        {
            FXFileStream stream;
            if(stream.open(opendialog.getFilename().text(),FXStreamLoad))
                {
                    img->loadPixels(stream);
                    stream.close();
                    img->create();
                }
        }
        m_img = new Image(img,getApp());
        myCanvas->setWidth(m_img->GetFXImage()->getWidth());
        myCanvas->setHeight(m_img->GetFXImage()->getHeight());
        FXDCWindow dc(myCanvas);
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    return 1;

}

long
GNEApplicationWindow::onCmdExport(FXObject*,FXSelector,void*)
{
    if(m_img)
    {
    FXFileStream stream;
    if(stream.open("c:/testaa.bmp",FXStreamSave))
        {
            m_img->GetFXImage()->savePixels(stream);
            stream.close();
        }
    }
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
    if(!myRunThread->simulationAvailable()) {
        myStatusbar->getStatusLine()->setText("No simulation loaded!");
        return 1;
    }
    // check whether it was started before and paused;
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
GNEApplicationWindow::onUpdAddALane(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myAmLoading||!myRunThread->simulationAvailable()||!gAllowAggregated
        ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
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
GNEApplicationWindow::onCmdNewLaneA(FXObject*,FXSelector,void*)
{
    openNewView(GUISUMOViewParent::LANE_AGGREGATED_VIEW);
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
        }
        delete e;
    }
    myToolBar->forceRefresh();
}


void
GNEApplicationWindow::handleEvent_SimulationLoaded(GUIEvent *e)
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
    if(gStartAtBegin&&ec->_net!=0) {
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
        default:
            throw 1;
        }
        //
        onCmdStop(0, 0, 0);
        FXMessageBox::warning(this, MBOX_OK, "Simulation Ended",
            text.str().c_str());
    } else {
        onCmdStop(0, 0, 0);
    }
    if(gQuitOnEnd) {
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
    myLoadThread->load(file);
    string text = string("Loading '") + file + string("'.");
    myStatusbar->getStatusLine()->setText(text.c_str());
    myStatusbar->getStatusLine()->setNormalText(text.c_str());
    update();
}



void
GNEApplicationWindow::openNewView(GUISUMOViewParent::ViewType type)
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
    } else {
        w = new GUISUMOViewParent( myMDIClient, getBuildGLCanvas(),
            myMDIMenu, FXString(caption.c_str()), myRunThread->getNet(),
            this, type, GUIIconSubSys::getIcon(ICON_APP), 0, opts);
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
GNEApplicationWindow::getBuildGLCanvas() const
{
    if(myMDIClient->numChildren()==0) {
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
    //
    update();
}


FXCursor *
GNEApplicationWindow::getDefaultCursor()
{
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


FXTimer *
GNEApplicationWindow::addTimeout(FXObject *tgt, FXSelector sel,
                                 FXuint ms, void *ptr)
{
    return getApp()->addTimeout(tgt, sel, ms, ptr);
}


FXTimer *
GNEApplicationWindow::removeTimeout(FXObject *tgt, FXSelector sel)
{
    return getApp()->removeTimeout(tgt, sel);
}


size_t
GNEApplicationWindow::getCurrentSimTime() const
{
    return myRunThread->getCurrentTimeStep();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


