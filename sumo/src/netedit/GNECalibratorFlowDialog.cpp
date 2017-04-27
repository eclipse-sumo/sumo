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
#include <utils/common/MsgHandler.h>

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

GNECalibratorFlowDialog::GNECalibratorFlowDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorFlow& calibratorFlow, bool updatingElement) :
    GNEAdditionalDialog(calibratorFlow.getCalibratorParent(), 600, 300),
    myCalibratorDialogParent(calibratorDialog),
    myCalibratorFlow(&calibratorFlow),
    myUpdatingElement(updatingElement),
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
    const std::vector<GNECalibratorVehicleType>& vtypes = myCalibratorFlow->getCalibratorParent()->getCalibratorVehicleTypes();
    for (std::vector<GNECalibratorVehicleType>::const_iterator i = vtypes.begin(); i != vtypes.end(); i++) {
        myComboBoxVehicleType->appendItem(i->getVehicleTypeID().c_str());
    }
    myComboBoxVehicleType->setNumVisible(10);

    // fill comboBox of Routes
    const std::vector<GNECalibratorRoute>& routes = myCalibratorFlow->getCalibratorParent()->getCalibratorRoutes();
    for (std::vector<GNECalibratorRoute>::const_iterator i = routes.begin(); i != routes.end(); i++) {
        myComboBoxRoute->appendItem(i->getRouteID().c_str());
    }
    myComboBoxRoute->setNumVisible(10);

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
        // write warning if netedit is running in testing mode
        if (myCalibratorDialogParent->getCalibratorParent()->getViewNet()->isTestingModeEnabled() == true) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning dialog box
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error " + std::string((myUpdatingElement == true) ? ("updating") : ("creating")) + " " + toString(myCalibratorFlow->getCalibratorParent()->getTag()) +
                               "'s " + toString(myCalibratorFlow->getTag())).c_str(), "%s",
                              (toString(myCalibratorFlow->getCalibratorParent()->getTag()) + "'s " + toString(myCalibratorFlow->getTag()) +
                               " cannot be " + std::string((myUpdatingElement == true) ? ("updated") : ("created")) + " because parameter " + toString(myInvalidAttr) +
                               " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        if (myCalibratorDialogParent->getCalibratorParent()->getViewNet()->isTestingModeEnabled() == true) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
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
    // At start we assumed, that all values are valid
    myCalibratorFlowValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // set color of myTextFieldFlowID, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->getFlowID() == myTextFieldFlowID->getText().text()) {
        myTextFieldFlowID->setTextColor(FXRGB(0, 0, 0));
    } else if (myCopyOfCalibratorFlow->setFlowID(myTextFieldFlowID->getText().text()) == true) {
        myTextFieldFlowID->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldFlowID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myComboBoxVehicleType, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setVehicleType(myComboBoxVehicleType->getText().text()) == true) {
        myComboBoxVehicleType->setTextColor(FXRGB(0, 0, 0));
    } else {
        myComboBoxVehicleType->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_TYPE;
    }
    // set color of myComboBoxRoute, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setRoute(myComboBoxRoute->getText().text()) == true) {
        myComboBoxRoute->setTextColor(FXRGB(0, 0, 0));
    } else {
        myComboBoxRoute->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ROUTE;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setColor(myTextFieldColor->getText().text()) == true) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldDepartLane, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setDepartLane(myTextFieldDepartLane->getText().text()) == true) {
        myTextFieldDepartLane->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldDepartLane->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTLANE;
    }
    // set color of myTextFieldDepartPos, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setDepartPos(myTextFieldDepartPos->getText().text()) == true) {
        myTextFieldDepartPos->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldDepartPos->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTPOS;
    }
    // set color of setDepartSpeed, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setDepartSpeed(myTextFieldDepartSpeed->getText().text()) == true) {
        myTextFieldDepartSpeed->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldDepartSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTSPEED;
    }
    // set color of myTextFieldArrivalLane, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setArrivalLane(myTextFieldArrivalLane->getText().text()) == true) {
        myTextFieldArrivalLane->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldArrivalLane->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALLANE;
    }
    // set color of myTextFieldArrivalPos, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setArrivalPos(myTextFieldArrivalPos->getText().text()) == true) {
        myTextFieldArrivalPos->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldArrivalPos->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALPOS;
    }
    // set color of setArrivalSpeed, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setArrivalSpeed(myTextFieldArrivalSpeed->getText().text()) == true) {
        myTextFieldArrivalSpeed->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldArrivalSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALSPEED;
    }
    // set color of myTextFieldLine, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setLine(myTextFieldLine->getText().text()) == true) {
        myTextFieldLine->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldLine->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_LINE;
    }
    // set color of myTextFieldPersonNumber, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setPersonNumber(myTextFieldPersonNumber->getText().text()) == true) {
        myTextFieldPersonNumber->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldPersonNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_PERSON_NUMBER;
    }
    // set color of myTextFieldContainerNumber, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setContainerNumber(myTextFieldContainerNumber->getText().text()) == true) {
        myTextFieldContainerNumber->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldContainerNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_CONTAINER_NUMBER;
    }
    // set reroute
    if (myRerouteCheckButton->getCheck()) {
        myCopyOfCalibratorFlow->setReroute(true);
        myRerouteCheckButton->setText("true");
    } else {
        myCopyOfCalibratorFlow->setReroute(false);
        myRerouteCheckButton->setText("false");
    }
    // set color of myTextFieldDepartPosLat, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setDepartPosLat(myTextFieldDepartPosLat->getText().text()) == true) {
        myTextFieldDepartPosLat->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldDepartPosLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTPOS_LAT;
    }
    // set color of myTextFieldArrivalPosLat, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setArrivalPosLat(myTextFieldArrivalPosLat->getText().text()) == true) {
        myTextFieldArrivalPosLat->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldArrivalPosLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALPOS_LAT;
    }
    // set color of myTextFieldBegin, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setBegin(myTextFieldBegin->getText().text()) == true) {
        myTextFieldBegin->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldBegin->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_END;
    }
    // set color of myTextFieldEnd, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setEnd(myTextFieldEnd->getText().text()) == true) {
        myTextFieldEnd->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldEnd->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_BEGIN;
    }
    // set color of myTextFieldNumber, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setNumber(myTextFieldNumber->getText().text()) == true) {
        myTextFieldNumber->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_NUMBER;
    }
    // set color of myTextFieldVehsPerHour, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setVehsPerHour(myTextFieldVehsPerHour->getText().text()) == true) {
        myTextFieldVehsPerHour->setTextColor(FXRGB(0, 0, 0));
    } else if (myRadioButtonVehsPerHour->getCheck()) {
        myTextFieldVehsPerHour->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_VEHSPERHOUR;
    } else {
        // if radio button is disabled, set default color
        myTextFieldVehsPerHour->setTextColor(FXRGB(0, 0, 0));
    }
    // set color of myTextFieldPeriod, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setPeriod(myTextFieldPeriod->getText().text()) == true) {
        myTextFieldPeriod->setTextColor(FXRGB(0, 0, 0));
    } else if (myRadioButtonPeriod->getCheck()) {
        myTextFieldPeriod->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_PERIOD;
    } else {
        // if radio button is disabled, set default color
        myTextFieldPeriod->setTextColor(FXRGB(0, 0, 0));
    }
    // set color of myTextFieldProbability, depending if current value is valid or not
    if (myCopyOfCalibratorFlow->setProbability(myTextFieldProbability->getText().text()) == true) {
        myTextFieldProbability->setTextColor(FXRGB(0, 0, 0));
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
        myCopyOfCalibratorFlow->setTypeOfFlow(GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR);
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
        myCopyOfCalibratorFlow->setTypeOfFlow(GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD);
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
        myCopyOfCalibratorFlow->setTypeOfFlow(GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY);
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
    myTextFieldFlowID->setText(myCopyOfCalibratorFlow->getFlowID().c_str());
    myComboBoxVehicleType->setText(myCopyOfCalibratorFlow->getVehicleType().c_str());
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
    myRerouteCheckButton->setCheck(myCopyOfCalibratorFlow->getReroute());
    myTextFieldDepartPosLat->setText(myCopyOfCalibratorFlow->getDepartPosLat().c_str());
    myTextFieldArrivalPosLat->setText(myCopyOfCalibratorFlow->getArrivalPosLat().c_str());
    myTextFieldBegin->setText(toString(myCopyOfCalibratorFlow->getBegin()).c_str());
    myTextFieldEnd->setText(toString(myCopyOfCalibratorFlow->getEnd()).c_str());
    myTextFieldNumber->setText(toString(myCopyOfCalibratorFlow->getNumber()).c_str());
    myTextFieldVehsPerHour->setText(toString(myCopyOfCalibratorFlow->getVehsPerHour()).c_str());
    myTextFieldPeriod->setText(toString(myCopyOfCalibratorFlow->getPeriod()).c_str());
    myTextFieldProbability->setText(toString(myCopyOfCalibratorFlow->getProbability()).c_str());
    // upsate type of flow
    if (myCopyOfCalibratorFlow->getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR) {
        onCmdSetTypeOfFlow(myRadioButtonVehsPerHour, 0, 0);
    } else if (myCopyOfCalibratorFlow->getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD) {
        onCmdSetTypeOfFlow(myRadioButtonPeriod, 0, 0);
    } else if (myCopyOfCalibratorFlow->getFlowType() == GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY) {
        onCmdSetTypeOfFlow(myRadioButtonProbability, 0, 0);
    }
}


/****************************************************************************/
