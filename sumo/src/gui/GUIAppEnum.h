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
    /// Open a new Lane aggregated View
    MID_NEW_LANEAVIEW,
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
    //@}

    //@{ Settings Menu Item - IDs
    /// Application Settings - Menu Entry
    MID_APPSETTINGS,
    /// Simulation Settings - Menu Entry
    MID_SIMSETTINGS,
    /// The Simulation Delay Control
    MID_SIMDELAY,
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
    /// Show Grid - Button
    MID_SHOWGRID,
    /// Show Tool Tips - Button
    MID_SHOWTOOLTIPS,
    /// Show Full Geom - Button
    MID_SHOWFULLGEOM,
    //@}

    //@{ Common View IDs
    /// GLCanvas - ID
    MID_GLCANVAS,
    /// Right Mouse Button Timeout
    ID_RMOUSETIMEOUT,
    //@}

    //@{ View Settings - IDs
    /// Change Vehicle Coloring Scheme- combo
    MID_COLOURVEHICLES,
    /// Change Lane Coloring Scheme- combo
    MID_COLOURLANES,
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
    /// Add to selected Items - Menu Etry
    MID_ADDSELECT,
    /// Remove from selected Items - Menu Etry
    MID_REMOVESELECT,
    /// Open the object's manipulator
    MID_MANIP,
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
    MID_PERIOD

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
