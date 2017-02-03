/****************************************************************************/
/// @file    GNERerouterIntervalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    eb 2017
/// @version $Id: GNERerouterIntervalDialog.cpp 22824 2017-02-02 09:51:02Z palcraft $
///
/// Dialog for edit rerouter intervals
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

#include "GNERerouterIntervalDialog.h"
#include "GNERerouterDialog.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERerouterInterval.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterIntervalDialog) GNERerouterIntervalDialogMap[] = {
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_CLOSINGLANEREORUTE,   GNERerouterIntervalDialog::onCmdDoubleClickedClosingLaneReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_CLOSINGREROUTE,       GNERerouterIntervalDialog::onCmdDoubleClickedClosingReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_DESTPROBREROUTE,      GNERerouterIntervalDialog::onCmdDoubleClickedDestProbReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_ROUTEPROBREROUTE,     GNERerouterIntervalDialog::onCmdDoubleClickedRouteProbReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,       GNERerouterIntervalDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,       GNERerouterIntervalDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_MODE_ADDITIONALDIALOG_RESET,        GNERerouterIntervalDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNERerouterIntervalDialog, FXDialogBox, GNERerouterIntervalDialogMap, ARRAYNUMBER(GNERerouterIntervalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterIntervalDialog::GNERerouterIntervalDialog(GNERerouterInterval &rerouterInterval) :
    GNEAdditionalDialog(rerouterInterval.getRerouterParent(), 640, 480),
    myRerouterInterval(&rerouterInterval) {
    // create horizoontal frame for begin and end elements
    FXHorizontalFrame * beginEndElements = new FXHorizontalFrame(myContentFrame, GUIDesignHorizontalFrame);
    myBeginEndLabel = new FXLabel(beginEndElements, "Begin and end times", 0, GUIDesignLabelLeftThick);

    myBeginTextField = new FXTextField(beginEndElements, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGESTART, GUIDesignTextFieldReal);
    myBeginTextField->setText(toString(myRerouterInterval->getBegin()).c_str());
    
    myEndTextField = new FXTextField(beginEndElements, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGEEND, GUIDesignTextFieldReal);
    myEndTextField->setText(toString(myRerouterInterval->getEnd()).c_str());

    myCheckLabel = new FXLabel(beginEndElements, " ", 0, GUIDesignLabelLeftThick);

    // Create frames for tables
    FXHorizontalFrame *columns = new FXHorizontalFrame(myContentFrame, LAYOUT_FILL |PACK_UNIFORM_WIDTH);
    FXVerticalFrame *columnLeft = new FXVerticalFrame(columns, LAYOUT_FILL, 0, 0, 0, 0,0,0,0,0,0,0);
    FXVerticalFrame *columnRight = new FXVerticalFrame(columns, LAYOUT_FILL, 0, 0, 0, 0,0,0,0,0,0,0);

    // Create labels and tables
    myClosingLaneReroutesLabel = new FXLabel(columnLeft, "List of Closing Lane Reroutes", 0, GUIDesignLabelThick);
    myClosingLaneRerouteList = new FXTable(columnLeft, this, MID_GNE_REROUTEDIALOG_CLOSINGLANEREORUTE, LAYOUT_FILL);
    
    myCLosingReroutesLabel = new FXLabel(columnLeft, "List of Closing Reroutes", 0, GUIDesignLabelThick);
    myClosingRerouteList = new FXTable(columnLeft, this, MID_GNE_REROUTEDIALOG_CLOSINGREROUTE, LAYOUT_FILL);
    
    myDestProbReroutesLabel = new FXLabel(columnRight, "List of Destiny Probability Reroutes", 0, GUIDesignLabelThick);
    myDestProbRerouteList = new FXTable(columnRight, this, MID_GNE_REROUTEDIALOG_DESTPROBREROUTE, LAYOUT_FILL);
    
    myRouteProbReroutesLabel = new FXLabel(columnRight, "List of Route Probability Reroutes", 0, GUIDesignLabelThick);
    myRouteProbReroute = new FXTable(columnRight, this, MID_GNE_REROUTEDIALOG_ROUTEPROBREROUTE, LAYOUT_FILL);

    // copy Elements
    myCopyOfClosingLaneReroutes = myRerouterInterval->getClosingLaneReroutes();
    myCopyOfClosingReroutes = myRerouterInterval->getClosingReroutes();
    myCopyOfmyDestProbReroutes = myRerouterInterval->getDestProbReroutes();
    myCopyOfRouteProbReroutes = myRerouterInterval->getRouteProbReroutes();
    // update tables
    updateClosingLaneReroutesTable();
    updateClosingReroutesTable();
    updateDestProbReroutesTable();
    updateRouteProbReroutesTable();
}


GNERerouterIntervalDialog::~GNERerouterIntervalDialog() {
}


long
GNERerouterIntervalDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    
    // in this point we need to use GNEChange_RerouterInterval to allow undo/redos of rerouterIntervals

    // set new intervals into rerouter
    //myRerouterInterval->set



    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // Copy original intervals again
    myCopyOfClosingLaneReroutes = myRerouterInterval->getClosingLaneReroutes();
    myCopyOfClosingReroutes = myRerouterInterval->getClosingReroutes();
    myCopyOfmyDestProbReroutes = myRerouterInterval->getDestProbReroutes();
    myCopyOfRouteProbReroutes = myRerouterInterval->getRouteProbReroutes();
    // update tables
    updateClosingLaneReroutesTable();
    updateClosingReroutesTable();
    updateDestProbReroutesTable();
    updateRouteProbReroutesTable();
    return 1;
}


long 
GNERerouterIntervalDialog::onCmdDoubleClickedClosingLaneReroute(FXObject*, FXSelector, void*) {
    // First check if list is emty
    if(myClosingLaneRerouteList->getNumRows() > 0) {
        // check if add button was pressed
        if(myClosingLaneRerouteList->getItem((int)myCopyOfClosingLaneReroutes.size(), 4)->hasFocus()) {
            // add new element and update table
            myCopyOfClosingLaneReroutes.push_back(GNEClosingLaneReroute(*myRerouterInterval, NULL, std::vector<SUMOVehicleClass>(), std::vector<SUMOVehicleClass>()));
            updateClosingLaneReroutesTable();
            return 1;
        } else {
            // check if some delete button was pressed
            for(int i = 0; i < (int)myCopyOfClosingLaneReroutes.size(); i++) {
                if(myClosingLaneRerouteList->getItem(i, 4)->hasFocus()) {
                    myClosingLaneRerouteList->removeRows(i);
                    myCopyOfClosingLaneReroutes.erase(myCopyOfClosingLaneReroutes.begin() + i);
                    return 1;
                }
            }
            return 1;
        }
    } else {
        return 0;
    }
}


long 
GNERerouterIntervalDialog::onCmdDoubleClickedClosingReroute(FXObject*, FXSelector, void*) {
    // First check if list is emty
    if(myClosingRerouteList->getNumRows() > 0) {
        // check if add button was pressed
        if(myClosingRerouteList->getItem((int)myCopyOfClosingReroutes.size(), 4)->hasFocus()) {
            // add new element and update table
            myCopyOfClosingReroutes.push_back(GNEClosingReroute(*myRerouterInterval, NULL, std::vector<SUMOVehicleClass>(), std::vector<SUMOVehicleClass>()));
            updateClosingReroutesTable();
            return 1;
        } else {
            // check if some delete button was pressed
            for(int i = 0; i < (int)myCopyOfClosingReroutes.size(); i++) {
                if(myClosingRerouteList->getItem(i, 4)->hasFocus()) {
                    myClosingRerouteList->removeRows(i);
                    myCopyOfClosingReroutes.erase(myCopyOfClosingReroutes.begin() + i);
                    return 1;
                }
            }
            return 1;
        }
    } else {
        return 0;
    }
}


long 
GNERerouterIntervalDialog::onCmdDoubleClickedDestProbReroute(FXObject*, FXSelector, void*) {
    // First check if list is emty
    if(myDestProbRerouteList->getNumRows() > 0) {
        // check if add button was pressed
        if(myDestProbRerouteList->getItem((int)myCopyOfmyDestProbReroutes.size(), 3)->hasFocus()) {
            // add new element and update table
            myCopyOfmyDestProbReroutes.push_back(GNEDestProbReroute(*myRerouterInterval, NULL, 0));
            updateDestProbReroutesTable();
            return 1;
        } else {
            // check if some delete button was pressed
            for(int i = 0; i < (int)myCopyOfmyDestProbReroutes.size(); i++) {
                if(myDestProbRerouteList->getItem(i, 3)->hasFocus()) {
                    myDestProbRerouteList->removeRows(i);
                    myCopyOfmyDestProbReroutes.erase(myCopyOfmyDestProbReroutes.begin() + i);
                    return 1;
                }
            }
            return 1;
        }
    } else {
        return 0;
    }
}


long 
GNERerouterIntervalDialog::onCmdDoubleClickedRouteProbReroute(FXObject*, FXSelector, void*) {
    // First check if list is emty
    if(myRouteProbReroute->getNumRows() > 0) {
        // check if add button was pressed
        if(myRouteProbReroute->getItem((int)myCopyOfRouteProbReroutes.size(), 3)->hasFocus()) {
            // add new element and update table
            myCopyOfRouteProbReroutes.push_back(GNERouteProbReroute(*myRerouterInterval,"", 0));
            updateRouteProbReroutesTable();
            return 1;
        } else {
            // check if some delete button was pressed
            for(int i = 0; i < (int)myCopyOfRouteProbReroutes.size(); i++) {
                if(myRouteProbReroute->getItem(i, 3)->hasFocus()) {
                    myRouteProbReroute->removeRows(i);
                    myCopyOfRouteProbReroutes.erase(myCopyOfRouteProbReroutes.begin() + i);
                    return 1;
                }
            }
            return 1;
        }
    } else {
        return 0;
    }
}



void 
GNERerouterIntervalDialog::updateClosingLaneReroutesTable() {
    // clear table
    myClosingLaneRerouteList->clearItems();
    // set number of rows
    myClosingLaneRerouteList->setTableSize(int(myCopyOfClosingLaneReroutes.size()) + 1, 5);
    // Configure list
    myClosingLaneRerouteList->setVisibleColumns(5);
    myClosingLaneRerouteList->setColumnWidth(0, 60);
    myClosingLaneRerouteList->setColumnWidth(1, 100);
    myClosingLaneRerouteList->setColumnWidth(2, 100);
    myClosingLaneRerouteList->setColumnWidth(3, 20);
    myClosingLaneRerouteList->setColumnWidth(4, 20);
    myClosingLaneRerouteList->setColumnText(0, "Closed edge");
    myClosingLaneRerouteList->setColumnText(1, "Allowed vehicles");
    myClosingLaneRerouteList->setColumnText(2, "Disallowed vehicles");
    myClosingLaneRerouteList->setColumnText(3, "");
    myClosingLaneRerouteList->setColumnText(4, "");
    myClosingLaneRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNEClosingLaneReroute>::iterator i = myCopyOfClosingLaneReroutes.begin(); i != myCopyOfClosingLaneReroutes.end(); i++) {
        // Set closing edge
        if(i->getClosedEdge() != NULL) {
            item = new FXTableItem(i->getClosedEdge()->getID().c_str());
            myClosingLaneRerouteList->setItem(indexRow, 0, item);
        } else {
            item = new FXTableItem("");
            myClosingLaneRerouteList->setItem(indexRow, 0, item);
        }
        // Set allow
        // @todo item = new FXTableItem(toString((*i)->getEnd()).c_str());
        // @todo myClosingLaneRerouteList->setItem(indexRow, 1, item);
        // Set disallow
        // @todo item = new FXTableItem(toString((*i)->getEnd()).c_str());
        // @todo myClosingLaneRerouteList->setItem(indexRow, 2, item);
        // set valid
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_EMPTY));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myClosingLaneRerouteList->setItem(indexRow, 3, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myClosingLaneRerouteList->setItem(indexRow, 4, item);
        // Update index
        indexRow++;
    }
    // set add
    item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_ADD));
    item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
    myClosingLaneRerouteList->setItem(indexRow, 4, item);
}


void 
GNERerouterIntervalDialog::updateClosingReroutesTable() {
    // clear table
    myClosingRerouteList->clearItems();
    // set number of rows
    myClosingRerouteList->setTableSize(int(myCopyOfClosingReroutes.size()) + 1, 5);
    // Configure list
    myClosingRerouteList->setVisibleColumns(5);
    myClosingRerouteList->setColumnWidth(0, 60);
    myClosingRerouteList->setColumnWidth(1, 100);
    myClosingRerouteList->setColumnWidth(2, 100);
    myClosingRerouteList->setColumnWidth(3, 20);
    myClosingRerouteList->setColumnWidth(4, 20);
    myClosingRerouteList->setColumnText(0, "Closed edge");
    myClosingRerouteList->setColumnText(1, "Allowed vehicles");
    myClosingRerouteList->setColumnText(2, "Disallowed vehicles");
    myClosingRerouteList->setColumnText(3, "");
    myClosingRerouteList->setColumnText(4, "");
    myClosingRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNEClosingReroute>::iterator i = myCopyOfClosingReroutes.begin(); i != myCopyOfClosingReroutes.end(); i++) {
        // Set closing edge
        if(i->getClosedEdge() != NULL) {
            item = new FXTableItem(i->getClosedEdge()->getID().c_str());
            myClosingRerouteList->setItem(indexRow, 0, item);
        } else {
            item = new FXTableItem("");
            myClosingRerouteList->setItem(indexRow, 0, item);
        }
        // Set allow
        // @todo item = new FXTableItem(toString((*i)->getEnd()).c_str());
        // @todo myClosingRerouteList->setItem(indexRow, 1, item);
        // Set disallow
        // @todo item = new FXTableItem(toString((*i)->getEnd()).c_str());
        // @todo myClosingRerouteList->setItem(indexRow, 1, item);
        // set valid
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_EMPTY));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myClosingRerouteList->setItem(indexRow, 3, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myClosingRerouteList->setItem(indexRow, 4, item);
        // Update index
        indexRow++;
    }
    // set add
    item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_ADD));
    item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
    myClosingRerouteList->setItem(indexRow, 4, item);
}


void 
GNERerouterIntervalDialog::updateDestProbReroutesTable() {
    // clear table
    myDestProbRerouteList->clearItems();
    // set number of rows
    myDestProbRerouteList->setTableSize(int(myCopyOfmyDestProbReroutes.size()) + 1, 4);
    // Configure list
    myDestProbRerouteList->setVisibleColumns(4);
    myDestProbRerouteList->setColumnWidth(0, 130);
    myDestProbRerouteList->setColumnWidth(1, 130);
    myDestProbRerouteList->setColumnWidth(2, 20);
    myDestProbRerouteList->setColumnWidth(3, 20);
    myDestProbRerouteList->setColumnText(0, "Edge destination");
    myDestProbRerouteList->setColumnText(1, "Probability ");
    myDestProbRerouteList->setColumnText(2, "");
    myDestProbRerouteList->setColumnText(3, "");
    myDestProbRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNEDestProbReroute>::iterator i = myCopyOfmyDestProbReroutes.begin(); i != myCopyOfmyDestProbReroutes.end(); i++) {
        // Set new destination
        if(i->getNewDestination() != NULL) {
            item = new FXTableItem(i->getNewDestination()->getID().c_str());
            myDestProbRerouteList->setItem(indexRow, 0, item);
        } else {
            item = new FXTableItem("");
            myDestProbRerouteList->setItem(indexRow, 0, item);
        }
        // Set probability
        item = new FXTableItem(toString(i->getProbability()).c_str());
        myDestProbRerouteList->setItem(indexRow, 1, item);

        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myDestProbRerouteList->setItem(indexRow, 3, item);
        // Update index
        indexRow++;
    }
    // set add
    item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_ADD));
    item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
    myDestProbRerouteList->setItem(indexRow, 3, item);
}


void 
GNERerouterIntervalDialog::updateRouteProbReroutesTable() {
    // clear table
    myRouteProbReroute->clearItems();
    // set number of rows
    myRouteProbReroute->setTableSize(int(myCopyOfRouteProbReroutes.size()) + 1, 4);
    // Configure list
    myRouteProbReroute->setVisibleColumns(4);
    myRouteProbReroute->setColumnWidth(0, 130);
    myRouteProbReroute->setColumnWidth(1, 130);
    myRouteProbReroute->setColumnWidth(2, 20);
    myRouteProbReroute->setColumnWidth(3, 20);
    myRouteProbReroute->setColumnText(0, "New Route ID");
    myRouteProbReroute->setColumnText(1, "Probability");
    myRouteProbReroute->setColumnText(2, "");
    myRouteProbReroute->setColumnText(3, "");
    myRouteProbReroute->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNERouteProbReroute>::iterator i = myCopyOfRouteProbReroutes.begin(); i != myCopyOfRouteProbReroutes.end(); i++) {
        // Set new route
        item = new FXTableItem(i->getNewRouteId().c_str());
        myRouteProbReroute->setItem(indexRow, 0, item);
        // Set probability
        item = new FXTableItem(toString(i->getProbability()).c_str());
        myRouteProbReroute->setItem(indexRow, 1, item);

        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myRouteProbReroute->setItem(indexRow, 3, item);
        // Update index
        indexRow++;
    }
    // set add
    item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_ADD));
    item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
    myRouteProbReroute->setItem(indexRow, 3, item);
}


/****************************************************************************/
