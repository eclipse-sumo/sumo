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
    // called when user click over buttons
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_ADD,    GNERerouterIntervalDialog::onCmdElementListAdd),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_SORT,   GNERerouterIntervalDialog::onCmdElementListSort),
    // clicked table (Double and triple clicks allow to remove element more fast)
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_ELEMENTLIST_EDIT,   GNERerouterIntervalDialog::onCmdElementListClick),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_ELEMENTLIST_EDIT,   GNERerouterIntervalDialog::onCmdElementListUpdate),
    // use "update" instead of "command" to avoid problems mit icons
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_REROUTEDIALOG_EDIT_INTERVAL,    GNERerouterIntervalDialog::onCmdChangeBeginEnd),
};

// Object implementation
FXIMPLEMENT(GNERerouterIntervalDialog, GNEElementDialog<GNEAdditional>, GNERerouterIntervalDialogMap, ARRAYNUMBER(GNERerouterIntervalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

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
    myClosingReroutes = new ElementList<GNEAdditional, GNEChange_Additional>(this, columnLeft, SUMO_TAG_CLOSING_REROUTE, myElement->getChildAdditionals(), true);
    // disable if there are no edges in net
    if (rerouterInterval->getNet()->getAttributeCarriers()->getEdges().size() == 0) {
        myClosingReroutes->disableList(TL("No edges in net"));
    }
    // create closing lane reroute element list
    myClosingLaneReroutes = new ElementList<GNEAdditional, GNEChange_Additional>(this, columnLeft, SUMO_TAG_CLOSING_LANE_REROUTE, myElement->getChildAdditionals(), true);
    // disable if there are no edges in net
    if (rerouterInterval->getNet()->getAttributeCarriers()->getLanes().size() == 0) {
        myClosingLaneReroutes->disableList(TL("No lanes in net"));
    }
    // dest prob reroute
    myDestProbReroutes = new ElementList<GNEAdditional, GNEChange_Additional>(this, columnCenter, SUMO_TAG_DEST_PROB_REROUTE, myElement->getChildAdditionals(), true);
    // disable if there are no edges in net
    if (rerouterInterval->getNet()->getAttributeCarriers()->getEdges().size() == 0) {
        myDestProbReroutes->disableList(TL("No edges in net"));
    }
    // route prob reroute
    myRouteProbReroutes = new ElementList<GNEAdditional, GNEChange_Additional>(this, columnCenter, SUMO_TAG_ROUTE_PROB_REROUTE, myElement->getChildAdditionals(), true);
    // disable if the rerouter has multiple edges (random routes can only work from one edge)
    if (rerouterInterval->getParentAdditionals().at(0)->getChildEdges().size() > 1) {
        myRouteProbReroutes->disableList(TL("Rerouter has more than one edge"));
    }
    // disable if there are no routes in net
    if (myElement->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size() == 0) {
        myRouteProbReroutes->disableList(TL("No routes in net"));
    }
    // parking area reroute
    myParkingAreaReroutes = new ElementList<GNEAdditional, GNEChange_Additional>(this, columnRight, SUMO_TAG_PARKING_AREA_REROUTE, myElement->getChildAdditionals(), false);
    // disable if there are no parking areas in net
    if (rerouterInterval->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).size() == 0) {
        myParkingAreaReroutes->disableList(TL("No parkingAreas in net"));
    }
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
    // nothing to do
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
    } else if (myClosingReroutes->getEditedElements().empty() && myClosingLaneReroutes->getEditedElements().empty() &&
               myRouteProbReroutes->getEditedElements().empty() && myDestProbReroutes->getEditedElements().empty() &&
               myParkingAreaReroutes->getEditedElements().empty()) {
        infoB = TLF("at least one % must be defined.", myElement->getTagStr());
    } else if (!myClosingReroutes->isValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_CLOSING_REROUTE));
    } else if (!myClosingLaneReroutes->isValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_CLOSING_LANE_REROUTE));
    } else if (!myRouteProbReroutes->isValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_ROUTE_PROB_REROUTE));
    } else if (!myDestProbReroutes->isValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_DEST_PROB_REROUTE));
    } else if (!myParkingAreaReroutes->isValid()) {
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
    myClosingReroutes->refreshList();
    myClosingLaneReroutes->refreshList();
    myDestProbReroutes->refreshList();
    myRouteProbReroutes->refreshList();
    myParkingAreaReroutes->refreshList();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdElementListAdd(FXObject* obj, FXSelector, void*) {
    // create new element depending of the elementList
    if (myClosingReroutes->checkObject(obj)) {
        // get edge
        const auto edge = myElement->getNet()->getAttributeCarriers()->getEdges().begin()->second;
        // create closing reroute
        return myClosingReroutes->addElement(new GNEClosingReroute(myElement, edge, SVCAll));
    } else if (myClosingLaneReroutes->checkObject(obj)) {
        // get lane
        const auto lane = myElement->getNet()->getAttributeCarriers()->getEdges().begin()->second->getChildLanes().front();
        // create closing lane reroute
        return myClosingLaneReroutes->addElement(new GNEClosingLaneReroute(myElement, lane, SVCAll));
    } else if (myDestProbReroutes->checkObject(obj)) {
        // get edge
        const auto edge = myElement->getNet()->getAttributeCarriers()->getEdges().begin()->second;
        // create dest prob reroute
        return myDestProbReroutes->addElement(new GNEDestProbReroute(myElement, edge, 1));
    } else if (myRouteProbReroutes->checkObject(obj)) {
        // get route
        const auto route = myElement->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second;
        // create route prob reroute
        return myRouteProbReroutes->addElement(new GNERouteProbReroute(myElement, route, 1));
    } else if (myParkingAreaReroutes->checkObject(obj)) {
        // get parking area
        const auto parkingArea = myElement->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).begin()->second;
        // create parking area reroute
        return myParkingAreaReroutes->addElement(new GNEParkingAreaReroute(myElement, parkingArea, 1, 1));
    } else {
        throw ProcessError("Invalid object in GNERerouterIntervalDialog::onCmdElementListEdit");
    }
}


long
GNERerouterIntervalDialog::onCmdElementListSort(FXObject* obj, FXSelector, void*) {
    // continue depending of the elementList
    if (myClosingReroutes->checkObject(obj)) {
        return myClosingReroutes->sortElements();
    } else if (myClosingLaneReroutes->checkObject(obj)) {
        return myClosingLaneReroutes->sortElements();
    } else if (myDestProbReroutes->checkObject(obj)) {
        return myDestProbReroutes->sortElements();
    } else if (myRouteProbReroutes->checkObject(obj)) {
        return myRouteProbReroutes->sortElements();
    } else if (myParkingAreaReroutes->checkObject(obj)) {
        return myParkingAreaReroutes->sortElements();
    } else {
        throw ProcessError("Invalid object in GNERerouterIntervalDialog::onCmdElementListEdit");
    }
}


long
GNERerouterIntervalDialog::onCmdElementListClick(FXObject* obj, FXSelector sel, void* ptr) {
    // continue depending of the elementList
    if (myClosingReroutes->checkObject(obj)) {
        return myClosingReroutes->onCmdClickedList(obj, sel, ptr);
    } else if (myClosingLaneReroutes->checkObject(obj)) {
        return myClosingLaneReroutes->onCmdClickedList(obj, sel, ptr);
    } else if (myDestProbReroutes->checkObject(obj)) {
        return myDestProbReroutes->onCmdClickedList(obj, sel, ptr);
    } else if (myRouteProbReroutes->checkObject(obj)) {
        return myRouteProbReroutes->onCmdClickedList(obj, sel, ptr);
    } else if (myParkingAreaReroutes->checkObject(obj)) {
        return myParkingAreaReroutes->onCmdClickedList(obj, sel, ptr);
    } else {
        throw ProcessError("Invalid object in GNERerouterIntervalDialog::onCmdElementListEdit");
    }
}


long
GNERerouterIntervalDialog::onCmdElementListUpdate(FXObject* obj, FXSelector sel, void* ptr) {
    // continue depending of the elementList
    if (myClosingReroutes->checkObject(obj)) {
        return myClosingReroutes->onCmdUpdateList(obj, sel, ptr);
    } else if (myClosingLaneReroutes->checkObject(obj)) {
        return myClosingLaneReroutes->onCmdUpdateList(obj, sel, ptr);
    } else if (myDestProbReroutes->checkObject(obj)) {
        return myDestProbReroutes->onCmdUpdateList(obj, sel, ptr);
    } else if (myRouteProbReroutes->checkObject(obj)) {
        return myRouteProbReroutes->onCmdUpdateList(obj, sel, ptr);
    } else if (myParkingAreaReroutes->checkObject(obj)) {
        return myParkingAreaReroutes->onCmdUpdateList(obj, sel, ptr);
    } else {
        throw ProcessError("Invalid object in GNERerouterIntervalDialog::onCmdElementListEdit");
    }
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

/****************************************************************************/
