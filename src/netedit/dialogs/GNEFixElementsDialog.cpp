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
/// @file    GNEFixElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2023
///
// Dialog used to fix elements during saving
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEInternalTest.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEFixElementsDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixElementsDialog) GNEFixElementsDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixElementsDialog::onCmdSelectOption),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEFixElementsDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEFixElementsDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEFixElementsDialog, MFXDialogBox, GNEFixElementsDialogMap, ARRAYNUMBER(GNEFixElementsDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFixElementsDialog::GNEFixElementsDialog(GNEViewNet* viewNet, const std::string title, GUIIcon icon, const int sizeX, const int sizeY) :
    MFXDialogBox(viewNet->getViewParent()->getGNEAppWindows(), title.c_str(), GUIDesignDialogBoxExplicitStretchable(sizeX, sizeY)),
    myViewNet(viewNet) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(icon));
    // create main frame
    myMainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
}


GNEFixElementsDialog::~GNEFixElementsDialog() {}


long
GNEFixElementsDialog::closeFixDialog(const bool success) {
    if (success) {
        // stop modal with TRUE (continue saving)
        getApp()->stopModal(this, TRUE);
    } else {
        // stop modal with FALSE (abort saving)
        getApp()->stopModal(this, FALSE);
    }
    hide();
    return 1;
}


// ---------------------------------------------------------------------------
// GNEFixElementsDialog::Buttons - methods
// ---------------------------------------------------------------------------

GNEFixElementsDialog::Buttons::Buttons(GNEFixElementsDialog* fixElementsDialog) :
    FXHorizontalFrame(fixElementsDialog->myMainFrame, GUIDesignHorizontalFrame) {
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = GUIDesigns::buildFXButton(this, TL("&Accept"), "", "", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), fixElementsDialog, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = GUIDesigns::buildFXButton(this, TL("&Cancel"), "", "", GUIIconSubSys::getIcon(GUIIcon::CANCEL), fixElementsDialog, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
}

/****************************************************************************/
