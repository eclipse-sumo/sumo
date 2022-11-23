/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUISaveDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Dialog for ask user about saving elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>


// Message box buttons
enum {
    MBOX_OK                   = 0x10000000, /// Message box has a only an OK button
    MBOX_OK_CANCEL            = 0x20000000, /// Message box has OK and CANCEL buttons
    MBOX_YES_NO               = 0x30000000, /// Message box has YES and NO buttons
    MBOX_YES_NO_CANCEL        = 0x40000000, /// Message box has YES, NO, and CANCEL buttons
    MBOX_QUIT_CANCEL          = 0x50000000, /// Message box has QUIT and CANCEL buttons
    MBOX_QUIT_SAVE_CANCEL     = 0x60000000, /// Message box has QUIT, SAVE, and CANCEL buttons
    MBOX_SKIP_SKIPALL_CANCEL  = 0x70000000, /// Message box has SKIP, SKIP ALL, and CANCEL buttons
    MBOX_SAVE_CANCEL_DONTSAVE = 0x80000000  /// Message box has DON'T SAVE, CANCEL and SAVE buttons
};

// Return values
enum {
    MBOX_CLICKED_YES      = 1,            /// The YES button was clicked
    MBOX_CLICKED_NO       = 2,            /// The NO button was clicked
    MBOX_CLICKED_OK       = 3,            /// The OK button was clicked
    MBOX_CLICKED_CANCEL   = 4,            /// The CANCEL button was clicked
    MBOX_CLICKED_QUIT     = 5,            /// The QUIT button was clicked
    MBOX_CLICKED_SAVE     = 6,            /// The SAVE button was clicked
    MBOX_CLICKED_SKIP     = 7,            /// The SKIP button was clicked
    MBOX_CLICKED_SKIPALL  = 8             /// The SKIP ALL button was clicked
};


/**
* A Message Box is a convenience class which provides a dialog for
* very simple common yes/no type interactions with the user.
* The message box has an optional icon, a title string, and the question
* which is presented to the user.  It also has up to three buttons which
* furnish standard responses to the question.
* Message boxes are usually ran modally: the question must be answered
* before the program may continue.
*/
class GUISaveDialog : public FXDialogBox {
    FXDECLARE(GUISaveDialog)

protected:
    GUISaveDialog() {}

private:
    GUISaveDialog(const GUISaveDialog&);

    GUISaveDialog &operator=(const GUISaveDialog&) = delete;

    void initialize(const FXString &text, FXIcon* ic, FXuint whichbuttons);

public:
    long onCmdClicked(FXObject*, FXSelector, void*);

    long onCmdCancel(FXObject*, FXSelector, void*);

    enum{
        ID_CLICKED_YES=FXDialogBox::ID_LAST, 
        ID_CLICKED_NO, 
        ID_CLICKED_OK, 
        ID_CLICKED_CANCEL, 
        ID_CLICKED_QUIT, 
        ID_CLICKED_SAVE, 
        ID_CLICKED_SKIP, 
        ID_CLICKED_SKIPALL, 
        ID_LAST
    };

    /// Construct message box with given caption, icon, and message text
    GUISaveDialog(FXWindow* owner, const FXString &caption, const FXString &text, FXIcon* ic=NULL, FXuint opts=0, FXint x=0, FXint y=0);

    /// Construct free floating message box with given caption, icon, and message text
    GUISaveDialog(FXApp* app, const FXString &caption, const FXString &text, FXIcon* ic=NULL, FXuint opts=0, FXint x=0, FXint y=0);

    /**
    * Show a modal error message.
    * The text message may contain printf-tyle formatting commands.
    */
    static FXuint error(FXWindow* owner, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show modal error message, in free floating window.
    */
    static FXuint error(FXApp* app, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show a modal warning message
    * The text message may contain printf-tyle formatting commands.
    */
    static FXuint warning(FXWindow* owner, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show modal warning message, in free floating window.
    */
    static FXuint warning(FXApp* app, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show a modal question dialog
    * The text message may contain printf-tyle formatting commands.
    */
    static FXuint question(FXWindow* owner, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show modal question message, in free floating window.
    */
    static FXuint question(FXApp* app, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show a modal information dialog
    * The text message may contain printf-tyle formatting commands.
    */
    static FXuint information(FXWindow* owner, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;

    /**
    * Show modal information message, in free floating window.
    */
    static FXuint information(FXApp* app, FXuint opts, const char* caption, const char* message, ...) FX_PRINTF(4, 5) ;
};