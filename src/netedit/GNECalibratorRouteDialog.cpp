/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorRouteDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator routes
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

#include "GNECalibratorRouteDialog.h"
#include "GNECalibrator.h"
#include "GNECalibratorRoute.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEChange_CalibratorItem.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorRouteDialog) GNECalibratorRouteDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CALIBRATORDIALOG_SET_VARIABLE,  GNECalibratorRouteDialog::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNECalibratorRouteDialog, GNEAdditionalDialog, GNECalibratorRouteDialogMap, ARRAYNUMBER(GNECalibratorRouteDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorRouteDialog::GNECalibratorRouteDialog(GNECalibratorRoute* editedCalibratorRoute, bool updatingElement) :
    GNEAdditionalDialog(editedCalibratorRoute->getCalibratorParent(), 400, 120),
    myEditedCalibratorRoute(editedCalibratorRoute),
    myUpdatingElement(updatingElement),
    myCalibratorRouteValid(true) {
    // change default header
    std::string typeOfOperation = myUpdatingElement ? "Edit " + toString(myEditedCalibratorRoute->getTag()) + " of " : "Create " + toString(myEditedCalibratorRoute->getTag()) + " for ";
    changeAdditionalDialogHeader(typeOfOperation + toString(myEditedCalibratorRoute->getCalibratorParent()->getTag()) + " '" + myEditedCalibratorRoute->getCalibratorParent()->getID() + "'");

    // Create auxiliar frames for data
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);

    // create ID's elements
    new FXLabel(columnLeft, toString(SUMO_ATTR_ID).c_str(), 0, GUIDesignLabelLeftThick);
    myTextFieldRouteID = new FXTextField(columnRight, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // create list of edge's elements
    new FXLabel(columnLeft, toString(SUMO_ATTR_EDGES).c_str(), 0, GUIDesignLabelLeftThick);
    myTextFieldEdges = new FXTextField(columnRight, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // create color's elements
    new FXLabel(columnLeft, toString(SUMO_ATTR_COLOR).c_str(), 0, GUIDesignLabelLeftThick);
    myTextFieldColor = new FXTextField(columnRight, GUIDesignTextFieldNCol, this, MID_GNE_CALIBRATORDIALOG_SET_VARIABLE, GUIDesignTextField);

    // update tables
    updateCalibratorRouteValues();

    // start a undo list for editing local to this additional
    initChanges();

    // add element if we aren't updating an existent element
    if (myUpdatingElement == false) {
        myEditedCalibratorRoute->getCalibratorParent()->getViewNet()->getUndoList()->add(new GNEChange_CalibratorItem(myEditedCalibratorRoute, true), true);
    }

    // open as modal dialog
    openAsModalDialog();
}


GNECalibratorRouteDialog::~GNECalibratorRouteDialog() {}


long
GNECalibratorRouteDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorRouteValid == false) {
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        std::string operation1 = myUpdatingElement ? ("updating") : ("creating");
        std::string operation2 = myUpdatingElement ? ("updated") : ("created");
        std::string parentTagString = toString(myEditedCalibratorRoute->getCalibratorParent()->getTag());
        std::string tagString = toString(myEditedCalibratorRoute->getTag());
        // open warning dialog box
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error " + operation1 + " " + parentTagString + "'s " + tagString).c_str(), "%s",
                              (parentTagString + "'s " + tagString + " cannot be " + operation2 +
                               " because parameter " + toString(myInvalidAttr) +
                               " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
        }
        return 0;
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // stop dialgo sucesfully
        getApp()->stopModal(this, TRUE);
        return 1;
    }
}


long
GNECalibratorRouteDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNECalibratorRouteDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update fields
    updateCalibratorRouteValues();
    return 1;
}


long
GNECalibratorRouteDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myCalibratorRouteValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // get pointer to undo list (Only for code legilibity)
    GNEUndoList* undoList = myEditedCalibratorRoute->getCalibratorParent()->getViewNet()->getUndoList();
    // set color of myTextFieldRouteID, depending if current value is valid or not
    if (myEditedCalibratorRoute->getID() == myTextFieldRouteID->getText().text()) {
        myTextFieldRouteID->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorRoute->setAttribute(SUMO_ATTR_ID, myTextFieldRouteID->getText().text(), undoList);
    } else if (myEditedCalibratorRoute->isValid(SUMO_ATTR_ID, myTextFieldRouteID->getText().text())) {
        myTextFieldRouteID->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorRoute->setAttribute(SUMO_ATTR_ID, myTextFieldRouteID->getText().text(), undoList);
    } else {
        myTextFieldRouteID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }

    // set color of myTextFieldRouteEdges, depending if current value is valEdges or not
    if (myEditedCalibratorRoute->isValid(SUMO_ATTR_EDGES, myTextFieldEdges->getText().text())) {
        myTextFieldEdges->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorRoute->setAttribute(SUMO_ATTR_EDGES, myTextFieldEdges->getText().text(), undoList);
    } else {
        myTextFieldEdges->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_EDGES;
    }

    // set color of myTextFieldColor, depending if current value is valid or not
    if (myEditedCalibratorRoute->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myEditedCalibratorRoute->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), undoList);
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    return 1;
}


void
GNECalibratorRouteDialog::updateCalibratorRouteValues() {
    myTextFieldRouteID->setText(myEditedCalibratorRoute->getID().c_str());
    myTextFieldEdges->setText(myEditedCalibratorRoute->getAttribute(SUMO_ATTR_EDGES).c_str());
    myTextFieldColor->setText(myEditedCalibratorRoute->getAttribute(SUMO_ATTR_COLOR).c_str());
}

/****************************************************************************/
