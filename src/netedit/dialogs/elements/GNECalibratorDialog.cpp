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
    // Create two columns, one for Routes and VehicleTypes, and other for Flows
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    // create route element list
    myRoutes = new RoutesList(this, columnLeft);
    // create closing lane reroute element list
    myVTypes = new VTypesList(this, columnLeft);
    // parking area reroute
    myCalibratorFlows = new CalibratorFlowsList(this, columnRight, myRoutes, myVTypes);
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
    std::string infoB;
    // set infoB
    if (!myRoutes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_ROUTE));
    } else if (!myVTypes->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_VTYPE));
    } else if (!myCalibratorFlows->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(GNE_TAG_CALIBRATOR_FLOW));
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
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myRoutes->updateList();
    myVTypes->updateList();
    myCalibratorFlows->updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECalibratorDialog::RoutesList - methods
// ---------------------------------------------------------------------------

GNECalibratorDialog::RoutesList::RoutesList(GNECalibratorDialog* rerouterDialog, FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterDialog, contentFrame, SUMO_TAG_ROUTE,
                           GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
}


long
GNECalibratorDialog::RoutesList::addNewElement() {
    // create route using calibrator as parent
    GNERoute* route = new GNERoute(myElementDialogParent->getElement());
    // insert route
    insertElement(route);
    // open route dialog
    const auto routeDialog = GNEAttributeCarrierDialog(route);
    // continue depending of result of routeDialog
    if (routeDialog.getResult() != GNEDialog::Result::ACCEPT) {
        // remove route
        return removeElement(route);
    } else {
        return 1;
    }
}


long
GNECalibratorDialog::RoutesList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex));
    return 1;
}

// ---------------------------------------------------------------------------
// GNECalibratorDialog::VTypesList - methods
// ---------------------------------------------------------------------------

GNECalibratorDialog::VTypesList::VTypesList(GNECalibratorDialog* rerouterDialog, FXVerticalFrame* contentFrame) :
    GNETemplateElementList(rerouterDialog, contentFrame, SUMO_TAG_VTYPE,
                           GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
}


long
GNECalibratorDialog::VTypesList::addNewElement() {
    // create vType
    GNEVType* vType = new GNEVType(myElementDialogParent->getElement());
    // insert vType
    insertElement(vType);
    // open route dialog
    const auto vTypeDialog = GNEVehicleTypeDialog(vType);
    // continue depending of result of routeDialog
    if (vTypeDialog.getResult() != GNEDialog::Result::ACCEPT) {
        // remove vType
        return removeElement(vType);
    } else {
        return 1;
    }
}


long
GNECalibratorDialog::VTypesList::openElementDialog(const size_t rowIndex) {
    // open vType dialog
    GNEVehicleTypeDialog(myEditedElements.at(rowIndex));
    return 1;
}

// ---------------------------------------------------------------------------
// GNECalibratorDialog::CalibratorFlowsList - methods
// ---------------------------------------------------------------------------

GNECalibratorDialog::CalibratorFlowsList::CalibratorFlowsList(GNECalibratorDialog* rerouterDialog, FXVerticalFrame* contentFrame,
        RoutesList* routesList, VTypesList* vTypesList) :
    GNETemplateElementList(rerouterDialog, contentFrame, GNE_TAG_CALIBRATOR_FLOW,
                           GNEElementList::Options::SORTELEMENTS | GNEElementList::Options::DIALOG_ELEMENT),
    myRoutesList(routesList),
    myVTypesList(vTypesList) {
    // disable if there are no routes in net
    if (rerouterDialog->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size() == 0) {
        disableList(TL("No routes in net"));
    }
}


long
GNECalibratorDialog::CalibratorFlowsList::addNewElement() {
    // get vType
    GNEDemandElement* vType = nullptr;
    if (myVTypesList->getEditedElements().size() > 0) {
        vType = myVTypesList->getEditedElements().back();
    } else {
        vType = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE).begin()->second;
    }
    // get route
    GNEDemandElement* route = nullptr;
    if (myVTypesList->getEditedElements().size() > 0) {
        route = myVTypesList->getEditedElements().back();
    } else {
        route = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second;
    }
    // check if route and vType are valid
    if (route && vType) {
        // create vType
        GNECalibratorFlow* calibratorFlow = new GNECalibratorFlow(myElementDialogParent->getElement(), vType, route);
        // add using undo-redo
        insertElement(calibratorFlow);
        // open route dialog
        const auto calibratorFlowDialog = GNEAttributeCarrierDialog(calibratorFlow);
        // continue depending of result of routeDialog
        if (calibratorFlowDialog.getResult() != GNEDialog::Result::CANCEL) {
            // add calibratorFlow
            return removeElement(calibratorFlow);
        }
    }
    return 1;
}


long
GNECalibratorDialog::CalibratorFlowsList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex));
    return 1;
}

/****************************************************************************/
