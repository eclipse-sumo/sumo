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
/// @file    GNEFixNetworkElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog used to fix network elements during saving
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>

#include "GNEFixNetworkElements.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixNetworkElements::FixEdgeOptions) FixEdgeOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixNetworkElements::FixEdgeOptions::onCmdSelectOption)
};

FXDEFMAP(GNEFixNetworkElements::FixCrossingOptions) FixCrossingOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixNetworkElements::FixCrossingOptions::onCmdSelectOption)
};

// Object abstract implementation
FXIMPLEMENT(GNEFixNetworkElements::FixEdgeOptions,     MFXGroupBoxModule, FixEdgeOptionsMap,        ARRAYNUMBER(FixEdgeOptionsMap))
FXIMPLEMENT(GNEFixNetworkElements::FixCrossingOptions, MFXGroupBoxModule, FixCrossingOptionsMap,    ARRAYNUMBER(FixCrossingOptionsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixEdgeOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixEdgeOptions::FixEdgeOptions(GNEFixNetworkElements* fixNetworkElementsParent) :
    GNEFixElementsDialog<GNENetworkElement*>::FixOptions(fixNetworkElementsParent, fixNetworkElementsParent->myLeftFrame, "Edges") {
    // Remove invalid edges
    myRemoveInvalidEdges = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                           TL("Remove invalid edges"), "",
                           TL("Remove invalid edges"),
                           this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid edges
    mySaveInvalidEdges = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                         TL("Save invalid edges"), "",
                         TL("Save invalid edges"),
                         this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid edges
    mySelectInvalidEdges = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                           TL("Select invalid edges"), "",
                           TL("Select invalid edges and abort saving"),
                           this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // register options
    registerOption(myRemoveInvalidEdges);
    registerOption(mySaveInvalidEdges);
    registerOption(mySelectInvalidEdges);
    // set option "removeInvalidEdges" as default
    myRemoveInvalidEdges->setCheck(true);
}


void
GNEFixNetworkElements::FixEdgeOptions::selectInternalTestSolution(const std::string& /*solution*/) {
    // finish
}


bool
GNEFixNetworkElements::FixEdgeOptions::applyFixOption() {
    if (myConflictedElements.size() > 0) {
        auto net = myFixElementDialogParent->getApplicationWindow()->getViewNet()->getNet();
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myRemoveInvalidEdges->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::EDGE, TL("delete invalid edges"));
            // iterate over invalid edges to delete it
            for (const auto& conflictedElement : myConflictedElements) {
                net->deleteEdge(net->getAttributeCarriers()->retrieveEdge(conflictedElement.getID()), undoList, false);
            }
            // end undo list
            undoList->end();
        } else if (mySelectInvalidEdges->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::EDGE, TL("select invalid edges"));
            // iterate over invalid single lane elements to select all elements
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
GNEFixNetworkElements::FixEdgeOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myRemoveInvalidEdges) {
        myRemoveInvalidEdges->setCheck(true);
        mySaveInvalidEdges->setCheck(false);
        mySelectInvalidEdges->setCheck(false);
    } else if (obj == mySaveInvalidEdges) {
        myRemoveInvalidEdges->setCheck(false);
        mySaveInvalidEdges->setCheck(true);
        mySelectInvalidEdges->setCheck(false);
    } else if (obj == mySelectInvalidEdges) {
        myRemoveInvalidEdges->setCheck(false);
        mySaveInvalidEdges->setCheck(false);
        mySelectInvalidEdges->setCheck(true);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixCrossingOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixCrossingOptions::FixCrossingOptions(GNEFixNetworkElements* fixNetworkElementsParent) :
    GNEFixElementsDialog<GNENetworkElement*>::FixOptions(fixNetworkElementsParent, fixNetworkElementsParent->myLeftFrame, "Crossings") {
    // Remove invalid crossings
    myRemoveInvalidCrossings = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                               TL("Remove invalid crossings"), "",
                               TL("Remove invalid crossings"),
                               this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid crossings
    mySaveInvalidCrossings = GUIDesigns::buildFXRadioButton(myLeftFrameOptions,
                             TL("Save invalid crossings"), "",
                             TL("Save invalid crossings"),
                             this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid crossing
    mySelectInvalidCrossings = GUIDesigns::buildFXRadioButton(myRightFrameOptions,
                               TL("Select invalid crossing"), "",
                               TL("Select invalid crossing and abort saving"),
                               this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // register options
    registerOption(myRemoveInvalidCrossings);
    registerOption(mySaveInvalidCrossings);
    registerOption(mySelectInvalidCrossings);
    // by default remove invalid crossings
    myRemoveInvalidCrossings->setCheck(TRUE);
}


void
GNEFixNetworkElements::FixCrossingOptions::selectInternalTestSolution(const std::string& solution) {
    // choose solution
    if (solution == "removeInvalidCrossings") {
        myRemoveInvalidCrossings->setCheck(TRUE, TRUE);
    } else if (solution == "saveInvalidCrossings") {
        mySaveInvalidCrossings->setCheck(TRUE, TRUE);
    } else if (solution == "selectInvalidCrossings") {
        mySelectInvalidCrossings->setCheck(TRUE, TRUE);
    }
}


bool
GNEFixNetworkElements::FixCrossingOptions::applyFixOption() {
    if (myConflictedElements.size() > 0) {
        auto net = myFixElementDialogParent->getApplicationWindow()->getViewNet()->getNet();
        auto undoList = myFixElementDialogParent->getApplicationWindow()->getUndoList();
        // continue depending of solution
        if (myRemoveInvalidCrossings->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::CROSSING, TL("delete invalid crossings"));
            // iterate over invalid crossings to delete it
            for (const auto& conflictedElement : myConflictedElements) {
                net->deleteCrossing(net->getAttributeCarriers()->retrieveCrossing(conflictedElement.getElement()), undoList);
            }
            // end undo list
            undoList->end();
        } else if (mySelectInvalidCrossings->getCheck() == TRUE) {
            // begin undo list
            undoList->begin(GUIIcon::CROSSING, TL("select invalid crossings"));
            // iterate over invalid single lane elements to select all elements
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
GNEFixNetworkElements::FixCrossingOptions::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myRemoveInvalidCrossings) {
        myRemoveInvalidCrossings->setCheck(true);
        mySaveInvalidCrossings->setCheck(false);
        mySelectInvalidCrossings->setCheck(false);
    } else if (obj == mySaveInvalidCrossings) {
        myRemoveInvalidCrossings->setCheck(false);
        mySaveInvalidCrossings->setCheck(true);
        mySelectInvalidCrossings->setCheck(false);
    } else if (obj == mySelectInvalidCrossings) {
        myRemoveInvalidCrossings->setCheck(false);
        mySaveInvalidCrossings->setCheck(false);
        mySelectInvalidCrossings->setCheck(true);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::GNEFixNetworkElements(GNEApplicationWindow* mainWindow,
        const std::vector<GNENetworkElement*>& elements) :
    GNEFixElementsDialog(mainWindow, TL("Fix network elements problems"), GUIIcon::SUPERMODENETWORK,
                         DialogType::FIX_NETWORKELEMENTS) {
    // create fix edge options
    myFixEdgeOptions = new FixEdgeOptions(this);
    // create fix crossing  options
    myFixCrossingOptions = new FixCrossingOptions(this);
    // split invalidNetworkElements in four groups
    std::vector<ConflictElement> invalidEdges, invalidCrossings;
    // fill groups
    for (const auto& invalidNetworkElement : elements) {
        // create conflict element
        auto fixElement = ConflictElement(invalidNetworkElement,
                                          invalidNetworkElement->getID(),
                                          invalidNetworkElement->getACIcon(),
                                          invalidNetworkElement->getNetworkElementProblem());
        // add depending of element type
        if (invalidNetworkElement->getTagProperty()->getTag() == SUMO_TAG_EDGE) {
            invalidEdges.push_back(fixElement);
        } else if (invalidNetworkElement->getTagProperty()->getTag() == SUMO_TAG_CROSSING) {
            invalidCrossings.push_back(fixElement);
        }
    }
    // fill options
    myFixEdgeOptions->setInvalidElements(invalidEdges);
    myFixCrossingOptions->setInvalidElements(invalidCrossings);
    // open modal dialog
    openDialog();
}


GNEFixNetworkElements::~GNEFixNetworkElements() {}

/****************************************************************************/
