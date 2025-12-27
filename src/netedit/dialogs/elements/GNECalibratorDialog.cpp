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
/// @file    GNECalibratorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
///
// Dialog for edit calibrators
/****************************************************************************/

#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/dialogs/elements/GNEVehicleTypeDialog.h>
#include <netedit/elements/additional/GNECalibratorFlow.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNECalibratorDialog.h"
#include "GNEAttributeCarrierDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorDialog::GNECalibratorDialog(GNEAdditional* calibrator) :
    GNETemplateElementDialog<GNEAdditional>(calibrator, DialogType::CALIBRATOR) {
    // parking area reroute
    myCalibratorFlows = new CalibratorFlowsList(this, myContentFrame);
    // open dialog
    openDialog();
}


GNECalibratorDialog::~GNECalibratorDialog() {}


void
GNECalibratorDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNECalibratorDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // declare strings
    const auto warningTitle = TLF("Error updating % '%'", myElement->getTagStr(), myElement->getID());
    const auto infoA = TLF("% '%' cannot be updated because", myElement->getTagStr(), myElement->getID());
    const auto infoB = TLF("there are invalid %s.", toString(GNE_TAG_CALIBRATOR_FLOW));
    // continue depending of info
    if (!myCalibratorFlows->isListValid()) {
        // open question dialog box with two lines
        GNEWarningBasicDialog(myElement->getNet()->getGNEApplicationWindow(), this, warningTitle, infoA, infoB);
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    myCalibratorFlows->updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECalibratorDialog::CalibratorFlowsList - methods
// ---------------------------------------------------------------------------

GNECalibratorDialog::CalibratorFlowsList::CalibratorFlowsList(GNECalibratorDialog* calibratorDialog, FXVerticalFrame* contentFrame) :
    GNETemplateElementList(calibratorDialog, contentFrame, GNE_TAG_CALIBRATOR_FLOW,
                           GNEElementList::Options::SORTELEMENTS | GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
    // disable if there are no routes in net
    if (calibratorDialog->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size() == 0) {
        disableList(TL("No routes in net"));
    }
}


long
GNECalibratorDialog::CalibratorFlowsList::addNewElement() {
    // get vType
    GNEDemandElement* vType = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
    // get route
    GNEDemandElement* route = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second;
    // calculate begin based in last calibrator flow
    SUMOTime begin = 0;
    for (const auto &child :  myElementDialogParent->getElement()->getChildAdditionals()) {
        if (child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_FLOW) {
            begin = GNEAttributeCarrier::parse<SUMOTime>(child->getAttribute(SUMO_ATTR_END));
        }
    }
    const SUMOTime duration = GNEAttributeCarrier::parse<SUMOTime>("3600");
    // create vType
    GNECalibratorFlow* calibratorFlow = new GNECalibratorFlow(myElementDialogParent->getElement(), begin, begin + duration, vType, route);
    // add using undo-redo
    insertElement(calibratorFlow);
    // open route dialog
    const GNEAttributeCarrierDialog calibratorFlowDialog(calibratorFlow, myElementDialogParent);
    // continue depending of result of routeDialog
    if (calibratorFlowDialog.getResult() != GNEDialog::Result::ACCEPT) {
        // add calibratorFlow
        return removeElement(calibratorFlow);
    } else if (calibratorFlow->getFileBucket()->getFilename().empty()) {
        // in this case, the bucket has to be updated manually
        if (vType->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED) == GNEAttributeCarrier::TRUE_STR) {
            vType->changeFileBucket(calibratorFlow->getFileBucket());
        }
        route->changeFileBucket(calibratorFlow->getFileBucket());
    } else {
        if (vType->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED) == GNEAttributeCarrier::TRUE_STR) {
            vType->setAttribute(GNE_ATTR_SAVEFILE, calibratorFlow->getFileBucket()->getFilename(), calibratorFlow->getNet()->getUndoList());
        }
        route->setAttribute(GNE_ATTR_SAVEFILE, calibratorFlow->getFileBucket()->getFilename(), calibratorFlow->getNet()->getUndoList());
    }
    updateList();
    return 1;
}


long
GNECalibratorDialog::CalibratorFlowsList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex), myElementDialogParent);
    return 1;
}

/****************************************************************************/
