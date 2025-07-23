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
/// @file    GNEAboutDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Help dialog used in netedit
/****************************************************************************/

#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEHelpDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEHelpDialog::GNEHelpDialog(GNEApplicationWindow* applicationWindow, const std::string &header, const std::string &text) :
    GNEDialog(applicationWindow, header.c_str(), GUIDesignDialogBox) {
    // set dialog icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::HELP));
    // Create label with the help text
    new FXLabel(this, text.c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create Button OK (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    GUIDesigns::buildFXButton(myHorizontalFrameOKButton, TL("OK"), "", TL("Close"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
}


GNEHelpDialog::~GNEHelpDialog() {
}


long
GNEHelpDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    return closeDialogAccepting();
}


long
GNEHelpDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    return closeDialogCanceling();
}

/****************************************************************************/
