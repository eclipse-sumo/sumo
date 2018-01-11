/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorVehicleTypeDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator vehicleTypes
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

#include "GNECalibratorVehicleTypeDialog.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNECalibratorVehicleType.h"
#include "GNEUndoList.h"
#include "GNEChange_CalibratorItem.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorVehicleTypeDialog) GNECalibratorVehicleTypeDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,  GNECalibratorVehicleTypeDialog::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNECalibratorVehicleTypeDialog, GNEAdditionalDialog, GNECalibratorVehicleTypeDialogMap, ARRAYNUMBER(GNECalibratorVehicleTypeDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorVehicleTypeDialog::GNECalibratorVehicleTypeDialog(GNECalibratorVehicleType* editedCalibratorVehicleType, bool updatingElement) :
    GNEAdditionalDialog(editedCalibratorVehicleType->getCalibratorParent(), 500, 370),
    myEditedCalibratorVehicleType(editedCalibratorVehicleType),
    myUpdatingElement(updatingElement),
    myCalibratorVehicleTypeValid(true),
    myInvalidAttr(SUMO_ATTR_NOTHING) {
    // change default header
    std::string typeOfOperation = myUpdatingElement ? "Edit " + toString(myEditedCalibratorVehicleType->getTag()) + " of " : "Create " + toString(myEditedCalibratorVehicleType->getTag()) + " for ";
    changeAdditionalDialogHeader(typeOfOperation + toString(myEditedCalibratorVehicleType->getCalibratorParent()->getTag()) + " '" + myEditedCalibratorVehicleType->getCalibratorParent()->getID() + "'");

    // Create auxiliar frames for values
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeftLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnLeftValues = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightValues = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // create FXComboBox for VClass
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_VCLASS).c_str(), 0, GUIDesignLabelThick);
    myComboBoxVClass = new FXComboBox(columnLeftLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxVClassLabelImage = new FXLabel(columnLeftValues, "", 0, GUIDesignLabelIconExtendedx46Ticked);
    myComboBoxVClassLabelImage->setBackColor(FXRGBA(255, 255, 255, 255));
    // fill combo Box with all VClass
    std::vector<std::string> VClassStrings = SumoVehicleClassStrings.getStrings();
    for (auto i : VClassStrings) {
        if (i != SumoVehicleClassStrings.getString(SVC_IGNORING)) {
            myComboBoxVClass->appendItem(i.c_str());
        }
    }
    // only show 10 VClasses
    myComboBoxVClass->setNumVisible(10);

    // create combo bof for vehicle shapes
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_GUISHAPE).c_str(), 0, GUIDesignLabelThick);
    myComboBoxShape = new FXComboBox(columnRightLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxShapeLabelImage = new FXLabel(columnRightValues, "", 0, GUIDesignLabelIconExtendedx46Ticked);
    myComboBoxShapeLabelImage->setBackColor(FXRGBA(255, 255, 255, 255));
    // fill combo Box with all vehicle shapes
    std::vector<std::string> VShapeStrings = SumoVehicleShapeStrings.getStrings();
    for (auto i : VShapeStrings) {
        if (i != SumoVehicleShapeStrings.getString(SVS_UNKNOWN)) {
            myComboBoxShape->appendItem(i.c_str());
        }
    }
    // only show 10 Shapes
    myComboBoxShape->setNumVisible(10);

    // 01 create FXTextField and Label for vehicleTypeID
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ID).c_str(), 0, GUIDesignLabelThick);
    myTextFieldVehicleTypeID = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 02 create FXTextField and Label for Accel
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ACCEL).c_str(), 0, GUIDesignLabelThick);
    myTextFieldAccel = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 03 create FXTextField and Label for Decel
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DECEL).c_str(), 0, GUIDesignLabelThick);
    myTextFieldDecel = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 04 create FXTextField and Label for Sigma
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SIGMA).c_str(), 0, GUIDesignLabelThick);
    myTextFieldSigma = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 05 create FXTextField and Label for Tau
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_TAU).c_str(), 0, GUIDesignLabelThick);
    myTextFieldTau = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 06 create FXTextField and Label for Length
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelThick);
    myTextFieldLength = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 07 create FXTextField and Label for MinGap
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_MINGAP).c_str(), 0, GUIDesignLabelThick);
    myTextFieldMinGap = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 08 create FXTextField and Label for MaxSpeed
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_MAXSPEED).c_str(), 0, GUIDesignLabelThick);
    myTextFieldMaxSpeed = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 09 create FXTextField and Label for SpeedFactor
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SPEEDFACTOR).c_str(), 0, GUIDesignLabelThick);
    myTextFieldSpeedFactor = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 10 create FXTextField and Label for SpeedDev
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SPEEDDEV).c_str(), 0, GUIDesignLabelThick);
    myTextFieldSpeedDev = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 11 create FXTextField and Label for Color
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_COLOR).c_str(), 0, GUIDesignLabelThick);
    myTextFieldColor = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 12 create FXTextField and Label for EmissionClass
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_EMISSIONCLASS).c_str(), 0, GUIDesignLabelThick);
    myTextFieldEmissionClass = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 01 create FXTextField and Label for Width
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_WIDTH).c_str(), 0, GUIDesignLabelThick);
    myTextFieldWidth = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 02 create FXTextField and Label for Filename
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_IMGFILE).c_str(), 0, GUIDesignLabelThick);
    myTextFieldFilename = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 03 create FXTextField and Label for Impatience
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_IMPATIENCE).c_str(), 0, GUIDesignLabelThick);
    myTextFieldImpatience = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 04 create FXTextField and Label for LaneChangeModel
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LANE_CHANGE_MODEL).c_str(), 0, GUIDesignLabelThick);
    myTextFieldLaneChangeModel = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 05 create FXTextField and Label for CarFollowModel
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_CAR_FOLLOW_MODEL).c_str(), 0, GUIDesignLabelThick);
    myTextFieldCarFollowModel = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 06 create FXTextField and Label for PersonCapacity
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_PERSON_CAPACITY).c_str(), 0, GUIDesignLabelThick);
    myTextFieldPersonCapacity = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);

    // 07 create FXTextField and Label for ContainerCapacity
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_CONTAINER_CAPACITY).c_str(), 0, GUIDesignLabelThick);
    myTextFieldContainerCapacity = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);

    // 08 create FXTextField and Label for BoardingDuration
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_BOARDING_DURATION).c_str(), 0, GUIDesignLabelThick);
    myTextFieldBoardingDuration = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 09 create FXTextField and Label for LoadingDuration
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LOADING_DURATION).c_str(), 0, GUIDesignLabelThick);
    myTextFieldLoadingDuration = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 10 create FXTextField and Label for LatAlignment
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LATALIGNMENT).c_str(), 0, GUIDesignLabelThick);
    myTextFieldLatAlignment = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 11 create FXTextField and Label for MinGapLat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_MINGAP_LAT).c_str(), 0, GUIDesignLabelThick);
    myTextFieldMinGapLat = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 12 create FXTextField and Label for MaxSpeedLat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_MAXSPEED_LAT).c_str(), 0, GUIDesignLabelThick);
    myTextFieldMaxSpeedLat = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // update fields
    updateCalibratorVehicleTypeValues();

    // start a undo list for editing local to this additional
    initChanges();

    // add element if we aren't updating an existent element
    if (myUpdatingElement == false) {
        myEditedCalibratorVehicleType->getCalibratorParent()->getViewNet()->getUndoList()->add(new GNEChange_CalibratorItem(myEditedCalibratorVehicleType, true), true);
    }

    // open as modal dialog
    openAsModalDialog();
}


GNECalibratorVehicleTypeDialog::~GNECalibratorVehicleTypeDialog() {}


long
GNECalibratorVehicleTypeDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorVehicleTypeValid == false) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        std::string operation1 = myUpdatingElement ? ("updating") : ("creating");
        std::string operation2 = myUpdatingElement ? ("updated") : ("created");
        std::string parentTagString = toString(myEditedCalibratorVehicleType->getCalibratorParent()->getTag());
        std::string tagString = toString(myEditedCalibratorVehicleType->getTag());
        // open warning dialogBox
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
GNECalibratorVehicleTypeDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorVehicleTypeDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update fields
    updateCalibratorVehicleTypeValues();
    return 1;
}


long
GNECalibratorVehicleTypeDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myCalibratorVehicleTypeValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // get pointer to undo list (Only for code legilibity)
    GNEUndoList* undoList = myEditedCalibratorVehicleType->getCalibratorParent()->getViewNet()->getUndoList();
    // set color of myComboBoxShape, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text())) {
        myComboBoxShape->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text(), undoList);
    } else {
        myComboBoxShape->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_GUISHAPE;
    }
    // set color of myComboBoxVClass, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text())) {
        myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text(), undoList);
        setVClassLabelImage();
    } else {
        myComboBoxVClass->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_VCLASS;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text())) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), undoList);
    } else if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text())) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), undoList);
    } else {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myTextFieldAccel, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_ACCEL, myTextFieldAccel->getText().text())) {
        myTextFieldAccel->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_ACCEL, myTextFieldAccel->getText().text(), undoList);
    } else {
        myTextFieldAccel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_ACCEL;
    }
    // set color of myTextFieldDecel, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_DECEL, myTextFieldDecel->getText().text())) {
        myTextFieldDecel->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_DECEL, myTextFieldDecel->getText().text(), undoList);
    } else {
        myTextFieldDecel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_DECEL;
    }
    // set color of myTextFieldSigma, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_SIGMA, myTextFieldSigma->getText().text())) {
        myTextFieldSigma->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_SIGMA, myTextFieldSigma->getText().text(), undoList);
    } else {
        myTextFieldSigma->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SIGMA;
    }
    // set color of myTextFieldTau, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_TAU, myTextFieldTau->getText().text())) {
        myTextFieldTau->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_TAU, myTextFieldTau->getText().text(), undoList);
    } else {
        myTextFieldTau->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_TAU;
    }
    // set color of myTextFieldLength, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_LENGTH, myTextFieldLength->getText().text())) {
        myTextFieldLength->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_LENGTH, myTextFieldLength->getText().text(), undoList);
    } else {
        myTextFieldLength->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LENGTH;
    }
    // set color of myTextFieldMinGap, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_MINGAP, myTextFieldMinGap->getText().text())) {
        myTextFieldMinGap->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_MINGAP, myTextFieldMinGap->getText().text(), undoList);
    } else {
        myTextFieldMinGap->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MINGAP;
    }
    // set color of myTextFieldMaxSpeed, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeed->getText().text())) {
        myTextFieldMaxSpeed->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeed->getText().text(), undoList);
    } else {
        myTextFieldMaxSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MAXSPEED;
    }
    // set color of myTextFieldSpeedFactor, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_SPEEDFACTOR, myTextFieldSpeedFactor->getText().text())) {
        myTextFieldSpeedFactor->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_SPEEDFACTOR, myTextFieldSpeedFactor->getText().text(), undoList);
    } else {
        myTextFieldSpeedFactor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SPEEDFACTOR;
    }
    // set color of myTextFieldSpeedDev, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_SPEEDDEV, myTextFieldSpeedDev->getText().text())) {
        myTextFieldSpeedDev->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_SPEEDDEV, myTextFieldSpeedDev->getText().text(), undoList);
    } else {
        myTextFieldSpeedDev->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SPEEDDEV;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), undoList);
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldEmissionClass, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_EMISSIONCLASS, myTextFieldEmissionClass->getText().text())) {
        myTextFieldEmissionClass->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_EMISSIONCLASS, myTextFieldEmissionClass->getText().text(), undoList);
    } else {
        myTextFieldEmissionClass->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_EMISSIONCLASS;
    }
    // set color of myTextFieldWidth, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_WIDTH, myTextFieldWidth->getText().text())) {
        myTextFieldWidth->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_WIDTH, myTextFieldWidth->getText().text(), undoList);
    } else {
        myTextFieldWidth->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_WIDTH;
    }
    // set color of myTextFieldFilename, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_IMGFILE, myTextFieldFilename->getText().text())) {
        myTextFieldFilename->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_IMGFILE, myTextFieldFilename->getText().text(), undoList);
    } else {
        myTextFieldFilename->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_IMGFILE;
    }
    // set color of myTextFieldImpatience, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_IMPATIENCE, myTextFieldImpatience->getText().text())) {
        myTextFieldImpatience->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_IMPATIENCE, myTextFieldImpatience->getText().text(), undoList);
    } else {
        myTextFieldImpatience->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_IMPATIENCE;
    }
    // set color of myTextFieldLaneChangeModel, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_LANE_CHANGE_MODEL, myTextFieldLaneChangeModel->getText().text())) {
        myTextFieldLaneChangeModel->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_LANE_CHANGE_MODEL, myTextFieldLaneChangeModel->getText().text(), undoList);
    } else {
        myTextFieldLaneChangeModel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LANE_CHANGE_MODEL;
    }
    // set color of myTextFieldCarFollowModel, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_CAR_FOLLOW_MODEL, myTextFieldCarFollowModel->getText().text())) {
        myTextFieldCarFollowModel->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL, myTextFieldCarFollowModel->getText().text(), undoList);
    } else {
        myTextFieldCarFollowModel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_CAR_FOLLOW_MODEL;
    }
    // set color of myTextFieldPersonCapacity, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_PERSON_CAPACITY, myTextFieldPersonCapacity->getText().text())) {
        myTextFieldPersonCapacity->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_PERSON_CAPACITY, myTextFieldPersonCapacity->getText().text(), undoList);
    } else {
        myTextFieldPersonCapacity->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_PERSON_CAPACITY;
    }
    // set color of myTextFieldContainerCapacity, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_CONTAINER_CAPACITY, myTextFieldContainerCapacity->getText().text())) {
        myTextFieldContainerCapacity->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_CONTAINER_CAPACITY, myTextFieldContainerCapacity->getText().text(), undoList);
    } else {
        myTextFieldContainerCapacity->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_CONTAINER_CAPACITY;
    }
    // set color of myTextFieldBoardingDuration, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_BOARDING_DURATION, myTextFieldBoardingDuration->getText().text())) {
        myTextFieldBoardingDuration->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_BOARDING_DURATION, myTextFieldBoardingDuration->getText().text(), undoList);
    } else {
        myTextFieldBoardingDuration->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_BOARDING_DURATION;
    }
    // set color of myTextFieldLoadingDuration, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_LOADING_DURATION, myTextFieldLoadingDuration->getText().text())) {
        myTextFieldLoadingDuration->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_LOADING_DURATION, myTextFieldLoadingDuration->getText().text(), undoList);
    } else {
        myTextFieldLoadingDuration->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LOADING_DURATION;
    }
    // set color of myTextFieldLatAlignment, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_LATALIGNMENT, myTextFieldLatAlignment->getText().text())) {
        myTextFieldLatAlignment->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_LATALIGNMENT, myTextFieldLatAlignment->getText().text(), undoList);
    } else {
        myTextFieldLatAlignment->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LATALIGNMENT;
    }
    // set color of myTextFieldMinGapLat, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_MINGAP, myTextFieldMinGapLat->getText().text())) {
        myTextFieldMinGapLat->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_MINGAP, myTextFieldMinGapLat->getText().text(), undoList);
    } else {
        myTextFieldMinGapLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MINGAP_LAT;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myEditedCalibratorVehicleType->isValid(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeedLat->getText().text())) {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorVehicleType->setAttribute(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeedLat->getText().text(), undoList);
    } else {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MAXSPEED_LAT;
    }
    return 1;
}


void
GNECalibratorVehicleTypeDialog::updateCalibratorVehicleTypeValues() {
    //set values of myEditedCalibratorVehicleType int fields
    myTextFieldVehicleTypeID->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_ID).c_str());
    myComboBoxVClass->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_VCLASS).c_str());
    myComboBoxShape->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_GUISHAPE).c_str());
    myTextFieldAccel->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_ACCEL).c_str());
    myTextFieldDecel->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_DECEL).c_str());
    myTextFieldSigma->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_SIGMA).c_str());
    myTextFieldTau->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_TAU).c_str());
    myTextFieldLength->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_LENGTH).c_str());
    myTextFieldMinGap->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_MINGAP).c_str());
    myTextFieldMaxSpeed->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_MAXSPEED).c_str());
    myTextFieldSpeedFactor->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_SPEEDFACTOR).c_str());
    myTextFieldSpeedDev->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_SPEEDDEV).c_str());
    myTextFieldColor->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_COLOR).c_str());
    myTextFieldEmissionClass->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_EMISSIONCLASS).c_str());
    myTextFieldWidth->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_WIDTH).c_str());
    myTextFieldFilename->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_IMGFILE).c_str());
    myTextFieldImpatience->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_IMPATIENCE).c_str());
    myTextFieldLaneChangeModel->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_LANE_CHANGE_MODEL).c_str());
    myTextFieldCarFollowModel->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL).c_str());
    myTextFieldPersonCapacity->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_PERSON_CAPACITY).c_str());
    myTextFieldContainerCapacity->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_CONTAINER_CAPACITY).c_str());
    myTextFieldBoardingDuration->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_BOARDING_DURATION).c_str());
    myTextFieldLoadingDuration->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_LOADING_DURATION).c_str());
    myTextFieldLatAlignment->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_LATALIGNMENT).c_str());
    myTextFieldMinGapLat->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_MINGAP_LAT).c_str());
    myTextFieldMaxSpeedLat->setText(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_MAXSPEED_LAT).c_str());
    // set image labels
    setVClassLabelImage();
}


void
GNECalibratorVehicleTypeDialog::setVClassLabelImage() {
    // set Icon in label depending of current VClass
    switch (getVehicleClassID(myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_VCLASS))) {
        case SVC_PRIVATE:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_PRIVATE));
            break;
        case SVC_EMERGENCY:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_EMERGENCY));
            break;
        case SVC_AUTHORITY:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_AUTHORITY));
            break;
        case SVC_ARMY:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_ARMY));
            break;
        case SVC_VIP:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_VIP));
            break;
        case SVC_PASSENGER:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_PASSENGER));
            break;
        case SVC_HOV:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_HOV));
            break;
        case SVC_TAXI:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_TAXI));
            break;
        case SVC_BUS:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_BUS));
            break;
        case SVC_COACH:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_COACH));
            break;
        case SVC_DELIVERY:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_DELIVERY));
            break;
        case SVC_TRUCK:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_TRUCK));
            break;
        case SVC_TRAILER:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_TRAILER));
            break;
        case SVC_TRAM:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_TRAM));
            break;
        case SVC_RAIL_URBAN:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_RAIL_URBAN));
            break;
        case SVC_RAIL:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_RAIL));
            break;
        case SVC_RAIL_ELECTRIC:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_RAIL_ELECTRIC));
            break;
        case SVC_MOTORCYCLE:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_MOTORCYCLE));
            break;
        case SVC_MOPED:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_MOPED));
            break;
        case SVC_BICYCLE:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_BICYCLE));
            break;
        case SVC_PEDESTRIAN:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_PEDESTRIAN));
            break;
        case SVC_E_VEHICLE:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_EVEHICLE));
            break;
        case SVC_SHIP:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_SHIP));
            break;
        case SVC_CUSTOM1:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_CUSTOM1));
            break;
        case SVC_CUSTOM2:
            myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_CUSTOM2));
            break;
        default:
            throw InvalidArgument("Invalid " + toString(SUMO_ATTR_VCLASS) + " " + myEditedCalibratorVehicleType->getAttribute(SUMO_ATTR_VCLASS));
            break;
    }
}

/****************************************************************************/
