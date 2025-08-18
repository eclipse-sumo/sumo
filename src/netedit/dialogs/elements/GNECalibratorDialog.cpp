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

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/dialogs/basic/GNEQuestionBasicDialog.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/dialogs/elements/GNERouteDialog.h>
#include <netedit/dialogs/elements/GNEVehicleTypeDialog.h>
#include <netedit/elements/additional/GNECalibratorFlow.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNECalibratorDialog.h"
#include "GNECalibratorFlowDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECalibratorDialog) GNECalibratorDialogMap[] = {
    // called when user click over buttons
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_ADD,    GNECalibratorDialog::onCmdElementListAdd),
    // clicked table (Double and triple clicks allow to remove element more fast)
    FXMAPFUNC(SEL_CLICKED,  MID_GNE_ELEMENTLIST_EDIT,   GNECalibratorDialog::onCmdElementListClick),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_ELEMENTLIST_EDIT,   GNECalibratorDialog::onCmdElementListUpdate),
};

// Object implementation
FXIMPLEMENT(GNECalibratorDialog, GNEElementDialog<GNEAdditional>, GNECalibratorDialogMap, ARRAYNUMBER(GNECalibratorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorDialog::GNECalibratorDialog(GNEAdditional* calibrator) :
    GNEElementDialog<GNEAdditional>(calibrator, true) {
    // Create two columns, one for Routes and VehicleTypes, and other for Flows
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    // create route element list
    myRoutes = new ElementList<GNEDemandElement, GNEChange_DemandElement>(this, columnLeft, SUMO_TAG_ROUTE, myElement->getChildDemandElements(), true);
    // create closing lane reroute element list
    myVTypes = new ElementList<GNEDemandElement, GNEChange_DemandElement>(this, columnLeft, SUMO_TAG_VTYPE, myElement->getChildDemandElements(), true);
    // parking area reroute
    myCalibratorFlows = new ElementList<GNEAdditional, GNEChange_Additional>(this, columnRight, GNE_TAG_CALIBRATOR_FLOW, myElement->getChildAdditionals(), false);
    // disable if there are no routes in net
    if (myElement->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).size() == 0) {
        myCalibratorFlows->disableList(TL("No routes in net"));
    }
    // add element if we aren't updating an existent element
    if (!myUpdatingElement) {
        myElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(myElement, true), true);
    }
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
    const auto title = TLF("Error updating % '%'", myElement->getTagStr(), myElement->getID());
    const auto infoA = TLF("% '%' cannot be updated because", myElement->getTagStr(), myElement->getID());
    std::string infoB;
    // set infoB
    if (!myRoutes->isValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_ROUTE));
    } else if (!myVTypes->isValid()) {
        infoB = TLF("there are invalid %s.", toString(SUMO_TAG_VTYPE));
    } else if (!myCalibratorFlows->isValid()) {
        infoB = TLF("there are invalid %s.", toString(GNE_TAG_CALIBRATOR_FLOW));
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
GNECalibratorDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    return closeDialogCanceling();
}


long
GNECalibratorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myRoutes->refreshList();
    myVTypes->refreshList();
    myCalibratorFlows->refreshList();
    return 1;
}


long
GNECalibratorDialog::onCmdElementListAdd(FXObject* obj, FXSelector, void*) {
    // create new element depending of the elementList
    if (myRoutes->checkObject(obj)) {
        // create route using calibrator as parent
        GNERoute* route = new GNERoute(myElement);
        // open route dialog
        const auto routeDialog = GNERouteDialog(route, false);
        // continue depending of result of routeDialog
        if (routeDialog.getResult() == GNEDialog::Result::ACCEPT) {
            // add route
            return myRoutes->addElement(route);
        } else {
            // delete route
            delete route;
        }
    } else if (myVTypes->checkObject(obj)) {
        // create vType
        GNEVType* vType = new GNEVType(myElement);
        // open route dialog
        const auto vTypeDialog = GNEVehicleTypeDialog(vType, false);
        // continue depending of result of routeDialog
        if (vTypeDialog.getResult() == GNEDialog::Result::ACCEPT) {
            // add vType
            return myVTypes->addElement(vType);
        } else {
            // delete vType
            delete vType;
        }
    } else if (myCalibratorFlows->checkObject(obj)) {
        // get vType
        GNEDemandElement* vType = nullptr;
        if (myVTypes->getEditedElements().size() > 0) {
            vType = myVTypes->getEditedElements().back();
        } else {
            vType = myElement->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE).begin()->second;
        }
        // get route
        GNEDemandElement* route = nullptr;
        if (myRoutes->getEditedElements().size() > 0) {
            route = myRoutes->getEditedElements().back();
        } else {
            route = myElement->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE).begin()->second;
        }
        // check if route and vType are valid
        if (route && vType) {
            // create vType
            GNECalibratorFlow* calibratorFlow = new GNECalibratorFlow(myElement, vType, route);
            // open route dialog
            const auto calibratorFlowDialog = GNECalibratorFlowDialog(calibratorFlow, false);
            // continue depending of result of routeDialog
            if (calibratorFlowDialog.getResult() == GNEDialog::Result::ACCEPT) {
                // add calibratorFlow
                return myCalibratorFlows->addElement(calibratorFlow);
            } else {
                // delete calibratorFlow
                delete calibratorFlow;
            }
        }
    } else {
        throw ProcessError("Invalid object in GNECalibratorDialog::onCmdElementListEdit");
    }
}


long
GNECalibratorDialog::onCmdElementListClick(FXObject* obj, FXSelector sel, void* ptr) {
    // continue depending of the elementList
    if (myRoutes->checkObject(obj)) {
        return myRoutes->onCmdClickedList(obj, sel, ptr);
    } else if (myVTypes->checkObject(obj)) {
        return myVTypes->onCmdClickedList(obj, sel, ptr);
    } else if (myCalibratorFlows->checkObject(obj)) {
        return myCalibratorFlows->onCmdClickedList(obj, sel, ptr);
    } else {
        throw ProcessError("Invalid object in GNECalibratorDialog::onCmdElementListEdit");
    }
}


long
GNECalibratorDialog::onCmdElementListUpdate(FXObject* obj, FXSelector sel, void* ptr) {
    // continue depending of the elementList
    if (myRoutes->checkObject(obj)) {
        return myRoutes->onCmdUpdateList(obj, sel, ptr);
    } else if (myVTypes->checkObject(obj)) {
        return myVTypes->onCmdUpdateList(obj, sel, ptr);
    } else if (myCalibratorFlows->checkObject(obj)) {
        return myCalibratorFlows->onCmdUpdateList(obj, sel, ptr);
    } else {
        throw ProcessError("Invalid object in GNECalibratorDialog::onCmdElementListEdit");
    }
}

/****************************************************************************/
