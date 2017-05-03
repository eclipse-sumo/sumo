/****************************************************************************/
/// @file    GNECalibratorVehicleTypeDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrator vehicleTypes
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

#include "GNECalibratorVehicleTypeDialog.h"
#include "GNECalibratorDialog.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorVehicleTypeDialog) GNECalibratorVehicleTypeDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,   GNECalibratorVehicleTypeDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,   GNECalibratorVehicleTypeDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,    GNECalibratorVehicleTypeDialog::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,  GNECalibratorVehicleTypeDialog::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNECalibratorVehicleTypeDialog, FXDialogBox, GNECalibratorVehicleTypeDialogMap, ARRAYNUMBER(GNECalibratorVehicleTypeDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorVehicleTypeDialog::GNECalibratorVehicleTypeDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorVehicleType& calibratorVehicleType, bool updatingElement) :
    GNEAdditionalDialog(calibratorVehicleType.getCalibratorParent(), 500, 375),
    myCalibratorDialogParent(calibratorDialog),
    myCalibratorVehicleType(&calibratorVehicleType),
    myUpdatingElement(updatingElement),
    myCalibratorVehicleTypeValid(true),
    myInvalidAttr(SUMO_ATTR_NOTHING) {
    // change default header
    changeAdditionalDialogHeader("Edit " + toString(calibratorVehicleType.getTag()) + " of " + toString(calibratorVehicleType.getCalibratorParent()->getTag()) +
                                 " '" + calibratorVehicleType.getCalibratorParent()->getID() + "'");

    // Create auxiliar frames for values
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeftLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnLeftValues = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightValues = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // FXComboBox for VClass
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_VCLASS).c_str(), 0, GUIDesignLabelThick);
    myComboBoxVClass = new FXComboBox(columnLeftLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxVClassLabelImage = new FXLabel(columnLeftValues, "", 0, GUIDesignLabelIconExtendedx46Ticked);
    myComboBoxVClassLabelImage->setBackColor(FXRGBA(255, 255, 255, 255));
    // fill combo Box
    std::vector<std::string> VClassStrings = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::iterator i = VClassStrings.begin(); i != VClassStrings.end(); i++) {
        if ((*i) != SumoVehicleClassStrings.getString(SVC_IGNORING)) {
            myComboBoxVClass->appendItem(i->c_str());
        }
    }
    myComboBoxVClass->setNumVisible(10);

    // FXComboBox for Shape
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_SHAPE).c_str(), 0, GUIDesignLabelThick);
    myComboBoxShape = new FXComboBox(columnRightLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxShapeLabelImage = new FXLabel(columnRightValues, "", 0, GUIDesignLabelIconExtendedx46Ticked);
    myComboBoxShapeLabelImage->setBackColor(FXRGBA(255, 255, 255, 255));
    // fill combo Box
    std::vector<std::string> VShapeStrings = SumoVehicleShapeStrings.getStrings();
    for (std::vector<std::string>::iterator i = VShapeStrings.begin(); i != VShapeStrings.end(); i++) {
        if ((*i) != SumoVehicleShapeStrings.getString(SVS_UNKNOWN)) {
            myComboBoxShape->appendItem(i->c_str());
        }
    }
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
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_FILE).c_str(), 0, GUIDesignLabelThick);
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

    // create copy of GNECalibratorVehicleType
    myCopyOfCalibratorVehicleType = new GNECalibratorVehicleType(myCalibratorVehicleType->getCalibratorParent());

    // copy all values of myCalibratorVehicleType into myCopyOfCalibratorVehicleType to set initial values
    (*myCopyOfCalibratorVehicleType) = (*myCalibratorVehicleType);

    // update fields
    updateCalibratorVehicleTypeValues();
}


GNECalibratorVehicleTypeDialog::~GNECalibratorVehicleTypeDialog() {
    // delete copy
    delete myCopyOfCalibratorVehicleType;
}


long
GNECalibratorVehicleTypeDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorVehicleTypeValid == false) {
        // write warning if netedit is running in testing mode
        if (myCalibratorDialogParent->getCalibratorParent()->getViewNet()->isTestingModeEnabled() == true) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning dialogBox
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error " + std::string((myUpdatingElement == true) ? ("updating") : ("creating")) + " " + toString(myCalibratorVehicleType->getCalibratorParent()->getTag()) +
                               "'s " + toString(myCalibratorVehicleType->getTag())).c_str(), "%s",
                              (toString(myCalibratorVehicleType->getCalibratorParent()->getTag()) + "'s " + toString(myCalibratorVehicleType->getTag()) +
                               " cannot be " + std::string((myUpdatingElement == true) ? ("updated") : ("created")) + " because parameter " + toString(myInvalidAttr) +
                               " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        if (myCalibratorDialogParent->getCalibratorParent()->getViewNet()->isTestingModeEnabled() == true) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else {
        // copy all values of myCopyOfCalibratorVehicleType into myCalibratorVehicleType
        (*myCalibratorVehicleType) = (*myCopyOfCalibratorVehicleType);
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNECalibratorVehicleTypeDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorVehicleTypeDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // copy all values of myCalibratorVehicleType into myCopyOfCalibratorVehicleType to set initial values
    (*myCopyOfCalibratorVehicleType) = (*myCalibratorVehicleType);
    // update fields
    updateCalibratorVehicleTypeValues();
    return 1;
}


long
GNECalibratorVehicleTypeDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myCalibratorVehicleTypeValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // set color of myComboBoxShape, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setShape(myComboBoxShape->getText().text()) == true) {
        myComboBoxShape->setTextColor(FXRGB(0, 0, 0));
    } else {
        myComboBoxShape->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SHAPE;
    }
    // set color of myComboBoxVClass, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setVClass(myComboBoxVClass->getText().text()) == true) {
        myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
        setVClassLabelImage();
    } else {
        myComboBoxVClass->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_VCLASS;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->getVehicleTypeID() == myTextFieldVehicleTypeID->getText().text()) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
    } else if (myCopyOfCalibratorVehicleType->setVehicleTypeID(myTextFieldVehicleTypeID->getText().text()) == true) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myTextFieldAccel, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setAccel(myTextFieldAccel->getText().text()) == true) {
        myTextFieldAccel->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldAccel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_ACCEL;
    }
    // set color of myTextFieldDecel, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setDecel(myTextFieldDecel->getText().text()) == true) {
        myTextFieldDecel->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldDecel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_DECEL;
    }
    // set color of myTextFieldSigma, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setSigma(myTextFieldSigma->getText().text()) == true) {
        myTextFieldSigma->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldSigma->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SIGMA;
    }
    // set color of myTextFieldTau, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setTau(myTextFieldTau->getText().text()) == true) {
        myTextFieldTau->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldTau->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_TAU;
    }
    // set color of myTextFieldLength, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setLength(myTextFieldLength->getText().text()) == true) {
        myTextFieldLength->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldLength->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LENGTH;
    }
    // set color of myTextFieldMinGap, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setMinGap(myTextFieldMinGap->getText().text()) == true) {
        myTextFieldMinGap->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldMinGap->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MINGAP;
    }
    // set color of myTextFieldMaxSpeed, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setMaxSpeed(myTextFieldMaxSpeed->getText().text()) == true) {
        myTextFieldMaxSpeed->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldMaxSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MAXSPEED;
    }
    // set color of myTextFieldSpeedFactor, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setSpeedFactor(myTextFieldSpeedFactor->getText().text()) == true) {
        myTextFieldSpeedFactor->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldSpeedFactor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SPEEDFACTOR;
    }
    // set color of myTextFieldSpeedDev, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setSpeedDev(myTextFieldSpeedDev->getText().text()) == true) {
        myTextFieldSpeedDev->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldSpeedDev->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SPEEDDEV;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setColor(myTextFieldColor->getText().text()) == true) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldEmissionClass, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setEmissionClass(myTextFieldEmissionClass->getText().text()) == true) {
        myTextFieldEmissionClass->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldEmissionClass->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_EMISSIONCLASS;
    }
    // set color of myTextFieldWidth, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setWidth(myTextFieldWidth->getText().text()) == true) {
        myTextFieldWidth->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldWidth->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_WIDTH;
    }
    // set color of myTextFieldFilename, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setFilename(myTextFieldFilename->getText().text()) == true) {
        myTextFieldFilename->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldFilename->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_FILE;
    }
    // set color of myTextFieldImpatience, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setImpatience(myTextFieldImpatience->getText().text()) == true) {
        myTextFieldImpatience->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldImpatience->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_IMPATIENCE;
    }
    // set color of myTextFieldLaneChangeModel, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setLaneChangeModel(myTextFieldLaneChangeModel->getText().text()) == true) {
        myTextFieldLaneChangeModel->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldLaneChangeModel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LANE_CHANGE_MODEL;
    }
    // set color of myTextFieldCarFollowModel, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setCarFollowModel(myTextFieldCarFollowModel->getText().text()) == true) {
        myTextFieldCarFollowModel->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldCarFollowModel->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_CAR_FOLLOW_MODEL;
    }
    // set color of myTextFieldPersonCapacity, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setPersonCapacity(myTextFieldPersonCapacity->getText().text()) == true) {
        myTextFieldPersonCapacity->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldPersonCapacity->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_PERSON_CAPACITY;
    }
    // set color of myTextFieldContainerCapacity, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setContainerCapacity(myTextFieldContainerCapacity->getText().text()) == true) {
        myTextFieldContainerCapacity->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldContainerCapacity->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_CONTAINER_CAPACITY;
    }
    // set color of myTextFieldBoardingDuration, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setBoardingDuration(myTextFieldBoardingDuration->getText().text()) == true) {
        myTextFieldBoardingDuration->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldBoardingDuration->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_BOARDING_DURATION;
    }
    // set color of myTextFieldLoadingDuration, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setLoadingDuration(myTextFieldLoadingDuration->getText().text()) == true) {
        myTextFieldLoadingDuration->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldLoadingDuration->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LOADING_DURATION;
    }
    // set color of myTextFieldLatAlignment, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setLatAlignment(myTextFieldLatAlignment->getText().text()) == true) {
        myTextFieldLatAlignment->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldLatAlignment->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LATALIGNMENT;
    }
    // set color of myTextFieldMinGapLat, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setMinGapLat(myTextFieldMinGapLat->getText().text()) == true) {
        myTextFieldMinGapLat->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldMinGapLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MINGAP_LAT;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myCopyOfCalibratorVehicleType->setMaxSpeedLat(myTextFieldMaxSpeedLat->getText().text()) == true) {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MAXSPEED_LAT;
    }
    return 1;
}


void
GNECalibratorVehicleTypeDialog::updateCalibratorVehicleTypeValues() {
    //set values of myCopyOfCalibratorVehicleType int fields
    myTextFieldVehicleTypeID->setText(myCopyOfCalibratorVehicleType->getVehicleTypeID().c_str());
    myComboBoxVClass->setText(getVehicleClassNames(myCopyOfCalibratorVehicleType->getVClass()).c_str());
    myComboBoxShape->setText(getVehicleShapeName(myCopyOfCalibratorVehicleType->getShape()).c_str());
    myTextFieldAccel->setText(toString(myCopyOfCalibratorVehicleType->getAccel()).c_str());
    myTextFieldDecel->setText(toString(myCopyOfCalibratorVehicleType->getDecel()).c_str());
    myTextFieldSigma->setText(toString(myCopyOfCalibratorVehicleType->getSigma()).c_str());
    myTextFieldTau->setText(toString(myCopyOfCalibratorVehicleType->getTau()).c_str());
    myTextFieldLength->setText(toString(myCopyOfCalibratorVehicleType->getLength()).c_str());
    myTextFieldMinGap->setText(toString(myCopyOfCalibratorVehicleType->getMinGap()).c_str());
    myTextFieldMaxSpeed->setText(toString(myCopyOfCalibratorVehicleType->getMaxSpeed()).c_str());
    myTextFieldSpeedFactor->setText(toString(myCopyOfCalibratorVehicleType->getSpeedFactor()).c_str());
    myTextFieldSpeedDev->setText(toString(myCopyOfCalibratorVehicleType->getSpeedDev()).c_str());
    myTextFieldColor->setText(myCopyOfCalibratorVehicleType->getColor().c_str());
    myTextFieldEmissionClass->setText(myCopyOfCalibratorVehicleType->getEmissionClass().c_str());
    myTextFieldWidth->setText(toString(myCopyOfCalibratorVehicleType->getWidth()).c_str());
    myTextFieldFilename->setText(myCopyOfCalibratorVehicleType->getFilename().c_str());
    myTextFieldImpatience->setText(toString(myCopyOfCalibratorVehicleType->getImpatience()).c_str());
    myTextFieldLaneChangeModel->setText(toString(myCopyOfCalibratorVehicleType->getLaneChangeModel()).c_str());
    myTextFieldCarFollowModel->setText(myCopyOfCalibratorVehicleType->getCarFollowModel().c_str());
    myTextFieldPersonCapacity->setText(toString(myCopyOfCalibratorVehicleType->getPersonCapacity()).c_str());
    myTextFieldContainerCapacity->setText(toString(myCopyOfCalibratorVehicleType->getContainerCapacity()).c_str());
    myTextFieldBoardingDuration->setText(toString(myCopyOfCalibratorVehicleType->getBoardingDuration()).c_str());
    myTextFieldLoadingDuration->setText(toString(myCopyOfCalibratorVehicleType->getLoadingDuration()).c_str());
    myTextFieldLatAlignment->setText(toString(myCopyOfCalibratorVehicleType->getLatAlignment()).c_str());
    myTextFieldMinGapLat->setText(toString(myCopyOfCalibratorVehicleType->getMinGapLat()).c_str());
    myTextFieldMaxSpeedLat->setText(toString(myCopyOfCalibratorVehicleType->getMaxSpeedLat()).c_str());
    // set image labels
    setVClassLabelImage();
}


void
GNECalibratorVehicleTypeDialog::setVClassLabelImage() {
    // set Icon in label depending of current VClass
    switch (myCopyOfCalibratorVehicleType->getVClass()) {
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
            throw InvalidArgument("Invalid " + toString(SUMO_ATTR_VCLASS) + " " + toString(myCopyOfCalibratorVehicleType->getVClass()));
            break;
    }
}

/****************************************************************************/
