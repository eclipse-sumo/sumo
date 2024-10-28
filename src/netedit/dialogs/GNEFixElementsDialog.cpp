/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2023
///
// Dialog used to fix elements during saving
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEFixElementsDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixElementsDialog) GNEFixElementsDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixElementsDialog::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEFixElementsDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEFixElementsDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEFixElementsDialog, FXDialogBox, GNEFixElementsDialogMap, ARRAYNUMBER(GNEFixElementsDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFixElementsDialog::GNEFixElementsDialog(GNEViewNet* viewNet, const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals) :
    FXDialogBox(viewNet->getApp(), ("Fix additional problems"), GUIDesignDialogBoxExplicitStretchable(500, 380)),
    myViewNet(viewNet) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::BUSSTOP));
    // create main frame
    myMainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create AdditionalList
    myAdditionalList = new AdditionalList(this, invalidSingleLaneAdditionals, invalidMultiLaneAdditionals);
    // create position options
    myPositionOptions = new PositionOptions(this);
    // create consecutive lane options
    myConsecutiveLaneOptions = new ConsecutiveLaneOptions(this);
    // check if position options has to be disabled
    if (myAdditionalList->myInvalidSingleLaneAdditionals.empty()) {
        myPositionOptions->disablePositionOptions();
    }
    // check if consecutive lane options has to be disabled
    if (myAdditionalList->myInvalidMultiLaneAdditionals.empty()) {
        myConsecutiveLaneOptions->disableConsecutiveLaneOptions();
    }
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(myMainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("&Accept"), "", "", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("&Cancel"), "", "", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}


GNEFixElementsDialog::~GNEFixElementsDialog() {
}


long
GNEFixElementsDialog::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    myPositionOptions->selectOption(obj);
    myConsecutiveLaneOptions->selectOption(obj);
    return 1;
}


long
GNEFixElementsDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    bool continueSaving = true;
    // first check options from single lane additionals
    if (myAdditionalList->myInvalidSingleLaneAdditionals.size() > 0) {
        if (myPositionOptions->activateFriendlyPositionAndSave->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid additionals");
            // iterate over invalid single lane elements to enable friendly position
            for (auto i : myAdditionalList->myInvalidSingleLaneAdditionals) {
                i->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
        } else if (myPositionOptions->fixPositionsAndSave->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "fix positions of invalid additionals");
            // iterate over invalid single lane elements to fix positions
            for (auto i : myAdditionalList->myInvalidSingleLaneAdditionals) {
                i->fixAdditionalProblem();
            }
            myViewNet->getUndoList()->end();
        } else if (myPositionOptions->selectInvalidStopsAndCancel->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(myAdditionalList->myInvalidSingleLaneAdditionals.front(),
                                            "select invalid additionals");
            // iterate over invalid single lane elements to select all elements
            for (auto i : myAdditionalList->myInvalidSingleLaneAdditionals) {
                i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
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
            for (auto i : myAdditionalList->myInvalidMultiLaneAdditionals) {
                i->fixAdditionalProblem();
            }
            // we need to check if after first fix there is still  Invalid MultiL-ane Additionals with errors
            auto copyOfInvalidMultiLaneAdditionals = myAdditionalList->myInvalidMultiLaneAdditionals;
            myAdditionalList->myInvalidMultiLaneAdditionals.clear();
            for (auto i : copyOfInvalidMultiLaneAdditionals) {
                if (!i->isAdditionalValid()) {
                    myAdditionalList->myInvalidMultiLaneAdditionals.push_back(i);
                }
            }
        } else if (myConsecutiveLaneOptions->removeInvalidElements->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (auto i : myAdditionalList->myInvalidMultiLaneAdditionals) {
                myViewNet->getNet()->deleteAdditional(i, myViewNet->getUndoList());
            }
            // clear myInvalidMultiLaneAdditionals due there isn't more invalid multi lane additionals
            myAdditionalList->myInvalidMultiLaneAdditionals.clear();
        }
        // fix problem of positions
        if (myPositionOptions->activateFriendlyPositionAndSave->getCheck() == TRUE) {
            // iterate over invalid single lane elements to enable friendly position
            for (auto i : myAdditionalList->myInvalidSingleLaneAdditionals) {
                i->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
        } else if (myPositionOptions->fixPositionsAndSave->getCheck() == TRUE) {
            // iterate over invalid single lane elements to fix positions
            for (auto i : myAdditionalList->myInvalidSingleLaneAdditionals) {
                i->fixAdditionalProblem();
            }
        }
        myViewNet->getUndoList()->end();
    }
    if (continueSaving) {
        // stop modal with TRUE (continue saving)
        getApp()->stopModal(this, TRUE);
    } else {
        // stop modal with TRUE (abort saving)
        getApp()->stopModal(this, FALSE);
    }
    return 1;
}


long
GNEFixElementsDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal (abort saving)
    getApp()->stopModal(this, FALSE);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::DemandList - methods
// ---------------------------------------------------------------------------

GNEFixElementsDialog::AdditionalList::AdditionalList(GNEFixElementsDialog* fixAdditionalPositions, const std::vector<GNEAdditional*>& invalidSingleLaneAdditionals, const std::vector<GNEAdditional*>& invalidMultiLaneAdditionals) :
    FXGroupBox(fixAdditionalPositions->myMainFrame, "Stopping places and E2 detectors with conflicts", GUIDesignGroupBoxFrameFill),
    myInvalidSingleLaneAdditionals(invalidSingleLaneAdditionals),
    myInvalidMultiLaneAdditionals(invalidMultiLaneAdditionals) {
    // Create table, copy intervals and update table
    myTable = new FXTable(this, this, MID_GNE_FIXSTOPPINGPLACES_CHANGE, GUIDesignTableAdditionals);
    myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myTable->setEditable(false);
    // clear table
    myTable->clearItems();
    // set number of rows
    myTable->setTableSize(int(myInvalidSingleLaneAdditionals.size() + myInvalidMultiLaneAdditionals.size()), 3);
    // Configure list
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
    for (auto i : myInvalidSingleLaneAdditionals) {
        // Set icon
        item = new FXTableItem("", i->getACIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(i->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // Set conflict
        item = new FXTableItem(i->getAdditionalProblem().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
    // iterate over multi lane additionals
    for (auto i : myInvalidMultiLaneAdditionals) {
        // Set icon
        item = new FXTableItem("", i->getACIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(i->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // set conflict
        item = new FXTableItem((i->getAdditionalProblem()).c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}

// ---------------------------------------------------------------------------
// GNEFixElementsDialog::PositionOptions - methods
// ---------------------------------------------------------------------------

GNEFixElementsDialog::PositionOptions::PositionOptions(GNEFixElementsDialog* fixAdditionalPositions) :
    MFXGroupBoxModule(fixAdditionalPositions->myMainFrame, "Select a solution for StoppingPlaces and E2 detectors") {
    // create horizontal frames for radio buttons
    FXHorizontalFrame* RadioButtons = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    // create Vertical Frame for left options
    FXVerticalFrame* RadioButtonsLeft = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    activateFriendlyPositionAndSave = GUIDesigns::buildFXRadioButton(RadioButtonsLeft, TL("Activate friendlyPos and save"), "", TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                                      fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    saveInvalid = GUIDesigns::buildFXRadioButton(RadioButtonsLeft, TL("Save invalid positions"), "", TL("Save stopping places and E2 detectors with invalid positions"),
                  fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create Vertical Frame for right options
    FXVerticalFrame* RadioButtonsRight = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    fixPositionsAndSave = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Fix positions and save"), "", TL("Position of stopping places and E2 detectors will be fixed"),
                          fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    selectInvalidStopsAndCancel = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Select invalid additionals"), "", TL("Cancel saving of additionals and select invalid stopping places and E2 detectors"),
                                  fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "activateFriendlyPositionAndSave" as default
    activateFriendlyPositionAndSave->setCheck(true);
}


void
GNEFixElementsDialog::PositionOptions::selectOption(FXObject* option) {
    if (option == activateFriendlyPositionAndSave) {
        activateFriendlyPositionAndSave->setCheck(true);
        fixPositionsAndSave->setCheck(false);
        saveInvalid->setCheck(false);
        selectInvalidStopsAndCancel->setCheck(false);
    } else if (option == fixPositionsAndSave) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(true);
        saveInvalid->setCheck(false);
        selectInvalidStopsAndCancel->setCheck(false);
    } else if (option == saveInvalid) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(false);
        saveInvalid->setCheck(true);
        selectInvalidStopsAndCancel->setCheck(false);
    } else if (option == selectInvalidStopsAndCancel) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(false);
        saveInvalid->setCheck(false);
        selectInvalidStopsAndCancel->setCheck(true);
    }
}


void
GNEFixElementsDialog::PositionOptions::enablePositionOptions() {
    activateFriendlyPositionAndSave->enable();
    fixPositionsAndSave->enable();
    saveInvalid->enable();
    selectInvalidStopsAndCancel->enable();
}


void
GNEFixElementsDialog::PositionOptions::disablePositionOptions() {
    activateFriendlyPositionAndSave->disable();
    fixPositionsAndSave->disable();
    saveInvalid->disable();
    selectInvalidStopsAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixElementsDialog::ConsecutiveLaneOptions - methods
// ---------------------------------------------------------------------------

GNEFixElementsDialog::ConsecutiveLaneOptions::ConsecutiveLaneOptions(GNEFixElementsDialog* fixAdditionalPositions) :
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
    activateFriendlyPositionAndSave = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Activate friendlyPos and save"), "", TL("Friendly pos parameter will be activated in all stopping places and E2 detectors"),
                                      fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    fixPositionsAndSave = GUIDesigns::buildFXRadioButton(RadioButtonsRight, TL("Fix positions and save"), "", TL("Position of stopping places and E2 detectors will be fixed"),
                          fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "buildConnectionBetweenLanes" and "activateFriendlyPositionAndSave" as default
    buildConnectionBetweenLanes->setCheck(true);
    activateFriendlyPositionAndSave->setCheck(true);
}


void
GNEFixElementsDialog::ConsecutiveLaneOptions::selectOption(FXObject* option) {
    // set top buttons
    if (option == buildConnectionBetweenLanes) {
        buildConnectionBetweenLanes->setCheck(true);
        removeInvalidElements->setCheck(false);
    } else if (option == removeInvalidElements) {
        buildConnectionBetweenLanes->setCheck(false);
        removeInvalidElements->setCheck(true);
    }
    // set down buttons
    if (option == activateFriendlyPositionAndSave) {
        activateFriendlyPositionAndSave->setCheck(true);
        fixPositionsAndSave->setCheck(false);
    } else if (option == fixPositionsAndSave) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(true);
    }
}


void
GNEFixElementsDialog::ConsecutiveLaneOptions::enableConsecutiveLaneOptions() {
    buildConnectionBetweenLanes->enable();
    removeInvalidElements->enable();
    activateFriendlyPositionAndSave->enable();
    fixPositionsAndSave->enable();
}


void
GNEFixElementsDialog::ConsecutiveLaneOptions::disableConsecutiveLaneOptions() {
    buildConnectionBetweenLanes->disable();
    removeInvalidElements->disable();
    activateFriendlyPositionAndSave->disable();
    fixPositionsAndSave->disable();
}


/****************************************************************************/
