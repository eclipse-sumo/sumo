/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    MFXDialogBox.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2025
///
// Custom FXDialogBox that supports internal tests
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEInternalTest.h>

#include "MFXDialogBox.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXDialogBox) MFXDialogBoxMap[] = {
    // close dialog accepting changes
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_ACCEPT, MFXDialogBox::onCmdAccept),
    // close dialog discarding changes
    FXMAPFUNC(SEL_CLOSE,    0,                      MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL, MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL, MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL, MFXDialogBox::onCmdCancel),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(MFXDialogBox, FXDialogBox, MFXDialogBoxMap, ARRAYNUMBER(MFXDialogBoxMap))

// ===========================================================================
// method definitions
// ===========================================================================

MFXDialogBox::MFXDialogBox(GNEApplicationWindow* applicationWindow, const FXString& name, FXuint opts, FXint x, FXint y,
                           FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs) :
    FXDialogBox(applicationWindow->getApp(), name, opts, x, y, w, h, pl, pr, pt, pb, hs, vs),
    myApplicationWindow(applicationWindow) {
    // check option with only mainWindow
}


bool
MFXDialogBox::openModal(FXuint placement) {
    // create and show dialog
    create();
    show(placement);
    // refresh the application
    getApp()->refresh();
    // continue depending on whether we are testing or not
    if (myApplicationWindow->getInternalTest()) {
        myTesting = true;
        // execute every modal dialog test step
        for (const auto& modalStep : myApplicationWindow->getInternalTest()->getCurrentStep()->getModalDialogTestSteps()) {
            // this will be unified
            if (modalStep->getEvent()) {
                handle(myApplicationWindow->getInternalTest(), modalStep->getSelector(), modalStep->getEvent());
            } else if (modalStep->getDialogTest()) {
                handle(myApplicationWindow->getInternalTest(), modalStep->getSelector(), modalStep->getDialogTest());
            }
        }
    } else {
        myTesting = false;
        getApp()->runModalFor(this);
    }
    return myAccepted;
}


bool
MFXDialogBox::getAccepted() const {
    return myAccepted;
}


long
MFXDialogBox::closeDialogAccepting() {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, TRUE);
    }
    // hide dialog
    hide();
    // enable accepted flag
    myAccepted = true;
    return 1;
}


long
MFXDialogBox::closeDialogCanceling() {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, FALSE);
    }
    // hide dialog
    hide();
    // disable accepted flag
    myAccepted = false;
    return 0;
}