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
/// @file    GNEFixDemandElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to fix demand elements during saving
/****************************************************************************/

#include <netedit/dialogs/basic/GNEErrorBasicDialog.h>
#include <netedit/dialogs/basic/GNEInformationBasicDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEFixDemandElementsDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::GNEFixDemandElementsDialog(GNEApplicationWindow *mainWindow) :
    GNEFixElementsDialog(mainWindow, TL("Fix demand elements problems"), GUIIcon::SUPERMODEDEMAND, 800, 620) {
    // create fix route options
    myFixRouteOptions = new FixRouteOptions(this);
    // create fix vehicle  options
    myFixVehicleOptions = new FixVehicleOptions(this);
    // create fix stops options
    myFixStopPositionOptions = new FixStopPositionOptions(this);
    // create fix person plans options
    myFixPersonPlanOptions = new FixPersonPlanOptions(this);
}


GNEFixDemandElementsDialog::~GNEFixDemandElementsDialog() {
}


void
GNEFixDemandElementsDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    // run internal test in all modules
    myFixRouteOptions->runInternalTest(dialogArgument);
    myFixVehicleOptions->runInternalTest(dialogArgument);
    myFixStopPositionOptions->runInternalTest(dialogArgument);
    myFixPersonPlanOptions->runInternalTest(dialogArgument);
    // accept changes
    onCmdAccept(nullptr, 0, nullptr);
}


GNEDialog::Result
GNEFixDemandElementsDialog::openDialog(const std::vector<GNEDemandElement*>& invalidDemandElements) {
    // split invalidDemandElements in four groups
    std::vector<GNEDemandElement*> invalidRoutes, invalidVehicles, invalidStops, invalidPlans;
    // fill groups
    for (const auto& invalidDemandElement : invalidDemandElements) {
        if (invalidDemandElement->getTagProperty()->isRoute()) {
            invalidRoutes.push_back(invalidDemandElement);
        } else if (invalidDemandElement->getTagProperty()->isVehicle()) {
            invalidVehicles.push_back(invalidDemandElement);
        } else if (invalidDemandElement->getTagProperty()->isVehicleStop()) {
            invalidStops.push_back(invalidDemandElement);
        } else {
            invalidPlans.push_back(invalidDemandElement);
        }
    }
    // fill options
    myFixRouteOptions->setInvalidElements(invalidRoutes);
    myFixVehicleOptions->setInvalidElements(invalidVehicles);
    myFixStopPositionOptions->setInvalidElements(invalidStops);
    myFixPersonPlanOptions->setInvalidElements(invalidPlans);
    // open modal dialog
    return openModal();
}


long
GNEFixDemandElementsDialog::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    // select options
    myFixRouteOptions->selectOption(obj);
    myFixVehicleOptions->selectOption(obj);
    myFixStopPositionOptions->selectOption(obj);
    myFixPersonPlanOptions->selectOption(obj);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixRouteOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixRouteOptions::FixRouteOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myApplicationWindow, fixDemandElementsParent->myLeftFrame, TL("Routes")) {
    // Remove invalid routes
    myRemoveInvalidRoutes = new FXRadioButton(myLeftFrameOptions, TL("Remove invalid routes"),
                                              fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid routes
    mySaveInvalidRoutes = new FXRadioButton(myLeftFrameOptions, TL("Save invalid routes"),
                                            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid routes
    mySelectRouteInvalids = new FXRadioButton(myRightFrameOptions, TL("Select conflicted routes"),
                                              fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    myRemoveStopsOutOfRoute = new FXCheckButton(myRightFrameOptions, TL("Remove stops out of route"),
                                                fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // leave option "removeInvalidRoutes" as default
    myRemoveInvalidRoutes->setCheck(true);
    // ... and remove stops out of route
    myRemoveStopsOutOfRoute->setCheck(TRUE);
}


void
GNEFixDemandElementsDialog::FixRouteOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    // chooose solution
    if (dialogArgument->fixSolution == "saveRouteInvalids") {
        mySaveInvalidRoutes->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "removeRouteInvalids") {
        myRemoveInvalidRoutes->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "selectRouteInvalids") {
        mySelectRouteInvalids->setCheck(TRUE, TRUE);
    }
}


bool
GNEFixDemandElementsDialog::FixRouteOptions::applyFixOption() {
    if (myInvalidElements.size() > 0) {
        if (myRemoveInvalidRoutes->getCheck() == TRUE) {
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::ROUTE, TL("delete invalid routes"));
            // iterate over invalid routes to delete it
            for (const auto& invalidRoute : myInvalidElements) {
                // special case for embedded routes
                if (invalidRoute->getTagProperty()->getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                    myMainWindow->getViewNet()->getNet()->deleteDemandElement(invalidRoute->getParentDemandElements().front(), myMainWindow->getUndoList());
                } else {
                    myMainWindow->getViewNet()->getNet()->deleteDemandElement(invalidRoute, myMainWindow->getUndoList());
                }
            }
            // end undo list
            myMainWindow->getUndoList()->end();
        } else if (mySelectRouteInvalids->getCheck() == TRUE) {
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::ROUTE, TL("select invalid routes"));
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidRoute : myInvalidElements) {
                invalidRoute->setAttribute(GNE_ATTR_SELECTED, "true", myMainWindow->getUndoList());
            }
            // end undo list
            myMainWindow->getUndoList()->end();
            // abort saving
            return false;
        }
        // check if remove stops
        if (myRemoveStopsOutOfRoute->getCheck() == TRUE) {
            // get all stops to remove
            std::vector<GNEDemandElement*> stopsToRemove;
            for (const auto& invalidRoute : myInvalidElements) {
                const auto invaldstops = invalidRoute->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::STOP, TL("delete invalid stops"));
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                myMainWindow->getViewNet()->getNet()->deleteDemandElement(stopToRemove, myMainWindow->getUndoList());
            }
            // end undo list
            myMainWindow->getUndoList()->end();
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixRouteOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myRemoveInvalidRoutes) {
        myRemoveInvalidRoutes->setCheck(true);
        mySaveInvalidRoutes->setCheck(false);
        mySelectRouteInvalids->setCheck(false);
    } else if (obj == mySaveInvalidRoutes) {
        myRemoveInvalidRoutes->setCheck(false);
        mySaveInvalidRoutes->setCheck(true);
        mySelectRouteInvalids->setCheck(false);
    } else if (obj == mySelectRouteInvalids) {
        myRemoveInvalidRoutes->setCheck(false);
        mySaveInvalidRoutes->setCheck(false);
        mySelectRouteInvalids->setCheck(true);
    }
    return 1;
}


void
GNEFixDemandElementsDialog::FixRouteOptions::enableOptions() {
    myRemoveInvalidRoutes->enable();
    mySaveInvalidRoutes->enable();
    mySelectRouteInvalids->enable();
    myRemoveStopsOutOfRoute->enable();
}


void
GNEFixDemandElementsDialog::FixRouteOptions::disableOptions() {
    myRemoveInvalidRoutes->disable();
    mySaveInvalidRoutes->disable();
    mySelectRouteInvalids->disable();
    myRemoveStopsOutOfRoute->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixVehicleOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixVehicleOptions::FixVehicleOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myApplicationWindow, fixDemandElementsParent->myLeftFrame, TL("Vehicles")) {
    // Remove invalid vehicles
    myRemoveInvalidVehicles = new FXRadioButton(myLeftFrameOptions, TL("Remove invalid vehicles"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid vehicles
    mySaveInvalidVehicles = new FXRadioButton(myLeftFrameOptions, TL("Save invalid vehicles"),
                                            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid vehicle
    mySelectInvalidVehiclesAndCancel = new FXRadioButton(myRightFrameOptions, TL("Select conflicted vehicle"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    myRemoveStopsOutOfVehicle = new FXCheckButton(myRightFrameOptions, TL("Remove stops out of vehicle's route"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // by default remove invalid vehicles
    myRemoveInvalidVehicles->setCheck(TRUE);
    // ... and remove stops out of route
    myRemoveStopsOutOfVehicle->setCheck(TRUE);
}


void
GNEFixDemandElementsDialog::FixVehicleOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
}


bool
GNEFixDemandElementsDialog::FixVehicleOptions::applyFixOption() {
    if (myInvalidElements.size() > 0) {
        if (myRemoveInvalidVehicles->getCheck() == TRUE) {
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::VEHICLE, TL("delete invalid vehicles"));
            // iterate over invalid vehicles to delete it
            for (const auto& invalidVehicle : myInvalidElements) {
                // check that vehicle was not removed previously in cascade
                if (myMainWindow->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(invalidVehicle->getTagProperty()->getTag(), invalidVehicle->getID(), false) != nullptr) {
                    myMainWindow->getViewNet()->getNet()->deleteDemandElement(invalidVehicle, myMainWindow->getUndoList());
                }
            }
            // end undo list
            myMainWindow->getUndoList()->end();
        } else if (mySelectInvalidVehiclesAndCancel->getCheck() == TRUE) {
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::ROUTE, TL("select invalid routes"));
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidVehicle : myInvalidElements) {
                invalidVehicle->setAttribute(GNE_ATTR_SELECTED, "true", myMainWindow->getUndoList());
            }
            // end undo list
            myMainWindow->getUndoList()->end();
            // abort saving
            return false;
        }
        // check if remove stops
        if (myRemoveStopsOutOfVehicle->getCheck() == TRUE) {
            // get all stops to remove
            std::vector<GNEDemandElement*> stopsToRemove;
            for (const auto& invalidVehicle : myInvalidElements) {
                const auto invaldstops = invalidVehicle->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::STOP, TL("delete invalid stops"));
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                myMainWindow->getViewNet()->getNet()->deleteDemandElement(stopToRemove, myMainWindow->getUndoList());
            }
            // end undo list
            myMainWindow->getUndoList()->end();
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixVehicleOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myRemoveInvalidVehicles) {
        myRemoveInvalidVehicles->setCheck(true);
        mySaveInvalidVehicles->setCheck(false);
        mySelectInvalidVehiclesAndCancel->setCheck(false);
    } else if (obj == mySaveInvalidVehicles) {
        myRemoveInvalidVehicles->setCheck(false);
        mySaveInvalidVehicles->setCheck(true);
        mySelectInvalidVehiclesAndCancel->setCheck(false);
    } else if (obj == mySelectInvalidVehiclesAndCancel) {
        myRemoveInvalidVehicles->setCheck(false);
        mySaveInvalidVehicles->setCheck(false);
        mySelectInvalidVehiclesAndCancel->setCheck(true);
    }
    return 1;
}


void
GNEFixDemandElementsDialog::FixVehicleOptions::enableOptions() {
    myRemoveInvalidVehicles->enable();
    mySaveInvalidVehicles->enable();
    mySelectInvalidVehiclesAndCancel->enable();
    myRemoveStopsOutOfVehicle->enable();
}


void
GNEFixDemandElementsDialog::FixVehicleOptions::disableOptions() {
    myRemoveInvalidVehicles->disable();
    mySaveInvalidVehicles->disable();
    mySelectInvalidVehiclesAndCancel->disable();
    myRemoveStopsOutOfVehicle->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixStopPositionOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixStopPositionOptions::FixStopPositionOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myApplicationWindow, fixDemandElementsParent->myRightFrame, TL("Stop positions")) {
    // Activate friendlyPos and save
    myActivateFriendlyPositionAndSave = new FXRadioButton(myLeftFrameOptions, TL("Activate friendlyPos and save"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid position
    mySaveInvalid = new FXRadioButton(myLeftFrameOptions, TL("Save invalid positions"),
                                    fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid Stops
    mySelectInvalidStopsAndCancel = new FXRadioButton(myRightFrameOptions, TL("Select conflicted Stops"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Fix positions and save
    myFixPositionsAndSave = new FXRadioButton(myRightFrameOptions, TL("Fix positions and save"),
                                            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // leave option "activateFriendlyPositionAndSave" as default
    myActivateFriendlyPositionAndSave->setCheck(true);
}


void
GNEFixDemandElementsDialog::FixStopPositionOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
}


bool
GNEFixDemandElementsDialog::FixStopPositionOptions::applyFixOption() {
    // check options for stops
    if (myInvalidElements.size() > 0) {
        if (myActivateFriendlyPositionAndSave->getCheck() == TRUE) {
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::STOP, TLF("change % of invalid stops", toString(SUMO_ATTR_FRIENDLY_POS)));
            // iterate over invalid stops to enable friendly position
            for (const auto& stop : myInvalidElements) {
                stop->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myMainWindow->getUndoList());
            }
            myMainWindow->getUndoList()->end();
        } else if (myFixPositionsAndSave->getCheck() == TRUE) {
            myMainWindow->getUndoList()->begin(GUIIcon::STOP, TL("fix positions of invalid stops"));
            // iterate over invalid stops to fix positions
            for (const auto& stop : myInvalidElements) {
                stop->fixDemandElementProblem();
            }
            myMainWindow->getUndoList()->end();
        } else if (mySelectInvalidStopsAndCancel->getCheck() == TRUE) {
            myMainWindow->getUndoList()->begin(GUIIcon::STOP, TL("select invalid stops"));
            // iterate over invalid stops to select all elements
            for (const auto& stop : myInvalidElements) {
                stop->setAttribute(GNE_ATTR_SELECTED, "true", myMainWindow->getUndoList());
            }
            // end undo list
            myMainWindow->getUndoList()->end();
            // abort saving
            return false;
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixStopPositionOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myActivateFriendlyPositionAndSave) {
        myActivateFriendlyPositionAndSave->setCheck(true);
        myFixPositionsAndSave->setCheck(false);
        mySaveInvalid->setCheck(false);
        mySelectInvalidStopsAndCancel->setCheck(false);
    } else if (obj == myFixPositionsAndSave) {
        myActivateFriendlyPositionAndSave->setCheck(false);
        myFixPositionsAndSave->setCheck(true);
        mySaveInvalid->setCheck(false);
        mySelectInvalidStopsAndCancel->setCheck(false);
    } else if (obj == mySaveInvalid) {
        myActivateFriendlyPositionAndSave->setCheck(false);
        myFixPositionsAndSave->setCheck(false);
        mySaveInvalid->setCheck(true);
        mySelectInvalidStopsAndCancel->setCheck(false);
    } else if (obj == mySelectInvalidStopsAndCancel) {
        myActivateFriendlyPositionAndSave->setCheck(false);
        myFixPositionsAndSave->setCheck(false);
        mySaveInvalid->setCheck(false);
        mySelectInvalidStopsAndCancel->setCheck(true);
    }
    return 1;
}


void
GNEFixDemandElementsDialog::FixStopPositionOptions::enableOptions() {
    myActivateFriendlyPositionAndSave->enable();
    myFixPositionsAndSave->enable();
    mySaveInvalid->enable();
    mySelectInvalidStopsAndCancel->enable();
}


void
GNEFixDemandElementsDialog::FixStopPositionOptions::disableOptions() {
    myActivateFriendlyPositionAndSave->disable();
    myFixPositionsAndSave->disable();
    mySaveInvalid->disable();
    mySelectInvalidStopsAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixPersonPlanOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixPersonPlanOptions::FixPersonPlanOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent->myApplicationWindow, fixDemandElementsParent->myRightFrame, TL("Person/container plans")) {
    // Delete person plan
    myDeletePersonPlan = new FXRadioButton(myLeftFrameOptions, TL("Delete person plan"),
                                         fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid person plans
    mySaveInvalid = new FXRadioButton(myLeftFrameOptions, TL("Save invalid person plans"),
                                    fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid person plans
    mySelectInvalidPersonPlansAndCancel = new FXRadioButton(myRightFrameOptions, TL("Select conflicted person plans"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // leave option "activateFriendlyPositionAndSave" as default
    myDeletePersonPlan->setCheck(true);
}


void
GNEFixDemandElementsDialog::FixPersonPlanOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
}


bool 
GNEFixDemandElementsDialog::FixPersonPlanOptions::applyFixOption() {
    // check options for person plans
    if (myInvalidElements.size() > 0) {
        if (myDeletePersonPlan->getCheck() == TRUE) {
            // begin undo list
            myMainWindow->getUndoList()->begin(GUIIcon::MODEPERSONPLAN, TL("delete invalid person plans"));
            // remove all invalid person plans
            for (const auto& personPlan : myInvalidElements) {
                myMainWindow->getViewNet()->getNet()->deleteDemandElement(personPlan, myMainWindow->getUndoList());
            }
            myMainWindow->getUndoList()->end();
        } else if (mySelectInvalidPersonPlansAndCancel->getCheck() == TRUE) {
            myMainWindow->getUndoList()->begin(GUIIcon::MODEPERSONPLAN, TL("select invalid person plans"));
            // iterate over invalid person plans to select all elements
            for (const auto& personPlan : myInvalidElements) {
                personPlan->setAttribute(GNE_ATTR_SELECTED, "true", myMainWindow->getUndoList());
            }
            // end undo list
            myMainWindow->getUndoList()->end();
            // abort saving
            return false;
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixPersonPlanOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myDeletePersonPlan) {
        myDeletePersonPlan->setCheck(true);
        mySaveInvalid->setCheck(false);
        mySelectInvalidPersonPlansAndCancel->setCheck(false);
    } else if (obj == mySaveInvalid) {
        myDeletePersonPlan->setCheck(false);
        mySaveInvalid->setCheck(true);
        mySelectInvalidPersonPlansAndCancel->setCheck(false);
    } else if (obj == mySelectInvalidPersonPlansAndCancel) {
        myDeletePersonPlan->setCheck(false);
        mySaveInvalid->setCheck(false);
        mySelectInvalidPersonPlansAndCancel->setCheck(true);
    }
    return true;
}


void
GNEFixDemandElementsDialog::FixPersonPlanOptions::enableOptions() {
    myDeletePersonPlan->enable();
    mySaveInvalid->enable();
    mySelectInvalidPersonPlansAndCancel->enable();
}


void
GNEFixDemandElementsDialog::FixPersonPlanOptions::disableOptions() {
    myDeletePersonPlan->disable();
    mySaveInvalid->disable();
    mySelectInvalidPersonPlansAndCancel->disable();
}

/****************************************************************************/
