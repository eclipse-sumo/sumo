#ifndef GUIAppEnum_h
#define GUIAppEnum_h
//---------------------------------------------------------------------------//
//                        GUIAppEnum.h -
//  Message and object IDs used by the FOX-version of the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 08.03.2004
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
// Revision 1.12  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.11  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:11:14  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/07/12 12:52:07  dkrajzew
// code style adapted
//
// Revision 1.7  2005/05/04 09:22:51  dkrajzew
// adding of surrounding lanes to selection added; entries for viewport definition added
//
// Revision 1.6  2005/02/01 10:13:17  dkrajzew
// added missing enums for netedit
//
// Revision 1.5  2005/01/27 14:35:22  dkrajzew
// added the netedit-window
//
// Revision 1.4  2004/12/16 12:21:39  dkrajzew
// debugging
//
// Revision 1.3  2004/12/12 17:23:59  agaubatz
// Editor Tool Widgets included
//
// Revision 1.2  2004/12/02 13:54:23  agaubatz
// Netedit update, A. Gaubatz
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.3  2004/11/22 12:55:31  dksumo
// 'netedit' values added
//
// Revision 1.2  2004/10/29 06:21:22  dksumo
// added a slider enumeration value
//
// Revision 1.1  2004/10/22 12:50:57  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2004/08/02 13:14:10  dkrajzew
// clear message window and coloring scheme changes applied
//
// Revision 1.3  2004/07/02 08:10:56  dkrajzew
// edition of breakpoints and additional weights added
//
// Revision 1.2  2004/04/02 11:07:17  dkrajzew
// item selection menu entry ids added
//
// Revision 1.1  2004/03/19 12:56:10  dkrajzew
// porting to FOX
//
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


/* =========================================================================
 * enumeration
 * ======================================================================= */
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
#ifdef HAVE_MESOSIM
    /// Open a new edge meso View
    MID_NEW_MESOVIEW,
#endif
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
    /// Show Full Geom - Button
    MID_SHOWFULLGEOM,
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
    /// Show Object Parameter - Menu Entry
    MID_SHOWPARS,
    /// Show TLS phases - Menu Entry
    MID_SHOWPHASES,
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
    /// Show vehicle's current route
    MID_SHOW_CURRENTROUTE,
    /// Hide vehicle's current route
    MID_HIDE_CURRENTROUTE,
    /// Show all vehicle's routes
    MID_SHOW_ALLROUTES,
    /// Hide all vehicle's routes
    MID_HIDE_ALLROUTES,
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
    /// Allow SUMORealing aggregated Views - Option
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

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
