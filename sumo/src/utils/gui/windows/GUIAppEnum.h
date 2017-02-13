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
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
    /// @brief Show about - dialog
    ID_ABOUT = FXMainWindow::ID_LAST,

    /// @name application specific
    /// @{

    /// @brief Main window-ID
    MID_WINDOW,
    /// @brief Main window closes
    MID_QUIT,
    /// @}


    /// @name for common buttons
    /// @{

    /// @brief Ok-button pressed
    MID_OK,
    /// @brief Cancel-button pressed
    MID_CANCEL,
    /// @}


    /// @name Main Window File Menu - IDs
    /// @{

    /// @brief Open configuration - ID
    MID_OPEN_CONFIG,
    /// @brief Open network - ID
    MID_OPEN_NETWORK,
    /// @brief Load additional file with poi and polygons
    MID_OPEN_SHAPES,
    /// @brief Load additional file with additional elements
    MID_OPEN_ADDITIONALS,
    /// @brief Reload the previously loaded simulation
    MID_RELOAD,
    /// @brief Loads a file previously loaded
    MID_RECENTFILE,
    /// @brief Close simulation - ID
    MID_CLOSE,
    /// @brief About SUMO - ID
    MID_ABOUT,
    /// @}


    /// @name Main Window Simulation Control - IDs
    /// @{

    /// @brief Start the simulation
    MID_START,
    /// @brief Stop the simulation
    MID_STOP,
    /// @brief Perform a single simulation step
    MID_STEP,
    /// @}


    /// @name Main window windows control - IDs
    /// @{

    /// @brief Open a new microscopic view
    MID_NEW_MICROVIEW,
    /// @brief Open a new microscopic 3D view
    MID_NEW_OSGVIEW,
    /// @}


    /// @name SubThread - IDs
    /// @{

    /// @brief The loading thread
    ID_LOADTHREAD_EVENT,
    /// @brief The Simulation execution thread
    ID_RUNTHREAD_EVENT,
    /// @brief ID for message passing between threads
    ID_THREAD_EVENT,
    /// @}


    /// @name Edit Menu Item - IDs
    /// @{

    /// @brief Open editor for selections
    MID_EDITCHOSEN,
    /// @brief Edit simulation breakpoints
    MID_EDIT_BREAKPOINTS,
    /// @brief Open in netedit
    MID_NETEDIT,
    /// @}


    /// @name Settings Menu Item - IDs
    /// @{

    /// @brief Application settings - menu entry
    MID_APPSETTINGS,
    /// @brief Gaming mode - menu entry
    MID_GAMING,
    /// @brief Fullscreen mode - menu entry
    MID_FULLSCREEN,
    /// @brief Locator configuration - menu entry
    MID_LISTINTERNAL,
    MID_LISTPARKING,
    MID_LISTTELEPORTING,
    /// @brief The Simulation delay control
    MID_SIMDELAY,
    /// @}


    /// @brief Clear simulation output
    MID_CLEARMESSAGEWINDOW,
    /// @brief Show network statistics
    MID_SHOWNETSTATS,
    /// @brief Show vehicle statistics
    MID_SHOWVEHSTATS,
    /// @brief Show person statistics
    MID_SHOWPERSONSTATS,


    /// @name Common View Settings - IDs
    /// @{

    /// @brief Recenter view - button
    MID_RECENTERVIEW,
    /// @brief Allow rotation - button
    MID_ALLOWROTATION,
    /// @brief Locate junction - button
    MID_LOCATEJUNCTION,
    /// @brief Locate edge - button
    MID_LOCATEEDGE,
    /// @brief Locate vehicle - button
    MID_LOCATEVEHICLE,
    /// @brief Locate person - button
    MID_LOCATEPERSON,
    /// @brief Locate TLS - button
    MID_LOCATETLS,
    /// @brief Locate BusStop - button
    MID_LOCATEADDITIONAL_BUSSTOP,
    /// @brief Locate ChargingStation - button
    MID_LOCATEADDITIONAL_CHARGINGSTATION,
    /// @brief Locate addtional structure - button
    MID_LOCATEADD,
    /// @brief Locate poi - button
    MID_LOCATEPOI,
    /// @brief Locate polygons - button
    MID_LOCATEPOLY,
    /// @brief Open viewport editor - button
    MID_EDITVIEWPORT,
    /// @brief Open view editor - button
    MID_EDITVIEWSCHEME,
    /// @brief Show tool tips - button
    MID_SHOWTOOLTIPS,
    /// @brief Make snapshot - button
    MID_MAKESNAPSHOT,
    /// @brief toogle zooming style
    MID_ZOOM_STYLE,
    /// @brief toogle time display mode
    MID_TIME_TOOGLE,
    /// @brief toogle delay between alternative value
    MID_DELAY_TOOGLE,
    /// @}


    /// @name Common view IDs
    /// @{

    /// @brief GLCanvas - ID
    MID_GLCANVAS,
    /// @brief chore
    MID_CHORE,
    /// @}


    /// @name View settings - IDs
    /// @{

    /// @brief Change coloring scheme - combo
    MID_COLOURSCHEMECHANGE,
    /// @}


    /// @name Object PopUp Entries - IDs
    /// @{

    /// @brief Center view to object - popup entry
    MID_CENTER,
    /// @brief Copy object name - popup entry
    MID_COPY_NAME,
    /// @brief Copy typed object name - popup entry
    MID_COPY_TYPED_NAME,
    /// @brief Copy edge name (for lanes only)
    MID_COPY_EDGE_NAME,
    /// @brief Copy cursor position - popup entry
    MID_COPY_CURSOR_POSITION,
    /// @brief Copy cursor geo-coordinate position - popup entry
    MID_COPY_CURSOR_GEOPOSITION,
    /// @brief Show object parameter - popup entry
    MID_SHOWPARS,
    /// @brief Show object type parameter - popup entry
    MID_SHOWTYPEPARS,
    /// @brief Show TLS phases - popup entry
    MID_SWITCH_OFF,
    MID_SWITCH,
    MID_SHOWPHASES = MID_SWITCH + 20,
    /// @brief Begin to track phases - menu entry
    MID_TRACKPHASES,
    /// @brief Add to selected items - menu entry
    MID_ADDSELECT,
    /// @brief Remove from selected items - Menu Etry
    MID_REMOVESELECT,
    /// @brief Open the object's manipulator
    MID_MANIP,
    /// @brief Draw a vehicle's route
    MID_DRAWROUTE,
    /// @brief Show vehicle's current route
    MID_SHOW_CURRENTROUTE,
    /// @brief Hide vehicle's current route
    MID_HIDE_CURRENTROUTE,
    /// @brief Show vehicle's best lanes
    MID_SHOW_BEST_LANES,
    /// @brief Hide vehicle's best lanes
    MID_HIDE_BEST_LANES,
    /// @brief Show all vehicle's routes
    MID_SHOW_ALLROUTES,
    /// @brief Hide all vehicle's routes
    MID_HIDE_ALLROUTES,
    /// @brief Start to track a vehicle
    MID_START_TRACK,
    /// @brief Stop to track a vehicle
    MID_STOP_TRACK,
    /// @brief select foes of a vehicle
    MID_SHOW_FOES,
    MID_SHOW_LFLINKITEMS,
    MID_HIDE_LFLINKITEMS,
    /// @brief Show persons's path on walkingarea
    MID_SHOW_WALKINGAREA_PATH,
    /// @brief Hide persons's path on walkingarea
    MID_HIDE_WALKINGAREA_PATH,
    /// @}


    /// @name IDs used by Trackers
    /// @{

    /// @brief The Table
    MID_TABLE,
    /// @brief A Simulation step was performed
    MID_SIMSTEP,
    /// @brief A Tracker shall be opened
    MID_OPENTRACKER,
    /// @}


    /// @name General Setting Dialogs - IDs
    /// @{

    /// @brief Ok-button was pushed
    MID_SETTINGS_OK,
    /// @brief Cancel-button was pushed
    MID_SETTINGS_CANCEL,
    /// @}


    /// @name Application Settings - Dialog IDs
    /// @{

    /// @brief Close simulation at end - Option
    MID_QUITONSIMEND,
    /// @brief Start simulation when loaded - Option
    MID_AUTOSTART,
    /// @brief Demo mode - Option
    MID_DEMO,
    /// @brief Allow textures - Option
    MID_ALLOWTEXTURES,
    /// @brief Locate links in messages - Option
    MID_LOCATELINKS,
    /// @}


    /// @name GLObjChooser - Dialog IDs
    /// @{

    /// @brief Center object
    MID_CHOOSER_CENTER,
    /// @brief Text entry
    MID_CHOOSER_TEXT,
    /// @brief Object list
    MID_CHOOSER_LIST,
    /// @brief Filter selected
    MID_CHOOSER_FILTER,
    /// @}


    /// @name GLChosenEditor - Dialog IDs
    /// @{

    /// @brief set subset of elements
    MID_CHOOSEN_ELEMENTS,
    /// @brief Load set
    MID_CHOOSEN_LOAD,
    /// @brief Save set
    MID_CHOOSEN_SAVE,
    /// @brief Clear set
    MID_CHOOSEN_CLEAR,
    /// @brief Reset set
    MID_CHOOSEN_RESET,
    /// @brief Deselect selected items
    MID_CHOOSEN_DESELECT,
    /// @brief Deselect selected items
    MID_CHOOSEN_INVERT,
    /// @}

    /// @name childs of elements in netedit
    /// @{

    /// @brief tree list with the childs
    MID_GNE_CHILDS,
    /// @brief In GNEDeleteFrame, center element
    MID_GNE_DELETEFRAME_CENTER,
    /// @brief In GNEDeleteFrame, inspect element
    MID_GNE_DELETEFRAME_INSPECT,
    /// @brief In GNEDeleteFrame, delete element
    MID_GNE_DELETEFRAME_DELETE,
    /// @brief tree list with the childs
    MID_GNEINSPECT_CHILDS,
    /// @brief In GNEInspectFrame, center element
    MID_GNE_INSPECTFRAME_CENTER,
    /// @brief In GNEInspectFrame, inspect element
    MID_GNE_INSPECTFRAME_INSPECT,
    /// @brief In GNEInspectFrame, delete element
    MID_GNE_INSPECTFRAME_DELETE,
    /// @brief automatically delete additional childs
    MID_GNE_AUTOMATICALLYDELETEADDITIONALS,
    /// @}


    /// @name IDs for visualization settings - Dialog
    /// @{

    /// @brief Informs the dialog about a value's change
    MID_SIMPLE_VIEW_COLORCHANGE,
    /// @brief Informs the dialog about switching to another scheme
    MID_SIMPLE_VIEW_NAMECHANGE,
    /// @brief For the save-to-db - button
    MID_SIMPLE_VIEW_SAVE,
    /// @brief For the delete - button
    MID_SIMPLE_VIEW_DELETE,
    /// @brief For the export-to-file - button
    MID_SIMPLE_VIEW_EXPORT,
    /// @brief For the import-from-file - button
    MID_SIMPLE_VIEW_IMPORT,
    /// @brief For the load-decals - button
    MID_SIMPLE_VIEW_LOAD_DECALS,
    /// @brief For the save-decals - button
    MID_SIMPLE_VIEW_SAVE_DECALS,
    /// @}

    ///@brief help button
    MID_HELP,

    /// @name dynamic interaction with the simulation via SUMO-GUI
    /// @{

    MID_CLOSE_LANE,
    MID_CLOSE_EDGE,
    MID_ADD_REROUTER,
    /// @}


    /// @name NETEDIT Message IDs
    /// @{

    /// @brief abort current edit operation
    MID_GNE_ABORT,
    /// @brief hot key <DEL>
    MID_GNE_HOTKEY_DEL,
    /// @brief hot key <ENTER>
    MID_GNE_HOTKEY_ENTER,
    /// @}

    /// @name NETEDIT edit modes
    /// @{

    /// @brief mode has changed
    MID_GNE_MODE_CHANGE,
    /// @brief mode has changed
    MID_GNE_MODEADDITIONAL_CHANGE,
    /// @brief mode for adding edges
    MID_GNE_MODE_CREATE_EDGE,
    /// @brief mode for moving things
    MID_GNE_MODE_MOVE,
    /// @brief mode for deleting things
    MID_GNE_MODE_DELETE,
    /// @brief mode for inspecting object attributes
    MID_GNE_MODE_INSPECT,
    /// @brief mode for selecting objects
    MID_GNE_MODE_SELECT,
    /// @brief mode for connecting lanes
    MID_GNE_MODE_CONNECT,
    /// @brief mode for editing tls
    MID_GNE_MODE_TLS,
    /// @brief mode for editing additional
    MID_GNE_MODE_ADDITIONAL,
    /// @brief mode for editing crossing
    MID_GNE_MODE_CROSSING,
    /// @}


    /// @brief selector match box messages
    MID_GNE_SELMB_TAG,
    MID_GNE_SELMB_STRING,

    /// @brief additional match box messages
    MID_GNE_MODE_ADDITIONAL_ITEM,
    MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,
    MID_GNE_MODE_ADDITIONAL_FORCEPOSITION,
    MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT,
    MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_DIAL,
    MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL,

    /// @brief Inspector attribute dialog
    MID_GNE_MODE_INSPECT_ACCEPT,
    MID_GNE_MODE_INSPECT_CANCEL,
    MID_GNE_MODE_INSPECT_RESET,

    /// @brief Additional dialogs
    MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,
    MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,
    MID_GNE_MODE_ADDITIONALDIALOG_RESET,

    /// @brief Variable Speed Signal dialog
    MID_GNE_VARIABLESPEEDSIGN_ADDROW,
    MID_GNE_VARIABLESPEEDSIGN_REMOVEROW,
    MID_GNE_VARIABLESPEEDSIGN_CHANGEVALUE,

    /// @brief Rerouter dialog
    MID_GNE_REROUTEDIALOG_TABLE_INTERVAL,
    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE,
    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,
    MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,
    MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,
    MID_GNE_REROUTEDIALOG_ADD_INTERVAL,
    MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE,
    MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE,
    MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE,
    MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE,
    MID_GNE_REROUTEDIALOG_CHANGESTART,
    MID_GNE_REROUTEDIALOG_CHANGEEND,

    /// @brief Calibrator dialog
    MID_GNE_MODE_CALIBRATOR_TABLE,
    MID_GNE_MODE_CALIBRATOR_EDITVALUE,


    /// @brief processing menu messages
    MID_GNE_COMPUTE_JUNCTIONS,
    MID_GNE_CLEAN_JUNCTIONS,
    MID_GNE_JOIN_JUNCTIONS,
    MID_GNE_OPTIONS,

    /// @name tls mode messages
    /// @{

    /// @brief replace program with a newly guessed program
    MID_GNE_GUESS_PROGRAM,
    /// @brief definition related controls
    MID_GNE_DEF_OFFSET,
    MID_GNE_DEF_SWITCH,
    MID_GNE_DEF_CREATE,
    MID_GNE_DEF_DELETE,
    MID_GNE_DEF_RENAME,
    MID_GNE_DEF_SUBRENAME,
    MID_GNE_DEF_ADDOFF,
    /// @}

    /// @name phase related controls
    /// @{
    /// @brief select phase thable
    MID_GNE_PHASE_TABLE,
    /// @brief create phase thable
    MID_GNE_PHASE_CREATE,
    /// @brief delete  phase thable
    MID_GNE_PHASE_DELETE,
    /// @}

    /// @brief misc messages
    MID_GNE_NEW_NETWORK,
    MID_GNE_OPEN_FOREIGN,
    MID_GNE_VIS_HEIGHT,
    MID_GNE_LOAD_POIS,
    // MID_GNE_WIZARD,
    /// @brief save network
    MID_GNE_SAVE_NETWORK,
    MID_GNE_SAVE_AS_NETWORK,
    MID_GNE_SAVE_PLAIN_XML,
    MID_GNE_SAVE_JOINED,
    MID_GNE_SAVE_POIS,
    /// @brief Save additionals
    MID_GNE_SAVE_ADDITIONALS,
    MID_GNE_SAVE_ADDITIONALS_AS,
    /// @brief insert contents of another network
    MID_GNE_INSERT_NETWORK,
    /// @brief show connections
    MID_GNE_SHOW_CONNECTIONS,
    /// @brief show junctions as bubbles
    MID_GNE_SHOW_BUBBLES,
    /// @brief attribute edited
    MID_GNE_SET_ATTRIBUTE,
    /// @brief attribute blocking
    MID_GNE_SET_BLOCKING,
    /// @brief show connection
    MID_GNE_SHOW_CONNECTION,
    /// @brief inspect connection
    MID_GNE_INSPECT_CONNECTION,
    /// @brief go back to the previous element
    MID_GNE_INSPECT_GOBACK,
    /// @brief add row
    MID_GNE_ADDROW,
    /// @brief remove row
    MID_GNE_REMOVEROW,
    /// @brief add set
    MID_GNE_ADDSET,
    /// @brief remove set
    MID_GNE_REMOVESET,
    /// @brief search additional parent
    MID_GNE_SEARCHADDITIONALPARENT,
    /// @brief select additional parent
    MID_GNE_SELECTADDITIONALPARENT,
    /// @brief search edge
    MID_GNE_SEARCHEDGE,
    /// @brief show only selected edge in a list
    MID_GNE_SHOWONLYSELECTEDEDGES,
    /// @brief use selected edges only
    MID_GNE_USEONLYSELECTEDEDGES,
    /// @brief select edge
    MID_GNE_SELECTEDGE,
    /// @brief clear selection of edges
    MID_GNE_CLEAREDGESELECTION,
    /// @brief invert selection of edges
    MID_GNE_INVERTEDGESELECTION,
    /// @brief search lane
    MID_GNE_SEARCHLANE,
    /// @brief use selected lanes
    MID_GNE_USESELECTEDLANES,
    /// @brief select lane
    MID_GNE_SELECTLANE,
    /// @brief clear selection of lanes
    MID_GNE_CLEARLANESELECTION,
    /// @brief invert selection of lanes
    MID_GNE_INVERTLANESELECTION,
    /// @brief open edit dialog
    MID_GNE_OPEN_ATTRIBUTE_EDITOR,
    /// @brief split an edge
    MID_GNE_SPLIT_EDGE,
    /// @brief split an edge
    MID_GNE_SPLIT_EDGE_BIDI,
    /// @brief reverse an edge
    MID_GNE_REVERSE_EDGE,
    /// @brief add reverse edge
    MID_GNE_ADD_REVERSE_EDGE,
    /// @brief edit junction shape
    MID_GNE_NODE_SHAPE,
    /// @brief turn junction into geometry node
    MID_GNE_NODE_REPLACE,
    /// @brief set non-default geometry endpoint
    MID_GNE_SET_EDGE_ENDPOINT,
    /// @brief restore geometry endpoint to node position
    MID_GNE_RESET_EDGE_ENDPOINT,
    /// @brief remove inner geometry
    MID_GNE_STRAIGHTEN,
    /// @brief simplify polygon geometry
    MID_GNE_SIMPLIFY_SHAPE,
    /// @brief delete geometry point
    MID_GNE_DELETE_GEOMETRY,
    /// @brief duplicate a lane
    MID_GNE_DUPLICATE_LANE,
    /// @brief transform lane to sidewalk
    MID_GNE_TRANSFORM_LANE_SIDEWALK,
    /// @brief transform lane to bikelane
    MID_GNE_TRANSFORM_LANE_BIKE,
    /// @brief transform lane to busLane
    MID_GNE_TRANSFORM_LANE_BUS,
    /// @brief revert transformation
    MID_GNE_REVERT_TRANSFORMATION,
    /// @brief add sidewalk
    MID_GNE_ADD_LANE_SIDEWALK,
    /// @brief add bikelane
    MID_GNE_ADD_LANE_BIKE,
    /// @brief add busLane
    MID_GNE_ADD_LANE_BUS,
    /// @brief remove sidewalk
    MID_GNE_REMOVE_LANE_SIDEWALK,
    /// @brief remove bikelane
    MID_GNE_REMOVE_LANE_BIKE,
    /// @brief remove busLane
    MID_GNE_REMOVE_LANE_BUS,
    /// @brief set object as template
    MID_GNE_SET_TEMPLATE,
    /// @brief copy template
    MID_GNE_COPY_TEMPLATE,
    /// @brief remove template
    MID_GNE_REMOVE_TEMPLATE,
    /// @brief select dead end lanes
    MID_GNE_SELECT_DEAD_ENDS,
    /// @brief select lanes that have no connection leading to it
    MID_GNE_SELECT_DEAD_STARTS,
    /// @brief select lanes that are connected from concurrent lanes
    MID_GNE_SELECT_CONFLICTS,
    /// @brief select lanes with connections that have the pass attribute set to 'true'
    MID_GNE_SELECT_PASS,
    /// @brief changes the visual scaling of selected items
    MID_GNE_SELECT_SCALE,
    /// @brief Create crossing
    MID_GNE_CREATE_CROSSING,
    /// @brief Size of frame area updated
    MID_GNE_SIZEOF_FRAMEAREAWIDTH_UPDATED,
    /// @}

    MID_LAST
};


#endif

/****************************************************************************/

