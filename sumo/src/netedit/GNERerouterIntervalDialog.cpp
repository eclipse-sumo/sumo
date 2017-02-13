/****************************************************************************/
/// @file    GNERerouterIntervalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    eb 2017
/// @version $Id$
///
/// Dialog for edit rerouter intervals
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

#include "GNERerouterIntervalDialog.h"
#include "GNERerouterDialog.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERerouterInterval.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterIntervalDialog) GNERerouterIntervalDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE,   GNERerouterIntervalDialog::onCmdAddClosingLaneReroute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE,       GNERerouterIntervalDialog::onCmdAddClosingReroute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE,      GNERerouterIntervalDialog::onCmdAddDestProbReroute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE,     GNERerouterIntervalDialog::onCmdAddRouteProbReroute),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdClickedClosingReroute),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdClickedDestProbReroute),
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdClickedRouteProbReroute),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdEditClosingLaneReroute),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdEditClosingReroute),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdEditDestProbReroute),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdEditRouteProbReroute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_CHANGESTART,              GNERerouterIntervalDialog::onCmdChangeBegin),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REROUTEDIALOG_CHANGEEND,                GNERerouterIntervalDialog::onCmdChangeEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT,           GNERerouterIntervalDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_CANCEL,           GNERerouterIntervalDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONALDIALOG_RESET,            GNERerouterIntervalDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNERerouterIntervalDialog, FXDialogBox, GNERerouterIntervalDialogMap, ARRAYNUMBER(GNERerouterIntervalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterIntervalDialog::GNERerouterIntervalDialog(GNERerouterDialog* rerouterDialog, GNERerouterInterval& rerouterInterval) :
    GNEAdditionalDialog(rerouterInterval.getRerouterParent(), 640, 480),
    myRerouterDialogParent(rerouterDialog),
    myRerouterInterval(&rerouterInterval),
    myBeginEndValid(true),
    myClosingLaneReroutesValid(true),
    myClosingReroutesValid(true),
    myDestProbReroutesValid(true),
    myRouteProbReroutesValid(true) {
    // change default header
    changeAdditionalDialogHeader("Edit " + toString(rerouterInterval.getTag()) + " of " + toString(rerouterInterval.getRerouterParent()->getTag()) +
                                 " '" + rerouterInterval.getRerouterParent()->getID() + "'");

    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // create horizontal frame for begin and end label
    FXHorizontalFrame* beginEndElementsLeft = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(beginEndElementsLeft, (toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " of " + toString(rerouterInterval.getTag())).c_str(), 0, GUIDesignLabelLeftThick);
    myCheckLabel = new FXLabel(beginEndElementsLeft, "", 0, GUIDesignLabelOnlyIcon);

    // create horizontal frame for begin and end text fields
    FXHorizontalFrame* beginEndElementsRight = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myBeginTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGESTART, GUIDesignTextFieldReal);
    myBeginTextField->setText(toString(myRerouterInterval->getBegin()).c_str());
    myEndTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_CHANGEEND, GUIDesignTextFieldReal);
    myEndTextField->setText(toString(myRerouterInterval->getEnd()).c_str());

    // set interval flag depending if interval exists
    if (myRerouterDialogParent->findInterval(myRerouterInterval->getBegin(), myRerouterInterval->getEnd())) {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        myBeginEndValid = true;
    } else {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        myBeginEndValid = false;
    }

    // Create labels and tables
    FXHorizontalFrame* buttonAndLabelClosingLaneReroute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddClosingLaneReroutes = new FXButton(buttonAndLabelClosingLaneReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE, GUIDesignButtonIcon);
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

    // copy Elements
    myCopyOfClosingLaneReroutes = myRerouterInterval->getClosingLaneReroutes();
    myCopyOfClosingReroutes = myRerouterInterval->getClosingReroutes();
    myCopyOfDestProbReroutes = myRerouterInterval->getDestProbReroutes();
    myCopyOfRouteProbReroutes = myRerouterInterval->getRouteProbReroutes();

    // update tables
    updateClosingLaneReroutesTable();
    updateClosingReroutesTable();
    updateDestProbReroutesTable();
    updateRouteProbReroutesTable();
}


GNERerouterIntervalDialog::~GNERerouterIntervalDialog() {
}


long
GNERerouterIntervalDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myBeginEndValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myRerouterInterval->getTag()) + " of " + toString(myRerouterInterval->getRerouterParent()->getTag())).c_str(), "%s",
                              (toString(myRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myRerouterInterval->getTag()) +
                               " cannot be updated because " + toString(myRerouterInterval->getTag()) + " defined by " + toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " is invalid.").c_str());
        return 0;
    } else if (myCopyOfClosingLaneReroutes.empty() && myCopyOfClosingReroutes.empty() && myCopyOfDestProbReroutes.empty() && myCopyOfRouteProbReroutes.empty()) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myRerouterInterval->getTag()) + " of " + toString(myRerouterInterval->getRerouterParent()->getTag())).c_str(), "%s",
                              (toString(myRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myRerouterInterval->getTag()) +
                               " cannot be updated because at least one " + toString(myRerouterInterval->getTag()) + "'s element must be defined.").c_str());
        return 0;
    } else if (myClosingLaneReroutesValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myRerouterInterval->getTag()) + " of " + toString(myRerouterInterval->getRerouterParent()->getTag())).c_str(), "%s",
                              (toString(myRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myRerouterInterval->getTag()) +
                               " cannot be updated because there are invalid " + toString(SUMO_TAG_CLOSING_LANE_REROUTE) + "s.").c_str());
        return 0;
    } else if (myClosingReroutesValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myRerouterInterval->getTag()) + " of " + toString(myRerouterInterval->getRerouterParent()->getTag())).c_str(), "%s",
                              (toString(myRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myRerouterInterval->getTag()) +
                               " cannot be updated because there are invalid " + toString(SUMO_TAG_CLOSING_REROUTE) + "s.").c_str());
        return 0;
    } else if (myDestProbReroutesValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myRerouterInterval->getTag()) + " of " + toString(myRerouterInterval->getRerouterParent()->getTag())).c_str(), "%s",
                              (toString(myRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myRerouterInterval->getTag()) +
                               " cannot be updated because there are invalid " + toString(SUMO_TAG_DEST_PROB_REROUTE) + "s.").c_str());
        return 0;
    } else if (myRouteProbReroutesValid == false) {
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error updating " + toString(myRerouterInterval->getTag()) + " of " + toString(myRerouterInterval->getRerouterParent()->getTag())).c_str(), "%s",
                              (toString(myRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myRerouterInterval->getTag()) +
                               " cannot be updated because there are invalid " + toString(SUMO_TAG_ROUTE_PROB_REROUTE) + "s.").c_str());
        return 0;
    } else {
        // set new values of rerouter interval
        myRerouterInterval->setClosingLaneReroutes(myCopyOfClosingLaneReroutes);
        myRerouterInterval->setClosingReroutes(myCopyOfClosingReroutes);
        myRerouterInterval->setDestProbReroutes(myCopyOfDestProbReroutes);
        myRerouterInterval->setRouteProbReroutes(myCopyOfRouteProbReroutes);
        myRerouterInterval->setBegin(GNEAttributeCarrier::parse<SUMOReal>(myBeginTextField->getText().text()));
        myRerouterInterval->setEnd(GNEAttributeCarrier::parse<SUMOReal>(myEndTextField->getText().text()));

        // Stop Modal
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNERerouterIntervalDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // Copy original intervals again
    myCopyOfClosingLaneReroutes = myRerouterInterval->getClosingLaneReroutes();
    myCopyOfClosingReroutes = myRerouterInterval->getClosingReroutes();
    myCopyOfDestProbReroutes = myRerouterInterval->getDestProbReroutes();
    myCopyOfRouteProbReroutes = myRerouterInterval->getRouteProbReroutes();
    // update tables
    updateClosingLaneReroutesTable();
    updateClosingReroutesTable();
    updateDestProbReroutesTable();
    updateRouteProbReroutesTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddClosingLaneReroute(FXObject*, FXSelector, void*) {
    // add new element and update table
    myCopyOfClosingLaneReroutes.push_back(GNEClosingLaneReroute(*myRerouterInterval, NULL));
    updateClosingLaneReroutesTable();
    onCmdEditClosingLaneReroute(0, 0, 0);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddClosingReroute(FXObject*, FXSelector, void*) {
    // add new element and update table
    myCopyOfClosingReroutes.push_back(GNEClosingReroute(*myRerouterInterval, NULL));
    updateClosingReroutesTable();
    onCmdEditClosingLaneReroute(0, 0, 0);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddDestProbReroute(FXObject*, FXSelector, void*) {
    // add new element and update table
    myCopyOfDestProbReroutes.push_back(GNEDestProbReroute(*myRerouterInterval, NULL, 0));
    updateDestProbReroutesTable();
    onCmdEditDestProbReroute(0, 0, 0);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddRouteProbReroute(FXObject*, FXSelector, void*) {
    // add new element and update table
    myCopyOfRouteProbReroutes.push_back(GNERouteProbReroute(*myRerouterInterval, "", 0));
    updateRouteProbReroutesTable();
    onCmdEditRouteProbReroute(0, 0, 0);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfClosingLaneReroutes.size(); i++) {
        if (myClosingLaneRerouteList->getItem(i, 4)->hasFocus()) {
            myClosingLaneRerouteList->removeRows(i);
            myCopyOfClosingLaneReroutes.erase(myCopyOfClosingLaneReroutes.begin() + i);
            updateClosingLaneReroutesTable();
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedClosingReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfClosingReroutes.size(); i++) {
        if (myClosingRerouteList->getItem(i, 4)->hasFocus()) {
            myClosingRerouteList->removeRows(i);
            myCopyOfClosingReroutes.erase(myCopyOfClosingReroutes.begin() + i);
            updateClosingReroutesTable();
            return 1;
        }
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdClickedDestProbReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfDestProbReroutes.size(); i++) {
        if (myDestProbRerouteList->getItem(i, 3)->hasFocus()) {
            myDestProbRerouteList->removeRows(i);
            myCopyOfDestProbReroutes.erase(myCopyOfDestProbReroutes.begin() + i);
            updateDestProbReroutesTable();
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedRouteProbReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myCopyOfRouteProbReroutes.size(); i++) {
        if (myRouteProbRerouteList->getItem(i, 3)->hasFocus()) {
            myRouteProbRerouteList->removeRows(i);
            myCopyOfRouteProbReroutes.erase(myCopyOfRouteProbReroutes.begin() + i);
            updateRouteProbReroutesTable();
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdEditClosingLaneReroute(FXObject*, FXSelector, void*) {
    myClosingLaneReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myClosingLaneRerouteList->getNumRows(); i++) {
        GNELane* lane = myRerouterInterval->getRerouterParent()->getViewNet()->getNet()->retrieveLane(myClosingLaneRerouteList->getItem(i, 0)->getText().text(), false);
        if (lane == NULL) {
            myClosingLaneReroutesValid = false;
            myClosingLaneRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (canParseVehicleClasses(myClosingLaneRerouteList->getItem(i, 1)->getText().text()) == false) {
            myClosingLaneReroutesValid = false;
            myClosingLaneRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (canParseVehicleClasses(myClosingLaneRerouteList->getItem(i, 2)->getText().text()) == false) {
            myClosingLaneReroutesValid = false;
            myClosingLaneRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            myCopyOfClosingLaneReroutes.at(i).setAllowedVehicles(parseVehicleClasses(myClosingLaneRerouteList->getItem(i, 1)->getText().text()));
            myCopyOfClosingLaneReroutes.at(i).setDisallowedVehicles(parseVehicleClasses(myClosingLaneRerouteList->getItem(i, 2)->getText().text()));
            myCopyOfClosingLaneReroutes.at(i).setClosedLane(lane);
            myClosingLaneRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myClosingLaneRerouteList->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditClosingReroute(FXObject*, FXSelector, void*) {
    myClosingReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myClosingRerouteList->getNumRows(); i++) {
        GNEEdge* edge = myRerouterInterval->getRerouterParent()->getViewNet()->getNet()->retrieveEdge(myClosingRerouteList->getItem(i, 0)->getText().text(), false);
        if (edge == NULL) {
            myClosingReroutesValid = false;
            myClosingRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (canParseVehicleClasses(myClosingRerouteList->getItem(i, 1)->getText().text()) == false) {
            myClosingReroutesValid = false;
            myClosingRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (canParseVehicleClasses(myClosingRerouteList->getItem(i, 2)->getText().text()) == false) {
            myClosingReroutesValid = false;
            myClosingRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            myCopyOfClosingReroutes.at(i).setAllowedVehicles(parseVehicleClasses(myClosingRerouteList->getItem(i, 1)->getText().text()));
            myCopyOfClosingReroutes.at(i).setDisallowedVehicles(parseVehicleClasses(myClosingRerouteList->getItem(i, 2)->getText().text()));
            myCopyOfClosingReroutes.at(i).setClosedEdge(edge);
            myClosingRerouteList->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myClosingRerouteList->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditDestProbReroute(FXObject*, FXSelector, void*) {
    myDestProbReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myDestProbRerouteList->getNumRows(); i++) {
        GNEEdge* edge = myRerouterInterval->getRerouterParent()->getViewNet()->getNet()->retrieveEdge(myDestProbRerouteList->getItem(i, 0)->getText().text(), false);
        SUMOReal probability = -1;
        // try to parse probability
        if (GNEAttributeCarrier::canParse<SUMOReal>(myDestProbRerouteList->getItem(i, 1)->getText().text())) {
            probability = GNEAttributeCarrier::parse<SUMOReal>(myDestProbRerouteList->getItem(i, 1)->getText().text());
        }
        // check if values are valid
        if (edge == NULL) {
            myDestProbReroutesValid = false;
            myDestProbRerouteList->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if ((probability < 0) || (probability > 1)) {
            myDestProbReroutesValid = false;
            myDestProbRerouteList->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            myCopyOfDestProbReroutes.at(i).setNewDestination(edge);
            myCopyOfDestProbReroutes.at(i).setProbability(probability);
            myDestProbRerouteList->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
            myDestProbRerouteList->update();
        }
    }
    // update list
    myDestProbRerouteList->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditRouteProbReroute(FXObject*, FXSelector, void*) {
    myRouteProbReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myRouteProbRerouteList->getNumRows(); i++) {
        SUMOReal probability = -1;
        std::string route = myRouteProbRerouteList->getItem(i, 0)->getText().text();
        // try to parse probability
        if (GNEAttributeCarrier::canParse<SUMOReal>(myRouteProbRerouteList->getItem(i, 1)->getText().text())) {
            probability = GNEAttributeCarrier::parse<SUMOReal>(myRouteProbRerouteList->getItem(i, 1)->getText().text());
        }
        // check if values are valid
        if (route.empty()) {
            myRouteProbReroutesValid = false;
            myRouteProbRerouteList->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if ((probability < 0) || (probability > 1)) {
            myRouteProbReroutesValid = false;
            myRouteProbRerouteList->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            myCopyOfRouteProbReroutes.at(i).setNewRouteId(myRouteProbRerouteList->getItem(i, 0)->getText().text());
            myCopyOfRouteProbReroutes.at(i).setProbability(probability);
            myRouteProbRerouteList->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
            myRouteProbRerouteList->update();
        }
    }
    // update list
    myRouteProbRerouteList->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdChangeBegin(FXObject*, FXSelector, void*) {
    if (GNEAttributeCarrier::canParse<SUMOReal>(myBeginTextField->getText().text()) &&
            myRerouterDialogParent->checkInterval(GNEAttributeCarrier::parse<SUMOReal>(myBeginTextField->getText().text()), myRerouterInterval->getEnd())) {
        myBeginEndValid = true;
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        return 1;
    } else {
        myBeginEndValid = false;
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        return 0;
    }
}


long
GNERerouterIntervalDialog::onCmdChangeEnd(FXObject*, FXSelector, void*) {
    if (GNEAttributeCarrier::canParse<SUMOReal>(myEndTextField->getText().text()) &&
            myRerouterDialogParent->checkInterval(myRerouterInterval->getBegin(), GNEAttributeCarrier::parse<SUMOReal>(myEndTextField->getText().text()))) {
        myBeginEndValid = true;
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        return 1;
    } else {
        myBeginEndValid = false;
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        return 0;
    }
}


void
GNERerouterIntervalDialog::updateClosingLaneReroutesTable() {
    // clear table
    myClosingLaneRerouteList->clearItems();
    // set number of rows
    myClosingLaneRerouteList->setTableSize(int(myCopyOfClosingLaneReroutes.size()), 5);
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
    for (std::vector<GNEClosingLaneReroute>::iterator i = myCopyOfClosingLaneReroutes.begin(); i != myCopyOfClosingLaneReroutes.end(); i++) {
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
        if (myClosingLaneReroutesValid) {
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
}


void
GNERerouterIntervalDialog::updateClosingReroutesTable() {
    // clear table
    myClosingRerouteList->clearItems();
    // set number of rows
    myClosingRerouteList->setTableSize(int(myCopyOfClosingReroutes.size()), 5);
    // Configure list
    myClosingRerouteList->setVisibleColumns(5);
    myClosingRerouteList->setColumnWidth(0, 83);
    myClosingRerouteList->setColumnWidth(1, 83);
    myClosingRerouteList->setColumnWidth(2, 82);
    myClosingRerouteList->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myClosingRerouteList->setColumnWidth(4, GUIDesignTableIconCellWidth);
    myClosingRerouteList->setColumnText(0, toString(SUMO_ATTR_EDGE).c_str());
    myClosingRerouteList->setColumnText(1, toString(SUMO_ATTR_ALLOW).c_str());
    myClosingRerouteList->setColumnText(2, toString(SUMO_ATTR_DISALLOW).c_str());
    myClosingRerouteList->setColumnText(3, "");
    myClosingRerouteList->setColumnText(4, "");
    myClosingRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNEClosingReroute>::iterator i = myCopyOfClosingReroutes.begin(); i != myCopyOfClosingReroutes.end(); i++) {
        // Set closing edge
        if (i->getClosedEdge() != NULL) {
            item = new FXTableItem(i->getClosedEdge()->getID().c_str());
            myClosingRerouteList->setItem(indexRow, 0, item);
        } else {
            item = new FXTableItem("");
            myClosingRerouteList->setItem(indexRow, 0, item);
        }
        // set allow vehicles
        item = new FXTableItem(getVehicleClassNames(i->getAllowedVehicles()).c_str());
        myClosingRerouteList->setItem(indexRow, 1, item);
        // set disallow vehicles
        item = new FXTableItem(getVehicleClassNames(i->getDisallowedVehicles()).c_str());
        myClosingRerouteList->setItem(indexRow, 2, item);
        // set valid
        item = new FXTableItem("");
        if (myClosingReroutesValid) {
            item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        } else {
            item->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        }
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingRerouteList->setItem(indexRow, 3, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingRerouteList->setItem(indexRow, 4, item);
        // Update index
        indexRow++;
    }
}


void
GNERerouterIntervalDialog::updateDestProbReroutesTable() {
    // clear table
    myDestProbRerouteList->clearItems();
    // set number of rows
    myDestProbRerouteList->setTableSize(int(myCopyOfDestProbReroutes.size()), 4);
    // Configure list
    myDestProbRerouteList->setVisibleColumns(4);
    myDestProbRerouteList->setColumnWidth(0, 124);
    myDestProbRerouteList->setColumnWidth(1, 124);
    myDestProbRerouteList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myDestProbRerouteList->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myDestProbRerouteList->setColumnText(0, toString(SUMO_ATTR_EDGE).c_str());
    myDestProbRerouteList->setColumnText(1, toString(SUMO_ATTR_PROB).c_str());
    myDestProbRerouteList->setColumnText(2, "");
    myDestProbRerouteList->setColumnText(3, "");
    myDestProbRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNEDestProbReroute>::iterator i = myCopyOfDestProbReroutes.begin(); i != myCopyOfDestProbReroutes.end(); i++) {
        // Set new destination
        if (i->getNewDestination() != NULL) {
            item = new FXTableItem(i->getNewDestination()->getID().c_str());
            myDestProbRerouteList->setItem(indexRow, 0, item);
        } else {
            item = new FXTableItem("");
            myDestProbRerouteList->setItem(indexRow, 0, item);
        }
        // Set probability
        item = new FXTableItem(toString(i->getProbability()).c_str());
        myDestProbRerouteList->setItem(indexRow, 1, item);
        // set valid
        item = new FXTableItem("");
        if (myDestProbReroutesValid) {
            item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        } else {
            item->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        }
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myDestProbRerouteList->setItem(indexRow, 2, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myDestProbRerouteList->setItem(indexRow, 3, item);
        // Update index
        indexRow++;
    }
}


void
GNERerouterIntervalDialog::updateRouteProbReroutesTable() {
    // clear table
    myRouteProbRerouteList->clearItems();
    // set number of rows
    myRouteProbRerouteList->setTableSize(int(myCopyOfRouteProbReroutes.size()), 4);
    // Configure list
    myRouteProbRerouteList->setVisibleColumns(4);
    myRouteProbRerouteList->setColumnWidth(0, 124);
    myRouteProbRerouteList->setColumnWidth(1, 124);
    myRouteProbRerouteList->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myRouteProbRerouteList->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myRouteProbRerouteList->setColumnText(0, toString(SUMO_ATTR_ROUTE).c_str());
    myRouteProbRerouteList->setColumnText(1, toString(SUMO_ATTR_PROB).c_str());
    myRouteProbRerouteList->setColumnText(2, "");
    myRouteProbRerouteList->setColumnText(3, "");
    myRouteProbRerouteList->getRowHeader()->setWidth(0);
    // Declare index for rows and pointer to FXTableItem
    int indexRow = 0;
    FXTableItem* item = 0;
    // iterate over values
    for (std::vector<GNERouteProbReroute>::iterator i = myCopyOfRouteProbReroutes.begin(); i != myCopyOfRouteProbReroutes.end(); i++) {
        // Set new route
        item = new FXTableItem(i->getNewRouteId().c_str());
        myRouteProbRerouteList->setItem(indexRow, 0, item);
        // Set probability
        item = new FXTableItem(toString(i->getProbability()).c_str());
        myRouteProbRerouteList->setItem(indexRow, 1, item);
        // set valid
        item = new FXTableItem("");
        if (myRouteProbReroutesValid) {
            item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        } else {
            item->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        }
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myRouteProbRerouteList->setItem(indexRow, 2, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myRouteProbRerouteList->setItem(indexRow, 3, item);
        // Update index
        indexRow++;
    }
}


/****************************************************************************/
