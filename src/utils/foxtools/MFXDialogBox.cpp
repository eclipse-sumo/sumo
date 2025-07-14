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
    FXMAPFUNC(SEL_CLOSE,    0,                      MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_ACCEPT, MFXDialogBox::onCmdAccept),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL, MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL, MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL, MFXDialogBox::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_INTERNALTEST,       MFXDialogBox::onCmdInternalTest),
};

// Object implementation
FXIMPLEMENT(MFXDialogBox, FXDialogBox, MFXDialogBoxMap, ARRAYNUMBER(MFXDialogBoxMap))

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


FXuint
MFXDialogBox::openModalDialog(InternalTest* internalTests, FXuint placement) {
    create();
    show(placement);
    getApp()->refresh();
    if (internalTests) {
        myTesting = true;
        // execute every modal dialog test step
        for (const auto& modalStep : internalTests->getCurrentStep()->getModalDialogTestSteps()) {
            if (modalStep->getEvent()) {
                handle(internalTests, modalStep->getSelector(), modalStep->getEvent());
            } else if (modalStep->getModalArguments()) {
                handle(internalTests, modalStep->getSelector(), modalStep->getModalArguments());
            }
        }
        return 1;
    } else {
        myTesting = false;
        return getApp()->runModalFor(this);
    }
}


void
MFXDialogBox::runInternalTest(const InternalTestStep::ModalArguments* /*modalArguments*/) {
    // temporal until #16893
}


long
MFXDialogBox::onCmdAccept(FXObject*, FXSelector, void*) {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, TRUE);
    }
    hide();
    return 1;
}


long
MFXDialogBox::onCmdCancel(FXObject*, FXSelector, void*) {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, FALSE);
    }
    hide();
    return 1;
}


long
MFXDialogBox::onCmdInternalTest(FXObject*, FXSelector, void* ptr) {
    auto modalArguments = static_cast<const InternalTestStep::ModalArguments*>(ptr);
    // ensure modalArguments is not null
    if (modalArguments) {
        // run internal test
        runInternalTest(modalArguments);
    }
    // complete
    return 1;
}


FXuint
MFXDialogBox::execute(FXuint placement) {
    return FXDialogBox::execute(placement);
}
