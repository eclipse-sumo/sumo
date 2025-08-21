/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNECalibratorFlowDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
///
// Dialog for edit calibrator flows
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNECalibratorFlowDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorFlowDialog) GNECalibratorFlowDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,   GNECalibratorFlowDialog::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNECalibratorFlowDialog, GNETemplateElementDialog<GNEAdditional>, GNECalibratorFlowDialogMap, ARRAYNUMBER(GNECalibratorFlowDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorFlowDialog::GNECalibratorFlowDialog(GNEAdditional* calibratorFlow, bool updatingElement) :
    GNETemplateElementDialog<GNEAdditional>(calibratorFlow, updatingElement),
    myCalibratorFlowValid(false),
    myInvalidAttr(SUMO_ATTR_VEHSPERHOUR) {
    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeftLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnLeftValue = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightLabel = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRightValue = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    // 1 create combobox for type
    new FXLabel(columnLeftLabel, toString(SUMO_TAG_VTYPE).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myComboBoxVehicleType = new MFXComboBoxIcon(columnLeftValue, GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    // 2 create combobox for route
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ROUTE).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myComboBoxRoute = new MFXComboBoxIcon(columnLeftValue, GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
                                          this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    // 3 create textfield for vehs per hour
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_VEHSPERHOUR).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldVehsPerHour = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 4 create textfield for vehs per hour
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_SPEED).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldSpeed = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 5 create textfield for color
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_COLOR).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldColor = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 6 create textfield for lane
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DEPARTLANE).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldDepartLane = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 7 create textfield for pos
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DEPARTPOS).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldDepartPos = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 8 create textfield for speed
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_DEPARTSPEED).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldDepartSpeed = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 9 create textfield for lane
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ARRIVALLANE).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldArrivalLane = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 10 create textfield for arrival pos
    new FXLabel(columnLeftLabel, toString(SUMO_ATTR_ARRIVALPOS).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldArrivalPos = new FXTextField(columnLeftValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 11 create textfield for arrival speed
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_ARRIVALSPEED).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldArrivalSpeed = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 12 create textfield for arrival line
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_LINE).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldLine = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 13 create textfield for person number
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_PERSON_NUMBER).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldPersonNumber = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 14 create textfield for container number
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_CONTAINER_NUMBER).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldContainerNumber = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 15 create textfield for reroute
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_REROUTE).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myRerouteCheckButton = new FXCheckButton(columnRightValue, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // 16 create textfield for depart pos lat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_DEPARTPOS_LAT).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldDepartPosLat = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 17 create textfield for arrival pos lat
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_ARRIVALPOS_LAT).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldArrivalPosLat = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 18 create textfield for begin
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_BEGIN).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldBegin = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // 19 create textfield for end
    new FXLabel(columnRightLabel, toString(SUMO_ATTR_END).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldEnd = new FXTextField(columnRightValue, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // fill comboBox of VTypes
    for (const auto& vType : myElement->getNet()->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        myComboBoxVehicleType->appendIconItem(vType.second->getID().c_str(), vType.second->getACIcon());
    }
    // fill comboBox of Routes
    for (const auto& route : myElement->getNet()->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
        myComboBoxRoute->appendIconItem(route.second->getID().c_str(), route.second->getACIcon());
    }
    // update tables
    updateCalibratorFlowValues();
    // add element if we aren't updating an existent element
    if (!myUpdatingElement) {
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myElement, true), true);
    }
    // open dialog
    openDialog();
}


GNECalibratorFlowDialog::~GNECalibratorFlowDialog() {}


void
GNECalibratorFlowDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNECalibratorFlowDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    std::string title;
    std::string infoA;
    std::string infoB;
    const auto& parentTagString = myElement->getParentAdditionals().at(0)->getTagStr();
    // set title
    if (myUpdatingElement) {
        title = TLF("Error updating %'s %", myElement->getTagStr(), parentTagString);
        infoA = TLF("%'s % cannot be updated", parentTagString, myElement->getTagStr());
    } else {
        title = TLF("Error creating %'s %", myElement->getTagStr(), parentTagString);
        infoA = TLF("%'s % cannot be created", parentTagString, myElement->getTagStr());
    }

    if (!myCalibratorFlowValid) {
        infoB = TLF("because parameter % is invalid.", toString(myInvalidAttr));
        /*
            } else if (!myElement->getParentAdditionals().at(0)->checkChildAdditionalsOverlapping()) {
                infoB = TLF("because there is overlapping with another %.", myElement->getTagStr());
        */
    } else if ((myElement->getAttribute(SUMO_ATTR_VEHSPERHOUR).empty() && myElement->getAttribute(SUMO_ATTR_SPEED).empty()) ||
               (!myElement->getAttribute(SUMO_ATTR_VEHSPERHOUR).empty() && !myElement->getAttribute(SUMO_ATTR_SPEED).empty())) {
        infoB = TLF("because parameters % and % cannot be defined together.", toString(SUMO_ATTR_VEHSPERHOUR), toString(SUMO_ATTR_SPEED));
    }
    // continue depending of infoB
    if (infoB.size() > 0) {
        // open warning Box
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                              title, infoA, infoB);
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
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
    // get pointer to undo list (Only for code legibility)
    GNEUndoList* undoList = myElement->getNet()->getViewNet()->getUndoList();
    // set color of myComboBoxVehicleType, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_TYPE, myComboBoxVehicleType->getText().text())) {
        myComboBoxVehicleType->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_TYPE, myComboBoxVehicleType->getText().text(), undoList);
    } else {
        myComboBoxVehicleType->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_TYPE;
    }
    // set color of myComboBoxRoute, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_ROUTE, myComboBoxRoute->getText().text())) {
        myComboBoxRoute->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ROUTE, myComboBoxRoute->getText().text(), undoList);
    } else {
        myComboBoxRoute->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ROUTE;
    }
    // set color of myTextFieldVehsPerHour, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_VEHSPERHOUR, myTextFieldVehsPerHour->getText().text())) {
        myTextFieldVehsPerHour->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_VEHSPERHOUR, myTextFieldVehsPerHour->getText().text(), undoList);
    } else {
        myTextFieldVehsPerHour->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_VEHSPERHOUR;
    }
    // set color of myTextFieldSpeed, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_SPEED, myTextFieldSpeed->getText().text())) {
        myTextFieldSpeed->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_SPEED, myTextFieldSpeed->getText().text(), undoList);
        // Check VehsPerHour again
        if (myElement->isValid(SUMO_ATTR_VEHSPERHOUR, myTextFieldVehsPerHour->getText().text())) {
            myTextFieldVehsPerHour->setTextColor(FXRGB(0, 0, 0));
            myElement->setAttribute(SUMO_ATTR_VEHSPERHOUR, myTextFieldVehsPerHour->getText().text(), undoList);
            if (myInvalidAttr == SUMO_ATTR_VEHSPERHOUR) {
                myCalibratorFlowValid = true;
                myInvalidAttr = SUMO_ATTR_NOTHING;
            }
        }
    } else {
        myTextFieldSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_SPEED;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), undoList);
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    // set color of myTextFieldDepartLane, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_DEPARTLANE, myTextFieldDepartLane->getText().text())) {
        myTextFieldDepartLane->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_DEPARTLANE, myTextFieldDepartLane->getText().text(), undoList);
    } else {
        myTextFieldDepartLane->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTLANE;
    }
    // set color of myTextFieldDepartPos, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_DEPARTPOS, myTextFieldDepartPos->getText().text())) {
        myTextFieldDepartPos->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_DEPARTPOS, myTextFieldDepartPos->getText().text(), undoList);
    } else {
        myTextFieldDepartPos->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTPOS;
    }
    // set color of setDepartSpeed, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_DEPARTSPEED, myTextFieldDepartSpeed->getText().text())) {
        myTextFieldDepartSpeed->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_DEPARTSPEED, myTextFieldDepartSpeed->getText().text(), undoList);
    } else {
        myTextFieldDepartSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTSPEED;
    }
    // set color of myTextFieldArrivalLane, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_ARRIVALLANE, myTextFieldArrivalLane->getText().text())) {
        myTextFieldArrivalLane->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ARRIVALLANE, myTextFieldArrivalLane->getText().text(), undoList);
    } else {
        myTextFieldArrivalLane->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALLANE;
    }
    // set color of myTextFieldArrivalPos, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_ARRIVALPOS, myTextFieldArrivalPos->getText().text())) {
        myTextFieldArrivalPos->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ARRIVALPOS, myTextFieldArrivalPos->getText().text(), undoList);
    } else {
        myTextFieldArrivalPos->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALPOS;
    }
    // set color of setArrivalSpeed, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_ARRIVALSPEED, myTextFieldArrivalSpeed->getText().text())) {
        myTextFieldArrivalSpeed->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ARRIVALSPEED, myTextFieldArrivalSpeed->getText().text(), undoList);
    } else {
        myTextFieldArrivalSpeed->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALSPEED;
    }
    // set color of myTextFieldLine, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_LINE, myTextFieldLine->getText().text())) {
        myTextFieldLine->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_LINE, myTextFieldLine->getText().text(), undoList);
    } else {
        myTextFieldLine->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_LINE;
    }
    // set color of myTextFieldPersonNumber, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_PERSON_NUMBER, myTextFieldPersonNumber->getText().text())) {
        myTextFieldPersonNumber->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_PERSON_NUMBER, myTextFieldPersonNumber->getText().text(), undoList);
    } else {
        myTextFieldPersonNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_PERSON_NUMBER;
    }
    // set color of myTextFieldContainerNumber, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_CONTAINER_NUMBER, myTextFieldContainerNumber->getText().text())) {
        myTextFieldContainerNumber->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_CONTAINER_NUMBER, myTextFieldContainerNumber->getText().text(), undoList);
    } else {
        myTextFieldContainerNumber->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_CONTAINER_NUMBER;
    }
    // set reroute
    if (myRerouteCheckButton->getCheck()) {
        myElement->setAttribute(SUMO_ATTR_REROUTE, "true", undoList);
        myRerouteCheckButton->setText("true");
    } else {
        myElement->setAttribute(SUMO_ATTR_REROUTE, "false", undoList);
        myRerouteCheckButton->setText("false");
    }
    // set color of myTextFieldDepartPosLat, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_DEPARTPOS_LAT, myTextFieldDepartPosLat->getText().text())) {
        myTextFieldDepartPosLat->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_DEPARTPOS_LAT, myTextFieldDepartPosLat->getText().text(), undoList);
    } else {
        myTextFieldDepartPosLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_DEPARTPOS_LAT;
    }
    // set color of myTextFieldArrivalPosLat, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_ARRIVALPOS_LAT, myTextFieldArrivalPosLat->getText().text())) {
        myTextFieldArrivalPosLat->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ARRIVALPOS_LAT, myTextFieldArrivalPosLat->getText().text(), undoList);
    } else {
        myTextFieldArrivalPosLat->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_ARRIVALPOS_LAT;
    }
    // set color of myTextFieldBegin, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_BEGIN, myTextFieldBegin->getText().text())) {
        myTextFieldBegin->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_BEGIN, myTextFieldBegin->getText().text(), undoList);
    } else {
        myTextFieldBegin->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_END;
    }
    // set color of myTextFieldEnd, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_END, myTextFieldEnd->getText().text())) {
        myTextFieldEnd->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_END, myTextFieldEnd->getText().text(), undoList);
    } else {
        myTextFieldEnd->setTextColor(FXRGB(255, 0, 0));
        myCalibratorFlowValid = false;
        myInvalidAttr = SUMO_ATTR_BEGIN;
    }
    return 1;
}


void
GNECalibratorFlowDialog::updateCalibratorFlowValues() {
    // update fields
    myComboBoxVehicleType->setCurrentItem(myComboBoxVehicleType->findItem(myElement->getAttribute(SUMO_ATTR_TYPE).c_str()));
    const int routeIndex = myComboBoxVehicleType->findItem(myElement->getAttribute(SUMO_ATTR_ROUTE).c_str());
    if (routeIndex == -1) {
        myComboBoxRoute->setCurrentItem(0);
    } else {
        myComboBoxRoute->setCurrentItem(routeIndex);
    }
    myTextFieldVehsPerHour->setText(myElement->getAttribute(SUMO_ATTR_VEHSPERHOUR).c_str());
    myTextFieldSpeed->setText(myElement->getAttribute(SUMO_ATTR_SPEED).c_str());
    myTextFieldColor->setText(myElement->getAttribute(SUMO_ATTR_COLOR).c_str());
    myTextFieldDepartLane->setText(myElement->getAttribute(SUMO_ATTR_DEPARTLANE).c_str());
    myTextFieldDepartPos->setText(myElement->getAttribute(SUMO_ATTR_DEPARTPOS).c_str());
    myTextFieldDepartSpeed->setText(myElement->getAttribute(SUMO_ATTR_DEPARTSPEED).c_str());
    myTextFieldArrivalLane->setText(myElement->getAttribute(SUMO_ATTR_ARRIVALLANE).c_str());
    myTextFieldArrivalPos->setText(myElement->getAttribute(SUMO_ATTR_ARRIVALPOS).c_str());
    myTextFieldArrivalSpeed->setText(myElement->getAttribute(SUMO_ATTR_ARRIVALSPEED).c_str());
    myTextFieldLine->setText(myElement->getAttribute(SUMO_ATTR_LINE).c_str());
    myTextFieldPersonNumber->setText(myElement->getAttribute(SUMO_ATTR_PERSON_NUMBER).c_str());
    myTextFieldContainerNumber->setText(myElement->getAttribute(SUMO_ATTR_CONTAINER_NUMBER).c_str());
    myRerouteCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(myElement->getAttribute(SUMO_ATTR_REROUTE).c_str()));
    myTextFieldDepartPosLat->setText(myElement->getAttribute(SUMO_ATTR_DEPARTPOS_LAT).c_str());
    myTextFieldArrivalPosLat->setText(myElement->getAttribute(SUMO_ATTR_ARRIVALPOS_LAT).c_str());
    myTextFieldBegin->setText(myElement->getAttribute(SUMO_ATTR_BEGIN).c_str());
    myTextFieldEnd->setText(myElement->getAttribute(SUMO_ATTR_END).c_str());
}


/****************************************************************************/
