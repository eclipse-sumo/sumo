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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_CHANGE,           GNEDialog_AllowDisallow::onCmdValueChanged),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,   GNEDialog_AllowDisallow::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,   GNEDialog_AllowDisallow::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,    GNEDialog_AllowDisallow::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEDialog_AllowDisallow, FXDialogBox, GNEDialog_AllowDisallowMap, ARRAYNUMBER(GNEDialog_AllowDisallowMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDialog_AllowDisallow::GNEDialog_AllowDisallow(FXApp* app, std::string *allow) :
    FXDialogBox(app, ("Edit allowed and disallowed " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignDialogBoxExplicit, 0, 0, 1024, 470, 0, 0, 0, 0), 
    myAllow(allow),
    myCopyOfAllow(*allow) {
    FXHorizontalFrame *vehicleFrame = NULL;
    FXVerticalFrame *buttonAndInformation = NULL;
    FXHorizontalFrame *buttonAndStatusFrame = NULL;
    FXLabel *labelVehicleIcon = NULL;
    FXLabel *labelVehicleDescription = NULL;
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // Create frame for contents
    myContentFrame = new FXHorizontalFrame(mainFrame, GUIDesignContentsFrame);
    FXVerticalFrame *myContentLeftFrame = new FXVerticalFrame(myContentFrame, GUIDesignAuxiliarFrame);
    // add vehicle frame for SVC_IGNORING
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_IGNORING), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_IGNORING].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_IGNORING].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "May drive on all lanes", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_PRIVATE
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_PRIVATE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_PRIVATE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_PRIVATE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "A passenger car assigned for private use", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_EMERGENCY
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_EMERGENCY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_EMERGENCY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_EMERGENCY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Vehicle designated to respond to an emergency", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_AUTHORITY
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_AUTHORITY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_AUTHORITY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_AUTHORITY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Vehicle of a governmental security agency", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_ARMY
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_ARMY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_ARMY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_ARMY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Vehicle designed for military forces", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_VIP
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_VIP), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_VIP].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_VIP].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "A civilian security armored car used by VIPs", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_PASSENGER
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_PASSENGER), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_PASSENGER].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_PASSENGER].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Default vehicle class", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_HOV
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_HOV), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_HOV].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_HOV].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "High-Occupancy Vehicle (two or more passengers)", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_TAXI
    vehicleFrame = new FXHorizontalFrame(myContentLeftFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TAXI), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TAXI].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TAXI].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Vehicle for hire with a driver", 0, GUIDesignLabelLeftThick);
    // create center frame
    FXVerticalFrame *myContentCenterFrame = new FXVerticalFrame(myContentFrame, GUIDesignAuxiliarFrame);  
    // add vehicle frame for SVC_BUS
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_BUS), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_BUS].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_BUS].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Urban line traffic", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_COACH
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_COACH), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_COACH].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_COACH].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Overland transport", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_DELIVERY
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_DELIVERY), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_DELIVERY].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_DELIVERY].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Vehicles specialized to deliver goods", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_TRUCK
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TRUCK), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TRUCK].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TRUCK].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Vehicle designed to transport cargo", 0, GUIDesignLabelLeftThick);
     // add vehicle frame for SVC_TRAILER
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TRAILER), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TRAILER].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TRAILER].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Truck with trailer", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_TRAM
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_TRAM), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_TRAM].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_TRAM].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Rail vehicle which runs on tracks", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_RAIL
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_RAIL_URBAN), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_RAIL_URBAN].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_RAIL_URBAN].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Heavier than tram", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_RAIL_URBAN
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_RAIL), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_RAIL].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_RAIL].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Heavy rail vehicle (ICE)", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_RAIL_ELECTRIC
    vehicleFrame = new FXHorizontalFrame(myContentCenterFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_RAIL_ELECTRIC), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_RAIL_ELECTRIC].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_RAIL_ELECTRIC].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Rail electric vehicle (Trolleybus)", 0, GUIDesignLabelLeftThick);
    // create right frame
    FXVerticalFrame *myContentRightFrame = new FXVerticalFrame(myContentFrame, GUIDesignAuxiliarFrame);
    // add vehicle frame for SVC_MOTORCYCLE
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_MOTORCYCLE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_MOTORCYCLE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_MOTORCYCLE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Two- or three-wheeled motor vehicle", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_MOPED
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_MOPED), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_MOPED].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_MOPED].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Motorcycle not allowed in motorways", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_BICYCLE
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_BICYCLE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_BICYCLE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_BICYCLE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Human-powered, pedal-driven vehicle", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_PEDESTRIAN
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_PEDESTRIAN), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_PEDESTRIAN].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_PEDESTRIAN].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Person traveling on foot", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_E_VEHICLE
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_EVEHICLE), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_E_VEHICLE].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_E_VEHICLE].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Future electric mobility vehicles", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_SHIP
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_SHIP), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_SHIP].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_SHIP].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Basic class for navigating waterway", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_CUSTOM1
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_CUSTOM1), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_CUSTOM1].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_CUSTOM1].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Reserved for user-defined semantics", 0, GUIDesignLabelLeftThick);
    // add vehicle frame for SVC_CUSTOM2
    vehicleFrame = new FXHorizontalFrame(myContentRightFrame, GUIDesignAuxiliarHorizontalFrame);
    labelVehicleIcon = new FXLabel(vehicleFrame, "",GUIIconSubSys::getIcon(ICON_VCLASS_CUSTOM2), GUIDesignLabelIconVClass);
    labelVehicleIcon->setBackColor(FXRGBA(255,255,255,255));
    // create frame for information and button
    buttonAndInformation = new FXVerticalFrame(vehicleFrame, GUIDesignAuxiliarHorizontalFrame);
    buttonAndStatusFrame = new FXHorizontalFrame(buttonAndInformation, GUIDesignAuxiliarHorizontalFrame);
    // create status and text button
    myVClassMap[SVC_CUSTOM2].first = new FXButton(buttonAndStatusFrame, "", GUIIconSubSys::getIcon(ICON_EMPTY), this, MID_GNE_ALLOWDISALLOW_CHANGE, GUIDesignButtonIcon);
    myVClassMap[SVC_CUSTOM2].second = new FXLabel(buttonAndStatusFrame, "status", NULL, GUIDesignLabelLeftThick);
    // create label for description of vehicle
    labelVehicleDescription = new FXLabel(buttonAndInformation, "Reserved for user-defined semantics", 0, GUIDesignLabelLeftThick);
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
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*>>::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
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
GNEDialog_AllowDisallow::onCmdAccept(FXObject*, FXSelector, void*) {
    // clear allow and disallow VClasses
    std::vector<std::string> allowedVehicles;
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*>>::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
        // check if vehicle is alloweddepending of the Icon
        if(i->second.first->getIcon() == GUIIconSubSys::getIcon(ICON_ACCEPT)) {
            allowedVehicles.push_back(getVehicleClassNames(i->first) );
        }
    }
    // set new allowed vehicles
    (*myAllow) = joinToString(allowedVehicles, " ");
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
    for(std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*>>::iterator i = myVClassMap.begin(); i != myVClassMap.end(); i++) {
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

/****************************************************************************/
