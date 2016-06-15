/****************************************************************************/
/// @file    GNEVariableSpeedSignalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id: GNEVariableSpeedSignalDialog.cpp 20472 2016-04-15 15:36:45Z palcraft $
///
/// A class for edit phases of Variable Speed Signals
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
#include <utils/common/TplCheck.h>
#include "GNEVariableSpeedSignalDialog.h"
#include "GNEVariableSpeedSignal.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVariableSpeedSignalDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GNEVariableSpeedSignalDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GNEVariableSpeedSignalDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GNEVariableSpeedSignalDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VARIABLESPEEDSIGNAL_ADDROW,   GNEVariableSpeedSignalDialog::onCmdAddRow),
};

// Object implementation
FXIMPLEMENT(GNEVariableSpeedSignalDialog, FXDialogBox, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// static member definitions
// ===========================================================================

static int dialogWidth = 240;
static int dialogHeight = 240;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignalDialog::GNEVariableSpeedSignalDialog(GNEVariableSpeedSignal *variableSpeedSignalParent) : 
    GNEAdditionalDialog(variableSpeedSignalParent, dialogWidth, dialogHeight),
    myVariableSpeedSignalParent(variableSpeedSignalParent) {

    // List with the data
    myDataList = new FXTable(myContentFrame, this, MID_GNE_VARIABLESPEEDSIGNAL_REMOVEROW, LAYOUT_FILL_X | LAYOUT_FILL_Y);

    // Configure list
    myDataList->setTableSize(4, 3);
    myDataList->setVisibleColumns(3);
    myDataList->setColumnWidth(0, dialogWidth * 0.35);
    myDataList->setColumnWidth(1, dialogWidth * 0.35);
    myDataList->setColumnWidth(2, (dialogWidth * 0.3) - 10);
    myDataList->setColumnText(0, "timeStep");
    myDataList->setColumnText(1, "speed");
    myDataList->setColumnText(2, "remove");
    myDataList->getRowHeader()->setWidth(0);

    // Horizontal frame for row elements
    myRowFrame = new FXHorizontalFrame(myContentFrame, LAYOUT_FILL_X);

    // Text field with step
    myRowStep = new FXTextField(myRowFrame, 10, this, MID_GNE_VARIABLESPEEDSIGNAL_CHANGEVALUE, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);

    // Text field with speed
    myRowSpeed = new FXTextField(myRowFrame, 10, this, MID_GNE_VARIABLESPEEDSIGNAL_CHANGEVALUE, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);

    // Button for insert row
    myAddRow = new FXButton(myRowFrame, "Add", 0, this, MID_GNE_VARIABLESPEEDSIGNAL_ADDROW);
        
    // Execute additional dialog (To make it modal)
    execute();
}

GNEVariableSpeedSignalDialog::~GNEVariableSpeedSignalDialog() {
}


long 
GNEVariableSpeedSignalDialog::onCMDInsertRow(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEVariableSpeedSignalDialog::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEVariableSpeedSignalDialog::onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) {
    // Stop Modal with positive out
    getApp()->stopModal(this,TRUE);
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) {
    // Stop Modal with negative out
    getApp()->stopModal(this,FALSE);
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdAddRow(FXObject* sender, FXSelector sel, void* data) {

    if(TplCheck::_str2SUMOTime(myRowStep->getText().text()) == false)
        return 0;

    if(TplCheck::_str2SUMOReal(myRowSpeed->getText().text()) == false)
        return 0;

    fillTable();

    return 1;
}


void
GNEVariableSpeedSignalDialog::fillTable()
{

}

/****************************************************************************/
