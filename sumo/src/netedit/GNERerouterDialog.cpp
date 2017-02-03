/****************************************************************************/
/// @file    GNERerouterDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// Dialog for edit rerouters
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
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNERerouterDialog.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERerouterIntervalDialog.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_MODE_ADDITIONALDIALOG_TABLE,    GNERerouterDialog::onCmdDoubleClicked),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,   GNERerouterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,   GNERerouterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_MODE_ADDITIONALDIALOG_RESET,    GNERerouterDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNERerouterDialog, FXDialogBox, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterDialog::GNERerouterDialog(GNERerouter* rerouterParent) :
    GNEAdditionalDialog(rerouterParent, 320, 240),
    myRerouterParent(rerouterParent) {
    // Create table, copy intervals and update table
    myIntervalList = new FXTable(myContentFrame, this, MID_GNE_MODE_ADDITIONALDIALOG_TABLE, GUIDesignTableNoRowColSelectables);
    myIntervalList->setEditable(false);
    myRerouterIntervals = myRerouterParent->getRerouterIntervals();
    updateTable();
    // Execute additional dialog (To make it modal)
    execute();
}


GNERerouterDialog::~GNERerouterDialog() {
}


GNERerouter* 
GNERerouterDialog::getRerouterParent() const {
    return myRerouterParent;
}


long
GNERerouterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    
    // in this point we need to use GNEChange_RerouterInterval to allow undo/redos of rerouterIntervals

    // set new intervals into rerouter
    myRerouterParent->setRerouterIntervals(myRerouterIntervals);
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNERerouterDialog::onCmdCancel(FXObject*, FXSelector, void*) {

    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNERerouterDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // Copy original intervals again and update table
    myRerouterIntervals = myRerouterParent->getRerouterIntervals();
    updateTable();
    return 1;
}



long 
GNERerouterDialog::onCmdDoubleClicked(FXObject*, FXSelector, void*) {
    // First check if
    if(myIntervalList->getNumRows() > 0) {
        // check if add button was pressed
        if(myIntervalList->getItem((int)myRerouterIntervals.size(), 3)->hasFocus()) {
            GNERerouterInterval newInterval(myRerouterParent, 0, 0);
            if(GNERerouterIntervalDialog(newInterval).execute() == TRUE) {
                myRerouterIntervals.push_back(newInterval);
                updateTable();
                return 1;
            } else {
                return 0;
            }
        } else {
            // check if some delete button was pressed
            for(int i = 0; i < (int)myRerouterIntervals.size(); i++) {
                if(myIntervalList->getItem(i, 3)->hasFocus()) {
                    // remove row
                    myIntervalList->removeRows(i);
                    myRerouterIntervals.erase(myRerouterIntervals.begin() + i);
                    return 1;
                }
            }
            // check if some edit button was pressed
            for(int i = 0; i < (int)myRerouterIntervals.size(); i++) {
                if(myIntervalList->getItem(i, 2)->hasFocus()) {
                    // edit interval
                    GNERerouterIntervalDialog(*(myRerouterIntervals.begin() + i)).execute();
                    return 1;
                }
            }
            // nothing to do
            return 0;
        }
    } else {
        // there aren't row
        return 0;
    }
}


void
GNERerouterDialog::updateTable() {
     // clear table
    myIntervalList->clearItems();
    // set number of rows
    myIntervalList->setTableSize(int(myRerouterIntervals.size()) + 1, 4);
    // Configure list
    myIntervalList->setVisibleColumns(4);
    myIntervalList->setColumnWidth(0, 106);
    myIntervalList->setColumnWidth(1, 106);
    myIntervalList->setColumnWidth(2, 49);
    myIntervalList->setColumnWidth(3, 50);
    myIntervalList->setColumnText(0, "start");
    myIntervalList->setColumnText(1, "end");
    myIntervalList->setColumnText(2, "edit");
    myIntervalList->setColumnText(3, "");
    myIntervalList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNERerouterInterval>::iterator i = myRerouterIntervals.begin(); i != myRerouterIntervals.end(); i++) {
        // Set time
        item = new FXTableItem(toString(i->getBegin()).c_str());
        myIntervalList->setItem(indexRow, 0, item);
        // Set speed
        item = new FXTableItem(toString(i->getEnd()).c_str());
        myIntervalList->setItem(indexRow, 1, item);
        // Set edit
        item = new FXTableItem("");
        myIntervalList->setItem(indexRow, 2, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myIntervalList->setItem(indexRow, 3, item);
        // Update index
        indexRow++;
    }
    // set add
    item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_ADD));
    item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
    myIntervalList->setItem(indexRow, 3, item);
}

/****************************************************************************/
