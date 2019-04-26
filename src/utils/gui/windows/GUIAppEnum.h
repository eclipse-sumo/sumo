/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

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


    /// @name hotkeys
    /// @{
    /// @brief hotkey for mode editing additionals AND stops
    MID_HOTKEY_A_ADDITIONALMODE_STOPMODE,
    /// @brief hotkey for mode connecting lanes
    MID_HOTKEY_C_CONNECTMODE,
    /// @brief hotkey for mode deleting things
    MID_HOTKEY_D_DELETEMODE,
    /// @brief hotkey for mode adding edges
    MID_HOTKEY_E_EDGEMODE,
    /// @brief hotkey for mode inspecting object attributes
    MID_HOTKEY_I_INSPECTMODE,
    /// @brief hotkey for mode moving element
    MID_HOTKEY_M_MOVEMODE,
    /// @brief hotkey for mode creating polygons
    MID_HOTKEY_P_POLYGONMODE,
    /// @brief hotkey for mode selecting objects
    MID_HOTKEY_S_SELECTMODE,
    /// @brief hotkey for mode editing crossing AND routes
    MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE,
    /// @brief hotkey for mode editing TLS AND Vehicle Types
    MID_HOTKEY_T_TLSMODE_VTYPEMODE,
    /// @brief hotkey for mode create vehicles
    MID_HOTKEY_V_VEHICLEMODE,
    /// @brief hotkey for mode editing connection prohibitions
    MID_HOTKEY_W_PROHIBITIONMODE,
    /// @brief hotkey for mode editing TAZ
    MID_HOTKEY_Z_TAZMODE,

    /// @}

    /// @name Ctrl - hotkeys
    /// @{
    /// @brief Start the simulation in SUMO and open Additionals Elemements in NETEDIT
    MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS,
    /// @brief Edit simulation breakpoints
    MID_HOTKEY_CTRL_B_EDITBREAKPOINT,
    /// @brief Perform a single simulation step in SUMO and open Demand Elements in NETEDIT
    MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMAND,
    /// @brief Toogle Gaming mode in SUMO and grid in NETEDIT
    MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID,
    /// @brief Fullscreen mode - menu entry
    MID_HOTKEY_CTRL_F_FULSCREENMODE,
    /// @brief save joined junctions
    MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS,
    /// @brief save network as plain XML
    MID_HOTKEY_CTRL_L_SAVEASPLAINXML,
    /// @brief create new empty newtork
    MID_HOTKEY_CTRL_N_NEWNETWORK,
    /// @brief Main window closes
    MID_HOTKEY_CTRL_Q_CLOSE,
    /// @brief Stop the simulation in SUMO and save network in NETEDIT
    MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,
    /// @brief Reload the previously loaded simulation
    MID_HOTKEY_CTRL_R_RELOAD,
    /// @brief Open current network in SUMO or in NETEDIT
    MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,
    /// @brief Close simulation - ID
    MID_HOTKEY_CTRL_W_CLOSESIMULATION,
    /// @brief Undo
    MID_HOTKEY_CTRL_Y_REDO,
    /// @brief Redo
    MID_HOTKEY_CTRL_Z_UNDO,
    /// @brief Toogle show connections in Netedit
    MID_HOTKEY_CTRL_SHIFT_C_SHOWCONNECTIONS,
    /// @brief Toogle move elevation
    MID_HOTKEY_CTRL_SHIFT_H_TOOGLEELEVATION,
    /// @brief Toogle select edges in Netedit
    MID_HOTKEY_CTRL_SHIFT_I_SELECTEDGES,
    /// @brief Toogle chaing edges
    MID_HOTKEY_CTRL_SHIFT_J_TOOGLECHAIN,
    /// @brief Toogle two way
    MID_HOTKEY_CTRL_SHIFT_K_TOOGLETWOWAY,

    /// @}

    /// @name Other hotkeys
    /// @{

    /// @brief Save Additional Elements
    MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS,
    /// @brief Save Demand Elements
    MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMAND,
    /// @brief save newtwork as
    MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORKAS,
    /// @brief compute junctions
    MID_HOTKEY_F5_RECOMPUTE,
    /// @brief compute junctions with volatile options
    MID_HOTKEY_SHIFT_F5_RECOMPUTEVOLATILE,
    /// @brief hot key <F1> open online documentation
    MID_HOTKEY_F1_ONLINEDOCUMENTATION,
    /// @brief hot key <F2> open about dialog
    MID_HOTKEY_F2_ABOUT,
    /// @brief hot key <F3> set network mode in NETEDIT
    MID_HOTKEY_F3_SUPERMODE_NETWORK,
    /// @brief hot key <F4> set demand mode in NETEDIT
    MID_HOTKEY_F4_SUPERMODE_DEMAND,
    /// @brief hot key <ESC> abort current edit operation
    MID_GNE_HOTKEY_ESC,
    /// @brief hot key <F12> focus upper element of current frame
    MID_GNE_HOTKEY_F12,
    /// @brief hot key <DEL> delete selections or elements
    MID_GNE_HOTKEY_DEL,
    /// @brief hot key <ENTER> accept current operation
    MID_GNE_HOTKEY_ENTER,
    /// @brief save TLS Programs
    MID_GNE_HOTKEY_CTRL_SHIFT_K,
    /// @brief clean junctions without edges
    MID_GNE_HOTKEY_F6,
    /// @brief join selected junctions
    MID_GNE_HOTKEY_F7,
    /// @brief clean invalid crossings
    MID_GNE_HOTKEY_F8,
    /// @brief open options menu
    MID_GNE_HOTKEY_F10,

    /// @}

    /// @name application specific
    /// @{

    /// @brief Main window-ID
    MID_WINDOW,

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
    MID_HOTKEY_CTRL_P,
    /// @brief Load edge data for visualization
    MID_OPEN_EDGEDATA,
    /// @brief Load additional file with additional elements
    MID_OPEN_TLSPROGRAMS,
    /// @brief Loads a file previously loaded
    MID_RECENTFILE,
    /// @}


    /// @name Main Window Simulation Control - IDs
    /// @{
    /// @brief Save state to file
    MID_SIMSAVE,
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
    /// @brief Open in netedit
    MID_NETEDIT,
    /// @}


    /// @name Settings Menu Item - IDs
    /// @{

    /// @brief Application settings - menu entry
    MID_APPSETTINGS,
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

    /// @brief update traci status
    MID_TRACI_STATUS,

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
    /// @brief scale traffic
    MID_DEMAND_SCALE,
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
    /// @brief Show transportable plan
    MID_SHOWPLAN,
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
    /// @brief Show vehicle's future route
    MID_SHOW_FUTUREROUTE,
    /// @brief Hide vehicle's future route
    MID_HIDE_FUTUREROUTE,
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
    /// @brief Track object
    MID_CHOOSER_TRACK,
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
    /// @brief Deselect selected items
    MID_CHOOSEN_NAME,
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

    /// @name Toolbar file messages
    /// @{

    /// @brief open foreign network
    MID_GNE_TOOLBARFILE_OPENFOREIGN,
    /// @brief save additionals as
    MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,
    /// @brief save TLS Programs as
    MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,
    /// @brief save demand elements as
    MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,
    /// @}


    /// @name GNEViewParent dialog messages
    /// @{

    /// @brief Size of frame area updated
    MID_GNE_VIEWPARENT_FRAMEAREAWIDTH,
    /// @}


    /// @name GNEViewNet messages
    /// @{

    /// @brief show demand elements
    MID_GNE_VIEWNET_SHOW_DEMAND_ELEMENTS,
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
    // MID_GNE_WIZARD,
    /// @brief attribute added
    MID_GNE_ADD_ATTRIBUTE,
    /// @brief attribute removed
    MID_GNE_REMOVE_ATTRIBUTE,
    /// @brief attribute edited
    MID_GNE_SET_ATTRIBUTE,
    /// @brief create something
    MID_GNE_CREATE,
    /// @brief used to select a type of element in a combo box
    MID_GNE_SET_TYPE,
    /// @brief text attribute edited
    MID_GNE_SET_ATTRIBUTE_TEXT,
    /// @brief bool attribute edited
    MID_GNE_SET_ATTRIBUTE_BOOL,
    /// @brief attribute edited trought dialog
    MID_GNE_SET_ATTRIBUTE_DIALOG,
    /// @brief attribute selected using a radio button
    MID_GNE_SET_ATTRIBUTE_RADIOBUTTON,
    /// @brief select items
    MID_GNE_SELECT,
    /// @}


    /// @name GNESelectorFrame messages
    /// @{

    /// @brief select tag in selector frame
    MID_GNE_SELECTORFRAME_SELECTTAG,
    /// @brief select attribute in selector frame
    MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,
    /// @brief process string
    MID_GNE_SELECTORFRAME_PROCESSSTRING,
    /// @brief changes the visual scaling of selected items
    MID_GNE_SELECTORFRAME_SELECTSCALE,
    /// @brief start drawing polygon
    MID_GNE_STARTDRAWING,
    /// @brief stop drawing polygon
    MID_GNE_STOPDRAWING,
    /// @brief abort drawing polygon
    MID_GNE_ABORTDRAWING,
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
    /// @brief inspect next element
    MID_GNE_INSPECTORFRAME_NEXT,
    /// @brief inspect previous element
    MID_GNE_INSPECTORFRAME_PREVIOUS,
    /// @brief show list of overlapped elements
    MID_GNE_INSPECTORFRAME_SHOWLIST,
    /// @brief list item selected
    MID_GNE_INSPECTORFRAME_ITEMSELECTED,
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
    /// @brief mark unused states
    MID_GNE_TLSFRAME_MARKUNUSED,
    /// @brief mark unused states
    MID_GNE_TLSFRAME_ADDUNUSED,
    /// @brief Load Program
    MID_GNE_TLSFRAME_LOAD_PROGRAM,
    /// @brief cleanup unused states
    MID_GNE_TLSFRAME_SAVE_PROGRAM,
    /// @}


    /// @name GNETLSEditorFrame messages
    /// @{
    /// @brief create vehicle types
    MID_GNE_VEHICLETYPEFRAME_CREATE,
    /// @brief delete a vehicle type
    MID_GNE_VEHICLETYPEFRAME_DELETE,
    /// @brief reset a default vehicle type
    MID_GNE_VEHICLETYPEFRAME_RESET,
    /// @brief copy a vehicle type
    MID_GNE_VEHICLETYPEFRAME_COPY,
    /// @}


    /// @name GNECrossingFrame messages
    /// @{

    /// @brief Create crossing
    MID_GNE_CROSSINGFRAME_CREATECROSSING,
    /// @}

    /// @name GNEAdditionalFrame messages
    /// @{

    /// @brief add row
    MID_GNE_ADDITIONALFRAME_ADDROW,
    /// @brief remove row
    MID_GNE_ADDITIONALFRAME_REMOVEROW,
    /// @brief search element
    MID_GNE_ADDITIONALFRAME_SEARCH,
    /// @brief use selected elements
    MID_GNE_ADDITIONALFRAME_USESELECTED,
    /// @brief select element
    MID_GNE_ADDITIONALFRAME_SELECT,
    /// @brief clear selection of elements
    MID_GNE_ADDITIONALFRAME_CLEARSELECTION,
    /// @brief invert selection of eleents
    MID_GNE_ADDITIONALFRAME_INVERTSELECTION,
    /// @brief start selection of consecutive egdes/lanes
    MID_GNE_ADDITIONALFRAME_STARTSELECTION,
    /// @brief stop selection of consecutive egdes/lanes
    MID_GNE_ADDITIONALFRAME_STOPSELECTION,
    /// @brief abort selection of consecutive egdes/lanes
    MID_GNE_ADDITIONALFRAME_ABORTSELECTION,
    /// @}


    /// @name GNEProhibitionFrame messages
    /// @{

    /// @brief select prohibiting connection
    MID_GNE_PROHIBITIONFRAME_SELECTPROHIBITING,
    /// @brief select connections to be prohibited
    MID_GNE_PROHIBITIONFRAME_SELECTPROHIBITED,
    /// @brief abort drawing polygon
    MID_GNE_PROHIBITIONFRAME_ABORTDRAWING,
    /// @}


    /// @name GNEVehicleFrame messages
    /// @{

    /// @brief abort route creation
    MID_GNE_VEHICLEFRAME_ABORT,
    /// @brief finish route creation
    MID_GNE_VEHICLEFRAME_FINISHCREATION,
    /// @brief remove last inserted edge
    MID_GNE_VEHICLEFRAME_REMOVELASTEDGE,
    /// @}


    /// @name GNEEdge messages
    /// @{

    /// @brief change default geometry endpoints
    MID_GNE_EDGE_EDIT_ENDPOINT,
    /// @brief reset default geometry endpoints
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
    /// @brief turn junction into multiple junctions
    MID_GNE_JUNCTION_SPLIT,
    /// @brief turn junction into multiple junctions and reconnect them heuristically
    MID_GNE_JUNCTION_SPLIT_RECONNECT,
    /// @brief edit junction shape
    MID_GNE_JUNCTION_EDIT_SHAPE,
    /// @brief reset junction shape
    MID_GNE_JUNCTION_RESET_SHAPE,
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
    /// @brief remove greenVerge
    MID_GNE_LANE_RESET_CUSTOMSHAPE,
    /// @brief transform lane to sidewalk
    MID_GNE_LANE_TRANSFORM_SIDEWALK,
    /// @brief transform lane to bikelane
    MID_GNE_LANE_TRANSFORM_BIKE,
    /// @brief transform lane to busLane
    MID_GNE_LANE_TRANSFORM_BUS,
    /// @brief transform lane to greenVerge
    MID_GNE_LANE_TRANSFORM_GREENVERGE,
    /// @brief add sidewalk
    MID_GNE_LANE_ADD_SIDEWALK,
    /// @brief add bikelane
    MID_GNE_LANE_ADD_BIKE,
    /// @brief add busLane
    MID_GNE_LANE_ADD_BUS,
    /// @brief add greenVerge
    MID_GNE_LANE_ADD_GREENVERGE,
    /// @brief remove sidewalk
    MID_GNE_LANE_REMOVE_SIDEWALK,
    /// @brief remove bikelane
    MID_GNE_LANE_REMOVE_BIKE,
    /// @brief remove busLane
    MID_GNE_LANE_REMOVE_BUS,
    /// @brief remove greenVerge
    MID_GNE_LANE_REMOVE_GREENVERGE,
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
    /// @brief Click over Table
    MID_GNE_VARIABLESPEEDSIGN_TABLE,
    /// @brief sort table values
    MID_GNE_VARIABLESPEEDSIGN_SORT,
    /// @}


    /// @name Rerouter Dialog messages
    /// @{

    /// @brief select table interval
    MID_GNE_REROUTEDIALOG_TABLE_INTERVAL,
    /// @brief change table closing lane reroute reroute
    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE,
    /// @brief change table route closing reroute
    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,
    /// @brief change table destiny probability reroute
    MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,
    /// @brief change table route probability reroute
    MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,
    /// @brief change table parkingAreaReroute
    MID_GNE_REROUTEDIALOG_TABLE_PARKINGAREAREROUTE,
    /// @brief add interval
    MID_GNE_REROUTEDIALOG_ADD_INTERVAL,
    /// @brief sort rerouter intervals
    MID_GNE_REROUTEDIALOG_SORT_INTERVAL,
    /// @brief add closing lane reroute
    MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE,
    /// @brief add closing reroute
    MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE,
    /// @brief add destiny probability route
    MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE,
    /// @brief add route probability route
    MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE,
    /// @brief add parkingAreaReroute
    MID_GNE_REROUTEDIALOG_ADD_PARKINGAREAREROUTE,
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
    MID_GNE_ALLOWDISALLOW_SELECTONLYROAD,
    /// @}


    /// @name Fix Stoppingplaces Dialog messages
    /// @{

    /// @brief FixStoppingPlaces dialog
    MID_GNE_FIXSTOPPINGPLACES_CHANGE,
    /// @}

    /// @name Generic Parameters Dialog messages
    /// @{

    /// @brief Generic parameters dialog load
    MID_GNE_GENERICPARAMETERS_LOAD,
    /// @brief Generic parameters dialog save
    MID_GNE_GENERICPARAMETERS_SAVE,
    /// @brief Generic parameters dialog clear
    MID_GNE_GENERICPARAMETERS_CLEAR,
    /// @brief Generic parameters dialog sort
    MID_GNE_GENERICPARAMETERS_SORT,
    /// @}


    /// @brief last element of enum (not used)
    MID_LAST
};


#endif

/****************************************************************************/

