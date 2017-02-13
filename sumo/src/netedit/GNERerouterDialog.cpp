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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_ADD_INTERVAL,     GNERerouterDialog::onCmdAddInterval),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_REROUTEDIALOG_TABLE_INTERVAL,   GNERerouterDialog::onCmdClickedInterval),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,   GNERerouterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,   GNERerouterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,    GNERerouterDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNERerouterDialog, FXDialogBox, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterDialog::GNERerouterDialog(GNERerouter* rerouterParent) :
    GNEAdditionalDialog(rerouterParent, 320, 240),
    myRerouterParent(rerouterParent) {

    // create add buton and label
    FXHorizontalFrame* buttonAndLabelInterval = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    myAddInterval = new FXButton(buttonAndLabelInterval, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_INTERVAL, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelInterval, ("Add new " + toString(SUMO_TAG_CLOSING_LANE_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);

    // Create table, copy intervals and update table
    myIntervalList = new FXTable(myContentFrame, this, MID_GNE_REROUTEDIALOG_TABLE_INTERVAL, GUIDesignTableAdditionals);
    myIntervalList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myIntervalList->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myIntervalList->setEditable(false);
    myCopyOfRerouterIntervals = myRerouterParent->getRerouterIntervals();
    updateIntervalTable();

    // Execute additional dialog (To make it modal)
    execute();
}


GNERerouterDialog::~GNERerouterDialog() {
}


GNERerouter*
GNERerouterDialog::getRerouterParent() const {
    return myRerouterParent;
}


bool
GNERerouterDialog::findInterval(SUMOReal begin, SUMOReal end) const {
    // Iterate over intervals
    for (std::vector<GNERerouterInterval>::const_iterator i = myCopyOfRerouterIntervals.begin(); i != myCopyOfRerouterIntervals.end(); i++) {
        if ((i->getBegin() == begin) && (i->getEnd() == end)) {
            return true;
        }
    }
    return false;
}


bool
GNERerouterDialog::checkInterval(SUMOReal begin, SUMOReal end) const {
    if ((begin < 0) || (end < 0)) {
        return false;
    } else if ((begin == 0) && (end == 0)) {
        return false;
    } else if (begin >= end) {
        return false;
    } else {
        /// CHECK OVERLAPPING (Ticket #2843)
        return true;
    }
}


bool
GNERerouterDialog::checkModifyInterval(SUMOReal oldBegin, SUMOReal oldEnd, SUMOReal newBegin, SUMOReal newEnd) const {
    // Iterate over intervals
    for (std::vector<GNERerouterInterval>::const_iterator i = myCopyOfRerouterIntervals.begin(); i != myCopyOfRerouterIntervals.end(); i++) {
        if ((i->getBegin() == oldBegin) && (i->getEnd() == oldEnd)) {
            // Check that new interval is valid
            if ((newBegin < 0) || (newEnd < 0)) {
                return false;
            } else if ((newBegin == 0) && (newEnd == 0)) {
                return false;
            } else if (newBegin >= newEnd) {
                return false;
            } else {
                /// CHECK OVERLAPPING (Ticket #2843)
                return true;
            }
        }
    }
    return false;
}


long
GNERerouterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // in this point we need to use GNEChange_RerouterInterval to allow undo/redos of rerouterIntervals
    // see Ticket #2844
    // set new intervals into rerouter
    myRerouterParent->setRerouterIntervals(myCopyOfRerouterIntervals);
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
    myCopyOfRerouterIntervals = myRerouterParent->getRerouterIntervals();
    updateIntervalTable();
    return 1;
}


long
GNERerouterDialog::onCmdAddInterval(FXObject*, FXSelector, void*) {
    // create empty rerouter interval and configure it with GNERerouterIntervalDialog
    GNERerouterInterval newInterval(myRerouterParent, 0, 0);
    if (GNERerouterIntervalDialog(this, newInterval).execute() == TRUE) {
        // if new interval was sucesfully configured, add it to myCopyOfRerouterIntervals
        myCopyOfRerouterIntervals.push_back(newInterval);
        updateIntervalTable();
        return 1;
    } else {
        return 0;
    }
}


long
GNERerouterDialog::onCmdClickedInterval(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfRerouterIntervals.size(); i++) {
        if (myIntervalList->getItem(i, 2)->hasFocus()) {
            // remove row
            myIntervalList->removeRows(i);
            myCopyOfRerouterIntervals.erase(myCopyOfRerouterIntervals.begin() + i);
            return 1;
        }
    }
    // check if some begin or o end  button was pressed
    for (int i = 0; i < (int)myCopyOfRerouterIntervals.size(); i++) {
        if (myIntervalList->getItem(i, 0)->hasFocus() || myIntervalList->getItem(i, 1)->hasFocus()) {
            // edit interval
            GNERerouterIntervalDialog(this, *(myCopyOfRerouterIntervals.begin() + i)).execute();
            return 1;
        }
    }
    // nothing to do
    return 0;
}


void
GNERerouterDialog::updateIntervalTable() {
    // clear table
    myIntervalList->clearItems();
    // set number of rows
    myIntervalList->setTableSize(int(myCopyOfRerouterIntervals.size()), 3);
    // Configure list
    myIntervalList->setVisibleColumns(4);
    myIntervalList->setColumnWidth(0, 137);
    myIntervalList->setColumnWidth(1, 136);
    myIntervalList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myIntervalList->setColumnText(0, toString(SUMO_ATTR_BEGIN).c_str());
    myIntervalList->setColumnText(1, toString(SUMO_ATTR_END).c_str());
    myIntervalList->setColumnText(2, "");
    myIntervalList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNERerouterInterval>::iterator i = myCopyOfRerouterIntervals.begin(); i != myCopyOfRerouterIntervals.end(); i++) {
        // Set time
        item = new FXTableItem(toString(i->getBegin()).c_str());
        myIntervalList->setItem(indexRow, 0, item);
        // Set speed
        item = new FXTableItem(toString(i->getEnd()).c_str());
        myIntervalList->setItem(indexRow, 1, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myIntervalList->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}

/****************************************************************************/
