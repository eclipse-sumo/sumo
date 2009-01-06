/****************************************************************************/
/// @file    GUIDialog_SimSettings.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// The simulation-settings dialog
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDialog_SimSettings_h
#define GUIDialog_SimSettings_h


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
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_SimSettings
 * @brief The simulation-settings dialog
 */
class GUIDialog_SimSettings : public FXDialogBox
{
    // FOX-callbacks
    FXDECLARE(GUIDialog_SimSettings)
public:
    /// Constructor
    GUIDialog_SimSettings(FXMainWindow* parent, bool *quitOnEnd,
                          bool *suppressEnd);

    /// Destructor
    ~GUIDialog_SimSettings();

    /// Called when the user presses the Ok-button
    long onCmdOk(FXObject*,FXSelector,void*);

    /// Called when the user presses the Cancel-button
    long onCmdCancel(FXObject*,FXSelector,void*);

    /// Called when the user changes the state of the "quit on end"-checkbox
    long onCmdQuitOnEnd(FXObject*,FXSelector,void*);

    /// Called when the user changes the state of the "suppress end information"-checkbox
    long onCmdSuppressEnd(FXObject*,FXSelector,void*);


private:
    /// Current dialog setting whether the application shall quit on simulation end
    bool myAppQuitOnEnd;

    /// Current dialog setting whether no simulation end information shall be printed
    bool mySuppressEnd;

    /// Pointer to the bool that shall be set with the current "quit on end"-value
    bool *mySetAppQuitOnEnd;

    /// Pointer to the bool that shall be set with the current "suppress end information"-value
    bool *mySetSuppressEnd;

protected:
    /// Fox needs this
    GUIDialog_SimSettings() { }

};


#endif

/****************************************************************************/

