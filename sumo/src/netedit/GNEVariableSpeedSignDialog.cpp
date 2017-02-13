/****************************************************************************/
/// @file    GNEVariableSpeedSignDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A class for edit phases of Variable Speed Signals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
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
#include <utils/common/TplCheck.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEVariableSpeedSignDialog.h"
#include "GNEVariableSpeedSign.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVariableSpeedSignDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,  GNEVariableSpeedSignDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,  GNEVariableSpeedSignDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_ADDITIONALDIALOG_RESET,   GNEVariableSpeedSignDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_VARIABLESPEEDSIGN_ADDROW,    GNEVariableSpeedSignDialog::onCmdAddRow),
    FXMAPFUNC(SEL_DOUBLECLICKED, MID_GNE_VARIABLESPEEDSIGN_REMOVEROW, GNEVariableSpeedSignDialog::onCmdRemoveRow),
};

// Object implementation
FXIMPLEMENT(GNEVariableSpeedSignDialog, FXDialogBox, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignDialog::GNEVariableSpeedSignDialog(GNEVariableSpeedSign* variableSpeedSignParent) :
    GNEAdditionalDialog(variableSpeedSignParent, 240, 240),
    myVariableSpeedSignParent(variableSpeedSignParent) {

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

    // Get values of variable speed signal
    myVSSValues = myVariableSpeedSignParent->getVariableSpeedSignSteps();

    // update table
    updateTable();

    // Execute additional dialog (To make it modal)
    execute();
}

GNEVariableSpeedSignDialog::~GNEVariableSpeedSignDialog() {
}


long
GNEVariableSpeedSignDialog::onCmdAddRow(FXObject*, FXSelector, void*) {
    // Declare variables for time and speed
    SUMOReal time, speed;

    // Get Time
    if (TplCheck::_str2SUMOReal(myRowStep->getText().text()) == false) {
        return 0;
    } else {
        time = TplConvert::_str2SUMOReal(myRowStep->getText().text());
    }

    // get SPeed
    if (TplCheck::_str2SUMOReal(myRowSpeed->getText().text()) == false) {
        return 0;
    } else {
        speed = TplConvert::_str2SUMOReal(myRowSpeed->getText().text());
    }

    // Set new time and their speed if don't exist already
    if (myVSSValues.find(time) == myVSSValues.end()) {
        myVSSValues[time] = speed;
    } else {
        return false;
    }

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
            myVSSValues.erase(TplConvert::_2int(myDataList->getItem(i, 0)->getText().text()));
            myDataList->removeRows(i);
            // update table
            updateTable();
            return 1;
        }
    }
    return 0;
}


long
GNEVariableSpeedSignDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Save new data in Variable Speed Signal edited
    myVariableSpeedSignParent->setVariableSpeedSignSteps(myVSSValues);
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
    // Get old values
    myVSSValues = myVariableSpeedSignParent->getVariableSpeedSignSteps();
    updateTable();
    return 1;
}


void
GNEVariableSpeedSignDialog::updateTable() {
    // clear table
    myDataList->clearItems();
    // set number of rows
    myDataList->setTableSize(int(myVSSValues.size()), 3);
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
    for (std::map<SUMOReal, SUMOReal>::iterator i = myVSSValues.begin(); i != myVSSValues.end(); i++) {
        // Set time
        item = new FXTableItem(toString(i->first).c_str());
        myDataList->setItem(indexRow, 0, item);
        // Set speed
        item = new FXTableItem(toString(i->second).c_str());
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
