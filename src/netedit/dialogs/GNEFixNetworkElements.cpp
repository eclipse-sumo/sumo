/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEFixNetworkElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog used to fix network elements during saving
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEFixNetworkElements.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixNetworkElements) GNEFixNetworkElementsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixNetworkElements::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEFixNetworkElements::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEFixNetworkElements::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEFixNetworkElements, FXDialogBox, GNEFixNetworkElementsMap, ARRAYNUMBER(GNEFixNetworkElementsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFixNetworkElements - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::GNEFixNetworkElements(GNEViewNet* viewNet, const std::vector<GNENetworkElement*>& invalidNetworkElements) :
    FXDialogBox(viewNet->getApp(), "Fix network elements problems", GUIDesignDialogBoxExplicit(800, 620)),
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
    // create buttons
    myButtons = new Buttons(this);
    // split invalidNetworkElements in four groups
    std::vector<GNENetworkElement*> invalidRoutes, invalidVehicles, invalidStops, invalidPlans;
    // fill groups
    for (const auto& invalidNetworkElement : invalidNetworkElements) {
        if (invalidNetworkElement->getTagProperty().isRoute()) {
            invalidRoutes.push_back(invalidNetworkElement);
        } else if (invalidNetworkElement->getTagProperty().isVehicle()) {
            invalidVehicles.push_back(invalidNetworkElement);
        } else if (invalidNetworkElement->getTagProperty().isStop()) {
            invalidStops.push_back(invalidNetworkElement);
        } else {
            invalidPlans.push_back(invalidNetworkElement);
        }
    }
    // fill options
    myFixRouteOptions->setInvalidElements(invalidRoutes);
    myFixVehicleOptions->setInvalidElements(invalidVehicles);
}


GNEFixNetworkElements::~GNEFixNetworkElements() {
}


long
GNEFixNetworkElements::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    // select options
    myFixRouteOptions->selectOption(obj);
    myFixVehicleOptions->selectOption(obj);
    return 1;
}


long
GNEFixNetworkElements::onCmdAccept(FXObject*, FXSelector, void*) {
    bool abortSaving = false;
    // fix elements
    myFixRouteOptions->fixElements(abortSaving);
    myFixVehicleOptions->fixElements(abortSaving);
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
GNEFixNetworkElements::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal (abort saving)
    getApp()->stopModal(this, FALSE);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixOptions::FixOptions(FXVerticalFrame* frameParent, const std::string& title, GNEViewNet* viewNet) :
    FXGroupBoxModule(frameParent, title, FXGroupBoxModule::Options::SAVE),
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
GNEFixNetworkElements::FixOptions::setInvalidElements(const std::vector<GNENetworkElement*>& invalidElements) {
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
        item = new FXTableItem(myInvalidElements.at(i)->getNetworkElementProblem().c_str());
        item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
        myTable->setItem(i, 2, item);
    }
    // check if enable or disable options
    if (invalidElements.size() > 0) {
        enableOptions();
        toogleSaveButton(true);
    } else {
        disableOptions();
        toogleSaveButton(false);
    }
}

bool
GNEFixNetworkElements::FixOptions::saveContents() const {
    const FXString file = MFXUtils::getFilename2Write(myTable,
                          "Save list of conflicted items", ".txt",
                          GUIIconSubSys::getIcon(GUIIcon::SAVE), gCurrentFolder);
    if (file == "") {
        return false;
    }
    try {
        // open output device
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        // get invalid element ID and problem
        for (const auto& invalidElement : myInvalidElements) {
            dev << invalidElement->getID() << ":" << invalidElement->getNetworkElementProblem() << "\n";
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
// GNEFixNetworkElements::FixRouteOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixRouteOptions::FixRouteOptions(GNEFixNetworkElements* fixNetworkElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixNetworkElementsParent->myLeftFrame, "Routes", viewNet) {
    // Remove invalid routes
    removeInvalidRoutes = new FXRadioButton(myLeftFrame, "Remove invalid routes",
                                            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid routes
    saveInvalidRoutes = new FXRadioButton(myLeftFrame, "Save invalid routes",
                                          fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid routes
    selectInvalidRoutesAndCancel = new FXRadioButton(myRightFrame, "Select conflicted routes",
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    removeStopsOutOfRoute = new FXCheckButton(myRightFrame, "Remove stops out of route",
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // leave option "removeInvalidRoutes" as default
    removeInvalidRoutes->setCheck(true);
    // ... and remove stops out of route
    removeStopsOutOfRoute->setCheck(TRUE);
}


void
GNEFixNetworkElements::FixRouteOptions::selectOption(FXObject* option) {
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
GNEFixNetworkElements::FixRouteOptions::fixElements(bool& abortSaving) {
    if (myInvalidElements.size() > 0) {
        if (removeInvalidRoutes->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid routes");
            // iterate over invalid routes to delete it
            for (const auto& invalidRoute : myInvalidElements) {
                // special case for embedded routes
                if (invalidRoute->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                    myViewNet->getNet()->deleteNetworkElement(invalidRoute->getParentNetworkElements().front(), myViewNet->getUndoList());
                } else {
                    myViewNet->getNet()->deleteNetworkElement(invalidRoute, myViewNet->getUndoList());
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
            std::vector<GNENetworkElement*> stopsToRemove;
            for (const auto& invalidRoute : myInvalidElements) {
                const auto invaldstops = invalidRoute->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid stops");
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                myViewNet->getNet()->deleteNetworkElement(stopToRemove, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        }
    }
}


void
GNEFixNetworkElements::FixRouteOptions::enableOptions() {
    removeInvalidRoutes->enable();
    saveInvalidRoutes->enable();
    selectInvalidRoutesAndCancel->enable();
    removeStopsOutOfRoute->enable();
}


void
GNEFixNetworkElements::FixRouteOptions::disableOptions() {
    removeInvalidRoutes->disable();
    saveInvalidRoutes->disable();
    selectInvalidRoutesAndCancel->disable();
    removeStopsOutOfRoute->disable();
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixVehicleOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixVehicleOptions::FixVehicleOptions(GNEFixNetworkElements* fixNetworkElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixNetworkElementsParent->myLeftFrame, "Vehicles", viewNet) {
    // Remove invalid vehicles
    removeInvalidVehicles = new FXRadioButton(myLeftFrame, "Remove invalid vehicles",
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid vehicles
    saveInvalidVehicles = new FXRadioButton(myLeftFrame, "Save invalid vehicles",
                                            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid vehicle
    selectInvalidVehiclesAndCancel = new FXRadioButton(myRightFrame, "Select conflicted vehicle",
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Remove stops out of route
    removeStopsOutOfVehicle = new FXCheckButton(myRightFrame, "Remove stops out of vehicle's route",
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignCheckButtonFix);
    // by default remove invalid vehicles
    removeInvalidVehicles->setCheck(TRUE);
    // ... and remove stops out of route
    removeStopsOutOfVehicle->setCheck(TRUE);
}


void
GNEFixNetworkElements::FixVehicleOptions::selectOption(FXObject* option) {
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
GNEFixNetworkElements::FixVehicleOptions::fixElements(bool& abortSaving) {
    if (myInvalidElements.size() > 0) {
        if (removeInvalidVehicles->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid vehicles");
            // iterate over invalid vehicles to delete it
            for (const auto& invalidVehicle : myInvalidElements) {
                myViewNet->getNet()->deleteNetworkElement(invalidVehicle, myViewNet->getUndoList());
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
            std::vector<GNENetworkElement*> stopsToRemove;
            for (const auto& invalidVehicle : myInvalidElements) {
                const auto invaldstops = invalidVehicle->getInvalidStops();
                // append to stopsToRemove
                stopsToRemove.insert(stopsToRemove.end(), invaldstops.begin(), invaldstops.end());
            }
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid stops");
            // remove all
            for (const auto& stopToRemove : stopsToRemove) {
                myViewNet->getNet()->deleteNetworkElement(stopToRemove, myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        }
    }
}


void
GNEFixNetworkElements::FixVehicleOptions::enableOptions() {
    removeInvalidVehicles->enable();
    saveInvalidVehicles->enable();
    selectInvalidVehiclesAndCancel->enable();
    removeStopsOutOfVehicle->enable();
}


void
GNEFixNetworkElements::FixVehicleOptions::disableOptions() {
    removeInvalidVehicles->disable();
    saveInvalidVehicles->disable();
    selectInvalidVehiclesAndCancel->disable();
    removeStopsOutOfVehicle->disable();
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::Buttons - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::Buttons::Buttons(GNEFixNetworkElements* fixNetworkElementsParent) :
    FXHorizontalFrame(fixNetworkElementsParent->myMainFrame, GUIDesignHorizontalFrame) {
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(this, FXWindow::tr("&Accept"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), fixNetworkElementsParent, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(this, FXWindow::tr("&Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), fixNetworkElementsParent, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}

/****************************************************************************/
