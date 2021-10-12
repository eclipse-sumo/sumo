/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEUndoListDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// Dialog for show undo-list
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEUndoListDialog.h"
#include "GNERerouterIntervalDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEUndoListDialog) GNEUndoListDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,        0,                                      GNEUndoListDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_ACCEPT,                  GNEUndoListDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_CANCEL,                  GNEUndoListDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_REROUTEDIALOG_ADD_INTERVAL,     GNEUndoListDialog::onCmdSelectElement),
};

// Object implementation
FXIMPLEMENT(GNEUndoListDialog, FXTopWindow, GNEUndoListDialogMap, ARRAYNUMBER(GNEUndoListDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEUndoListDialog::GNEUndoListDialog(GNEViewNet* viewNet) :
    FXTopWindow(viewNet, "Undo-Redo list", GUIIconSubSys::getIcon(GUIIcon::UNDO), GUIIconSubSys::getIcon(GUIIcon::UNDO), GUIDesignDialogBoxExplicit(320, 240)),
    myViewNet(viewNet) {
/*
    // create Horizontal frame for row elements
    FXHorizontalFrame* myAddIntervalFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    // create Button and Label for adding new Wors
    myAddInterval = new FXButton(myAddIntervalFrame, "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_REROUTEDIALOG_ADD_INTERVAL, GUIDesignButtonIcon);
    new FXLabel(myAddIntervalFrame, ("Add new " + toString(SUMO_TAG_INTERVAL)).c_str(), nullptr, GUIDesignLabelThick);
    // create Button and Label for sort intervals
    mySortIntervals = new FXButton(myAddIntervalFrame, "", GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_GNE_REROUTEDIALOG_SORT_INTERVAL, GUIDesignButtonIcon);
    new FXLabel(myAddIntervalFrame, ("Sort " + toString(SUMO_TAG_INTERVAL) + "s").c_str(), nullptr, GUIDesignLabelThick);

    // Create table
    myIntervalTable = new FXTable(myContentFrame, this, MID_GNE_REROUTEDIALOG_TABLE_INTERVAL, GUIDesignTableAdditionals);
    myIntervalTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
    myIntervalTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
    myIntervalTable->setEditable(false);

    // update intervals
    updateIntervalTable();

    // start a undo list for editing local to this additional
    initChanges();

    // Open dialog as modal
    openAsModalDialog();
*/
}


GNEUndoListDialog::~GNEUndoListDialog() {}


long
GNEUndoListDialog::onCmdAccept(FXObject*, FXSelector, void*) {
/*
    // Check if there is overlapping between Intervals
    if (!myEditedAdditional->checkChildAdditionalsOverlapping()) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
        // open warning Box
        FXMessageBox::warning(getApp(), MBOX_OK, "Overlapping detected", "%s", ("Values of '" + myEditedAdditional->getID() + "' cannot be saved. There are intervals overlapped.").c_str());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
        return 0;
    } else {
        // accept changes before closing dialog
        acceptChanges();
        // Stop Modal
        getApp()->stopModal(this, TRUE);
*/
        return 1;
}


long
GNEUndoListDialog::onCmdCancel(FXObject*, FXSelector, void*) {
/*
    // cancel changes
    cancelChanges();
    // Stop Modal
    getApp()->stopModal(this, FALSE);
*/
    return 1;
}


long
GNEUndoListDialog::onCmdSelectElement(FXObject*, FXSelector, void*) {
    return 1;
}


void
GNEUndoListDialog::updateTable() {

}

/****************************************************************************/
