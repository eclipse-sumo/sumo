/****************************************************************************/
/// @file    GNECalibratorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
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
#include <utils/common/TplCheck.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

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
    FXMAPFUNC(SEL_COMMAND,       MID_GNE_MODE_CALIBRATOR_TABLE,         GNECalibratorDialog::onCmdAddRow),
    FXMAPFUNC(SEL_DOUBLECLICKED, MID_GNE_MODE_CALIBRATOR_TABLE,         GNECalibratorDialog::onCmdRemoveRow),
};

// Object implementation
FXIMPLEMENT(GNECalibratorDialog, FXDialogBox, GNECalibratorDialogMap, ARRAYNUMBER(GNECalibratorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorDialog::GNECalibratorDialog(GNECalibrator* calibratorParent) :
    GNEAdditionalDialog(calibratorParent, 1024, 360),
    myCalibratorParent(calibratorParent) {

    // create List with the data
    myDataList = new FXTable(myContentFrame, this, MID_GNE_MODE_CALIBRATOR_TABLE, GUIDesignTableLimitedHeight);
    myDataList->setEditable(false);

    // create Horizontal frame for row elements
    myRowFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);

    // create Text field for id
    myTextFieldId = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for begin
    myTextFieldBegin = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldReal);

    // create Text field for end
    myTextFieldEnd = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldReal);

    // create Text field for type
    myTextFieldType = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for route
    myTextFieldRoute = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for color
    myTextFieldColor = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for depart lane
    myTextFieldDepartLane = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for depart pos
    myTextFieldDepartPos = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for depart speed
    myTextFieldDepartSpeed = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for arrival lane
    myTextFieldArrivalLane = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for arrival pos
    myTextFieldArrivalPos = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for arrival speed
    myTextFieldArrivalSpeed = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for line
    myTextFieldLine = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignLabelThick);

    // create Text field for person number
    myTextFieldPersonNumber = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldInt);

    // create Text field for container number
    myTextFieldContainerNumber = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldInt);

    // create Text field for vehsPerHour
    myTextFieldVehsPerHour = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldReal);

    // create Text field for period
    myTextFieldPeriod = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldReal);

    // create Text field for probability
    myTextFieldProbability = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldReal);

    // create Text field for number
    myTextFieldNumber = new FXTextField(myRowFrame, 10, this, MID_GNE_MODE_CALIBRATOR_EDITVALUE, GUIDesignTextFieldInt);

    // create Button for insert row
    myAddRow = new FXButton(myRowFrame, "Add", 0, this, MID_GNE_MODE_CALIBRATOR_TABLE, GUIDesignLabelThick);

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
    // Declare new calibrator
    GNECalibrator::GNECalibratorFlow* calibratorFlow = new GNECalibrator::GNECalibratorFlow(0, "", "");
    // set ID
    std::string id = myTextFieldId->getText().text();

    // set Start
    if (myTextFieldBegin->getText().empty()) {
        return 0;
    } else {
        calibratorFlow->setBegin(TplConvert::_str2SUMOReal(myTextFieldBegin->getText().text()));
    }

    // set End
    if (myTextFieldEnd->getText().empty()) {
        return 0;
    } else {
        calibratorFlow->setEnd(TplConvert::_str2SUMOReal(myTextFieldEnd->getText().text()));
    }

    // set Type
    if (myTextFieldType->getText().empty()) {
        return 0;
    } else {
        calibratorFlow->setType(myTextFieldType->getText().text());
    }

    // set Route
    if (myTextFieldType->getText().empty()) {
        return 0;
    } else {
        calibratorFlow->setRoute(myTextFieldRoute->getText().text());
    }

    // Set color
    calibratorFlow->setColor(myTextFieldColor->getText().text());

    // Set depart lane
    calibratorFlow->setDepartLane(myTextFieldDepartLane->getText().text());

    // Set depart pos
    calibratorFlow->setDepartPos(myTextFieldDepartPos->getText().text());

    // Set depart speed
    calibratorFlow->setDepartSpeed(myTextFieldDepartSpeed->getText().text());

    // Set arrival lane
    calibratorFlow->setArrivalLane(myTextFieldArrivalLane->getText().text());

    // Set arrival pos
    calibratorFlow->setArrivalPos(myTextFieldArrivalPos->getText().text());

    // Set arrival speed
    calibratorFlow->setArrivalSpeed(myTextFieldArrivalSpeed->getText().text());

    // Set Line
    calibratorFlow->setLine(myTextFieldLine->getText().text());

    // set PersionNumber
    calibratorFlow->setPersonNumber(TplConvert::_str2int(myTextFieldPersonNumber->getText().text()));

    // set Container Number
    calibratorFlow->setContainerNumber(TplConvert::_str2int(myTextFieldContainerNumber->getText().text()));

    // Set vehsPerHour
    calibratorFlow->setVehsPerHour(TplConvert::_str2SUMOReal(myTextFieldVehsPerHour->getText().text()));

    // set Period
    calibratorFlow->setPeriod(TplConvert::_str2SUMOReal(myTextFieldPeriod->getText().text()));

    // set Probability
    calibratorFlow->setProbability(TplCheck::_str2SUMOReal(myTextFieldProbability->getText().text()));

    // set Number
    calibratorFlow->setNumber(TplConvert::_str2int(myTextFieldNumber->getText().text()));

    // addd new calibrator flow to table
    if (std::find(myFlowValues.begin(), myFlowValues.end(), calibratorFlow) == myFlowValues.end()) {
        myFlowValues.push_back(calibratorFlow);
    } else {
        delete calibratorFlow;
        return 0;
    }

    // Update table
    updateTable();
    return 1;
}


long
GNECalibratorDialog::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    // Iterate over rows to find the row to erase
    for (int i = 0; i < myDataList->getNumRows(); i++) {
        if (myDataList->getItem(i, 19)->isSelected()) {
            // Remove element of table and map
            myFlowValues.erase(myFlowValues.begin() + i);
            myDataList->removeRows(i);
            // update table
            updateTable();
            return 1;
        }
    }
    return 0;
}


long
GNECalibratorDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Get values of variable speed signal
    myCalibratorParent->setFlowValues(myFlowValues);
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNECalibratorDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // Get values of variable speed signal
    myFlowValues = myCalibratorParent->getFlowValues();
    // Update Table
    updateTable();
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
    myDataList->setColumnWidth(0, getWidth() / 20);
    myDataList->setColumnWidth(1, getWidth() / 20);
    myDataList->setColumnWidth(2, getWidth() / 20);
    myDataList->setColumnWidth(3, getWidth() / 20);
    myDataList->setColumnWidth(4, getWidth() / 20);
    myDataList->setColumnWidth(5, getWidth() / 20);
    myDataList->setColumnWidth(6, getWidth() / 20);
    myDataList->setColumnWidth(7, getWidth() / 20);
    myDataList->setColumnWidth(8, getWidth() / 20);
    myDataList->setColumnWidth(9, getWidth() / 20);
    myDataList->setColumnWidth(10, getWidth() / 20);
    myDataList->setColumnWidth(11, getWidth() / 20);
    myDataList->setColumnWidth(12, getWidth() / 20);
    myDataList->setColumnWidth(13, getWidth() / 20);
    myDataList->setColumnWidth(14, getWidth() / 20);
    myDataList->setColumnWidth(15, getWidth() / 20);
    myDataList->setColumnWidth(16, getWidth() / 20);
    myDataList->setColumnWidth(17, getWidth() / 20);
    myDataList->setColumnWidth(18, getWidth() / 20);
    myDataList->setColumnWidth(19, getWidth() / 20 - 10);
    myDataList->setColumnText(0, "Id");
    myDataList->setColumnText(1, "Begin");
    myDataList->setColumnText(2, "End");
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
    FXTableItem* item = 0;

    // iterate over values
    for (std::vector<GNECalibrator::GNECalibratorFlow*>::iterator i = myFlowValues.begin(); i != myFlowValues.end(); i++) {
        // set begin
        item = new FXTableItem(toString((*i)->getBegin()).c_str());
        myDataList->setItem(indexRow, 1, item);

        // set end
        item = new FXTableItem(toString((*i)->getEnd()).c_str());
        myDataList->setItem(indexRow, 2, item);

        // set type
        item = new FXTableItem(toString((*i)->getType()).c_str());
        myDataList->setItem(indexRow, 3, item);

        // set route
        item = new FXTableItem(toString((*i)->getRoute()).c_str());
        myDataList->setItem(indexRow, 4, item);

        // set color
        item = new FXTableItem(toString((*i)->getColor()).c_str());
        myDataList->setItem(indexRow, 5, item);

        // set depart lane
        item = new FXTableItem(toString((*i)->getDepartLane()).c_str());
        myDataList->setItem(indexRow, 6, item);

        // set depart pos
        item = new FXTableItem(toString((*i)->getDepartPos()).c_str());
        myDataList->setItem(indexRow, 7, item);

        // set depart speed
        item = new FXTableItem(toString((*i)->getDepartSpeed()).c_str());
        myDataList->setItem(indexRow, 8, item);

        // set arrival lane
        item = new FXTableItem(toString((*i)->getArrivalLane()).c_str());
        myDataList->setItem(indexRow, 9, item);

        // set arrival pos
        item = new FXTableItem(toString((*i)->getArrivalPos()).c_str());
        myDataList->setItem(indexRow, 10, item);

        // set arrival speed
        item = new FXTableItem(toString((*i)->getArrivalSpeed()).c_str());
        myDataList->setItem(indexRow, 11, item);

        // set line
        item = new FXTableItem(toString((*i)->getLine()).c_str());
        myDataList->setItem(indexRow, 12, item);

        // set person number
        item = new FXTableItem(toString((*i)->getPersonNumber()).c_str());
        myDataList->setItem(indexRow, 13, item);

        // set container number
        item = new FXTableItem(toString((*i)->getContainerNumber()).c_str());
        myDataList->setItem(indexRow, 14, item);

        // set vehsPerHour
        item = new FXTableItem(toString((*i)->getVehsPerHour()).c_str());
        myDataList->setItem(indexRow, 15, item);

        // set period
        item = new FXTableItem(toString((*i)->getPeriod()).c_str());
        myDataList->setItem(indexRow, 16, item);

        // set probability
        item = new FXTableItem(toString((*i)->getProbability()).c_str());
        myDataList->setItem(indexRow, 17, item);

        // set number
        item = new FXTableItem(toString((*i)->getNumber()).c_str());
        myDataList->setItem(indexRow, 18, item);

        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        myDataList->setItem(indexRow, 19, item);

        // Update index
        indexRow++;
    }
}

/****************************************************************************/
