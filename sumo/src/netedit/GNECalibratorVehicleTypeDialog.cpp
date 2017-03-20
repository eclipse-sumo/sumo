/****************************************************************************/
/// @file    GNECalibratorVehicleTypeDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id: GNECalibratorVehicleTypeDialog.cpp 23529 2017-03-18 10:22:36Z behrisch $
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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,           GNECalibratorVehicleTypeDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,           GNECalibratorVehicleTypeDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,            GNECalibratorVehicleTypeDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNECalibratorVehicleTypeDialog, FXDialogBox, GNECalibratorVehicleTypeDialogMap, ARRAYNUMBER(GNECalibratorVehicleTypeDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorVehicleTypeDialog::GNECalibratorVehicleTypeDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorVehicleType &calibratorVehicleType) :
    GNEAdditionalDialog(calibratorVehicleType.getCalibratorParent(), 500, 375),
    myCalibratorDialogParent(calibratorDialog),
    myCalibratorVehicleType(&calibratorVehicleType),
    myCalibratorVehicleTypeValid(true) {
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
    myComboBoxVClassLabelImage = new FXLabel(columnLeftValues, "", 0, GUIDesignLabelIconBigExtended);
    myComboBoxVClassLabelImage->setBackColor(FXRGBA(255,255,255,255));
    
    // FXComboBox for Shape
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_SHAPE).c_str(), 0, GUIDesignLabelThick);
    myComboBoxShape = new FXComboBox(columnRightLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxShapeLabelImage = new FXLabel(columnRightValues, "", 0, GUIDesignLabelIconBigExtended);
    myComboBoxShapeLabelImage->setBackColor(FXRGBA(255,255,255,255));
    
    // 01 create FXTextField and Label for vehicleTypeID
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ID).c_str(), 0, GUIDesignLabelThick);
    myTextFieldVehicleTypeID = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 02 create FXTextField and Label for Accel
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ACCEL).c_str(), 0, GUIDesignLabelThick);
    myTextFieldAccel = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);;

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
    
    // create copy of  GNECalibratorVehicleType
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
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myCalibratorVehicleType->getTag()) + " of " + toString(myCalibratorVehicleType->getCalibratorParent()->getTag())).c_str(), "%s",
                              (toString(myCalibratorVehicleType->getCalibratorParent()->getTag()) + "'s " + toString(myCalibratorVehicleType->getTag()) +
                               " cannot be updated because " + toString(myCalibratorVehicleType->getTag()) + " defined by " + toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " is invalid.").c_str());
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
}


/****************************************************************************/
