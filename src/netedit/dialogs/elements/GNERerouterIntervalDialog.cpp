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
/// @file    GNERerouterIntervalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    eb 2017
///
// Dialog for edit rerouter intervals
/****************************************************************************/

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/elements/additional/GNEClosingLaneReroute.h>
#include <netedit/elements/additional/GNEClosingReroute.h>
#include <netedit/elements/additional/GNEDestProbReroute.h>
#include <netedit/elements/additional/GNEParkingAreaReroute.h>
#include <netedit/elements/additional/GNERouteProbReroute.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNERerouterIntervalDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterIntervalDialog) GNERerouterIntervalDialogMap[] = {
    // use "update" instead of "command" to avoid problems mit icons
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_REROUTEDIALOG_EDIT_INTERVAL,    GNERerouterIntervalDialog::onCmdChangeBeginEnd),
};

// Object implementation
FXIMPLEMENT(GNERerouterIntervalDialog, GNEElementDialog<GNEAdditional>, GNERerouterIntervalDialogMap, ARRAYNUMBER(GNERerouterIntervalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::GNERerouterIntervalDialog(GNEAdditional* rerouterInterval, const bool updatingElement) :
    GNEElementDialog<GNEAdditional>(rerouterInterval, updatingElement) {
    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnCenter = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    // create horizontal frame for begin and end label
    FXHorizontalFrame* beginEndElementsLeft = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(beginEndElementsLeft, (toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " of " + myElement->getTagStr()).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myCheckLabel = new FXLabel(beginEndElementsLeft, "", GUIIconSubSys::getIcon(GUIIcon::CORRECT), GUIDesignLabelIcon32x32Thicked);
    // create horizontal frame for begin and end text fields
    FXHorizontalFrame* beginEndElementsRight = new FXHorizontalFrame(columnCenter, GUIDesignAuxiliarHorizontalFrame);
    myBeginTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_EDIT_INTERVAL, GUIDesignTextField);
    myBeginTextField->setText(toString(myElement->getAttribute(SUMO_ATTR_BEGIN)).c_str());
    myEndTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_EDIT_INTERVAL, GUIDesignTextField);
    myEndTextField->setText(toString(myElement->getAttribute(SUMO_ATTR_END)).c_str());
    // create closing reroute element list
    myClosingReroutes = new ClosingReroutesList(this, columnLeft);
    // create closing lane reroute element list
    myClosingLaneReroutes = new ClosingLaneReroutesList(this, columnLeft);
    // dest prob reroute
    myDestProbReroutes = new DestProbReroutesList(this, columnCenter);
    // route prob reroute
    myRouteProbReroutes = new RouteProbReroutesList(this, columnCenter);
    // parking area reroute
    myParkingAreaReroutes = new ParkingAreaReroutesList(this, columnRight);
    // add element if we aren't updating an existent element
    if (!myUpdatingElement) {
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myElement, true), true);
    }
    // open dialog
    openDialog();
}


GNERerouterIntervalDialog::~GNERerouterIntervalDialog() {}


void
GNERerouterIntervalDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do (yet)
}


long
GNERerouterIntervalDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // declare strings
    std::string title;
    std::string infoA;
    std::string infoB;
    // get rerouter parent
    const auto rerouterParent = myElement->getParentAdditionals().at(0);
    // set title
    if (myUpdatingElement) {
        title = TLF("Error updating % of % '%'", myElement->getTagStr(), rerouterParent->getTagStr(), rerouterParent->getID());
    } else {
        title = TLF("Error creating % of % '%'", myElement->getTagStr(), rerouterParent->getTagStr(), rerouterParent->getID());
    }
    // set infoA
    if (myUpdatingElement) {
        infoA = TLF("% of % '%' cannot be updated because", myElement->getTagStr(), rerouterParent->getTagStr(), rerouterParent->getID());
    } else {
        infoA = TLF("% of % '%' cannot be created because", myElement->getTagStr(), rerouterParent->getTagStr(), rerouterParent->getID());
    }
    // set infoB
    if (!myBeginEndValid) {
        infoB = TLF("% defined by % and % is invalid.", myElement->getTagStr(), toString(SUMO_ATTR_BEGIN), toString(SUMO_ATTR_END));
    } else if (myClosingReroutes->getEditedAdditionals().empty() && myClosingLaneReroutes->getEditedAdditionals().empty() &&
               myRouteProbReroutes->getEditedAdditionals().empty() && myDestProbReroutes->getEditedAdditionals().empty() &&
               myParkingAreaReroutes->getEditedAdditionals().empty()) {
        infoB = TLF("at least one % must be defined.", myElement->getTagStr());
    } else if (!myClosingReroutes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_CLOSING_REROUTE));
    } else if (!myClosingLaneReroutes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_CLOSING_LANE_REROUTE));
    } else if (!myRouteProbReroutes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_ROUTE_PROB_REROUTE));
    } else if (!myDestProbReroutes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_DEST_PROB_REROUTE));
    } else if (!myParkingAreaReroutes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_PARKING_AREA_REROUTE));
    }
    // continue depending of info
    if (infoB.size() > 0) {
        // open question dialog box with two lines
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(), title, infoA, infoB);
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // Stop Modal
        closeDialogAccepting();
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    return closeDialogCanceling();
}


long
GNERerouterIntervalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myClosingReroutes->updateTable();
    myClosingLaneReroutes->updateTable();
    myDestProbReroutes->updateTable();
    myRouteProbReroutes->updateTable();
    myParkingAreaReroutes->updateTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdChangeBeginEnd(FXObject*, FXSelector, void*) {
    const auto begin = GNEAttributeCarrier::canParse<SUMOTime>(myBeginTextField->getText().text()) ? GNEAttributeCarrier::parse<SUMOTime>(myBeginTextField->getText().text()) : -1;
    const auto end = GNEAttributeCarrier::canParse<SUMOTime>(myEndTextField->getText().text()) ? GNEAttributeCarrier::parse<SUMOTime>(myEndTextField->getText().text()) : -1;
    // check that both begin and end are positive, and begin <= end
    myBeginEndValid = (begin >= 0) && (end >= 0) && (begin <= end);
    if (myBeginEndValid) {
        // set new values in rerouter interval
        myElement->setAttribute(SUMO_ATTR_BEGIN, myBeginTextField->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
        myElement->setAttribute(SUMO_ATTR_END, myEndTextField->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
        // change icon
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
    } else {
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::ClosingReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::ClosingReroutesList::ClosingReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame) :
    GNEAdditionalList(rerouterIntervalDialog, contentFrame, SUMO_TAG_CLOSING_REROUTE, true) {
    // disable if there are no edges in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getEdges().size() == 0) {
        disableList(TL("No edges in net"));
    }
}


long
GNERerouterIntervalDialog::ClosingReroutesList::addRow() {
    // get edge
    const auto edge = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getEdges().begin()->second;
    // create closing reroute
    myEditedAdditionalElements.push_back(new GNEClosingReroute(myElementDialogParent->getElement(), edge, SVCAll));
    // update table
    return updateTable();
}


long
GNERerouterIntervalDialog::ClosingReroutesList::openDialog(const size_t rowIndex) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::ClosingLaneReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::ClosingLaneReroutesList::ClosingLaneReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame) :
    GNEAdditionalList(rerouterIntervalDialog, contentFrame, SUMO_TAG_CLOSING_LANE_REROUTE, true) {
    // disable if there are no edges in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getLanes().size() == 0) {
        disableList(TL("No lanes in net"));
    }
}


long
GNERerouterIntervalDialog::ClosingLaneReroutesList::addRow() {
    // get lane
    const auto lane = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getEdges().begin()->second->getChildLanes().front();
    // create closing lane reroute
    myEditedAdditionalElements.push_back(new GNEClosingLaneReroute(myElementDialogParent->getElement(), lane, SVCAll));
    // update table
    return updateTable();
}


long
GNERerouterIntervalDialog::ClosingLaneReroutesList::openDialog(const size_t rowIndex) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::DestProbReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::DestProbReroutesList::DestProbReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame) :
    GNEAdditionalList(rerouterIntervalDialog, contentFrame, SUMO_TAG_DEST_PROB_REROUTE, true) {
    // disable if there are no edges in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getEdges().size() == 0) {
        disableList(TL("No edges in net"));
    }
}


long
GNERerouterIntervalDialog::DestProbReroutesList::addRow() {
    // get edge
    const auto edge = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getEdges().begin()->second;
    // create dest prob reroute
    myEditedAdditionalElements.push_back(new GNEDestProbReroute(myElementDialogParent->getElement(), edge, 1));
    // update table
    return updateTable();
}


long
GNERerouterIntervalDialog::DestProbReroutesList::openDialog(const size_t rowIndex) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::RouteProbReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::RouteProbReroutesList::RouteProbReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame) :
    GNEAdditionalList(rerouterIntervalDialog, contentFrame, SUMO_TAG_ROUTE_PROB_REROUTE, true) {
    // disable if the rerouter has multiple edges (random routes can only work from one edge)
    if (rerouterIntervalDialog->getElement()->getParentAdditionals().at(0)->getChildEdges().size() > 1) {
        disableList(TL("Rerouter has more than one edge"));
    }
    // disable if there are no routes in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size() == 0) {
        disableList(TL("No routes in net"));
    }
}


long
GNERerouterIntervalDialog::RouteProbReroutesList::addRow() {
    // get route
    const auto route = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second;
    // create route prob reroute
    myEditedAdditionalElements.push_back(new GNERouteProbReroute(myElementDialogParent->getElement(), route, 1));
    // update table
    return updateTable();
}


long
GNERerouterIntervalDialog::RouteProbReroutesList::openDialog(const size_t rowIndex) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::ParkingAreaReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::ParkingAreaReroutesList::ParkingAreaReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog, FXVerticalFrame* contentFrame) :
    GNEAdditionalList(rerouterIntervalDialog, contentFrame, SUMO_TAG_PARKING_AREA_REROUTE, false) {
    // disable if there are no parking areas in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).size() == 0) {
        disableList(TL("No parkingAreas in net"));
    }
}


long
GNERerouterIntervalDialog::ParkingAreaReroutesList::addRow() {
    // get parking area
    const auto parkingArea = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).begin()->second;
    // create parking area reroute
    myEditedAdditionalElements.push_back(new GNEParkingAreaReroute(myElementDialogParent->getElement(), parkingArea, 1, 1));
    // update table
    return updateTable();
}


long
GNERerouterIntervalDialog::ParkingAreaReroutesList::openDialog(const size_t rowIndex) {
    return 1;
}

/****************************************************************************/
