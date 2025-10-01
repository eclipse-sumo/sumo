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
/// @file    GNESaveDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Dialog used for saving elements
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNESaveDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNESaveDialog::GNESaveDialog(GNEApplicationWindow* applicationWindow, const std::string& elementTypes) :
    GNEDialog(applicationWindow, TLF("Save %", elementTypes), GUIIcon::SAVE, DialogType::SAVE,
              GNEDialog::Buttons::SAVE_DONTSAVE_CANCEL, OpenType::MODAL, ResizeMode::STATIC) {
    // create dialog layout (obtained from FXMessageBox)
    auto infoFrame = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add information label
    const std::string info = TLF("You have unsaved %.", elementTypes) + std::string("\n") + 
                             TL("Do you wish to close and discard all changes?");
    new FXLabel(infoFrame, info.c_str(), NULL, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // create applyToAll button
    myApplyToAllButton = new FXCheckButton(infoFrame, TL("Apply to all unsaved elements"), nullptr, 0, GUIDesignCheckButton);
    // open modal dialog
    openDialog();
}


GNESaveDialog::~GNESaveDialog() {
}


void
GNESaveDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNESaveDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // close dialog accepting
    closeDialogAccepting();
    // check if return apply to all
    if (myApplyToAllButton && (myApplyToAllButton->getCheck() == TRUE)) {
        myResult = Result::ACCEPT_ALL;
    }
    return 1;
}


long
GNESaveDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // close dialog accepting
    closeDialogCanceling();
    // check if return apply to all
    if (myApplyToAllButton && (myApplyToAllButton->getCheck() == TRUE)) {
        myResult = Result::CANCEL_ALL;
    }
    return 0;
}

/****************************************************************************/
