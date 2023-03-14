/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNERunToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for running tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/FileHelpers.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>

#include "GNERunToolDialog.h"


#define MARGING 4

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERunToolDialog) GNERunToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNERunToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,     GNERunToolDialog::onCmdRun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNERunToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNERunToolDialog::onCmdReset)
};

// Object implementation
FXIMPLEMENT(GNERunToolDialog, FXDialogBox, GNERunToolDialogMap, ARRAYNUMBER(GNERunToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunToolDialog::GNERunToolDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Tool", GUIDesignDialogBoxExplicit(0, 0)),
    myGNEApp(GNEApp) {
    new FXSeparator(this);
    // create buttons centered
    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonsFrame, (TL("Run") + std::string("\t\t") + TL("close accepting changes")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_RUN, GUIDesignButtonAccept);
    new FXButton(buttonsFrame, (TL("Cancel") + std::string("\t\t") + TL("close discarding changes")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXButton(buttonsFrame, (TL("Reset") + std::string("\t\t") + TL("reset to previous values")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNERunToolDialog::~GNERunToolDialog() {}


GNEApplicationWindow*
GNERunToolDialog::getGNEApp() const {
    return myGNEApp;
}


void
GNERunToolDialog::runTool(GNETool* /*tool*/) {
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    myGNEApp->getApp()->runModalFor(this);
}


void
GNERunToolDialog::hide() {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    FXDialogBox::hide();
}


long
GNERunToolDialog::onCmdRun(FXObject*, FXSelector, void*) {
    // RUN

    // hide tool dialog
    hide();
    return 1;
}


long
GNERunToolDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // just hide tool dialog
    hide();
    return 1;
}


long
GNERunToolDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}


GNERunToolDialog::GNERunToolDialog() :
    myGNEApp(nullptr) {
}

/****************************************************************************/
