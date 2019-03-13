/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_FixDemandElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
/// @version $Id$
///
// Dialog used to fix demand elements during saving
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <netbuild/NBEdge.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/demandelements/GNERouteHandler.h>

#include "GNEDialog_FixDemandElements.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog_FixDemandElements) GNEDialog_FixDemandElementsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,                  GNEDialog_FixDemandElements::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,  GNEDialog_FixDemandElements::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,  GNEDialog_FixDemandElements::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEDialog_FixDemandElements, FXDialogBox, GNEDialog_FixDemandElementsMap, ARRAYNUMBER(GNEDialog_FixDemandElementsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_FixDemandElements::GNEDialog_FixDemandElements(GNEViewNet* viewNet, const std::vector<GNEDemandElement*>& invalidDemandElements) :
    FXDialogBox(viewNet->getApp(), ("Fix additional problems"), GUIDesignDialogBoxExplicit, 0, 0, 478, 350, 0, 0, 0, 0),
    myViewNet(viewNet),
    myInvalidDemandElements(invalidDemandElements) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_BUSSTOP));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create label for table
    new FXLabel(mainFrame, "List of Stopping places and E2 detectors with conflicts", nullptr, GUIDesignLabelCenterThick);
    // Create table, copy intervals and update table
    myTable = new FXTable(mainFrame, this, MID_GNE_FIXSTOPPINGPLACES_CHANGE, GUIDesignTableAdditionals);
    myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myTable->setEditable(false);
    // clear table
    myTable->clearItems();
    // set number of rows
    myTable->setTableSize((int)myInvalidDemandElements.size(), 3);
    // Configure list
    myTable->setVisibleColumns(4);
    myTable->setColumnWidth(0, GUIDesignTableIconCellWidth);
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
    for (auto i : myInvalidDemandElements) {
        // Set icon
        item = new FXTableItem("", i->getIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(indexRow, 0, item);
        // Set ID
        item = new FXTableItem(i->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 1, item);
        // Set conflict
        item = new FXTableItem(i->getDemandElementProblem().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
    // create position options
    myPositionOptions.buildPositionOptions(this, mainFrame);
    // check if position options has to be disabled
    if (myInvalidDemandElements.empty()) {
        myPositionOptions.disablePositionOptions();
    }
    // create consecutive lane options
    myConsecutiveLaneOptions.buildConsecutiveLaneOptions(this, mainFrame);
    // create dialog buttons bot centered
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, FXWindow::tr("&Accept"), GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, FXWindow::tr("&Cancel"), GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}


GNEDialog_FixDemandElements::~GNEDialog_FixDemandElements() {
}


long
GNEDialog_FixDemandElements::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    myPositionOptions.selectOption(obj);
    myConsecutiveLaneOptions.selectOption(obj);
    return 1;
}


long
GNEDialog_FixDemandElements::onCmdAccept(FXObject*, FXSelector, void*) {
    bool continueSaving = true;
    // first check options from single lane additionals
    if (myInvalidDemandElements.size() > 0) {
        if (myPositionOptions.activateFriendlyPositionAndSave->getCheck() == TRUE) {
            myViewNet->getUndoList()->p_begin("change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid additionals");
            // iterate over invalid single lane elements to enable friendly position
            for (auto i : myInvalidDemandElements) {
                i->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->p_end();
        } else if (myPositionOptions.fixPositionsAndSave->getCheck() == TRUE) {
            myViewNet->getUndoList()->p_begin("fix positions of invalid additionals");
            // iterate over invalid single lane elements to fix positions
            for (auto i : myInvalidDemandElements) {
                i->fixDemandElementProblem();
            }
            myViewNet->getUndoList()->p_end();
        } else if (myPositionOptions.selectInvalidStopsAndCancel->getCheck() == TRUE) {
            myViewNet->getUndoList()->p_begin("select invalid additionals");
            // iterate over invalid single lane elements to select all elements
            for (auto i : myInvalidDemandElements) {
                i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->p_end();
            // abort saving
            continueSaving = false;
        }
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
GNEDialog_FixDemandElements::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal (abort saving)
    getApp()->stopModal(this, FALSE);
    return 1;
}


void
GNEDialog_FixDemandElements::PositionOptions::buildPositionOptions(GNEDialog_FixDemandElements* fixAdditionalPositions, FXVerticalFrame* mainFrame) {
    // create label for elements
    new FXLabel(mainFrame, "Select a solution for StoppingPlaces and E2 detectors:", nullptr, GUIDesignLabelCenterThick);
    // create horizontal frames for radio buttons
    FXHorizontalFrame* RadioButtons = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    // create Vertical Frame for left options
    FXVerticalFrame* RadioButtonsLeft = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    activateFriendlyPositionAndSave = new FXRadioButton(RadioButtonsLeft, "Activate friendlyPos and save\t\tFriendly pos parameter will be activated in all stopping places and E2 detectors",
            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    saveInvalid = new FXRadioButton(RadioButtonsLeft, "Save invalid positions\t\tSave stopping places and E2 detectors with invalid positions",
                                    fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create Vertical Frame for right options
    FXVerticalFrame* RadioButtonsRight = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    fixPositionsAndSave = new FXRadioButton(RadioButtonsRight, "Fix positions and save\t\tPosition of stopping places and E2 detectors will be fixed",
                                            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    selectInvalidStopsAndCancel = new FXRadioButton(RadioButtonsRight, "Select invalid additionals\t\tCancel saving of additionals and select invalid stopping places and E2 detectors",
            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "activateFriendlyPositionAndSave" as default
    activateFriendlyPositionAndSave->setCheck(true);
}


void
GNEDialog_FixDemandElements::PositionOptions::selectOption(FXObject* option) {
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
GNEDialog_FixDemandElements::PositionOptions::enablePositionOptions() {
    activateFriendlyPositionAndSave->enable();
    fixPositionsAndSave->enable();
    saveInvalid->enable();
    selectInvalidStopsAndCancel->enable();
}


void
GNEDialog_FixDemandElements::PositionOptions::disablePositionOptions() {
    activateFriendlyPositionAndSave->disable();
    fixPositionsAndSave->disable();
    saveInvalid->disable();
    selectInvalidStopsAndCancel->disable();
}


void
GNEDialog_FixDemandElements::ConsecutiveLaneOptions::buildConsecutiveLaneOptions(GNEDialog_FixDemandElements* fixAdditionalPositions, FXVerticalFrame* mainFrame) {
    // create label for elements
    new FXLabel(mainFrame, "Select a solution for Multilane E2 detectors:", nullptr, GUIDesignLabelCenterThick);
    // create horizontal frames for radio buttons
    FXHorizontalFrame* RadioButtons = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    // create Vertical Frame for left options
    FXVerticalFrame* RadioButtonsLeft = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    buildConnectionBetweenLanes = new FXRadioButton(RadioButtonsLeft, "Build connections between lanes\t\tNew connections will be created between non-connected lanes",
            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    removeInvalidElements = new FXRadioButton(RadioButtonsLeft, "Remove invalid E2 detectors\t\tRemove Multilane E2 Detectors with non-connected lanes",
            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // add a vertical separator beween both options
    new FXVerticalSeparator(RadioButtons, GUIDesignVerticalSeparator);
    // create Vertical Frame for right options
    FXVerticalFrame* RadioButtonsRight = new FXVerticalFrame(RadioButtons, GUIDesignAuxiliarVerticalFrame);
    activateFriendlyPositionAndSave = new FXRadioButton(RadioButtonsRight, "Activate friendlyPos and save\t\tFriendly pos parameter will be activated in all stopping places and E2 detectors",
            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    fixPositionsAndSave = new FXRadioButton(RadioButtonsRight, "Fix positions and save\t\tPosition of stopping places and E2 detectors will be fixed",
                                            fixAdditionalPositions, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // leave option "buildConnectionBetweenLanes" and "activateFriendlyPositionAndSave" as default
    buildConnectionBetweenLanes->setCheck(true);
    activateFriendlyPositionAndSave->setCheck(true);
}


void
GNEDialog_FixDemandElements::ConsecutiveLaneOptions::selectOption(FXObject* option) {
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
GNEDialog_FixDemandElements::ConsecutiveLaneOptions::enableConsecutiveLaneOptions() {
    buildConnectionBetweenLanes->enable();
    removeInvalidElements->enable();
    activateFriendlyPositionAndSave->enable();
    fixPositionsAndSave->enable();
}


void
GNEDialog_FixDemandElements::ConsecutiveLaneOptions::disableConsecutiveLaneOptions() {
    buildConnectionBetweenLanes->disable();
    removeInvalidElements->disable();
    activateFriendlyPositionAndSave->disable();
    fixPositionsAndSave->disable();
}

/****************************************************************************/
