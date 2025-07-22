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

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/tests/InternalTest.h>
#include <utils/tests/InternalTestStep.h>

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

MFXDialogBox::MFXDialogBox(FXApp* a, const FXString& name, FXuint opts, FXint x, FXint y,
                           FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs):
    FXDialogBox(a, name, opts, x, y, w, h, pl, pr, pt, pb, hs, vs) {
}


MFXDialogBox::MFXDialogBox(FXWindow* owner, const FXString& name, FXuint opts, FXint x, FXint y,
                           FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs):
    FXDialogBox(owner, name, opts, x, y, w, h, pl, pr, pt, pb, hs, vs) {
}


bool
MFXDialogBox::openModal(InternalTest* internalTests, FXuint placement) {
    // create and show dialog
    create();
    show(placement);
    // refresh the application
    getApp()->refresh();
    // continue depending on whether we are testing or not
    if (internalTests) {
        myTesting = true;
        // execute every modal dialog test step
        for (const auto& modalStep : internalTests->getCurrentStep()->getModalDialogTestSteps()) {
            // this will be unified
            if (modalStep->getEvent()) {
                handle(internalTests, modalStep->getSelector(), modalStep->getEvent());
            } else if (modalStep->getDialogTest()) {
                handle(internalTests, modalStep->getSelector(), modalStep->getDialogTest());
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