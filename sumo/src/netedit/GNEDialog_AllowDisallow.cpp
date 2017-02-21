/****************************************************************************/
/// @file    GNEDialog_AllowDisallow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// Dialog for edit rerouters
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
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXAttributes.h>

#include "GNEDialog_AllowDisallow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDialog_AllowDisallow) GNEDialog_AllowDisallowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_CHANGE,               GNEDialog_AllowDisallow::onCmdValueChanged),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTALL,            GNEDialog_AllowDisallow::onCmdSelectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_UNSELECTALL,          GNEDialog_AllowDisallow::onCmdUnselectAll),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_SELECTONLYNONROAD,    GNEDialog_AllowDisallow::onCmdSelectOnlyNonRoad),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,       GNEDialog_AllowDisallow::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,       GNEDialog_AllowDisallow::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,        GNEDialog_AllowDisallow::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEDialog_AllowDisallow, FXDialogBox, GNEDialog_AllowDisallowMap, ARRAYNUMBER(GNEDialog_AllowDisallowMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_AllowDisallow::GNEDialog_AllowDisallow(FXApp* app, std::string *allow) :
    FXDialogBox(app, ("Edit allowed and disallowed " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignDialogBoxExplicit, 0, 0, 1024, 590, 0, 0, 0, 0), 
    myAllow(allow),
    myCopyOfAllow(*allow) {
    // set vehicle icon for this dialog
    this->setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create groupbox for options
    FXGroupBox *myGroupBoxOptions = new FXGroupBox(mainFrame, "Selection options", GUIDesignGroupBoxFrame);
    FXHorizontalFrame *myOptionsFrame = new FXHorizontalFrame(myGroupBoxOptions, GUIDesignAuxiliarHorizontalFrame);
    mySelectAllVClassButton = new FXButton(myOptionsFrame, "", GUIIconSubSys::getIcon(ICON_OK), this, MID_GNE_ALLOWDISALLOW_SELECTALL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, "Select all vehicles", NULL, GUIDesignLabelLeftThick);
    myUnselectAllVClassButton = new FXButton(myOptionsFrame, "", GUIIconSubSys::getIcon(ICON_OK), this, MID_GNE_ALLOWDISALLOW_UNSELECTALL, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, "Unselect all vehicles", NULL, GUIDesignLabelLeftThick);
    mySelectOnlyNonRoadVClassButton = new FXButton(myOptionsFrame, "", GUIIconSubSys::getIcon(ICON_OK), this, MID_GNE_ALLOWDISALLOW_SELECTONLYNONROAD, GUIDesignButtonIcon);
    new FXLabel(myOptionsFrame, "Select only non-road vehicles", NULL, GUIDesignLabelLeftThick);
    // create groupbox for vehicles
    FXGroupBox *myGroupBoxVehiclesFrame = new FXGroupBox(mainFrame, ("Select " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignGroupBoxFrame);
    // Create frame for vehicles's columns
    FXHorizontalFrame *myVehiclesFrame = new FXHorizontalFrame(myGroupBoxVehiclesFrame, GUIDesignContentsFrame);
    // create left frame and fill it (9 vehicles)
    FXVerticalFrame *myContentLeftFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClassPassenger(myContentLeftFrame);
    buildVClassTaxi(myContentLeftFrame);
    buildVClassBus(myContentLeftFrame);
    buildVClassCoach(myContentLeftFrame);
    buildVClassDelivery(myContentLeftFrame);
    buildVClassTruck(myContentLeftFrame);
    buildVClassTrailer(myContentLeftFrame);
    buildVClassEmergency(myContentLeftFrame);
    // create center frame and fill it (9 vehicles)
    FXVerticalFrame *myContentCenterFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);  
    buildVClassMotorcycle(myContentCenterFrame);
    buildVClassMoped(myContentCenterFrame);
    buildVClassBicycle(myContentCenterFrame);
    buildVClassPedestrian(myContentCenterFrame);
    buildVClassTram(myContentCenterFrame);
    buildVClassRailUrban(myContentCenterFrame);
    buildVClassRail(myContentCenterFrame);
    buildVClassRailElectric(myContentCenterFrame);
    buildVClassShip(myContentCenterFrame);
    buildVClassEVehicle(myContentCenterFrame);
    // create right frame and fill it  (8 vehicles)
    FXVerticalFrame *myContentRightFrame = new FXVerticalFrame(myVehiclesFrame, GUIDesignAuxiliarFrame);
    buildVClassPrivate(myContentRightFrame);
    buildVClassArmy(myContentRightFrame);
    buildVClassAuthority(myContentRightFrame);
    buildVClassVip(myContentRightFrame);
    buildVClassHov(myContentRightFrame);
    buildVClassCustom1(myContentRightFrame);
    buildVClassCustom2(myContentRightFrame);
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(ICON_RESET), this, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // reset dialog
    onCmdReset(0,0,0);
}


GNEDialog_AllowDisallow::~GNEDialog_AllowDisallow() {
}


long 
GNEDialog_AllowDisallow::onCmdValueChanged(FXObject* obj, FXSelector, void*) {
    FXButton *buttonPressed = dynamic_cast<FXButton*>(obj);
    // change icon of button
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> >::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        if(i->second.first == buttonPressed) {
            if(buttonPressed->getIcon() == GUIIconSubSys::getIcon(ICON_ACCEPT)) {
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
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> >::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        i->second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
    }
    return 1;
}


long 
GNEDialog_AllowDisallow::onCmdUnselectAll(FXObject*, FXSelector, void*) {
    // change all icons to cancel
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> >::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        i->second.first->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
    }
    return 1;
}


long 
GNEDialog_AllowDisallow::onCmdSelectOnlyNonRoad(FXObject*, FXSelector, void*) {
    // change all non-road icons to accept, and to cancel for the rest
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> >::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        if((i->first == SVC_TRAM) || (i->first == SVC_RAIL) || (i->first == SVC_RAIL_URBAN) || (i->first == SVC_RAIL_ELECTRIC) || (i->first == SVC_SHIP)) {
            i->second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
        } else {
            i->second.first->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
        }
    }
    return 1;
}


long
GNEDialog_AllowDisallow::onCmdAccept(FXObject*, FXSelector, void*) {
    // clear allow and disallow VClasses
    std::vector<std::string> allowedVehicles;
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> >::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        // check if vehicle is alloweddepending of the Icon
        if(i->second.first->getIcon() == GUIIconSubSys::getIcon(ICON_ACCEPT)) {
            allowedVehicles.push_back(getVehicleClassNames(i->first) );
        }
    }
    // chek if all vehicles are enabled and set new allowed vehicles
    if(allowedVehicles.size() == 25) {
        (*myAllow) = "all";
    } else {
        (*myAllow) = joinToString(allowedVehicles, " ");
    }
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
    // clear allow and disallow VClasses
    std::vector<std::string> allowStringVector;
    SUMOSAXAttributes::parseStringVector(myCopyOfAllow, allowStringVector);
    // iterate over myVClassMap and set icons
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> >::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        if(std::find(allowStringVector.begin(), allowStringVector.end(), getVehicleClassNames(i->first)) != allowStringVector.end()) {
            i->second.first->setIcon(GUIIconSubSys::getIcon(ICON_ACCEPT));
            i->second.second->setText((getVehicleClassNames(i->first) + " allowed").c_str());
        } else {
            i->second.first->setIcon(GUIIconSubSys::getIcon(ICON_CANCEL));
            i->second.second->setText((getVehicleClassNames(i->first) + " disallowed").c_str());
        }
    }
    return 1;
}


void 
GNEDialog_AllowDisallow::buildVClassPrivate(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_PRIVATE
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_PRIVATE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_PRIVATE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_PRIVATE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "A passenger car assigned for private use", 0, GUIDesignLabelLeftThick);
}


void 
    GNEDialog_AllowDisallow::buildVClassEmergency(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_EMERGENCY
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_EMERGENCY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_EMERGENCY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_EMERGENCY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Vehicle designated to respond to an emergency", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassAuthority(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_AUTHORITY
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_AUTHORITY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_AUTHORITY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_AUTHORITY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Vehicle of a governmental security agency", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassArmy(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_ARMY
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_ARMY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_ARMY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_ARMY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Vehicle designed for military forces", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassVip(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_VIP
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_VIP), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_VIP].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_VIP].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "A civilian security armored car used by VIPs", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassPassenger(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_PASSENGER
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_PASSENGER), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_PASSENGER].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_PASSENGER].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Default vehicle class", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassHov(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_HOV
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_HOV), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_HOV].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_HOV].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "High-Occupancy Vehicle (two or more passengers)", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassTaxi(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_TAXI
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TAXI), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TAXI].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TAXI].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Vehicle for hire with a driver", 0, GUIDesignLabelLeftThick); 
}


void 
GNEDialog_AllowDisallow::buildVClassBus(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_BUS
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_BUS), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_BUS].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_BUS].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Urban line traffic", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassCoach(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_COACH
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_COACH), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_COACH].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_COACH].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Overland transport", 0, GUIDesignLabelLeftThick);
}


void
GNEDialog_AllowDisallow::buildVClassDelivery(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_DELIVERY
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_DELIVERY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_DELIVERY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_DELIVERY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Vehicles specialized to deliver goods", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassTruck(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_TRUCK
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TRUCK), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TRUCK].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TRUCK].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Vehicle designed to transport cargo", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassTrailer(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_TRAILER
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TRAILER), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TRAILER].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TRAILER].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Truck with trailer", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassTram(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_TRAM
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TRAM), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TRAM].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TRAM].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Rail vehicle which runs on tracks", 0, GUIDesignLabelLeftThick);
}




void 
GNEDialog_AllowDisallow::buildVClassRailUrban(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_RAIL_URBAN
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_RAIL), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_RAIL].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_RAIL].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Heavy rail vehicle (ICE)", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassRail(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_RAIL
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_RAIL_URBAN), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_RAIL_URBAN].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_RAIL_URBAN].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Heavier than tram", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassRailElectric(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_RAIL_ELECTRIC
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_RAIL_ELECTRIC), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_RAIL_ELECTRIC].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_RAIL_ELECTRIC].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Rail electric vehicle (Trolleybus)", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassMotorcycle(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_MOTORCYCLE
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_MOTORCYCLE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_MOTORCYCLE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_MOTORCYCLE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Two- or three-wheeled motor vehicle", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassMoped(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_MOPED
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_MOPED), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_MOPED].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_MOPED].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Motorcycle not allowed in motorways", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassBicycle(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_BICYCLE
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_BICYCLE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_BICYCLE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_BICYCLE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Human-powered, pedal-driven vehicle", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassPedestrian(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_PEDESTRIAN
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_PEDESTRIAN), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_PEDESTRIAN].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_PEDESTRIAN].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Person traveling on foot", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassEVehicle(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_E_VEHICLE
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_EVEHICLE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_E_VEHICLE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_E_VEHICLE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Future electric mobility vehicles", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassShip(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_SHIP
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_SHIP), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_SHIP].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_SHIP].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Basic class for navigating waterway", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassCustom1(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_CUSTOM1
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_CUSTOM1), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_CUSTOM1].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_CUSTOM1].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Reserved for user-defined semantics", 0, GUIDesignLabelLeftThick);
}


void 
GNEDialog_AllowDisallow::buildVClassCustom2(FXVerticalFrame *contentsFrame) {
    // add vehicle frame for SVC_CUSTOM2
    FXHorizontalFrame *vehicleFrame = new FXHorizontalFrame(contentsFrame, GUIDesignAuxiliarHorizontalFrame);
    FXLabel *labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_CUSTOM2), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    FXVerticalFrame *buttonAndInformationFrame = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    FXHorizontalFrame *buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformationFrame, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_CUSTOM2].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_CUSTOM2].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    new FXLabel(buttonAndInformationFrame, "Reserved for user-defined semantics", 0, GUIDesignLabelLeftThick);
}

/****************************************************************************/
