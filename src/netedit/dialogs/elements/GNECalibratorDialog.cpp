/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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

namespace {
void
syncDemandParentFileBucket(GNEDemandElement* demandParent, GNEAdditional* calibratorFlow) {
    if ((demandParent == nullptr) || (demandParent->getFileBucket() == calibratorFlow->getFileBucket())) {
        return;
    }
    if (demandParent->getTagProperty()->getTag() == SUMO_TAG_VTYPE) {
        const bool isDefault = GNEAttributeCarrier::parse<bool>(demandParent->getAttribute(GNE_ATTR_DEFAULT_VTYPE));
        const bool isModified = (demandParent->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED) == GNEAttributeCarrier::TRUE_STR);
        if (isDefault && !isModified) {
            return;
        }
    }
    if (calibratorFlow->getFileBucket()->getFilename().empty()) {
        demandParent->changeFileBucket(calibratorFlow->getFileBucket());
    } else {
        demandParent->setAttribute(GNE_ATTR_SAVEFILE, calibratorFlow->getFileBucket()->getFilename(), calibratorFlow->getNet()->getUndoList());
    }
}
} // namespace


GNECalibratorDialog::GNECalibratorDialog(GNEAdditional* calibrator) :
    GNETemplateElementDialog<GNEAdditional>(calibrator, DialogType::CALIBRATOR) {
    // create two columns for route-based and type-based calibrator flows
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarVerticalFrame);
    myCalibratorFlows = new CalibratorFlowsList(this, columnLeft);
    myCalibratorTypeFlows = new CalibratorTypeFlowsList(this, columnRight);
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
    if (!myCalibratorFlows->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(GNE_TAG_CALIBRATOR_FLOW));
    } else if (!myCalibratorTypeFlows->isListValid()) {
        infoB = TLF("there are invalid %s.", toString(GNE_TAG_CALIBRATOR_TYPEFLOW));
    }
    // continue depending of info
    if (!infoB.empty()) {
        // open question dialog box with two lines
        GNEWarningBasicDialog(myElement->getNet()->getGNEApplicationWindow(), this, warningTitle, infoA, infoB);
        return 1;
    } else {
        // ensure referenced demand elements are saved in the calibrator's file bucket
        for (const auto& child : myElement->getChildAdditionals()) {
            if ((child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_FLOW) ||
                    (child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_TYPEFLOW)) {
                for (const auto& demandParent : child->getParentDemandElements()) {
                    syncDemandParentFileBucket(demandParent, child);
                }
            }
        }
        // update geometry so newly created/edited listed flows render immediately
        myElement->updateGeometry();
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    myCalibratorFlows->updateList();
    myCalibratorTypeFlows->updateList();
    return 1;
}


long
GNECalibratorDialog::sortAllCalibratorFlows() {
    std::vector<std::tuple<double, double, size_t, GNEAdditional*> > flowTuples;
    size_t idx = 0;
    for (const auto& child : myElement->getChildAdditionals()) {
        const auto tag = child->getTagProperty()->getTag();
        if ((tag == GNE_TAG_CALIBRATOR_FLOW) || (tag == GNE_TAG_CALIBRATOR_TYPEFLOW)) {
            flowTuples.emplace_back(
                GNEAttributeCarrier::parse<double>(child->getAttribute(SUMO_ATTR_BEGIN)),
                GNEAttributeCarrier::parse<double>(child->getAttribute(SUMO_ATTR_END)),
                idx++, child);
        }
    }
    std::sort(flowTuples.begin(), flowTuples.end());
    std::vector<GNEAdditional*> sortedFlows;
    sortedFlows.reserve(flowTuples.size());
    for (const auto& t : flowTuples) {
        sortedFlows.push_back(std::get<3>(t));
    }
    myElement->sortChildren(sortedFlows);
    myElement->updateGeometry();
    myCalibratorFlows->updateList();
    myCalibratorTypeFlows->updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECalibratorDialog::CalibratorFlowsList - methods
// ---------------------------------------------------------------------------

GNECalibratorDialog::CalibratorFlowsList::CalibratorFlowsList(GNECalibratorDialog* calibratorDialog, FXVerticalFrame* contentFrame) :
    GNETemplateElementList(calibratorDialog, contentFrame, GNE_TAG_CALIBRATOR_FLOW,
                           GNEElementList::Options::SORTELEMENTS | GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
    // disable if there are no routes in net
    const auto& demandElements = calibratorDialog->getElement()->getNet()->getAttributeCarriers()->getDemandElements();
    if ((demandElements.at(SUMO_TAG_ROUTE).size() == 0) && (demandElements.at(SUMO_TAG_ROUTE_DISTRIBUTION).size() == 0)) {
        disableList(TL("No routes in net"));
    }
}


long
GNECalibratorDialog::CalibratorFlowsList::addNewElement() {
    const auto& ACs = myElementDialogParent->getElement()->getNet()->getAttributeCarriers();
    // get vType
    GNEDemandElement* vType = ACs->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
    // get route
    GNEDemandElement* route = (ACs->getDemandElements().at(SUMO_TAG_ROUTE).size() > 0) ?
                              ACs->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second :
                              ACs->getDemandElements().at(SUMO_TAG_ROUTE_DISTRIBUTION).begin()->second;
    // calculate begin based on last calibrator flow
    SUMOTime begin = 0;
    for (const auto& child : myElementDialogParent->getElement()->getChildAdditionals()) {
        if ((child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_FLOW) ||
                (child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_TYPEFLOW)) {
            begin = MAX2(begin, GNEAttributeCarrier::parse<SUMOTime>(child->getAttribute(SUMO_ATTR_END)));
        }
    }
    const SUMOTime duration = GNEAttributeCarrier::parse<SUMOTime>("3600");
    // create calibrator flow
    GNECalibratorFlow* calibratorFlow = new GNECalibratorFlow(myElementDialogParent->getElement(), begin, begin + duration, vType, route);
    // add using undo-redo
    insertElement(calibratorFlow);
    // open attribute carrier dialog
    const GNEAttributeCarrierDialog calibratorFlowDialog(calibratorFlow, myElementDialogParent);
    // continue depending of result of dialog
    if (calibratorFlowDialog.getResult() != GNEDialog::Result::ACCEPT) {
        return removeElement(calibratorFlow);
    }
    for (const auto& demandParent : calibratorFlow->getParentDemandElements()) {
        syncDemandParentFileBucket(demandParent, calibratorFlow);
    }
    updateList();
    return 1;
}


long
GNECalibratorDialog::CalibratorFlowsList::sortRows() {
    return static_cast<GNECalibratorDialog*>(myElementDialogParent)->sortAllCalibratorFlows();
}


long
GNECalibratorDialog::CalibratorFlowsList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex), myElementDialogParent);
    updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECalibratorDialog::CalibratorTypeFlowsList - methods
// ---------------------------------------------------------------------------

GNECalibratorDialog::CalibratorTypeFlowsList::CalibratorTypeFlowsList(GNECalibratorDialog* calibratorDialog, FXVerticalFrame* contentFrame) :
    GNETemplateElementList(calibratorDialog, contentFrame, GNE_TAG_CALIBRATOR_TYPEFLOW,
                           GNEElementList::Options::SORTELEMENTS | GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
}


long
GNECalibratorDialog::CalibratorTypeFlowsList::addNewElement() {
    // get default vType
    GNEDemandElement* vType = myElementDialogParent->getElement()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
    // calculate begin based on last calibrator flow
    SUMOTime begin = 0;
    for (const auto& child : myElementDialogParent->getElement()->getChildAdditionals()) {
        if ((child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_FLOW) ||
                (child->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_TYPEFLOW)) {
            begin = MAX2(begin, GNEAttributeCarrier::parse<SUMOTime>(child->getAttribute(SUMO_ATTR_END)));
        }
    }
    const SUMOTime duration = GNEAttributeCarrier::parse<SUMOTime>("3600");
    // create calibrator type flow
    GNECalibratorTypeFlow* calibratorTypeFlow = new GNECalibratorTypeFlow(myElementDialogParent->getElement(), begin, begin + duration, vType);
    // add using undo-redo
    insertElement(calibratorTypeFlow);
    // open attribute carrier dialog
    const GNEAttributeCarrierDialog calibratorTypeFlowDialog(calibratorTypeFlow, myElementDialogParent);
    // continue depending of result of dialog
    if (calibratorTypeFlowDialog.getResult() != GNEDialog::Result::ACCEPT) {
        return removeElement(calibratorTypeFlow);
    }
    for (const auto& demandParent : calibratorTypeFlow->getParentDemandElements()) {
        syncDemandParentFileBucket(demandParent, calibratorTypeFlow);
    }
    updateList();
    return 1;
}


long
GNECalibratorDialog::CalibratorTypeFlowsList::sortRows() {
    return static_cast<GNECalibratorDialog*>(myElementDialogParent)->sortAllCalibratorFlows();
}


long
GNECalibratorDialog::CalibratorTypeFlowsList::openElementDialog(const size_t rowIndex) {
    // open attribute carrier dialog
    GNEAttributeCarrierDialog(myEditedElements.at(rowIndex), myElementDialogParent);
    updateList();
    return 1;
}

/****************************************************************************/
