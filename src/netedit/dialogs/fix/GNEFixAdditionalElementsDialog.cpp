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
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixAdditionalElementsDialog) GNEFixAdditionalElementsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixAdditionalElementsDialog::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEFixAdditionalElementsDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEFixAdditionalElementsDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEFixAdditionalElementsDialog, GNEFixElementsDialog, GNEFixAdditionalElementsMap, ARRAYNUMBER(GNEFixAdditionalElementsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFixAdditionalElementsDialog::GNEFixAdditionalElementsDialog(GNEViewNet* viewNet) :
    GNEFixElementsDialog(viewNet, TL("Fix additional problems"), GUIIcon::BUSSTOP, 500, 380) {
    // create AdditionalList
    myAdditionalList = new AdditionalList(this);
    // create position options
    myPositionOptions = new PositionOptions(this);
    // create consecutive lane options
    myConsecutiveLaneOptions = new ConsecutiveLaneOptions(this);
    // create buttons
    myButtons = new Buttons(this);
}


GNEFixAdditionalElementsDialog::~GNEFixAdditionalElementsDialog() {
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
    // set focus in accept button
    myButtons->myAcceptButton->setFocus();
    // open modal dialog
    return openModal();
}


void
GNEFixAdditionalElementsDialog::runInternalTest(const InternalTestStep::DialogTest* dialogTest) {
    // chooose solution
    if (dialogTest->fixSolution == "savePositionInvalids") {
        myPositionOptions->saveInvalids->setCheck(TRUE, TRUE);
    } else if (dialogTest->fixSolution == "fixPositions") {
        myPositionOptions->fixPositions->setCheck(TRUE, TRUE);
    } else if (dialogTest->fixSolution == "selectPositionInvalids") {
        myPositionOptions->selectInvalids->setCheck(TRUE, TRUE);
    } else if (dialogTest->fixSolution == "activatePositionFriendlyPos") {
        myPositionOptions->activateFriendlyPosition->setCheck(TRUE, TRUE);
    }
    // accept changes
    onCmdAccept(nullptr, 0, nullptr);
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
            myViewNet->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid additionals");
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
        } else if (myPositionOptions->fixPositions->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "fix positions of invalid additionals");
            // iterate over invalid single lane elements to fix positions
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->fixAdditionalProblem();
            }
            myViewNet->getUndoList()->end();
        } else if (myPositionOptions->selectInvalids->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "select invalid additionals");
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
            // abort saving
            continueSaving = false;
        }
    }
    // now check options from multi lane additionals
    if (myAdditionalList->myInvalidMultiLaneAdditionals.size() > 0) {
        myViewNet->getUndoList()->begin(myAdditionalList->myInvalidMultiLaneAdditionals.front(),
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
                myViewNet->getNet()->deleteAdditional(invalidMultiLaneAdditional, myViewNet->getUndoList());
            }
            // clear myInvalidMultiLaneAdditionals due there isn't more invalid multi lane additionals
            myAdditionalList->myInvalidMultiLaneAdditionals.clear();
        }
        // fix problem of positions
        if (myPositionOptions->activateFriendlyPosition->getCheck() == TRUE) {
            // iterate over invalid single lane elements to enable friendly position
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
        } else if (myPositionOptions->fixPositions->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (const auto& invalidSingleLaneAdditional : myAdditionalList->myInvalidSingleLaneAdditionals) {
                invalidSingleLaneAdditional->fixAdditionalProblem();
            }
        }
        myViewNet->getUndoList()->end();
    }
    return closeFixDialog(continueSaving);
}


long
GNEFixAdditionalElementsDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    return closeFixDialog(false);
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::DemandList - methods
// ---------------------------------------------------------------------------

GNEFixAdditionalElementsDialog::AdditionalList::AdditionalList(GNEFixAdditionalElementsDialog* fixAdditionalPositions) :
    FXGroupBox(fixAdditionalPositions->myMainFrame, "Stopping places and E2 detectors with conflicts", GUIDesignGroupBoxFrameFill) {
    // Create table
    myTable = new FXTable(this, this, MID_GNE_FIXSTOPPINGPLACES_CHANGE, GUIDesignTableAdditionals);
}


void
GNEFixAdditionalElementsDialog::AdditionalList::updateList(const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals) {
    // update containers
    myInvalidSingleLaneAdditionals = invalidSingleLaneAdditionals;
    myInvalidMultiLaneAdditionals = invalidMultiLaneAdditionals;
    // clear table
    myTable->clearItems();
    // set number of rows
    myTable->setTableSize(int(myInvalidSingleLaneAdditionals.size() + myInvalidMultiLaneAdditionals.size()), 3);
    // configure table
    myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myTable->setEditable(false);
    myTable->setVisibleColumns(4);
    myTable->setColumnWidth(0, GUIDesignHeight);
    myTable->setColumnWidth(1, 160);
    myTable->setColumnWidth(2, 280);
    myTable->setColumnText(0, "");
    myTable->setColumnText(1, toString(SUMO_ATTR_ID).c_str());
    myTable->setColumnText(2, "Conflict");
    myTable->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = nullptr;
    // iterate over single lane additionals
    for (const auto& invalidSingleLaneAdditional : myInvalidSingleLaneAdditionals) {
        // Set icon
        item = new FXTableItem("", invalidSingleLaneAdditional->getACIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(invalidSingleLaneAdditional->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // Set conflict
        item = new FXTableItem(invalidSingleLaneAdditional->getAdditionalProblem().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
    // iterate over multi lane additionals
    for (const auto& invalidMultiLaneAdditional : myInvalidMultiLaneAdditionals) {
        // Set icon
        item = new FXTableItem("", invalidMultiLaneAdditional->getACIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(invalidMultiLaneAdditional->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // set conflict
        item = new FXTableItem((invalidMultiLaneAdditional->getAdditionalProblem()).c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}

// ---------------------------------------------------------------------------
// GNEFixAdditionalElementsDialog::PositionOptions - methods
// ---------------------------------------------------------------------------

GNEFixAdditionalElementsDialog::PositionOptions::PositionOptions(GNEFixAdditionalElementsDialog* fixAdditionalPositions) :
    MFXGroupBoxModule(fixAdditionalPositions->myMainFrame, "Select a solution for StoppingPlaces and E2 detectors") {
    // create horizontal frames for radio buttons
    FXHorizontalFrame* RadioButtons = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    // create Vertical Frame for left options
    FXVerticalFrame* RadioButtonsLeft = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    activateFriendlyPosition = GUIDesigns::buildFXRadioButton(RadioButtonsLeft, TL("Activate friendlyPos and save"), "", TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                               fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    saveInvalids = GUIDesigns::buildFXRadioButton(RadioButtonsLeft, TL("Save invalid positions"), "", TL("Save stopping places and E2 detectors with invalid positions"),
                   fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create Vertical Frame for right options
    FXVerticalFrame* RadioButtonsRight = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    fixPositions = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Fix positions and save"), "", TL("Position of stopping places and E2 detectors will be fixed"),
                   fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    selectInvalids = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Select invalid additionals"), "", TL("Cancel saving of additionals and select invalid stopping places and E2 detectors"),
                     fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "activateFriendlyPosition" as default
    activateFriendlyPosition->setCheck(true);
}


void
GNEFixAdditionalElementsDialog::PositionOptions::selectOption(FXObject* option) {
    if (option == activateFriendlyPosition) {
        activateFriendlyPosition->setCheck(true);
        fixPositions->setCheck(false);
        saveInvalids->setCheck(false);
        selectInvalids->setCheck(false);
    } else if (option == fixPositions) {
        activateFriendlyPosition->setCheck(false);
        fixPositions->setCheck(true);
        saveInvalids->setCheck(false);
        selectInvalids->setCheck(false);
    } else if (option == saveInvalids) {
        activateFriendlyPosition->setCheck(false);
        fixPositions->setCheck(false);
        saveInvalids->setCheck(true);
        selectInvalids->setCheck(false);
    } else if (option == selectInvalids) {
        activateFriendlyPosition->setCheck(false);
        fixPositions->setCheck(false);
        saveInvalids->setCheck(false);
        selectInvalids->setCheck(true);
    }
}


void
GNEFixAdditionalElementsDialog::PositionOptions::enablePositionOptions() {
    activateFriendlyPosition->enable();
    fixPositions->enable();
    saveInvalids->enable();
    selectInvalids->enable();
}


void
GNEFixAdditionalElementsDialog::PositionOptions::disablePositionOptions() {
    activateFriendlyPosition->disable();
    fixPositions->disable();
    saveInvalids->disable();
    selectInvalids->disable();
}

// ---------------------------------------------------------------------------
// GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions - methods
// ---------------------------------------------------------------------------

GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::ConsecutiveLaneOptions(GNEFixAdditionalElementsDialog* fixAdditionalPositions) :
    MFXGroupBoxModule(fixAdditionalPositions->myMainFrame, "Select a solution for Multilane E2 detectors") {
    // create horizontal frames for radio buttons
    FXHorizontalFrame* RadioButtons = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    // create Vertical Frame for left options
    FXVerticalFrame* RadioButtonsLeft = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    buildConnectionBetweenLanes = GUIDesigns::buildFXRadioButton(RadioButtonsLeft, TL("Build connections between lanes"), "", TL("New connections will be created between non-connected lanes"),
                                  fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    removeInvalidElements = GUIDesigns::buildFXRadioButton(RadioButtonsLeft, TL("Remove invalid E2 detectors"), "", TL("Remove Multilane E2 Detectors with non-connected lanes"),
                            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // add a vertical separator between both options
    new FXVerticalSeparator(RadioButtons, GUIDesignVerticalSeparator);
    // create Vertical Frame for right options
    FXVerticalFrame* RadioButtonsRight = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    activateFriendlyPosition = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Activate friendlyPos and save"), "", TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                               fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    fixPositions = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Fix positions and save"), "", TL("Position of stopping places and E2 detectors will be fixed"),
                   fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "buildConnectionBetweenLanes" and "activateFriendlyPosition" as default
    buildConnectionBetweenLanes->setCheck(true);
    activateFriendlyPosition->setCheck(true);
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::selectOption(FXObject* option) {
    // set top buttons
    if (option == buildConnectionBetweenLanes) {
        buildConnectionBetweenLanes->setCheck(true);
        removeInvalidElements->setCheck(false);
    } else if (option == removeInvalidElements) {
        buildConnectionBetweenLanes->setCheck(false);
        removeInvalidElements->setCheck(true);
    }
    // set down buttons
    if (option == activateFriendlyPosition) {
        activateFriendlyPosition->setCheck(true);
        fixPositions->setCheck(false);
    } else if (option == fixPositions) {
        activateFriendlyPosition->setCheck(false);
        fixPositions->setCheck(true);
    }
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::enableConsecutiveLaneOptions() {
    buildConnectionBetweenLanes->enable();
    removeInvalidElements->enable();
    activateFriendlyPosition->enable();
    fixPositions->enable();
}


void
GNEFixAdditionalElementsDialog::ConsecutiveLaneOptions::disableConsecutiveLaneOptions() {
    buildConnectionBetweenLanes->disable();
    removeInvalidElements->disable();
    activateFriendlyPosition->disable();
    fixPositions->disable();
}

/****************************************************************************/
