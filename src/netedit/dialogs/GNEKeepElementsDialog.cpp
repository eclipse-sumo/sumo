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
#include <utils/gui/div/GUIDesigns.h>

#include "GNEKeepElementsDialog.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEKeepElementsDialog::GNEKeepElementsDialog(GNEApplicationWindow* applicationWindow, const std::string elementType) :
    GNEDialog(applicationWindow, TLF("Keep % elements", elementType).c_str(), GUIIcon::QUESTION_SMALL,
              GNEDialog::Buttons::KEEPNEW_KEEPOLD_CANCEL, GNEDialog::OpenType::MODAL) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create label
    new FXLabel(mainFrame, (TLF("Selected % file was already loaded.", elementType) + "\n" +
                TL("Keep new or old elements?")).c_str(), nullptr, GUIDesignLabelKeepElements);
    // open modal dialog
    openModalDialog();
}


GNEKeepElementsDialog::~GNEKeepElementsDialog() {
}


void
GNEKeepElementsDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // finish
}

/****************************************************************************/
