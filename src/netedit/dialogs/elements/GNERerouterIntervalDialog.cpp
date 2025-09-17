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

#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/elements/additional/GNEClosingLaneReroute.h>
#include <netedit/elements/additional/GNEClosingReroute.h>
#include <netedit/elements/additional/GNEDestProbReroute.h>
#include <netedit/elements/additional/GNEParkingAreaReroute.h>
#include <netedit/elements/additional/GNERouteProbReroute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEAttributeCarrierDialog.h"
#include "GNERerouterIntervalDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterIntervalDialog::GNERerouterIntervalDialog(GNEAdditional* rerouterInterval) :
    GNETemplateElementDialog<GNEAdditional>(rerouterInterval, DialogType::REROUTERINTERVAL) {
    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* columnCenter = new FXVerticalFrame(columns, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarVerticalFrame);
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
    // get rerouter parent
    const auto rerouterParent = myElement->getParentAdditionals().at(0);
    // declare strings
    const std::string warningTitle = TLF("Error updating % of % '%'", myElement->getTagStr(), rerouterParent->getTagStr(), rerouterParent->getID());
    const std::string infoA = TLF("% of % '%' cannot be updated because", myElement->getTagStr(), rerouterParent->getTagStr(), rerouterParent->getID());
    std::string infoB;
    // set infoB
    if (myClosingReroutes->getEditedElements().empty() && myClosingLaneReroutes->getEditedElements().empty() &&
            myRouteProbReroutes->getEditedElements().empty() && myDestProbReroutes->getEditedElements().empty() &&
            myParkingAreaReroutes->getEditedElements().empty()) {
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
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(), warningTitle, infoA, infoB);
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNERerouterIntervalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myClosingReroutes->updateList();
    myClosingLaneReroutes->updateList();
    myDestProbReroutes->updateList();
    myRouteProbReroutes->updateList();
    myParkingAreaReroutes->updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::ClosingReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::ClosingReroutesList::ClosingReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog,
        FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterIntervalDialog, contentFrame, SUMO_TAG_CLOSING_REROUTE,
                           GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::DIALOG_VCLASS | GNEElementList::Options::FIXED_HEIGHT) {
    // disable if there are no edges in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getEdges().size() == 0) {
        disableList(TL("No edges in net"));
    }
}


long
GNERerouterIntervalDialog::ClosingReroutesList::addNewElement() {
    // get edge
    const auto edge = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getEdges().begin()->second;
    // create closing reroute
    return insertElement(new GNEClosingReroute(myElementDialogParent->getElement(), edge, SVCAll));
}


long
GNERerouterIntervalDialog::ClosingReroutesList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex)->getParentEdges().front());
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::ClosingLaneReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::ClosingLaneReroutesList::ClosingLaneReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog,
        FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterIntervalDialog, contentFrame, SUMO_TAG_CLOSING_LANE_REROUTE,
                           GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::DIALOG_VCLASS | GNEElementList::Options::FIXED_HEIGHT) {
    // disable if there are no edges in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getLanes().size() == 0) {
        disableList(TL("No lanes in net"));
    }
}


long
GNERerouterIntervalDialog::ClosingLaneReroutesList::addNewElement() {
    // get lane
    const auto lane = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getEdges().begin()->second->getChildLanes().front();
    // create closing lane reroute
    return insertElement(new GNEClosingLaneReroute(myElementDialogParent->getElement(), lane, SVCAll));
}


long
GNERerouterIntervalDialog::ClosingLaneReroutesList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex)->getParentLanes().front());
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::DestProbReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::DestProbReroutesList::DestProbReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog,
        FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterIntervalDialog, contentFrame, SUMO_TAG_DEST_PROB_REROUTE,
                           GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
    // disable if there are no edges in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getEdges().size() == 0) {
        disableList(TL("No edges in net"));
    }
}


long
GNERerouterIntervalDialog::DestProbReroutesList::addNewElement() {
    // get edge
    const auto edge = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getEdges().begin()->second;
    // create dest prob reroute
    return insertElement(new GNEDestProbReroute(myElementDialogParent->getElement(), edge, 1));
}


long
GNERerouterIntervalDialog::DestProbReroutesList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex)->getParentEdges().front());
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::RouteProbReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::RouteProbReroutesList::RouteProbReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog,
        FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterIntervalDialog, contentFrame, SUMO_TAG_ROUTE_PROB_REROUTE,
                           GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
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
GNERerouterIntervalDialog::RouteProbReroutesList::addNewElement() {
    // get route
    const auto route = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second;
    // create route prob reroute
    return insertElement(new GNERouteProbReroute(myElementDialogParent->getElement(), route, 1));
}


long
GNERerouterIntervalDialog::RouteProbReroutesList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex)->getParentDemandElements().front());
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterIntervalDialog::ParkingAreaReroutesList - methods
// ---------------------------------------------------------------------------

GNERerouterIntervalDialog::ParkingAreaReroutesList::ParkingAreaReroutesList(GNERerouterIntervalDialog* rerouterIntervalDialog,
        FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterIntervalDialog, contentFrame, SUMO_TAG_PARKING_AREA_REROUTE,
                           GNEElementList::Options::DIALOG_ELEMENT) {
    // disable if there are no parking areas in net
    if (rerouterIntervalDialog->getElement()->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).size() == 0) {
        disableList(TL("No parkingAreas in net"));
    }
}


long
GNERerouterIntervalDialog::ParkingAreaReroutesList::addNewElement() {
    // get parking area
    const auto parkingArea = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).begin()->second;
    // create parking area reroute
    return insertElement(new GNEParkingAreaReroute(myElementDialogParent->getElement(), parkingArea, 1, 1));
}


long
GNERerouterIntervalDialog::ParkingAreaReroutesList::openElementDialog(const size_t rowIndex) {
// open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex)->getParentAdditionals().back());
    return 1;
}

/****************************************************************************/
