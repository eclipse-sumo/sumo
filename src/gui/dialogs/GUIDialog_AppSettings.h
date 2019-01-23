/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIDialog_AppSettings.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// The application-settings dialog
/****************************************************************************/
#ifndef GUIDialog_AppSettings_h
#define GUIDialog_AppSettings_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
    /** @brief Constructor
     *
     * @param[in] parent The parent window
     */
    GUIDialog_AppSettings(FXMainWindow* parent);

    /// @brief Destructor
    ~GUIDialog_AppSettings();



    /// @name FOX-callbacks
    /// @{

    /// @brief Called on OK-button pressure
    long onCmdOk(FXObject*, FXSelector, void*);

    /// @brief Called on Cancel-button pressure
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief Called on button change
    long onCmdSelect(FXObject*, FXSelector sel, void*);
    /// @}


private:
    /// @brief Information whether the application shall be quit
    bool myAppQuitOnEnd;

    /// @brief Information whether the simulation shall start directly after loading
    bool myAppAutoStart;

    /// @brief Information whether the simulation restarts after ending (demo mode)
    bool myAppDemo;

    /// @brief Information whether textures may be used
    bool myAllowTextures;

    /// @brief Information whether locate links appear in messages
    bool myLocateLinks;


protected:
    /// default constructor (needed by FOX)
    GUIDialog_AppSettings() { }

};


#endif

/****************************************************************************/

