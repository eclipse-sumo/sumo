/****************************************************************************/
/// @file    GNECalibratorRouteDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrator routes
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

#include "GNECalibratorRouteDialog.h"
#include "GNECalibratorDialog.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorRouteDialog) GNECalibratorRouteDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,           GNECalibratorRouteDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,           GNECalibratorRouteDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,            GNECalibratorRouteDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNECalibratorRouteDialog, FXDialogBox, GNECalibratorRouteDialogMap, ARRAYNUMBER(GNECalibratorRouteDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorRouteDialog::GNECalibratorRouteDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorRoute &calibratorRoute) :
    GNEAdditionalDialog(calibratorRoute.getCalibratorParent(), 640, 480),
    myCalibratorDialogParent(calibratorDialog),
    myCalibratorRoute(&calibratorRoute),
    myCalibratorRouteValid(true) {
    // change default header
    changeAdditionalDialogHeader("Edit " + toString(calibratorRoute.getTag()) + " of " + toString(calibratorRoute.getCalibratorParent()->getTag()) +
                                 " '" + calibratorRoute.getCalibratorParent()->getID() + "'");

    /*
    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    
    // create horizontal frame for begin and end label
    FXHorizontalFrame* beginEndElementsLeft = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(beginEndElementsLeft, (toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " of " + toString(calibratorRoute.getTag())).c_str(), 0, GUIDesignLabelLeftThick);
    myCheckLabel = new FXLabel(beginEndElementsLeft, "", 0, GUIDesignLabelOnlyIcon);

    // create horizontal frame for begin and end text fields
    FXHorizontalFrame* beginEndElementsRight = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myBeginTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGESTART, GUIDesignTextFieldReal);
    myBeginTextField->setText(toString(myCalibratorRoute->getBegin()).c_str());
    myEndTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGEEND, GUIDesignTextFieldReal);
    myEndTextField->setText(toString(myCalibratorRoute->getEnd()).c_str());

    // set interval flag depending if interval exists
    if (myCalibratorDialogParent->findInterval(myCalibratorRoute->getBegin(), myCalibratorRoute->getEnd())) {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        myBeginEndValid = true;
    } else {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        myBeginEndValid = false;
    }

    // Create labels and tables
    FXHorizontalFrame* buttonAndLabelClosingLaneReroute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddCalibratorRoute = new FXButton(buttonAndLabelClosingLaneReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE, GUIDesignButtonIcon);
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
    updateCalibratorRouteValues();
}


GNECalibratorRouteDialog::~GNECalibratorRouteDialog() {
}


long
GNECalibratorRouteDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorRouteValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myCalibratorRoute->getTag()) + " of " + toString(myCalibratorRoute->getCalibratorParent()->getTag())).c_str(), "%s",
                              (toString(myCalibratorRoute->getCalibratorParent()->getTag()) + "'s " + toString(myCalibratorRoute->getTag()) +
                               " cannot be updated because " + toString(myCalibratorRoute->getTag()) + " defined by " + toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " is invalid.").c_str());
        return 0;
    } else {
        // set new calibrator route
        // myCalibratorRoute->set....
        // Stop Modal
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNECalibratorRouteDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorRouteDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // set default values
    // fields->set(---)
    // update tables
    updateCalibratorRouteValues();
    return 1;
}


void
GNECalibratorRouteDialog::updateCalibratorRouteValues() {
    /*
    // clear table
    myClosingLaneRerouteList->clearItems();
    // set number of rows
    myClosingLaneRerouteList->setTableSize(int(myCopyOfCalibratorRoute.size()), 5);
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
    for (GNECalibratorRoute::iterator i = myCopyOfCalibratorRoute.begin(); i != myCopyOfCalibratorRoute.end(); i++) {
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
        if (myCalibratorRouteValid) {
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
