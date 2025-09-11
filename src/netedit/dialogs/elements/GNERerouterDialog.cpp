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
/// @file    GNERerouterDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// Dialog for edit rerouters
/****************************************************************************/

#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/elements/additional/GNERerouterInterval.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNERerouterDialog.h"
#include "GNERerouterIntervalDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterDialog::GNERerouterDialog(GNEAdditional* rerouter) :
    GNETemplateElementDialog<GNEAdditional>(rerouter, DialogType::REROUTER) {
    // create rerouter intervals element list
    myRerouterIntervals = new RerouterIntervalsList(this);
    // open dialog
    openDialog();
}


GNERerouterDialog::~GNERerouterDialog() {}


void
GNERerouterDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNERerouterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Check if there is overlapping between Intervals
    if (!myRerouterIntervals->isOverlapping()) {
        // open warning Box
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                              TLF("Rerouter intervals of % '%' cannot be saved", toString(SUMO_TAG_REROUTER), myElement->getID()),
                              TL(". There are intervals overlapped."));
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNERerouterDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myRerouterIntervals->updateList();
    return 1;
}

// ---------------------------------------------------------------------------
// GNERerouterDialog::RerouterIntervalsList - methods
// ---------------------------------------------------------------------------

GNERerouterDialog::RerouterIntervalsList::RerouterIntervalsList(GNERerouterDialog* rerouterDialog) :
    GNETemplateElementList(rerouterDialog, rerouterDialog->getContentFrame(), SUMO_TAG_INTERVAL,
                           GNEElementList::Options::SORTELEMENTS | GNEElementList::Options::DIALOG_ELEMENT | GNEElementList::Options::FIXED_HEIGHT) {
}


long
GNERerouterDialog::RerouterIntervalsList::addNewElement() {
    SUMOTime end = 0;
    // get end with biggest end
    for (const auto& interval : getEditedElements()) {
        const auto intervalEnd = string2time(interval->getAttribute(SUMO_ATTR_END));
        if (end < intervalEnd) {
            end = intervalEnd;
        }
    }
    // create interval
    return insertElement(new GNERerouterInterval(myElementDialogParent->getElement(), end, end + string2time("3600")));
}


long
GNERerouterDialog::RerouterIntervalsList::openElementDialog(const size_t rowIndex) {
    // simply open dialog for the edited additional element
    GNERerouterIntervalDialog(getEditedElements().at(rowIndex));
    return 1;
}


bool
GNERerouterDialog::RerouterIntervalsList::isOverlapping() const {
    // declare a vector to keep sorted children
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedIntervals;
    // iterate over child interval
    for (const auto& interval : getEditedElements()) {
        // add interval to sorted intervals
        sortedIntervals.push_back(std::make_pair(std::make_pair(0., 0.), interval));
        // set begin and end
        sortedIntervals.back().first.first = interval->getAttributeDouble(SUMO_ATTR_BEGIN);
        sortedIntervals.back().first.second = interval->getAttributeDouble(SUMO_ATTR_END);
    }
    // sort intervals by begin and end
    std::sort(sortedIntervals.begin(), sortedIntervals.end());
    // if we have only one interval or less, there is no overlapping
    if (sortedIntervals.size() <= 1) {
        return true;
    } else {
        // check if the next end is bigger than the current begin
        for (int i = 0; i < (int)sortedIntervals.size() - 1; i++) {
            if (sortedIntervals.at(i).first.second > sortedIntervals.at(i + 1).first.first) {
                return false;
            }
        }
    }
    return true;
}

/****************************************************************************/
