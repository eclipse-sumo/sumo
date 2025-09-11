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
/// @file    GNEBasicDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// A basic dialog with simple buttons (ok, accept, cancel, etc.)
/****************************************************************************/
#include <config.h>

#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEBasicDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEBasicDialog::GNEBasicDialog(GNEApplicationWindow* applicationWindow, const std::string& title,
                               const std::string& info, GUIIcon titleIcon, DialogType type,
                               GNEDialog::Buttons buttons, GUIIcon largeIcon) :
    GNEDialog(applicationWindow, title.c_str(), titleIcon, type, buttons, OpenType::MODAL, ResizeMode::STATIC) {
    // create dialog layout (obtained from FXMessageBox)
    auto infoFrame = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add icon label (only if large icon is defined)
    if (largeIcon != GUIIcon::EMPTY) {
        new FXLabel(infoFrame, FXString::null, GUIIconSubSys::getIcon(largeIcon), ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    }
    // add information label
    new FXLabel(infoFrame, info.c_str(), NULL, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // open modal dialog
    openDialog();
}


GNEBasicDialog::~GNEBasicDialog() {
}


void
GNEBasicDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}

/****************************************************************************/
