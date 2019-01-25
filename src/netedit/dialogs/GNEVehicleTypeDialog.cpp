/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleTypeDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Dialog for edit calibrator vehicleTypes
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/additionals/GNECalibrator.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/demandelements/GNEVehicleType.h>
#include <netedit/GNEUndoList.h>

#include "GNEVehicleTypeDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVehicleTypeDialog) GNEVehicleTypeDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,  GNEVehicleTypeDialog::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNEVehicleTypeDialog, GNEDemandElementDialog, GNEVehicleTypeDialogMap, ARRAYNUMBER(GNEVehicleTypeDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVehicleTypeDialog::GNEVehicleTypeDialog(GNEDemandElement* editedVehicleType, bool updatingElement) :
    GNEDemandElementDialog(editedVehicleType, updatingElement, 500, 370),
    myVehicleTypeValid(true),
    myInvalidAttr(SUMO_ATTR_NOTHING) {
    // change default header
    changeDemandElementDialogHeader(updatingElement ? "Edit " + myEditedDemandElement->getTagStr() + " of " : "Create " + myEditedDemandElement->getTagStr());

    // Create auxiliar frames for values
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeftLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnLeftValues = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightValues = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // create FXComboBox for VClass
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_VCLASS).c_str(), nullptr, GUIDesignLabelThick);
    myComboBoxVClass = new FXComboBox(columnLeftLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxVClassLabelImage = new FXLabel(columnLeftValues, "", nullptr, GUIDesignLabelIconExtendedx46Ticked);
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
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_GUISHAPE).c_str(), nullptr, GUIDesignLabelThick);
    myComboBoxShape = new FXComboBox(columnRightLabel, GUIDesignComboBoxNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignComboBox);
    myComboBoxShapeLabelImage = new FXLabel(columnRightValues, "", nullptr, GUIDesignLabelIconExtendedx46Ticked);
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
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ID).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldVehicleTypeID = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 02 create FXTextField and Label for Accel
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ACCEL).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldAccel = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 03 create FXTextField and Label for Decel
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DECEL).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldDecel = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 04 create FXTextField and Label for Sigma
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SIGMA).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldSigma = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 05 create FXTextField and Label for Tau
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_TAU).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldTau = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 06 create FXTextField and Label for Length
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_LENGTH).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldLength = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 07 create FXTextField and Label for MinGap
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_MINGAP).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldMinGap = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 08 create FXTextField and Label for MaxSpeed
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_MAXSPEED).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldMaxSpeed = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 09 create FXTextField and Label for SpeedFactor
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SPEEDFACTOR).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldSpeedFactor = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 10 create FXTextField and Label for SpeedDev
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SPEEDDEV).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldSpeedDev = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 11 create FXTextField and Label for Color
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_COLOR).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldColor = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 12 create FXTextField and Label for EmissionClass
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_EMISSIONCLASS).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldEmissionClass = new FXTextField(columnLeftValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 01 create FXTextField and Label for Width
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldWidth = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 02 create FXTextField and Label for Filename
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_IMGFILE).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldFilename = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 03 create FXTextField and Label for Impatience
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_IMPATIENCE).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldImpatience = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 04 create FXTextField and Label for LaneChangeModel
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LANE_CHANGE_MODEL).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldLaneChangeModel = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 05 create FXTextField and Label for CarFollowModel
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_CAR_FOLLOW_MODEL).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldCarFollowModel = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 06 create FXTextField and Label for PersonCapacity
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_PERSON_CAPACITY).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldPersonCapacity = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);

    // 07 create FXTextField and Label for ContainerCapacity
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_CONTAINER_CAPACITY).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldContainerCapacity = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldInt);

    // 08 create FXTextField and Label for BoardingDuration
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_BOARDING_DURATION).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldBoardingDuration = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 09 create FXTextField and Label for LoadingDuration
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LOADING_DURATION).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldLoadingDuration = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 10 create FXTextField and Label for LatAlignment
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LATALIGNMENT).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldLatAlignment = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // 11 create FXTextField and Label for MinGapLat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_MINGAP_LAT).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldMinGapLat = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // 12 create FXTextField and Label for MaxSpeedLat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_MAXSPEED_LAT).c_str(), nullptr, GUIDesignLabelThick);
    myTextFieldMaxSpeedLat = new FXTextField(columnRightValues, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextFieldReal);

    // update fields
    updateVehicleTypeValues();

    // start a undo list for editing local to this additional
    initChanges();

    // add element if we aren't updating an existent element
    if (myUpdatingElement == false) {
        myEditedDemandElement->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(myEditedDemandElement, true), true);
    }

    // open as modal dialog
    openAsModalDialog();
}


GNEVehicleTypeDialog::~GNEVehicleTypeDialog() {}


long
GNEVehicleTypeDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myVehicleTypeValid == false) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
        std::string operation1 = myUpdatingElement ? ("updating") : ("creating");
        std::string operation2 = myUpdatingElement ? ("updated") : ("created");
        std::string tagString = myEditedDemandElement->getTagStr();
        // open warning dialogBox
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error " + operation1 + " " + tagString).c_str(), "%s",
                              (tagString + " cannot be " + operation2 +
                               " because parameter " + toString(myInvalidAttr) +
                               " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
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
GNEVehicleTypeDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEVehicleTypeDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update fields
    updateVehicleTypeValues();
    return 1;
}


long
GNEVehicleTypeDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myVehicleTypeValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // set color of myComboBoxShape, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text())) {
        myComboBoxShape->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myComboBoxShape->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_GUISHAPE;
    }
    // set color of myComboBoxVClass, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text())) {
        myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
        setVClassLabelImage();
    } else {
        myComboBoxVClass->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_VCLASS;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text())) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else if (myEditedDemandElement->getAttribute(SUMO_ATTR_ID) == myTextFieldVehicleTypeID->getText().text()) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myTextFieldAccel, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_ACCEL, myTextFieldAccel->getText().text())) {
        myTextFieldAccel->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_ACCEL, myTextFieldAccel->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldAccel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_ACCEL;
    }
    // set color of myTextFieldDecel, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_DECEL, myTextFieldDecel->getText().text())) {
        myTextFieldDecel->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_DECEL, myTextFieldDecel->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldDecel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_DECEL;
    }
    // set color of myTextFieldSigma, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_SIGMA, myTextFieldSigma->getText().text())) {
        myTextFieldSigma->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_SIGMA, myTextFieldSigma->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldSigma->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SIGMA;
    }
    // set color of myTextFieldTau, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_TAU, myTextFieldTau->getText().text())) {
        myTextFieldTau->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_TAU, myTextFieldTau->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldTau->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_TAU;
    }
    // set color of myTextFieldLength, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_LENGTH, myTextFieldLength->getText().text())) {
        myTextFieldLength->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_LENGTH, myTextFieldLength->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldLength->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LENGTH;
    }
    // set color of myTextFieldMinGap, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_MINGAP, myTextFieldMinGap->getText().text())) {
        myTextFieldMinGap->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_MINGAP, myTextFieldMinGap->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMinGap->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MINGAP;
    }
    // set color of myTextFieldMaxSpeed, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeed->getText().text())) {
        myTextFieldMaxSpeed->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeed->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMaxSpeed->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MAXSPEED;
    }
    // set color of myTextFieldSpeedFactor, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_SPEEDFACTOR, myTextFieldSpeedFactor->getText().text())) {
        myTextFieldSpeedFactor->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_SPEEDFACTOR, myTextFieldSpeedFactor->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldSpeedFactor->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SPEEDFACTOR;
    }
    // set color of myTextFieldSpeedDev, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_SPEEDDEV, myTextFieldSpeedDev->getText().text())) {
        myTextFieldSpeedDev->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_SPEEDDEV, myTextFieldSpeedDev->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldSpeedDev->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_SPEEDDEV;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldEmissionClass, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_EMISSIONCLASS, myTextFieldEmissionClass->getText().text())) {
        myTextFieldEmissionClass->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_EMISSIONCLASS, myTextFieldEmissionClass->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldEmissionClass->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_EMISSIONCLASS;
    }
    // set color of myTextFieldWidth, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_WIDTH, myTextFieldWidth->getText().text())) {
        myTextFieldWidth->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_WIDTH, myTextFieldWidth->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldWidth->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_WIDTH;
    }
    // set color of myTextFieldFilename, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_IMGFILE, myTextFieldFilename->getText().text())) {
        myTextFieldFilename->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_IMGFILE, myTextFieldFilename->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldFilename->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_IMGFILE;
    }
    // set color of myTextFieldImpatience, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_IMPATIENCE, myTextFieldImpatience->getText().text())) {
        myTextFieldImpatience->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_IMPATIENCE, myTextFieldImpatience->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldImpatience->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_IMPATIENCE;
    }
    // set color of myTextFieldLaneChangeModel, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_LANE_CHANGE_MODEL, myTextFieldLaneChangeModel->getText().text())) {
        myTextFieldLaneChangeModel->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_LANE_CHANGE_MODEL, myTextFieldLaneChangeModel->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldLaneChangeModel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LANE_CHANGE_MODEL;
    }
    // set color of myTextFieldCarFollowModel, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_CAR_FOLLOW_MODEL, myTextFieldCarFollowModel->getText().text())) {
        myTextFieldCarFollowModel->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL, myTextFieldCarFollowModel->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldCarFollowModel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_CAR_FOLLOW_MODEL;
    }
    // set color of myTextFieldPersonCapacity, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_PERSON_CAPACITY, myTextFieldPersonCapacity->getText().text())) {
        myTextFieldPersonCapacity->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_PERSON_CAPACITY, myTextFieldPersonCapacity->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldPersonCapacity->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_PERSON_CAPACITY;
    }
    // set color of myTextFieldContainerCapacity, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_CONTAINER_CAPACITY, myTextFieldContainerCapacity->getText().text())) {
        myTextFieldContainerCapacity->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_CONTAINER_CAPACITY, myTextFieldContainerCapacity->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldContainerCapacity->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_CONTAINER_CAPACITY;
    }
    // set color of myTextFieldBoardingDuration, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_BOARDING_DURATION, myTextFieldBoardingDuration->getText().text())) {
        myTextFieldBoardingDuration->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_BOARDING_DURATION, myTextFieldBoardingDuration->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldBoardingDuration->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_BOARDING_DURATION;
    }
    // set color of myTextFieldLoadingDuration, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_LOADING_DURATION, myTextFieldLoadingDuration->getText().text())) {
        myTextFieldLoadingDuration->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_LOADING_DURATION, myTextFieldLoadingDuration->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldLoadingDuration->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LOADING_DURATION;
    }
    // set color of myTextFieldLatAlignment, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_LATALIGNMENT, myTextFieldLatAlignment->getText().text())) {
        myTextFieldLatAlignment->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_LATALIGNMENT, myTextFieldLatAlignment->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldLatAlignment->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_LATALIGNMENT;
    }
    // set color of myTextFieldMinGapLat, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_MINGAP, myTextFieldMinGapLat->getText().text())) {
        myTextFieldMinGapLat->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_MINGAP, myTextFieldMinGapLat->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMinGapLat->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MINGAP_LAT;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myEditedDemandElement->isValid(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeedLat->getText().text())) {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(0, 0, 0));
        myEditedDemandElement->setAttribute(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeedLat->getText().text(), myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeValid = false;
        myInvalidAttr = SUMO_ATTR_MAXSPEED_LAT;
    }
    return 1;
}


void
GNEVehicleTypeDialog::updateVehicleTypeValues() {
    //set values of myEditedDemandElement int fields
    myTextFieldVehicleTypeID->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_ID).c_str());
    myComboBoxVClass->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS).c_str());
    myComboBoxShape->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_GUISHAPE).c_str());
    myTextFieldAccel->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_ACCEL).c_str());
    myTextFieldDecel->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_DECEL).c_str());
    myTextFieldSigma->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_SIGMA).c_str());
    myTextFieldTau->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_TAU).c_str());
    myTextFieldLength->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_LENGTH).c_str());
    myTextFieldMinGap->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_MINGAP).c_str());
    myTextFieldMaxSpeed->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_MAXSPEED).c_str());
    myTextFieldSpeedFactor->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_SPEEDFACTOR).c_str());
    myTextFieldSpeedDev->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_SPEEDDEV).c_str());
    myTextFieldColor->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_COLOR).c_str());
    myTextFieldEmissionClass->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_EMISSIONCLASS).c_str());
    myTextFieldWidth->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_WIDTH).c_str());
    myTextFieldFilename->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_IMGFILE).c_str());
    myTextFieldImpatience->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_IMPATIENCE).c_str());
    myTextFieldLaneChangeModel->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_LANE_CHANGE_MODEL).c_str());
    myTextFieldCarFollowModel->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL).c_str());
    myTextFieldPersonCapacity->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_PERSON_CAPACITY).c_str());
    myTextFieldContainerCapacity->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_CONTAINER_CAPACITY).c_str());
    myTextFieldBoardingDuration->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_BOARDING_DURATION).c_str());
    myTextFieldLoadingDuration->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_LOADING_DURATION).c_str());
    myTextFieldLatAlignment->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_LATALIGNMENT).c_str());
    myTextFieldMinGapLat->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_MINGAP_LAT).c_str());
    myTextFieldMaxSpeedLat->setText(myEditedDemandElement->getAttribute(SUMO_ATTR_MAXSPEED_LAT).c_str());
    // set image labels
    setVClassLabelImage();
}


void
GNEVehicleTypeDialog::setVClassLabelImage() {
    // set Icon in label depending of current VClass
    switch (getVehicleClassID(myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS))) {
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
            throw InvalidArgument("Invalid " + toString(SUMO_ATTR_VCLASS) + " " + myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS));
            break;
    }
}

/****************************************************************************/
