/****************************************************************************/
/// @file    GNECalibratorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
/// Dialog for edit rerouters
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include "GNECalibratorDialog.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorDialog) GNECalibratorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,  GNECalibratorDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,  GNECalibratorDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_ADDITIONALDIALOG_RESET,   GNECalibratorDialog::onCmdReset),
    FXMAPFUNC(SEL_DOUBLECLICKED, MID_GNE_VARIABLESPEEDSIGNAL_REMOVEROW, GNECalibratorDialog::onCmdRemoveRow),
};

// Object implementation
FXIMPLEMENT(GNECalibratorDialog, FXDialogBox, GNECalibratorDialogMap, ARRAYNUMBER(GNECalibratorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorDialog::GNECalibratorDialog(GNECalibrator *calibratorParent) : 
    GNEAdditionalDialog(calibratorParent, 1024, 360),
    myCalibratorParent(calibratorParent) {

    // create List with the data
    myDataList = new FXTable(myContentFrame, this, MID_GNE_REROUTER_REMOVEROW, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    myDataList->setEditable(false);

    // create Horizontal frame for row elements
    myRowFrame = new FXHorizontalFrame(myContentFrame, LAYOUT_FILL_X);

    // create Text field for id
    myTextFieldId = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for type
    myTextFieldType = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for route
    myTextFieldRoute = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for color
    myTextFieldColor = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for depart lane
    myTextFieldDepartLane = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for depart pos
    myTextFieldDepartPos = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for depart speed
    myTextFieldDepartSpeed = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for arrival lane
    myTextFieldArrivalLane = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for arrival pos
    myTextFieldArrivalPos = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for arrival speed
    myTextFieldArrivalSpeed = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for line
    myTextFieldLine = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for person number
    myTextFieldPersonNumber = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);

    // create Text field for container number
    myTextFieldContainerNumber = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);
    
    // create Text field for vehsPerHour
    myTextFieldVehsPerHour = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);
    
    // create Text field for period
    myTextFieldPeriod = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);
    
    // create Text field for probability
    myTextFieldProbability = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);
    
    // create Text field for number
    myTextFieldnumber = new FXTextField(myRowFrame, 10, this, MID_GNE_REROUTER_CHANGEVALUE, LAYOUT_FILL_X);
    
    // create Button for insert row
    myAddRow = new FXButton(myRowFrame, "Add", 0, this, MID_GNE_REROUTER_ADDROW, LAYOUT_FILL_X);

    // Get values of variable speed signal
    myFlowValues = myCalibratorParent->getFlowValues();
    
    // Fill table
    updateTable();

    // Execute additional dialog (To make it modal)
    execute(); 
}


GNECalibratorDialog::~GNECalibratorDialog() {
}


long
GNECalibratorDialog::onCmdAddRow(FXObject*, FXSelector, void*) {
    // Declare variables for time and speed
    SUMOTime time;
    SUMOReal speed;
    /*
    // Get Time
    if(TplCheck::_str2SUMOTime(myRowStep->getText().text()) == false)
        return 0;
    else
// @toDo IMPLEMENT _str2Time TO TIME
        time = TplConvert::_str2int(myRowStep->getText().text());

    // get SPeed
    if(TplCheck::_str2SUMOReal(myRowSpeed->getText().text()) == false)
        return 0;
    else
        speed = TplConvert::_str2SUMOReal(myRowSpeed->getText().text());
    
    // Set new time and their speed if don't exist already
    if(myVSSValues.find(time) == myVSSValues.end())
        myVSSValues[time] = speed;
    else
        return false;
        */
    // Update table
    updateTable();
    return 1;
}


long 
GNECalibratorDialog::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    /*
    // Iterate over rows to find the row to erase
    for(int i = 0; i < myDataList->getNumRows(); i++)
        if(myDataList->getItem(i, 2)->isSelected()) {
            // Remove element of table and map
// @todo IMPLEMENT _2SUMOTIme
            myVSSValues.erase(TplConvert::_2int(myDataList->getItem(i, 0)->getText().text()));
            myDataList->removeRows(i);
            // update table
            updateTable();
            return 1;
        }
        */
    return 0;
}


long 
GNECalibratorDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this,TRUE);
    return 1;
}


long 
GNECalibratorDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this,TRUE);
    return 1;
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}


void
GNECalibratorDialog::updateTable() {
    // clear table
    myDataList->clearItems();
    // set number of rows
    myDataList->setTableSize(int(myFlowValues.size()), 20);
    // Configure list
    myDataList->setVisibleColumns(20);
    myDataList->setColumnWidth(0, getWidth() * 0.05);
    myDataList->setColumnWidth(1, getWidth() * 0.05);
    myDataList->setColumnWidth(2, getWidth() * 0.05);
    myDataList->setColumnWidth(3, getWidth() * 0.05);
    myDataList->setColumnWidth(4, getWidth() * 0.05);
    myDataList->setColumnWidth(5, getWidth() * 0.05);
    myDataList->setColumnWidth(6, getWidth() * 0.05);
    myDataList->setColumnWidth(7, getWidth() * 0.05);
    myDataList->setColumnWidth(8, getWidth() * 0.05);
    myDataList->setColumnWidth(9, getWidth() * 0.05);
    myDataList->setColumnWidth(10, getWidth() * 0.05);
    myDataList->setColumnWidth(11, getWidth() * 0.05);
    myDataList->setColumnWidth(12, getWidth() * 0.05);
    myDataList->setColumnWidth(13, getWidth() * 0.05);
    myDataList->setColumnWidth(14, getWidth() * 0.05);
    myDataList->setColumnWidth(15, getWidth() * 0.05);
    myDataList->setColumnWidth(16, getWidth() * 0.05);
    myDataList->setColumnWidth(17, getWidth() * 0.05);
    myDataList->setColumnWidth(18, getWidth() * 0.05);
    myDataList->setColumnWidth(19, (getWidth() * 0.05) - 10);
    myDataList->setColumnText(0, "Begin");
    myDataList->setColumnText(1, "End");
    myDataList->setColumnText(2, "Id");
    myDataList->setColumnText(3, "Type");
    myDataList->setColumnText(4, "Route");
    myDataList->setColumnText(5, "Color");
    myDataList->setColumnText(6, "Dep.Lane");
    myDataList->setColumnText(7, "Dep.Pos.");
    myDataList->setColumnText(8, "Dep.Speed");
    myDataList->setColumnText(9, "Arr.Lane");
    myDataList->setColumnText(10, "Arr.Pos.");
    myDataList->setColumnText(11, "Arr.Speed");
    myDataList->setColumnText(12, "Line");
    myDataList->setColumnText(13, "Per.Num.");
    myDataList->setColumnText(14, "Con.Num");
    myDataList->setColumnText(15, "VehPerHour");
    myDataList->setColumnText(16, "Period");
    myDataList->setColumnText(17, "Prob.");
    myDataList->setColumnText(18, "Number");
    myDataList->setColumnText(19, "Del.");

    myDataList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem *item = 0;
    // iterate over values
    for(std::map<std::pair<SUMOTime, SUMOTime>, GNECalibrator::CalibratorFlow>::iterator i = myFlowValues.begin(); i != myFlowValues.end(); i++) {
         ;/** // Set time
        item = new FXTableItem(toString(i->first).c_str());
        myDataList->setItem (indexRow, 0, item);
        // Set speed
        item = new FXTableItem(toString(i->second).c_str());
        myDataList->setItem (indexRow, 1, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myDataList->setItem (indexRow, 2, item);
        // Update index
        */
    }
    
}

/****************************************************************************/
