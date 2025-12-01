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
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>

#include "GNEFixAdditionalElementsDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixAdditionalElementsDialog::PositionOptions) PositionOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixAdditionalElementsDialog::PositionOptions::onCmdSelectOption)
};

FXDEFMAP(GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions) ConsecutiveLaneOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::onCmdSelectOption)
};

// Object abstract implementation
FXIMPLEMENT(GNEFixAdditionalElementsDialog::PositionOptions,        MFXGroupBoxModule, PositionOptionsMap,          ARRAYNUMBER(PositionOptionsMap))
FXIMPLEMENT(GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions, MFXGroupBoxModule, ConsecutiveLaneOptionsMap,   ARRAYNUMBER(ConsecutiveLaneOptionsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

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
                                 this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // save invalid positions
    mySaveInvalids = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                     TL("Save invalid positions"), "",
                     TL("Save stopping places and E2 detectors with invalid positions"),
                     this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // fix positions
    myFixPositions = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                     TL("Fix positions and save"), "",
                     TL("Position of stopping places and E2 detectors will be fixed"),
                     this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // select invalids
    mySelectInvalids = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                       TL("Select invalid additionals"), "",
                       TL("Cancel saving of additionals and select invalid stopping places and E2 detectors"),
                       this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // register options
    registerOption(myActivateFriendlyPosition);
    registerOption(mySaveInvalids);
    registerOption(myFixPositions);
    registerOption(mySelectInvalids);
    // set option "activateFriendlyPosition" as default
    myActivateFriendlyPosition->setCheck(true);
}


void
GNEFixAdditionalElementsDialog::PositionOptions::selectInternalTestSolution(const std::string& solution) {
    // choose solution
    if (solution == "savePositionInvalids") {
        mySaveInvalids->setCheck(TRUE, TRUE);
    } else if (solution == "fixPositions") {
        myFixPositions->setCheck(TRUE, TRUE);
    } else if (solution == "selectPositionInvalids") {
        mySelectInvalids->setCheck(TRUE, TRUE);
    } else if (solution == "activatePositionFriendlyPos") {
        myActivateFriendlyPosition->setCheck(TRUE, TRUE);
    }
}


bool
GNEFixAdditionalElementsDialog::PositionOptions::applyFixOption() {
    if (myConflictedElements.size() > 0) {
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myActivateFriendlyPosition->getCheck() == TRUE) {
            undoList->begin(myConflictedElements.front().getElement(),
                            TLF("change % of invalid additionals", toString(SUMO_ATTR_FRIENDLY_POS)));
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", undoList);
            }
            undoList->end();
        } else if (myFixPositions->getCheck() == TRUE) {
            undoList->begin(myConflictedElements.front().getElement(),
                            TL("fix positions of invalid additionals"));
            // iterate over invalid single lane elements to fix positions
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->fixAdditionalProblem();
            }
            undoList->end();
        } else if (mySelectInvalids->getCheck() == TRUE) {
            undoList->begin(myConflictedElements.front().getElement(),
                            TL("select invalid additionals"));
            // iterate over invalid single lane elements to select all elements
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
            undoList->end();
            // abort saving
            return false;
        }
    }
    return true;
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
                                    this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // remove invalid elements
    myRemoveInvalidElements = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                              TL("Remove invalid E2 detectors"), "",
                              TL("Remove Multilane E2 Detectors with non-connected lanes"),
                              this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // activate friendly position
    myActivateFriendlyPosition = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                                 TL("Activate friendlyPos and save"), "",
                                 TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                                 this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // fix positions
    myFixPositions = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                     TL("Fix positions and save"), "",
                     TL("Position of stopping places and E2 detectors will be fixed"),
                     this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // register options
    registerOption(myBuildConnectionBetweenLanes);
    registerOption(myRemoveInvalidElements);
    registerOption(myActivateFriendlyPosition);
    registerOption(myFixPositions);
    // leave option "buildConnectionBetweenLanes" and "activateFriendlyPosition" as default
    myBuildConnectionBetweenLanes->setCheck(true);
    myActivateFriendlyPosition->setCheck(true);
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::selectInternalTestSolution(const std::string& /*solution*/) {
}


bool
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::applyFixOption() {
    if (myConflictedElements.size() > 0) {
        auto net = myFixElementDialogParent->getApplicationWindow()->getViewNet()->getNet();
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // all fix implies undo-redo
        undoList->begin(myConflictedElements.front().getElement(),
                        TL("fix multilane additionals problems"));
        // continue depending of solution
        if (myBuildConnectionBetweenLanes->getCheck() == TRUE) {
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->fixAdditionalProblem();
            }
            // we need to check if after first fix there is still invalid MultiLane Additionals with errors
            const std::vector<ConflictElement> copyOfConflictedElements = myConflictedElements;
            myConflictedElements.clear();
            for (const auto& conflictedElement : copyOfConflictedElements) {
                if (!conflictedElement.getElement()->isAdditionalValid()) {
                    myConflictedElements.push_back(conflictedElement);
                }
            }
        } else if (myRemoveInvalidElements->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (const auto& conflictedElement : myConflictedElements) {
                net->deleteAdditional(conflictedElement.getElement(), undoList);
            }
            // clear myInvalidMultiLaneAdditionals due there isn't more invalid multi lane additionals
            myConflictedElements.clear();
        }
        // fix problem of positions
        if (myActivateFriendlyPosition->getCheck() == TRUE) {
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", undoList);
            }
        } else if (myFixPositions->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->fixAdditionalProblem();
            }
        }
        // end undo list
        undoList->end();
    }
    return true;
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
    return true;
}

// ---------------------------------------------------------------------------
// GNEFixAdditionalElementsDialog - methods
// ---------------------------------------------------------------------------

GNEFixAdditionalElementsDialog::GNEFixAdditionalElementsDialog(GNEApplicationWindow* mainWindow,
        const std::vector<GNEAdditional*>& elements) :
    GNEFixElementsDialog(mainWindow, TL("Fix additional problems"), GUIIcon::MODEADDITIONAL,
                         DialogType::FIX_ADDITIONALELEMENTS) {
    // create position options
    myPositionOptions = new PositionOptions(this);
    // create consecutive lane options
    myConsecutiveLaneOptions = new ConsecutiveLaneOptions(this);
    // split invalidDemandElements in four groups
    std::vector<ConflictElement> invalidSingleLanes, invalidMultiLanes;
    // fill groups
    for (const auto& invalidAdditionalElement : elements) {
        // create conflict element
        auto fixElement = ConflictElement(invalidAdditionalElement,
                                          invalidAdditionalElement->getID(),
                                          invalidAdditionalElement->getACIcon(),
                                          invalidAdditionalElement->getAdditionalProblem());
        // add depending of element type
        if (invalidAdditionalElement->getTagProperty()->hasAttribute(SUMO_ATTR_LANE)) {
            invalidSingleLanes.push_back(fixElement);
        } else if (invalidAdditionalElement->getTagProperty()->hasAttribute(SUMO_ATTR_LANES)) {
            invalidMultiLanes.push_back(fixElement);
        }
    }
    // fill options
    myPositionOptions->setInvalidElements(invalidSingleLanes);
    myConsecutiveLaneOptions->setInvalidElements(invalidMultiLanes);
    // open modal dialog
    openDialog();
}


GNEFixAdditionalElementsDialog::~GNEFixAdditionalElementsDialog() {
}

/****************************************************************************/
