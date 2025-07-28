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
/// @file    GNEDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Custom FXDialogBox used in Netedit that supports internal tests
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEInternalTest.h>

#include "GNEDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog) MFXDialogBoxMap[] = {
    // close dialog accepting changes
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_ACCEPT, GNEDialog::onCmdAccept),
    // close dialog discarding changes
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEDialog::onCmdCancel),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL, GNEDialog::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL, GNEDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL, GNEDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEDialog, FXDialogBox, MFXDialogBoxMap, ARRAYNUMBER(MFXDialogBoxMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEDialog::GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name, FXuint opts, FXint x, FXint y,
                     FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs) :
    FXDialogBox(applicationWindow->getApp(), name.c_str(), opts, x, y, w, h, pl, pr, pt, pb, hs, vs),
    myApplicationWindow(applicationWindow) {
    // create content frame
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y);

}


GNEDialog::Result
GNEDialog::openModal(FXuint placement) {
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
            } else if (modalStep->getDialogArguments()) {
                handle(myApplicationWindow->getInternalTest(), modalStep->getSelector(), modalStep->getDialogArguments());
            }
        }
    } else {
        myTesting = false;
        getApp()->runModalFor(this);
    }
    return myResult;
}


GNEDialog::Result
GNEDialog::getResult() const {
    return myResult;
}


long
GNEDialog::closeDialogAccepting() {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, TRUE);
    }
    // hide dialog
    hide();
    // set result
    myResult = Result::ACCEPT;
    return 1;
}


long
GNEDialog::closeDialogDeclining() {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, TRUE);
    }
    // hide dialog
    hide();
    // set result
    myResult = Result::DECLINE;
    return 1;
}


long
GNEDialog::closeDialogCanceling() {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, FALSE);
    }
    // hide dialog
    hide();
    // set result
    myResult = Result::CANCEL;
    return 0;
}