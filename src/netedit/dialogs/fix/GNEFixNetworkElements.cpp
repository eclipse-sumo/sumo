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
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/basic/GNEErrorBasicDialog.h>
#include <netedit/dialogs/basic/GNEInformationBasicDialog.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEFixNetworkElements.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFixNetworkElements - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::GNEFixNetworkElements(GNEApplicationWindow *mainWindow) :
    GNEFixElementsDialog(mainWindow, TL("Fix network elements problems"), GUIIcon::SUPERMODENETWORK, 600, 620) {
    // create fix edge options
    myFixEdgeOptions = new FixEdgeOptions(this);
    // create fix crossing  options
    myFixCrossingOptions = new FixCrossingOptions(this);
}


GNEFixNetworkElements::~GNEFixNetworkElements() {}


void
GNEFixNetworkElements::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    // run internal test in all modules
    myFixEdgeOptions->runInternalTest(dialogArgument);
    myFixCrossingOptions->runInternalTest(dialogArgument);
    // accept changes
    onCmdAccept(nullptr, 0, nullptr);
}


GNEDialog::Result
GNEFixNetworkElements::openDialog(const std::vector<GNENetworkElement*>& element) {
    // split invalidNetworkElements in four groups
    std::vector<ConflictElement> invalidEdges, invalidCrossings;
    // fill groups
    for (const auto& invalidNetworkElement : element) {
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
    return openModal();
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixEdgeOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixEdgeOptions::FixEdgeOptions(GNEFixNetworkElements* fixNetworkElementsParent) :
    GNEFixElementsDialog<GNENetworkElement*>::FixOptions(fixNetworkElementsParent, fixNetworkElementsParent->myLeftFrame, "Edges") {
    // Remove invalid edges
    myRemoveInvalidEdges = new FXRadioButton(myLeftFrameOptions,
                                             TL("Remove invalid edges"),
                                             this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid edges
    mySaveInvalidEdges = new FXRadioButton(myLeftFrameOptions,
                                           TL("Save invalid edges"),
                                           this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid edges
    mySelectInvalidEdgesAndCancel = new FXRadioButton(myRightFrameOptions,
                                                      TL("Select conflicted edges"),
                                                      this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // set option "removeInvalidEdges" as default
    myRemoveInvalidEdges->setCheck(true);
}


void
GNEFixNetworkElements::FixEdgeOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
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
        } else if (mySelectInvalidEdgesAndCancel->getCheck() == TRUE) {
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
        mySelectInvalidEdgesAndCancel->setCheck(false);
    } else if (obj == mySaveInvalidEdges) {
        myRemoveInvalidEdges->setCheck(false);
        mySaveInvalidEdges->setCheck(true);
        mySelectInvalidEdgesAndCancel->setCheck(false);
    } else if (obj == mySelectInvalidEdgesAndCancel) {
        myRemoveInvalidEdges->setCheck(false);
        mySaveInvalidEdges->setCheck(false);
        mySelectInvalidEdgesAndCancel->setCheck(true);
    }
    return 1;
}


void
GNEFixNetworkElements::FixEdgeOptions::enableOptions() {
    myRemoveInvalidEdges->enable();
    mySaveInvalidEdges->enable();
    mySelectInvalidEdgesAndCancel->enable();
}


void
GNEFixNetworkElements::FixEdgeOptions::disableOptions() {
    myRemoveInvalidEdges->disable();
    mySaveInvalidEdges->disable();
    mySelectInvalidEdgesAndCancel->disable();
}

// ---------------------------------------------------------------------------
// GNEFixNetworkElements::FixCrossingOptions - methods
// ---------------------------------------------------------------------------

GNEFixNetworkElements::FixCrossingOptions::FixCrossingOptions(GNEFixNetworkElements* fixNetworkElementsParent) :
    GNEFixElementsDialog<GNENetworkElement*>::FixOptions(fixNetworkElementsParent, fixNetworkElementsParent->myLeftFrame, "Crossings") {
    // Remove invalid crossings
    myRemoveInvalidCrossings = new FXRadioButton(myLeftFrameOptions,
                                                 TL("Remove invalid crossings"),
                                                 this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Save invalid crossings
    mySaveInvalidCrossings = new FXRadioButton(myLeftFrameOptions,
                                               TL("Save invalid crossings"),
                                               this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // Select invalid crossing
    mySelectInvalidCrossings = new FXRadioButton(myRightFrameOptions,
                                                 TL("Select conflicted crossing"),
                                                 this, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonFix);
    // by default remove invalid crossings
    myRemoveInvalidCrossings->setCheck(TRUE);
}


void
GNEFixNetworkElements::FixCrossingOptions::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    // chooose solution
    if (dialogArgument->fixSolution == "removeInvalidCrossings") {
        myRemoveInvalidCrossings->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "saveInvalidCrossings") {
        mySaveInvalidCrossings->setCheck(TRUE, TRUE);
    } else if (dialogArgument->fixSolution == "selectInvalidCrossings") {
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


void
GNEFixNetworkElements::FixCrossingOptions::enableOptions() {
    myRemoveInvalidCrossings->enable();
    mySaveInvalidCrossings->enable();
    mySelectInvalidCrossings->enable();
}


void
GNEFixNetworkElements::FixCrossingOptions::disableOptions() {
    myRemoveInvalidCrossings->disable();
    mySaveInvalidCrossings->disable();
    mySelectInvalidCrossings->disable();
}

/****************************************************************************/
