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
// Dialog for edit vehicleTypes
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

FXDEFMAP(GNEVehicleTypeDialog::VTypeCommonAtributes) VTypeCommonAtributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEVehicleTypeDialog::VTypeCommonAtributes::onCmdSetVariable),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEVehicleTypeDialog::VTypeCommonAtributes::onCmdSetColor)
};

FXDEFMAP(GNEVehicleTypeDialog::CarFollowingModelParameters) CarFollowingModelParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEVehicleTypeDialog::CarFollowingModelParameters::onCmdSetVariable),
};


// Object implementation
FXIMPLEMENT(GNEVehicleTypeDialog::VTypeCommonAtributes,         FXGroupBox,     VTypeCommonAtributesMap,        ARRAYNUMBER(VTypeCommonAtributesMap))
FXIMPLEMENT(GNEVehicleTypeDialog::CarFollowingModelParameters,  FXGroupBox,     CarFollowingModelParametersMap, ARRAYNUMBER(CarFollowingModelParametersMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VClassRow - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeCommonAtributes::VClassRow::VClassRow(VTypeCommonAtributes* VTypeCommonAtributesParent, FXVerticalFrame* column) :
    FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame),
    myVTypeCommonAtributesParent(VTypeCommonAtributesParent) {
    // create two auxiliar frames
    FXVerticalFrame* verticalFrameLabelAndComboBox = new FXVerticalFrame(this, GUIDesignAuxiliarVerticalFrame);
    // create FXComboBox for VClass
    new FXLabel(verticalFrameLabelAndComboBox, toString(SUMO_ATTR_VCLASS).c_str(), nullptr, GUIDesignLabelAttribute150);
    myComboBoxVClass = new FXComboBox(verticalFrameLabelAndComboBox, GUIDesignComboBoxNCol, 
        VTypeCommonAtributesParent, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myComboBoxVClassLabelImage = new FXLabel(this, "", nullptr, GUIDesignLabelTickedIcon180x46);
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
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::VClassRow::setVariable() {
    // set color of myComboBoxVClass, depending if current value is valid or not
    myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
    if (myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text())) {
        myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
        myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text(), 
            myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
        setVClassLabelImage();
    } else {
        myComboBoxVClass->setTextColor(FXRGB(255, 0, 0));
        myVTypeCommonAtributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
        myVTypeCommonAtributesParent->myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_VCLASS;
    }
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::VClassRow::updateValues() {
    myComboBoxVClass->setText(myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS).c_str());
    setVClassLabelImage();
}


void
GNEVehicleTypeDialog::VTypeCommonAtributes::VClassRow::setVClassLabelImage() {
    // by default vclass is passenger
    if (myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS).empty()) {
        myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_PASSENGER));
    } else {
        // set Icon in label depending of current VClass
        switch (getVehicleClassID(myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS))) {
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
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_IGNORING));
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VShapeRow - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeCommonAtributes::VShapeRow::VShapeRow(VTypeCommonAtributes* VTypeCommonAtributesParent, FXVerticalFrame* column) :
    FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame),
    myVTypeCommonAtributesParent(VTypeCommonAtributesParent) {
    // create two auxiliar frames
    FXVerticalFrame* verticalFrameLabelAndComboBox = new FXVerticalFrame(this, GUIDesignAuxiliarVerticalFrame);
    // create combo for for vehicle shapes
    new FXLabel(verticalFrameLabelAndComboBox, toString(SUMO_ATTR_GUISHAPE).c_str(), nullptr, GUIDesignLabelAttribute150);
    myComboBoxShape = new FXComboBox(verticalFrameLabelAndComboBox, GUIDesignComboBoxNCol, 
        VTypeCommonAtributesParent, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myComboBoxShapeLabelImage = new FXLabel(this, "", nullptr, GUIDesignLabelTickedIcon180x46);
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
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::VShapeRow::setVariable() {
    // set color of myComboBoxShape, depending if current value is valid or not
    if (myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text())) {
        myComboBoxShape->setTextColor(FXRGB(0, 0, 0));
        myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text(), 
            myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
        setVShapeLabelImage();
    } else {
        myComboBoxShape->setTextColor(FXRGB(255, 0, 0));
        myVTypeCommonAtributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
        myVTypeCommonAtributesParent->myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_GUISHAPE;
    }
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::VShapeRow::updateValues() {
    myComboBoxShape->setText(myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_GUISHAPE).c_str());
    setVShapeLabelImage();
}


void
GNEVehicleTypeDialog::VTypeCommonAtributes::VShapeRow::setVShapeLabelImage() {
    // set Icon in label depending of current VClass
    switch (getVehicleShapeID(myVTypeCommonAtributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_GUISHAPE))) {
        case SVS_UNKNOWN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_UNKNOWN));
            break;
        case SVS_PEDESTRIAN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_PEDESTRIAN));
            break;
        case SVS_BICYCLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_BICYCLE));
            break;
        case SVS_MOPED:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_MOPED));
            break;
        case SVS_MOTORCYCLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_MOTORCYCLE));
            break;
        case SVS_PASSENGER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_PASSENGER));
            break;
        case SVS_PASSENGER_SEDAN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_PASSENGER_SEDAN));
            break;
        case SVS_PASSENGER_HATCHBACK:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_PASSENGER_HATCHBACK));
            break;
        case SVS_PASSENGER_WAGON:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_PASSENGER_WAGON));
            break;
        case SVS_PASSENGER_VAN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_PASSENGER_VAN));
            break;
        case SVS_DELIVERY:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_DELIVERY));
            break;
        case SVS_TRUCK:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_TRUCK));
            break;
        case SVS_TRUCK_SEMITRAILER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_TRUCK_SEMITRAILER));
            break;
        case SVS_TRUCK_1TRAILER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_TRUCK_1TRAILER));
            break;
        case SVS_BUS:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_BUS));
            break;
        case SVS_BUS_COACH:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_BUS_COACH));
            break;
        case SVS_BUS_FLEXIBLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_BUS_FLEXIBLE));
            break;
        case SVS_BUS_TROLLEY:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_BUS_TROLLEY));
            break;
        case SVS_RAIL:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_RAIL));
            break;
        case SVS_RAIL_CAR:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_RAIL_CAR));
            break;
        case SVS_RAIL_CARGO:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_RAIL_CARGO));
            break;
        case SVS_E_VEHICLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_E_VEHICLE));
            break;
        case SVS_ANT:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_ANT));
            break;
        case SVS_SHIP:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_SHIP));
            break;
        case SVS_EMERGENCY:
        case SVS_FIREBRIGADE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_EMERGENCY));
            break;
        case SVS_POLICE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_POLICE));
            break;
        case SVS_RICKSHAW:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VSHAPE_RICKSHAW));
            break;
        default:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(ICON_VCLASS_IGNORING));
            break;
    }
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VTypeCommonAtributes - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeCommonAtributes::VTypeCommonAtributes(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column) :
    FXGroupBox(column, "Vehicle Type attributes", GUIDesignGroupBoxFrame),
    myVehicleTypeDialog(vehicleTypeDialog) {
    // create two columns
    FXHorizontalFrame* columnsCommonVTypes = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // build left attributes
    buildCommonAttributesA(new FXVerticalFrame(columnsCommonVTypes, GUIDesignAuxiliarFrame));
    // build right attributes
    buildCommonAttributesB(new FXVerticalFrame(columnsCommonVTypes, GUIDesignAuxiliarFrame));
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::buildCommonAttributesA(FXVerticalFrame* column) {
    // 01 Create VClassRow
    myVClassRow = new VClassRow(this, column);

    // 02 create FXTextField and Label for vehicleTypeID
    myTextFieldVehicleTypeID = buildRowFloat(column, SUMO_ATTR_ID);

    // 03 create FXTextField and Button for Color
    FXHorizontalFrame* row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    myButtonColor = new FXButton(row, toString(SUMO_ATTR_COLOR).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonRectangular150x23);
    myTextFieldColor = new FXTextField(row, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth180);

    // 04 create FXTextField and Label for Length
    myTextFieldLength = buildRowFloat(column, SUMO_ATTR_LENGTH);

    // 05 create FXTextField and Label for MinGap
    myTextFieldMinGap = buildRowFloat(column, SUMO_ATTR_MINGAP);

    // 06 create FXTextField and Label for MaxSpeed
    myTextFieldMaxSpeed = buildRowFloat(column, SUMO_ATTR_MAXSPEED);

    // 07 create FXTextField and Label for SpeedFactor
    myTextFieldSpeedFactor = buildRowFloat(column, SUMO_ATTR_SPEEDFACTOR);

    // 08 create FXTextField and Label for SpeedDev
    myTextFieldSpeedDev = buildRowFloat(column, SUMO_ATTR_SPEEDDEV);

    // 09 create FXTextField and Label for EmissionClass
    myTextFieldEmissionClass = buildRowFloat(column, SUMO_ATTR_EMISSIONCLASS);

    // 10 create FXTextField and Label for Width
    myTextFieldWidth = buildRowFloat(column, SUMO_ATTR_WIDTH);

    // 11 create FXTextField and Label for Filename
    myTextFieldFilename = buildRowString(column, SUMO_ATTR_IMGFILE);
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::buildCommonAttributesB(FXVerticalFrame* column) {
    // 01 Create VShapeRow
    myVShapeRow = new VShapeRow(this, column);

    // 02 create FXTextField and Label for Impatience
    myTextFieldImpatience = buildRowFloat(column, SUMO_ATTR_IMPATIENCE);

    // 03 create FXComboBox and Label for LaneChangeModel
    FXHorizontalFrame* row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, toString(SUMO_ATTR_LANE_CHANGE_MODEL).c_str(), nullptr, GUIDesignLabelAttribute150);
    myComboBoxLaneChangeModel = new FXComboBox(row, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    
    // fill combo Box with all VClass
    std::vector<std::string> laneChangeModelStrings = SUMOXMLDefinitions::LaneChangeModels.getStrings();
    for (auto i : laneChangeModelStrings) {
        myComboBoxLaneChangeModel->appendItem(i.c_str());
    }
    myComboBoxLaneChangeModel->setNumVisible(myComboBoxLaneChangeModel->getNumItems());

    // 04 create FXTextField and Label for PersonCapacity
    myTextFieldPersonCapacity = buildRowInt(column, SUMO_ATTR_PERSON_CAPACITY);

    // 05 create FXTextField and Label for ContainerCapacity
    myTextFieldContainerCapacity = buildRowInt(column, SUMO_ATTR_CONTAINER_CAPACITY);

    // 06 create FXTextField and Label for BoardingDuration
    myTextFieldBoardingDuration = buildRowFloat(column, SUMO_ATTR_BOARDING_DURATION);

    // 07 create FXTextField and Label for LoadingDuration
    myTextFieldLoadingDuration = buildRowFloat(column, SUMO_ATTR_LOADING_DURATION);

    // 08 create ComboBox and Label for LatAlignment
    row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, toString(SUMO_ATTR_LATALIGNMENT).c_str(), nullptr, GUIDesignLabelAttribute150);
    myComboBoxLatAlignment = new FXComboBox(row, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    
    // fill combo Box with all Lateral Alignments
    std::vector<std::string> lateralAlignmentsStrings = SUMOXMLDefinitions::LateralAlignments.getStrings();
    for (auto i : lateralAlignmentsStrings) {
        myComboBoxLatAlignment->appendItem(i.c_str());
    }
    myComboBoxLatAlignment->setNumVisible(myComboBoxLatAlignment->getNumItems());

    // 09 create FXTextField and Label for MinGapLat
    myTextFieldMinGapLat = buildRowFloat(column, SUMO_ATTR_MINGAP_LAT);

    // 10 create FXTextField and Label for MaxSpeedLat
    myTextFieldMaxSpeedLat = buildRowFloat(column, SUMO_ATTR_MAXSPEED_LAT);

    // 11 create FXTextField and Label for ActionStepLenght
    myTextFieldActionStepLenght = buildRowFloat(column, SUMO_ATTR_ACTIONSTEPLENGTH);
}


void 
GNEVehicleTypeDialog::VTypeCommonAtributes::updateValues() {
    // set variables of special rows VType and VShape
    myVClassRow->updateValues();
    myVShapeRow->updateValues();
    //set values of myEditedDemandElement into fields
    myTextFieldVehicleTypeID->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_ID).c_str());
    myTextFieldLength->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_LENGTH).c_str());
    myTextFieldMinGap->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_MINGAP).c_str());
    myTextFieldMaxSpeed->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_MAXSPEED).c_str());
    myTextFieldSpeedFactor->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_SPEEDFACTOR).c_str());
    myTextFieldSpeedDev->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_SPEEDDEV).c_str());
    myTextFieldColor->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_COLOR).c_str());
    myTextFieldEmissionClass->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_EMISSIONCLASS).c_str());
    myTextFieldWidth->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_WIDTH).c_str());
    myTextFieldFilename->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_IMGFILE).c_str());
    myTextFieldImpatience->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_IMPATIENCE).c_str());
    myComboBoxLaneChangeModel->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_LANE_CHANGE_MODEL).c_str());
    myTextFieldPersonCapacity->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_PERSON_CAPACITY).c_str());
    myTextFieldContainerCapacity->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_CONTAINER_CAPACITY).c_str());
    myTextFieldBoardingDuration->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_BOARDING_DURATION).c_str());
    myTextFieldLoadingDuration->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_LOADING_DURATION).c_str());
    myComboBoxLatAlignment->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_LATALIGNMENT).c_str());
    myTextFieldMinGapLat->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_MINGAP_LAT).c_str());
    myTextFieldMaxSpeedLat->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_MAXSPEED_LAT).c_str());
}


long 
GNEVehicleTypeDialog::VTypeCommonAtributes::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myVehicleTypeDialog->myVehicleTypeValid = true;
    myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_NOTHING;
    // set variables of special rows VType and VShape
    myVClassRow->setVariable();
    myVShapeRow->setVariable();
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text())) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else if (myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_ID) == myTextFieldVehicleTypeID->getText().text()) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myTextFieldLength, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_LENGTH, myTextFieldLength->getText().text())) {
        myTextFieldLength->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_LENGTH, myTextFieldLength->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldLength->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_LENGTH;
    }
    // set color of myTextFieldMinGap, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_MINGAP, myTextFieldMinGap->getText().text())) {
        myTextFieldMinGap->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_MINGAP, myTextFieldMinGap->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMinGap->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_MINGAP;
    }
    // set color of myTextFieldMaxSpeed, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeed->getText().text())) {
        myTextFieldMaxSpeed->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeed->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMaxSpeed->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_MAXSPEED;
    }
    // set color of myTextFieldSpeedFactor, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_SPEEDFACTOR, myTextFieldSpeedFactor->getText().text())) {
        myTextFieldSpeedFactor->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_SPEEDFACTOR, myTextFieldSpeedFactor->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldSpeedFactor->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_SPEEDFACTOR;
    }
    // set color of myTextFieldSpeedDev, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_SPEEDDEV, myTextFieldSpeedDev->getText().text())) {
        myTextFieldSpeedDev->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_SPEEDDEV, myTextFieldSpeedDev->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldSpeedDev->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_SPEEDDEV;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldEmissionClass, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_EMISSIONCLASS, myTextFieldEmissionClass->getText().text())) {
        myTextFieldEmissionClass->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_EMISSIONCLASS, myTextFieldEmissionClass->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldEmissionClass->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_EMISSIONCLASS;
    }
    // set color of myTextFieldWidth, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_WIDTH, myTextFieldWidth->getText().text())) {
        myTextFieldWidth->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_WIDTH, myTextFieldWidth->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldWidth->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_WIDTH;
    }
    // set color of myTextFieldFilename, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_IMGFILE, myTextFieldFilename->getText().text())) {
        myTextFieldFilename->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_IMGFILE, myTextFieldFilename->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldFilename->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_IMGFILE;
    }
    // set color of myTextFieldImpatience, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_IMPATIENCE, myTextFieldImpatience->getText().text())) {
        myTextFieldImpatience->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_IMPATIENCE, myTextFieldImpatience->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldImpatience->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_IMPATIENCE;
    }
    // set color of myTextFieldLaneChangeModel, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_LANE_CHANGE_MODEL, myComboBoxLaneChangeModel->getText().text())) {
        myComboBoxLaneChangeModel->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_LANE_CHANGE_MODEL, myComboBoxLaneChangeModel->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myComboBoxLaneChangeModel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_LANE_CHANGE_MODEL;
    }
    // set color of myTextFieldPersonCapacity, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_PERSON_CAPACITY, myTextFieldPersonCapacity->getText().text())) {
        myTextFieldPersonCapacity->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_PERSON_CAPACITY, myTextFieldPersonCapacity->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldPersonCapacity->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_PERSON_CAPACITY;
    }
    // set color of myTextFieldContainerCapacity, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_CONTAINER_CAPACITY, myTextFieldContainerCapacity->getText().text())) {
        myTextFieldContainerCapacity->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_CONTAINER_CAPACITY, myTextFieldContainerCapacity->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldContainerCapacity->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_CONTAINER_CAPACITY;
    }
    // set color of myTextFieldBoardingDuration, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_BOARDING_DURATION, myTextFieldBoardingDuration->getText().text())) {
        myTextFieldBoardingDuration->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_BOARDING_DURATION, myTextFieldBoardingDuration->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldBoardingDuration->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_BOARDING_DURATION;
    }
    // set color of myTextFieldLoadingDuration, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_LOADING_DURATION, myTextFieldLoadingDuration->getText().text())) {
        myTextFieldLoadingDuration->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_LOADING_DURATION, myTextFieldLoadingDuration->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldLoadingDuration->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_LOADING_DURATION;
    }
    // set color of myTextFieldLatAlignment, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_LATALIGNMENT, myComboBoxLatAlignment->getText().text())) {
        myComboBoxLatAlignment->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_LATALIGNMENT, myComboBoxLatAlignment->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myComboBoxLatAlignment->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_LATALIGNMENT;
    }
    // set color of myTextFieldMinGapLat, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_MINGAP, myTextFieldMinGapLat->getText().text())) {
        myTextFieldMinGapLat->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_MINGAP, myTextFieldMinGapLat->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMinGapLat->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_MINGAP_LAT;
    }
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeedLat->getText().text())) {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_MAXSPEED, myTextFieldMaxSpeedLat->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myTextFieldMaxSpeedLat->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_MAXSPEED_LAT;
    }
    return true;
}


long 
GNEVehicleTypeDialog::VTypeCommonAtributes::onCmdSetColor(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldColor->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
        myTextFieldColor->setText(newValue.c_str());
        if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_COLOR, newValue)) {
            myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_COLOR, newValue, myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
            // If previously value was incorrect, change font color to black
            myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
            myTextFieldColor->killFocus();
        }
    }
    return 1;
}


FXTextField* 
GNEVehicleTypeDialog::VTypeCommonAtributes::buildRowInt(FXPacker* column, SumoXMLAttr attr) {
    FXHorizontalFrame* row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, toString(attr).c_str(), nullptr, GUIDesignLabelAttribute150);
    return new FXTextField(row, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth180Int);
}


FXTextField * 
GNEVehicleTypeDialog::VTypeCommonAtributes::buildRowFloat(FXPacker* column, SumoXMLAttr attr) {
    FXHorizontalFrame* row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, toString(attr).c_str(), nullptr, GUIDesignLabelAttribute150);
    return new FXTextField(row, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth180Real);
}


FXTextField * 
GNEVehicleTypeDialog::VTypeCommonAtributes::buildRowString(FXPacker* column, SumoXMLAttr attr) {
    FXHorizontalFrame* row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, toString(attr).c_str(), nullptr, GUIDesignLabelAttribute150);
    return new FXTextField(row, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth180);
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VShapeRow - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelParameters(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column) :
    FXGroupBox(column, "Car Following Model", GUIDesignGroupBoxFrame),
    myVehicleTypeDialog(vehicleTypeDialog) {

    // create vertical frame for rows
    myVerticalFrameRows = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);

    // declare combo box
    FXHorizontalFrame* row = new FXHorizontalFrame(myVerticalFrameRows, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, "Algorithm", nullptr, GUIDesignLabelAttribute150);
    myComboBoxCarFollowModel = new FXComboBox(row, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    
    // fill combo Box with all Car following models
    std::vector<std::string> CFModels = SUMOXMLDefinitions::CarFollowModels.getStrings();
    for (auto i : CFModels) {
        myComboBoxCarFollowModel->appendItem(i.c_str());
    }
    myComboBoxCarFollowModel->setNumVisible(10);

    // 01 create FX and Label for Accel
    myAccelRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_ACCEL);
    myRows.push_back(myAccelRow);

    // 02 create FX and Label for Decel
    myDecelRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_DECEL);
    myRows.push_back(myDecelRow);

    // 03 create FX and Label for Apparent decel
    myApparentDecelRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_APPARENTDECEL);
    myRows.push_back(myApparentDecelRow);

    // 04 create FX and Label for emergency decel
    myEmergencyDecelRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_EMERGENCYDECEL);
    myRows.push_back(myEmergencyDecelRow);

    // 05 create FX and Label for Sigma
    mySigmaRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_SIGMA);
    myRows.push_back(mySigmaRow);

    // 06 create FX and Label for Tau
    myTauRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TAU);
    myRows.push_back(myTauRow);

    // 07 myMinGapFactor FX and Label for MinGapFactor
    myMinGapFactorRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_COLLISION_MINGAP_FACTOR);
    myRows.push_back(myMinGapFactorRow);

    // 08 create FX and Label for K
    myKRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_K);
    myRows.push_back(myKRow);

    // 09 create FX and Label for PHI
    myPhiRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_KERNER_PHI);
    myRows.push_back(myPhiRow);

    // 10 create FX and Label for Deleta
    myDeltaRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_IDM_DELTA);
    myRows.push_back(myDeltaRow);

    // 11 create FX and Label for Stepping
    mySteppingRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_IDM_STEPPING);
    myRows.push_back(mySteppingRow);

    // 12 create FX and Label for Security
    mySecurityRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_WIEDEMANN_SECURITY);
    myRows.push_back(mySecurityRow);

    // 13 create FX and Label for Estimation
    myEstimationRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_WIEDEMANN_ESTIMATION);
    myRows.push_back(myEstimationRow);

    // 14 create FX and Label for TMP1
    myTmp1Row = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TMP1);
    myRows.push_back(myTmp1Row);

    // 15 create FX and Label for TMP2
    myTmp2Row = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TMP2);
    myRows.push_back(myTmp2Row);

    // 16 create FX and Label for TMP3
    myTmp3Row = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TMP3);
    myRows.push_back(myTmp3Row);

    // 17 create FX and Label for TMP4
    myTmp4Row = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TMP4);
    myRows.push_back(myTmp4Row);

    // 18 create FX and Label for TMP5
    myTmp5Row = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TMP5);
    myRows.push_back(myTmp5Row);

    // 19 create FX and Label for Estimation
    myTrainTypeRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_TRAIN_TYPE);
    myRows.push_back(myTrainTypeRow);

    // 20 create FX and Label for Tau Last
    myTrauLastRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_PWAGNER2009_TAULAST);
    myRows.push_back(myTrauLastRow);

    // 21 create FX and Label for Aprob
    myAprobRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_PWAGNER2009_APPROB);
    myRows.push_back(myAprobRow);

    // 22 create FX and Label for Adapt Factor
    myAdaptFactorRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_IDMM_ADAPT_FACTOR);
    myRows.push_back(myAdaptFactorRow);

    // 23 create FX and Label for Adapt Time
    myAdaptTimeRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_IDMM_ADAPT_TIME);
    myRows.push_back(myAdaptTimeRow);

    // create myLabelIncompleteAttribute
    myLabelIncompleteAttribute = new FXLabel(myVerticalFrameRows, "Some attributes wasn't\nimplemented yet", nullptr, GUIDesignLabelAboutInfoCenter);
    myLabelIncompleteAttribute->hide();

    // show or hidde ComboBox depending of current selected CFM
    refreshCFMFields();
}


void 
GNEVehicleTypeDialog::CarFollowingModelParameters::refreshCFMFields() {
    // start hidding all rows
    for (const auto &i : myRows) {
        i->hide();
    }
    // hide myLabelIncompleteAttribute
    myLabelIncompleteAttribute->hide();
    // show textfield depending of current CFM
    if (SUMOXMLDefinitions::CarFollowModels.hasString(myComboBoxCarFollowModel->getText().text())) {
        // show textfield depending of selected CFM
        switch (SUMOXMLDefinitions::CarFollowModels.get(myComboBoxCarFollowModel->getText().text())) {
            case SUMO_TAG_CF_KRAUSS:
            case SUMO_TAG_CF_KRAUSS_ORIG1:
            case SUMO_TAG_CF_KRAUSS_PLUS_SLOPE:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myApparentDecelRow->show();
                myEmergencyDecelRow->show();
                mySigmaRow->show();
                break;
            case SUMO_TAG_CF_KRAUSSX:
                myTauRow->show();
                myTmp1Row->show();
                myTmp2Row->show();
                myTmp3Row->show();
                myTmp4Row->show();
                myTmp5Row->show();
                break;
            case SUMO_TAG_CF_SMART_SK:
            case SUMO_TAG_CF_DANIEL1:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                mySigmaRow->show();
                myMinGapFactorRow->show();
                myTmp1Row->show();
                myTmp2Row->show();
                myTmp3Row->show();
                myTmp4Row->show();
                myTmp5Row->show();
                break;
            case SUMO_TAG_CF_PWAGNER2009:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                mySigmaRow->show();
                myMinGapFactorRow->show();
                myTrauLastRow->show();
                myAprobRow->show();
                break;
            case SUMO_TAG_CF_IDM:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                mySteppingRow->show();
                myMinGapFactorRow->show();
                break;
            case SUMO_TAG_CF_IDMM:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                mySteppingRow->show();
                myMinGapFactorRow->show();
                myAdaptFactorRow->show();
                myAdaptTimeRow->show();
                break;
            case SUMO_TAG_CF_BKERNER:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                myKRow->show();
                myPhiRow->show();
                myMinGapFactorRow->show();
                break;
            case SUMO_TAG_CF_WIEDEMANN:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();  
                myMinGapFactorRow->show();
                mySecurityRow->show();
                myEstimationRow->show();
                break;
            case SUMO_TAG_CF_RAIL:
                myTauRow->show();
                myTrainTypeRow->show();
                break;
            case SUMO_TAG_CF_ACC:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                myMinGapFactorRow->show();
                // show myLabelIncompleteAttribute
                myLabelIncompleteAttribute->show();
                /**
                The follow parameters has to be added:
                    SUMO_ATTR_SC_GAIN
                    SUMO_ATTR_GCC_GAIN_SPEED
                    SUMO_ATTR_GCC_GAIN_SPACE
                    SUMO_ATTR_GC_GAIN_SPEED
                    SUMO_ATTR_GC_GAIN_SPACE
                    SUMO_ATTR_CA_GAIN_SPEED
                    SUMO_ATTR_CA_GAIN_SPACE
                */
                break;
            case SUMO_TAG_CF_CACC:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                myMinGapFactorRow->show();
                // show myLabelIncompleteAttribute
                myLabelIncompleteAttribute->show();
                /**
                The follow parameters has to be added:
                    SUMO_ATTR_SC_GAIN_CACC
                    SUMO_ATTR_GCC_GAIN_GAP_CACC
                    SUMO_ATTR_GCC_GAIN_GAP_DOT_CACC
                    SUMO_ATTR_GC_GAIN_GAP_CACC
                    SUMO_ATTR_GC_GAIN_GAP_DOT_CACC
                    SUMO_ATTR_CA_GAIN_GAP_CACC
                    SUMO_ATTR_CA_GAIN_GAP_DOT_CACC
                    SUMO_ATTR_GCC_GAIN_SPEED
                    SUMO_ATTR_GCC_GAIN_SPACE
                    SUMO_ATTR_GC_GAIN_SPEED
                    SUMO_ATTR_GC_GAIN_SPACE
                    SUMO_ATTR_CA_GAIN_SPEED
                    SUMO_ATTR_CA_GAIN_SPACE
                */
                break;
            case SUMO_TAG_CF_CC:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                // show myLabelIncompleteAttribute
                myLabelIncompleteAttribute->show();
                /**
                The follow parameters has to be added:
                    SUMO_ATTR_CF_CC_C1
                    SUMO_ATTR_CF_CC_CCDECEL
                    SUMO_ATTR_CF_CC_CONSTSPACING
                    SUMO_ATTR_CF_CC_KP
                    SUMO_ATTR_CF_CC_LAMBDA
                    SUMO_ATTR_CF_CC_OMEGAN
                    SUMO_ATTR_CF_CC_TAU
                    SUMO_ATTR_CF_CC_XI
                    SUMO_ATTR_CF_CC_LANES_COUNT
                    SUMO_ATTR_CF_CC_CCACCEL
                    SUMO_ATTR_CF_CC_PLOEG_KP
                    SUMO_ATTR_CF_CC_PLOEG_KD
                    SUMO_ATTR_CF_CC_PLOEG_H
                    SUMO_ATTR_CF_CC_FLATBED_KA
                    SUMO_ATTR_CF_CC_FLATBED_KV
                    SUMO_ATTR_CF_CC_FLATBED_KP
                    SUMO_ATTR_CF_CC_FLATBED_D
                    SUMO_ATTR_CF_CC_FLATBED_H
                */
                break;
            default:
                break;
        }
    }
    myVerticalFrameRows->recalc();
    update();
}


void 
GNEVehicleTypeDialog::CarFollowingModelParameters::updateValues() {
    //set values of myEditedDemandElement into fields
    if (myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL).empty()) {
        myComboBoxCarFollowModel->setCurrentItem(0);
    } else {
        myComboBoxCarFollowModel->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL).c_str());
    }
    // update value in all Rows
    for (const auto &i : myRows) {
        i->updateValue();
    }
}


long 
GNEVehicleTypeDialog::CarFollowingModelParameters::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myVehicleTypeDialog->myVehicleTypeValid = true;
    myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_NOTHING;
     // set color of myTextFieldCarFollowModel, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_CAR_FOLLOW_MODEL, myComboBoxCarFollowModel->getText().text())) {
        myComboBoxCarFollowModel->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL, myComboBoxCarFollowModel->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        myComboBoxCarFollowModel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_CAR_FOLLOW_MODEL;
    }
    // set variable in all Rows
    for (const auto &i : myRows) {
        i->setVariable();
    }
    // refresh fields
    refreshCFMFields();
    return true;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog - public methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::GNEVehicleTypeDialog(GNEDemandElement* editedVehicleType, bool updatingElement) :
    GNEDemandElementDialog(editedVehicleType, updatingElement, 1022, 350),
    myVehicleTypeValid(true),
    myInvalidAttr(SUMO_ATTR_NOTHING) {

    // change default header
    changeDemandElementDialogHeader(updatingElement ? "Edit " + myEditedDemandElement->getTagStr() + " of " : "Create " + myEditedDemandElement->getTagStr());
    
    // Create auxiliar frames for values
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);

    myVTypeCommonAtributes = new VTypeCommonAtributes(this, columns);

    myCarFollowingModelParameters = new CarFollowingModelParameters(this, columns);

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


void
GNEVehicleTypeDialog::updateVehicleTypeValues() {
    // update values of Vehicle Type common attributes
    myVTypeCommonAtributes->updateValues();
    // update values of Car Following Model Parameters
    myCarFollowingModelParameters->updateValues();
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog - private methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelRow::CarFollowingModelRow(CarFollowingModelParameters *carFollowingModelParametersParent, FXVerticalFrame* verticalFrame, SumoXMLAttr attr) :
    FXHorizontalFrame(verticalFrame, GUIDesignAuxiliarHorizontalFrame),
    myCarFollowingModelParametersParent(carFollowingModelParametersParent),
    myAttr(attr) {
    myLabel = new FXLabel(this, toString(attr).c_str(), nullptr, GUIDesignLabelAttribute150);
    textField = new FXTextField(this, GUIDesignTextFieldNCol, carFollowingModelParametersParent, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth180Real);
}


void 
GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelRow::setVariable() {
    // set color of textField, depending if current value is valid or not
    if (myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, textField->getText().text())) {
        textField->setTextColor(FXRGB(0, 0, 0));
        myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, textField->getText().text(), 
            myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->getViewNet()->getUndoList());
    } else {
        textField->setTextColor(FXRGB(255, 0, 0));
        // mark VType as invalid
        myCarFollowingModelParametersParent->myVehicleTypeDialog->myVehicleTypeValid = false;
        myCarFollowingModelParametersParent->myVehicleTypeDialog->myInvalidAttr = myAttr;
    }
}


void 
GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelRow::updateValue() {
    // set text of myTextField using current value of VType
    textField->setText(myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
}

/****************************************************************************/
