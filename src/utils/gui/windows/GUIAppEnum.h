/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
    /// @brief open additional dialog (used in netedit)
    MID_OPEN_ADDITIONAL_DIALOG,
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

    /// @brief set type of selection
    MID_CHOOSEN_OPERATION,
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

    /// NETEDIT

    /// @brief tree list with the childs
    MID_GNE_DELETEFRAME_CHILDS,
    /// @brief In GNEDeleteFrame, center element
    MID_GNE_DELETEFRAME_CENTER,
    /// @brief In GNEDeleteFrame, inspect element
    MID_GNE_DELETEFRAME_INSPECT,
    /// @brief In GNEDeleteFrame, delete element
    MID_GNE_DELETEFRAME_DELETE,
    /// @brief delete only geometry points
    MID_GNE_DELETEFRAME_ONLYGEOMETRYPOINTS,
    /// @brief automatically delete additional childs
    MID_GNE_DELETEFRAME_AUTODELETEADDITIONALS,
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


    /// @name NETEDIT hotkeys
    /// @{

    /// @brief hot key <ESC> abort current edit operation
    MID_GNE_HOTKEY_ESC,
    /// @brief hot key <F12> focus upper element of current frame
    MID_GNE_HOTKEY_FOCUSFRAME,
    /// @brief hot key <DEL> delete selections or elements
    MID_GNE_HOTKEY_DEL,
    /// @brief hot key <ENTER> accept current operation
    MID_GNE_HOTKEY_ENTER,
    /// @}


    /// @name Toolbar file messages
    /// @{

    /// @brief create new empty newtork
    MID_GNE_TOOLBARFILE_NEWNETWORK,
    /// @brief open foreign network
    MID_GNE_TOOLBARFILE_OPENFOREIGN,
    /// @brief save newtork
    MID_GNE_TOOLBARFILE_SAVENETWORK,
    /// @brief save newtwork as
    MID_GNE_TOOLBARFILE_SAVENETWORK_AS,
    /// @brief save network as plain XML
    MID_GNE_TOOLBARFILE_SAVEPLAINXML,
    /// @brief save joined junctions
    MID_GNE_TOOLBARFILE_SAVEJOINED,
    /// @brief Save shapes
    MID_GNE_TOOLBARFILE_SAVESHAPES,
    /// @brief save shapes as
    MID_GNE_TOOLBARFILE_SAVESHAPES_AS,
    /// @brief Save additionals
    MID_GNE_TOOLBARFILE_SAVEADDITIONALS,
    /// @brief save additionals as
    MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,
    /// @}


    /// @name Toolbar processing messages
    /// @{

    /// @brief compute junctions
    MID_GNE_PROCESSING_COMPUTEJUNCTIONS,
    /// @brief compute junctions with volatile options
    MID_GNE_PROCESSING_COMPUTEJUNCTIONS_VOLATILE,
    /// @brief clean junctions without edges
    MID_GNE_PROCESSING_CLEANJUNCTIONS,
    /// @brief join selected junctions
    MID_GNE_PROCESSING_JOINJUNCTIONS,
    /// @brief clean invalid crossings
    MID_GNE_PROCESSING_CLEANINVALIDCROSSINGS,
    /// @brief open options menu
    MID_GNE_PROCESSING_OPTIONS,
    /// @}


    /// @name Toolbar setModes messages
    /// @{

    /// @brief mode for adding edges
    MID_GNE_SETMODE_CREATE_EDGE,
    /// @brief mode for moving things
    MID_GNE_SETMODE_MOVE,
    /// @brief mode for deleting things
    MID_GNE_SETMODE_DELETE,
    /// @brief mode for inspecting object attributes
    MID_GNE_SETMODE_INSPECT,
    /// @brief mode for selecting objects
    MID_GNE_SETMODE_SELECT,
    /// @brief mode for connecting lanes
    MID_GNE_SETMODE_CONNECT,
    /// @brief mode for editing tls
    MID_GNE_SETMODE_TLS,
    /// @brief mode for editing additional
    MID_GNE_SETMODE_ADDITIONAL,
    /// @brief mode for editing crossing
    MID_GNE_SETMODE_CROSSING,
    /// @brief mode for creating polygons
    MID_GNE_SETMODE_POLYGON,
    /// @}


    /// @name GNEViewParent dialog messages
    /// @{

    /// @brief Size of frame area updated
    MID_GNE_VIEWPARENT_FRAMEAREAWIDTH,
    /// @}


    /// @name GNEViewNet messages
    /// @{

    /// @brief show connections
    MID_GNE_VIEWNET_SHOW_CONNECTIONS,
    /// @brief select edges
    MID_GNE_VIEWNET_SELECT_EDGES,
    /// @brief show junctions as bubbles
    MID_GNE_VIEWNET_SHOW_BUBBLES,
    /// @brief move elevation instead of x,y
    MID_GNE_VIEWNET_MOVE_ELEVATION,
    /// @brief show grid
    MID_GNE_VIEWNET_SHOW_GRID,
    /// @}


    /// @name GNEFrame messages
    /// @{

    /// @brief set GEO Attribute
    MID_GNEFRAME_GEOATTRIBUTE,
    /// @brief enable or disable use GEO
    MID_GNEFRAME_USEGEO,


    // MID_GNE_WIZARD,
    /// @brief attribute edited
    MID_GNE_SET_ATTRIBUTE,
    /// @brief attribute block shape
    MID_GNE_SET_BLOCKING_SHAPE,
    /// @brief attribute close shape
    MID_GNE_SET_CLOSING_SHAPE,

    /// @}


    /// @name GNESeletorFrame messages
    /// @{

    /// @brief select tag in selector frame
    MID_GNE_SELECTORFRAME_SELECTTAG,
    /// @brief select attribute in selector frame
    MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,
    /// @brief process string
    MID_GNE_SELECTORFRAME_PROCESSSTRING,
    /// @brief changes the visual scaling of selected items
    MID_GNE_SELECTORFRAME_SELECTSCALE,
    /// @}


    /// @name GNEConnectorFrame messages
    /// @{

    /// @brief select dead end lanes
    MID_GNE_CONNECTORFRAME_SELECTDEADENDS,
    /// @brief select lanes that have no connection leading to it
    MID_GNE_CONNECTORFRAME_SELECTDEADSTARTS,
    /// @brief select lanes that are connected from concurrent lanes
    MID_GNE_CONNECTORFRAME_SELECTCONFLICTS,
    /// @brief select lanes with connections that have the pass attribute set to 'true'
    MID_GNE_CONNECTORFRAME_SELECTPASS,
    /// @}


    /// @name GNEInspectorFrame messages
    /// @{

    /// @brief In GNEINSPECTORFRAME, center element
    MID_GNE_INSPECTORFRAME_CENTER,
    /// @brief In GNEINSPECTORFRAME, inspect element
    MID_GNE_INSPECTORFRAME_INSPECT,
    /// @brief In GNEINSPECTORFRAME, delete element
    MID_GNE_INSPECTORFRAME_DELETE,
    /// @brief set object as template
    MID_GNE_INSPECTORFRAME_SETTEMPLATE,
    /// @brief copy template
    MID_GNE_INSPECTORFRAME_COPYTEMPLATE,
    /// @brief go back to the previous element
    MID_GNE_INSPECTORFRAME_GOBACK,
    /// @brief open edit attribute dialog
    MID_GNE_INSPECTORFRAME_ATTRIBUTEEDITOR,
    /// @}


    /// @name GNETLSEditorFrame messages
    /// @{

    /// @brief selected junction von TLS
    MID_GNE_TLSFRAME_SELECT_JUNCTION,
    /// @brief update TLS status
    MID_GNE_TLSFRAME_UPDATE_STATUS,
    /// @brief replace program with a newly guessed program
    MID_GNE_TLSFRAME_GUESSPROGRAM,
    /// @brief definition related controls
    MID_GNE_TLSFRAME_OFFSET,
    /// @brief switch between programs
    MID_GNE_TLSFRAME_SWITCH,
    /// @brief Create TLS
    MID_GNE_TLSFRAME_CREATE,
    /// @brief delete TLS
    MID_GNE_TLSFRAME_DELETE,
    /// @brief rename TLS
    MID_GNE_TLSFRAME_RENAME,
    /// @brief sub-rename TLS
    MID_GNE_TLSFRAME_SUBRENAME,
    /// @brief add off to TLS
    MID_GNE_TLSFRAME_ADDOFF,
    /// @brief select phase thable
    MID_GNE_TLSFRAME_PHASE_TABLE,
    /// @brief create phase thable
    MID_GNE_TLSFRAME_PHASE_CREATE,
    /// @brief delete  phase thable
    MID_GNE_TLSFRAME_PHASE_DELETE,
    /// @brief cleanup unused states
    MID_GNE_TLSFRAME_CLEANUP,
    /// @}


    /// @name GNECrossingFrame messages
    /// @{

    /// @brief Create crossing
    MID_GNE_CROSSINGFRAME_CREATECROSSING,
    /// @brief use selected edges only
    MID_GNE_CROSSINGFRAME_USEONLYSELECTEDEDGES,
    /// @}


    /// @name GNEAdditionalFrame messages
    /// @{

    /// @brief select type of additional
    MID_GNE_ADDITIONALFRAME_SELECTADDITIONALTYPE,
    /// @brief change reference point
    MID_GNE_ADDITIONALFRAME_REFERENCEPOINT,
    /// @brief change parameter of type text
    MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT,
    /// @brief change parameter of tpye bool
    MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_BOOL,
    /// @brief attribute block movement
    MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT,
    /// @brief attribute Parent
    MID_GNE_ADDITIONALFRAME_CHANGEPARENT,
    /// @brief add row
    MID_GNE_ADDITIONALFRAME_ADDROW,
    /// @brief remove row
    MID_GNE_ADDITIONALFRAME_REMOVEROW,
    /// @brief select additional parent
    MID_GNE_ADDITIONALFRAME_SELECTADDITIONALPARENT,
    /// @brief search edge
    MID_GNE_ADDITIONALFRAME_SEARCHEDGE,
    /// @brief show only selected edge in a list
    MID_GNE_ADDITIONALFRAME_SHOWONLYSELECTEDEDGES,
    /// @brief select edge
    MID_GNE_ADDITIONALFRAME_SELECTEDGE,
    /// @brief clear selection of edges
    MID_GNE_ADDITIONALFRAME_CLEAREDGESELECTION,
    /// @brief invert selection of edges
    MID_GNE_ADDITIONALFRAME_INVERTEDGESELECTION,
    /// @brief search lane
    MID_GNE_ADDITIONALFRAME_SEARCHLANE,
    /// @brief use selected lanes
    MID_GNE_ADDITIONALFRAME_USESELECTEDLANES,
    /// @brief select lane
    MID_GNE_ADDITIONALFRAME_SELECTLANE,
    /// @brief clear selection of lanes
    MID_GNE_ADDITIONALFRAME_CLEARLANESELECTION,
    /// @brief invert selection of lanes
    MID_GNE_ADDITIONALFRAME_INVERTLANESELECTION,
    /// @}


    /// @name GNEPolygonFrame messages
    /// @{

    /// @brief start drawing polygon
    MID_GNE_POLYGONFRAME_STARTDRAWING,
    /// @brief stop drawing polygon
    MID_GNE_POLYGONFRAME_STOPDRAWING,
    /// @brief abort drawing polygon
    MID_GNE_POLYGONFRAME_ABORTDRAWING,
    /// @brief enable or disable closing polygon after drawing
    MID_GNE_POLYGONFRAME_CLOSE,
    /// @}


    /// @name GNEEdge messages
    /// @{

    /// @brief set non-default geometry endpoint
    MID_GNE_EDGE_SET_ENDPOINT,
    /// @brief restore geometry endpoint to node position
    MID_GNE_EDGE_RESET_ENDPOINT,
    /// @brief remove inner geometry
    MID_GNE_EDGE_STRAIGHTEN,
    /// @brief smooth geometry
    MID_GNE_EDGE_SMOOTH,
    /// @brief interpolate z values linear between junctions
    MID_GNE_EDGE_STRAIGHTEN_ELEVATION,
    /// @brief smooth elevation with regard to adjoining edges
    MID_GNE_EDGE_SMOOTH_ELEVATION,
    /// @brief split an edge
    MID_GNE_EDGE_SPLIT,
    /// @brief split an edge
    MID_GNE_EDGE_SPLIT_BIDI,
    /// @brief reverse an edge
    MID_GNE_EDGE_REVERSE,
    /// @brief add reverse edge
    MID_GNE_EDGE_ADD_REVERSE,
    /// @}


    /// @name GNEJunction messages
    /// @{

    /// @brief clear junction's connections
    MID_GNE_JUNCTION_CLEAR_CONNECTIONS,
    /// @brief reset junction's connections
    MID_GNE_JUNCTION_RESET_CONNECTIONS,
    /// @brief turn junction into geometry node
    MID_GNE_JUNCTION_REPLACE,
    /// @brief edit junction shape
    MID_GNE_JUNCTION_EDIT_SHAPE,
    /// @}


    /// @name GNEConnection messages
    /// @{

    /// @brief edit junction shape
    MID_GNE_CONNECTION_EDIT_SHAPE,
    /// @}


    /// @name GNECrossing messages
    /// @{

    /// @brief edit junction shape
    MID_GNE_CROSSING_EDIT_SHAPE,
    /// @}


    /// @name GNEPoly messages
    /// @{

    /// @brief simplify polygon geometry
    MID_GNE_POLYGON_SIMPLIFY_SHAPE,
    /// @brief close opened polygon
    MID_GNE_POLYGON_CLOSE,
    /// @brief open closed polygon
    MID_GNE_POLYGON_OPEN,
    /// @brief Set a vertex of polygon as first verte
    MID_GNE_POLYGON_SET_FIRST_POINT,
    /// @brief delete geometry point
    MID_GNE_POLYGON_DELETE_GEOMETRY_POINT,
    /// @}


    /// @name GNEPOI messages
    /// @{

    /// @brief Transform POI to POILane, and viceversa
    MID_GNE_POI_TRANSFORM,
    /// @}

    /// @name GNELane messages
    /// @{

    /// @brief duplicate a lane
    MID_GNE_LANE_DUPLICATE,
    /// @brief transform lane to sidewalk
    MID_GNE_LANE_TRANSFORM_SIDEWALK,
    /// @brief transform lane to bikelane
    MID_GNE_LANE_TRANSFORM_BIKE,
    /// @brief transform lane to busLane
    MID_GNE_LANE_TRANSFORM_BUS,
    /// @brief add sidewalk
    MID_GNE_LANE_ADD_SIDEWALK,
    /// @brief add bikelane
    MID_GNE_LANE_ADD_BIKE,
    /// @brief add busLane
    MID_GNE_LANE_ADD_BUS,
    /// @brief remove sidewalk
    MID_GNE_LANE_REMOVE_SIDEWALK,
    /// @brief remove bikelane
    MID_GNE_LANE_REMOVE_BIKE,
    /// @brief remove busLane
    MID_GNE_LANE_REMOVE_BUS,
    /// @}


    /// @name Additional Dialog messages (used in all GNEAdditionalDialogs)
    /// @{

    /// @brief accept button
    MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,
    /// @brief cancel button
    MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,
    /// @brief reset button
    MID_GNE_ADDITIONALDIALOG_BUTTONRESET,
    /// @}

    /// @name Variable Speed Sign Dialog messages
    /// @{
    /// @brief add row
    MID_GNE_VARIABLESPEEDSIGN_ADDROW,
    /// @brief remove row
    MID_GNE_VARIABLESPEEDSIGN_TABLE_STEPS,
    /// @}


    /// @name Rerouter Dialog messages
    /// @{

    /// @brief select table interval
    MID_GNE_REROUTEDIALOG_TABLE_INTERVAL,
    /// @brief change table clisng lane reroute reroute
    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE,
    /// @brief change table route closing reroute
    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,
    /// @brief change table destiny probability reroute
    MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,
    /// @brief change table route probability reroute
    MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,
    /// @brief add interval
    MID_GNE_REROUTEDIALOG_ADD_INTERVAL,
    /// @brief add closing lane reroute
    MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE,
    /// @brief add closing reroute
    MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE,
    /// @brief add destiny probability route
    MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE,
    /// @brief add route probability route
    MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE,
    /// @brief edit interval
    MID_GNE_REROUTEDIALOG_EDIT_INTERVAL,
    /// @}


    /// @name Calibrator Dialog messages
    /// @{

    /// @brief change table route
    MID_GNE_CALIBRATORDIALOG_TABLE_ROUTE,
    /// @brief add new route
    MID_GNE_CALIBRATORDIALOG_ADD_ROUTE,
    /// @brief change table flow
    MID_GNE_CALIBRATORDIALOG_TABLE_FLOW,
    MID_GNE_CALIBRATORDIALOG_ADD_FLOW,
    /// @brief change table route
    MID_GNE_CALIBRATORDIALOG_TABLE_VEHICLETYPE,
    /// @brief add vehicle type
    MID_GNE_CALIBRATORDIALOG_ADD_VEHICLETYPE,
    /// @brief set new variable
    MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,
    /// @brief set flow type
    MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE,
    /// @}

    /// @name allowDisallow Dialog messages
    /// @{

    /// @brief select/unselect single vehicle
    MID_GNE_ALLOWDISALLOW_CHANGE,
    /// @brief select all vehicles
    MID_GNE_ALLOWDISALLOW_SELECTALL,
    /// @brief unselect all vehicles
    MID_GNE_ALLOWDISALLOW_UNSELECTALL,
    /// @brief select only non road vehicles
    MID_GNE_ALLOWDISALLOW_SELECTONLYNONROAD,
    /// @}


    /// @name Fix Stoppingplaces Dialog messages
    /// @{

    /// @brief FixStoppingPlaces dialog
    MID_GNE_FIXSTOPPINGPLACES_CHANGE,
    /// @}


    /// @brief last element of enum (not used)
    MID_LAST
};


#endif

/****************************************************************************/

