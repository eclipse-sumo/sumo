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
#include "GNERerouterDialog.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GNERerouterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GNERerouterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GNERerouterDialog::onCmdReset),
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
    myIntervalList = new FXTable(myContentFrame, this, MID_GNE_CALIBRATOR_REMOVEROW, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    copyIntervals();
    updateTable();

    // Execute additional dialog (To make it modal)
    execute();
}


GNERerouterDialog::~GNERerouterDialog() {
}


long
GNERerouterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // set new intervals into rerouter
    myRerouterParent->setRerouterIntervals(myRerouterIntervals);
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNERerouterDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Clear copied intervals
    for(std::vector<GNERerouterInterval*>::const_iterator i = myRerouterIntervals.begin(); i != myRerouterIntervals.end(); i++) {
        delete (*i);
    }
    myRerouterIntervals.clear();
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNERerouterDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // clear copied/modified intervals
    for(std::vector<GNERerouterInterval*>::const_iterator i = myRerouterIntervals.begin(); i != myRerouterIntervals.end(); i++) {
        delete (*i);
    }
    // Copy original intervals again and update table
    copyIntervals();
    updateTable();
    return 1;
}



long 
GNERerouterDialog::onCmdAddInterval(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNERerouterDialog::onCmdRemoveInterval(FXObject*, FXSelector, void*) {
    return 1;
}


void 
GNERerouterDialog::copyIntervals() {
    myRerouterIntervals.clear();
    for(std::vector<GNERerouterInterval*>::const_iterator i = myRerouterParent->getRerouterIntervals().begin(); i != myRerouterParent->getRerouterIntervals().end(); i++) {
        myRerouterIntervals.push_back(new GNERerouterInterval(*i));
    }
}

void
GNERerouterDialog::updateTable() {
     // clear table
    myIntervalList->clearItems();
    /**
    // set number of rows
    myIntervalList->setTableSize(int(myVSSValues.size()), 3);
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
    **/
}

/****************************************************************************/
