/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEAllowVClassesDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// Dialog for edit allow VClass attribute
/****************************************************************************/

#include <netedit/GNEViewNet.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEAllowVClassesDialog.h"



// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAllowVClassesDialog) GNEAllowVClassesDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_CHANGE,       GNEAllowVClassesDialog::onCmdValueChanged),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTALL,    GNEAllowVClassesDialog::onCmdSelectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_UNSELECTALL,  GNEAllowVClassesDialog::onCmdUnselectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_ONLY_ROAD,    GNEAllowVClassesDialog::onCmdSelectOnlyRoad),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_ONLY_RAIL,    GNEAllowVClassesDialog::onCmdSelectOnlyRail),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,              GNEAllowVClassesDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,              GNEAllowVClassesDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,               GNEAllowVClassesDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEAllowVClassesDialog, FXDialogBox, GNEAllowVClassesDialogMap, ARRAYNUMBER(GNEAllowVClassesDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAllowVClassesDialog::GNEAllowVClassesDialog(GNEViewNet* viewNet, GNEAttributeCarrier* AC, SumoXMLAttr attr, bool* acceptChanges) :
    FXDialogBox(viewNet->getApp(), ("Edit " + toString(attr) + " " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignDialogBox),
    myViewNet(viewNet),
    myAC(AC),
    myEditedAttr(attr),
    myAcceptChanges(acceptChanges),
    myAllow(nullptr) {
    // call constructor
    constructor();
}


GNEAllowVClassesDialog::GNEAllowVClassesDialog(GNEViewNet* viewNet, std::string* allow, bool* acceptChanges) :
    FXDialogBox(viewNet->getApp(), TL("Edit allow vClasses"), GUIDesignDialogBox),
    myViewNet(viewNet),
    myAC(nullptr),
    myEditedAttr(SUMO_ATTR_ALLOW),
    myAcceptChanges(acceptChanges),
    myAllow(allow) {
    // call constructor
    constructor();
}


GNEAllowVClassesDialog::~GNEAllowVClassesDialog() {
}


long
GNEAllowVClassesDialog::onCmdValueChanged(FXObject* obj, FXSelector, void*) {
    FXButton* buttonPressed = dynamic_cast<FXButton*>(obj);
    // change icon of button
    for (const auto& vClass : myVClassMap) {
        if (vClass.second.first == buttonPressed) {
            if (buttonPressed->getIcon() == GUIIconSubSys::getIcon(GUIIcon::ACCEPT)) {
                buttonPressed->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
            } else {
                buttonPressed->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
            }
            return 1;
        }
    }
    return 1;
}


long
GNEAllowVClassesDialog::onCmdSelectAll(FXObject*, FXSelector, void*) {
    // change all icons to accept
    for (const auto& vClass : myVClassMap) {
        vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
    }
    return 1;
}


long
GNEAllowVClassesDialog::onCmdUnselectAll(FXObject*, FXSelector, void*) {
    // change all icons to cancel
    for (const auto& vClass : myVClassMap) {
        vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
    }
    return 1;
}


long
GNEAllowVClassesDialog::onCmdSelectOnlyRoad(FXObject*, FXSelector, void*) {
    // change all non-road icons to disallow, and allow for the rest
    for (const auto& vClass : myVClassMap) {
        if ((vClass.first & (SVC_PEDESTRIAN | SVC_NON_ROAD)) == 0) {
            vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
        } else {
            vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
        }
    }
    return 1;
}


long
GNEAllowVClassesDialog::onCmdSelectOnlyRail(FXObject*, FXSelector, void*) {
    // change all non-road icons to disallow, and allow for the rest
    for (const auto& vClass : myVClassMap) {
        if ((vClass.first & SVC_RAIL_CLASSES) != 0) {
            vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
        } else {
            vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
        }
    }
    return 1;
}


long
GNEAllowVClassesDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // clear allow and disallow VClasses
    std::vector<std::string> allowedVehicles, disallowedVehicles;
    for (const auto& vClass : myVClassMap) {
        // check if vehicle is allowed depending on the Icon
        if (vClass.second.first->getIcon() == GUIIconSubSys::getIcon(GUIIcon::ACCEPT)) {
            allowedVehicles.push_back(getVehicleClassNames(vClass.first));
        } else {
            disallowedVehicles.push_back(getVehicleClassNames(vClass.first));
        }
    }
    // check if all vehicles are enabled and set new allowed vehicles
    if (myAC) {
        myAC->setAttribute(myEditedAttr, joinToString(allowedVehicles, " "), myViewNet->getUndoList());
    } else {
        // update strings
        *myAllow = joinToString(allowedVehicles, " ");
    }
    // enable accept flag
    *myAcceptChanges = true;
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEAllowVClassesDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // disable accept flag
    *myAcceptChanges = false;
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEAllowVClassesDialog::onCmdReset(FXObject*, FXSelector, void*) {
    std::string allow;
    // set allow depending of myAC
    if (myAC) {
        allow = myAC->getAttribute(myEditedAttr);
    } else {
        allow = *myAllow;
    }
    // continue depending of allow
    if (allow == "all") {
        // iterate over myVClassMap and set all icons as true
        for (const auto& vClass : myVClassMap) {
            vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
        }
    } else {
        // declare string vector for saving all vclasses
        const std::vector<std::string>& allowStringVector = StringTokenizer(allow).getVector();
        const std::set<std::string> allowSet(allowStringVector.begin(), allowStringVector.end());
        // iterate over myVClassMap and set icons
        for (const auto& vClass : myVClassMap) {
            if (allowSet.count(getVehicleClassNames(vClass.first)) > 0) {
                vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::ACCEPT));
            } else {
                vClass.second.first->setIcon(GUIIconSubSys::getIcon(GUIIcon::CANCEL));
            }
        }
    }
    return 1;
}


void
GNEAllowVClassesDialog::constructor() {
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create groupbox for options
    FXGroupBox* myGroupBoxOptions = new FXGroupBox(mainFrame, TL("Selection options"), GUIDesignGroupBoxFrame);
    FXHorizontalFrame* myOptionsFrame = new FXHorizontalFrame(myGroupBoxOptions, GUIDesignAuxiliarHorizontalFrame);
    // allow all
    GUIDesigns::buildFXButton(myOptionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_ALLOWDISALLOW_SELECTALL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, TL("Allow all vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // only road
    GUIDesigns::buildFXButton(myOptionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_ALLOWDISALLOW_ONLY_ROAD, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, TL("Allow only road vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // only rail
    GUIDesigns::buildFXButton(myOptionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_ALLOWDISALLOW_ONLY_RAIL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, TL("Allow only rail vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // disallow all
    GUIDesigns::buildFXButton(myOptionsFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_ALLOWDISALLOW_UNSELECTALL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, TL("Disallow all vehicles"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create groupbox for vehicles
    FXGroupBox* myGroupBoxVehiclesFrame = new FXGroupBox(mainFrame, ("Select " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignGroupBoxFrame);
    // Create frame for vehicles's columns
    FXHorizontalFrame* myVehiclesFrame = new FXHorizontalFrame(myGroupBoxVehiclesFrame, GUIDesignContentsFrame);
    // create left frame and fill it
    FXVerticalFrame* myContentLeftFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClass(myContentLeftFrame, SVC_PASSENGER, GUIIcon::VCLASS_PASSENGER, TL("Default vehicle class"));
    buildVClass(myContentLeftFrame, SVC_PRIVATE, GUIIcon::VCLASS_PRIVATE, TL("A passenger car assigned for private use"));
    buildVClass(myContentLeftFrame, SVC_TAXI, GUIIcon::VCLASS_TAXI, TL("Vehicle for hire with a driver"));
    buildVClass(myContentLeftFrame, SVC_BUS, GUIIcon::VCLASS_BUS, TL("Urban line traffic"));
    buildVClass(myContentLeftFrame, SVC_COACH, GUIIcon::VCLASS_COACH, TL("Overland transport"));
    buildVClass(myContentLeftFrame, SVC_DELIVERY, GUIIcon::VCLASS_DELIVERY, TL("Vehicles specialized in delivering goods"));
    buildVClass(myContentLeftFrame, SVC_TRUCK, GUIIcon::VCLASS_TRUCK, TL("Vehicle designed to transport cargo"));
    buildVClass(myContentLeftFrame, SVC_TRAILER, GUIIcon::VCLASS_TRAILER, TL("Truck with trailer"));
    buildVClass(myContentLeftFrame, SVC_EMERGENCY, GUIIcon::VCLASS_EMERGENCY, TL("Vehicle designated to respond to an emergency"));
    buildVClass(myContentLeftFrame, SVC_MOTORCYCLE, GUIIcon::VCLASS_MOTORCYCLE, TL("Two- or three-wheeled motor vehicle"));
    buildVClass(myContentLeftFrame, SVC_MOPED, GUIIcon::VCLASS_MOPED, TL("Motorcycle not allowed in motorways"));
    // create center frame and fill it
    FXVerticalFrame* myContentCenterFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClass(myContentCenterFrame, SVC_BICYCLE, GUIIcon::VCLASS_BICYCLE, TL("Human-powered, pedal-driven vehicle"));
    buildVClass(myContentCenterFrame, SVC_SCOOTER, GUIIcon::VCLASS_SCOOTER, TL("An electric scooter or a kick scooter"));
    buildVClass(myContentCenterFrame, SVC_PEDESTRIAN, GUIIcon::VCLASS_PEDESTRIAN, TL("Person traveling on foot"));
    buildVClass(myContentCenterFrame, SVC_WHEELCHAIR, GUIIcon::VCLASS_WHEELCHAIR, TL("A mobility impaired person"));
    buildVClass(myContentCenterFrame, SVC_TRAM, GUIIcon::VCLASS_TRAM, TL("Rail vehicle which runs on tracks"));
    buildVClass(myContentCenterFrame, SVC_RAIL_ELECTRIC, GUIIcon::VCLASS_RAIL_ELECTRIC, TL("Rail electric vehicle"));
    buildVClass(myContentCenterFrame, SVC_RAIL_FAST, GUIIcon::VCLASS_RAIL_FAST, TL("High-speed rail vehicle"));
    buildVClass(myContentCenterFrame, SVC_RAIL_URBAN, GUIIcon::VCLASS_RAIL_URBAN, TL("Heavier than tram"));
    buildVClass(myContentCenterFrame, SVC_RAIL, GUIIcon::VCLASS_RAIL, TL("Heavy rail vehicle"));
    buildVClass(myContentCenterFrame, SVC_CABLE_CAR, GUIIcon::VCLASS_CABLE_CAR, TL("A conveyance suspended on a cable"));
    buildVClass(myContentCenterFrame, SVC_SUBWAY, GUIIcon::VCLASS_SUBWAY, TL("A railway that mostly runs underground"));
    // create right frame and fill it  (8 vehicles)
    FXVerticalFrame* myContentRightFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClass(myContentRightFrame, SVC_E_VEHICLE, GUIIcon::VCLASS_EVEHICLE, TL("Future electric mobility vehicles"));
    buildVClass(myContentRightFrame, SVC_ARMY, GUIIcon::VCLASS_ARMY, TL("Vehicle designed for military forces"));
    buildVClass(myContentRightFrame, SVC_SHIP, GUIIcon::VCLASS_SHIP, TL("Basic class for navigating waterway"));
    buildVClass(myContentRightFrame, SVC_AUTHORITY, GUIIcon::VCLASS_AUTHORITY, TL("Vehicle of a governmental security agency"));
    buildVClass(myContentRightFrame, SVC_VIP, GUIIcon::VCLASS_VIP, TL("A civilian security armored car used by VIPs"));
    buildVClass(myContentRightFrame, SVC_HOV, GUIIcon::VCLASS_HOV, TL("High-Occupancy Vehicle (two or more passengers)"));
    buildVClass(myContentRightFrame, SVC_CONTAINER, GUIIcon::VCLASS_CONTAINER, TL("A transport container"));
    buildVClass(myContentRightFrame, SVC_AIRCRAFT, GUIIcon::VCLASS_AIRCRAFT, TL("An airplane"));
    buildVClass(myContentRightFrame, SVC_DRONE, GUIIcon::VCLASS_DRONE, TL("A small unmanned robot"));
    buildVClass(myContentRightFrame, SVC_CUSTOM1, GUIIcon::VCLASS_CUSTOM1, TL("Reserved for user-defined semantics"));
    buildVClass(myContentRightFrame, SVC_CUSTOM2, GUIIcon::VCLASS_CUSTOM2, TL("Reserved for user-defined semantics"));
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = GUIDesigns::buildFXButton(buttonsFrame, TL("accept"), "", TL("close"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("cancel"), "", TL("close"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    myResetButton = GUIDesigns::buildFXButton(buttonsFrame, TL("reset"), "", TL("close"),  GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // reset dialog
    onCmdReset(nullptr, 0, nullptr);
}


void
GNEAllowVClassesDialog::buildVClass(FXVerticalFrame* contentsFrame, SUMOVehicleClass vclass, GUIIcon vclassIcon, const std::string& description) {
    // add frame for vehicle icons
    FXHorizontalFrame* vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel* labelVehicleIcon = new FXLabel(vehicleFrame, "", GUIIconSubSys::getIcon(vclassIcon), GUIDesignLabelIcon64x32Thicked);
    labelVehicleIcon->setBackColor(FXRGBA(255, 255, 255, 255));
    // create frame for information and button
    FXVerticalFrame* buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame* buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[vclass].first = GUIDesigns::buildFXButton(buttonAndStatusFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[vclass].second = new FXLabel(buttonAndStatusFrame, toString(vclass).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, description.c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
}


/****************************************************************************/
