/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEVariableSpeedSignDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id: GNEVariableSpeedSignDialog.cpp 26739 2017-10-26 09:05:34Z palcraft $
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


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVariableSpeedSignDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_VARIABLESPEEDSIGN_ADDROW,    GNEVariableSpeedSignDialog::onCmdAddRow),
    FXMAPFUNC(SEL_DOUBLECLICKED, MID_GNE_VARIABLESPEEDSIGN_REMOVEROW, GNEVariableSpeedSignDialog::onCmdRemoveRow),
};

// Object implementation
FXIMPLEMENT(GNEVariableSpeedSignDialog, GNEAdditionalDialog, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignDialog::GNEVariableSpeedSignDialog(GNEVariableSpeedSign* editedVariableSpeedSign) :
    GNEAdditionalDialog(myEditedVariableSpeedSign, 240, 240),
    myEditedVariableSpeedSign(editedVariableSpeedSign) {

    // create List with the data
    myDataList = new FXTable(myContentFrame, this, MID_GNE_VARIABLESPEEDSIGN_REMOVEROW, GUIDesignTableLimitedHeight);
    myDataList->setEditable(false);

    // create Horizontal frame for row elements
    myRowFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);

    // create Text field for the timeStep
    myRowStep = new FXTextField(myRowFrame, 10, this, MID_GNE_VARIABLESPEEDSIGN_CHANGEVALUE, GUIDesignTextField);

    // create Text field for the speed
    myRowSpeed = new FXTextField(myRowFrame, 10, this, MID_GNE_VARIABLESPEEDSIGN_CHANGEVALUE, GUIDesignTextField);

    // create Button for insert row
    myAddRow = new FXButton(myRowFrame, "Add", 0, this, MID_GNE_VARIABLESPEEDSIGN_ADDROW, GUIDesignButtonIcon);

    // update table
    updateTable();

    // Open dialog as modal
    openAsModalDialog();
}

GNEVariableSpeedSignDialog::~GNEVariableSpeedSignDialog() {}


GNEVariableSpeedSign* 
GNEVariableSpeedSignDialog::getEditedVariableSpeedSign() const {
    return myEditedVariableSpeedSign;
}


long
GNEVariableSpeedSignDialog::onCmdAddRow(FXObject*, FXSelector, void*) {
    // Declare variables for time and speed
    /*
    GNEVariableSpeedSignStep step(myEditedVariableSpeedSign);
    // Get Time
    if (GNEAttributeCarrier::canParse<double>(myRowStep->getText().text()) == false) {
        return 0;
    } else {
        step.setTime(GNEAttributeCarrier::parse<double>(myRowStep->getText().text()));
    }

    // get SPeed
    if (GNEAttributeCarrier::canParse<double>(myRowSpeed->getText().text()) == false) {
        return 0;
    } else {
        step.setSpeed(GNEAttributeCarrier::parse<double>(myRowSpeed->getText().text()));
    }

    // Set new time and their speed if don't exist already
    if (std::find(mySteps.begin(), mySteps.end(), step) == mySteps.end()) {
        mySteps.push_back(step);
    } else {
        return false;
    }
    */
    // Update table
    updateTable();
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    // Iterate over rows to find the row to erase
    for (int i = 0; i < myDataList->getNumRows(); i++) {
        if (myDataList->getItem(i, 2)->isSelected()) {
            // Remove element of table and map
            /*
            mySteps.erase(mySteps.begin() + i);
            myDataList->removeRows(i);
            */
            // update table
            updateTable();
            return 1;
        }
    }
    return 0;
}


long
GNEVariableSpeedSignDialog::onCmdAccept(FXObject*, FXSelector, void*) {


    // Stop Modal with positive out
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdCancel(FXObject*, FXSelector, void*) {

    // Stop Modal with negative out
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdReset(FXObject*, FXSelector, void*) {


    updateTable();
    return 1;
}


void
GNEVariableSpeedSignDialog::updateTable() {
    // clear table
    myDataList->clearItems();
    // set number of rows
    myDataList->setTableSize(int(myEditedVariableSpeedSign->getSteps().size()), 3);
    // Configure list
    myDataList->setVisibleColumns(3);
    myDataList->setColumnWidth(0, getWidth() / 3);
    myDataList->setColumnWidth(1, getWidth() / 3);
    myDataList->setColumnWidth(2, getWidth() / 3 - 10);
    myDataList->setColumnText(0, "timeStep");
    myDataList->setColumnText(1, "speed (km/h)");
    myDataList->setColumnText(2, "remove");
    myDataList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (auto i : myEditedVariableSpeedSign->getSteps()) {
        // Set time
        item = new FXTableItem(i->getAttribute(SUMO_ATTR_TIME).c_str());
        myDataList->setItem(indexRow, 0, item);
        // Set speed
        item = new FXTableItem(i->getAttribute(SUMO_ATTR_SPEED).c_str());
        myDataList->setItem(indexRow, 1, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myDataList->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}

/****************************************************************************/
