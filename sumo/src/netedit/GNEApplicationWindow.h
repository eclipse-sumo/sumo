#ifndef GNEApplicationWindow_h
#define GNEApplicationWindow_h
//---------------------------------------------------------------------------//
//                        GNEApplicationWindow.h -
//  Class for the main gui window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 22. Nov 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.5  2004/12/21 16:56:24  agaubatz
// debug
//
// Revision 1.4  2004/12/20 23:07:23  der_maik81
// all algorithms get the configdialog as parameter
//
// Revision 1.3  2004/12/16 12:17:03  dkrajzew
// debugging
//
// Revision 1.2  2004/12/02 13:54:23  agaubatz
// Netedit update, A. Gaubatz
//
// Revision 1.1  2004/11/23 10:45:06  dkrajzew
// netedit by A. Gaubatz added
//
// Revision 1.1  2004/11/22 12:45:40  dksumo
// added 'netedit' classes
//
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
#include <utils/gui/windows/GUIMainWindow.h>
#include <gui/GUISUMOViewParent.h>
#include "Image.h"
#include "Graph.h"
#include "ConfigDialog.h"
#include "InfoDialog.h"


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
class Image;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * @class GNEApplicationWindow
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
class GNEApplicationWindow :
        public GUIMainWindow, public MFXInterThreadEventClient
{
    // FOX-declarations
    FXDECLARE(GNEApplicationWindow)
public:

    /** constructor */
    GNEApplicationWindow(FXApp* a, GUIThreadFactory &threadFactory,
        int glWidth, int glHeight, const std::string &config);

    /** destructor */
    virtual ~GNEApplicationWindow();

    /** @brief Creates the main window
        (required by FOX) */
    void create();

    /// Detaches the tool/menu bar
    void detach();

    void loadSelection(const std::string &file) const { throw 1; } // !!! to be implemented

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
    long onCmdLoadImage(FXObject*,FXSelector,void*);
    long onCmdSaveImage(FXObject*,FXSelector,void*);

    /** @brief Called by FOX if the application shall be closed
        Called either by FileMenu->Quit, the normal close-menu or SIGINT  */
    long onCmdQuit(FXObject*,FXSelector,void*);

    long onCmdEditChosen(FXObject*,FXSelector,void*);
    virtual long onCmdEditAddWeights(FXObject*,FXSelector,void*);
    long onCmdEditBreakpoints(FXObject*,FXSelector,void*);

    //!!!!! Menu Einträge von Andreas (Anfang)

    /// Extracts the bitmaps´ pixels which are determined as road area
    long onCmdExtractStreets(FXObject*,FXSelector,void*);

    /// Erodes the Bitmap (only 0/1-Bitmap)
    long onCmdErode(FXObject*,FXSelector,void*);

    /// Dilates the Bitmap (only 0/1-Bitmap)
    long onCmdDilate(FXObject*,FXSelector,void*);

    /// Creates an morphological opening (dilatation followed by erosion) of the Image
    /// (only 0/1-Bitmap)
    long onCmdOpening(FXObject*,FXSelector,void*);

    /// Creates an morphological closing (erosion followed by dilatation) of the Image
    /// (only 0/1-Bitmap)
    long onCmdClosing(FXObject*,FXSelector,void*);

    /// minimizes small white areas in a black surronding (only 0/1-Bitmap)
    long onCmdCloseGaps(FXObject*,FXSelector,void*);

    /// Filters small black Spots in the Image (only 0/1-Bitmap)
    long onCmdEraseStains(FXObject*,FXSelector,void*);

    /// Skeletonizes the image (only 0/1-Bitmap)
    long onCmdSkeletonize(FXObject*,FXSelector,void*);

    /// Creates a graph from the ´street skeleton´
    long onCmdCreateGraph(FXObject*,FXSelector,void*);

    //Opens a Dialog Window for Image-Filter Configuration
    long onCmdShowBMPDialog(FXObject*,FXSelector,void*);

    /// Opens the Show Graph on empty Bitmap Dialog
    long onCmdShowGraphOnEmptyBitmap(FXObject*,FXSelector,void*);

    /// Opens the Show Graph on actial Bitmap Dialog
    long onCmdShowGraphOnActualBitmap(FXObject*,FXSelector,void*);

    /// Opens the Reduce Vertices Dialog
    long onCmdReduceVertexes(FXObject*,FXSelector,void*);

    /// Opens the Reduce Vertices Plus Dialog
    long onCmdReduceVertexesPlus(FXObject*,FXSelector,void*);

    /// Opens the Reduce Edges Dialog
    long onCmdReduceEdges(FXObject*,FXSelector,void*);

    /// Opens the Merge Vertices Dialog
    long onCmdMergeVertexes(FXObject*,FXSelector,void*);

    /// Opens the Export Vertices XML Dialog
    long onCmdExportVertexesXML(FXObject*,FXSelector,void*);

    /// Opens the Export Edges XML Dialog
    long onCmdExportEdgesXML(FXObject*,FXSelector,void*);

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
    long onCmdClearMsgWindow(FXObject*,FXSelector,void*);

    long onLoadThreadEvent(FXObject*, FXSelector, void*);
    long onRunThreadEvent(FXObject*, FXSelector, void*);

    long onPaint(FXObject*,FXSelector,void*);
    long onMouseDown(FXObject*,FXSelector,void*);
    long onMouseUp(FXObject*,FXSelector,void*);
    long onMouseMove(FXObject*,FXSelector,void*);
    long onCmdClear(FXObject*,FXSelector,void*);
    long onUpdClear(FXObject*,FXSelector,void*);

    FXTimer *addTimeout(FXObject *tgt, FXSelector sel,
        FXuint ms=1000, void *ptr=NULL);
    FXTimer *removeTimeout(FXObject *tgt, FXSelector sel);

    FXGLCanvas *getBuildGLCanvas() const;
    size_t getCurrentSimTime() const;

    FXCursor *getDefaultCursor();


private:
    /** starts to load a simulation */
    void load(const std::string &file);

    /** this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /** opens a new simulation display */
    void openNewView(GUISUMOViewParent::ViewType type);

protected:
    /// FOX needs this for static members
    GNEApplicationWindow() { }
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
    FXMenuPane *myFileMenu, /**mySubFileMenu1,*/ *myEditMenu, *myImageMenu, *myGraphMenu,
        *mySettingsMenu, *myWindowsMenu, *myHelpMenu;

    /// A window to display messages, warnings and error in
    GUIMessageWindow *myMessageWindow;

    /// The splitter that divides the main window into vies and the log window
    FXSplitter *myMainSplitter;

    /// The status bar
    FXStatusBar *myStatusbar;

    /// for some menu detaching fun
    FXToolBarShell *myToolBarDrag, *myMenuBarDrag, *myIMGToolBarDrag;

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

    // The Image Toolbar
    FXToolBar *myIMGToolBar;

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

    //Scrollwindow for the canvas
    FXScrollWindow *scrollBox;

    FXHorizontalFrame *hoFrame;

    // Drawing Canvas
    FXCanvas *myCanvas;

    // Bitmap Member variable
    Image *m_img;

    // Bitmap Member variable
    Graph graph;

    // Image Configuration Dialog
    ConfigDialog *dialog;
	InfoDialog *dialog2;

	// default filename for the imagefiledialog
	FXString imgfilename;

	//Popupbutton for painttool
	FXPopup* paintpop;

	//Popupbutton for painttool
	FXPopup* rubberpop;

	//flag for Streetextraction..Algorithm may only be used once
	bool extrFlag;

private:
  int                mdflag;                  // Mouse button down?
  int                dirty;                   // Canvas has been painted?
  FXColor            drawColor;               // Color for the line
  int                m_mergeTol;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

