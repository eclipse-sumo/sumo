/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVariableSpeedSignDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
// A class for edit phases of Variable Speed Signals
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEVariableSpeedSignDialog.h"
#include "GNEVariableSpeedSign.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEChange_VariableSpeedSignItem.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVariableSpeedSignDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_VARIABLESPEEDSIGN_ADDROW,       GNEVariableSpeedSignDialog::onCmdAddStep),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_VARIABLESPEEDSIGN_TABLE_STEPS,  GNEVariableSpeedSignDialog::onCmdClickedStep),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_VARIABLESPEEDSIGN_TABLE_STEPS,  GNEVariableSpeedSignDialog::onCmdClickedStep),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_VARIABLESPEEDSIGN_TABLE_STEPS,  GNEVariableSpeedSignDialog::onCmdClickedStep),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_VARIABLESPEEDSIGN_TABLE_STEPS,  GNEVariableSpeedSignDialog::onCmdEditStep),
};

// Object implementation
FXIMPLEMENT(GNEVariableSpeedSignDialog, GNEAdditionalDialog, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignDialog::GNEVariableSpeedSignDialog(GNEVariableSpeedSign* editedVariableSpeedSign) :
    GNEAdditionalDialog(editedVariableSpeedSign, 300, 400),
    myEditedVariableSpeedSign(editedVariableSpeedSign),
    myStepsValids(false) {

    // create Horizontal frame for row elements
    myAddStepFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    // create Button and Label
    myAddStepButton = new FXButton(myAddStepFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_VARIABLESPEEDSIGN_ADDROW, GUIDesignButtonIcon);
    new FXLabel(myAddStepFrame, ("Add new " + toString(SUMO_TAG_STEP)).c_str(), 0, GUIDesignLabelThick);

    // create List with the data
    myStepsTable = new FXTable(myContentFrame, this, MID_GNE_VARIABLESPEEDSIGN_TABLE_STEPS, GUIDesignTableAdditionals);
    myStepsTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myStepsTable->setSelTextColor(FXRGBA(0, 0, 0, 255));

    // update table
    updateTableSteps();

    // start a undo list for editing local to this additional
    initChanges();

    // Open dialog as modal
    openAsModalDialog();
}


GNEVariableSpeedSignDialog::~GNEVariableSpeedSignDialog() {}


GNEVariableSpeedSign*
GNEVariableSpeedSignDialog::getEditedVariableSpeedSign() const {
    return myEditedVariableSpeedSign;
}


long
GNEVariableSpeedSignDialog::onCmdAddStep(FXObject*, FXSelector, void*) {
    // Declare variables for time and speed
    GNEVariableSpeedSignStep* step = new GNEVariableSpeedSignStep(this);
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->add(new GNEChange_VariableSpeedSignItem(step, true), true);
    // Update table
    updateTableSteps();
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdEditStep(FXObject*, FXSelector, void*) {
    myStepsValids = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myStepsTable->getNumRows(); i++) {
        GNEVariableSpeedSignStep* step = myEditedVariableSpeedSign->getVariableSpeedSignSteps().at(i);
        if (step->isValid(SUMO_ATTR_TIME, myStepsTable->getItem(i, 0)->getText().text()) == false) {
            myStepsValids = false;
            myStepsTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (step->isValid(SUMO_ATTR_SPEED, myStepsTable->getItem(i, 1)->getText().text()) == false) {
            myStepsValids = false;
            myStepsTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            // set new values in Closing  reroute
            step->setAttribute(SUMO_ATTR_TIME, myStepsTable->getItem(i, 0)->getText().text(), myEditedVariableSpeedSign->getViewNet()->getUndoList());
            step->setAttribute(SUMO_ATTR_SPEED, myStepsTable->getItem(i, 1)->getText().text(), myEditedVariableSpeedSign->getViewNet()->getUndoList());
            // set Correct label
            myStepsTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myStepsTable->update();
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdClickedStep(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myEditedVariableSpeedSign->getVariableSpeedSignSteps().size(); i++) {
        if (myStepsTable->getItem(i, 3)->hasFocus()) {
            myStepsTable->removeRows(i);
            myEditedVariableSpeedSign->getViewNet()->getUndoList()->add(new GNEChange_VariableSpeedSignItem(myEditedVariableSpeedSign->getVariableSpeedSignSteps().at(i), false), true);
            // Update table
            updateTableSteps();
            return 1;
        }
    }
    return 0;
}


long
GNEVariableSpeedSignDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myStepsValids == false) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        //
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, ("Error updating " + toString(SUMO_TAG_VSS) + " " + toString(SUMO_TAG_STEP)).c_str(), "%s",
                              (toString(SUMO_TAG_VSS) + " " + toString(SUMO_TAG_STEP) + "cannot be updated because there are invalid values").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // sort steps after finish
        myEditedVariableSpeedSign->sortVariableSpeedSignSteps();
        // stop dialgo sucesfully
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNEVariableSpeedSignDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update steps tables
    updateTableSteps();
    return 1;
}


void
GNEVariableSpeedSignDialog::updateTableSteps() {
    // clear table
    myStepsTable->clearItems();
    // set number of rows
    myStepsTable->setTableSize(int(myEditedVariableSpeedSign->getVariableSpeedSignSteps().size()), 4);
    // Configure list
    myStepsTable->setVisibleColumns(4);
    myStepsTable->setColumnWidth(0, 115);
    myStepsTable->setColumnWidth(1, 114);
    myStepsTable->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myStepsTable->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myStepsTable->setColumnText(0, "timeStep");
    myStepsTable->setColumnText(1, "speed (km/h)");
    myStepsTable->setColumnText(2, "");
    myStepsTable->setColumnText(3, "");
    myStepsTable->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    FXTableItem* item = 0;
    // iterate over values
    for (int i = 0; i < (int)myEditedVariableSpeedSign->getVariableSpeedSignSteps().size(); i++) {
        // Set time
        item = new FXTableItem(myEditedVariableSpeedSign->getVariableSpeedSignSteps().at(i)->getAttribute(SUMO_ATTR_TIME).c_str());
        myStepsTable->setItem(i, 0, item);
        // Set speed
        item = new FXTableItem(myEditedVariableSpeedSign->getVariableSpeedSignSteps().at(i)->getAttribute(SUMO_ATTR_SPEED).c_str());
        myStepsTable->setItem(i, 1, item);
        // set valid icon
        item = new FXTableItem("");
        item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myStepsTable->setItem(i, 2, item);
        // set remove Icon
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myStepsTable->setItem(i, 3, item);
    }
}

/****************************************************************************/
