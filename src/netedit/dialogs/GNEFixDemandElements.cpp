/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixDemandElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to fix demand elements during saving
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEFixDemandElements.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixDemandElements) GNEFixDemandElementsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixDemandElements::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEFixDemandElements::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEFixDemandElements::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEFixDemandElements, FXDialogBox, GNEFixDemandElementsMap, ARRAYNUMBER(GNEFixDemandElementsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFixDemandElements - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::GNEFixDemandElements(GNEViewNet* viewNet, const std::vector<GNEDemandElement*>& invalidDemandElements) :
    FXDialogBox(viewNet->getApp(), "Fix demand elements problems", GUIDesignDialogBoxExplicit(800, 620)),
    myViewNet(viewNet) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
    // create main frame
    myMainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frames for options
    FXHorizontalFrame* optionsFrame = new FXHorizontalFrame(myMainFrame, GUIDesignAuxiliarFrame);
    myLeftFrame = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
    myRightFrame = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
    // create fix route options
    myFixRouteOptions = new FixRouteOptions(this);
    // create fix vehicle  options
    myFixVehicleOptions = new FixVehicleOptions(this);
    // create fix stops options
    myFixStopOptions = new FixStopOptions(this);
    // create fix person plans options
    myFixPersonPlanOptions = new FixPersonPlanOptions(this);
    // create buttons
    myButtons = new Buttons(this);
    // split invalidDemandElements in four groups
    std::vector<GNEDemandElement*> invalidRoutes, invalidVehicles, invalidStops, invalidPlans;
    // fill groups
    for (const auto &invalidDemandElement : invalidDemandElements) {
        if (invalidDemandElement->getTagProperty().isRoute()) {
            invalidRoutes.push_back(invalidDemandElement);
        } else if (invalidDemandElement->getTagProperty().isVehicle()) {
            invalidVehicles.push_back(invalidDemandElement);
        } else if (invalidDemandElement->getTagProperty().isStop()) {
            invalidStops.push_back(invalidDemandElement);
        } else {
            invalidPlans.push_back(invalidDemandElement);
        }
    }
    // fill options
    myFixRouteOptions->setInvalidElements(invalidRoutes);
    myFixVehicleOptions->setInvalidElements(invalidVehicles);
    myFixStopOptions->setInvalidElements(invalidStops);
    myFixPersonPlanOptions->setInvalidElements(invalidPlans);
}


GNEFixDemandElements::~GNEFixDemandElements() {
}


long
GNEFixDemandElements::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    myFixRouteOptions->selectOption(obj);
    myFixVehicleOptions->selectOption(obj);
    myFixStopOptions->selectOption(obj);
    myFixPersonPlanOptions->selectOption(obj);
    return 1;
}


long
GNEFixDemandElements::onCmdAccept(FXObject*, FXSelector, void*) {
    bool continueSaving = true;
/*
    // check options for invalid routes
    if (myDemandList->myInvalidRoutes.size() > 0) {
        if (myFixRouteOptions->removeInvalidRoutes->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid routes");
            // iterate over invalid routes to delete it
            for (auto i : myDemandList->myInvalidRoutes) {
                myViewNet->getNet()->deleteDemandElement(i, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        } else if (myFixRouteOptions->selectInvalidRoutesAndCancel->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "select invalid routes");
            // iterate over invalid single lane elements to select all elements
            for (auto i : myDemandList->myInvalidRoutes) {
                i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            continueSaving = false;
        }
    }
    // check options for invalid vehicles
    if (myDemandList->myInvalidVehicles.size() > 0) {
        if (myFixVehicleOptions->removeInvalidVehicles->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::VEHICLE, "delete invalid vehicles");
            // iterate over invalid stops to delete it
            for (auto i : myDemandList->myInvalidVehicles) {
                myViewNet->getNet()->deleteDemandElement(i, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        } else if (myFixVehicleOptions->selectInvalidVehiclesAndCancel->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::VEHICLE, "select invalid vehicles");
            // iterate over invalid single lane elements to select all elements
            for (auto i : myDemandList->myInvalidVehicles) {
                i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            continueSaving = false;
        }
    }
    // check options for stops
    if (myDemandList->myInvalidStops.size() > 0) {
        if (myFixStopOptions->activateFriendlyPositionAndSave->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::STOP, "change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid stops");
            // iterate over invalid stops to enable friendly position
            for (auto i : myDemandList->myInvalidStops) {
                i->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
        } else if (myFixStopOptions->fixPositionsAndSave->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(GUIIcon::STOP, "fix positions of invalid stops");
            // iterate over invalid stops to fix positions
            for (const auto &stop : myDemandList->myInvalidStops) {
                stop->fixDemandElementProblem();
            }
            myViewNet->getUndoList()->end();
        } else if (myFixStopOptions->selectInvalidStopsAndCancel->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(GUIIcon::STOP, "select invalid stops");
            // iterate over invalid stops to select all elements
            for (auto i : myDemandList->myInvalidStops) {
                i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            continueSaving = false;
        }
    }
    // check options for person plans
    if (myDemandList->myInvalidPersonPlans.size() > 0) {
        if (myFixPersonPlanOptions->deletePersonPlan->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin("change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid person plans");
            // iterate over invalid person plans to enable friendly position
            for (auto i : myDemandList->myInvalidPersonPlans) {
                i->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
        } else if (myFixPersonPlanOptions->selectInvalidPersonPlansAndCancel->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(GUIIcon::MODEPERSONPLAN, "select invalid person plans");
            // iterate over invalid person plans to select all elements
            for (auto i : myDemandList->myInvalidPersonPlans) {
                i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            continueSaving = false;
        }
    }
    */
    if (continueSaving) {
        // stop modal with TRUE (continue saving)
        getApp()->stopModal(this, TRUE);
    } else {
        // stop modal with TRUE (abort saving)
        getApp()->stopModal(this, FALSE);
    }
    return 1;
}


long
GNEFixDemandElements::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal (abort saving)
    getApp()->stopModal(this, FALSE);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixOptions::FixOptions(FXVerticalFrame* frameParent, const std::string &title) :
    FXGroupBoxModule(frameParent, title, FXGroupBoxModule::Options::SAVE) {
    // Create table
    myTable = new FXTable(this, this, MID_TABLE, GUIDesignTableFixElements);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create vertical frames
    myLeftFrame = new FXVerticalFrame(horizontalFrame, GUIDesignAuxiliarVerticalFrame);
    myRightFrame = new FXVerticalFrame(horizontalFrame, GUIDesignAuxiliarVerticalFrame);
    // reset table
    setInvalidElements({});
}


void 
GNEFixDemandElements::FixOptions::setInvalidElements(const std::vector<GNEDemandElement*>& invalidElements) {
    // update invalid elements
    myInvalidElements = invalidElements;
    // configure table
    myTable->setTableSize((int)(myInvalidElements.size()), 3);
    myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myTable->setEditable(false);
    // configure header
    myTable->setVisibleColumns(4);
    myTable->setColumnWidth(0, GUIDesignHeight);
    myTable->setColumnWidth(1, 150);
    myTable->setColumnWidth(2, 200);
    myTable->setColumnText(0, "");
    myTable->setColumnText(1, toString(SUMO_ATTR_ID).c_str());
    myTable->setColumnText(2, "Conflict");
    myTable->getRowHeader()->setWidth(0);
    // Declare pointer to FXTableItem
    FXTableItem* item = nullptr;
    // iterate over invalid routes
    for (int i = 0; i < (int)myInvalidElements.size(); i++) {
        // Set icon
        item = new FXTableItem("", myInvalidElements.at(i)->getIcon());
        item->setIconPosition(FXTableItem::CENTER_X);
        myTable->setItem(i, 0, item);
        // Set ID
        item = new FXTableItem(myInvalidElements.at(i)->getID().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(i, 1, item);
        // Set conflict
        item = new FXTableItem(myInvalidElements.at(i)->getDemandElementProblem().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(i, 2, item);
    }
}

bool
GNEFixDemandElements::FixOptions::saveContents() const {
    return false;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixRouteOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixRouteOptions::FixRouteOptions(GNEFixDemandElements* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myLeftFrame, "Routes") {
    // Remove invalid routes
    removeInvalidRoutes = new FXRadioButton(myLeftFrame, "Remove invalid routes",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid routes
    saveInvalidRoutes = new FXRadioButton(myLeftFrame, "Save invalid routes",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid routes
    selectInvalidRoutesAndCancel = new FXRadioButton(myRightFrame, "Select invalid routes",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    removeStopsOutOfRoute = new FXCheckButton(myRightFrame, "Remove stops out of route",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // leave option "removeInvalidRoutes" as default
    removeInvalidRoutes->setCheck(true);
    // ... and remove stops out of route
    removeStopsOutOfRoute->setCheck(TRUE);
}


void
GNEFixDemandElements::FixRouteOptions::selectOption(FXObject* option) {
    if (option == removeInvalidRoutes) {
        removeInvalidRoutes->setCheck(true);
        saveInvalidRoutes->setCheck(false);
        selectInvalidRoutesAndCancel->setCheck(false);
    } else if (option == saveInvalidRoutes) {
        removeInvalidRoutes->setCheck(false);
        saveInvalidRoutes->setCheck(true);
        selectInvalidRoutesAndCancel->setCheck(false);
    } else if (option == selectInvalidRoutesAndCancel) {
        removeInvalidRoutes->setCheck(false);
        saveInvalidRoutes->setCheck(false);
        selectInvalidRoutesAndCancel->setCheck(true);
    }
}


void
GNEFixDemandElements::FixRouteOptions::enableFixRouteOptions() {
    removeInvalidRoutes->enable();
    saveInvalidRoutes->enable();
    selectInvalidRoutesAndCancel->enable();
}


void
GNEFixDemandElements::FixRouteOptions::disableFixRouteOptions() {
    removeInvalidRoutes->disable();
    saveInvalidRoutes->disable();
    selectInvalidRoutesAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixVehicleOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixVehicleOptions::FixVehicleOptions(GNEFixDemandElements* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myLeftFrame, "Vehicles") {
    // Remove invalid vehicles
    removeInvalidVehicles = new FXRadioButton(myLeftFrame, "Remove invalid vehicles",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid vehicles
    saveInvalidVehicles = new FXRadioButton(myLeftFrame, "Save invalid vehicles",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid vehicle
    selectInvalidVehiclesAndCancel = new FXRadioButton(myRightFrame, "Select invalid vehicle",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    removeStopsOutOfRoute = new FXCheckButton(myRightFrame, "Remove stops out of route",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // by default remove invalid vehicles
    removeInvalidVehicles->setCheck(TRUE);
    // ... and remove stops out of route
    removeStopsOutOfRoute->setCheck(TRUE);
}


void
GNEFixDemandElements::FixVehicleOptions::selectOption(FXObject* option) {
    if (option == removeInvalidVehicles) {
        removeInvalidVehicles->setCheck(true);
        saveInvalidVehicles->setCheck(false);
        selectInvalidVehiclesAndCancel->setCheck(false);
    } else if (option == saveInvalidVehicles) {
        removeInvalidVehicles->setCheck(false);
        saveInvalidVehicles->setCheck(true);
        selectInvalidVehiclesAndCancel->setCheck(false);
    } else if (option == selectInvalidVehiclesAndCancel) {
        removeInvalidVehicles->setCheck(false);
        saveInvalidVehicles->setCheck(false);
        selectInvalidVehiclesAndCancel->setCheck(true);
    }
}


void
GNEFixDemandElements::FixVehicleOptions::enableFixVehicleOptions() {
    removeInvalidVehicles->enable();
    saveInvalidVehicles->enable();
    selectInvalidVehiclesAndCancel->enable();
}


void
GNEFixDemandElements::FixVehicleOptions::disableFixVehicleOptions() {
    removeInvalidVehicles->disable();
    saveInvalidVehicles->disable();
    selectInvalidVehiclesAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixStopOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixStopOptions::FixStopOptions(GNEFixDemandElements* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myRightFrame, "Stops") {
    // Activate friendlyPos and save
    activateFriendlyPositionAndSave = new FXRadioButton(myLeftFrame, "Activate friendlyPos and save",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid position
    saveInvalid = new FXRadioButton(myLeftFrame, "Save invalid positions",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid Stops
    selectInvalidStopsAndCancel = new FXRadioButton(myRightFrame, "Select invalid Stops",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Fix positions and save
    fixPositionsAndSave = new FXRadioButton(myRightFrame, "Fix positions and save",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // leave option "activateFriendlyPositionAndSave" as default
    activateFriendlyPositionAndSave->setCheck(true);
}


void
GNEFixDemandElements::FixStopOptions::selectOption(FXObject* option) {
    if (option == activateFriendlyPositionAndSave) {
        activateFriendlyPositionAndSave->setCheck(true);
        fixPositionsAndSave->setCheck(false);
        saveInvalid->setCheck(false);
        selectInvalidStopsAndCancel->setCheck(false);
    } else if (option == fixPositionsAndSave) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(true);
        saveInvalid->setCheck(false);
        selectInvalidStopsAndCancel->setCheck(false);
    } else if (option == saveInvalid) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(false);
        saveInvalid->setCheck(true);
        selectInvalidStopsAndCancel->setCheck(false);
    } else if (option == selectInvalidStopsAndCancel) {
        activateFriendlyPositionAndSave->setCheck(false);
        fixPositionsAndSave->setCheck(false);
        saveInvalid->setCheck(false);
        selectInvalidStopsAndCancel->setCheck(true);
    }
}


void
GNEFixDemandElements::FixStopOptions::enableFixStopOptions() {
    activateFriendlyPositionAndSave->enable();
    fixPositionsAndSave->enable();
    saveInvalid->enable();
    selectInvalidStopsAndCancel->enable();
}


void
GNEFixDemandElements::FixStopOptions::disableFixStopOptions() {
    activateFriendlyPositionAndSave->disable();
    fixPositionsAndSave->disable();
    saveInvalid->disable();
    selectInvalidStopsAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixPersonPlanOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixPersonPlanOptions::FixPersonPlanOptions(GNEFixDemandElements* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myRightFrame, "Person/container plans") {
    // Delete person plan
    deletePersonPlan = new FXRadioButton(myLeftFrame, "Delete person plan",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid person plans
    saveInvalid = new FXRadioButton(myLeftFrame, "Save invalid person plans",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid person plans
    selectInvalidPersonPlansAndCancel = new FXRadioButton(myRightFrame, "Select invalid person plans",
        fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // leave option "activateFriendlyPositionAndSave" as default
    deletePersonPlan->setCheck(true);
}


void
GNEFixDemandElements::FixPersonPlanOptions::selectOption(FXObject* option) {
    if (option == deletePersonPlan) {
        deletePersonPlan->setCheck(true);
        saveInvalid->setCheck(false);
        selectInvalidPersonPlansAndCancel->setCheck(false);
        selectInvalidPersonPlansAndCancel->setCheck(false);
    } else if (option == saveInvalid) {
        deletePersonPlan->setCheck(false);
        saveInvalid->setCheck(true);
        selectInvalidPersonPlansAndCancel->setCheck(false);
    } else if (option == selectInvalidPersonPlansAndCancel) {
        deletePersonPlan->setCheck(false);
        saveInvalid->setCheck(false);
        selectInvalidPersonPlansAndCancel->setCheck(true);
    }
}


void
GNEFixDemandElements::FixPersonPlanOptions::enableFixPersonPlanOptions() {
    deletePersonPlan->enable();
    saveInvalid->enable();
    selectInvalidPersonPlansAndCancel->enable();
}


void
GNEFixDemandElements::FixPersonPlanOptions::disableFixPersonPlanOptions() {
    deletePersonPlan->disable();
    saveInvalid->disable();
    selectInvalidPersonPlansAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::Buttons - methods
// ---------------------------------------------------------------------------

 GNEFixDemandElements::Buttons::Buttons(GNEFixDemandElements* fixDemandElementsParent) :
    FXHorizontalFrame(fixDemandElementsParent->myMainFrame, GUIDesignHorizontalFrame) {
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(this, FXWindow::tr("&Accept"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), fixDemandElementsParent, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(this, FXWindow::tr("&Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), fixDemandElementsParent, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}
/****************************************************************************/
