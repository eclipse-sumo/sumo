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

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/elements/additional/GNERerouterInterval.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNERerouterDialog.h"
#include "GNERerouterIntervalDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERerouterDialog) GNERerouterDialogMap[] = {
    // called when user click over buttons
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_ADD,    GNERerouterDialog::onCmdElementListAdd),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_SORT,   GNERerouterDialog::onCmdElementListSort),
};

// Object implementation
FXIMPLEMENT(GNERerouterDialog, GNEElementDialog<GNEAdditional>, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterDialog::GNERerouterDialog(GNEAdditional* rerouter) :
    GNEElementDialog<GNEAdditional>(rerouter, false) {
    // create rerouter intervals element list
    myRerouterIntervals = new ElementList<GNEAdditional, GNEChange_Additional>(this, getContentFrame(), SUMO_TAG_INTERVAL, myElement->getChildAdditionals(), true);
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
    if (!myElement->checkChildAdditionalsOverlapping()) {
        // open warning Box
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                              TLF("Rerouter intervals of % '%' cannot be saved", toString(SUMO_TAG_REROUTER), myElement->getID()),
                              TL(". There are intervals overlapped."));
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // Stop Modal
        closeDialogAccepting();
    }
    return 1;
}


long
GNERerouterDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // cancel changes
    cancelChanges();
    // Stop Modal
    return closeDialogCanceling();
}


long
GNERerouterDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    // update tables
    myRerouterIntervals->updateTable();
    return 1;
}


long
GNERerouterDialog::onCmdElementListAdd(FXObject*, FXSelector, void*) {
    // create closing reroute
    return myRerouterIntervals->addElement(new GNERerouterInterval(this));
}


long
GNERerouterDialog::onCmdElementListSort(FXObject*, FXSelector, void*) {
    // sort rerouter intervals
    myRerouterIntervals->sortElements();
    return 1;
}

/****************************************************************************/
