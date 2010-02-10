/****************************************************************************/
/// @file    GUIDialog_AppSettings.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// The application-settings dialog
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
#ifndef GUIDialog_AppSettings_h
#define GUIDialog_AppSettings_h


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
 * @class GUIDialog_AppSettings
 * @brief The dialog to change the application (gui) settings.
 */
class GUIDialog_AppSettings : public FXDialogBox {
    // is a FOX-object with an own mapping
    FXDECLARE(GUIDialog_AppSettings)
public:
    /// constructor
    GUIDialog_AppSettings(FXMainWindow* parent);

    /// destructor
    ~GUIDialog_AppSettings();

    /// Called on OK-button pressure
    long onCmdOk(FXObject*,FXSelector,void*);

    /// Called on Cancel-button pressure
    long onCmdCancel(FXObject*,FXSelector,void*);

    /// Called on "Quit on end"-button change
    long onCmdQuitOnEnd(FXObject*,FXSelector,void*);

    /// Called on "Allow textures"-button change
    long onCmdAllowTextures(FXObject*,FXSelector,void*);

private:
    /// Information whether the application shall be quit
    bool myAppQuitOnEnd;

    /// Information whether textures may be used
    bool myAllowTextures;

protected:
    /// default constructor (needed by FOX)
    GUIDialog_AppSettings() { }

};


#endif

/****************************************************************************/

