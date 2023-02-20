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
/// @file    GNEFixNetworkElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog used to fix network elements during saving
/****************************************************************************/

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
    FXDialogBox(viewNet->getApp(), TL("Fix network elements problems"), GUIDesignDialogBoxExplicitStretchable(600, 620)),
    myViewNet(viewNet) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
    // create main frame
    myMainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frames for options
    FXHorizontalFrame* optionsFrame = new FXHorizontalFrame(myMainFrame, GUIDesignAuxiliarFrame);
    myLeftFrame = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
    myRightFrame = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
    // create fix edge options
    myFixEdgeOptions = new FixEdgeOptions(this, viewNet);
    // create fix crossing  options
    myFixCrossingOptions = new FixCrossingOptions(this, viewNet);
    // create buttons
    myButtons = new Buttons(this);
    // split invalidNetworkElements in four groups
    std::vector<GNENetworkElement*> invalidEdges, invalidCrossings;
    // fill groups
    for (const auto& invalidNetworkElement : invalidNetworkElements) {
        if (invalidNetworkElement->getTagProperty().getTag() == SUMO_TAG_EDGE) {
            invalidEdges.push_back(invalidNetworkElement);
        } else if (invalidNetworkElement->getTagProperty().getTag() == SUMO_TAG_CROSSING) {
            invalidCrossings.push_back(invalidNetworkElement);
        }
    }
    // fill options
    myFixEdgeOptions->setInvalidElements(invalidEdges);
    myFixCrossingOptions->setInvalidElements(invalidCrossings);
}


GNEFixNetworkElements::~GNEFixNetworkElements() {
}


long
GNEFixNetworkElements::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    // select options
    myFixEdgeOptions->selectOption(obj);
    myFixCrossingOptions->selectOption(obj);
    return 1;
}


long
GNEFixNetworkElements::onCmdAccept(FXObject*, FXSelector, void*) {
    bool abortSaving = false;
    // fix elements
    myFixEdgeOptions->fixElements(abortSaving);
    myFixCrossingOptions->fixElements(abortSaving);
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
    myTable->setColumnWidth(2, 390);
    myTable->setColumnText(0, "");
    myTable->setColumnText(1, toString(SUMO_ATTR_ID).c_str());
    myTable->setColumnText(2, "Conflict");
    myTable->getRowHeader()->setWidth(0);
    // Declare pointer to FXTableItem
    FXTableItem* item = nullptr;
    // iterate over invalid edges
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
        item = new FXTableItem(myInvalidElements.at(i)->getNetworkElementProblem().c_str());
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
GNEFixNetworkElements::FixOptions::saveContents() const {
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
            dev << invalidElement->getID() << ":" << invalidElement->getNetworkElementProblem() << "\n";
        }
        // close output device
        dev.close();
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'Saving list of conflicted items successfully'");
        // open message box error
        FXMessageBox::information(myTable, MBOX_OK, TL("Saving successfully"), "%s", "List of conflicted items was successfully saved");
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'Saving list of conflicted items successfully' with 'OK'");
    } catch (IOError& e) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'error saving list of conflicted items'");
        // open message box error
        FXMessageBox::error(myTable, MBOX_OK, TL("Saving list of conflicted items failed"), "%s", e.what());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'error saving list of conflicted items' with 'OK'");
    }
    return true;
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixEdgeOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixEdgeOptions::FixEdgeOptions(GNEFixNetworkElements* fixNetworkElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixNetworkElementsParent->myLeftFrame, "Edges", viewNet) {
    // Remove invalid edges
    removeInvalidEdges = new FXRadioButton(myLeftFrame, TL("Remove invalid edges"),
                                           fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid edges
    saveInvalidEdges = new FXRadioButton(myLeftFrame, TL("Save invalid edges"),
                                         fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid edges
    selectInvalidEdgesAndCancel = new FXRadioButton(myRightFrame, TL("Select conflicted edges"),
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // leave option "removeInvalidEdges" as default
    removeInvalidEdges->setCheck(true);
}


void
GNEFixNetworkElements::FixEdgeOptions::selectOption(FXObject* option) {
    if (option == removeInvalidEdges) {
        removeInvalidEdges->setCheck(true);
        saveInvalidEdges->setCheck(false);
        selectInvalidEdgesAndCancel->setCheck(false);
    } else if (option == saveInvalidEdges) {
        removeInvalidEdges->setCheck(false);
        saveInvalidEdges->setCheck(true);
        selectInvalidEdgesAndCancel->setCheck(false);
    } else if (option == selectInvalidEdgesAndCancel) {
        removeInvalidEdges->setCheck(false);
        saveInvalidEdges->setCheck(false);
        selectInvalidEdgesAndCancel->setCheck(true);
    }
}


void
GNEFixNetworkElements::FixEdgeOptions::fixElements(bool& abortSaving) {
    if (myInvalidElements.size() > 0) {
        if (removeInvalidEdges->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid edges");
            // iterate over invalid edges to delete it
            for (const auto& invalidEdge : myInvalidElements) {
                myViewNet->getNet()->deleteEdge(myViewNet->getNet()->getAttributeCarriers()->retrieveEdge(invalidEdge->getID()), myViewNet->getUndoList(), false);
            }
            // end undo list
            myViewNet->getUndoList()->end();
        } else if (selectInvalidEdgesAndCancel->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "select invalid edges");
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidEdge : myInvalidElements) {
                invalidEdge->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            abortSaving = true;
        }
    }
}


void
GNEFixNetworkElements::FixEdgeOptions::enableOptions() {
    removeInvalidEdges->enable();
    saveInvalidEdges->enable();
    selectInvalidEdgesAndCancel->enable();
}


void
GNEFixNetworkElements::FixEdgeOptions::disableOptions() {
    removeInvalidEdges->disable();
    saveInvalidEdges->disable();
    selectInvalidEdgesAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixCrossingOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixCrossingOptions::FixCrossingOptions(GNEFixNetworkElements* fixNetworkElementsParent, GNEViewNet* viewNet) :
    FixOptions(fixNetworkElementsParent->myLeftFrame, "Crossings", viewNet) {
    // Remove invalid crossings
    removeInvalidCrossings = new FXRadioButton(myLeftFrame, TL("Remove invalid crossings"),
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid crossings
    saveInvalidCrossings = new FXRadioButton(myLeftFrame, TL("Save invalid crossings"),
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid crossing
    selectInvalidCrossingsAndCancel = new FXRadioButton(myRightFrame, TL("Select conflicted crossing"),
            fixNetworkElementsParent, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // by default remove invalid crossings
    removeInvalidCrossings->setCheck(TRUE);
}


void
GNEFixNetworkElements::FixCrossingOptions::selectOption(FXObject* option) {
    if (option == removeInvalidCrossings) {
        removeInvalidCrossings->setCheck(true);
        saveInvalidCrossings->setCheck(false);
        selectInvalidCrossingsAndCancel->setCheck(false);
    } else if (option == saveInvalidCrossings) {
        removeInvalidCrossings->setCheck(false);
        saveInvalidCrossings->setCheck(true);
        selectInvalidCrossingsAndCancel->setCheck(false);
    } else if (option == selectInvalidCrossingsAndCancel) {
        removeInvalidCrossings->setCheck(false);
        saveInvalidCrossings->setCheck(false);
        selectInvalidCrossingsAndCancel->setCheck(true);
    }
}


void
GNEFixNetworkElements::FixCrossingOptions::fixElements(bool& abortSaving) {
    if (myInvalidElements.size() > 0) {
        if (removeInvalidCrossings->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "delete invalid crossings");
            // iterate over invalid crossings to delete it
            for (const auto& invalidCrossing : myInvalidElements) {
                myViewNet->getNet()->deleteCrossing(myViewNet->getNet()->getAttributeCarriers()->retrieveCrossing(invalidCrossing), myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
        } else if (selectInvalidCrossingsAndCancel->getCheck() == TRUE) {
            // begin undo list
            myViewNet->getUndoList()->begin(GUIIcon::ROUTE, "select invalid edges");
            // iterate over invalid single lane elements to select all elements
            for (const auto& invalidCrossing : myInvalidElements) {
                invalidCrossing->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
            // end undo list
            myViewNet->getUndoList()->end();
            // abort saving
            abortSaving = true;
        }
    }
}


void
GNEFixNetworkElements::FixCrossingOptions::enableOptions() {
    removeInvalidCrossings->enable();
    saveInvalidCrossings->enable();
    selectInvalidCrossingsAndCancel->enable();
}


void
GNEFixNetworkElements::FixCrossingOptions::disableOptions() {
    removeInvalidCrossings->disable();
    saveInvalidCrossings->disable();
    selectInvalidCrossingsAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::Buttons - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::Buttons::Buttons(GNEFixNetworkElements* fixNetworkElementsParent) :
    FXHorizontalFrame(fixNetworkElementsParent->myMainFrame, GUIDesignHorizontalFrame) {
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(this, TL("&Accept"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), fixNetworkElementsParent, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(this, TL("&Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), fixNetworkElementsParent, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // set focus in accept button
    myAcceptButton->setFocus();
}

/****************************************************************************/
