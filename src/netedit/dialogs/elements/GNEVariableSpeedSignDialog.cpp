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
/// @file    GNEVariableSpeedSignDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// Dialog for edit variableSpeedSigns
/****************************************************************************/

#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/elements/additional/GNEVariableSpeedSignStep.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/options/OptionsCont.h>

#include "GNEVariableSpeedSignDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignDialog::GNEVariableSpeedSignDialog(GNEAdditional* variableSpeedSign) :
    GNETemplateElementDialog<GNEAdditional>(variableSpeedSign, DialogType::VSS) {
    // create variableSpeedSign steps element list
    myVariableSpeedSignSteps = new VariableSpeedSignStepsList(this);
    // open dialog
    openDialog();
}


GNEVariableSpeedSignDialog::~GNEVariableSpeedSignDialog() {}


void
GNEVariableSpeedSignDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEVariableSpeedSignDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Check if there is overlapping between Steps
    if (!myVariableSpeedSignSteps->isSorted()) {
        // open warning Box
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                              TLF("VariableSpeedSign steps of % '%' cannot be saved", toString(SUMO_TAG_VSS), myElement->getID()),
                              TL("Steps has to be sorted."));
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNEVariableSpeedSignDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myVariableSpeedSignSteps->updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVariableSpeedSignDialog::VariableSpeedSignStepsList - methods
// ---------------------------------------------------------------------------

GNEVariableSpeedSignDialog::VariableSpeedSignStepsList::VariableSpeedSignStepsList(GNEVariableSpeedSignDialog* variableSpeedSignDialog) :
    GNETemplateElementList(variableSpeedSignDialog, variableSpeedSignDialog->getContentFrame(), SUMO_TAG_STEP,
                           GNEElementList::Options::SORTELEMENTS | GNEElementList::Options::FIXED_HEIGHT) {
}


long
GNEVariableSpeedSignDialog::VariableSpeedSignStepsList::addNewElement() {
    // create step depending of number of steps
    if (getEditedElements().empty()) {
        return insertElement(new GNEVariableSpeedSignStep(myElementDialogParent->getElement(), 0,
                             OptionsCont::getOptions().getFloat("default.speed")));
    } else {
        SUMOTime biggestTime = 0;
        // get end with biggest end
        for (const auto& step : getEditedElements()) {
            const auto time = string2time(step->getAttribute(SUMO_ATTR_TIME));
            if (biggestTime < time) {
                biggestTime = time;
            }
        }
        return insertElement(new GNEVariableSpeedSignStep(myElementDialogParent->getElement(), biggestTime + string2time("10"),
                             OptionsCont::getOptions().getFloat("default.speed")));
    }
}


long
GNEVariableSpeedSignDialog::VariableSpeedSignStepsList::openElementDialog(const size_t /*rowIndex*/) {
    // nothing to edit in steps
    return 1;
}


bool
GNEVariableSpeedSignDialog::VariableSpeedSignStepsList::isSorted() const {
    // declare a vector to store steps
    std::vector<double> sortedSteps;
    // save time steps
    for (const auto& step : getEditedElements()) {
        sortedSteps.push_back(step->getAttributeDouble(SUMO_ATTR_TIME));
    }
    // check if all are sorted
    if (sortedSteps.size() > 1) {
        // check if the next step is bigger than the current step
        for (int i = 0; i < (int)sortedSteps.size() - 1; i++) {
            if (sortedSteps.at(i) > sortedSteps.at(i + 1)) {
                return false;
            }
        }
    }
    return true;
}

/****************************************************************************/
