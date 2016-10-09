/****************************************************************************/
/// @file    GUIAppEnum.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// Message and object IDs used by the FOX-version of the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIAppEnum_h
#define GUIAppEnum_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// enumeration
// ===========================================================================
/**
 * @brief Message and object IDs used by the FOX-version of the gui
 */
enum {
    /// Show about - dialog
    ID_ABOUT = FXMainWindow::ID_LAST,

    /// @name application specific
    //@{

    /// Main window-ID
    MID_WINDOW,
    /// Main window closes
    MID_QUIT,
    //@}


    /// @name for common buttons
    //@{

    /// Ok-button pressed
    MID_OK,
    /// Cancel-button pressed
    MID_CANCEL,
    //@}


    /// @name Main Window File Menu - IDs
    //@{

    /// Open configuration - ID
    MID_OPEN_CONFIG,
    /// Open network - ID
    MID_OPEN_NETWORK,
    /// Load additional file with poi and polygons
    MID_OPEN_SHAPES,
    /// Load additional file with additional elements
    MID_OPEN_ADDITIONALS,
    /// Reload the previously loaded simulation
    MID_RELOAD,
    /// Loads a file previously loaded
    MID_RECENTFILE,
    /// Close simulation - ID
    MID_CLOSE,
    /// About SUMO - ID
    MID_ABOUT,
    //@}


    /// @name Main Window Simulation Control - IDs
    //@{

    /// Start the simulation
    MID_START,
    /// Stop the simulation
    MID_STOP,
    /// Perform a single simulation step
    MID_STEP,
    //@}


    /// @name Main window windows control - IDs
    //@{

    /// Open a new microscopic view
    MID_NEW_MICROVIEW,
    /// Open a new microscopic 3D view
    MID_NEW_OSGVIEW,
    //@}


    /// @name SubThread - IDs
    //@{

    /// The loading thread
    ID_LOADTHREAD_EVENT,
    /// The Simulation execution thread
    ID_RUNTHREAD_EVENT,
    /// ID for message passing between threads
    ID_THREAD_EVENT,
    //@}


    /// @name Edit Menu Item - IDs
    //@{

    /// Open editor for selections
    MID_EDITCHOSEN,
    /// Edit simulation breakpoints
    MID_EDIT_BREAKPOINTS,
    /// Open in netedit
    MID_NETEDIT,
    //@}


    /// @name Settings Menu Item - IDs
    //@{

    /// Application settings - menu entry
    MID_APPSETTINGS,
    /// Gaming mode - menu entry
    MID_GAMING,
    /// Fullscreen mode - menu entry
    MID_FULLSCREEN,
    /// Locator configuration - menu entry
    MID_LISTINTERNAL,
    /// The Simulation delay control
    MID_SIMDELAY,
    //@}


    /// Clear simulation output
    MID_CLEARMESSAGEWINDOW,
    /// Show network statistics
    MID_SHOWNETSTATS,
    /// Show vehicle statistics
    MID_SHOWVEHSTATS,
    /// Show person statistics
    MID_SHOWPERSONSTATS,


    /// @name Common View Settings - IDs
    //@{

    /// Recenter view - button
    MID_RECENTERVIEW,
    /// Allow rotation - button
    MID_ALLOWROTATION,
    /// Locate junction - button
    MID_LOCATEJUNCTION,
    /// Locate edge - button
    MID_LOCATEEDGE,
    /// Locate vehicle - button
    MID_LOCATEVEHICLE,
    /// Locate person - button
    MID_LOCATEPERSON,
    /// Locate TLS - button
    MID_LOCATETLS,
    /// Locate BusStop - button
    MID_LOCATEADDITIONAL_BUSSTOP,
    /// Locate ChargingStation - button
    MID_LOCATEADDITIONAL_CHARGINGSTATION,
    /// Locate addtional structure - button
    MID_LOCATEADD,
    /// Locate poi - button
    MID_LOCATEPOI,
    /// Locate polygons - button
    MID_LOCATEPOLY,
    /// Open viewport editor - button
    MID_EDITVIEWPORT,
    /// Open view editor - button
    MID_EDITVIEWSCHEME,
    /// Show tool tips - button
    MID_SHOWTOOLTIPS,
    /// Make snapshot - button
    MID_MAKESNAPSHOT,
    /// toogle zooming style
    MID_ZOOM_STYLE,
    /// toogle time display mode
    MID_TIME_TOOGLE,
    /// toogle delay between alternative value
    MID_DELAY_TOOGLE,
    //@}


    /// @name Common view IDs
    //@{

    /// GLCanvas - ID
    MID_GLCANVAS,
    /// chore
    MID_CHORE,
    //@}


    /// @name View settings - IDs
    //@{

    /// Change coloring scheme - combo
    MID_COLOURSCHEMECHANGE,
    //@}


    /// @name Object PopUp Entries - IDs
    //@{

    /// Center view to object - popup entry
    MID_CENTER,
    /// Copy object name - popup entry
    MID_COPY_NAME,
    /// Copy typed object name - popup entry
    MID_COPY_TYPED_NAME,
    /// Copy edge name (for lanes only)
    MID_COPY_EDGE_NAME,
    /// Copy cursor position - popup entry
    MID_COPY_CURSOR_POSITION,
    /// Copy cursor geo-coordinate position - popup entry
    MID_COPY_CURSOR_GEOPOSITION,
    /// Show object parameter - popup entry
    MID_SHOWPARS,
    /// Show object type parameter - popup entry
    MID_SHOWTYPEPARS,
    /// Show TLS phases - popup entry
    MID_SWITCH_OFF,
    MID_SWITCH,
    MID_SHOWPHASES = MID_SWITCH + 20,
    /// Begin to track phases - menu entry
    MID_TRACKPHASES,
    /// Add to selected items - menu entry
    MID_ADDSELECT,
    /// Remove from selected items - Menu Etry
    MID_REMOVESELECT,
    /// Open the object's manipulator
    MID_MANIP,
    /// Draw a vehicle's route
    MID_DRAWROUTE,
    /// Show vehicle's current route
    MID_SHOW_CURRENTROUTE,
    /// Hide vehicle's current route
    MID_HIDE_CURRENTROUTE,
    /// Show vehicle's best lanes
    MID_SHOW_BEST_LANES,
    /// Hide vehicle's best lanes
    MID_HIDE_BEST_LANES,
    /// Show all vehicle's routes
    MID_SHOW_ALLROUTES,
    /// Hide all vehicle's routes
    MID_HIDE_ALLROUTES,
    /// Start to track a vehicle
    MID_START_TRACK,
    /// Stop to track a vehicle
    MID_STOP_TRACK,
    /// select foes of a vehicle
    MID_SHOW_FOES,
    MID_SHOW_LFLINKITEMS,
    MID_HIDE_LFLINKITEMS,
    /// Show persons's path on walkingarea
    MID_SHOW_WALKINGAREA_PATH,
    /// Hide persons's path on walkingarea
    MID_HIDE_WALKINGAREA_PATH,
    //@}


    /// @name IDs used by Trackers
    //@{

    /// The Table
    MID_TABLE,
    /// A Simulation step was performed
    MID_SIMSTEP,
    /// A Tracker shall be opened
    MID_OPENTRACKER,
    //@}


    /// @name General Setting Dialogs - IDs
    //@{

    /// Ok-button was pushed
    MID_SETTINGS_OK,
    /// Cancel-button was pushed
    MID_SETTINGS_CANCEL,
    //@}


    /// @name Application Settings - Dialog IDs
    //@{

    /// Close simulation at end - Option
    MID_QUITONSIMEND,
    /// Start simulation when loaded - Option
    MID_AUTOSTART,
    /// Demo mode - Option
    MID_DEMO,
    /// Allow textures - Option
    MID_ALLOWTEXTURES,
    /// Locate links in messages - Option
    MID_LOCATELINKS,
    //@}


    /// @name GLObjChooser - Dialog IDs
    //@{

    /// Center object
    MID_CHOOSER_CENTER,
    /// Text entry
    MID_CHOOSER_TEXT,
    /// Object list
    MID_CHOOSER_LIST,
    /// Filter selected
    MID_CHOOSER_FILTER,
    //@}


    /// @name GLChosenEditor - Dialog IDs
    //@{

    /// set subset of elements
    MID_CHOOSEN_ELEMENTS, 
    /// Load set
    MID_CHOOSEN_LOAD,
    /// Save set
    MID_CHOOSEN_SAVE,
    /// Clear set
    MID_CHOOSEN_CLEAR,
    /// Reset set
    MID_CHOOSEN_RESET,
    /// Deselect selected items
    MID_CHOOSEN_DESELECT,
    /// Deselect selected items
    MID_CHOOSEN_INVERT,
    //@}


    /// @name IDs for visualization settings - Dialog
    //@{

    /// Informs the dialog about a value's change
    MID_SIMPLE_VIEW_COLORCHANGE,
    /// Informs the dialog about switching to another scheme
    MID_SIMPLE_VIEW_NAMECHANGE,
    /// For the save-to-db - button
    MID_SIMPLE_VIEW_SAVE,
    /// For the delete - button
    MID_SIMPLE_VIEW_DELETE,
    /// For the export-to-file - button
    MID_SIMPLE_VIEW_EXPORT,
    /// For the import-from-file - button
    MID_SIMPLE_VIEW_IMPORT,
    /// For the load-decals - button
    MID_SIMPLE_VIEW_LOAD_DECALS,
    /// For the save-decals - button
    MID_SIMPLE_VIEW_SAVE_DECALS,
    //@}

    /** help button */
    MID_HELP,

    /// @name dynamic interaction with the simulation via SUMO-GUI
    //@{
    MID_CLOSE_LANE,
    MID_CLOSE_EDGE,
    MID_ADD_REROUTER,
    //@}


    /// @name NETEDIT Message IDs
    //@{
    /** abort current edit operation */
    MID_GNE_ABORT,
    /** hot key <DEL> */
    MID_GNE_HOTKEY_DEL,
    /** hot key <ENTER> */
    MID_GNE_HOTKEY_ENTER,

    /** mode has changed */
    MID_GNE_MODE_CHANGE,

    /** mode has changed */
    MID_GNE_MODEADDITIONAL_CHANGE,

    /** mode for adding edges */
    MID_GNE_MODE_CREATE_EDGE,
    /** mode for moving things */
    MID_GNE_MODE_MOVE,
    /** mode for deleting things */
    MID_GNE_MODE_DELETE,
    /** mode for inspecting object attributes */
    MID_GNE_MODE_INSPECT,
    /** mode for selecting objects */
    MID_GNE_MODE_SELECT,
    /** mode for connecting lanes */
    MID_GNE_MODE_CONNECT,
    /** mode for editing tls */
    MID_GNE_MODE_TLS,
    /** mode for editing additional */
    MID_GNE_MODE_ADDITIONAL,

    /// selector match box messages
    MID_GNE_SELMB_TAG,
    MID_GNE_SELMB_STRING,

    /// additional match box messages
    MID_GNE_MODE_ADDITIONAL_ITEM,
    MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,
    MID_GNE_MODE_ADDITIONAL_FORCEPOSITION,
    MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT,
    MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL,

    /// Inspector attribute dialog
    MID_GNE_MODE_INSPECT_ACCEPT,
    MID_GNE_MODE_INSPECT_CANCEL,
    MID_GNE_MODE_INSPECT_RESET,

    /// Variable Speed Signal dialog
    MID_GNE_VARIABLESPEEDSIGNAL_ADDROW,
    MID_GNE_VARIABLESPEEDSIGNAL_REMOVEROW,
    MID_GNE_VARIABLESPEEDSIGNAL_CHANGEVALUE,

    /// Rerouter dialog
    MID_GNE_REROUTER_ADDROW,
    MID_GNE_REROUTER_REMOVEROW,
    MID_GNE_REROUTER_CHANGEVALUE,

    /// Calibrator dialog
    MID_GNE_CALIBRATOR_ADDROW,
    MID_GNE_CALIBRATOR_REMOVEROW,
    MID_GNE_CALIBRATOR_CHANGEVALUE,

    /// Additional dialogs
    MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,
    MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,
    MID_GNE_MODE_ADDITIONALDIALOG_RESET,

    /// processing menu messages
    MID_GNE_COMPUTE_JUNCTIONS,
    MID_GNE_CLEAN_JUNCTIONS,
    MID_GNE_JOIN_JUNCTIONS,
    MID_GNE_OPTIONS,

    /// tls mode messages
    /** replace program with a newly guessed program */
    MID_GNE_GUESS_PROGRAM,
    /** definition related controls */
    MID_GNE_DEF_OFFSET,
    MID_GNE_DEF_SWITCH,
    MID_GNE_DEF_CREATE,
    MID_GNE_DEF_DELETE,
    MID_GNE_DEF_RENAME,
    MID_GNE_DEF_SUBRENAME,
    MID_GNE_DEF_ADDOFF,
    /** phase related controls */
    MID_GNE_PHASE_TABLE,
    MID_GNE_PHASE_CREATE,
    MID_GNE_PHASE_DELETE,

    /// misc messages
    MID_GNE_NEW_NETWORK,
    MID_GNE_OPEN_FOREIGN,
    MID_GNE_VIS_HEIGHT,
    MID_GNE_LOAD_POIS,
    // MID_GNE_WIZARD,
    /** save network*/
    MID_GNE_SAVE_NETWORK,
    MID_GNE_SAVE_AS_NETWORK,
    MID_GNE_SAVE_PLAIN_XML,
    MID_GNE_SAVE_JOINED,
    MID_GNE_SAVE_POIS,
    /** Save additionals **/
    MID_GNE_SAVE_ADDITIONALS,
    MID_GNE_SAVE_ADDITIONALS_AS,
    /** insert contents of another network */
    MID_GNE_INSERT_NETWORK,
    /** show connections **/
    MID_GNE_SHOW_CONNECTIONS, 
    /** attribute edited */
    MID_GNE_SET_ATTRIBUTE,
    /** netEdit attribute blocking */
    MID_GNE_SET_BLOCKING,
    /** netEdit show connection */
    MID_GNE_SHOW_CONNECTION,
    /** netEdit inspect connection */ 
    MID_GNE_INSPECT_CONNECTION, 
    /** netEdit go back to the previous element */
    MID_GNE_INSPECT_GOBACK, 
    /** netEdit add row */
    MID_GNE_ADDROW,
    /** netEdit remove row */
    MID_GNE_REMOVEROW,
    /** netEdit add set */
    MID_GNE_ADDSET,
    /** netEdit remove set */
    MID_GNE_REMOVESET,
    /** netEdit search additionalSet */
    MID_GNE_SEARCHADDITIONALSET,
    /** netEdit select additionalSet */
    MID_GNE_SELECTADDITIONALSET,
    /** netEdit search edge */
    MID_GNE_SEARCHEDGE,
    /** netEdit use selected edge */
    MID_GNE_USESELECTEDEDGES,
    /** netEdit select edge */
    MID_GNE_SELECTEDGE,
    /** netEdit clear selection of edges */
    MID_GNE_CLEAREDGESELECTION,
    /** netEdit invert selection of edges */
    MID_GNE_INVERTEDGESELECTION,
    /** netEdit search lane */
    MID_GNE_SEARCHLANE,
    /** netEdit use selected lanes */
    MID_GNE_USESELECTEDLANES,
    /** netEdit select lane */
    MID_GNE_SELECTLANE,
    /** netEdit clear selection of lanes */
    MID_GNE_CLEARLANESELECTION,
    /** netEdit invert selection of lanes */
    MID_GNE_INVERTLANESELECTION,
    /** open edit dialog */
    MID_GNE_OPEN_ATTRIBUTE_EDITOR,
    /** split an edge */
    MID_GNE_SPLIT_EDGE,
    /** split an edge */
    MID_GNE_SPLIT_EDGE_BIDI,
    /** reverse an edge */
    MID_GNE_REVERSE_EDGE,
    /** add reverse edge */
    MID_GNE_ADD_REVERSE_EDGE,
    /** edit junction shape */
    MID_GNE_NODE_SHAPE,
    /** turn junction into geometry node */
    MID_GNE_NODE_REPLACE,
    /** set non-default geometry endpoint */
    MID_GNE_SET_EDGE_ENDPOINT,
    /** restore geometry endpoint to node position */
    MID_GNE_RESET_EDGE_ENDPOINT,
    /** remove inner geometry */
    MID_GNE_STRAIGHTEN,
    /** simplify polygon geometry */
    MID_GNE_SIMPLIFY_SHAPE,
    /** delete geometry point */
    MID_GNE_DELETE_GEOMETRY,
    /** duplicate a lane */
    MID_GNE_DUPLICATE_LANE,
    /** transform lane to sidewalk */
    MID_GNE_TRANSFORM_LANE_SIDEWALK,
    /** transform lane to bikelane */
    MID_GNE_TRANSFORM_LANE_BIKE,
    /** transform lane to busLane */
    MID_GNE_TRANSFORM_LANE_BUS,
    /** revert transformation */
    MID_GNE_REVERT_TRANSFORMATION,
    /** add sidewalk */
    MID_GNE_ADD_LANE_SIDEWALK,
    /** add bikelane */
    MID_GNE_ADD_LANE_BIKE,
    /** add busLane */
    MID_GNE_ADD_LANE_BUS,
    /** remove sidewalk */
    MID_GNE_REMOVE_LANE_SIDEWALK,
    /** remove bikelane */
    MID_GNE_REMOVE_LANE_BIKE,
    /** remove busLane */
    MID_GNE_REMOVE_LANE_BUS,
    /** set object as template*/
    MID_GNE_SET_TEMPLATE,
    /** copy template*/
    MID_GNE_COPY_TEMPLATE,
    /** remove template*/
    MID_GNE_REMOVE_TEMPLATE,
    /** select dead end lanes */
    MID_GNE_SELECT_DEAD_ENDS,
    /** select lanes that have no connection leading to it */
    MID_GNE_SELECT_DEAD_STARTS,
    /** select lanes that are connected from concurrent lanes */
    MID_GNE_SELECT_CONFLICTS,
    /** select lanes with connections that have the pass attribute set to 'true' */
    MID_GNE_SELECT_PASS,
    /** changes the visual scaling of selected items */
    MID_GNE_SELECT_SCALE,

    //@}

    MID_LAST
};


#endif

/****************************************************************************/

