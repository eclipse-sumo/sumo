/****************************************************************************/
/// @file    GUIAppEnum.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id: $
///
// Message and object IDs used by the FOX-version of the gui
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
#ifndef GUIAppEnum_h
#define GUIAppEnum_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// enumeration
// ===========================================================================
enum {
    /// Show About - Dialog
    ID_ABOUT=FXMainWindow::ID_LAST,

    //@{ Application specific
    /// Main Window-ID
    MID_WINDOW,
    /// Main Window closes
    MID_QUIT,
    //@}

    //@{ For some always used buttons
    /// Ok-Button pressed
    MID_OK,
    /// Cancel-Button pressed
    MID_CANCEL,
    //@}

    //@{ Main Window File Menu - IDs
    /// Open New File - ID
    MID_OPEN,
    /// Reload the previously loaded simulation
    MID_RELOAD,
    /// Loads a file previously loaded
    MID_RECENTFILE,
    /// Close Simulation - ID
    MID_CLOSE,
    /// About SUMO - ID
    MID_ABOUT,
    //@}

    //@{ Main Window Simulation Control - IDs
    /// Start the Simulation
    MID_START,
    /// Stop the Simulation
    MID_STOP,
    /// Perform a single Simulation Step
    MID_STEP,
    //@}

    //@{ Main Window Windows Control - IDs
    /// Open a new microscopic View
    MID_NEW_MICROVIEW,
    //@}

    //@{ SubThread - IDs
    /// The loading Thread
    ID_LOADTHREAD_EVENT,
    /// The Simulation execution Thread
    ID_RUNTHREAD_EVENT,
    /// ID for message passing between Threads
    ID_THREAD_EVENT,
    //@}


    //@{ Edit Menu Item - IDs
    /// Open Chosen Editor - ID
    MID_EDITCHOSEN,
    /// Edit Additional Weights - ID
    MID_EDIT_ADD_WEIGHTS,
    /// Edit simulation breakpoints
    MID_EDIT_BREAKPOINTS,

    MID_CONNECTIONS,
    //@}

    //@{ Settings Menu Item - IDs
    /// Application Settings - Menu Entry
    MID_APPSETTINGS,
    /// Simulation Settings - Menu Entry
    MID_SIMSETTINGS,
    /// The Simulation Delay Control
    MID_SIMDELAY,
    //@}

    //@{ Image Menu Item - IDs
    /// Street Extraction - Menu Entry
    MID_EXTRACT_STREETS,
    /// Dilation - Menu Entry
    MID_DILATION,
    /// Erosion - Menu Entry
    MID_EROSION,
    /// Opening - Menu Entry
    MID_OPENING,
    /// Closing - Menu Entry
    MID_CLOSING,
    /// Close Gaps - Menu Entry
    MID_CLOSE_GAPS,
    /// Skeletonize - Menu Entry
    MID_SKELETONIZE,
    /// Rarify - Menu Entry
    MID_RARIFY,
    /// Rarify - Menu Entry
    MID_ERASE_STAINS,
    /// Create Graph - Menu Entry
    MID_CREATE_GRAPH,
    /// Opens a dialoge window for image-filter configuration
    MID_OPEN_BMP_DIALOG,
    //@}
    // Clear simulation settings
    MID_CLEARMESSAGEWINDOW,

    //Create Drawing Canvas for an Image
    ID_CANVAS,
    ID_CLEAR,

    //@{ Graph Menu Item - IDs
    ///Show Graph on empty Bitmap - Menu Entry
    MID_SHOW_GRAPH_ON_EMPTY_BITMAP,
    ///Show Graph on actual Bitmap - Menu Entry
    MID_SHOW_GRAPH_ON_ACTUAL_BITMAP,
    ///Reduce Vertexes - Menu Entry
    MID_REDUCE_VERTEXES,
    ///Reduce Vertexes Plus - Menu Entry
    MID_REDUCE_VERTEXES_PLUS,
    ///Reduce Edges - Menu Entry
    MID_REDUCE_EDGES,
    ///Merge Vertexes - Menu Entry
    MID_MERGE_VERTEXES,
    ///Export Vertex XML -Menu Entry
    MID_EXPORT_VERTEXES_XML,
    ///Export Edges XML - Menu Entry
    MID_EXPORT_EDGES_XML,
    //@}

    //@{ Common View Settings - IDs
    /// Recenter View - Button
    MID_RECENTERVIEW,
    /// Show Legend - Button
    MID_SHOWLEGEND,
    /// Allow Rotation - Button
    MID_ALLOWROTATION,
    /// Locate Junction - Button
    MID_LOCATEJUNCTION,
    /// Locate Edge - Button
    MID_LOCATEEDGE,
    /// Locate Vehicle - Button
    MID_LOCATEVEHICLE,
    /// Locate addtional Structure - Button
    MID_LOCATEADD,
    /// Open Viewport Editor - Button
    MID_EDITVIEWPORT,
    /// Open View Editor - Button
    MID_EDITVIEW,
    /// Show Grid - Button
    MID_SHOWGRID,
    /// Show Tool Tips - Button
    MID_SHOWTOOLTIPS,
    /// Graph Edit Button
    MID_EDIT_GRAPH,
    /// Make Snapshot - Button
    MID_MAKESNAPSHOT,
    //@}

    //@{ Common View IDs
    /// GLCanvas - ID
    MID_GLCANVAS,
    // Right Mouse Button Timeout
    //    ID_RMOUSETIMEOUT,
    //@}

    //@{ View Settings - IDs
    /// Change Coloring Scheme- combo
    MID_COLOURSCHEMECHANGE,
    /// Change the aggregation Time Value - combo
    MID_LANEAGGTIME,
    /// The Aggregation Memory - dial
    MID_LANEAGGRMEM,
    //@}


    //@{ Object PopUp Entries - IDs
    /// Center View To Object - Menu Entry
    MID_CENTER,
    /// Center View To Object - Menu Entry
    MID_COPY_NAME,
    /// Center View To Object - Menu Entry
    MID_COPY_TYPED_NAME,
    /// Show Object Parameter - Menu Entry
    MID_SHOWPARS,
    /// Show TLS phases - Menu Entry
    MID_SWITCH,
    MID_SHOWPHASES = MID_SWITCH + 20,
    /// Begin to track phases - Menu Entry
    MID_TRACKPHASES,
    /// Add to selected Items - Menu Entry
    MID_ADDSELECT,
    /// Add successors to selected Items - Menu Entry
    MID_ADDSELECT_SUCC,
    /// Remove from selected Items - Menu Etry
    MID_REMOVESELECT,
    /// Open the object's manipulator
    MID_MANIP,
    /// Draw a vehicle's route
    MID_DRAWROUTE,
    /// Show vehicle's current route
    MID_SHOW_CURRENTROUTE,
    /// Hide vehicle's current route
    MID_HIDE_CURRENTROUTE,
    /// Show all vehicle's routes
    MID_SHOW_ALLROUTES,
    /// Hide all vehicle's routes
    MID_HIDE_ALLROUTES,
    /// Start to track a vehicle
    MID_START_TRACK,
    /// Stop to track a vehicle
    MID_STOP_TRACK,
    /// Rename the object
    MID_RENAME,
    /// Move object to another position
    MID_MOVETO,
    /// Change the color of an object
    MID_CHANGECOL,
    /// Change the type of an object
    MID_CHANGETYPE,
    /// Delete an object
    MID_DELETE,
    //@}

    //@{ IDs used by Trackers
    /// The Table
    MID_TABLE,
    /// A Simulation Step was performed
    MID_SIMSTEP,
    /// A Tracker shall be opened
    MID_OPENTRACKER,
    //@}

    //@{ General Setting Dialogs - IDs
    /// Ok-Button was pushed
    MID_SETTINGS_OK,
    /// Cancel-Button was pushed
    MID_SETTINGS_CANCEL,
    /// Save-Button was pushed
    MID_SETTINGS_SAVE,
    //@}

    //@{ Application Settings - Dialog IDs
    /// Close Simulation at End - Option
    MID_QUITONSIMEND,
    /// Suppress End Message - Option
    MID_SURPRESSENDINFO,
    /// Allow aggregated Views - Option
    MID_ALLOWAGGREGATED,
    /// Allow floating aggregated Views - Option
    MID_ALLOWAFLOATING,
    /// Allow textures - Option
    MID_ALLOWTEXTURES,
    //@}

    //@{ GLObjChooser - Dialog IDs
    /// Center object
    MID_CHOOSER_CENTER,
    /// Text entry
    MID_CHOOSER_TEXT,
    //@}

    //@{ GLChosenEditor - Dialog IDs
    /// Load set
    MID_CHOOSEN_LOAD,
    /// Save set
    MID_CHOOSEN_SAVE,
    /// Clear set
    MID_CHOOSEN_CLEAR,
    /// Deselect selected items
    MID_CHOOSEN_DESELECT,
    //@}


    MID_CLEARPREVIOUS,
    MID_SUBMIT,
    MID_PROBABILITY,
    MID_LEVEL,
    MID_PERIOD,
    MID_TIME_SLIDER,

    //@{ IDs for netedit
    /// Import a network - Menu entry
    MID_IMPORT_NET,
    /// Open a bitmap file - Menu entry
    MID_LOAD_IMAGE,
    /// Write a loaded bitmap file - Menu entry
    MID_SAVE_IMAGE,
    /// ID for the imageviewer
    MID_MAP_IMAGEVIEWER,
    /// Save generated edges and nodes - Menu entry
    MID_SAVE_EDGES_NODES,
    /// Save generated SUMO-network - Menu entry
    MID_SAVE_NET,
    //@}

    MID_SIMPLE_VIEW_COLORCHANGE,
    MID_SIMPLE_VIEW_NAMECHANGE,


    MID_CUTSWELL,

    MID_LAST
};


#endif

/****************************************************************************/

