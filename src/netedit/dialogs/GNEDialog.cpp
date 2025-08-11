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

#include "GNEDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog) MFXDialogBoxMap[] = {
    // interaction
    FXMAPFUNC(SEL_KEYPRESS,     0,  GNEDialog::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,  GNEDialog::onKeyRelease),
    // buttons
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,      GNEDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,      GNEDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,       GNEDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,         GNEDialog::onCmdRun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_BACK,        GNEDialog::onCmdBack),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADVANCED,    GNEDialog::onCmdAdvanced),
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

GNEDialog::GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
                     GUIIcon titleIcon, Buttons buttons, OpenType openType) :
    FXDialogBox(applicationWindow->getApp(), name.c_str(), GUIDesignGNEDialog
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    myApplicationWindow(applicationWindow),
    myOpenType(openType) {
    // build dialog
    buildDialog(titleIcon, buttons);
}


GNEDialog::GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
                     GUIIcon titleIcon, Buttons buttons, OpenType openType,
                     const int width, const int height) :
    FXDialogBox(applicationWindow->getApp(), name.c_str(), GUIDesignGNEDialogExplicit,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    myApplicationWindow(applicationWindow),
    myOpenType(openType) {
    // build dialog
    buildDialog(titleIcon, buttons);
    // set explicit size
    resize(width, height);
}


GNEDialog::GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
                     GUIIcon titleIcon, Buttons buttons, OpenType openType,
                     ResizeMode resizeMode) :
    FXDialogBox(applicationWindow->getApp(), name.c_str(),
                (resizeMode == ResizeMode::SHRINKABLE) ? GUIDesignGNEDialogShrinkable :
                (resizeMode == ResizeMode::STRETCHABLE) ? GUIDesignGNEDialogStretchable :
                GUIDesignGNEDialogResizable,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    myApplicationWindow(applicationWindow),
    myOpenType(openType) {
    // build dialog
    buildDialog(titleIcon, buttons);
}


GNEDialog::GNEDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
                     GUIIcon titleIcon, Buttons buttons, OpenType openType,
                     ResizeMode resizeMode, const int width, const int height) :
    FXDialogBox(applicationWindow->getApp(), name.c_str(),
                (resizeMode == ResizeMode::SHRINKABLE) ? GUIDesignGNEDialogShrinkableExplicit :
                (resizeMode == ResizeMode::STRETCHABLE) ? GUIDesignGNEDialogStretchableExplicit :
                GUIDesignGNEDialogResizableExplicit,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    myApplicationWindow(applicationWindow),
    myOpenType(openType) {
    // build dialog
    buildDialog(titleIcon, buttons);
    // set explicit size
    resize(width, height);
}


GNEDialog::Result
GNEDialog::getResult() const {
    return myResult;
}


GNEApplicationWindow*
GNEDialog::getApplicationWindow() const {
    return myApplicationWindow;
}


FXVerticalFrame*
GNEDialog::getContentFrame() const {
    return myContentFrame;
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
    throw ProcessError("onCmdReset function must be reimplemented in GNEDialog children");
}


long
GNEDialog::onCmdRun(FXObject*, FXSelector, void*) {
    throw ProcessError("onCmdRun function must be reimplemented in GNEDialog children");
}


long
GNEDialog::onCmdBack(FXObject*, FXSelector, void*) {
    throw ProcessError("onCmdBack function must be reimplemented in GNEDialog children");
}


long
GNEDialog::onCmdAdvanced(FXObject*, FXSelector, void*) {
    throw ProcessError("onCmdAdvanced function must be reimplemented in GNEDialog children");
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
GNEDialog::onKeyPress(FXObject* obj, FXSelector sel, void* ptr) {
    if (myTesting && (obj != myApplicationWindow->getInternalTest())) {
        return 1;
    } else {
        return FXDialogBox::onKeyPress(obj, sel, ptr);
    }
}


long
GNEDialog::onKeyRelease(FXObject* obj, FXSelector sel, void* ptr) {
    if (myTesting && (obj != myApplicationWindow->getInternalTest())) {
        return 1;
    } else {
        return FXDialogBox::onKeyRelease(obj, sel, ptr);
    }
}


void
GNEDialog::openDialog() {
    // create dialog
    create();
    // set focus in button
    myFocusButon->setFocus();
    // show in the center of app
    show(PLACEMENT_OWNER);
    // continue depending on whether we are testing or not
    const auto internalTest = myApplicationWindow->getInternalTest();
    if (internalTest) {
        myTesting = true;
        // execute every dialog step
        while (internalTest->getCurrentStep() && internalTest->getCurrentStep()->getCategory() == InternalTestStep::Category::DIALOG) {
            // get current step and set next step
            const auto testStep = internalTest->setNextStep();
            // continue depending on the dialog argument action
            switch (testStep->getDialogArgument()->action) {
                case InternalTestStep::DialogArgument::Action::ACCEPT:
                    onCmdAccept(internalTest, 0, nullptr);
                    break;
                case InternalTestStep::DialogArgument::Action::CANCEL:
                    onCmdCancel(internalTest, 0, nullptr);
                    break;
                case InternalTestStep::DialogArgument::Action::RESET:
                    onCmdReset(internalTest, 0, nullptr);
                    break;
                case InternalTestStep::DialogArgument::Action::ABORT:
                    onCmdAbort(nullptr, 0, nullptr);
                    break;
                case InternalTestStep::DialogArgument::Action::CUSTOM:
                    runInternalTest(testStep->getDialogArgument());
                    break;
                default:
                    handle(internalTest, testStep->getSelector(), testStep->getEvent());
                    break;
            }
        }
    } else {
        myTesting = false;
        // continue depending on the dialog type
        if (myOpenType == OpenType::MODAL) {
            // run modal dialog
            getApp()->runModalFor(this);
        }
    }
}


long
GNEDialog::closeDialogAccepting() {
    // check if stopping modal dialog
    if (!myTesting && (myOpenType == OpenType::MODAL)) {
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
    // check if stopping modal dialog
    if (!myTesting && (myOpenType == OpenType::MODAL)) {
        getApp()->stopModal(this, TRUE);
    }
    // hide dialog
    hide();
    // set result
    myResult = Result::CANCEL;
    return 0;
}


void
GNEDialog::buildDialog(GUIIcon titleIcon, GNEDialog::Buttons buttons) {
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(titleIcon));
    // create main frame
    auto mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create content frame
    myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignDialogContentFrame);
    // add separator
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
    // Create frame for buttons
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignDialogButtonsFrame);
    // add horizontal frame used to center buttons horizontally
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create buttons according to the type
    switch (buttons) {
        case Buttons::OK: {
            // ok button
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("OK"), "", TL("OK"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::YES_NO: {
            // yes button
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Yes"), "", TL("Yes"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // no button
            myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("No"), "", TL("No"),
                             GUIIconSubSys::getIcon(GUIIcon::NO), this,
                             MID_GNE_BUTTON_CANCEL, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::YES_NO_CANCEL: {
            // yes button
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Yes"), "", TL("Yes"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // no button
            myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("No"), "", TL("No"),
                             GUIIconSubSys::getIcon(GUIIcon::NO), this,
                             MID_GNE_BUTTON_CANCEL, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // cancel button
            myAbortButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"),
                            GUIIconSubSys::getIcon(GUIIcon::CANCEL), this,
                            MID_GNE_ABORT, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::ACCEPT: {
            // accept button
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::ACCEPT_CANCEL: {
            // accept button
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // cancel button
            myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"),
                             GUIIconSubSys::getIcon(GUIIcon::NO), this,
                             MID_GNE_BUTTON_CANCEL, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::ACCEPT_CANCEL_RESET: {
            // accept button
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // cancel button
            myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"),
                             GUIIconSubSys::getIcon(GUIIcon::NO), this,
                             MID_GNE_BUTTON_CANCEL, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // reset button
            myResetButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Reset"), "", TL("Reset changes"),
                            GUIIconSubSys::getIcon(GUIIcon::RESET), this,
                            MID_GNE_BUTTON_RESET, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::KEEPNEW_KEEPOLD_CANCEL: {
            // keep new button (using accept)
            myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Keep new"), "", TL("Keep new changes"),
                             GUIIconSubSys::getIcon(GUIIcon::YES), this,
                             MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // keep old button (using cancel)
            myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Keep old"), "", TL("Keep old changes"),
                             GUIIconSubSys::getIcon(GUIIcon::NO), this,
                             MID_GNE_BUTTON_CANCEL, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // cancel button (using abort)
            myAbortButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel changes"),
                            GUIIconSubSys::getIcon(GUIIcon::CANCEL), this,
                            MID_GNE_ABORT, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myAcceptButton;
            break;
        }
        case Buttons::RUN_CANCEL_RESET: {
            // run button
            myRunButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Run"), "", TL("Run"),
                                                    GUIIconSubSys::getIcon(GUIIcon::START), this,
                                                    MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // reset button
            myAbortButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"),
                            GUIIconSubSys::getIcon(GUIIcon::NO), this,
                            MID_GNE_ABORT, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // reset button
            myResetButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Reset"), "", TL("Reset changes"),
                            GUIIconSubSys::getIcon(GUIIcon::RESET), this,
                            MID_GNE_BUTTON_RESET, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myRunButton;
            break;
        }
        case Buttons::RUN_ADVANCED_CANCEL: {
            // run button
            myRunButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Run"), "", TL("Run"),
                                                    GUIIconSubSys::getIcon(GUIIcon::START), this,
                                                    MID_GNE_BUTTON_RUN, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // cancel button
            myAdvancedButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Advanced"), "", TL("Advanced options"),
                               GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON), this,
                               MID_GNE_BUTTON_ADVANCED, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // cancel button
            myAbortButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"),
                            GUIIconSubSys::getIcon(GUIIcon::CANCEL), this,
                            MID_GNE_ABORT, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myRunButton;
            break;
        }
        case Buttons::ABORT_RERUN_BACK_CLOSE: {
            // abort button (using cancel)
            myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Abort"), "", TL("Abort running"),
                             GUIIconSubSys::getIcon(GUIIcon::STOP), this,
                             MID_GNE_BUTTON_CANCEL, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // run button
            myRunButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Rerun"), "", TL("Rerun tool"),
                                                    GUIIconSubSys::getIcon(GUIIcon::RESET), this,
                                                    MID_GNE_BUTTON_RUN, GUIDesignButtonDialog);
            // buttons separator
            new FXHorizontalFrame(buttonsFrame, GUIDesignDialogButtonSeparator);
            // back button
            myBackButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Back"), "", TL("Back to tool dialog"),
                           GUIIconSubSys::getIcon(GUIIcon::BACK), this,
                           MID_GNE_BUTTON_BACK, GUIDesignButtonDialog);
            // set focus button
            myFocusButon = myCancelButton;
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
        GUIDesigns::buildFXButton(buttonsFrame, TL("Close"), "", TL("Close dialog"),
                                  GUIIconSubSys::getIcon(GUIIcon::CLOSE), this,
                                  MID_GNE_BUTTON_ACCEPT, GUIDesignButtonDialog);
        // add horizontal frame used to center buttons horizontally
        new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    }
}
