/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixAdditionalElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to fix additional elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEFixAdditionalElementsDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFixAdditionalElementsDialog::GNEFixAdditionalElementsDialog(GNEApplicationWindow *mainWindow) :
    GNEFixElementsDialog(mainWindow, TL("Fix additional problems"), GUIIcon::MODEADDITIONAL, 500, 380) {
    // create position options
    myPositionOptions = new PositionOptions(this);
    // create consecutive lane options
    myConsecutiveLaneOptions = new ConsecutiveLaneOptions(this);
}


GNEFixAdditionalElementsDialog::~GNEFixAdditionalElementsDialog() {
}


void
GNEFixAdditionalElementsDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    // run internal test in all modules
    myPositionOptions->runInternalTest(dialogArgument);
    myConsecutiveLaneOptions->runInternalTest(dialogArgument);
    // accept changes
    onCmdAccept(nullptr, 0, nullptr);
}


GNEDialog::Result
GNEFixAdditionalElementsDialog::openDialog(const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals,
                                     const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals) {
    myAdditionalList->updateList(invalidSingleLaneAdditionals, invalidMultiLaneAdditionals);
    // check if position options has to be disabled
    if (myAdditionalList->myInvalidSingleLaneAdditionals.empty()) {
        myPositionOptions->disablePositionOptions();
    }
    // check if consecutive lane options has to be disabled
    if (myAdditionalList->myInvalidMultiLaneAdditionals.empty()) {
        myConsecutiveLaneOptions->disableConsecutiveLaneOptions();
    }
    // open modal dialog
    return openModal();
}


long
GNEFixAdditionalElementsDialog::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    myPositionOptions->selectOption(obj);
    myConsecutiveLaneOptions->selectOption(obj);
    return 1;
}


long
GNEFixAdditionalElementsDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    bool continueSaving = true;
    // first check options from single lane additionals
    if (myAdditionalList->myInvalidSingleLaneAdditionals.size() > 0) {
        if (myPositionOptions->activateFriendlyPosition->getCheck() == TRUE) {
            myApplicationWindow->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid additionals");
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myApplicationWindow->getUndoList());
            }
            myApplicationWindow->getUndoList()->end();
        } else if (myPositionOptions->fixPositions->getCheck() == TRUE) {
            myApplicationWindow->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "fix positions of invalid additionals");
            // iterate over invalid single lane elements to fix positions
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->fixAdditionalProblem();
            }
            myApplicationWindow->getUndoList()->end();
        } else if (myPositionOptions->selectInvalids->getCheck() == TRUE) {
            myApplicationWindow->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "select invalid additionals");
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->setAttribute(GNE_ATTR_SELECTED, "true", myApplicationWindow->getUndoList());
            }
            myApplicationWindow->getUndoList()->end();
            // abort saving
            continueSaving = false;
        }
    }
    // now check options from multi lane additionals
    if (myAdditionalList->myInvalidMultiLaneAdditionals.size() > 0) {
        myApplicationWindow->getUndoList()->begin(myAdditionalList->myInvalidMultiLaneAdditionals.front(),
                                        "fix multilane additionals problems");
        // fix problems of consecutive lanes
        if (myConsecutiveLaneOptions->buildConnectionBetweenLanes->getCheck() == TRUE) {
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& invalidMultiLaneAdditional : myAdditionalList->myInvalidMultiLaneAdditionals) {
                invalidMultiLaneAdditional->fixAdditionalProblem();
            }
            // we need to check if after first fix there is still  Invalid MultiL-ane Additionals with errors
            const auto copyOfInvalidMultiLaneAdditionals = myAdditionalList->myInvalidMultiLaneAdditionals;
            myAdditionalList->myInvalidMultiLaneAdditionals.clear();
            for (const auto& invalidMultiLaneAdditional : copyOfInvalidMultiLaneAdditionals) {
                if (!invalidMultiLaneAdditional->isAdditionalValid()) {
                    myAdditionalList->myInvalidMultiLaneAdditionals.push_back(invalidMultiLaneAdditional);
                }
            }
        } else if (myConsecutiveLaneOptions->removeInvalidElements->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (const auto& invalidMultiLaneAdditional : myAdditionalList->myInvalidMultiLaneAdditionals) {
                myApplicationWindow->getViewNet()->getNet()->deleteAdditional(invalidMultiLaneAdditional, myApplicationWindow->getUndoList());
            }
            // clear myInvalidMultiLaneAdditionals due there isn't more invalid multi lane additionals
            myAdditionalList->myInvalidMultiLaneAdditionals.clear();
        }
        // fix problem of positions
        if (myPositionOptions->activateFriendlyPosition->getCheck() == TRUE) {
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myApplicationWindow->getUndoList());
            }
        } else if (myPositionOptions->fixPositions->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->fixAdditionalProblem();
            }
        }
        myApplicationWindow->getUndoList()->end();
    }
    return closeFixDialog(continueSaving);
}

// ---------------------------------------------------------------------------
// GNEFixAdditionalElementsDialog::PositionOptions - methods
// ---------------------------------------------------------------------------

GNEFixAdditionalElementsDialog::PositionOptions::PositionOptions(GNEFixAdditionalElementsDialog* fixAdditionalPositions) :
    GNEFixElementsDialog<GNEAdditional*>::FixOptions(fixAdditionalPositions, fixAdditionalPositions->myLeftFrame,
                                                     TL("Select a solution for StoppingPlaces and E2 detectors")) {
    // activate friendly position
    myActivateFriendlyPosition = GUIDesigns::buildFXRadioButton(myLeftFrameOptions, 
                                                                TL("Activate friendlyPos and save"), "",
                                                                TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                                                                this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // save invalid positions
    mySaveInvalids = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                                                    TL("Save invalid positions"), "", 
                                                    TL("Save stopping places and E2 detectors with invalid positions"),
                                                    this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // fix positions
    myFixPositions = GUIDesigns::buildFXRadioButton(myRightFrameOptions, 
                                                    TL("Fix positions and save"), "",
                                                    TL("Position of stopping places and E2 detectors will be fixed"),
                                                    this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // select invalids
    mySelectInvalids = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                                                      TL("Select invalid additionals"), "",
                                                      TL("Cancel saving of additionals and select invalid stopping places and E2 detectors"),
                                                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // set option "activateFriendlyPosition" as default
    myActivateFriendlyPosition->setCheck(true);
}


void
GNEFixAdditionalElementsDialog::PositionOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    // chooose solution
    if (dialogArgument->fixSolution == "savePositionInvalids") {
        mySaveInvalids->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "fixPositions") {
        myFixPositions->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "selectPositionInvalids") {
        mySelectInvalids->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "activatePositionFriendlyPos") {
        myActivateFriendlyPosition->setCheck(TRUE, TRUE);
    }
}


bool
GNEFixAdditionalElementsDialog::PositionOptions::applyFixOption() {
}


long
GNEFixAdditionalElementsDialog::PositionOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myActivateFriendlyPosition) {
        myActivateFriendlyPosition->setCheck(true);
        myFixPositions->setCheck(false);
        mySaveInvalids->setCheck(false);
        mySelectInvalids->setCheck(false);
    } else if (obj == myFixPositions) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(true);
        mySaveInvalids->setCheck(false);
        mySelectInvalids->setCheck(false);
    } else if (obj == mySaveInvalids) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(false);
        mySaveInvalids->setCheck(true);
        mySelectInvalids->setCheck(false);
    } else if (obj == mySelectInvalids) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(false);
        mySaveInvalids->setCheck(false);
        mySelectInvalids->setCheck(true);
    }
    return 1;
}


void
GNEFixAdditionalElementsDialog::PositionOptions::enableOptions() {
    myActivateFriendlyPosition->enable();
    myFixPositions->enable();
    mySaveInvalids->enable();
    mySelectInvalids->enable();
}


void
GNEFixAdditionalElementsDialog::PositionOptions::disableOptions() {
    myActivateFriendlyPosition->disable();
    myFixPositions->disable();
    mySaveInvalids->disable();
    mySelectInvalids->disable();
}

// ---------------------------------------------------------------------------
// GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions - methods
// ---------------------------------------------------------------------------

GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::ConsecutiveLaneOptions(GNEFixAdditionalElementsDialog* fixAdditionalPositions) :
    GNEFixElementsDialog<GNEAdditional*>::FixOptions(fixAdditionalPositions, fixAdditionalPositions->myLeftFrame,
                                                     TL("Select a solution for Multilane E2 detectors")) {
    // build connection between lanes
    myBuildConnectionBetweenLanes = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                                                                   TL("Build connections between lanes"), "",
                                                                   TL("New connections will be created between non-connected lanes"),
                                                                   this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // remove invalid elements
    myRemoveInvalidElements = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                                                             TL("Remove invalid E2 detectors"), "", 
                                                             TL("Remove Multilane E2 Detectors with non-connected lanes"),
                                                             this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // activate friendly position
    myActivateFriendlyPosition = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                                                                TL("Activate friendlyPos and save"), "",
                                                                TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                                                                this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // fix positions
    myFixPositions = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                                                    TL("Fix positions and save"), "",
                                                    TL("Position of stopping places and E2 detectors will be fixed"),
                                                     this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "buildConnectionBetweenLanes" and "activateFriendlyPosition" as default
    myBuildConnectionBetweenLanes->setCheck(true);
    myActivateFriendlyPosition->setCheck(true);
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
}


bool
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::applyFixOption() {

}


long
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    // set top buttons
    if (obj == myBuildConnectionBetweenLanes) {
        myBuildConnectionBetweenLanes->setCheck(true);
        myRemoveInvalidElements->setCheck(false);
    } else if (obj == myRemoveInvalidElements) {
        myBuildConnectionBetweenLanes->setCheck(false);
        myRemoveInvalidElements->setCheck(true);
    }
    // set down buttons
    if (obj == myActivateFriendlyPosition) {
        myActivateFriendlyPosition->setCheck(true);
        myFixPositions->setCheck(false);
    } else if (obj == myFixPositions) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(true);
    }
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::enableOptions() {
    myBuildConnectionBetweenLanes->enable();
    myRemoveInvalidElements->enable();
    myActivateFriendlyPosition->enable();
    myFixPositions->enable();
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::disableOptions() {
    myBuildConnectionBetweenLanes->disable();
    myRemoveInvalidElements->disable();
    myActivateFriendlyPosition->disable();
    myFixPositions->disable();
}

/****************************************************************************/
