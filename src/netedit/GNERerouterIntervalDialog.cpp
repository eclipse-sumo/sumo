/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouterIntervalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    eb 2017
/// @version $Id$
///
// Dialog for edit rerouter intervals
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
#include <utils/common/MsgHandler.h>

#include "GNERerouterIntervalDialog.h"
#include "GNERerouterDialog.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEChange_RerouterItem.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterIntervalDialog) GNERerouterIntervalDialogMap[] = {
    // called when user click over buttons
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE,   GNERerouterIntervalDialog::onCmdAddClosingLaneReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE,       GNERerouterIntervalDialog::onCmdAddClosingReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE,      GNERerouterIntervalDialog::onCmdAddDestProbReroute),
    FXMAPFUNC(SEL_COMMAND,          MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE,     GNERerouterIntervalDialog::onCmdAddRouteProbReroute),

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

    // use "update" instead of "command" to avoid problems mit icons
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GNERerouterIntervalDialog::onCmdEditClosingLaneReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE,     GNERerouterIntervalDialog::onCmdEditClosingReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE,    GNERerouterIntervalDialog::onCmdEditDestProbReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE,   GNERerouterIntervalDialog::onCmdEditRouteProbReroute),
    FXMAPFUNC(SEL_UPDATE,           MID_GNE_REROUTEDIALOG_EDIT_INTERVAL,            GNERerouterIntervalDialog::onCmdChangeBeginEnd),
};

// Object implementation
FXIMPLEMENT(GNERerouterIntervalDialog, GNEAdditionalDialog, GNERerouterIntervalDialogMap, ARRAYNUMBER(GNERerouterIntervalDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterIntervalDialog::GNERerouterIntervalDialog(GNERerouterInterval* rerouterInterval, bool updatingElement) :
    GNEAdditionalDialog(rerouterInterval->getRerouterParent(), 640, 480),
    myEditedRerouterInterval(rerouterInterval),
    myUpdatingElement(updatingElement),
    myBeginEndValid(true),
    myClosingLaneReroutesValid(true),
    myClosingReroutesValid(true),
    myDestProbReroutesValid(true),
    myRouteProbReroutesValid(true) {
    // change default header
    std::string typeOfOperation = myUpdatingElement ? "Edit " + toString(myEditedRerouterInterval->getTag()) + " of " : "Create " + toString(myEditedRerouterInterval->getTag()) + " for ";
    changeAdditionalDialogHeader(typeOfOperation + toString(myEditedRerouterInterval->getRerouterParent()->getTag()) + " '" + myEditedRerouterInterval->getRerouterParent()->getID() + "'");

    // Create auxiliar frames for tables
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // create horizontal frame for begin and end label
    FXHorizontalFrame* beginEndElementsLeft = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(beginEndElementsLeft, (toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " of " + toString(myEditedRerouterInterval->getTag())).c_str(), 0, GUIDesignLabelLeftThick);
    myCheckLabel = new FXLabel(beginEndElementsLeft, "", GUIIconSubSys::getIcon(ICON_CORRECT), GUIDesignLabelIcon32x32Thicked);

    // create horizontal frame for begin and end text fields
    FXHorizontalFrame* beginEndElementsRight = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myBeginTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_EDIT_INTERVAL, GUIDesignTextFieldReal);
    myBeginTextField->setText(toString(myEditedRerouterInterval->getAttribute(SUMO_ATTR_BEGIN)).c_str());
    myEndTextField = new FXTextField(beginEndElementsRight, GUIDesignTextFieldNCol, this, MID_GNE_REROUTEDIALOG_EDIT_INTERVAL, GUIDesignTextFieldReal);
    myEndTextField->setText(toString(myEditedRerouterInterval->getAttribute(SUMO_ATTR_END)).c_str());

    // Create labels and tables
    FXHorizontalFrame* buttonAndLabelClosingLaneReroute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddClosingLaneReroutes = new FXButton(buttonAndLabelClosingLaneReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_CLOSINGLANEREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelClosingLaneReroute, ("Add new " + toString(SUMO_TAG_CLOSING_LANE_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myClosingLaneRerouteTable = new FXTable(columnLeft, this, MID_GNE_REROUTEDIALOG_TABLE_CLOSINGLANEREROUTE, GUIDesignTableAdditionals);
    myClosingLaneRerouteTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myClosingLaneRerouteTable->setSelTextColor(FXRGBA(0, 0, 0, 255));

    FXHorizontalFrame* buttonAndLabelClosinReroute = new FXHorizontalFrame(columnLeft, GUIDesignAuxiliarHorizontalFrame);
    myAddClosingReroutes = new FXButton(buttonAndLabelClosinReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_CLOSINGREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelClosinReroute, ("Add new " + toString(SUMO_TAG_CLOSING_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myClosingRerouteTable = new FXTable(columnLeft, this, MID_GNE_REROUTEDIALOG_TABLE_CLOSINGREROUTE, GUIDesignTableAdditionals);
    myClosingRerouteTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myClosingRerouteTable->setSelTextColor(FXRGBA(0, 0, 0, 255));

    FXHorizontalFrame* buttonAndLabelDestProbReroute = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myAddDestProbReroutes = new FXButton(buttonAndLabelDestProbReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_DESTPROBREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelDestProbReroute, ("Add new " + toString(SUMO_TAG_DEST_PROB_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myDestProbRerouteTable = new FXTable(columnRight, this, MID_GNE_REROUTEDIALOG_TABLE_DESTPROBREROUTE, GUIDesignTableAdditionals);
    myDestProbRerouteTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myDestProbRerouteTable->setSelTextColor(FXRGBA(0, 0, 0, 255));

    FXHorizontalFrame* buttonAndLabelRouteProbReroute = new FXHorizontalFrame(columnRight, GUIDesignAuxiliarHorizontalFrame);
    myAddRouteProbReroute = new FXButton(buttonAndLabelRouteProbReroute, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_REROUTEDIALOG_ADD_ROUTEPROBREROUTE, GUIDesignButtonIcon);
    new FXLabel(buttonAndLabelRouteProbReroute, ("Add new " + toString(SUMO_TAG_ROUTE_PROB_REROUTE) + "s").c_str(), 0, GUIDesignLabelThick);
    myRouteProbRerouteTable = new FXTable(columnRight, this, MID_GNE_REROUTEDIALOG_TABLE_ROUTEPROBREROUTE, GUIDesignTableAdditionals);
    myRouteProbRerouteTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myRouteProbRerouteTable->setSelTextColor(FXRGBA(0, 0, 0, 255));

    // update tables
    updateClosingLaneReroutesTable();
    updateClosingReroutesTable();
    updateDestProbReroutesTable();
    updateRouteProbReroutesTable();

    // start a undo list for editing local to this additional
    initChanges();

    // add element if we aren't updating an existent element
    if (myUpdatingElement == false) {
        myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(myEditedRerouterInterval, true), true);
    }

    // open as modal dialog
    openAsModalDialog();
}


GNERerouterIntervalDialog::~GNERerouterIntervalDialog() {}


GNERerouterInterval*
GNERerouterIntervalDialog::getEditedRerouterInterval() const {
    return myEditedRerouterInterval;
}


long
GNERerouterIntervalDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // set strings for dialogs
    std::string errorTitle = "Error" + toString(myUpdatingElement ? "updating" : "creating") + " " + toString(myEditedRerouterInterval->getTag()) + " of " + toString(myEditedRerouterInterval->getRerouterParent()->getTag());
    std::string operationType = toString(myEditedRerouterInterval->getRerouterParent()->getTag()) + "'s " + toString(myEditedRerouterInterval->getTag()) + " cannot be " + (myUpdatingElement ? "updated" : "created") + " because ";
    if (myBeginEndValid == false) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, errorTitle.c_str(), "%s", (operationType + toString(myEditedRerouterInterval->getTag()) + " defined by " + toString(SUMO_ATTR_BEGIN) + " and " + toString(SUMO_ATTR_END) + " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else if (myEditedRerouterInterval->getClosingLaneReroutes().empty() &&
               myEditedRerouterInterval->getClosingReroutes().empty() &&
               myEditedRerouterInterval->getDestProbReroutes().empty() &&
               myEditedRerouterInterval->getRouteProbReroutes().empty()) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, errorTitle.c_str(), "%s", (operationType + "at least one " + toString(myEditedRerouterInterval->getTag()) + "'s element must be defined.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else if ((myEditedRerouterInterval->getClosingLaneReroutes().size() > 0) && (myClosingLaneReroutesValid == false)) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, errorTitle.c_str(), "%s", (operationType + "there are invalid " + toString(SUMO_TAG_CLOSING_LANE_REROUTE) + "s.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else if ((myEditedRerouterInterval->getClosingReroutes().size() > 0) && (myClosingReroutesValid == false)) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, errorTitle.c_str(), "%s", (operationType + "there are invalid " + toString(SUMO_TAG_CLOSING_REROUTE) + "s.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else if ((myEditedRerouterInterval->getDestProbReroutes().size() > 0) && (myDestProbReroutesValid == false)) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, errorTitle.c_str(), "%s", (operationType + "there are invalid " + toString(SUMO_TAG_DEST_PROB_REROUTE) + "s.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else if ((myEditedRerouterInterval->getRouteProbReroutes().size() > 0) && (myRouteProbReroutesValid == false)) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, errorTitle.c_str(), "%s", (operationType + "there are invalid " + toString(SUMO_TAG_ROUTE_PROB_REROUTE) + "s.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // Stop Modal
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNERerouterIntervalDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNERerouterIntervalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    updateClosingLaneReroutesTable();
    updateClosingReroutesTable();
    updateDestProbReroutesTable();
    updateRouteProbReroutesTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddClosingLaneReroute(FXObject*, FXSelector, void*) {
    // create closing lane reroute
    GNEClosingLaneReroute* closingLaneReroute = new GNEClosingLaneReroute(this);
    myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(closingLaneReroute, true), true);
    // update closing lane reroutes table
    updateClosingLaneReroutesTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddClosingReroute(FXObject*, FXSelector, void*) {
    // create closing reroute
    GNEClosingReroute* closingReroute = new GNEClosingReroute(this);
    myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(closingReroute, true), true);
    // update closing reroutes table
    updateClosingReroutesTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddDestProbReroute(FXObject*, FXSelector, void*) {
    // create closing reroute and add it to table
    GNEDestProbReroute* destProbReroute = new GNEDestProbReroute(this);
    myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(destProbReroute, true), true);
    // update dest Prob reroutes table
    updateDestProbReroutesTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdAddRouteProbReroute(FXObject*, FXSelector, void*) {
    // create route Prob Reroute
    GNERouteProbReroute* routeProbReroute = new GNERouteProbReroute(this);
    myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(routeProbReroute, true), true);
    // update route prob reroutes table
    updateRouteProbReroutesTable();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdClickedClosingLaneReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myEditedRerouterInterval->getClosingLaneReroutes().size(); i++) {
        if (myClosingLaneRerouteTable->getItem(i, 4)->hasFocus()) {
            myClosingLaneRerouteTable->removeRows(i);
            myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(myEditedRerouterInterval->getClosingLaneReroutes().at(i), false), true);
            updateClosingLaneReroutesTable();
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedClosingReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myEditedRerouterInterval->getClosingReroutes().size(); i++) {
        if (myClosingRerouteTable->getItem(i, 4)->hasFocus()) {
            myClosingRerouteTable->removeRows(i);
            myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(myEditedRerouterInterval->getClosingReroutes().at(i), false), true);
            updateClosingReroutesTable();
            return 1;
        }
    }
    return 1;
}


long
GNERerouterIntervalDialog::onCmdClickedDestProbReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myEditedRerouterInterval->getDestProbReroutes().size(); i++) {
        if (myDestProbRerouteTable->getItem(i, 3)->hasFocus()) {
            myDestProbRerouteTable->removeRows(i);
            myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(myEditedRerouterInterval->getDestProbReroutes().at(i), false), true);
            updateDestProbReroutesTable();
            return 1;
        }
    }
    return 0;
}


long
GNERerouterIntervalDialog::onCmdClickedRouteProbReroute(FXObject*, FXSelector, void*) {
    // check if some delete button was pressed
    for (int i = 0; i < (int)myEditedRerouterInterval->getRouteProbReroutes().size(); i++) {
        if (myRouteProbRerouteTable->getItem(i, 3)->hasFocus()) {
            myRouteProbRerouteTable->removeRows(i);
            myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList()->add(new GNEChange_RerouterItem(myEditedRerouterInterval->getRouteProbReroutes().at(i), false), true);
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
    for (int i = 0; i < myClosingLaneRerouteTable->getNumRows(); i++) {
        GNEClosingLaneReroute* closingLaneReroute = myEditedRerouterInterval->getClosingLaneReroutes().at(i);
        if (closingLaneReroute->isValid(SUMO_ATTR_ID, myClosingLaneRerouteTable->getItem(i, 0)->getText().text()) == false) {
            myClosingLaneReroutesValid = false;
            myClosingLaneRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (closingLaneReroute->isValid(SUMO_ATTR_ALLOW, myClosingLaneRerouteTable->getItem(i, 1)->getText().text()) == false) {
            myClosingLaneReroutesValid = false;
            myClosingLaneRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (closingLaneReroute->isValid(SUMO_ATTR_DISALLOW, myClosingLaneRerouteTable->getItem(i, 2)->getText().text()) == false) {
            myClosingLaneReroutesValid = false;
            myClosingLaneRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            // set new values in Closing Lane reroute
            closingLaneReroute->setAttribute(SUMO_ATTR_ID, myClosingLaneRerouteTable->getItem(i, 0)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            closingLaneReroute->setAttribute(SUMO_ATTR_ALLOW, myClosingLaneRerouteTable->getItem(i, 1)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            closingLaneReroute->setAttribute(SUMO_ATTR_DISALLOW, myClosingLaneRerouteTable->getItem(i, 2)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            // set Correct label
            myClosingLaneRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myClosingLaneRerouteTable->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditClosingReroute(FXObject*, FXSelector, void*) {
    myClosingReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myClosingRerouteTable->getNumRows(); i++) {
        GNEClosingReroute* closingReroute = myEditedRerouterInterval->getClosingReroutes().at(i);
        if (closingReroute->isValid(SUMO_ATTR_ID, myClosingRerouteTable->getItem(i, 0)->getText().text()) == false) {
            myClosingReroutesValid = false;
            myClosingRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (closingReroute->isValid(SUMO_ATTR_ALLOW, myClosingRerouteTable->getItem(i, 1)->getText().text()) == false) {
            myClosingReroutesValid = false;
            myClosingRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (closingReroute->isValid(SUMO_ATTR_DISALLOW, myClosingRerouteTable->getItem(i, 2)->getText().text()) == false) {
            myClosingReroutesValid = false;
            myClosingRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            // set new values in Closing  reroute
            closingReroute->setAttribute(SUMO_ATTR_ID, myClosingRerouteTable->getItem(i, 0)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            closingReroute->setAttribute(SUMO_ATTR_ALLOW, myClosingRerouteTable->getItem(i, 1)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            closingReroute->setAttribute(SUMO_ATTR_DISALLOW, myClosingRerouteTable->getItem(i, 2)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            // set Correct label
            myClosingRerouteTable->getItem(i, 3)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myClosingRerouteTable->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditDestProbReroute(FXObject*, FXSelector, void*) {
    myDestProbReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myDestProbRerouteTable->getNumRows(); i++) {
        GNEDestProbReroute* destProbReroute = myEditedRerouterInterval->getDestProbReroutes().at(i);
        if (destProbReroute->isValid(SUMO_ATTR_ID, myDestProbRerouteTable->getItem(i, 0)->getText().text()) == false) {
            myDestProbReroutesValid = false;
            myDestProbRerouteTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (destProbReroute->isValid(SUMO_ATTR_PROB, myDestProbRerouteTable->getItem(i, 1)->getText().text()) == false) {
            myDestProbReroutesValid = false;
            myDestProbRerouteTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            // set new values in Closing  reroute
            destProbReroute->setAttribute(SUMO_ATTR_ID, myDestProbRerouteTable->getItem(i, 0)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            destProbReroute->setAttribute(SUMO_ATTR_PROB, myDestProbRerouteTable->getItem(i, 1)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            // set Correct label
            myDestProbRerouteTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myDestProbRerouteTable->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdEditRouteProbReroute(FXObject*, FXSelector, void*) {
    myRouteProbReroutesValid = true;
    // iterate over table and check that all parameters are correct
    for (int i = 0; i < myRouteProbRerouteTable->getNumRows(); i++) {
        GNERouteProbReroute* routeProbReroute = myEditedRerouterInterval->getRouteProbReroutes().at(i);
        if (routeProbReroute->isValid(SUMO_ATTR_ID, myRouteProbRerouteTable->getItem(i, 0)->getText().text()) == false) {
            myRouteProbReroutesValid = false;
            myRouteProbRerouteTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else if (routeProbReroute->isValid(SUMO_ATTR_PROB, myRouteProbRerouteTable->getItem(i, 1)->getText().text()) == false) {
            myRouteProbReroutesValid = false;
            myRouteProbRerouteTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
        } else {
            // set new values in Closing  reroute
            routeProbReroute->setAttribute(SUMO_ATTR_ID, myRouteProbRerouteTable->getItem(i, 0)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            routeProbReroute->setAttribute(SUMO_ATTR_PROB, myRouteProbRerouteTable->getItem(i, 1)->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
            // set Correct label
            myRouteProbRerouteTable->getItem(i, 2)->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        }
    }
    // update list
    myRouteProbRerouteTable->update();
    return 1;
}


long
GNERerouterIntervalDialog::onCmdChangeBeginEnd(FXObject*, FXSelector, void*) {
    if (myEditedRerouterInterval->isValid(SUMO_ATTR_BEGIN, myBeginTextField->getText().text()) &&
            myEditedRerouterInterval->isValid(SUMO_ATTR_END, myEndTextField->getText().text())) {
        // set new values in rerouter interval
        myEditedRerouterInterval->setAttribute(SUMO_ATTR_BEGIN, myBeginTextField->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
        myEditedRerouterInterval->setAttribute(SUMO_ATTR_END, myEndTextField->getText().text(), myEditedRerouterInterval->getRerouterParent()->getViewNet()->getUndoList());
        // sort intervals of rerouter
        myEditedRerouterInterval->getRerouterParent()->sortIntervals();
        // change icon
        myBeginEndValid = true;
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
    } else {
        myBeginEndValid = false;
        myCheckLabel->setIcon(GUIIconSubSys::getIcon(ICON_ERROR));
    }
    return 0;
}


void
GNERerouterIntervalDialog::updateClosingLaneReroutesTable() {
    // clear table
    myClosingLaneRerouteTable->clearItems();
    // set number of rows
    myClosingLaneRerouteTable->setTableSize(int(myEditedRerouterInterval->getClosingLaneReroutes().size()), 5);
    // Configure list
    myClosingLaneRerouteTable->setVisibleColumns(5);
    myClosingLaneRerouteTable->setColumnWidth(0, 83);
    myClosingLaneRerouteTable->setColumnWidth(1, 83);
    myClosingLaneRerouteTable->setColumnWidth(2, 82);
    myClosingLaneRerouteTable->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myClosingLaneRerouteTable->setColumnWidth(4, GUIDesignTableIconCellWidth);
    myClosingLaneRerouteTable->setColumnText(0, toString(SUMO_ATTR_LANE).c_str());
    myClosingLaneRerouteTable->setColumnText(1, toString(SUMO_ATTR_ALLOW).c_str());
    myClosingLaneRerouteTable->setColumnText(2, toString(SUMO_ATTR_DISALLOW).c_str());
    myClosingLaneRerouteTable->setColumnText(3, "");
    myClosingLaneRerouteTable->setColumnText(4, "");
    myClosingLaneRerouteTable->getRowHeader()->setWidth(0);
    // Declare pointer to FXTableItem
    FXTableItem* item = 0;
    // iterate over values
    for (int i = 0; i < (int)myEditedRerouterInterval->getClosingLaneReroutes().size(); i++) {
        // Set closing edge
        item = new FXTableItem(myEditedRerouterInterval->getClosingLaneReroutes().at(i)->getAttribute(SUMO_ATTR_ID).c_str());
        myClosingLaneRerouteTable->setItem(i, 0, item);
        // set allow vehicles
        item = new FXTableItem(myEditedRerouterInterval->getClosingLaneReroutes().at(i)->getAttribute(SUMO_ATTR_ALLOW).c_str());
        myClosingLaneRerouteTable->setItem(i, 1, item);
        // set disallow vehicles
        item = new FXTableItem(myEditedRerouterInterval->getClosingLaneReroutes().at(i)->getAttribute(SUMO_ATTR_DISALLOW).c_str());
        myClosingLaneRerouteTable->setItem(i, 2, item);
        // set valid icon
        item = new FXTableItem("");
        item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingLaneRerouteTable->setItem(i, 3, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingLaneRerouteTable->setItem(i, 4, item);
    }
}


void
GNERerouterIntervalDialog::updateClosingReroutesTable() {
    // clear table
    myClosingRerouteTable->clearItems();
    // set number of rows
    myClosingRerouteTable->setTableSize(int(myEditedRerouterInterval->getClosingReroutes().size()), 5);
    // Configure list
    myClosingRerouteTable->setVisibleColumns(5);
    myClosingRerouteTable->setColumnWidth(0, 83);
    myClosingRerouteTable->setColumnWidth(1, 83);
    myClosingRerouteTable->setColumnWidth(2, 82);
    myClosingRerouteTable->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myClosingRerouteTable->setColumnWidth(4, GUIDesignTableIconCellWidth);
    myClosingRerouteTable->setColumnText(0, toString(SUMO_ATTR_EDGE).c_str());
    myClosingRerouteTable->setColumnText(1, toString(SUMO_ATTR_ALLOW).c_str());
    myClosingRerouteTable->setColumnText(2, toString(SUMO_ATTR_DISALLOW).c_str());
    myClosingRerouteTable->setColumnText(3, "");
    myClosingRerouteTable->setColumnText(4, "");
    myClosingRerouteTable->getRowHeader()->setWidth(0);
    // Declare pointer to FXTableItem
    FXTableItem* item = 0;
    // iterate over values
    for (int i = 0; i < (int)myEditedRerouterInterval->getClosingReroutes().size(); i++) {
        // Set closing edge
        item = new FXTableItem(myEditedRerouterInterval->getClosingReroutes().at(i)->getAttribute(SUMO_ATTR_ID).c_str());
        myClosingRerouteTable->setItem(i, 0, item);
        // set allow vehicles
        item = new FXTableItem(myEditedRerouterInterval->getClosingReroutes().at(i)->getAttribute(SUMO_ATTR_ALLOW).c_str());
        myClosingRerouteTable->setItem(i, 1, item);
        // set disallow vehicles
        item = new FXTableItem(myEditedRerouterInterval->getClosingReroutes().at(i)->getAttribute(SUMO_ATTR_DISALLOW).c_str());
        myClosingRerouteTable->setItem(i, 2, item);
        // set valid icon
        item = new FXTableItem("");
        item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingRerouteTable->setItem(i, 3, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myClosingRerouteTable->setItem(i, 4, item);
    }
}


void
GNERerouterIntervalDialog::updateDestProbReroutesTable() {
    // clear table
    myDestProbRerouteTable->clearItems();
    // set number of rows
    myDestProbRerouteTable->setTableSize(int(myEditedRerouterInterval->getDestProbReroutes().size()), 4);
    // Configure list
    myDestProbRerouteTable->setVisibleColumns(4);
    myDestProbRerouteTable->setColumnWidth(0, 124);
    myDestProbRerouteTable->setColumnWidth(1, 124);
    myDestProbRerouteTable->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myDestProbRerouteTable->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myDestProbRerouteTable->setColumnText(0, toString(SUMO_ATTR_EDGE).c_str());
    myDestProbRerouteTable->setColumnText(1, toString(SUMO_ATTR_PROB).c_str());
    myDestProbRerouteTable->setColumnText(2, "");
    myDestProbRerouteTable->setColumnText(3, "");
    myDestProbRerouteTable->getRowHeader()->setWidth(0);
    // Declare pointer to FXTableItem
    FXTableItem* item = 0;
    // iterate over values
    for (int i = 0; i < (int)myEditedRerouterInterval->getDestProbReroutes().size(); i++) {
        // Set new destination
        item = new FXTableItem(myEditedRerouterInterval->getDestProbReroutes().at(i)->getAttribute(SUMO_ATTR_ID).c_str());
        myDestProbRerouteTable->setItem(i, 0, item);
        // Set probability
        item = new FXTableItem(myEditedRerouterInterval->getDestProbReroutes().at(i)->getAttribute(SUMO_ATTR_PROB).c_str());
        myDestProbRerouteTable->setItem(i, 1, item);
        // set valid icon
        item = new FXTableItem("");
        item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myDestProbRerouteTable->setItem(i, 2, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myDestProbRerouteTable->setItem(i, 3, item);
    }
}


void
GNERerouterIntervalDialog::updateRouteProbReroutesTable() {
    // clear table
    myRouteProbRerouteTable->clearItems();
    // set number of rows
    myRouteProbRerouteTable->setTableSize(int(myEditedRerouterInterval->getRouteProbReroutes().size()), 4);
    // Configure list
    myRouteProbRerouteTable->setVisibleColumns(4);
    myRouteProbRerouteTable->setColumnWidth(0, 124);
    myRouteProbRerouteTable->setColumnWidth(1, 124);
    myRouteProbRerouteTable->setColumnWidth(2, GUIDesignTableIconCellWidth);
    myRouteProbRerouteTable->setColumnWidth(3, GUIDesignTableIconCellWidth);
    myRouteProbRerouteTable->setColumnText(0, toString(SUMO_ATTR_ROUTE).c_str());
    myRouteProbRerouteTable->setColumnText(1, toString(SUMO_ATTR_PROB).c_str());
    myRouteProbRerouteTable->setColumnText(2, "");
    myRouteProbRerouteTable->setColumnText(3, "");
    myRouteProbRerouteTable->getRowHeader()->setWidth(0);
    // Declare pointer to FXTableItem
    FXTableItem* item = 0;
    // iterate over values
    for (int i = 0; i < (int)myEditedRerouterInterval->getRouteProbReroutes().size(); i++) {
        // Set new route
        item = new FXTableItem(myEditedRerouterInterval->getRouteProbReroutes().at(i)->getAttribute(SUMO_ATTR_ID).c_str());
        myRouteProbRerouteTable->setItem(i, 0, item);
        // Set probability
        item = new FXTableItem(myEditedRerouterInterval->getRouteProbReroutes().at(i)->getAttribute(SUMO_ATTR_PROB).c_str());
        myRouteProbRerouteTable->setItem(i, 1, item);
        // set valid icon
        item = new FXTableItem("");
        item->setIcon(GUIIconSubSys::getIcon(ICON_CORRECT));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myRouteProbRerouteTable->setItem(i, 2, item);
        // set remove
        item = new FXTableItem("", GUIIconSubSys::getIcon(ICON_REMOVE));
        item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
        item->setEnabled(false);
        myRouteProbRerouteTable->setItem(i, 3, item);
    }
}


/****************************************************************************/
