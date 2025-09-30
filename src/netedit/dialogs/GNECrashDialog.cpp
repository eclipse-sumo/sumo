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
/// @file    GNECrashDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Dialog used for handling crashes produced in Netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNECrashDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNECrashDialog::GNECrashDialog(GNEApplicationWindow* applicationWindow, const ProcessError& processError) :
    GNEDialog(applicationWindow, TL("Critical error"), GUIIcon::ERROR_SMALL,
              DialogType::ABOUT, GNEDialog::Buttons::OK, OpenType::MODAL, ResizeMode::RESIZABLE) {
    // create dialog layout (obtained from FXMessageBox)
    auto contents = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add information label
    new FXLabel(contents, TL("Netedit found an internal critical error and will be closed:"), NULL, GUIDesignLabel(JUSTIFY_NORMAL));
    // create text field for exception
    myExceptionTextField = new MFXTextFieldIcon(contents, applicationWindow->getStaticTooltipMenu(), GUIIcon::EMPTY, nullptr, 0, GUIDesignTextField);
    myExceptionTextField->setEditable(FALSE);
    myExceptionTextField->setText(processError.what());
    // add information label
    new FXLabel(contents, TL("ErrorTrace:"), NULL, GUIDesignLabel(JUSTIFY_NORMAL));
    // Área de texto multilínea con scroll
    FXText* text = new FXText(contents, nullptr, 0, TEXT_WORDWRAP | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    text->setEditable(FALSE);
    text->setText(processError.getTrace().c_str());
    // open modal dialog
    openDialog();
}


GNECrashDialog::~GNECrashDialog() {
}


void
GNECrashDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}

/****************************************************************************/
