/****************************************************************************/
/// @file    GNECalibratorFlowDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrator flows
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

#include "GNECalibratorFlowDialog.h"
#include "GNECalibratorDialog.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorFlowDialog) GNECalibratorFlowDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,   GNECalibratorFlowDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,   GNECalibratorFlowDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,    GNECalibratorFlowDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,  GNECalibratorFlowDialog::onCmdSetVariable),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE,  GNECalibratorFlowDialog::onCmdSetTypeOfFlow),
};

// Object implementation
FXIMPLEMENT(GNECalibratorFlowDialog, FXDialogBox, GNECalibratorFlowDialogMap, ARRAYNUMBER(GNECalibratorFlowDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorFlowDialog::GNECalibratorFlowDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorFlow &calibratorFlow) :
    GNEAdditionalDialog(calibratorFlow.getCalibratorParent(), 600, 300),
    myCalibratorDialogParent(calibratorDialog),
    myCalibratorFlow(&calibratorFlow),
    myCalibratorFlowValid(true) {
    // change default header
    changeAdditionalDialogHeader("Edit " + toString(calibratorFlow.getTag()) + " of " + toString(calibratorFlow.getCalibratorParent()->getTag()) +
                                 " '" + calibratorFlow.getCalibratorParent()->getID() + "'");
    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeftLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnLeftValue = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightValue = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // 1 create textfield for flowID
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ID).c_str(), 0, GUIDesignLabelThick);
    myTextFieldFlowID = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 2 create combobox for type
    new FXLabel(columnLeftLabel, toString(SUMO_TAG_VTYPE).c_str(), 0, GUIDesignLabelThick);
    myComboBoxType = new FXComboBox(columnLeftValue, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    // 3 create combobox for route
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ROUTE).c_str(), 0, GUIDesignLabelThick);
    myComboBoxRoute = new FXComboBox(columnLeftValue, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    // 4 create textfield for color
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_COLOR).c_str(), 0, GUIDesignLabelThick);
    myTextFieldColor = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 5 create textfield for lane
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DEPARTLANE).c_str(), 0, GUIDesignLabelThick);
    myTextFieldDepartLane = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 6 create textfield for pos
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DEPARTPOS).c_str(), 0, GUIDesignLabelThick);
    myTextFieldDepartPos = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 7 create textfield for speed
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DEPARTSPEED).c_str(), 0, GUIDesignLabelThick);
    myTextFieldDepartSpeed = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 8 create textfield for lane
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ARRIVALLANE).c_str(), 0, GUIDesignLabelThick);
    myTextFieldArrivalLane = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 9 create textfield for arrival pos
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ARRIVALPOS).c_str(), 0, GUIDesignLabelThick);
    myTextFieldArrivalPos = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 10 create textfield for arrival speed
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ARRIVALSPEED).c_str(), 0, GUIDesignLabelThick);
    myTextFieldArrivalSpeed = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 11 create textfield for arrival line
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_LINE).c_str(), 0, GUIDesignLabelThick);
    myTextFieldLine = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 1 create textfield for person number
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_PERSON_NUMBER).c_str(), 0, GUIDesignLabelThick);
    myTextFieldPersonNumber = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);
    // 2 create textfield for container number
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_CONTAINER_NUMBER).c_str(), 0, GUIDesignLabelThick);
    myTextFieldContainerNumber = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);
    // 3 create textfield for reroute
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_REROUTE).c_str(), 0, GUIDesignLabelThick);
    myCheckBoxdReroute = new FXMenuCheck(columnRightValue, "Disabled", this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignMenuCheckFixedHeight);
    myCheckBoxdReroute->setHeight(23);
    // 4 create textfield for depart pos lat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_DEPARTPOS_LAT).c_str(), 0, GUIDesignLabelThick);
    myTextFieldDepartPosLat = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 5 create textfield for arrival pos lat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_ARRIVALPOS_LAT).c_str(), 0, GUIDesignLabelThick);
    myTextFieldArrivalPosLat = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);
    // 6 create textfield for begin
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_BEGIN).c_str(), 0, GUIDesignLabelThick);
    myTextFieldBegin = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    // 7 create textfield for end
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_END).c_str(), 0, GUIDesignLabelThick);
    myTextFieldEnd = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    // 8 create textfield for vehicle number
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_NUMBER).c_str(), 0, GUIDesignLabelThick);
    myTextFieldNumber = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);
    // 9 create textfield for vehs per hour
    myRadioButtonVehsPerHour = new FXRadioButton(columnRightLabel, toString(SUMO_ATTR_VEHSPERHOUR).c_str(), this, MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE, GUIDesignRadioButtonFixedHeight);
    myTextFieldVehsPerHour = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    myRadioButtonVehsPerHour->setHeight(23);
    // 10 create textfield for period
    myRadioButtonPeriod = new FXRadioButton(columnRightLabel, toString(SUMO_ATTR_PERIOD).c_str(), this, MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE, GUIDesignRadioButtonFixedHeight);
    myTextFieldPeriod = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    myRadioButtonPeriod->setHeight(23);
    // 11 create textfield for probability
    myRadioButtonProbability = new FXRadioButton(columnRightLabel, toString(SUMO_ATTR_PROB).c_str(), this, MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE, GUIDesignRadioButtonFixedHeight);
    myTextFieldProbability = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    myRadioButtonProbability->setHeight(23);

    // create copy of GNECalibratorFlow
    myCopyOfCalibratorFlow = new GNECalibratorFlow(myCalibratorFlow->getCalibratorParent());

    // copy all values of myCalibratorFlow into myCopyOfCalibratorFlow to set initial values
    (*myCopyOfCalibratorFlow) = (*myCalibratorFlow);

    // update tables
    updateCalibratorFlowValues();
}


GNECalibratorFlowDialog::~GNECalibratorFlowDialog() {
    // delete copy
    delete myCopyOfCalibratorFlow;
}


long
GNECalibratorFlowDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorFlowValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myCalibratorFlow->getTag()) + " of " + toString(myCalibratorFlow->getCalibratorParent()->getTag())).c_str(), "%s",
                              (toString(myCalibratorFlow->getCalibratorParent()->getTag()) + "'s " + toString(myCalibratorFlow->getTag()) +
                               " cannot be updated because " + toString(myCalibratorFlow->getTag()) + " defined by " + toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " is invalid.").c_str());
        return 0;
    } else {
        // copy all values of myCopyOfCalibratorFlow into myCalibratorFlow
        (*myCalibratorFlow) = (*myCopyOfCalibratorFlow);
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNECalibratorFlowDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorFlowDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // copy all values of myCalibratorFlow into myCopyOfCalibratorFlow to set initial values
    (*myCopyOfCalibratorFlow) = (*myCalibratorFlow);
    // update fields
    updateCalibratorFlowValues();
    return 1;
}


long
GNECalibratorFlowDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {

    return 1;
}


long
GNECalibratorFlowDialog::onCmdSetTypeOfFlow(FXObject*, FXSelector, void*) {

    return 1;
}


void
GNECalibratorFlowDialog::updateCalibratorFlowValues() {
    // update fields
    myTextFieldFlowID->setText(myCopyOfCalibratorFlow->getFlowID().c_str());
    myComboBoxType->setText(myCopyOfCalibratorFlow->getType().c_str());
    myComboBoxRoute->setText(myCopyOfCalibratorFlow->getRoute().c_str());
    myTextFieldColor->setText(myCopyOfCalibratorFlow->getColor().c_str());
    myTextFieldDepartLane->setText(myCopyOfCalibratorFlow->getDepartLane().c_str());
    myTextFieldDepartPos->setText(myCopyOfCalibratorFlow->getDepartPos().c_str());
    myTextFieldDepartSpeed->setText(myCopyOfCalibratorFlow->getDepartSpeed().c_str());
    myTextFieldArrivalLane->setText(myCopyOfCalibratorFlow->getArrivalLane().c_str());
    myTextFieldArrivalPos->setText(myCopyOfCalibratorFlow->getArrivalPos().c_str());
    myTextFieldArrivalSpeed->setText(myCopyOfCalibratorFlow->getArrivalSpeed().c_str());
    myTextFieldLine->setText(myCopyOfCalibratorFlow->getLine().c_str());
    myTextFieldPersonNumber->setText(toString(myCopyOfCalibratorFlow->getPersonNumber()).c_str());
    myTextFieldContainerNumber->setText(toString(myCopyOfCalibratorFlow->getContainerNumber()).c_str());
    myCheckBoxdReroute->setCheck(myCopyOfCalibratorFlow->getReroute());
    myTextFieldDepartPosLat->setText(myCopyOfCalibratorFlow->getDepartPosLat().c_str());
    myTextFieldArrivalPosLat->setText(myCopyOfCalibratorFlow->getArrivalPosLat().c_str());
    myTextFieldBegin->setText(toString(myCopyOfCalibratorFlow->getBegin()).c_str());
    myTextFieldEnd->setText(toString(myCopyOfCalibratorFlow->getEnd()).c_str());
    myTextFieldNumber->setText(toString(myCopyOfCalibratorFlow->getNumber()).c_str());
    myTextFieldVehsPerHour->setText(toString(myCopyOfCalibratorFlow->getVehsPerHour()).c_str());
    myTextFieldPeriod->setText(toString(myCopyOfCalibratorFlow->getPeriod()).c_str());
    myTextFieldProbability->setText(toString(myCopyOfCalibratorFlow->getProbability()).c_str());
}


/****************************************************************************/
