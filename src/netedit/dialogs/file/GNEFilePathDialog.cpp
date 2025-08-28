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
/// @file    GNEFilePathDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// A basic dialog for selecting a file path (used in GNEFileSelector)
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEFilePathDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEFilePathDialog::GNEFilePathDialog(GNEApplicationWindow* applicationWindow, const std::string& title,
                                     const std::string& info, const std::string& originalFilePath) :
    GNEDialog(applicationWindow, title.c_str(), GUIIcon::OPEN, GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
    myOriginalFilePath(originalFilePath) {
    // add information label
    new FXLabel(getContentFrame(), info.c_str(), NULL, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // create text field to enter the path
    myPathTextField = new MFXTextFieldTooltip(getContentFrame(), applicationWindow->getStaticTooltipMenu(), GUIDesignTextFieldNCol,
            this, 0, GUIDesignTextFieldFixed(300));
    // open modal dialog
    openDialog();
}


GNEFilePathDialog::~GNEFilePathDialog() {
}


void
GNEFilePathDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEFilePathDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // set an empty test
    myPathTextField->setText("", FALSE);
    return closeDialogCanceling();
}


long
GNEFilePathDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // restore original file path
    myPathTextField->setText(myOriginalFilePath.c_str(), TRUE);
    return 1;
}

/****************************************************************************/
