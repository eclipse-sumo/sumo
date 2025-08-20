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
/// @file    GNERouteDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
///
// Dialog for edit calibrator routes
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNERouteDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERouteDialog) GNERouteDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNERouteDialog::onCmdSetVariable),
};

// Object implementation
FXIMPLEMENT(GNERouteDialog, GNEElementDialog<GNEDemandElement>, GNERouteDialogMap, ARRAYNUMBER(GNERouteDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteDialog::GNERouteDialog(GNEDemandElement* route, bool updatingElement) :
    GNEElementDialog<GNEDemandElement>(route, updatingElement),
    myCalibratorRouteValid(true) {
    // Create auxiliar frames for data
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    // create ID's elements
    new FXLabel(columnLeft, toString(SUMO_ATTR_ID).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldRouteID = new FXTextField(columnRight, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create list of edge's elements
    new FXLabel(columnLeft, toString(SUMO_ATTR_EDGES).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldEdges = new FXTextField(columnRight, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create color's elements
    new FXLabel(columnLeft, toString(SUMO_ATTR_COLOR).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    myTextFieldColor = new FXTextField(columnRight, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // update tables
    updateCalibratorRouteValues();
    // add element if we aren't updating an existent element
    if (!myUpdatingElement) {
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(myElement, true), true);
        // Routes are created without edges
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_EDGES;
    }
    // open dialog
    openDialog();
}


GNERouteDialog::~GNERouteDialog() {}


void
GNERouteDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNERouteDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (!myCalibratorRouteValid) {
        std::string title;
        std::string info;
        if (myUpdatingElement) {
            title = TLF("Error updating % '%'", myElement->getTagStr(), myElement->getID());
            info = TLF("The % '%' cannot be updated because parameter %s is invalid.",
                       myElement->getTagStr(), myElement->getID(), toString(myInvalidAttr));
        } else {
            title = TLF("Error creating % '%'", myElement->getTagStr(), myElement->getID());
            info = TLF("The % '%' cannot be created because parameter %s is invalid.",
                       myElement->getTagStr(), myElement->getID(), toString(myInvalidAttr));
        }
        // open warning Box
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                              title, info);
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNERouteDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update fields
    updateCalibratorRouteValues();
    return 1;
}


long
GNERouteDialog::onCmdSetVariable(FXObject*, FXSelector, void*) {
    // At start we assumed, that all values are valid
    myCalibratorRouteValid = true;
    myInvalidAttr = SUMO_ATTR_NOTHING;
    // set color of myTextFieldRouteID, depending if current value is valid or not
    if (myElement->getID() == myTextFieldRouteID->getText().text()) {
        myTextFieldRouteID->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ID, myTextFieldRouteID->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
    } else if (myElement->isValid(SUMO_ATTR_ID, myTextFieldRouteID->getText().text())) {
        myTextFieldRouteID->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_ID, myTextFieldRouteID->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
    } else {
        myTextFieldRouteID->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_ID;
    }
    // set color of myTextFieldRouteEdges, depending if current value is valEdges or not
    if (myElement->isValid(SUMO_ATTR_EDGES, myTextFieldEdges->getText().text())) {
        myTextFieldEdges->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_EDGES, myTextFieldEdges->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
    } else {
        myTextFieldEdges->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_EDGES;
    }
    // set color of myTextFieldColor, depending if current value is valid or not
    if (myElement->isValid(SUMO_ATTR_COLOR, myTextFieldColor->getText().text())) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myElement->setAttribute(SUMO_ATTR_COLOR, myTextFieldColor->getText().text(), myElement->getNet()->getViewNet()->getUndoList());
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        myCalibratorRouteValid = false;
        myInvalidAttr = SUMO_ATTR_COLOR;
    }
    return 1;
}


void
GNERouteDialog::updateCalibratorRouteValues() {
    myTextFieldRouteID->setText(myElement->getID().c_str());
    myTextFieldEdges->setText(myElement->getAttribute(SUMO_ATTR_EDGES).c_str());
    myTextFieldColor->setText(myElement->getAttribute(SUMO_ATTR_COLOR).c_str());
}


/****************************************************************************/
