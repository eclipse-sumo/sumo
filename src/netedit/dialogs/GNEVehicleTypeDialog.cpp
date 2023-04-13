/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEVehicleTypeDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Dialog for edit vehicleTypes
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEVehicleTypeDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVehicleTypeDialog::VTypeAttributes) VTypeAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,              GNEVehicleTypeDialog::VTypeAttributes::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,       GNEVehicleTypeDialog::VTypeAttributes::onCmdOpenAttributeDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG,     GNEVehicleTypeDialog::VTypeAttributes::onCmdOpenParametersEditor)
};

FXDEFMAP(GNEVehicleTypeDialog::CarFollowingModelParameters) CarFollowingModelParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEVehicleTypeDialog::CarFollowingModelParameters::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNEVehicleTypeDialog::VTypeAttributes,              FXVerticalFrame,    VTypeAttributesMap,             ARRAYNUMBER(VTypeAttributesMap))
FXIMPLEMENT(GNEVehicleTypeDialog::CarFollowingModelParameters,  FXGroupBox,         CarFollowingModelParametersMap, ARRAYNUMBER(CarFollowingModelParametersMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VClassRow - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeAttributes::VClassRow::VClassRow(VTypeAttributes* VTypeAttributesParent, FXVerticalFrame* column) :
    FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame),
    myVTypeAttributesParent(VTypeAttributesParent) {
    // create two auxiliary frames
    FXVerticalFrame* verticalFrameLabelAndComboBox = new FXVerticalFrame(this, GUIDesignAuxiliarVerticalFrame);
    // create FXComboBox for VClass
    new FXLabel(verticalFrameLabelAndComboBox, toString(SUMO_ATTR_VCLASS).c_str(), nullptr, GUIDesignLabelThickedFixed(150));
    myComboBoxVClass = new MFXComboBoxIcon(verticalFrameLabelAndComboBox, GUIDesignComboBoxNCol, true,
                                           VTypeAttributesParent, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myComboBoxVClassLabelImage = new FXLabel(this, "", nullptr, GUIDesignLabelTickedIcon180x46);
    myComboBoxVClassLabelImage->setBackColor(FXRGBA(255, 255, 255, 255));
    // fill combo Box with all allowed VClass for the current edited VType
    for (const auto& vClass : myVTypeAttributesParent->myVehicleTypeDialog->getEditedDemandElement()->getTagProperty().getAttributeProperties(SUMO_ATTR_VCLASS).getDiscreteValues()) {
        myComboBoxVClass->appendIconItem(vClass.c_str(), VClassIcons::getVClassIcon(SumoVehicleClassStrings.get(vClass)));
    }
    // only show as maximum 10 VClasses
    if (myComboBoxVClass->getNumItems() > 10) {
        myComboBoxVClass->setNumVisible(10);
    } else {
        myComboBoxVClass->setNumVisible(myComboBoxVClass->getNumItems());
    }
}


SUMOVehicleClass
GNEVehicleTypeDialog::VTypeAttributes::VClassRow::setVariable() {
    // set color of myComboBoxVClass, depending if current value is valid or not
    myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
    if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text())) {
        myComboBoxVClass->setTextColor(FXRGB(0, 0, 0));
        // check if  VType has to be updated
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS) != myComboBoxVClass->getText().text()) {
            // update VClass in VType
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_VCLASS, myComboBoxVClass->getText().text(),
                    myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            // update label image
            setVClassLabelImage();
            // obtain default vType parameters
            SUMOVTypeParameter::VClassDefaultValues defaultVTypeParameters(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getVClass());
            // check if mutable rows need to be updated
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_LENGTH)) {
                myVTypeAttributesParent->myLength->updateValue(toString(defaultVTypeParameters.length));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_MINGAP)) {
                myVTypeAttributesParent->myMinGap->updateValue(toString(defaultVTypeParameters.minGap));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_MAXSPEED)) {
                myVTypeAttributesParent->myMaxSpeed->updateValue(toString(defaultVTypeParameters.maxSpeed));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_DESIRED_MAXSPEED)) {
                myVTypeAttributesParent->myDesiredMaxSpeed->updateValue(toString(defaultVTypeParameters.desiredMaxSpeed));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_SPEEDFACTOR)) {
                myVTypeAttributesParent->mySpeedFactor->updateValue(toString(defaultVTypeParameters.speedFactor.getParameter()[0]));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_EMISSIONCLASS)) {
                myVTypeAttributesParent->myEmissionClass->updateValue(toString(defaultVTypeParameters.emissionClass));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_WIDTH)) {
                myVTypeAttributesParent->myWidth->updateValue(toString(defaultVTypeParameters.width));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_HEIGHT)) {
                myVTypeAttributesParent->myHeight->updateValue(toString(defaultVTypeParameters.height));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_OSGFILE)) {
                myVTypeAttributesParent->myOSGFile->updateValue(toString(defaultVTypeParameters.osgFile));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_PERSON_CAPACITY)) {
                myVTypeAttributesParent->myPersonCapacity->updateValue(toString(defaultVTypeParameters.personCapacity));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_CONTAINER_CAPACITY)) {
                myVTypeAttributesParent->myContainerCapacity->updateValue(toString(defaultVTypeParameters.containerCapacity));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_CARRIAGE_LENGTH)) {
                myVTypeAttributesParent->myCarriageLength->updateValue(toString(defaultVTypeParameters.containerCapacity));
            }
            if (!myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isAttributeEnabled(SUMO_ATTR_LOCOMOTIVE_LENGTH)) {
                myVTypeAttributesParent->myLocomotiveLength->updateValue(toString(defaultVTypeParameters.containerCapacity));
            }
            // update GUIShape
            if (myComboBoxVClass->getText().empty()) {
                myVTypeAttributesParent->myVShapeRow->updateValue(SVC_PASSENGER);
            } else {
                myVTypeAttributesParent->myVShapeRow->updateValue(SumoVehicleClassStrings.get(myComboBoxVClass->getText().text()));
            }
        }
    } else {
        myComboBoxVClass->setTextColor(FXRGB(255, 0, 0));
        myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
        myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_VCLASS;
    }
    return myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getVClass();
}


SUMOVehicleClass
GNEVehicleTypeDialog::VTypeAttributes::VClassRow::updateValue() {
    const auto vClass = myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS);
    int index = 0;
    for (int i = 0; i < myComboBoxVClass->getNumItems(); i++) {
        if (myComboBoxVClass->getItem(i).text() == vClass) {
            index = i;
        }
    }
    myComboBoxVClass->setCurrentItem(index);
    setVClassLabelImage();
    return myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getVClass();
}


void
GNEVehicleTypeDialog::VTypeAttributes::VClassRow::setVClassLabelImage() {
    // by default vClass is passenger
    if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_VCLASS).empty()) {
        myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_PASSENGER));
    } else {
        // set Icon in label depending of current VClass
        switch (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getVClass()) {
            case SVC_PRIVATE:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_PRIVATE));
                break;
            case SVC_EMERGENCY:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_EMERGENCY));
                break;
            case SVC_AUTHORITY:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_AUTHORITY));
                break;
            case SVC_ARMY:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_ARMY));
                break;
            case SVC_VIP:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_VIP));
                break;
            case SVC_PASSENGER:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_PASSENGER));
                break;
            case SVC_HOV:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_HOV));
                break;
            case SVC_TAXI:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_TAXI));
                break;
            case SVC_BUS:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_BUS));
                break;
            case SVC_COACH:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_COACH));
                break;
            case SVC_DELIVERY:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_DELIVERY));
                break;
            case SVC_TRUCK:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_TRUCK));
                break;
            case SVC_TRAILER:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_TRAILER));
                break;
            case SVC_TRAM:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_TRAM));
                break;
            case SVC_RAIL_URBAN:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_RAIL_URBAN));
                break;
            case SVC_RAIL:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_RAIL));
                break;
            case SVC_RAIL_ELECTRIC:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_RAIL_ELECTRIC));
                break;
            case SVC_RAIL_FAST:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_RAIL_FAST));
                break;
            case SVC_MOTORCYCLE:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_MOTORCYCLE));
                break;
            case SVC_MOPED:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_MOPED));
                break;
            case SVC_BICYCLE:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_BICYCLE));
                break;
            case SVC_PEDESTRIAN:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_PEDESTRIAN));
                break;
            case SVC_E_VEHICLE:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_EVEHICLE));
                break;
            case SVC_SHIP:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_SHIP));
                break;
            case SVC_CUSTOM1:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_CUSTOM1));
                break;
            case SVC_CUSTOM2:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_CUSTOM2));
                break;
            default:
                myComboBoxVClassLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_IGNORING));
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VShapeRow - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeAttributes::VShapeRow::VShapeRow(VTypeAttributes* VTypeAttributesParent, FXVerticalFrame* column) :
    FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame),
    myVTypeAttributesParent(VTypeAttributesParent) {
    // create two auxiliar frames
    FXVerticalFrame* verticalFrameLabelAndComboBox = new FXVerticalFrame(this, GUIDesignAuxiliarVerticalFrame);
    // create combo for vehicle shapes
    new FXLabel(verticalFrameLabelAndComboBox, toString(SUMO_ATTR_GUISHAPE).c_str(), nullptr, GUIDesignLabelThickedFixed(150));
    myComboBoxShape = new MFXComboBoxIcon(verticalFrameLabelAndComboBox, GUIDesignComboBoxNCol, false,
                                          VTypeAttributesParent, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myComboBoxShapeLabelImage = new FXLabel(this, "", nullptr, GUIDesignLabelTickedIcon180x46);
    myComboBoxShapeLabelImage->setBackColor(FXRGBA(255, 255, 255, 255));
    // fill combo Box with all vehicle shapes
    std::vector<std::string> VShapeStrings = SumoVehicleShapeStrings.getStrings();
    for (const auto& VShapeString : VShapeStrings) {
        if (VShapeString != SumoVehicleShapeStrings.getString(SUMOVehicleShape::UNKNOWN)) {
            myComboBoxShape->appendIconItem(VShapeString.c_str(), nullptr);
        }
    }
    // only show 10 Shapes
    myComboBoxShape->setNumVisible(10);
}


void
GNEVehicleTypeDialog::VTypeAttributes::VShapeRow::setVariable() {
    // set color of myComboBoxShape, depending if current value is valid or not
    if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text())) {
        myComboBoxShape->setTextColor(FXRGB(0, 0, 0));
        myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text(),
                myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
        setVShapeLabelImage();
    } else {
        myComboBoxShape->setTextColor(FXRGB(255, 0, 0));
        myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
        myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_GUISHAPE;
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VShapeRow::updateValues() {
    myComboBoxShape->setText(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_GUISHAPE).c_str());
    setVShapeLabelImage();
}


void
GNEVehicleTypeDialog::VTypeAttributes::VShapeRow::updateValue(SUMOVehicleClass vClass) {
    // create new VClassDefaultValues using the new VClass
    SUMOVTypeParameter::VClassDefaultValues newVClass(vClass);
    myComboBoxShape->setText(SumoVehicleShapeStrings.getString(newVClass.shape).c_str());
    myComboBoxShape->setTextColor(FXRGB(0, 0, 0));
    myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_GUISHAPE, myComboBoxShape->getText().text(),
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
    setVShapeLabelImage();
}


void
GNEVehicleTypeDialog::VTypeAttributes::VShapeRow::setVShapeLabelImage() {
    // set Icon in label depending of current VClass
    switch (getVehicleShapeID(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_GUISHAPE))) {
        case SUMOVehicleShape::UNKNOWN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_UNKNOWN));
            break;
        case SUMOVehicleShape::PEDESTRIAN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_PEDESTRIAN));
            break;
        case SUMOVehicleShape::BICYCLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_BICYCLE));
            break;
        case SUMOVehicleShape::MOPED:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_MOPED));
            break;
        case SUMOVehicleShape::MOTORCYCLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_MOTORCYCLE));
            break;
        case SUMOVehicleShape::PASSENGER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_PASSENGER));
            break;
        case SUMOVehicleShape::PASSENGER_SEDAN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_PASSENGER_SEDAN));
            break;
        case SUMOVehicleShape::PASSENGER_HATCHBACK:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_PASSENGER_HATCHBACK));
            break;
        case SUMOVehicleShape::PASSENGER_WAGON:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_PASSENGER_WAGON));
            break;
        case SUMOVehicleShape::PASSENGER_VAN:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_PASSENGER_VAN));
            break;
        case SUMOVehicleShape::TAXI:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_TAXI));
            break;
        case SUMOVehicleShape::DELIVERY:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_DELIVERY));
            break;
        case SUMOVehicleShape::TRUCK:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_TRUCK));
            break;
        case SUMOVehicleShape::TRUCK_SEMITRAILER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_TRUCK_SEMITRAILER));
            break;
        case SUMOVehicleShape::TRUCK_1TRAILER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_TRUCK_1TRAILER));
            break;
        case SUMOVehicleShape::BUS:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_BUS));
            break;
        case SUMOVehicleShape::BUS_COACH:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_BUS_COACH));
            break;
        case SUMOVehicleShape::BUS_FLEXIBLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_BUS_FLEXIBLE));
            break;
        case SUMOVehicleShape::BUS_TROLLEY:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_BUS_TROLLEY));
            break;
        case SUMOVehicleShape::RAIL:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_RAIL));
            break;
        case SUMOVehicleShape::RAIL_CAR:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_RAIL_CAR));
            break;
        case SUMOVehicleShape::RAIL_CARGO:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_RAIL_CARGO));
            break;
        case SUMOVehicleShape::E_VEHICLE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_E_VEHICLE));
            break;
        case SUMOVehicleShape::ANT:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_ANT));
            break;
        case SUMOVehicleShape::SHIP:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_SHIP));
            break;
        case SUMOVehicleShape::EMERGENCY:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_EMERGENCY));
            break;
        case SUMOVehicleShape::FIREBRIGADE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_FIREBRIGADE));
            break;
        case SUMOVehicleShape::POLICE:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_POLICE));
            break;
        case SUMOVehicleShape::RICKSHAW:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_RICKSHAW));
            break;
        case SUMOVehicleShape::SCOOTER:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_SCOOTER));
            break;
        case SUMOVehicleShape::AIRCRAFT:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VSHAPE_AIRCRAFT));
            break;
        default:
            myComboBoxShapeLabelImage->setIcon(GUIIconSubSys::getIcon(GUIIcon::VCLASS_IGNORING));
            break;
    }
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VTypeAttributes - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::VTypeAttributeRow(VTypeAttributes* VTypeAttributesParent, FXVerticalFrame* verticalFrame, const SumoXMLAttr attr, const RowAttrType rowAttrType, const  std::vector<std::string>& values) :
    FXHorizontalFrame(verticalFrame, GUIDesignAuxiliarHorizontalFrame),
    myVTypeAttributesParent(VTypeAttributesParent),
    myAttr(attr),
    myRowAttrType(rowAttrType),
    myButton(nullptr),
    myTextField(nullptr),
    myComboBox(nullptr) {
    // first check if we have to create a button or a label
    if ((rowAttrType == ROWTYPE_COLOR) || (rowAttrType == ROWTYPE_FILENAME)) {
        myButton = new FXButton(this, filterAttributeName(attr), nullptr, VTypeAttributesParent, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonRectangular150);
    } else if (rowAttrType == ROWTYPE_PARAMETERS) {
        myButton = new FXButton(this, TL("Edit parameters"), nullptr, VTypeAttributesParent, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButtonRectangular150);
    } else {
        new FXLabel(this, filterAttributeName(attr), nullptr, GUIDesignLabelThickedFixed(150));
    }
    // now check if we have to create a textfield or a ComboBox
    if ((rowAttrType == ROWTYPE_STRING) || (rowAttrType == ROWTYPE_COLOR) || (rowAttrType == ROWTYPE_FILENAME) || (rowAttrType == ROWTYPE_PARAMETERS)) {
        myTextField = new FXTextField(this, GUIDesignTextFieldNCol, VTypeAttributesParent, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixed(180));
    } else if (rowAttrType == ROWTYPE_COMBOBOX) {
        myComboBox = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, false, VTypeAttributesParent, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxWidth180);
        // fill combo Box with values
        for (const auto& value : values) {
            myComboBox->appendIconItem(value.c_str(), nullptr);
        }
        // set 10 visible elements as maximum
        if (myComboBox->getNumItems() < 10) {
            myComboBox->setNumVisible(myComboBox->getNumItems());
        } else {
            myComboBox->setNumVisible(10);
        }
    } else {
        throw ProcessError(TL("Invalid row type"));
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::setVariable() {
    if (myRowAttrType == ROWTYPE_COMBOBOX) {
        // set color of myComboBox, depending if current value is valid or not
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, myComboBox->getText().text())) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, myComboBox->getText().text(),
                    myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            // update value after setting it
            updateValue();
        } else {
            myComboBox->setTextColor(FXRGB(255, 0, 0));
            // mark VType as invalid
            myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
            myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = myAttr;
        }
    } else if (myRowAttrType == ROWTYPE_COLOR) {
        // set color of myTextFieldColor, depending if current value is valid or not
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_COLOR, myTextField->getText().text())) {
            // set color depending if is a default value
            if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getTagProperty().getDefaultValue(SUMO_ATTR_COLOR) != myTextField->getText().text()) {
                myTextField->setTextColor(FXRGB(0, 0, 0));
            } else {
                myTextField->setTextColor(FXRGB(195, 195, 195));
            }
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_COLOR, myTextField->getText().text(), myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
        } else {
            myTextField->setTextColor(FXRGB(255, 0, 0));
            myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
            myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_COLOR;
        }
    } else {
        // set color of textField, depending if current value is valid or not
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, myTextField->getText().text())) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, myTextField->getText().text(),
                    myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            // update value after setting it
            updateValue();
        } else {
            myTextField->setTextColor(FXRGB(255, 0, 0));
            // mark VType as invalid
            myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
            myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = myAttr;
        }
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::setVariable(const std::string& defaultValue) {
    if (myComboBox) {
        // set color of myComboBox, depending if current value is valid or not
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, myComboBox->getText().text())) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, myComboBox->getText().text(),
                    myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            // update value after setting it
            updateValue(defaultValue);
        } else {
            myComboBox->setTextColor(FXRGB(255, 0, 0));
            // mark VType as invalid
            myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
            myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = myAttr;
        }
    } else {
        // set color of textField, depending if current value is valid or not
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, myTextField->getText().text())) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, myTextField->getText().text(),
                    myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            // update value after setting it
            updateValue(defaultValue);
        } else {
            myTextField->setTextColor(FXRGB(255, 0, 0));
            // mark VType as invalid
            myVTypeAttributesParent->myVehicleTypeDialog->myVehicleTypeValid = false;
            myVTypeAttributesParent->myVehicleTypeDialog->myInvalidAttr = myAttr;
        }
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::updateValue() {
    if (myRowAttrType == ROWTYPE_COMBOBOX) {
        // set text of myComboBox using current value of VType
        myComboBox->setText(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
        // set color depending if is a default value
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getTagProperty().getDefaultValue(myAttr) != myComboBox->getText().text()) {
            myComboBox->setTextColor(FXRGB(0, 0, 0));
        } else {
            myComboBox->setTextColor(FXRGB(195, 195, 195));
        }
    } else if (myRowAttrType == ROWTYPE_COLOR) {
        // set field color
        myTextField->setText(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
        // set color depending if is a default value
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getTagProperty().getDefaultValue(myAttr) != myTextField->getText().text()) {
            myTextField->setTextColor(FXRGB(0, 0, 0));
        } else {
            myTextField->setTextColor(FXRGB(195, 195, 195));
        }
    } else if (myAttr == GNE_ATTR_PARAMETERS) {
        // get parameters
        const std::string& parametersStr = myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr);
        // set text of myTextField using current value of VType
        myTextField->setText(parametersStr.c_str());
        // set text color
        myTextField->setTextColor(FXRGB(0, 0, 0));
        // clear parameters
        myParameters.clear();
        // separate value in a vector of string using | as separator
        StringTokenizer parameters(parametersStr, "|", true);
        // iterate over all values
        while (parameters.hasNext()) {
            // obtain key and value and save it in myParameters
            const std::vector<std::string> keyValue = StringTokenizer(parameters.next(), "=", true).getVector();
            if (keyValue.size() == 2) {
                myParameters[keyValue.front()] = keyValue.back();
            }
        }
    } else {
        // set text of myTextField using current value of VType
        myTextField->setText(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
        // set color depending if is a default value
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getTagProperty().getDefaultValue(myAttr) != myTextField->getText().text()) {
            myTextField->setTextColor(FXRGB(0, 0, 0));
        } else {
            myTextField->setTextColor(FXRGB(195, 195, 195));
        }
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::updateValue(const std::string& defaultValue) {
    if (myComboBox) {
        // set text of myComboBox using current value of VType
        myComboBox->setText(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
        // set color depending if is a default value
        if (defaultValue != myComboBox->getText().text()) {
            myComboBox->setTextColor(FXRGB(0, 0, 0));
        } else {
            myComboBox->setTextColor(FXRGB(195, 195, 195));
        }
    } else {
        // set text of myTextField using current value of VType
        myTextField->setText(myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
        // set color depending if is a default value
        if (defaultValue != myTextField->getText().text()) {
            myTextField->setTextColor(FXRGB(0, 0, 0));
        } else {
            myTextField->setTextColor(FXRGB(195, 195, 195));
        }
    }
}


const FXButton*
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::getButton() const {
    return myButton;
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::openColorDialog() {
    // create FXColorDialog
    FXColorDialog colordialog(this, TL("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myTextField->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myTextField->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
        myTextField->setText(newValue.c_str());
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, newValue)) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, newValue, myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            // If previously value was incorrect, change font color to black
            myTextField->setTextColor(FXRGB(0, 0, 0));
            myTextField->killFocus();
        }
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::openImageFileDialog() {
    // get the new image file
    FXFileDialog opendialog(this, TL("Open Image"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::VTYPE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("All files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // update global current folder
        gCurrentFolder = opendialog.getDirectory();
        // get image path
        std::string imagePath = opendialog.getFilename().text();
        // check if image is valid
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, imagePath)) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, imagePath, myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            myTextField->setText(imagePath.c_str());
            // If previously value was incorrect, change font color to black
            myTextField->setTextColor(FXRGB(0, 0, 0));
            myTextField->killFocus();
        }
    }
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::openOSGFileDialog() {
    // get the new file name
    FXFileDialog opendialog(this, TL("Open OSG File"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::VTYPE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("OSG file (*.obj)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // update global current folder
        gCurrentFolder = opendialog.getDirectory();
        // get image path
        std::string imagePath = opendialog.getFilename().text();
        // check if image is valid
        if (myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, imagePath)) {
            myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, imagePath, myVTypeAttributesParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
            myTextField->setText(imagePath.c_str());
            // If previously value was incorrect, change font color to black
            myTextField->setTextColor(FXRGB(0, 0, 0));
            myTextField->killFocus();
        }
    }
}


std::string
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::getParametersStr() const {
    return myTextField->getText().text();
}


std::vector<std::pair<std::string, std::string> >
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::getParametersVectorStr() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : myParameters) {
        result.push_back(std::make_pair(parameter.first, parameter.second));
    }
    return result;
}


void
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::setParameters(const std::vector<std::pair<std::string, std::string> >& parameters) {
    // first clear parameters
    myParameters.clear();
    // declare result
    std::string result;
    // iterate over parameters
    for (const auto& parameter : parameters) {
        // Generate an string using the following structure: "key1=value1|key2=value2|...
        result += parameter.first + "=" + parameter.second + "|";
        // fill parameters
        myParameters[parameter.first] = parameter.second;
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    // set text field
    myTextField->setText(result.c_str());
}


FXString
GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributeRow::filterAttributeName(const SumoXMLAttr attr) const {
    switch (attr) {
        // JM
        case SUMO_ATTR_JM_CROSSING_GAP:
            return "crossingGap";
        case SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME:
            return "driveAfterYellowTime";
        case SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME:
            return "driveAfterRedTime";
        case SUMO_ATTR_JM_DRIVE_RED_SPEED:
            return "driveRedSpeed";
        case SUMO_ATTR_JM_IGNORE_KEEPCLEAR_TIME:
            return "ignoreKeepClearTime";
        case SUMO_ATTR_JM_IGNORE_FOE_SPEED:
            return "ignoreFoeSpeed";
        case SUMO_ATTR_JM_IGNORE_FOE_PROB:
            return "ignoreFoeProb";
        case SUMO_ATTR_JM_SIGMA_MINOR:
            return "sigmaMinor";
        case SUMO_ATTR_JM_TIMEGAP_MINOR:
            return "timegapMinor";
        // LCM
        case SUMO_ATTR_LCA_STRATEGIC_PARAM:
            return "strategic";
        case SUMO_ATTR_LCA_COOPERATIVE_PARAM:
            return "cooperative";
        case SUMO_ATTR_LCA_SPEEDGAIN_PARAM:
            return "speedGain";
        case SUMO_ATTR_LCA_KEEPRIGHT_PARAM:
            return "keepRight";
        case SUMO_ATTR_LCA_SUBLANE_PARAM:
            return "sublane";
        case SUMO_ATTR_LCA_OPPOSITE_PARAM:
            return "opposite";
        case SUMO_ATTR_LCA_PUSHY:
            return "pushy";
        case SUMO_ATTR_LCA_PUSHYGAP:
            return "pushyGap";
        case SUMO_ATTR_LCA_ASSERTIVE:
            return "assertive";
        case SUMO_ATTR_LCA_IMPATIENCE:
            return "impatience";
        case SUMO_ATTR_LCA_TIME_TO_IMPATIENCE:
            return "timeToImpatience";
        case SUMO_ATTR_LCA_ACCEL_LAT:
            return "accelLat";
        case SUMO_ATTR_LCA_LOOKAHEADLEFT:
            return "lookaheadLeft";
        case SUMO_ATTR_LCA_SPEEDGAINRIGHT:
            return "speedGainRight";
        case SUMO_ATTR_LCA_MAXSPEEDLATSTANDING:
            return "maxSpeedLatStanding";
        case SUMO_ATTR_LCA_MAXSPEEDLATFACTOR:
            return "maxSpeedLatFactor";
        case SUMO_ATTR_LCA_TURN_ALIGNMENT_DISTANCE:
            return "turnAlignDistance";
        case SUMO_ATTR_LCA_OVERTAKE_RIGHT:
            return "overtakeRight";
        case SUMO_ATTR_LCA_KEEPRIGHT_ACCEPTANCE_TIME:
            return "keepRightAcceptanceTime";
        case SUMO_ATTR_LCA_OVERTAKE_DELTASPEED_FACTOR:
            return "overtakeDeltaSpeedFactor";
        /* case SUMO_ATTR_LCA_EXPERIMENTAL1:
            return "experimental1";
        */
        default:
            return toString(attr).c_str();
    }
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VTypeAttributes - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::VTypeAttributes::VTypeAttributes(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column) :
    FXVerticalFrame(column, GUIDesignAuxiliarVerticalFrame),
    myVehicleTypeDialog(vehicleTypeDialog) {
    // declare two auxiliary horizontal frames
    FXHorizontalFrame* firstAuxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    FXVerticalFrame* firstAuxiliarVerticalFrame = new FXVerticalFrame(firstAuxiliarHorizontalFrame, GUIDesignAuxiliarVerticalFrame);
    // create attributes for common attributes
    FXGroupBox* commonAttributes = new FXGroupBox(firstAuxiliarVerticalFrame, "Vehicle Type attributes", GUIDesignGroupBoxFrame);
    // create horizontal frame for columns of attributes
    FXHorizontalFrame* columnsBasicVTypeAttributes = new FXHorizontalFrame(commonAttributes, GUIDesignAuxiliarHorizontalFrame);
    // build left attributes
    buildAttributesA(new FXVerticalFrame(columnsBasicVTypeAttributes, GUIDesignAuxiliarFrame));
    // build right attributes
    buildAttributesB(new FXVerticalFrame(columnsBasicVTypeAttributes, GUIDesignAuxiliarFrame));
    // create GroupBox for Junction Model Attributes
    FXGroupBox* JMAttributes = new FXGroupBox(firstAuxiliarVerticalFrame, "Junction Model attributes", GUIDesignGroupBoxFrame);
    // create horizontal frame for columns of Junction Model attributes
    FXHorizontalFrame* columnsJMVTypeAttributes = new FXHorizontalFrame(JMAttributes, GUIDesignAuxiliarHorizontalFrame);
    // build left attributes
    buildJunctionModelAttributesA(new FXVerticalFrame(columnsJMVTypeAttributes, GUIDesignAuxiliarFrame));
    // build right attributes
    buildJunctionModelAttributesB(new FXVerticalFrame(columnsJMVTypeAttributes, GUIDesignAuxiliarFrame));
    // create GroupBox for Junction Model Attributes
    FXGroupBox* LCMAttributes = new FXGroupBox(firstAuxiliarHorizontalFrame, "Lane Change Model attributes", GUIDesignGroupBoxFrame);
    // create Lane Change Model Attributes
    buildLaneChangeModelAttributes(new FXVerticalFrame(LCMAttributes, GUIDesignAuxiliarFrame));
}


void
GNEVehicleTypeDialog::VTypeAttributes::buildAttributesA(FXVerticalFrame* column) {
    // 01 Create VClassRow
    myVClassRow = new VClassRow(this, column);

    // 02 create FXTextField and Label for vehicleTypeID
    FXHorizontalFrame* row = new FXHorizontalFrame(column, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, toString(SUMO_ATTR_ID).c_str(), nullptr, GUIDesignLabelThickedFixed(150));
    myTextFieldVehicleTypeID = new FXTextField(row, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixed(180));

    // 03 create FXTextField and Button for Color
    myColor = new VTypeAttributeRow(this, column, SUMO_ATTR_COLOR, VTypeAttributeRow::RowAttrType::ROWTYPE_COLOR);

    // 04 create FXTextField and Label for Length
    myLength = new VTypeAttributeRow(this, column, SUMO_ATTR_LENGTH, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 05 create FXTextField and Label for MinGap
    myMinGap = new VTypeAttributeRow(this, column, SUMO_ATTR_MINGAP, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 06 create FXTextField and Label for MaxSpeed
    myMaxSpeed = new VTypeAttributeRow(this, column, SUMO_ATTR_MAXSPEED, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 07 create VTypeAttributeRow and Label for desired max speed
    myDesiredMaxSpeed = new VTypeAttributeRow(this, column, SUMO_ATTR_DESIRED_MAXSPEED, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 08 create FXTextField and Label for SpeedFactor
    mySpeedFactor = new VTypeAttributeRow(this, column, SUMO_ATTR_SPEEDFACTOR, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 09 create FXTextField and Label for EmissionClass
    myEmissionClass = new VTypeAttributeRow(this, column, SUMO_ATTR_EMISSIONCLASS, VTypeAttributeRow::RowAttrType::ROWTYPE_COMBOBOX, PollutantsInterface::getAllClassesStr());

    // 10 create FXTextField and Label for Width
    myWidth = new VTypeAttributeRow(this, column, SUMO_ATTR_WIDTH, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 11 create FXTextField and Label for Height
    myHeight = new VTypeAttributeRow(this, column, SUMO_ATTR_HEIGHT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 12 create FXTextField and Label for Filename
    myFilename = new VTypeAttributeRow(this, column, SUMO_ATTR_IMGFILE, VTypeAttributeRow::RowAttrType::ROWTYPE_FILENAME);

    // 13 create FXTextField and Label for Filename
    myOSGFile = new VTypeAttributeRow(this, column, SUMO_ATTR_OSGFILE, VTypeAttributeRow::RowAttrType::ROWTYPE_FILENAME);

    // 14 create VTypeAttributeRow and Label for LaneChangeModel
    myLaneChangeModel = new VTypeAttributeRow(this, column, SUMO_ATTR_LANE_CHANGE_MODEL, VTypeAttributeRow::RowAttrType::ROWTYPE_COMBOBOX, SUMOXMLDefinitions::LaneChangeModels.getStrings());
}


void
GNEVehicleTypeDialog::VTypeAttributes::buildAttributesB(FXVerticalFrame* column) {
    // 01 Create VShapeRow
    myVShapeRow = new VShapeRow(this, column);

    // 02 create VTypeAttributeRow and Label for Probability
    myProbability = new VTypeAttributeRow(this, column, SUMO_ATTR_PROB, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 03 create VTypeAttributeRow and Label for PersonCapacity
    myPersonCapacity = new VTypeAttributeRow(this, column, SUMO_ATTR_PERSON_CAPACITY, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 04 create VTypeAttributeRow and Label for ContainerCapacity
    myContainerCapacity = new VTypeAttributeRow(this, column, SUMO_ATTR_CONTAINER_CAPACITY, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 05 create VTypeAttributeRow and Label for BoardingDuration
    myBoardingDuration = new VTypeAttributeRow(this, column, SUMO_ATTR_BOARDING_DURATION, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 06 create VTypeAttributeRow and Label for LoadingDuration
    myLoadingDuration = new VTypeAttributeRow(this, column, SUMO_ATTR_LOADING_DURATION, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 07 create ComboBox and Label for LatAlignment
    myLatAlignment = new VTypeAttributeRow(this, column, SUMO_ATTR_LATALIGNMENT, VTypeAttributeRow::RowAttrType::ROWTYPE_COMBOBOX, SUMOVTypeParameter::getLatAlignmentStrings());

    // 08 create VTypeAttributeRow and Label for MinGapLat
    myMinGapLat = new VTypeAttributeRow(this, column, SUMO_ATTR_MINGAP_LAT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 09 create VTypeAttributeRow and Label for MaxSpeedLat
    myMaxSpeedLat = new VTypeAttributeRow(this, column, SUMO_ATTR_MAXSPEED_LAT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 10 create VTypeAttributeRow and Label for ActionStepLength
    myActionStepLength = new VTypeAttributeRow(this, column, SUMO_ATTR_ACTIONSTEPLENGTH, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 11 create FXTextField and Label for Carriage length
    myCarriageLength = new VTypeAttributeRow(this, column, SUMO_ATTR_CARRIAGE_LENGTH, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 12 create FXTextField and Label for Locomotive length
    myLocomotiveLength = new VTypeAttributeRow(this, column, SUMO_ATTR_LOCOMOTIVE_LENGTH, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 13 create FXTextField and Label for carriage GAP
    myCarriageGap = new VTypeAttributeRow(this, column, SUMO_ATTR_CARRIAGE_GAP, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 14 create FXTextField and Label for parameters
    myParameters = new VTypeAttributeRow(this, column, GNE_ATTR_PARAMETERS, VTypeAttributeRow::RowAttrType::ROWTYPE_PARAMETERS);
}


void
GNEVehicleTypeDialog::VTypeAttributes::buildJunctionModelAttributesA(FXVerticalFrame* column) {
    // 01 create VTypeAttributeRow and Label for JMCrossingGap
    myJMCrossingGap = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_CROSSING_GAP, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 02 create VTypeAttributeRow and Label for JMIgnoreKeepclearTime
    myJMIgnoreKeepclearTime = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_IGNORE_KEEPCLEAR_TIME, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 03 create VTypeAttributeRow and Label for JMDriveAfterYellowTime
    myJMDriveAfterYellowTime = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 04 create VTypeAttributeRow and Label for JMDriveAfterRedTime
    myJMDriveAfterRedTime = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 05 create VTypeAttributeRow and Label for JMDriveRedSpeed
    myJMDriveRedSpeed = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_DRIVE_RED_SPEED, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);
}


void
GNEVehicleTypeDialog::VTypeAttributes::buildJunctionModelAttributesB(FXVerticalFrame* column) {
    // 01 create VTypeAttributeRow and Label for JMIgnoreFoeProb
    myJMIgnoreFoeProb = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_IGNORE_FOE_PROB, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 02 create VTypeAttributeRow and Label for JMIgnoreFoeSpeed
    myJMIgnoreFoeSpeed = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_IGNORE_FOE_SPEED, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 03 create VTypeAttributeRow and Label for JMSigmaMinor
    myJMSigmaMinor = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_SIGMA_MINOR, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 04 create VTypeAttributeRow and Label for JMTimeGapMinor
    myJMTimeGapMinor = new VTypeAttributeRow(this, column, SUMO_ATTR_JM_TIMEGAP_MINOR, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 05 create VTypeAttributeRow and Label for Impatience
    myJMImpatience = new VTypeAttributeRow(this, column, SUMO_ATTR_IMPATIENCE, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);
}


void
GNEVehicleTypeDialog::VTypeAttributes::buildLaneChangeModelAttributes(FXVerticalFrame* column) {
    // 01 create VTypeAttributeRow and Label for strategic param
    myLCAStrategicParam = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_STRATEGIC_PARAM, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 02 create VTypeAttributeRow and Label for cooperative param
    myLCACooperativeParam = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_COOPERATIVE_PARAM, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 03 create VTypeAttributeRow and Label for speed gain param
    myLCASpeedgainParam = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_SPEEDGAIN_PARAM, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 04 create VTypeAttributeRow and Label for keepright param
    myLCAKeeprightParam = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_KEEPRIGHT_PARAM, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 05 create VTypeAttributeRow and Label for sublane param
    myLCASublaneParam = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_SUBLANE_PARAM, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 06 create VTypeAttributeRow and Label for opposite param
    myLCAOppositeParam = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_OPPOSITE_PARAM, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 07 create VTypeAttributeRow and Label for pushy
    myLCAPushy = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_PUSHY, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 08 create VTypeAttributeRow and Label for pushy gap
    myLCAPushygap = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_PUSHYGAP, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 09 create VTypeAttributeRow and Label for assertive
    myLCAAssertive = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_ASSERTIVE, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 10 create VTypeAttributeRow and Label for impatience
    myLCAImpatience = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_IMPATIENCE, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 11 create VTypeAttributeRow and Label for time to impatience
    myLCATimeToImpatience = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_TIME_TO_IMPATIENCE, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 12 create VTypeAttributeRow and Label for accel lat
    myLCAAccelLat = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_ACCEL_LAT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 13 create VTypeAttributeRow and Label for look ahead lefth
    myLCALookAheadLeft = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_LOOKAHEADLEFT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 14 create VTypeAttributeRow and Label for speed gain right
    myLCASpeedGainRight = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_SPEEDGAINRIGHT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 15 create VTypeAttributeRow and Label for max speed lat standing
    myLCAMaxSpeedLatStanding = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_MAXSPEEDLATSTANDING, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 16 create VTypeAttributeRow and Label for max speed lat factor
    myLCAMaxSpeedLatFactor = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_MAXSPEEDLATFACTOR, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 17 create VTypeAttributeRow and Label for turn alignment distance
    myLCATurnAlignmentDistance = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_TURN_ALIGNMENT_DISTANCE, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 18 create VTypeAttributeRow and Label for overtake right
    myLCAOvertakeRight = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_OVERTAKE_RIGHT, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 19 create VTypeAttributeRow and Label for keep right acceptance time
    myLCAKeepRightAcceptanceTime = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_KEEPRIGHT_ACCEPTANCE_TIME, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 20 create VTypeAttributeRow and Label for overtake deltaspeed factor
    myLCAOvertakeDeltaSpeedFactor = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_OVERTAKE_DELTASPEED_FACTOR, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING);

    // 21 create VTypeAttributeRow and Label for experimental
    /* myLCAExperimental = new VTypeAttributeRow(this, column, SUMO_ATTR_LCA_EXPERIMENTAL1, VTypeAttributeRow::RowAttrType::ROWTYPE_STRING); */
}


void
GNEVehicleTypeDialog::VTypeAttributes::updateValues() {
    //set values of myEditedDemandElement into fields
    myTextFieldVehicleTypeID->setText(myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_ID).c_str());
    // set variables of special rows VClass and VShape
    SUMOVTypeParameter::VClassDefaultValues defaultVTypeParameters(myVClassRow->updateValue());
    myVShapeRow->updateValues();
    // update rows
    myLaneChangeModel->updateValue();
    myLatAlignment->updateValue(toString(defaultVTypeParameters.latAlignmentProcedure));
    myColor->updateValue();
    myLength->updateValue(toString(defaultVTypeParameters.length));
    myMinGap->updateValue(toString(defaultVTypeParameters.minGap));
    myMaxSpeed->updateValue(toString(defaultVTypeParameters.maxSpeed));
    myDesiredMaxSpeed->updateValue(toString(defaultVTypeParameters.desiredMaxSpeed));
    mySpeedFactor->updateValue(toString(defaultVTypeParameters.speedFactor.getParameter()[0]));
    myEmissionClass->updateValue(toString(defaultVTypeParameters.emissionClass));
    myWidth->updateValue(toString(defaultVTypeParameters.width));
    myHeight->updateValue(toString(defaultVTypeParameters.height));
    myFilename->updateValue();
    myOSGFile->updateValue(toString(defaultVTypeParameters.osgFile));
    myPersonCapacity->updateValue(toString(defaultVTypeParameters.personCapacity));
    myContainerCapacity->updateValue(toString(defaultVTypeParameters.containerCapacity));
    myCarriageLength->updateValue(toString(defaultVTypeParameters.carriageLength));
    myLocomotiveLength->updateValue(toString(defaultVTypeParameters.locomotiveLength));
    myBoardingDuration->updateValue();
    myLoadingDuration->updateValue();
    myMinGapLat->updateValue();
    myMaxSpeedLat->updateValue();
    myActionStepLength->updateValue();
    myProbability->updateValue();
    myCarriageGap->updateValue();
    // JM Parameters
    myJMCrossingGap->updateValue();
    myJMIgnoreKeepclearTime->updateValue();
    myJMDriveAfterYellowTime->updateValue();
    myJMDriveAfterRedTime->updateValue();
    myJMDriveRedSpeed->updateValue();
    myJMIgnoreFoeProb->updateValue();
    myJMIgnoreFoeSpeed->updateValue();
    myJMSigmaMinor->updateValue();
    myJMTimeGapMinor->updateValue();
    myJMImpatience->updateValue();
    // LCM Attributes
    myLCAStrategicParam->updateValue();
    myLCACooperativeParam->updateValue();
    myLCASpeedgainParam->updateValue();
    myLCAKeeprightParam->updateValue();
    myLCASublaneParam->updateValue();
    myLCAOppositeParam->updateValue();
    myLCAPushy->updateValue();
    myLCAPushygap->updateValue();
    myLCAAssertive->updateValue();
    myLCAImpatience->updateValue();
    myLCATimeToImpatience->updateValue();
    myLCAAccelLat->updateValue();
    myLCALookAheadLeft->updateValue();
    myLCASpeedGainRight->updateValue();
    myLCAMaxSpeedLatStanding->updateValue();
    myLCAMaxSpeedLatFactor->updateValue();
    myLCATurnAlignmentDistance->updateValue();
    myLCAOvertakeRight->updateValue();
    myLCAKeepRightAcceptanceTime->updateValue();
    myLCAOvertakeDeltaSpeedFactor->updateValue();
    /* myLCAExperimental->updateValue(); */
    // parameters
    myParameters->updateValue();
}


long
GNEVehicleTypeDialog::VTypeAttributes::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myVehicleTypeDialog->myVehicleTypeValid = true;
    myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_NOTHING;
    // set color of myTextFieldVehicleTypeID, depending if current value is valid or not
    if (myVehicleTypeDialog->myEditedDemandElement->isValid(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text())) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
    } else if (myVehicleTypeDialog->myEditedDemandElement->getAttribute(SUMO_ATTR_ID) == myTextFieldVehicleTypeID->getText().text()) {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(0, 0, 0));
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_ID, myTextFieldVehicleTypeID->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
    } else {
        myTextFieldVehicleTypeID->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_ID;
    }
    // set variables of special rows VClass and VShape
    SUMOVTypeParameter::VClassDefaultValues defaultVTypeParameters(myVClassRow->setVariable());
    // set variables of special rows VShape
    myVShapeRow->setVariable();
    // set attributes in rest rows
    myColor->setVariable();
    myLength->setVariable(toString(defaultVTypeParameters.length));
    myMinGap->setVariable(toString(defaultVTypeParameters.minGap));
    myMaxSpeed->setVariable(toString(defaultVTypeParameters.maxSpeed));
    myDesiredMaxSpeed->setVariable(toString(defaultVTypeParameters.desiredMaxSpeed));
    mySpeedFactor->setVariable(toString(defaultVTypeParameters.speedFactor.getParameter()[0]));
    myEmissionClass->setVariable(toString(defaultVTypeParameters.emissionClass));
    myWidth->setVariable(toString(defaultVTypeParameters.width));
    myHeight->setVariable(toString(defaultVTypeParameters.height));
    myOSGFile->setVariable(toString(defaultVTypeParameters.osgFile));
    // set attributes in rows
    myLaneChangeModel->setVariable();
    myLatAlignment->setVariable(toString(defaultVTypeParameters.latAlignmentProcedure));
    myLength->setVariable(toString(defaultVTypeParameters.length));
    myMinGap->setVariable(toString(defaultVTypeParameters.minGap));
    myMaxSpeed->setVariable(toString(defaultVTypeParameters.maxSpeed));
    mySpeedFactor->setVariable(toString(defaultVTypeParameters.speedFactor.getParameter()[0]));
    myEmissionClass->setVariable(toString(defaultVTypeParameters.emissionClass));
    myWidth->setVariable(toString(defaultVTypeParameters.width));
    myHeight->setVariable(toString(defaultVTypeParameters.height));
    myFilename->setVariable();
    myOSGFile->setVariable(toString(defaultVTypeParameters.osgFile));
    myPersonCapacity->setVariable(toString(defaultVTypeParameters.personCapacity));
    myContainerCapacity->setVariable(toString(defaultVTypeParameters.containerCapacity));
    myCarriageLength->setVariable(toString(defaultVTypeParameters.carriageLength));
    myLocomotiveLength->setVariable(toString(defaultVTypeParameters.locomotiveLength));
    myBoardingDuration->setVariable();
    myLoadingDuration->setVariable();
    myMinGapLat->setVariable();
    myMaxSpeedLat->setVariable();
    myActionStepLength->setVariable();
    myProbability->setVariable();
    myCarriageGap->setVariable();
    // JM Variables
    myJMCrossingGap->setVariable();
    myJMIgnoreKeepclearTime->setVariable();
    myJMDriveAfterYellowTime->setVariable();
    myJMDriveAfterRedTime->setVariable();
    myJMDriveRedSpeed->setVariable();
    myJMIgnoreFoeProb->setVariable();
    myJMIgnoreFoeSpeed->setVariable();
    myJMSigmaMinor->setVariable();
    myJMTimeGapMinor->setVariable();
    myJMImpatience->setVariable();
    // LCM Attributes
    myLCAStrategicParam->setVariable();
    myLCACooperativeParam->setVariable();
    myLCASpeedgainParam->setVariable();
    myLCAKeeprightParam->setVariable();
    myLCASublaneParam->setVariable();
    myLCAOppositeParam->setVariable();
    myLCAPushy->setVariable();
    myLCAPushygap->setVariable();
    myLCAAssertive->setVariable();
    myLCAImpatience->setVariable();
    myLCATimeToImpatience->setVariable();
    myLCAAccelLat->setVariable();
    myLCALookAheadLeft->setVariable();
    myLCASpeedGainRight->setVariable();
    myLCAMaxSpeedLatStanding->setVariable();
    myLCAMaxSpeedLatFactor->setVariable();
    myLCATurnAlignmentDistance->setVariable();
    myLCAOvertakeRight->setVariable();
    myLCAKeepRightAcceptanceTime->setVariable();
    myLCAOvertakeDeltaSpeedFactor->setVariable();
    /* myLCAExperimental->setVariable(); */
    myParameters->setVariable();
    return true;
}


long
GNEVehicleTypeDialog::VTypeAttributes::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    // check what dialog has to be opened
    if (obj == myColor->getButton()) {
        myColor->openColorDialog();
    } else if (obj == myFilename->getButton()) {
        myFilename->openImageFileDialog();
    } else if (obj == myOSGFile->getButton()) {
        myFilename->openOSGFileDialog();
    }
    return 1;
}


long
GNEVehicleTypeDialog::VTypeAttributes::onCmdOpenParametersEditor(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open parameters dialog");
    // edit parameters using dialog
    if (GNESingleParametersDialog(myParameters, myVehicleTypeDialog->getEditedDemandElement()->getNet()->getViewNet()).execute()) {
        // write debug information
        WRITE_DEBUG("Close parameters dialog");
        // set values edited in Parameter dialog in Edited AC
        myVehicleTypeDialog->getEditedDemandElement()->setAttribute(GNE_ATTR_PARAMETERS, myParameters->getParametersStr(), myVehicleTypeDialog->getEditedDemandElement()->getNet()->getViewNet()->getUndoList());
    } else {
        // write debug information
        WRITE_DEBUG("Cancel parameters dialog");
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog::VShapeRow - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelParameters(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column) :
    FXGroupBox(column, "Car Following Model attributes", GUIDesignGroupBoxFrame),
    myVehicleTypeDialog(vehicleTypeDialog) {

    // create vertical frame for rows
    myVerticalFrameRows = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);

    // declare combo box
    FXHorizontalFrame* row = new FXHorizontalFrame(myVerticalFrameRows, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(row, "Algorithm", nullptr, GUIDesignLabelThickedFixed(150));
    myComboBoxCarFollowModel = new MFXComboBoxIcon(row, GUIDesignComboBoxNCol, false, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);

    // fill combo Box with all Car following models
    std::vector<std::string> CFModels = SUMOXMLDefinitions::CarFollowModels.getStrings();
    for (const auto& CFModel : CFModels) {
        myComboBoxCarFollowModel->appendIconItem(CFModel.c_str(), nullptr);
    }
    myComboBoxCarFollowModel->setNumVisible(myComboBoxCarFollowModel->getNumItems());

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

    // 19 create FX and Label for trainType (allow strings)
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

    // 24 create FX and Label for W99 CC 01
    myW99CC1 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC1);
    myRows.push_back(myW99CC1);

    // 25 create FX and Label for W99 CC 02
    myW99CC2 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC2);
    myRows.push_back(myW99CC2);

    // 26 create FX and Label for W99 CC 03
    myW99CC3 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC3);
    myRows.push_back(myW99CC3);

    // 27 create FX and Label for W99 CC 04
    myW99CC4 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC4);
    myRows.push_back(myW99CC4);

    // 28 create FX and Label for W99 CC 05
    myW99CC5 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC5);
    myRows.push_back(myW99CC5);

    // 29 create FX and Label for W99 CC 06
    myW99CC6 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC6);
    myRows.push_back(myW99CC6);

    // 30 create FX and Label for W99 CC 07
    myW99CC7 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC7);
    myRows.push_back(myW99CC7);

    // 31 create FX and Label for W99 CC 08
    myW99CC8 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC8);
    myRows.push_back(myW99CC8);

    // 32 create FX and Label for W99 CC 09
    myW99CC9 = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_W99_CC9);
    myRows.push_back(myW99CC9);

    // X1 create FX and Label for Look ahead/preview Time
    myTpreviewRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_T_LOOK_AHEAD);
    myRows.push_back(myTpreviewRow);

    // X2 create FX and Label for Reaction Time
    myTreactionRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_T_REACTION);
    myRows.push_back(myTreactionRow);

    // X3 create FX and Label for Wiener Process Driving Error
    myTPersDriveRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_T_PERSISTENCE_DRIVE);
    myRows.push_back(myTPersDriveRow);

    // X4 create FX and Label for Wiener Process Estimation Error
    myTPersEstimateRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_T_PERSISTENCE_ESTIMATE);
    myRows.push_back(myTPersEstimateRow);

    // X5 create FX and Label for Coolness parameter
    myCcoolnessRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_C_COOLNESS);
    myRows.push_back(myCcoolnessRow);

    // X6 create FX and Label for leader speed estimation error
    mySigmaleaderRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_SIG_LEADER);
    myRows.push_back(mySigmaleaderRow);

    // X7 create FX and Label for Gap estimation error
    mySigmagapRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_SIG_GAP);
    myRows.push_back(mySigmagapRow);

    // X8 create FX and Label for Driving Error
    mySigmaerrorRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_SIG_ERROR);
    myRows.push_back(mySigmaerrorRow);

    // X9 create FX and Label for max jerk
    myJerkmaxRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_JERK_MAX);
    myRows.push_back(myJerkmaxRow);

    // X10 create FX and Label for AP Driver Update threshold
    myEpsilonaccRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_EPSILON_ACC);
    myRows.push_back(myEpsilonaccRow);

    // X11 create FX and Label for Startup Time to acc_max
    myTaccmaxRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_T_ACC_MAX);
    myRows.push_back(myTaccmaxRow);

    // X12 create FX and Label for Startup M flatness
    myMflatnessRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_M_FLATNESS);
    myRows.push_back(myMflatnessRow);

    // X13 create FX and Label for Startup M begin
    myMbeginRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_M_BEGIN);
    myRows.push_back(myMbeginRow);

    // X14 create FX and Label for using vehicle dynamics
    myUseVehDynamicsRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_USEVEHDYNAMICS);
    myRows.push_back(myUseVehDynamicsRow);

    // X14 create FX and Label for using vehicle dynamics
    myMaxVehPreviewRow = new CarFollowingModelRow(this, myVerticalFrameRows, SUMO_ATTR_CF_EIDM_MAX_VEH_PREVIEW);
    myRows.push_back(myMaxVehPreviewRow);

    // create myLabelIncompleteAttribute
    myLabelIncompleteAttribute = new FXLabel(myVerticalFrameRows, "Some attributes wasn't\nimplemented yet", nullptr, GUIDesignLabelAboutInfoCenter);
    myLabelIncompleteAttribute->hide();

    // show or hide ComboBox depending of current selected CFM
    refreshCFMFields();
}


void
GNEVehicleTypeDialog::CarFollowingModelParameters::refreshCFMFields() {
    // start hiding all rows
    for (const auto& row : myRows) {
        row->hide();
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
                myDeltaRow->show();
                mySteppingRow->show();
                myMinGapFactorRow->show();
                break;
            case SUMO_TAG_CF_IDMM:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                myDeltaRow->show();
                mySteppingRow->show();
                myMinGapFactorRow->show();
                myAdaptFactorRow->show();
                myAdaptTimeRow->show();
                break;
            case SUMO_TAG_CF_EIDM:
                myTauRow->show();
                myAccelRow->show();
                myDecelRow->show();
                myEmergencyDecelRow->show();
                myDeltaRow->show();
                mySteppingRow->show();
                myMinGapFactorRow->show();
                myTpreviewRow->show();
                myTreactionRow->show();
                myTPersDriveRow->show();
                myTPersEstimateRow->show();
                myCcoolnessRow->show();
                mySigmaleaderRow->show();
                mySigmagapRow->show();
                mySigmaerrorRow->show();
                myJerkmaxRow->show();
                myEpsilonaccRow->show();
                myTaccmaxRow->show();
                myMflatnessRow->show();
                myMbeginRow->show();
                myUseVehDynamicsRow->show();
                myMaxVehPreviewRow->show();
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
            case SUMO_TAG_CF_W99:
                myW99CC1->show();
                myW99CC2->show();
                myW99CC3->show();
                myW99CC4->show();
                myW99CC5->show();
                myW99CC6->show();
                myW99CC7->show();
                myW99CC8->show();
                myW99CC9->show();
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
    // refresh fields
    refreshCFMFields();
    // update value in all Rows
    for (const auto& row : myRows) {
        row->updateValue();
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
        myVehicleTypeDialog->myEditedDemandElement->setAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL, myComboBoxCarFollowModel->getText().text(), myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
    } else {
        myComboBoxCarFollowModel->setTextColor(FXRGB(255, 0, 0));
        myVehicleTypeDialog->myVehicleTypeValid = false;
        myVehicleTypeDialog->myInvalidAttr = SUMO_ATTR_CAR_FOLLOW_MODEL;
    }
    // set variable in all Rows
    for (const auto& row : myRows) {
        row->setVariable();
    }
    // refresh fields
    refreshCFMFields();
    return true;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog - public methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::GNEVehicleTypeDialog(GNEDemandElement* editedVehicleType, bool updatingElement) :
    GNEDemandElementDialog(editedVehicleType, updatingElement, 1372, 575),
    myVehicleTypeValid(true),
    myInvalidAttr(SUMO_ATTR_NOTHING) {

    // change default header
    changeDemandElementDialogHeader(updatingElement ? "Edit " + myEditedDemandElement->getTagStr() : "Create " + myEditedDemandElement->getTagStr());

    // Create auxiliar frames for values
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);

    // create vehicle type attributes
    myVTypeAttributes = new VTypeAttributes(this, columns);

    // create car following model parameters
    myCarFollowingModelParameters = new CarFollowingModelParameters(this, columns);

    // start a undo list for editing local to this additional
    initChanges();

    // add element if we aren't updating an existent element
    if (myUpdatingElement == false) {
        myEditedDemandElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(myEditedDemandElement, true), true);
    }

    // update values of Vehicle Type common attributes
    myVTypeAttributes->updateValues();

    // update values of Car Following Model Parameters
    myCarFollowingModelParameters->updateValues();

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
        // stop dialog successfully
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
    // update values of Vehicle Type common attributes
    myVTypeAttributes->updateValues();
    // update values of Car Following Model Parameters
    myCarFollowingModelParameters->updateValues();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeDialog - private methods
// ---------------------------------------------------------------------------

GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelRow::CarFollowingModelRow(CarFollowingModelParameters* carFollowingModelParametersParent, FXVerticalFrame* verticalFrame, SumoXMLAttr attr) :
    FXHorizontalFrame(verticalFrame, GUIDesignAuxiliarHorizontalFrame),
    myCarFollowingModelParametersParent(carFollowingModelParametersParent),
    myAttr(attr),
    myTextField(nullptr) {
    new FXLabel(this, toString(attr).c_str(), nullptr, GUIDesignLabelThickedFixed(150));
    myTextField = new FXTextField(this, GUIDesignTextFieldNCol, carFollowingModelParametersParent, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixed(180));
}


void
GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelRow::setVariable() {
    // set color of textField, depending if current value is valid or not
    if (myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->isValid(myAttr, myTextField->getText().text())) {
        // set color depending if is a default value
        if (myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->getTagProperty().getDefaultValue(myAttr) != myTextField->getText().text()) {
            myTextField->setTextColor(FXRGB(0, 0, 0));
        } else {
            myTextField->setTextColor(FXRGB(195, 195, 195));
        }
        myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->setAttribute(myAttr, myTextField->getText().text(),
                myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->getNet()->getViewNet()->getUndoList());
        // update value after setting it
        updateValue();
    } else {
        myTextField->setTextColor(FXRGB(255, 0, 0));
        // mark VType as invalid
        myCarFollowingModelParametersParent->myVehicleTypeDialog->myVehicleTypeValid = false;
        myCarFollowingModelParametersParent->myVehicleTypeDialog->myInvalidAttr = myAttr;
    }
}


void
GNEVehicleTypeDialog::CarFollowingModelParameters::CarFollowingModelRow::updateValue() {
    // set text of myTextField using current value of VType
    myTextField->setText(myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->getAttribute(myAttr).c_str());
    // set color depending if is a default value
    if (myCarFollowingModelParametersParent->myVehicleTypeDialog->myEditedDemandElement->getTagProperty().getDefaultValue(myAttr) != myTextField->getText().text()) {
        myTextField->setTextColor(FXRGB(0, 0, 0));
    } else {
        myTextField->setTextColor(FXRGB(195, 195, 195));
    }
}


/****************************************************************************/
