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
/// @file    GNEOverwritteElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewParent.h>

#include "GNEOverwritteElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEOverwritteElement::GNEOverwritteElement(const GNEAttributeCarrier* AC) :
    GNEDialog(AC->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
              TLF("Overwrite % '%'", AC->getTagProperty()->getTagStr(), AC->getID()), GUIIcon::QUESTION_SMALL,
              GNEDialog::Buttons::ACCEPT_CANCEL, GNEDialog::OpenType::MODAL, ResizeMode::STATIC) {
    // create dialog layout (obtained from FXMessageBox)
    auto infoFrame = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add information label
    new FXLabel(infoFrame, TLF("There is already a % '%'. Overwrite?", AC->getTagProperty()->getTagStr(), AC->getID()).c_str(),
                nullptr, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // open modal dialog
    openDialog();
}


GNEOverwritteElement::~GNEOverwritteElement() {
}


void
GNEOverwritteElement::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}

/****************************************************************************/
