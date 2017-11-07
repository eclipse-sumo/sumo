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
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEChange_VariableSpeedSignItem.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVariableSpeedSignDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_VARIABLESPEEDSIGN_ADDROW,       GNEVariableSpeedSignDialog::onCmdAddRow),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_VARIABLESPEEDSIGN_CLICKEDROW,   GNEVariableSpeedSignDialog::onCmdClickedRow),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_VARIABLESPEEDSIGN_CLICKEDROW,   GNEVariableSpeedSignDialog::onCmdClickedRow),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_VARIABLESPEEDSIGN_CLICKEDROW,   GNEVariableSpeedSignDialog::onCmdClickedRow),
};

// Object implementation
FXIMPLEMENT(GNEVariableSpeedSignDialog, GNEAdditionalDialog, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignDialog::GNEVariableSpeedSignDialog(GNEVariableSpeedSign* editedVariableSpeedSign) :
    GNEAdditionalDialog(editedVariableSpeedSign, 300, 400),
    myEditedVariableSpeedSign(editedVariableSpeedSign) {

    // create Horizontal frame for row elements
    myRowFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);

    // create Button for insert row
    myAddRow = new FXButton(myRowFrame, "Add", 0, this, MID_GNE_VARIABLESPEEDSIGN_ADDROW, GUIDesignButtonIcon);

    // create List with the data
    myDataList = new FXTable(myContentFrame, this, MID_GNE_VARIABLESPEEDSIGN_CLICKEDROW, GUIDesignTableAdditionals);
    myDataList->setEditable(true);

    // update table
    updateTableSteps();

    // start a undo list editing
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->p_begin("change " + toString(myEditedVariableSpeedSign->getTag()) + " values");

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
    GNEVariableSpeedSignStep *step = new GNEVariableSpeedSignStep(this);
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->add(new GNEChange_VariableSpeedSignItem(step, true), true);

    // Update table
    updateTableSteps();
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdClickedRow(FXObject*, FXSelector, void*) {
    return true;
}


long
GNEVariableSpeedSignDialog::onCmdAccept(FXObject*, FXSelector, void*) {



    // finish editing
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->p_end();
    // stop dialgo sucesfully
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // abort last command
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->p_abortLastCommandGroup();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEVariableSpeedSignDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // abort last command an start editing again
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->p_abortLastCommandGroup();
    myEditedVariableSpeedSign->getViewNet()->getUndoList()->p_begin("change " + toString(myEditedVariableSpeedSign->getTag()) + " values");
    // update steps tables
    updateTableSteps();
    return 1;
}


void
GNEVariableSpeedSignDialog::updateTableSteps() {
    // clear table
    myDataList->clearItems();
    // set number of rows
    myDataList->setTableSize(int(myEditedVariableSpeedSign->getSteps().size()), 3);
    // Configure list
    myDataList->setVisibleColumns(3);
    myDataList->setColumnWidth(0, 126);
    myDataList->setColumnWidth(1, 126);
    myDataList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myDataList->setColumnText(0, "timeStep");
    myDataList->setColumnText(1, "speed (km/h)");
    myDataList->setColumnText(2, "");
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
        // set remove ICON
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myDataList->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}

/****************************************************************************/
