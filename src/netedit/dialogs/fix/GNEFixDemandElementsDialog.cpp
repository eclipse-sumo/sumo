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

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>

#include "GNEFixDemandElementsDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixDemandElementsDialog::FixRouteOptions) FixRouteOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixDemandElementsDialog::FixRouteOptions::onCmdSelectOption)
};

FXDEFMAP(GNEFixDemandElementsDialog::FixVehicleOptions) FixVehicleOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixDemandElementsDialog::FixVehicleOptions::onCmdSelectOption)
};

FXDEFMAP(GNEFixDemandElementsDialog::FixStopPositionOptions) FixStopPositionOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixDemandElementsDialog::FixStopPositionOptions::onCmdSelectOption)
};

FXDEFMAP(GNEFixDemandElementsDialog::FixPlanOptions) FixPlanOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixDemandElementsDialog::FixPlanOptions::onCmdSelectOption)
};

// Object abstract implementation
FXIMPLEMENT(GNEFixDemandElementsDialog::FixRouteOptions,        MFXGroupBoxModule, FixRouteOptionsMap,          ARRAYNUMBER(FixRouteOptionsMap))
FXIMPLEMENT(GNEFixDemandElementsDialog::FixVehicleOptions,      MFXGroupBoxModule, FixVehicleOptionsMap,        ARRAYNUMBER(FixVehicleOptionsMap))
FXIMPLEMENT(GNEFixDemandElementsDialog::FixStopPositionOptions, MFXGroupBoxModule, FixStopPositionOptionsMap,   ARRAYNUMBER(FixStopPositionOptionsMap))
FXIMPLEMENT(GNEFixDemandElementsDialog::FixPlanOptions,   MFXGroupBoxModule, FixPlanOptionsMap,     ARRAYNUMBER(FixPlanOptionsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixRouteOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixRouteOptions::FixRouteOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent, fixDemandElementsParent->myLeftFrame, TL("Routes")) {
    // Remove invalid routes
    myRemoveInvalidRoutes = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                            TL("Remove invalid routes"), "",
                            TL("Remove invalid routes and save"),
                            this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid routes
    mySaveInvalidRoutes = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                          TL("Save invalid routes"), "",
                          TL("Save invalid routes"),
                          this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid routes
    mySelectRouteInvalids = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                            TL("Select invalid routes"), "",
                            TL("Select invalid routes and stop saving"),
                            this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    myRemoveStopsOutOfRoute = GUIDesigns::buildFXCheckButton(myRightFrameOptions,
                              TL("Remove stops out of route"), "",
                              TL("Remove stops out of route"),
                              this, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // register options
    registerOption(myRemoveInvalidRoutes);
    registerOption(mySaveInvalidRoutes);
    registerOption(mySelectRouteInvalids);
    registerOption(myRemoveStopsOutOfRoute);
    // leave option "removeInvalidRoutes" as default
    myRemoveInvalidRoutes->setCheck(true);
    // ... and remove stops out of route
    myRemoveStopsOutOfRoute->setCheck(TRUE);
}


void
GNEFixDemandElementsDialog::FixRouteOptions::selectInternalTestSolution(const std::string& solution) {
    // choose solution
    if (solution == "saveRouteInvalids") {
        mySaveInvalidRoutes->setCheck(TRUE, TRUE);
    } else if (solution == "removeRouteInvalids") {
        myRemoveInvalidRoutes->setCheck(TRUE, TRUE);
    } else if (solution == "selectRouteInvalids") {
        mySelectRouteInvalids->setCheck(TRUE, TRUE);
    }
}


bool
GNEFixDemandElementsDialog::FixRouteOptions::applyFixOption() {
    if (myConflictedElements.size() > 0) {
        auto net = myFixElementDialogParent->getApplicationWindow()->getViewNet()->getNet();
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myRemoveInvalidRoutes->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::ROUTE, TL("delete invalid routes"));
            // iterate over invalid routes to delete it
            for (const auto& conflictedElement : myConflictedElements) {
                // special case for embedded routes
                if (conflictedElement.getElement()->getTagProperty()->getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                    net->deleteDemandElement(conflictedElement.getElement()->getParentDemandElements().front(), undoList);
                } else {
                    net->deleteDemandElement(conflictedElement.getElement(), undoList);
                }
            }
            // end undo list
            undoList->end();
        } else if (mySelectRouteInvalids->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::ROUTE, TL("select invalid routes"));
            // iterate over invalid single lane elements to select all elements
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
            // end undo list
            undoList->end();
            // abort saving
            return false;
        }
        // check if remove stops
        if (myRemoveStopsOutOfRoute->getCheck() == TRUE) {
            // get all stops to remove
            std::vector<GNEDemandElement*> stopsToRemove;
            for (const auto& conflictedElement : myConflictedElements) {
                const auto invaldstops = conflictedElement.getElement()->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            undoList->begin(GUIIcon::STOP, TL("delete invalid stops"));
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                net->deleteDemandElement(stopToRemove, undoList);
            }
            // end undo list
            undoList->end();
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

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixVehicleOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixVehicleOptions::FixVehicleOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent, fixDemandElementsParent->myLeftFrame, TL("Vehicles")) {
    // Remove invalid vehicles
    myRemoveInvalidVehicles = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                              TL("Remove invalid vehicles"), "",
                              TL("Remove invalid vehicles and save"),
                              this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid vehicles
    mySaveInvalidVehicles = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                            TL("Save invalid vehicles"), "",
                            TL("Save invalid vehicles"),
                            this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid vehicle
    mySelectInvalidVehicles = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                              TL("Select invalid vehicles"), "",
                              TL("Select invalid vehicles and stop saving"),
                              this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    myRemoveStopsOutOfVehicle = GUIDesigns::buildFXCheckButton(myRightFrameOptions,
                                TL("Remove stops out of route"), "",
                                TL("Remove stops out of vehicle's route"),
                                this, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // register options
    registerOption(myRemoveInvalidVehicles);
    registerOption(mySaveInvalidVehicles);
    registerOption(mySelectInvalidVehicles);
    registerOption(myRemoveStopsOutOfVehicle);
    // by default remove invalid vehicles
    myRemoveInvalidVehicles->setCheck(TRUE);
    // ... and remove stops out of route
    myRemoveStopsOutOfVehicle->setCheck(TRUE);
}


void
GNEFixDemandElementsDialog::FixVehicleOptions::selectInternalTestSolution(const std::string& /*solution*/) {
}


bool
GNEFixDemandElementsDialog::FixVehicleOptions::applyFixOption() {
    if (myConflictedElements.size() > 0) {
        auto net = myFixElementDialogParent->getApplicationWindow()->getViewNet()->getNet();
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myRemoveInvalidVehicles->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::VEHICLE, TL("delete invalid vehicles"));
            // iterate over invalid vehicles to delete it
            for (const auto& conflictedElement : myConflictedElements) {
                // check that vehicle was not removed previously in cascade
                if (net->getAttributeCarriers()->retrieveDemandElement(conflictedElement.getElement()->getTagProperty()->getTag(), conflictedElement.getID(), false) != nullptr) {
                    net->deleteDemandElement(conflictedElement.getElement(), undoList);
                }
            }
            // end undo list
            undoList->end();
        } else if (mySelectInvalidVehicles->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::ROUTE, TL("select invalid routes"));
            // iterate over invalid single lane elements to select all elements
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
            // end undo list
            undoList->end();
            // abort saving
            return false;
        }
        // check if remove stops
        if (myRemoveStopsOutOfVehicle->getCheck() == TRUE) {
            // get all stops to remove
            std::vector<GNEDemandElement*> stopsToRemove;
            for (const auto& conflictedElement : myConflictedElements) {
                const auto invaldstops = conflictedElement.getElement()->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            undoList->begin(GUIIcon::STOP, TL("delete invalid stops"));
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                net->deleteDemandElement(stopToRemove, undoList);
            }
            // end undo list
            undoList->end();
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixVehicleOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myRemoveInvalidVehicles) {
        myRemoveInvalidVehicles->setCheck(true);
        mySaveInvalidVehicles->setCheck(false);
        mySelectInvalidVehicles->setCheck(false);
    } else if (obj == mySaveInvalidVehicles) {
        myRemoveInvalidVehicles->setCheck(false);
        mySaveInvalidVehicles->setCheck(true);
        mySelectInvalidVehicles->setCheck(false);
    } else if (obj == mySelectInvalidVehicles) {
        myRemoveInvalidVehicles->setCheck(false);
        mySaveInvalidVehicles->setCheck(false);
        mySelectInvalidVehicles->setCheck(true);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixStopPositionOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixStopPositionOptions::FixStopPositionOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent, fixDemandElementsParent->myRightFrame, TL("Stop positions")) {
    // Activate friendlyPos and save
    myActivateFriendlyPosition = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                                 TL("Activate friendlyPos"), "",
                                 TL("Activate friendlyPos and save"),
                                 this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid position
    mySaveInvalid = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                    TL("Save invalid stops"), "",
                    TL("Save elements with invalid positions"),
                    this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid Stops
    mySelectInvalidStops = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                           TL("Select conflicted stops"), "",
                           TL("Select conflicted stops and abort saving"),
                           this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Fix positions and save
    myFixPositions = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                     TL("Fix positions and save"), "",
                     TL("Fix stop positions and save"),
                     this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // register options
    registerOption(myActivateFriendlyPosition);
    registerOption(mySaveInvalid);
    registerOption(mySelectInvalidStops);
    registerOption(myFixPositions);
    // leave option "activateFriendlyPositionAndSave" as default
    myActivateFriendlyPosition->setCheck(true);
}


void
GNEFixDemandElementsDialog::FixStopPositionOptions::selectInternalTestSolution(const std::string& /*solution*/) {
}


bool
GNEFixDemandElementsDialog::FixStopPositionOptions::applyFixOption() {
    // check options for stops
    if (myConflictedElements.size() > 0) {
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myActivateFriendlyPosition->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::STOP, TLF("change % of invalid stops", toString(SUMO_ATTR_FRIENDLY_POS)));
            // iterate over invalid stops to enable friendly position
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", undoList);
            }
            undoList->end();
        } else if (myFixPositions->getCheck() == TRUE) {
            undoList->begin(GUIIcon::STOP, TL("fix positions of invalid stops"));
            // iterate over invalid stops to fix positions
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->fixDemandElementProblem();
            }
            undoList->end();
        } else if (mySelectInvalidStops->getCheck() == TRUE) {
            undoList->begin(GUIIcon::STOP, TL("select invalid stops"));
            // iterate over invalid stops to select all elements
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
            // end undo list
            undoList->end();
            // abort saving
            return false;
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixStopPositionOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myActivateFriendlyPosition) {
        myActivateFriendlyPosition->setCheck(true);
        myFixPositions->setCheck(false);
        mySaveInvalid->setCheck(false);
        mySelectInvalidStops->setCheck(false);
    } else if (obj == myFixPositions) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(true);
        mySaveInvalid->setCheck(false);
        mySelectInvalidStops->setCheck(false);
    } else if (obj == mySaveInvalid) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(false);
        mySaveInvalid->setCheck(true);
        mySelectInvalidStops->setCheck(false);
    } else if (obj == mySelectInvalidStops) {
        myActivateFriendlyPosition->setCheck(false);
        myFixPositions->setCheck(false);
        mySaveInvalid->setCheck(false);
        mySelectInvalidStops->setCheck(true);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog::FixPlanOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::FixPlanOptions::FixPlanOptions(GNEFixDemandElementsDialog* fixDemandElementsParent) :
    FixOptions(fixDemandElementsParent, fixDemandElementsParent->myRightFrame, TL("Person/container plans")) {
    // Delete person plan
    myRemoveInvalidPlan = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                          TL("Remove invalid plans"), "",
                          TL("Remove invalid plans"),
                          this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid person plans
    mySaveInvalid = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                    TL("Save invalid plans"), "",
                    TL("Save invalid plans"),
                    this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid person plans
    mySelectInvalidPlans = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                           TL("Select conflicted plans"), "",
                           TL("Select conflicted plans and abort saving"),
                           this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // register options
    registerOption(myRemoveInvalidPlan);
    registerOption(mySaveInvalid);
    registerOption(mySelectInvalidPlans);
    // set option "activateFriendlyPositionAndSave" as default
    myRemoveInvalidPlan->setCheck(true);
}


void
GNEFixDemandElementsDialog::FixPlanOptions::selectInternalTestSolution(const std::string& /*solution*/) {
}


bool
GNEFixDemandElementsDialog::FixPlanOptions::applyFixOption() {
    // check options for person plans
    if (myConflictedElements.size() > 0) {
        auto net = myFixElementDialogParent->getApplicationWindow()->getViewNet()->getNet();
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myRemoveInvalidPlan->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::MODEPERSONPLAN, TL("delete invalid person plans"));
            // remove all invalid person plans
            for (const auto& conflictedElement : myConflictedElements) {
                net->deleteDemandElement(conflictedElement.getElement(), undoList);
            }
            undoList->end();
        } else if (mySelectInvalidPlans->getCheck() == TRUE) {
            undoList->begin(GUIIcon::MODEPERSONPLAN, TL("select invalid person plans"));
            // iterate over invalid person plans to select all elements
            for (const auto& conflictedElement : myConflictedElements) {
                conflictedElement.getElement()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
            // end undo list
            undoList->end();
            // abort saving
            return false;
        }
    }
    return true;
}


long
GNEFixDemandElementsDialog::FixPlanOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myRemoveInvalidPlan) {
        myRemoveInvalidPlan->setCheck(true);
        mySaveInvalid->setCheck(false);
        mySelectInvalidPlans->setCheck(false);
    } else if (obj == mySaveInvalid) {
        myRemoveInvalidPlan->setCheck(false);
        mySaveInvalid->setCheck(true);
        mySelectInvalidPlans->setCheck(false);
    } else if (obj == mySelectInvalidPlans) {
        myRemoveInvalidPlan->setCheck(false);
        mySaveInvalid->setCheck(false);
        mySelectInvalidPlans->setCheck(true);
    }
    return true;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElementsDialog - methods
// ---------------------------------------------------------------------------

GNEFixDemandElementsDialog::GNEFixDemandElementsDialog(GNEApplicationWindow* mainWindow,
        const std::vector<GNEDemandElement*>& elements) :
    GNEFixElementsDialog(mainWindow, TL("Fix demand elements problems"), GUIIcon::SUPERMODEDEMAND,
                         DialogType::FIX_DEMANDELEMENTS) {
    // create fix route options
    myFixRouteOptions = new FixRouteOptions(this);
    // create fix vehicle  options
    myFixVehicleOptions = new FixVehicleOptions(this);
    // create fix stops options
    myFixStopPositionOptions = new FixStopPositionOptions(this);
    // create fix person plans options
    myFixPlanOptions = new FixPlanOptions(this);
    // split invalidDemandElements in four groups
    std::vector<ConflictElement> invalidRoutes, invalidVehicles, invalidStops, invalidPlans;
    // fill groups
    for (const auto& invalidDemandElement : elements) {
        // create conflict element
        auto fixElement = ConflictElement(invalidDemandElement,
                                          invalidDemandElement->getID(),
                                          invalidDemandElement->getACIcon(),
                                          invalidDemandElement->getDemandElementProblem());
        // add depending of element type
        if (invalidDemandElement->getTagProperty()->isRoute()) {
            invalidRoutes.push_back(fixElement);
        } else if (invalidDemandElement->getTagProperty()->isVehicle()) {
            invalidVehicles.push_back(fixElement);
        } else if (invalidDemandElement->getTagProperty()->isVehicleStop()) {
            invalidStops.push_back(fixElement);
        } else {
            invalidPlans.push_back(fixElement);
        }
    }
    // fill options
    myFixRouteOptions->setInvalidElements(invalidRoutes);
    myFixVehicleOptions->setInvalidElements(invalidVehicles);
    myFixStopPositionOptions->setInvalidElements(invalidStops);
    myFixPlanOptions->setInvalidElements(invalidPlans);
    // open modal dialog
    openDialog();
}


GNEFixDemandElementsDialog::~GNEFixDemandElementsDialog() {
}

/****************************************************************************/
