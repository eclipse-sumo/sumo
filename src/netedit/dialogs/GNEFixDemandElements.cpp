/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
    FXDialogBox(viewNet->getApp(), "Fix demand elements problems", GUIDesignDialogBoxExplicitStretchable(800, 620)),
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
    myFixRouteOptions = new FixRouteOptions(this, viewNet);
    // create fix vehicle  options
    myFixVehicleOptions = new FixVehicleOptions(this, viewNet);
    // create fix stops options
    myFixStopPositionOptions = new FixStopPositionOptions(this, viewNet);
    // create fix person plans options
    myFixPersonPlanOptions = new FixPersonPlanOptions(this, viewNet);
    // create buttons
    myButtons = new Buttons(this);
    // split invalidDemandElements in four groups
    std::vector<GNEDemandElement*> invalidRoutes, invalidVehicles, invalidStops, invalidPlans;
    // fill groups
    for (const auto& invalidDemandElement : invalidDemandElements) {
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
    myFixStopPositionOptions->setInvalidElements(invalidStops);
    myFixPersonPlanOptions->setInvalidElements(invalidPlans);
}


GNEFixDemandElements::~GNEFixDemandElements() {
}


long
GNEFixDemandElements::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    // select options
    myFixRouteOptions->selectOption(obj);
    myFixVehicleOptions->selectOption(obj);
    myFixStopPositionOptions->selectOption(obj);
    myFixPersonPlanOptions->selectOption(obj);
    return 1;
}


long
GNEFixDemandElements::onCmdAccept(FXObject*, FXSelector, void*) {
    bool abortSaving = false;
    // fix elements
    myFixRouteOptions->fixElements(abortSaving);
    myFixVehicleOptions->fixElements(abortSaving);
    myFixStopPositionOptions->fixElements(abortSaving);
    myFixPersonPlanOptions->fixElements(abortSaving);
    // check if abort saving
    if (abortSaving) {
        // stop modal with TRUE (abort saving)
        getApp()->stopModal(this, FALSE);
    } else {
        // stop modal with TRUE (continue saving)
        getApp()->stopModal(this, TRUE);
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

GNEFixDemandElements::FixOptions::FixOptions(FXVerticalFrame* frameParent, const std::string& title, GNEViewNet* viewNet) :
    MFXGroupBoxModule(frameParent, title, MFXGroupBoxModule::Options::SAVE),
    myViewNet(viewNet) {
    // Create table
    myTable = new FXTable(this, this, MID_TABLE, GUIDesignTableFixElements);
    myTable->disable();
    // create horizontal frame
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create vertical frames
    myLeftFrame = new FXVerticalFrame(horizontalFrame, GUIDesignAuxiliarVerticalFrame);
    myRightFrame = new FXVerticalFrame(horizontalFrame, GUIDesignAuxiliarVerticalFrame);
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
        item = new FXTableItem("", myInvalidElements.at(i)->getACIcon());
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
    // check if enable or disable options
    if (invalidElements.size() > 0) {
        enableOptions();
        toggleSaveButton(true);
    } else {
        disableOptions();
        toggleSaveButton(false);
    }
}

bool
GNEFixDemandElements::FixOptions::saveContents() const {
    const FXString file = MFXUtils::getFilename2Write(myTable,
                          TL("Save list of conflicted items"), ".txt",
                          GUIIconSubSys::getIcon(GUIIcon::SAVE), gCurrentFolder);
    if (file == "") {
        return false;
    }
    try {
        // open output device
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        // get invalid element ID and problem
        for (const auto& invalidElement : myInvalidElements) {
            dev << invalidElement->getID() << ":" << invalidElement->getDemandElementProblem() << "\n";
        }
        // close output device
        dev.close();
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'Saving list of conflicted items successfully'");
        // open message box error
        FXMessageBox::information(myTable, MBOX_OK, "Saving successfully", "%s", "List of conflicted items was successfully saved");
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'Saving list of conflicted items successfully' with 'OK'");
    } catch (IOError& e) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'error saving list of conflicted items'");
        // open message box error
        FXMessageBox::error(myTable, MBOX_OK, "Saving list of conflicted items failed", "%s", e.what());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'error saving list of conflicted items' with 'OK'");
    }
    return true;
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixRouteOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixRouteOptions::FixRouteOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixDemandElementsParent->myLeftFrame, "Routes", viewNet) {
    // Remove invalid routes
    removeInvalidRoutes = new FXRadioButton(myLeftFrame, TL("Remove invalid routes"),
                                            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid routes
    saveInvalidRoutes = new FXRadioButton(myLeftFrame, TL("Save invalid routes"),
                                          fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid routes
    selectInvalidRoutesAndCancel = new FXRadioButton(myRightFrame, TL("Select conflicted routes"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    removeStopsOutOfRoute = new FXCheckButton(myRightFrame, TL("Remove stops out of route"),
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
GNEFixDemandElements::FixRouteOptions::fixElements(bool& abortSaving) {
    if (myInvalidElements.size() > 0) {
        if (removeInvalidRoutes->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid routes");
            // iterate over invalid routes to delete it
            for (const auto& invalidRoute : myInvalidElements) {
                // special case for embedded routes
                if (invalidRoute->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                    myViewNet->getNet()->deleteDemandElement(invalidRoute->getParentDemandElements().front(), myViewNet->getUndoList());
                } else {
                    myViewNet->getNet()->deleteDemandElement(invalidRoute, myViewNet->getUndoList());
                }
            }
            // end undo list
            myViewNet->getUndoList()->end();
        } else if (selectInvalidRoutesAndCancel->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "select invalid routes");
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidRoute : myInvalidElements) {
                invalidRoute->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            abortSaving = true;
        }
        // check if remove stops
        if (removeStopsOutOfRoute->getCheck() == TRUE) {
            // get all stops to remove
            std::vector<GNEDemandElement*> stopsToRemove;
            for (const auto& invalidRoute : myInvalidElements) {
                const auto invaldstops = invalidRoute->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid stops");
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                myViewNet->getNet()->deleteDemandElement(stopToRemove, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        }
    }
}


void
GNEFixDemandElements::FixRouteOptions::enableOptions() {
    removeInvalidRoutes->enable();
    saveInvalidRoutes->enable();
    selectInvalidRoutesAndCancel->enable();
    removeStopsOutOfRoute->enable();
}


void
GNEFixDemandElements::FixRouteOptions::disableOptions() {
    removeInvalidRoutes->disable();
    saveInvalidRoutes->disable();
    selectInvalidRoutesAndCancel->disable();
    removeStopsOutOfRoute->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixVehicleOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixVehicleOptions::FixVehicleOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixDemandElementsParent->myLeftFrame, "Vehicles", viewNet) {
    // Remove invalid vehicles
    removeInvalidVehicles = new FXRadioButton(myLeftFrame, TL("Remove invalid vehicles"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid vehicles
    saveInvalidVehicles = new FXRadioButton(myLeftFrame, TL("Save invalid vehicles"),
                                            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid vehicle
    selectInvalidVehiclesAndCancel = new FXRadioButton(myRightFrame, TL("Select conflicted vehicle"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    removeStopsOutOfVehicle = new FXCheckButton(myRightFrame, TL("Remove stops out of vehicle's route"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // by default remove invalid vehicles
    removeInvalidVehicles->setCheck(TRUE);
    // ... and remove stops out of route
    removeStopsOutOfVehicle->setCheck(TRUE);
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
GNEFixDemandElements::FixVehicleOptions::fixElements(bool& abortSaving) {
    if (myInvalidElements.size() > 0) {
        if (removeInvalidVehicles->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid vehicles");
            // iterate over invalid vehicles to delete it
            for (const auto& invalidVehicle : myInvalidElements) {
                myViewNet->getNet()->deleteDemandElement(invalidVehicle, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        } else if (selectInvalidVehiclesAndCancel->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "select invalid routes");
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidVehicle : myInvalidElements) {
                invalidVehicle->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            abortSaving = true;
        }
        // check if remove stops
        if (removeStopsOutOfVehicle->getCheck() == TRUE) {
            // get all stops to remove
            std::vector<GNEDemandElement*> stopsToRemove;
            for (const auto& invalidVehicle : myInvalidElements) {
                const auto invaldstops = invalidVehicle->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid stops");
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                myViewNet->getNet()->deleteDemandElement(stopToRemove, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        }
    }
}


void
GNEFixDemandElements::FixVehicleOptions::enableOptions() {
    removeInvalidVehicles->enable();
    saveInvalidVehicles->enable();
    selectInvalidVehiclesAndCancel->enable();
    removeStopsOutOfVehicle->enable();
}


void
GNEFixDemandElements::FixVehicleOptions::disableOptions() {
    removeInvalidVehicles->disable();
    saveInvalidVehicles->disable();
    selectInvalidVehiclesAndCancel->disable();
    removeStopsOutOfVehicle->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixStopPositionOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixStopPositionOptions::FixStopPositionOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixDemandElementsParent->myRightFrame, "Stop positions", viewNet) {
    // Activate friendlyPos and save
    activateFriendlyPositionAndSave = new FXRadioButton(myLeftFrame, TL("Activate friendlyPos and save"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid position
    saveInvalid = new FXRadioButton(myLeftFrame, TL("Save invalid positions"),
                                    fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid Stops
    selectInvalidStopsAndCancel = new FXRadioButton(myRightFrame, TL("Select conflicted Stops"),
            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Fix positions and save
    fixPositionsAndSave = new FXRadioButton(myRightFrame, TL("Fix positions and save"),
                                            fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // leave option "activateFriendlyPositionAndSave" as default
    activateFriendlyPositionAndSave->setCheck(true);
}


void
GNEFixDemandElements::FixStopPositionOptions::selectOption(FXObject* option) {
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
GNEFixDemandElements::FixStopPositionOptions::fixElements(bool& abortSaving) {
    // check options for stops
    if (myInvalidElements.size() > 0) {
        if (activateFriendlyPositionAndSave->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::STOP, "change " + toString(SUMO_ATTR_FRIENDLY_POS) + " of invalid stops");
            // iterate over invalid stops to enable friendly position
            for (const auto& stop : myInvalidElements) {
                stop->setAttribute(SUMO_ATTR_FRIENDLY_POS, "true", myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
        } else if (fixPositionsAndSave->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(GUIIcon::STOP, "fix positions of invalid stops");
            // iterate over invalid stops to fix positions
            for (const auto& stop : myInvalidElements) {
                stop->fixDemandElementProblem();
            }
            myViewNet->getUndoList()->end();
        } else if (selectInvalidStopsAndCancel->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(GUIIcon::STOP, "select invalid stops");
            // iterate over invalid stops to select all elements
            for (const auto& stop : myInvalidElements) {
                stop->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            abortSaving = true;
        }
    }
}


void
GNEFixDemandElements::FixStopPositionOptions::enableOptions() {
    activateFriendlyPositionAndSave->enable();
    fixPositionsAndSave->enable();
    saveInvalid->enable();
    selectInvalidStopsAndCancel->enable();
}


void
GNEFixDemandElements::FixStopPositionOptions::disableOptions() {
    activateFriendlyPositionAndSave->disable();
    fixPositionsAndSave->disable();
    saveInvalid->disable();
    selectInvalidStopsAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixDemandElements::FixPersonPlanOptions - methods
// ---------------------------------------------------------------------------

GNEFixDemandElements::FixPersonPlanOptions::FixPersonPlanOptions(GNEFixDemandElements* fixDemandElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixDemandElementsParent->myRightFrame, "Person/container plans", viewNet) {
    // Delete person plan
    deletePersonPlan = new FXRadioButton(myLeftFrame, TL("Delete person plan"),
                                         fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid person plans
    saveInvalid = new FXRadioButton(myLeftFrame, TL("Save invalid person plans"),
                                    fixDemandElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid person plans
    selectInvalidPersonPlansAndCancel = new FXRadioButton(myRightFrame, TL("Select conflicted person plans"),
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
GNEFixDemandElements::FixPersonPlanOptions::fixElements(bool& abortSaving) {
    // check options for person plans
    if (myInvalidElements.size() > 0) {
        if (deletePersonPlan->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::MODEPERSONPLAN, "delete invalid person plans");
            // remove all invalid person plans
            for (const auto& personPlan : myInvalidElements) {
                myViewNet->getNet()->deleteDemandElement(personPlan, myViewNet->getUndoList());
            }
            myViewNet->getUndoList()->end();
        } else if (selectInvalidPersonPlansAndCancel->getCheck() == TRUE) {
            myViewNet->getUndoList()->begin(GUIIcon::MODEPERSONPLAN, "select invalid person plans");
            // iterate over invalid person plans to select all elements
            for (const auto& personPlan : myInvalidElements) {
                personPlan->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            abortSaving = false;
        }
    }
}


void
GNEFixDemandElements::FixPersonPlanOptions::enableOptions() {
    deletePersonPlan->enable();
    saveInvalid->enable();
    selectInvalidPersonPlansAndCancel->enable();
}


void
GNEFixDemandElements::FixPersonPlanOptions::disableOptions() {
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
    myAcceptButton = new FXButton(this, TL("&Accept"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), fixDemandElementsParent, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(this, TL("&Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), fixDemandElementsParent, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}

/****************************************************************************/
