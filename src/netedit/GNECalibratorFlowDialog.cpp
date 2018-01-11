/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorFlowDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator flows
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
#include <utils/common/MsgHandler.h>

#include "GNECalibratorFlowDialog.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorRoute.h"
#include "GNECalibratorVehicleType.h"
#include "GNECalibrator.h"
#include "GNEUndoList.h"
#include "GNEChange_CalibratorItem.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorFlowDialog) GNECalibratorFlowDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,   GNECalibratorFlowDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,   GNECalibratorFlowDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONRESET,    GNECalibratorFlowDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,   GNECalibratorFlowDialog::onCmdSetVariable),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE,   GNECalibratorFlowDialog::onCmdSetTypeOfFlow),
};

// Object implementation
FXIMPLEMENT(GNECalibratorFlowDialog, GNEAdditionalDialog, GNECalibratorFlowDialogMap, ARRAYNUMBER(GNECalibratorFlowDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorFlowDialog::GNECalibratorFlowDialog(GNECalibratorFlow* editedCalibratorFlow, bool updatingElement) :
    GNEAdditionalDialog(editedCalibratorFlow->getCalibratorParent(), 600, 300),
    myEditedCalibratorFlow(editedCalibratorFlow),
    myUpdatingElement(updatingElement),
    myCalibratorFlowValid(true) {
    // change default header
    std::string typeOfOperation = myUpdatingElement ? "Edit " + toString(myEditedCalibratorFlow->getTag()) + " of " : "Create " + toString(myEditedCalibratorFlow->getTag()) + " for ";
    changeAdditionalDialogHeader(typeOfOperation + toString(myEditedCalibratorFlow->getCalibratorParent()->getTag()) + " '" + myEditedCalibratorFlow->getCalibratorParent()->getID() + "'");

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
    myComboBoxVehicleType = new FXComboBox(columnLeftValue, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
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
    myRerouteCheckButton = new FXCheckButton(columnRightValue, "false", this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignCheckButtonAttribute);
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
    myRadioButtonVehsPerHour = new FXRadioButton(columnRightLabel, toString(SUMO_ATTR_VEHSPERHOUR).c_str(), this, MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE, GUIDesignRadioButtonAttribute);
    myTextFieldVehsPerHour = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    // 10 create textfield for period
    myRadioButtonPeriod = new FXRadioButton(columnRightLabel, toString(SUMO_ATTR_PERIOD).c_str(), this, MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE, GUIDesignRadioButtonAttribute);
    myTextFieldPeriod = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);
    // 11 create textfield for probability
    myRadioButtonProbability = new FXRadioButton(columnRightLabel, toString(SUMO_ATTR_PROB).c_str(), this, MID_GNE_CALIBRATORDIALOG_SET_FLOWTYPE, GUIDesignRadioButtonAttribute);
    myTextFieldProbability = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // fill comboBox of VTypes
    for (auto i : myEditedCalibratorFlow->getCalibratorParent()->getCalibratorVehicleTypes()) {
        myComboBoxVehicleType->appendItem(i->getID().c_str());
    }
    myComboBoxVehicleType->setNumVisible((int)myEditedCalibratorFlow->getCalibratorParent()->getCalibratorVehicleTypes().size());

    // fill comboBox of Routes
    for (auto i : myEditedCalibratorFlow->getCalibratorParent()->getCalibratorRoutes()) {
        myComboBoxRoute->appendItem(i->getID().c_str());
    }
    myComboBoxRoute->setNumVisible((int)myEditedCalibratorFlow->getCalibratorParent()->getCalibratorRoutes().size());

    // update tables
    updateCalibratorFlowValues();

    // start a undo list for editing local to this additional
    initChanges();

    // add element if we aren't updating an existent element
    if (myUpdatingElement == false) {
        myEditedCalibratorFlow->getCalibratorParent()->getViewNet()->getUndoList()->add(new GNEChange_CalibratorItem(myEditedCalibratorFlow, true), true);
    }

    // open as modal dialog
    openAsModalDialog();
}


GNECalibratorFlowDialog::~GNECalibratorFlowDialog() {}


long
GNECalibratorFlowDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorFlowValid == false) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        std::string operation1 = myUpdatingElement ? ("updating") : ("creating");
        std::string operation2 = myUpdatingElement ? ("updated") : ("created");
        std::string parentTagString = toString(myEditedCalibratorFlow->getCalibratorParent()->getTag());
        std::string tagString = toString(myEditedCalibratorFlow->getTag());
        // open warning dialog box
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error " + operation1 + " " + parentTagString + "'s " + tagString).c_str(), "%s",
                              (parentTagString + "'s " + tagString + " cannot be " + operation2 +
                               " because parameter " + toString(myInvalidAttr) +
                               " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // stop dialgo sucesfully
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNECalibratorFlowDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorFlowDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    updateCalibratorFlowValues();
    return 1;
}


long
GNECalibratorFlowDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myCalibratorFlowValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // get pointer to undo list (Only for code legilibity)
    GNEUndoList* undoList = myEditedCalibratorFlow->getCalibratorParent()->getViewNet()->getUndoList();
    // set color of myTextFieldFlowID, depending if current value is valid or not
    if (myEditedCalibratorFlow->getID() == myTextFieldFlowID->getText().text()) {
        myTextFieldFlowID->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ID, myTextFieldFlowID->getText().text(), undoList);
    } else if (myEditedCalibratorFlow->isValid(SUMO_ATTR_ID, myTextFieldFlowID->getText().text())) {
        myTextFieldFlowID->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ID, myTextFieldFlowID->getText().text(), undoList);
    } else {
        myTextFieldFlowID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myComboBoxVehicleType, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_TYPE, myComboBoxVehicleType->getText().text())) {
        myComboBoxVehicleType->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_TYPE, myComboBoxVehicleType->getText().text(), undoList);
    } else {
        myComboBoxVehicleType->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_TYPE;
    }
    // set color of myComboBoxRoute, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_ROUTE, myComboBoxRoute->getText().text())) {
        myComboBoxRoute->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ROUTE, myComboBoxRoute->getText().text(), undoList);
    } else {
        myComboBoxRoute->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ROUTE;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), undoList);
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldDepartLane, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_DEPARTLANE, myTextFieldDepartLane->getText().text())) {
        myTextFieldDepartLane->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_DEPARTLANE, myTextFieldDepartLane->getText().text(), undoList);
    } else {
        myTextFieldDepartLane->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTLANE;
    }
    // set color of myTextFieldDepartPos, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_DEPARTPOS, myTextFieldDepartPos->getText().text())) {
        myTextFieldDepartPos->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_DEPARTPOS, myTextFieldDepartPos->getText().text(), undoList);
    } else {
        myTextFieldDepartPos->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTPOS;
    }
    // set color of setDepartSpeed, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_DEPARTSPEED, myTextFieldDepartSpeed->getText().text())) {
        myTextFieldDepartSpeed->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_DEPARTSPEED, myTextFieldDepartSpeed->getText().text(), undoList);
    } else {
        myTextFieldDepartSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTSPEED;
    }
    // set color of myTextFieldArrivalLane, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_ARRIVALLANE, myTextFieldArrivalLane->getText().text())) {
        myTextFieldArrivalLane->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ARRIVALLANE, myTextFieldArrivalLane->getText().text(), undoList);
    } else {
        myTextFieldArrivalLane->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALLANE;
    }
    // set color of myTextFieldArrivalPos, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_ARRIVALPOS, myTextFieldArrivalPos->getText().text())) {
        myTextFieldArrivalPos->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ARRIVALPOS, myTextFieldArrivalPos->getText().text(), undoList);
    } else {
        myTextFieldArrivalPos->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALPOS;
    }
    // set color of setArrivalSpeed, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_ARRIVALSPEED, myTextFieldArrivalSpeed->getText().text())) {
        myTextFieldArrivalSpeed->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ARRIVALSPEED, myTextFieldArrivalSpeed->getText().text(), undoList);
    } else {
        myTextFieldArrivalSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALSPEED;
    }
    // set color of myTextFieldLine, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_LINE, myTextFieldLine->getText().text())) {
        myTextFieldLine->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_LINE, myTextFieldLine->getText().text(), undoList);
    } else {
        myTextFieldLine->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_LINE;
    }
    // set color of myTextFieldPersonNumber, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_PERSON_NUMBER, myTextFieldPersonNumber->getText().text())) {
        myTextFieldPersonNumber->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_PERSON_NUMBER, myTextFieldPersonNumber->getText().text(), undoList);
    } else {
        myTextFieldPersonNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_PERSON_NUMBER;
    }
    // set color of myTextFieldContainerNumber, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_CONTAINER_NUMBER, myTextFieldContainerNumber->getText().text())) {
        myTextFieldContainerNumber->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_CONTAINER_NUMBER, myTextFieldContainerNumber->getText().text(), undoList);
    } else {
        myTextFieldContainerNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_CONTAINER_NUMBER;
    }
    // set reroute
    if (myRerouteCheckButton->getCheck()) {
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_REROUTE, "true", undoList);
        myRerouteCheckButton->setText("true");
    } else {
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_REROUTE, "false", undoList);
        myRerouteCheckButton->setText("false");
    }
    // set color of myTextFieldDepartPosLat, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_DEPARTPOS_LAT, myTextFieldDepartPosLat->getText().text())) {
        myTextFieldDepartPosLat->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_DEPARTPOS_LAT, myTextFieldDepartPosLat->getText().text(), undoList);
    } else {
        myTextFieldDepartPosLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTPOS_LAT;
    }
    // set color of myTextFieldArrivalPosLat, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_ARRIVALPOS_LAT, myTextFieldArrivalPosLat->getText().text())) {
        myTextFieldArrivalPosLat->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_ARRIVALPOS_LAT, myTextFieldArrivalPosLat->getText().text(), undoList);
    } else {
        myTextFieldArrivalPosLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALPOS_LAT;
    }
    // set color of myTextFieldBegin, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_BEGIN, myTextFieldBegin->getText().text())) {
        myTextFieldBegin->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_BEGIN, myTextFieldBegin->getText().text(), undoList);
    } else {
        myTextFieldBegin->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_END;
    }
    // set color of myTextFieldEnd, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_END, myTextFieldEnd->getText().text())) {
        myTextFieldEnd->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_END, myTextFieldEnd->getText().text(), undoList);
    } else {
        myTextFieldEnd->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_BEGIN;
    }
    // set color of myTextFieldNumber, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_NUMBER, myTextFieldNumber->getText().text())) {
        myTextFieldNumber->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_NUMBER, myTextFieldNumber->getText().text(), undoList);
    } else {
        myTextFieldNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_NUMBER;
    }
    // set color of myTextFieldVehsPerHour, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_VEHSPERHOUR, myTextFieldVehsPerHour->getText().text())) {
        myTextFieldVehsPerHour->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_VEHSPERHOUR, myTextFieldVehsPerHour->getText().text(), undoList);
    } else if (myRadioButtonVehsPerHour->getCheck()) {
        myTextFieldVehsPerHour->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_VEHSPERHOUR;
    } else {
        // if radio button is disabled, set default color
        myTextFieldVehsPerHour->setTextColor(FXRGB(0, 0, 0));
    }
    // set color of myTextFieldPeriod, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_PERIOD, myTextFieldPeriod->getText().text())) {
        myTextFieldPeriod->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_PERIOD, myTextFieldPeriod->getText().text(), undoList);
    } else if (myRadioButtonPeriod->getCheck()) {
        myTextFieldPeriod->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_PERIOD;
    } else {
        // if radio button is disabled, set default color
        myTextFieldPeriod->setTextColor(FXRGB(0, 0, 0));
    }
    // set color of myTextFieldProbability, depending if current value is valid or not
    if (myEditedCalibratorFlow->isValid(SUMO_ATTR_PROB, myTextFieldProbability->getText().text())) {
        myTextFieldProbability->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorFlow->setAttribute(SUMO_ATTR_PROB, myTextFieldProbability->getText().text(), undoList);
    } else if (myRadioButtonProbability->getCheck()) {
        myTextFieldProbability->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_PROB;
    } else {
        // if radio button is disabled, set default color
        myTextFieldProbability->setTextColor(FXRGB(0, 0, 0));
    }
    return 1;
}


long
GNECalibratorFlowDialog::onCmdSetTypeOfFlow(FXObject* radioButton, FXSelector, void*) {
    if (radioButton == myRadioButtonVehsPerHour) {
        myRadioButtonVehsPerHour->setCheck(true);
        myTextFieldVehsPerHour->enable();
        myEditedCalibratorFlow->setFlowType(GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR);
        // disable other options
        myRadioButtonPeriod->setCheck(false);
        myTextFieldPeriod->disable();
        myRadioButtonProbability->setCheck(false);
        myTextFieldProbability->disable();
        onCmdSetVariable(0, 0, 0);
        return 1;
    } else if (radioButton == myRadioButtonPeriod) {
        myRadioButtonPeriod->setCheck(true);
        myTextFieldPeriod->enable();
        myEditedCalibratorFlow->setFlowType(GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD);
        // disable other options
        myRadioButtonVehsPerHour->setCheck(false);
        myTextFieldVehsPerHour->disable();
        myRadioButtonProbability->setCheck(false);
        myTextFieldProbability->disable();
        onCmdSetVariable(0, 0, 0);
        return 1;
    } else if (radioButton == myRadioButtonProbability) {
        myRadioButtonProbability->setCheck(true);
        myTextFieldProbability->enable();
        myEditedCalibratorFlow->setFlowType(GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY);
        // disable other options
        myRadioButtonVehsPerHour->setCheck(false);
        myTextFieldVehsPerHour->disable();
        myRadioButtonPeriod->setCheck(false);
        myTextFieldPeriod->disable();
        onCmdSetVariable(0, 0, 0);
        return 1;
    } else {
        return 0;
    }
}


void
GNECalibratorFlowDialog::updateCalibratorFlowValues() {
    // update fields
    myTextFieldFlowID->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_ID).c_str());
    myComboBoxVehicleType->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_TYPE).c_str());
    myComboBoxRoute->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_ROUTE).c_str());
    myTextFieldColor->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_COLOR).c_str());
    myTextFieldDepartLane->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_DEPARTLANE).c_str());
    myTextFieldDepartPos->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_DEPARTPOS).c_str());
    myTextFieldDepartSpeed->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_DEPARTSPEED).c_str());
    myTextFieldArrivalLane->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_ARRIVALLANE).c_str());
    myTextFieldArrivalPos->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_ARRIVALPOS).c_str());
    myTextFieldArrivalSpeed->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_ARRIVALSPEED).c_str());
    myTextFieldLine->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_LINE).c_str());
    myTextFieldPersonNumber->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_PERSON_NUMBER).c_str());
    myTextFieldContainerNumber->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_CONTAINER_NUMBER).c_str());
    myRerouteCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_REROUTE).c_str()));
    myTextFieldDepartPosLat->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_DEPARTPOS_LAT).c_str());
    myTextFieldArrivalPosLat->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_ARRIVALPOS_LAT).c_str());
    myTextFieldBegin->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_BEGIN).c_str());
    myTextFieldEnd->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_END).c_str());
    myTextFieldNumber->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_NUMBER).c_str());
    myTextFieldVehsPerHour->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_VEHSPERHOUR).c_str());
    myTextFieldPeriod->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_PERIOD).c_str());
    myTextFieldProbability->setText(myEditedCalibratorFlow->getAttribute(SUMO_ATTR_PROB).c_str());
    // upsate type of flow
    if (myEditedCalibratorFlow->getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR) {
        onCmdSetTypeOfFlow(myRadioButtonVehsPerHour, 0, 0);
    } else if (myEditedCalibratorFlow->getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD) {
        onCmdSetTypeOfFlow(myRadioButtonPeriod, 0, 0);
    } else if (myEditedCalibratorFlow->getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY) {
        onCmdSetTypeOfFlow(myRadioButtonProbability, 0, 0);
    }
}


/****************************************************************************/
