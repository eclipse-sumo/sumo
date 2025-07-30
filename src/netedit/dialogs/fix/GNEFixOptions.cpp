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
/// @file    GNEFixOptions.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Abstract groupbox module used in GNEFixElementsDialog to fix elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEFixElementsDialog.h"
#include "GNEFixOptions.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFixOptions) GNEFixOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEFixOptions::onCmdSelectOption),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEFixOptions, MFXGroupBoxModule, GNEFixOptionsMap, ARRAYNUMBER(GNEFixOptionsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFixOptions::GNEFixOptions(GNEFixElementsDialog *fixElementDialog, FXVerticalFrame* frameParent, const std::string& title, GUIIcon icon) :
    MFXGroupBoxModule(frameParent, title, MFXGroupBoxModule::Options::NOTHING),
    myFixElementDialogParent(fixElementDialog) {
    // add this fix option to list of fix options
    fixElementDialog->addFixOptions(this);
    // Create table
    myTable = new FXTable(frameParent, this, MID_TABLE, GUIDesignTableFixElements);
    // create frames for options
    FXHorizontalFrame* optionsFrame = new FXHorizontalFrame(frameParent, GUIDesignAuxiliarFrame);
    myLeftFrameOptions = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
    myRightFrameOptions = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrame);
}


GNEFixElementsDialog::~GNEFixElementsDialog() {}

/****************************************************************************/
