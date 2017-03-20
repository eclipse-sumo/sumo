/****************************************************************************/
/// @file    GNECalibratorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrators
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

#include "GNECalibratorDialog.h"
#include "GNECalibrator.h"
#include "GNECalibratorFlowDialog.h"
#include "GNECalibratorRouteDialog.h"
#include "GNECalibratorVehicleTypeDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorDialog) GNECalibratorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_ADD_ROUTE,         GNECalibratorDialog::onCmdAddRoute),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_CALIBRATORDIALOG_TABLE_ROUTE,       GNECalibratorDialog::onCmdClickedRoute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_ADD_FLOW,          GNECalibratorDialog::onCmdAddFlow),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_CALIBRATORDIALOG_TABLE_FLOW,        GNECalibratorDialog::onCmdClickedFlow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_ADD_VEHICLETYPE,   GNECalibratorDialog::onCmdAddVehicleType),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_CALIBRATORDIALOG_TABLE_VEHICLETYPE, GNECalibratorDialog::onCmdClickedVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,       GNECalibratorDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,       GNECalibratorDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,        GNECalibratorDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNECalibratorDialog, FXDialogBox, GNECalibratorDialogMap, ARRAYNUMBER(GNECalibratorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorDialog::GNECalibratorDialog(GNECalibrator* calibratorParent) :
    GNEAdditionalDialog(calibratorParent, 640, 480),
    myCalibratorParent(calibratorParent) {
    
    // Create two columns, one for Routes and VehicleTypes, and other for Flows
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // create add buton and label for routes
    FXHorizontalFrame* buttonAndLabelRoute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddRoute = new FXButton(buttonAndLabelRoute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_CALIBRATORDIALOG_ADD_ROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelRoute, ("Add new " + toString(SUMO_TAG_ROUTE) + "s").c_str(), 0, GUIDesignLabelThick);

    // Create table in left frame
    myRouteList = new FXTable(columnLeft, this, MID_GNE_CALIBRATORDIALOG_TABLE_ROUTE, GUIDesignTableAdditionals);
    myRouteList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myRouteList->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myRouteList->setEditable(false);

    // create add buton and label for vehicle types
    FXHorizontalFrame* buttonAndLabelVehicleType = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddVehicleType = new FXButton(buttonAndLabelVehicleType, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_CALIBRATORDIALOG_ADD_VEHICLETYPE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelVehicleType, ("Add new " + toString(SUMO_TAG_VTYPE) + "s").c_str(), 0, GUIDesignLabelThick);

    // Create table in left frame
    myVehicleTypeList = new FXTable(columnLeft, this, MID_GNE_CALIBRATORDIALOG_TABLE_ROUTE, GUIDesignTableAdditionals);
    myVehicleTypeList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myVehicleTypeList->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myVehicleTypeList->setEditable(false);

    // create add buton and label for flows in right frame
    FXHorizontalFrame* buttonAndLabelFlow = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myAddFlow = new FXButton(buttonAndLabelFlow, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_CALIBRATORDIALOG_ADD_FLOW, GUIDesignButtonIcon);
    myLabelFlow = new FXLabel(buttonAndLabelFlow, ("Add new " + toString(SUMO_TAG_FLOW) + "s").c_str(), 0, GUIDesignLabelThick);

    // Create table in right frame
    myFlowList = new FXTable(columnRight, this, MID_GNE_CALIBRATORDIALOG_TABLE_FLOW, GUIDesignTableAdditionals);
    myFlowList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myFlowList->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myFlowList->setEditable(false);

    // obtain copy of calibrator values
    myCopyOfCalibratorRoutes = myCalibratorParent->getCalibratorRoutes();
    myCopyOfCalibratorFlows = myCalibratorParent->getCalibratorFlows();
    myCopyOfCalibratorVehicleTypes = myCalibratorParent->getCalibratorVehicleTypes();

    // update tables
    updateRouteTable();
    updateFlowTable();
    updateVehicleTypeTable();

    // Execute additional dialog (To make it modal)
    execute();
}


GNECalibratorDialog::~GNECalibratorDialog() {
}


GNECalibrator*
GNECalibratorDialog::getCalibratorParent() const {
    return myCalibratorParent;
}


long
GNECalibratorDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // set new routes and flows into calibrator
    myCalibratorParent->setCalibratorRoutes(myCopyOfCalibratorRoutes);
    myCalibratorParent->setCalibratorFlows(myCopyOfCalibratorFlows);
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNECalibratorDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // Copy original routes and flows again and update table
    myCopyOfCalibratorRoutes = myCalibratorParent->getCalibratorRoutes();
    myCopyOfCalibratorFlows = myCalibratorParent->getCalibratorFlows();
    updateFlowTable();
    return 1;
}


long
GNECalibratorDialog::onCmdAddRoute(FXObject*, FXSelector, void*) {
    // create empty calibrator route and configure it with GNECalibratorRouteDialog
    GNECalibratorRoute newRoute(myCalibratorParent);
    if (GNECalibratorRouteDialog(this, newRoute).execute() == TRUE) {
        // if new route was sucesfully configured, add it to myCopyOfCalibratorRoutes
        myCopyOfCalibratorRoutes.push_back(newRoute);
        updateRouteTable();
        return 1;
    } else {
        return 0;
    }
}


long
GNECalibratorDialog::onCmdClickedRoute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfCalibratorRoutes.size(); i++) {
        if (myRouteList->getItem(i, 2)->hasFocus()) {
            // remove row
            myRouteList->removeRows(i);
            myCopyOfCalibratorRoutes.erase(myCopyOfCalibratorRoutes.begin() + i);
            return 1;
        }
    }
    // check if some begin or o end  button was pressed
    for (int i = 0; i < (int)myCopyOfCalibratorRoutes.size(); i++) {
        if (myRouteList->getItem(i, 0)->hasFocus() || myRouteList->getItem(i, 1)->hasFocus()) {
            // edit route
            GNECalibratorRouteDialog(this, *(myCopyOfCalibratorRoutes.begin() + i)).execute();
            return 1;
        }
    }
    // nothing to do
    return 0;
}


long
GNECalibratorDialog::onCmdAddFlow(FXObject*, FXSelector, void*) {
    // create empty calibrator flow and configure it with GNECalibratorFlowDialog
    GNECalibratorFlow newFlow(myCalibratorParent);
    if (GNECalibratorFlowDialog(this, newFlow).execute() == TRUE) {
        // if new flow was sucesfully configured, add it to myCopyOfCalibratorFlows
        myCopyOfCalibratorFlows.push_back(newFlow);
        updateFlowTable();
        return 1;
    } else {
        return 0;
    }
}


long
GNECalibratorDialog::onCmdClickedFlow(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfCalibratorFlows.size(); i++) {
        if (myFlowList->getItem(i, 2)->hasFocus()) {
            // remove row
            myFlowList->removeRows(i);
            myCopyOfCalibratorFlows.erase(myCopyOfCalibratorFlows.begin() + i);
            return 1;
        }
    }
    // check if some begin or o end  button was pressed
    for (int i = 0; i < (int)myCopyOfCalibratorFlows.size(); i++) {
        if (myFlowList->getItem(i, 0)->hasFocus() || myFlowList->getItem(i, 1)->hasFocus()) {
            // edit flow
            GNECalibratorFlowDialog(this, *(myCopyOfCalibratorFlows.begin() + i)).execute();
            return 1;
        }
    }
    // nothing to do
    return 0;
}


long
GNECalibratorDialog::onCmdAddVehicleType(FXObject*, FXSelector, void*) {
    // create empty calibrator flow and configure it with GNECalibratorVehicleTypeDialog
    GNECalibratorVehicleType newVehicleType(myCalibratorParent);
    if (GNECalibratorVehicleTypeDialog(this, newVehicleType).execute() == TRUE) {
        // if new flow was sucesfully configured, add it to myCopyOfCalibratorVehicleTypes
        myCopyOfCalibratorVehicleTypes.push_back(newVehicleType);
        updateVehicleTypeTable();
        return 1;
    } else {
        return 0;
    }
}


long
GNECalibratorDialog::onCmdClickedVehicleType(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfCalibratorVehicleTypes.size(); i++) {
        if (myVehicleTypeList->getItem(i, 2)->hasFocus()) {
            // remove row
            myVehicleTypeList->removeRows(i);
            myCopyOfCalibratorVehicleTypes.erase(myCopyOfCalibratorVehicleTypes.begin() + i);
            return 1;
        }
    }
    // check if some begin or o end  button was pressed
    for (int i = 0; i < (int)myCopyOfCalibratorVehicleTypes.size(); i++) {
        if (myVehicleTypeList->getItem(i, 0)->hasFocus() || myVehicleTypeList->getItem(i, 1)->hasFocus()) {
            // edit flow
            GNECalibratorVehicleTypeDialog(this, *(myCopyOfCalibratorVehicleTypes.begin() + i)).execute();
            return 1;
        }
    }
    // nothing to do
    return 0;
}


void
GNECalibratorDialog::updateRouteTable() {
    // clear table
    myRouteList->clearItems();
    // set number of rows
    myRouteList->setTableSize(int(myCopyOfCalibratorRoutes.size()), 3);
    // Configure list
    myRouteList->setVisibleColumns(4);
    myRouteList->setColumnWidth(0, 137);
    myRouteList->setColumnWidth(1, 136);
    myRouteList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myRouteList->setColumnText(0, toString(SUMO_ATTR_BEGIN).c_str());
    myRouteList->setColumnText(1, toString(SUMO_ATTR_END).c_str());
    myRouteList->setColumnText(2, "");
    myRouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNECalibratorRoute>::iterator i = myCopyOfCalibratorRoutes.begin(); i != myCopyOfCalibratorRoutes.end(); i++) {
        // Set ID
        item = new FXTableItem(toString(i->getRouteID()).c_str());
        myRouteList->setItem(indexRow, 0, item);
        // Set edges
        item = new FXTableItem(toString(i->getEdgesIDs()).c_str());
        myRouteList->setItem(indexRow, 1, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myRouteList->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}


void
GNECalibratorDialog::updateFlowTable() {
    // clear table
    myFlowList->clearItems();
    // set number of rows
    myFlowList->setTableSize(int(myCopyOfCalibratorFlows.size()), 3);
    // Configure list
    myFlowList->setVisibleColumns(4);
    myFlowList->setColumnWidth(0, 137);
    myFlowList->setColumnWidth(1, 136);
    myFlowList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myFlowList->setColumnText(0, toString(SUMO_ATTR_BEGIN).c_str());
    myFlowList->setColumnText(1, toString(SUMO_ATTR_END).c_str());
    myFlowList->setColumnText(2, "");
    myFlowList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNECalibratorFlow>::iterator i = myCopyOfCalibratorFlows.begin(); i != myCopyOfCalibratorFlows.end(); i++) {
        // Set id
        item = new FXTableItem(toString(i->getFlowID()).c_str());
        myFlowList->setItem(indexRow, 0, item);
        // Set route
        item = new FXTableItem(toString(i->getRoute()).c_str());
        myFlowList->setItem(indexRow, 1, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myFlowList->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
    // Enable or disable Add button and list depending of  currently there are routes and vehicle types defined
    std::string errorMsg;
    if(myCopyOfCalibratorRoutes.empty() && myCopyOfCalibratorVehicleTypes.empty()) {
        errorMsg = " and ";
    }
    if(myCopyOfCalibratorRoutes.size() == 0 || myCopyOfCalibratorVehicleTypes.size() == 0) {
        myAddFlow->disable();
        myFlowList->disable();
        std::string errorMessage = "No " + (myCopyOfCalibratorRoutes.empty()?(toString(SUMO_TAG_ROUTE) + "s"):("")) + errorMsg + 
                                   (myCopyOfCalibratorVehicleTypes.empty()?(toString(SUMO_TAG_VTYPE) + "s"):("")) + " defined";
        myLabelFlow->setText(errorMessage.c_str());
    } else {
        myAddFlow->enable();
        myFlowList->enable();
        myLabelFlow->setText(("Add new " + toString(SUMO_TAG_FLOW) + "s").c_str());
    }
}


void
GNECalibratorDialog::updateVehicleTypeTable() {
    // clear table
    myVehicleTypeList->clearItems();
    // set number of rows
    myVehicleTypeList->setTableSize(int(myCopyOfCalibratorVehicleTypes.size()), 3);
    // Configure list
    myVehicleTypeList->setVisibleColumns(4);
    myVehicleTypeList->setColumnWidth(0, 137);
    myVehicleTypeList->setColumnWidth(1, 136);
    myVehicleTypeList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myVehicleTypeList->setColumnText(0, toString(SUMO_ATTR_BEGIN).c_str());
    myVehicleTypeList->setColumnText(1, toString(SUMO_ATTR_END).c_str());
    myVehicleTypeList->setColumnText(2, "");
    myVehicleTypeList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNECalibratorVehicleType>::iterator i = myCopyOfCalibratorVehicleTypes.begin(); i != myCopyOfCalibratorVehicleTypes.end(); i++) {
        // Set id
        item = new FXTableItem(toString(i->getVehicleTypeID()).c_str());
        myVehicleTypeList->setItem(indexRow, 0, item);
        // Set route
        item = new FXTableItem(toString(i->getVClass()).c_str());
        myVehicleTypeList->setItem(indexRow, 1, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myVehicleTypeList->setItem(indexRow, 2, item);
        // Update index
        indexRow++;
    }
}
/****************************************************************************/
