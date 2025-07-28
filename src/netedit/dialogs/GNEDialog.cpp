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
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog) MFXDialogBoxMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT, GNEDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL, GNEDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET, GNEDialog::onCmdReset),
    // abort dialog
    FXMAPFUNC(SEL_CLOSE,    0,              GNEDialog::onCmdAbort),
    FXMAPFUNC(SEL_CHORE,    MID_GNE_ABORT,  GNEDialog::onCmdAbort),
    FXMAPFUNC(SEL_TIMEOUT,  MID_GNE_ABORT,  GNEDialog::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORT,  GNEDialog::onCmdAbort),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEDialog, FXDialogBox, MFXDialogBoxMap, ARRAYNUMBER(MFXDialogBoxMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEDialog::GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name, GUIIcon titleIcon,
                     Buttons buttons, FXuint opts, FXint x, FXint y, FXint w, FXint h,
                     FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs) :
    FXDialogBox(applicationWindow->getApp(), name.c_str(), opts, x, y, w, h, pl, pr, pt, pb, hs, vs),
    myApplicationWindow(applicationWindow) {
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(titleIcon));
    // create main frame
    auto mainFrame = new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // create content frame
    myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignAuxiliarFrame);
    // add separator
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
    // Create frame for buttons
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    // add horizontal frame used to center buttons horizontally
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create buttons according to the type
    switch (buttons) {
        case Buttons::OK: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("OK"), "", TL("OK"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonOK);
            break;
        }
        case Buttons::YES_NO: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Yes"), "", TL("Yes"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonOK);
            GUIDesigns::buildFXButton(buttonsFrame, TL("No"), "", TL("No"), GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonOK);
            break;
        }
        case Buttons::YES_NO_CANCEL: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Yes"), "", TL("Yes"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("No"), "", TL("No"), GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_ABORT, GUIDesignButtonCancel);
            break;
        }
        case Buttons::ACCEPT: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            break;
        }
        case Buttons::ACCEPT_CANCEL: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
            break;
        }
        case Buttons::ACCEPT_CANCEL_RESET: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Reset"), "", TL("Reset changes"), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_RESET, GUIDesignButtonReset);
            break;
        }
        case Buttons::KEEPNEW_KEEPOLD_CANCEL: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Keep new"), "", TL("Keep new changes"), GUIIconSubSys::getIcon(GUIIcon::YES), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Keep old"), "", TL("Keep old changes"), GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel changes"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_ABORT, GUIDesignButtonCancel);
            break;
        }
        case Buttons::RUN_CANCEL_RESET: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Run"), "", TL("Run"), GUIIconSubSys::getIcon(GUIIcon::START), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::NO), this, MID_GNE_ABORT, GUIDesignButtonCancel);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Reset"), "", TL("Reset changes"), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_RESET, GUIDesignButtonReset);
            break;
        }
        case Buttons::RUN_ADVANCED_CANCEL: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Run"), "", TL("Run"), GUIIconSubSys::getIcon(GUIIcon::START), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Advanced"), "", TL("Advanced options"), GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON), this, MID_GNE_BUTTON_ADVANCED, GUIDesignButtonAdvanced);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_ABORT, GUIDesignButtonCancel);
            break;
        }
        case Buttons::ABORT_RERUN_BACK_CLOSE: {
            myFocusButon = GUIDesigns::buildFXButton(buttonsFrame, TL("Abort"), "", TL("Abort running"), GUIIconSubSys::getIcon(GUIIcon::STOP), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Rerun"), "", TL("Rerun tool"), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Back"), "", TL("Back to tool dialog"), GUIIconSubSys::getIcon(GUIIcon::BACK), this, MID_GNE_BUTTON_RESET, GUIDesignButtonReset);
            break;
        }
        default:
            throw ProcessError("Invalid buttons combination in GNEDialog");
    }
    // add horizontal frame used to center buttons horizontally
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // check if add an extra line
    if (buttons == Buttons::ABORT_RERUN_BACK_CLOSE) {
        // add separator
        new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
        // Create frame for buttons
        FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
        // add horizontal frame used to center buttons horizontally
        new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
        // close button
        GUIDesigns::buildFXButton(buttonsFrame, TL("Close"), "", TL("Close dialog"), GUIIconSubSys::getIcon(GUIIcon::CLOSE), this, MID_GNE_ABORT, GUIDesignButtonReset);
        // add horizontal frame used to center buttons horizontally
        new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    }
}


GNEDialog::Result
GNEDialog::openModal(FXuint placement) {
    // create and show dialog
    create();
    show(placement);
    // refresh the application
    getApp()->refresh();
    // set focus in button
    myFocusButon->setFocus();
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
GNEDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    return closeDialogAccepting();
}


long
GNEDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    return closeDialogCanceling();
}


long
GNEDialog::onCmdReset(FXObject*, FXSelector, void*) {
    throw ProcessError("onCmdReset not implemented in GNEDialog, must be reimplemented in children");
}


long
GNEDialog::onCmdAbort(FXObject*, FXSelector, void*) {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, FALSE);
    }
    // hide dialog
    hide();
    // set result
    myResult = Result::ABORT;
    return 1;
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
GNEDialog::closeDialogCanceling() {
    // only stop modal if we're not testing
    if (myTesting == false) {
        getApp()->stopModal(this, TRUE);
    }
    // hide dialog
    hide();
    // set result
    myResult = Result::CANCEL;
    return 0;
}