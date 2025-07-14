#include "GNEKeepElementsDialog.h"
#include "GNEKeepElementsDialog.h"
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
/// @file    GNEKeepElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEKeepElementsDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEKeepElementsDialog) GNEKeepElementsDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,  GNEKeepElementsDialog::onCmdSelectOption),
    FXMAPFUNC(SEL_CLOSE,    0,               GNEKeepElementsDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEKeepElementsDialog, MFXDialogBox, GNEKeepElementsDialogMap, ARRAYNUMBER(GNEKeepElementsDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEKeepElementsDialog - methods
// ---------------------------------------------------------------------------

GNEKeepElementsDialog::GNEKeepElementsDialog(GNEApplicationWindow* applicationWindow, const std::string elementType) :
    MFXDialogBox(applicationWindow->getApp(), ("Keep " + elementType + " elements").c_str(), GUIDesignDialogBoxExplicit(400, 100)) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create label
    new FXLabel(mainFrame, ("Selected " + elementType + " file was already loaded.\n Keep new or old elements?").c_str(), nullptr, GUIDesignLabelKeepElements);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myKeepNewButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Keep new"), "", TL("Keep new elements"),  GUIIconSubSys::getIcon(GUIIcon::ACCEPT),  this, MID_GNE_SELECT, GUIDesignButtonKeepElements);
    myKeepOldButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Keep old"), "", TL("Keep old elements"),  GUIIconSubSys::getIcon(GUIIcon::BACK), this, MID_GNE_SELECT, GUIDesignButtonKeepElements);
    myCancelButton = GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel loading of elements"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_SELECT, GUIDesignButtonKeepElements);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create Dialog
    create();
    // set focus in keep news
    myKeepNewButton->setFocus();
    // set accept as
    // show in the given position
    show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    applicationWindow->getApp()->runModalFor(this);
}


GNEKeepElementsDialog::~GNEKeepElementsDialog() {
}


void
GNEKeepElementsDialog::runInternalTest(const InternalTestStep::FixDialogTest* /*dialogTest*/) {
    // finish
}


GNEKeepElementsDialog::Result
GNEKeepElementsDialog::getResult() const {
    return myResult;
}


long
GNEKeepElementsDialog::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    if (obj == myKeepOldButton) {
        myResult = Result::ACCEPT;
    } else if (obj == myKeepNewButton) {
        myResult = Result::OVERWRITE;
    }
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}

long
GNEKeepElementsDialog::onCmdClose(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}

/****************************************************************************/
