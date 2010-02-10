/****************************************************************************/
/// @file    GUIDialog_Breakpoints.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Jun 2004
/// @version $Id$
///
// Editor for simulation breakpoints
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
#ifndef GUIDialog_Breakpoints_h
#define GUIDialog_Breakpoints_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIApplicationWindow;
class MFXAddEditTypedTable;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_Breakpoints
 * @brief Editor for simulation breakpoints
 *
 * This dialog shows and lets the user edit the list of breakpoint - simulation
 *  time steps where the simulation halts.
 * @todo Use a LineReader instead of >> while reading
 */
class GUIDialog_Breakpoints : public FXMainWindow {
    // FOX-declarations
    FXDECLARE(GUIDialog_Breakpoints)
public:
    /// constructor
    GUIDialog_Breakpoints(GUIMainWindow *parent);

    /// destructor
    ~GUIDialog_Breakpoints();

    /// Called when the user presses the Load-button
    long onCmdLoad(FXObject*,FXSelector,void*);

    /// Called when the user presses the Save-button
    long onCmdSave(FXObject*,FXSelector,void*);

    /// Called when the user presses the Clear-button
    long onCmdClear(FXObject*,FXSelector,void*);

    /// Called when the user presses the Close-button
    long onCmdClose(FXObject*,FXSelector,void*);

    /// Called when the table was changed
    long onCmdEditTable(FXObject*,FXSelector,void*);
//    long onUpdSave(FXObject*,FXSelector,void*);

private:
    /// Rebuilds the entire list
    void rebuildList();

    /// Builds a text representation of the items in the list
    std::string encode2TXT();

private:
    /// the list that holds the ids
    MFXAddEditTypedTable *myTable;

    /// the parent window
    GUIMainWindow *myParent;

protected:
    /// FOX needs this
    GUIDialog_Breakpoints() { }

};


#endif

/****************************************************************************/

