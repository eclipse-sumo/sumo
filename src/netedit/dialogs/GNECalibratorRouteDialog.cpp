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
#include <config.h>

#include <iostream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/MsgHandler.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/additionals/GNECalibrator.h>
#include <netedit/additionals/GNECalibratorRoute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNECalibratorRouteDialog.h"


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

GNECalibratorRouteDialog::GNECalibratorRouteDialog(GNEAdditional* editedCalibratorRoute, bool updatingElement) :
    GNEAdditionalDialog(editedCalibratorRoute, updatingElement, 400, 120),
    myCalibratorRouteValid(true) {
    // change default header
    std::string typeOfOperation =  + " for ";
    changeAdditionalDialogHeader(myUpdatingElement ? "Edit " + toString(myEditedAdditional->getTag()) + " of " : "Create " + toString(myEditedAdditional->getTag()));

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
        myEditedAdditional->getViewNet()->getUndoList()->add(new GNEChange_Additional(myEditedAdditional, true), true);
        // Routes are created without edges
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_EDGES;
    }

    // open as modal dialog
    openAsModalDialog();
}


GNECalibratorRouteDialog::~GNECalibratorRouteDialog() {}


long
GNECalibratorRouteDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myCalibratorRouteValid == false) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
        std::string operation1 = myUpdatingElement ? ("updating") : ("creating");
        std::string operation2 = myUpdatingElement ? ("updated") : ("created");
        std::string tagString = toString(myEditedAdditional->getTag());
        // open warning dialog box
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Error " + operation1 + " " + tagString).c_str(), "%s",
                              (tagString + " cannot be " + operation2 + " because parameter " + toString(myInvalidAttr) + " is invalid.").c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
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
    // set color of myTextFieldRouteID, depending if current value is valid or not
    if (myEditedAdditional->getID() == myTextFieldRouteID->getText().text()) {
        myTextFieldRouteID->setTextColor(FXRGB(0, 0, 0));
        myEditedAdditional->setAttribute(SUMO_ATTR_ID, myTextFieldRouteID->getText().text(), myEditedAdditional->getViewNet()->getUndoList());
    } else if (myEditedAdditional->isValid(SUMO_ATTR_ID, myTextFieldRouteID->getText().text())) {
        myTextFieldRouteID->setTextColor(FXRGB(0, 0, 0));
        myEditedAdditional->setAttribute(SUMO_ATTR_ID, myTextFieldRouteID->getText().text(), myEditedAdditional->getViewNet()->getUndoList());
    } else {
        myTextFieldRouteID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myTextFieldRouteEdges, depending if current value is valEdges or not
    if (myEditedAdditional->isValid(SUMO_ATTR_EDGES, myTextFieldEdges->getText().text())) {
        myTextFieldEdges->setTextColor(FXRGB(0, 0, 0));
        myEditedAdditional->setAttribute(SUMO_ATTR_EDGES, myTextFieldEdges->getText().text(), myEditedAdditional->getViewNet()->getUndoList());
    } else {
        myTextFieldEdges->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_EDGES;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myEditedAdditional->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myEditedAdditional->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), myEditedAdditional->getViewNet()->getUndoList());
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    return 1;
}


void
GNECalibratorRouteDialog::updateCalibratorRouteValues() {
    myTextFieldRouteID->setText(myEditedAdditional->getID().c_str());
    myTextFieldEdges->setText(myEditedAdditional->getAttribute(SUMO_ATTR_EDGES).c_str());
    myTextFieldColor->setText(myEditedAdditional->getAttribute(SUMO_ATTR_COLOR).c_str());
}

/****************************************************************************/
