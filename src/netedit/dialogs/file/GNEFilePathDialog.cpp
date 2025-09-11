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
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEFilePathDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEFilePathDialog::GNEFilePathDialog(GNEApplicationWindow* applicationWindow, const std::string& title,
                                     const std::string& info, const std::string& originalFilePath) :
    GNEDialog(applicationWindow, title.c_str(), GUIIcon::OPEN, DialogType::FILEPATH,
              GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
    myOriginalFilePath(originalFilePath) {
    // create dialog layout (obtained from FXMessageBox)
    //auto infoFrame = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add information label
    new FXLabel(myContentFrame, info.c_str(), nullptr, GUIDesignLabel(JUSTIFY_NORMAL));
    // create text field to enter the path
    myPathTextField = new MFXTextFieldIcon(myContentFrame, applicationWindow->getStaticTooltipMenu(), GUIIcon::EMPTY,
                                           nullptr, 0, GUIDesignTextField);
    // set original file path
    myPathTextField->setText(originalFilePath.c_str());
    // open modal dialog
    openDialog();
}


GNEFilePathDialog::~GNEFilePathDialog() {
}


void
GNEFilePathDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do (yet)
}


std::string
GNEFilePathDialog::getFilePath() const {
    return myPathTextField->getText().text();
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
