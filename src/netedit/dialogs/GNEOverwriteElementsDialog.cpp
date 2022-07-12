/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEOverwriteElementsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEOverwriteElementsDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOverwriteElementsDialog) GNEOverwriteElementsDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,  GNEOverwriteElementsDialog::onCmdSelectOption),
};

// Object implementation
FXIMPLEMENT(GNEOverwriteElementsDialog, FXDialogBox, GNEOverwriteElementsDialogMap, ARRAYNUMBER(GNEOverwriteElementsDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEOverwriteElementsDialog - methods
// ---------------------------------------------------------------------------

GNEOverwriteElementsDialog::GNEOverwriteElementsDialog(GNEApplicationWindow* applicationWindow) :
    FXDialogBox(applicationWindow->getApp(), "Fix demand elements problems", GUIDesignDialogBoxExplicit(300, 400)) {
    // set busStop icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
}


GNEOverwriteElementsDialog::~GNEOverwriteElementsDialog() {
}


long
GNEOverwriteElementsDialog::onCmdSelectOption(FXObject* obj, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
