/****************************************************************************/
/// @file    GUIAppEnum.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// Message and object IDs used by the FOX-version of the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
    ID_ABOUT=FXMainWindow::ID_LAST,

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

    /// Open chosen editor - ID
    MID_EDITCHOSEN,
    /// Edit simulation breakpoints
    MID_EDIT_BREAKPOINTS,
    //@}


    /// @name Settings Menu Item - IDs
    //@{

    /// Application settings - menu entry
    MID_APPSETTINGS,
    /// Gaming mode - menu entry
    MID_GAMING,
    /// The Simulation delay control
    MID_SIMDELAY,
    //@}


    // Clear simulation settings
    MID_CLEARMESSAGEWINDOW,


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
    /// Locate TLS - button
    MID_LOCATETLS,
    /// Locate addtional structure - button
    MID_LOCATEADD,
    /// Locate shape - button
    MID_LOCATESHAPE,
    /// Open viewport editor - button
    MID_EDITVIEWPORT,
    /// Open view editor - button
    MID_EDITVIEWSCHEME,
    /// Show tool tips - button
    MID_SHOWTOOLTIPS,
    /// Make snapshot - button
    MID_MAKESNAPSHOT,
    //@}


    /// @name Common view IDs
    //@{

    /// GLCanvas - ID
    MID_GLCANVAS,
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
    /// Copy cursor position - popup entry
    MID_COPY_CURSOR_POSITION,
    /// Show object parameter - popup entry
    MID_SHOWPARS,
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
    /// Allow textures - Option
    MID_ALLOWTEXTURES,
    //@}


    /// @name GLObjChooser - Dialog IDs
    //@{

    /// Center object
    MID_CHOOSER_CENTER,
    /// Text entry
    MID_CHOOSER_TEXT,
    /// Object list
    MID_CHOOSER_LIST,
    //@}


    /// @name GLChosenEditor - Dialog IDs
    //@{

    /// Load set
    MID_CHOOSEN_LOAD,
    /// Save set
    MID_CHOOSEN_SAVE,
    /// Clear set
    MID_CHOOSEN_CLEAR,
    /// Deselect selected items
    MID_CHOOSEN_DESELECT,
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


    MID_CUTSWELL,

    MID_LAST
};


#endif

/****************************************************************************/

