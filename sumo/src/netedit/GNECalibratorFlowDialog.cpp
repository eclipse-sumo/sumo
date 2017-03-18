/****************************************************************************/
/// @file    GNECalibratorFlowDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrator flows
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

#include "GNECalibratorFlowDialog.h"
#include "GNECalibratorDialog.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorFlowDialog) GNECalibratorFlowDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,           GNECalibratorFlowDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,           GNECalibratorFlowDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,            GNECalibratorFlowDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNECalibratorFlowDialog, FXDialogBox, GNECalibratorFlowDialogMap, ARRAYNUMBER(GNECalibratorFlowDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorFlowDialog::GNECalibratorFlowDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorFlow &calibratorFlow) :
    GNEAdditionalDialog(calibratorFlow.getCalibratorParent(), 640, 480),
    myCalibratorDialogParent(calibratorDialog),
    myCalibratorFlow(&calibratorFlow),
    myCalibratorFlowValid(true) {
    // change default header
    changeAdditionalDialogHeader("Edit " + toString(calibratorFlow.getTag()) + " of " + toString(calibratorFlow.getCalibratorParent()->getTag()) +
                                 " '" + calibratorFlow.getCalibratorParent()->getID() + "'");
    /*
    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    
    // create horizontal frame for begin and end label
    FXHorizontalFrame* beginEndElementsLeft = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(beginEndElementsLeft, (toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " of " + toString(calibratorFlow.getTag())).c_str(), 0, GUIDesignLabelLeftThick);
    myCheckLabel = new FXLabel(beginEndElementsLeft, "", 0, GUIDesignLabelOnlyIcon);

    // create horizontal frame for begin and end text fields
    FXHorizontalFrame* beginEndElementsRight = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myBeginTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGESTART, GUIDesignTextFieldReal);
    myBeginTextField->setText(toString(myCalibratorFlow->getBegin()).c_str());
    myEndTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGEEND, GUIDesignTextFieldReal);
    myEndTextField->setText(toString(myCalibratorFlow->getEnd()).c_str());

    // set interval flag depending if interval exists
    if (myCalibratorDialogParent->findInterval(myCalibratorFlow->getBegin(), myCalibratorFlow->getEnd())) {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        myBeginEndValid = true;
    } else {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        myBeginEndValid = false;
    }

    // Create labels and tables
    FXHorizontalFrame* buttonAndLabelClosingLaneReroute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddCalibratorFlow = new FXButton(buttonAndLabelClosingLaneReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelClosingLaneReroute, ("Add new " + toString(SUMO_TAG_CLOSING_LANE_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myClosingLaneRerouteList = new FXTable(columnLeft, this, MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GUIDesignTableAdditionals);
    myClosingLaneRerouteList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myClosingLaneRerouteList->setSelTextColor(FXRGBA(0, 0, 0, 255));

    FXHorizontalFrame* buttonAndLabelClosinReroute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddClosingReroutes = new FXButton(buttonAndLabelClosinReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelClosinReroute, ("Add new " + toString(SUMO_TAG_CLOSING_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myClosingRerouteList = new FXTable(columnLeft, this, MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE, GUIDesignTableAdditionals);
    myClosingRerouteList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myClosingRerouteList->setSelTextColor(FXRGBA(0, 0, 0, 255));

    FXHorizontalFrame* buttonAndLabelDestProbReroute = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myAddDestProbReroutes = new FXButton(buttonAndLabelDestProbReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelDestProbReroute, ("Add new " + toString(SUMO_TAG_DEST_PROB_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myDestProbRerouteList = new FXTable(columnRight, this, MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE, GUIDesignTableAdditionals);
    myDestProbRerouteList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myDestProbRerouteList->setSelTextColor(FXRGBA(0, 0, 0, 255));

    FXHorizontalFrame* buttonAndLabelRouteProbReroute = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myAddRouteProbReroute = new FXButton(buttonAndLabelRouteProbReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelRouteProbReroute, ("Add new " + toString(SUMO_TAG_ROUTE_PROB_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myRouteProbRerouteList = new FXTable(columnRight, this, MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE, GUIDesignTableAdditionals);
    myRouteProbRerouteList->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myRouteProbRerouteList->setSelTextColor(FXRGBA(0, 0, 0, 255));
    */
    // update tables
    updateCalibratorFlowValues();
}


GNECalibratorFlowDialog::~GNECalibratorFlowDialog() {
}


long
GNECalibratorFlowDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorFlowValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myCalibratorFlow->getTag()) + " of " + toString(myCalibratorFlow->getCalibratorParent()->getTag())).c_str(), "%s",
                              (toString(myCalibratorFlow->getCalibratorParent()->getTag()) + "'s " + toString(myCalibratorFlow->getTag()) +
                               " cannot be updated because " + toString(myCalibratorFlow->getTag()) + " defined by " + toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " is invalid.").c_str());
        return 0;
    } else {
        // set new calibrator flow
        // myCalibratorFlow->set....
        // Stop Modal
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNECalibratorFlowDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorFlowDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // set default values
    // fields->set(---)
    // update tables
    updateCalibratorFlowValues();
    return 1;
}


void
GNECalibratorFlowDialog::updateCalibratorFlowValues() {
    /*
    // clear table
    myClosingLaneRerouteList->clearItems();
    // set number of rows
    myClosingLaneRerouteList->setTableSize(int(myCopyOfCalibratorFlow.size()), 5);
    // Configure list
    myClosingLaneRerouteList->setVisibleColumns(5);
    myClosingLaneRerouteList->setColumnWidth(0, 83);
    myClosingLaneRerouteList->setColumnWidth(1, 83);
    myClosingLaneRerouteList->setColumnWidth(2, 82);
    myClosingLaneRerouteList->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myClosingLaneRerouteList->setColumnWidth(4, GUIDesignTableIconCellWidth);
    myClosingLaneRerouteList->setColumnText(0, toString(SUMO_ATTR_LANE).c_str());
    myClosingLaneRerouteList->setColumnText(1, toString(SUMO_ATTR_ALLOW).c_str());
    myClosingLaneRerouteList->setColumnText(2, toString(SUMO_ATTR_DISALLOW).c_str());
    myClosingLaneRerouteList->setColumnText(3, "");
    myClosingLaneRerouteList->setColumnText(4, "");
    myClosingLaneRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (GNECalibratorFlow::iterator i = myCopyOfCalibratorFlow.begin(); i != myCopyOfCalibratorFlow.end(); i++) {
        // Set closing edge
        if (i->getClosedLane() != NULL) {
            item = new FXTableItem(i->getClosedLane()->getID().c_str());
            myClosingLaneRerouteList->setItem(indexRow, 0, item);
        } else {
            item = new FXTableItem("");
            myClosingLaneRerouteList->setItem(indexRow, 0, item);
        }
        // set allow vehicles
        item = new FXTableItem(getVehicleClassNames(i->getAllowedVehicles()).c_str());
        myClosingLaneRerouteList->setItem(indexRow, 1, item);
        // set disallow vehicles
        item = new FXTableItem(getVehicleClassNames(i->getDisallowedVehicles()).c_str());
        myClosingLaneRerouteList->setItem(indexRow, 2, item);
        // set valid icon
        item = new FXTableItem("");
        if (myCalibratorFlowValid) {
            item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        } else {
            item->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        }
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingLaneRerouteList->setItem(indexRow, 3, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingLaneRerouteList->setItem(indexRow, 4, item);
        // Update index
        indexRow++;
    }
    */
}


/****************************************************************************/
