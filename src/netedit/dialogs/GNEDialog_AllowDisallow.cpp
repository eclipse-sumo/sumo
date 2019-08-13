/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_AllowDisallow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
// Dialog for edit rerouters
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/StringTokenizer.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewNet.h>

#include "GNEDialog_AllowDisallow.h"



// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog_AllowDisallow) GNEDialog_AllowDisallowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_CHANGE,               GNEDialog_AllowDisallow::onCmdValueChanged),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTALL,            GNEDialog_AllowDisallow::onCmdSelectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_UNSELECTALL,          GNEDialog_AllowDisallow::onCmdUnselectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTONLYROAD,       GNEDialog_AllowDisallow::onCmdSelectOnlyRoad),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,      GNEDialog_AllowDisallow::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,      GNEDialog_AllowDisallow::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONRESET,       GNEDialog_AllowDisallow::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEDialog_AllowDisallow, FXDialogBox, GNEDialog_AllowDisallowMap, ARRAYNUMBER(GNEDialog_AllowDisallowMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_AllowDisallow::GNEDialog_AllowDisallow(GNEViewNet* viewNet, GNEAttributeCarrier* AC) :
    FXDialogBox(viewNet->getApp(), ("Edit " + toString(SUMO_ATTR_ALLOW) + " " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignDialogBox),
    myViewNet(viewNet),
    myAC(AC) {
    assert(AC->getTagProperty().hasAttribute(SUMO_ATTR_ALLOW));
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create groupbox for options
    FXGroupBox* myGroupBoxOptions = new FXGroupBox(mainFrame, "Selection options", GUIDesignGroupBoxFrame);
    FXHorizontalFrame* myOptionsFrame = new FXHorizontalFrame(myGroupBoxOptions, GUIDesignAuxiliarHorizontalFrame);
    mySelectAllVClassButton = new FXButton(myOptionsFrame, "", GUIIconSubSys::getIcon(ICON_OK), this, MID_GNE_ALLOWDISALLOW_SELECTALL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, "Allow all vehicles", nullptr, GUIDesignLabelLeftThick);
    mySelectOnlyRoadVClassButton = new FXButton(myOptionsFrame, "", GUIIconSubSys::getIcon(ICON_OK), this, MID_GNE_ALLOWDISALLOW_SELECTONLYROAD, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, "Allow only road vehicles", nullptr, GUIDesignLabelLeftThick);
    myUnselectAllVClassButton = new FXButton(myOptionsFrame, "", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ALLOWDISALLOW_UNSELECTALL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, "Disallow all vehicles", nullptr, GUIDesignLabelLeftThick);
    // create groupbox for vehicles
    FXGroupBox* myGroupBoxVehiclesFrame = new FXGroupBox(mainFrame, ("Select " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignGroupBoxFrame);
    // Create frame for vehicles's columns
    FXHorizontalFrame* myVehiclesFrame = new FXHorizontalFrame(myGroupBoxVehiclesFrame, GUIDesignContentsFrame);
    // create left frame and fill it
    FXVerticalFrame* myContentLeftFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClass(myContentLeftFrame, SVC_PASSENGER, ICON_VCLASS_PASSENGER, "Default vehicle class");
    buildVClass(myContentLeftFrame, SVC_PRIVATE, ICON_VCLASS_PRIVATE, "A passenger car assigned for private use");
    buildVClass(myContentLeftFrame, SVC_TAXI, ICON_VCLASS_TAXI, "Vehicle for hire with a driver");
    buildVClass(myContentLeftFrame, SVC_BUS, ICON_VCLASS_BUS, "Urban line traffic");
    buildVClass(myContentLeftFrame, SVC_COACH, ICON_VCLASS_COACH, "Overland transport");
    buildVClass(myContentLeftFrame, SVC_DELIVERY, ICON_VCLASS_DELIVERY, "Vehicles specialized to deliver goods");
    buildVClass(myContentLeftFrame, SVC_TRUCK, ICON_VCLASS_TRUCK, "Vehicle designed to transport cargo");
    buildVClass(myContentLeftFrame, SVC_TRAILER, ICON_VCLASS_TRAILER, "Truck with trailer");
    buildVClass(myContentLeftFrame, SVC_EMERGENCY, ICON_VCLASS_EMERGENCY, "Vehicle designated to respond to an emergency");
    // create center frame and fill it
    FXVerticalFrame* myContentCenterFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClass(myContentCenterFrame, SVC_MOTORCYCLE, ICON_VCLASS_MOTORCYCLE, "Two- or three-wheeled motor vehicle");
    buildVClass(myContentCenterFrame, SVC_MOPED, ICON_VCLASS_MOPED, "Motorcycle not allowed in motorways");
    buildVClass(myContentCenterFrame, SVC_BICYCLE, ICON_VCLASS_BICYCLE, "Human-powered, pedal-driven vehicle");
    buildVClass(myContentCenterFrame, SVC_PEDESTRIAN, ICON_VCLASS_PEDESTRIAN, "Person traveling on foot");
    buildVClass(myContentCenterFrame, SVC_TRAM, ICON_VCLASS_TRAM, "Rail vehicle which runs on tracks");
    buildVClass(myContentCenterFrame, SVC_RAIL_ELECTRIC, ICON_VCLASS_RAIL_ELECTRIC, "Rail electric vehicle");
    buildVClass(myContentCenterFrame, SVC_RAIL_FAST, ICON_VCLASS_RAIL_ELECTRIC, "High-speed rail vehicle");
    buildVClass(myContentCenterFrame, SVC_RAIL_URBAN, ICON_VCLASS_RAIL_URBAN, "Heavier than tram");
    buildVClass(myContentCenterFrame, SVC_RAIL, ICON_VCLASS_RAIL, "Heavy rail vehicle");
    // create right frame and fill it  (8 vehicles)
    FXVerticalFrame* myContentRightFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClass(myContentRightFrame, SVC_E_VEHICLE, ICON_VCLASS_EVEHICLE, "Future electric mobility vehicles");
    buildVClass(myContentRightFrame, SVC_ARMY, ICON_VCLASS_ARMY, "Vehicle designed for military forces");
    buildVClass(myContentRightFrame, SVC_SHIP, ICON_VCLASS_SHIP, "Basic class for navigating waterway");
    buildVClass(myContentRightFrame, SVC_AUTHORITY, ICON_VCLASS_AUTHORITY, "Vehicle of a governmental security agency");
    buildVClass(myContentRightFrame, SVC_VIP, ICON_VCLASS_VIP, "A civilian security armored car used by VIPs");
    buildVClass(myContentRightFrame, SVC_HOV, ICON_VCLASS_HOV, "High-Occupancy Vehicle (two or more passengers)");
    buildVClass(myContentRightFrame, SVC_CUSTOM1, ICON_VCLASS_CUSTOM1, "Reserved for user-defined semantics");
    buildVClass(myContentRightFrame, SVC_CUSTOM2, ICON_VCLASS_CUSTOM2, "Reserved for user-defined semantics");
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(ICON_RESET), this, MID_GNE_ADDITIONALDIALOG_BUTTONRESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // reset dialog
    onCmdReset(nullptr, 0, nullptr);
}


GNEDialog_AllowDisallow::~GNEDialog_AllowDisallow() {
}


long
GNEDialog_AllowDisallow::onCmdValueChanged(FXObject* obj, FXSelector, void*) {
    FXButton* buttonPressed = dynamic_cast<FXButton*>(obj);
    // change icon of button
    for (auto i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        if (i->second.first == buttonPressed) {
            if (buttonPressed->getIcon() == GUIIconSubSys::getIcon(ICON_ACCEPT)) {
                buttonPressed->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
            } else {
                buttonPressed->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
            }
            return 1;
        }
    }
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdSelectAll(FXObject*, FXSelector, void*) {
    // change all icons to accept
    for (auto i : myVClassMap) {
        i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
    }
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdUnselectAll(FXObject*, FXSelector, void*) {
    // change all icons to cancel
    for (auto i : myVClassMap) {
        i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
    }
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdSelectOnlyRoad(FXObject*, FXSelector, void*) {
    // change all non-road icons to disallow, and allow for the rest
    for (auto i : myVClassMap) {
        if ((i.first & (SVC_PEDESTRIAN | SVC_NON_ROAD)) == 0) {
            i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
        } else {
            i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
        }
    }
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdAccept(FXObject*, FXSelector, void*) {
    // clear allow and disallow VClasses
    std::vector<std::string> allowedVehicles;
    for (auto i : myVClassMap) {
        // check if vehicle is alloweddepending of the Icon
        if (i.second.first->getIcon() == GUIIconSubSys::getIcon(ICON_ACCEPT)) {
            allowedVehicles.push_back(getVehicleClassNames(i.first));
        }
    }
    // chek if all vehicles are enabled and set new allowed vehicles
    myAC->setAttribute(SUMO_ATTR_ALLOW, joinToString(allowedVehicles, " "), myViewNet->getUndoList());
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdReset(FXObject*, FXSelector, void*) {
    if (myAC->getAttribute(SUMO_ATTR_ALLOW) == "all") {
        // iterate over myVClassMap and set all icons as true
        for (auto i : myVClassMap) {
            i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
        }
    } else {
        // declare string vector for saving all vclasses
        const std::vector<std::string>& allowStringVector = StringTokenizer(myAC->getAttribute(SUMO_ATTR_ALLOW)).getVector();
        const std::set<std::string> allowSet(allowStringVector.begin(), allowStringVector.end());
        // iterate over myVClassMap and set icons
        for (auto i : myVClassMap) {
            if (allowSet.count(getVehicleClassNames(i.first)) > 0) {
                i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
            } else {
                i.second.first->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
            }
        }
    }
    return 1;
}


void
GNEDialog_AllowDisallow::buildVClass(FXVerticalFrame* contentsFrame, SUMOVehicleClass vclass, GUIIcon vclassIcon, const std::string& description) {
    // add frame for vehicle icons
    FXHorizontalFrame* vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel* labelVehicleIcon = new FXLabel(vehicleFrame, "", GUIIconSubSys::getIcon(vclassIcon), GUIDesignLabelIcon64x32Thicked);
    labelVehicleIcon->setBackColor(FXRGBA(255, 255, 255, 255));
    // create frame for information and button
    FXVerticalFrame* buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame* buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[vclass].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[vclass].second = new FXLabel(buttonAndStatusFrame, toString(vclass).c_str(), nullptr, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, description.c_str(), nullptr, GUIDesignLabelLeftThick);
}

/****************************************************************************/
