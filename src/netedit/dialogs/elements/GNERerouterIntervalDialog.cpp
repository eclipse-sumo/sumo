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
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE,   GNERerouterIntervalDialog::onCmdAddClosingLaneReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE,       GNERerouterIntervalDialog::onCmdAddClosingReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE,      GNERerouterIntervalDialog::onCmdAddDestProbReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE,     GNERerouterIntervalDialog::onCmdAddRouteProbReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_PARKINGAREAREROUTE,   GNERerouterIntervalDialog::onCmdAddParkingAreaReroute),

    // clicked table (Double and triple clicks allow to remove element more fast)
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdClickedClosingReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdClickedClosingReroute),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdClickedClosingReroute),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdClickedDestProbReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdClickedDestProbReroute),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdClickedDestProbReroute),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdClickedRouteProbReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdClickedRouteProbReroute),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdClickedRouteProbReroute),
    FXMAPFUNC(SEL_CLICKED,          MID_GNE_REROUTEDIALOG_TABLE_PARKINGAREAREROUTE, GNERerouterIntervalDialog::onCmdClickedParkingAreaReroute),
    FXMAPFUNC(SEL_DOUBLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_PARKINGAREAREROUTE, GNERerouterIntervalDialog::onCmdClickedParkingAreaReroute),
    FXMAPFUNC(SEL_TRIPLECLICKED,    MID_GNE_REROUTEDIALOG_TABLE_PARKINGAREAREROUTE, GNERerouterIntervalDialog::onCmdClickedParkingAreaReroute),

    // use "update" instead of "command" to avoid problems mit icons
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdEditClosingLaneReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdEditClosingReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdEditDestProbReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdEditRouteProbReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_PARKINGAREAREROUTE, GNERerouterIntervalDialog::onCmdEditParkingAreaReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_EDIT_INTERVAL,            GNERerouterIntervalDialog::onCmdChangeBeginEnd),
};

// Object implementation
FXIMPLEMENT(GNERerouterIntervalDialog, GNEElementDialog<GNEAdditional>, GNERerouterIntervalDialogMap, ARRAYNUMBER(GNERerouterIntervalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterIntervalDialog::GNERerouterIntervalDialog(GNEAdditional* rerouterInterval, bool updatingElement) :
    GNEElementDialog<GNEAdditional>(rerouterInterval, updatingElement, 960, 480) {
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
    // get pointer to database
    const auto* tagPropertiesDatabase = myElement->getNet()->getViewNet()->getNet()->getTagPropertiesDatabase();
    // closing reroute
    myClosingReroutes = new ElementList<GNEAdditional>(this, columnLeft, SUMO_TAG_CLOSING_REROUTE,
            MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE, MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE);
    // closing lane reroute
    myClosingLaneReroutes = new ElementList<GNEAdditional>(this, columnLeft, SUMO_TAG_CLOSING_LANE_REROUTE,
            MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE, MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE);
    // dest prob reroute
    myDestProbReroutes = new ElementList<GNEAdditional>(this, columnCenter, SUMO_TAG_DEST_PROB_REROUTE,
            MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE, MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE);
    // route prob reroute
    myRouteProbReroutes = new ElementList<GNEAdditional>(this, columnCenter, SUMO_TAG_ROUTE_PROB_REROUTE,
            MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE, MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE);
    // parking area reroute
    myParkingAreaReroutes = new ElementList<GNEAdditional>(this, columnRight, SUMO_TAG_PARKING_AREA_REROUTE,
            MID_GNE_REROUTEDIALOG_ADD_PARKINGAREAREROUTE, MID_GNE_REROUTEDIALOG_TABLE_PARKINGAREAREROUTE);
    // disable add parkingAreaReroute Button and change label if there isn't parkingAreas in net
    if (rerouterInterval->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).size() == 0) {
        myParkingAreaReroutes->disableTable(TL("No parkingAreas in net"));
    }
    // disable add routeProbReroute Button and change label if the rerouter has multiple edges (random routes can only work from one edge)
    // for whatever reason, sonar complains in the next line that parkingAreaRerouteLabel may leak, but fox does the cleanup
    if (rerouterInterval->getParentAdditionals().at(0)->getChildEdges().size() > 1) {  // NOSONAR
        myRouteProbReroutes->disableTable(TL("Rerouter has more than one edge"));
    }
    // fill tables
    for (const auto& child : myElement->getChildAdditionals()) {
        if (child->getTagProperty()->getTag() == SUMO_TAG_CLOSING_REROUTE) {
            myClosingReroutes->addElement(child, false);
        } else if (child->getTagProperty()->getTag() == SUMO_TAG_CLOSING_LANE_REROUTE) {
            myClosingLaneReroutes->addElement(child, false);
        } else if (child->getTagProperty()->getTag() == SUMO_TAG_DEST_PROB_REROUTE) {
            myDestProbReroutes->addElement(child, false);
        } else if (child->getTagProperty()->getTag() == SUMO_TAG_ROUTE_PROB_REROUTE) {
            myRouteProbReroutes->addElement(child, false);
        } else if (child->getTagProperty()->getTag() == SUMO_TAG_PARKING_AREA_REROUTE) {
            myParkingAreaReroutes->addElement(child, false);
        }
    }
    // update again all tables
    myClosingReroutes->updateList();
    myClosingLaneReroutes->updateList();
    myDestProbReroutes->updateList();
    myRouteProbReroutes->updateList();
    myParkingAreaReroutes->updateList();
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
    // set title
    if (myUpdatingElement) {
        title = TLF("Error updating % of %", myElement->getTagStr(), myElement->getParentAdditionals().at(0)->getTagStr());
    } else {
        title = TLF("Error creating % of %", myElement->getTagStr(), myElement->getParentAdditionals().at(0)->getTagStr());
    }
    // set infoA
    if (myUpdatingElement) {
        infoA = TLF("%'s % cannot be updated because", myElement->getParentAdditionals().at(0)->getTagStr(), myElement->getTagStr());
    } else {
        infoA = TLF("%'s % cannot be created because", myElement->getParentAdditionals().at(0)->getTagStr(), myElement->getTagStr());
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
    myClosingReroutes->updateList();
    myClosingLaneReroutes->updateList();
    myDestProbReroutes->updateList();
    myRouteProbReroutes->updateList();
    myParkingAreaReroutes->updateList();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddClosingLaneReroute(FXObject*, FXSelector, void*) {
    // first check if there is lanes in the network
    if (myElement->getNet()->getAttributeCarriers()->getEdges().size() > 0) {
        // get lane
        GNELane* lane = myElement->getNet()->getAttributeCarriers()->getEdges().begin()->second->getChildLanes().front();
        // create closing lane reroute
        GNEClosingLaneReroute* closingLaneReroute = new GNEClosingLaneReroute(myElement, lane, SVCAll);
        // add it using undoList
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
        // add it in table
        myClosingLaneReroutes->addElement(closingLaneReroute, true);
    } else {
        WRITE_WARNING(TL("There are no lanes in the network"));
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddClosingReroute(FXObject*, FXSelector, void*) {
    // first check if there is lanes in the network
    if (myElement->getNet()->getAttributeCarriers()->getEdges().size() > 0) {
        // get edge
        GNEEdge* edge = myElement->getNet()->getAttributeCarriers()->getEdges().begin()->second;
        // create closing reroute
        GNEClosingReroute* closingReroute = new GNEClosingReroute(myElement, edge, SVCAll);
        // add it using undoList
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(closingReroute, true), true);
        // add it in table
        myClosingReroutes->addElement(closingReroute, true);
    } else {
        WRITE_WARNING(TL("There are no edges in the network"));
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddDestProbReroute(FXObject*, FXSelector, void*) {
    // first check if there is lanes in the network
    if (myElement->getNet()->getAttributeCarriers()->getEdges().size() > 0) {
        // get edge
        GNEEdge* edge = myElement->getNet()->getAttributeCarriers()->getEdges().begin()->second;
        // create closing reroute and add it to table
        GNEDestProbReroute* destProbReroute = new GNEDestProbReroute(myElement, edge, 1);
        // add it using undoList
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(destProbReroute, true), true);
        // add it in table
        myDestProbReroutes->addElement(destProbReroute, true);
    } else {
        WRITE_WARNING(TL("There are no edges in the network"));
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddRouteProbReroute(FXObject*, FXSelector, void*) {
    // get routes
    const auto& routes = myElement->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE);
    // check if there is at least one route
    if (routes.size() > 0) {
        // create route Prob Reroute
        GNERouteProbReroute* routeProbReroute = new GNERouteProbReroute(myElement, routes.begin()->second, 1);
        // add it using undoList
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(routeProbReroute, true), true);
        // add it in table
        myRouteProbReroutes->addElement(routeProbReroute, true);
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddParkingAreaReroute(FXObject*, FXSelector, void*) {
    // first check if there is lanes in the network
    if (myElement->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).size() > 0) {
        // get parking area
        GNEAdditional* parkingArea = myElement->getNet()->getAttributeCarriers()->getAdditionals().at(SUMO_TAG_PARKING_AREA).begin()->second;
        // create parkingAreaReroute and add it to table
        GNEParkingAreaReroute* parkingAreaReroute = new GNEParkingAreaReroute(myElement, parkingArea, 1, 1);
        // add it using undoList
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(parkingAreaReroute, true), true);
        // add it in table
        myParkingAreaReroutes->addElement(parkingAreaReroute, true);
    } else {
        WRITE_WARNING(TL("There are no parking areas in the network"));
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myClosingLaneReroutes->getEditedElements().size(); i++) {
        if (myClosingLaneReroutes->getItem(i, 4)->hasFocus()) {
            myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myClosingLaneReroutes->getEditedElements().at(i), false), true);
            myClosingLaneReroutes->removeElement(i);
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedClosingReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myClosingReroutes->getEditedElements().size(); i++) {
        if (myClosingReroutes->getItem(i, 4)->hasFocus()) {
            myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myClosingReroutes->getEditedElements().at(i), false), true);
            myClosingReroutes->removeElement(i);
            return 1;
        }
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdClickedDestProbReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myDestProbReroutes->getEditedElements().size(); i++) {
        if (myDestProbReroutes->getItem(i, 3)->hasFocus()) {
            myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myDestProbReroutes->getEditedElements().at(i), false), true);
            myDestProbReroutes->removeElement(i);
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedRouteProbReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myRouteProbReroutes->getEditedElements().size(); i++) {
        if (myRouteProbReroutes->getItem(i, 3)->hasFocus()) {
            myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myRouteProbReroutes->getEditedElements().at(i), false), true);
            myRouteProbReroutes->removeElement(i);
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedParkingAreaReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myParkingAreaReroutes->getEditedElements().size(); i++) {
        if (myParkingAreaReroutes->getItem(i, 4)->hasFocus()) {
            myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myParkingAreaReroutes->getEditedElements().at(i), false), true);
            myParkingAreaReroutes->removeElement(i);
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdEditClosingLaneReroute(FXObject*, FXSelector, void*) {
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myClosingLaneReroutes->getNumRows(); i++) {
        GNEAdditional* closingLaneReroute = myClosingLaneReroutes->getEditedElements().at(i);
        if (!SUMOXMLDefinitions::isValidNetID(myClosingLaneReroutes->getItem(i, 0)->getText().text())) {
            myClosingLaneReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (closingLaneReroute->isValid(SUMO_ATTR_LANE, myClosingLaneReroutes->getItem(i, 0)->getText().text()) == false) {
            myClosingLaneReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (closingLaneReroute->isValid(SUMO_ATTR_ALLOW, myClosingLaneReroutes->getItem(i, 1)->getText().text()) == false) {
            myClosingLaneReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (closingLaneReroute->isValid(SUMO_ATTR_DISALLOW, myClosingLaneReroutes->getItem(i, 2)->getText().text()) == false) {
            myClosingLaneReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else {
            // check if allow/disallow should be changed
            bool changeAllow = myClosingLaneReroutes->getItem(i, 1)->getText().text() != closingLaneReroute->getAttribute(SUMO_ATTR_ALLOW);
            bool changeDisallow = myClosingLaneReroutes->getItem(i, 2)->getText().text() != closingLaneReroute->getAttribute(SUMO_ATTR_DISALLOW);
            // set new values in Closing  reroute
            closingLaneReroute->setAttribute(SUMO_ATTR_LANE, myClosingLaneReroutes->getItem(i, 0)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            if (changeAllow) {
                closingLaneReroute->setAttribute(SUMO_ATTR_ALLOW, myClosingLaneReroutes->getItem(i, 1)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
                myClosingLaneReroutes->getItem(i, 2)->setText(closingLaneReroute->getAttribute(SUMO_ATTR_DISALLOW).c_str());

            }
            if (changeDisallow) {
                closingLaneReroute->setAttribute(SUMO_ATTR_DISALLOW, myClosingLaneReroutes->getItem(i, 2)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
                myClosingLaneReroutes->getItem(i, 1)->setText(closingLaneReroute->getAttribute(SUMO_ATTR_ALLOW).c_str());
            }
            // set Correct label
            myClosingLaneReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        }
    }
    // update list
    myClosingLaneReroutes->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditClosingReroute(FXObject*, FXSelector, void*) {
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myClosingReroutes->getNumRows(); i++) {
        GNEAdditional* closingReroute = myClosingReroutes->getEditedElements().at(i);
        if (!SUMOXMLDefinitions::isValidNetID(myClosingReroutes->getItem(i, 0)->getText().text())) {
            myClosingReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (closingReroute->isValid(SUMO_ATTR_EDGE, myClosingReroutes->getItem(i, 0)->getText().text()) == false) {
            myClosingReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (closingReroute->isValid(SUMO_ATTR_ALLOW, myClosingReroutes->getItem(i, 1)->getText().text()) == false) {
            myClosingReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (closingReroute->isValid(SUMO_ATTR_DISALLOW, myClosingReroutes->getItem(i, 2)->getText().text()) == false) {
            myClosingReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else {
            // check if allow/disallow should be changed
            bool changeAllow = myClosingReroutes->getItem(i, 1)->getText().text() != closingReroute->getAttribute(SUMO_ATTR_ALLOW);
            bool changeDisallow = myClosingReroutes->getItem(i, 2)->getText().text() != closingReroute->getAttribute(SUMO_ATTR_DISALLOW);
            // set new values in Closing  reroute
            closingReroute->setAttribute(SUMO_ATTR_EDGE, myClosingReroutes->getItem(i, 0)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            if (changeAllow) {
                closingReroute->setAttribute(SUMO_ATTR_ALLOW, myClosingReroutes->getItem(i, 1)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
                myClosingReroutes->getItem(i, 2)->setText(closingReroute->getAttribute(SUMO_ATTR_DISALLOW).c_str());

            }
            if (changeDisallow) {
                closingReroute->setAttribute(SUMO_ATTR_DISALLOW, myClosingReroutes->getItem(i, 2)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
                myClosingReroutes->getItem(i, 1)->setText(closingReroute->getAttribute(SUMO_ATTR_ALLOW).c_str());
            }
            // set Correct label
            myClosingReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        }
    }
    // update list
    myClosingReroutes->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditDestProbReroute(FXObject*, FXSelector, void*) {
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myDestProbReroutes->getNumRows(); i++) {
        GNEAdditional* destProbReroute = myDestProbReroutes->getEditedElements().at(i);
        if (!SUMOXMLDefinitions::isValidNetID(myDestProbReroutes->getItem(i, 0)->getText().text())) {
            myDestProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (destProbReroute->isValid(SUMO_ATTR_EDGE, myDestProbReroutes->getItem(i, 0)->getText().text()) == false) {
            myDestProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (destProbReroute->isValid(SUMO_ATTR_PROB, myDestProbReroutes->getItem(i, 1)->getText().text()) == false) {
            myDestProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else {
            // set new values in Closing  reroute
            destProbReroute->setAttribute(SUMO_ATTR_EDGE, myDestProbReroutes->getItem(i, 0)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            destProbReroute->setAttribute(SUMO_ATTR_PROB, myDestProbReroutes->getItem(i, 1)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            // set Correct label
            myDestProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        }
    }
    // update list
    myDestProbReroutes->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditRouteProbReroute(FXObject*, FXSelector, void*) {
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myRouteProbReroutes->getNumRows(); i++) {
        GNEAdditional* routeProbReroute = myRouteProbReroutes->getEditedElements().at(i);
        if (!SUMOXMLDefinitions::isValidNetID(myRouteProbReroutes->getItem(i, 0)->getText().text())) {
            myRouteProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (routeProbReroute->isValid(SUMO_ATTR_PROB, myRouteProbReroutes->getItem(i, 1)->getText().text()) == false) {
            myRouteProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else {
            // set new values in Closing  reroute
            routeProbReroute->setAttribute(SUMO_ATTR_ROUTE, myRouteProbReroutes->getItem(i, 0)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            routeProbReroute->setAttribute(SUMO_ATTR_PROB, myRouteProbReroutes->getItem(i, 1)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            // set Correct label
            myRouteProbReroutes->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        }
    }
    // update list
    myRouteProbReroutes->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditParkingAreaReroute(FXObject*, FXSelector, void*) {
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myParkingAreaReroutes->getNumRows(); i++) {
        GNEAdditional* parkingAreaReroute = myParkingAreaReroutes->getEditedElements().at(i);
        if (parkingAreaReroute->isValid(SUMO_ATTR_PARKING, myParkingAreaReroutes->getItem(i, 0)->getText().text()) == false) {
            myParkingAreaReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (parkingAreaReroute->isValid(SUMO_ATTR_PROB, myParkingAreaReroutes->getItem(i, 1)->getText().text()) == false) {
            myParkingAreaReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else if (parkingAreaReroute->isValid(SUMO_ATTR_VISIBLE, myParkingAreaReroutes->getItem(i, 2)->getText().text()) == false) {
            myParkingAreaReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
        } else {
            // set new values in Closing  reroute
            parkingAreaReroute->setAttribute(SUMO_ATTR_PARKING, myParkingAreaReroutes->getItem(i, 0)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            parkingAreaReroute->setAttribute(SUMO_ATTR_PROB, myParkingAreaReroutes->getItem(i, 1)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            parkingAreaReroute->setAttribute(SUMO_ATTR_VISIBLE, myParkingAreaReroutes->getItem(i, 2)->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
            // set Correct label
            myParkingAreaReroutes->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
        }
    }
    // update list
    myParkingAreaReroutes->update();
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

/****************************************************************************/
