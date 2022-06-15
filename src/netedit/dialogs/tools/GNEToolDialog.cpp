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
/// @file    GNEToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Abstract dialog for tools
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/GNENet.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEToolDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEToolDialog) GNEToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEToolDialog::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   0,                      GNEToolDialog::onCmdUpdate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEToolDialog::onCmdClose),
};

// Object implementation
FXIMPLEMENT(GNEToolDialog, FXTopWindow, GNEToolDialogMap, ARRAYNUMBER(GNEToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEToolDialog::GNEToolDialog(GNEApplicationWindow* GNEApp, const std::string &name, const int dialogWidth, const int dialogHeight) :
    FXTopWindow(GNEApp->getApp(), name.c_str(), GUIIconSubSys::getIcon(GUIIcon::EMPTY), GUIIconSubSys::getIcon(GUIIcon::EMPTY), GUIDesignDialogBoxExplicit(dialogWidth, dialogHeight)),
    myGNEApp(GNEApp) {
    // build horizontalFrame for content
    myContentFrame = new FXVerticalFrame(this, GUIDesignVerticalFrame);
}


GNEToolDialog::~GNEToolDialog() {}


void
GNEToolDialog::openToolDialog() {
    // show dialog
    show(PLACEMENT_SCREEN);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    myGNEApp->getApp()->runModalFor(this);
    
}


void
GNEToolDialog::hideToolDialog() {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    hide();
}


bool
GNEToolDialog::shown() const {
    return FXWindow::shown();
}


void
GNEToolDialog::addArgument(const GNEToolDialogElements::Argument *argument) {
    myArguments.push_back(argument);
}


long
GNEToolDialog::onCmdClose(FXObject*, FXSelector, void*) {
    // close dialog
    hide();
    return 1;
}


long 
GNEToolDialog::onCmdUpdate(FXObject*, FXSelector, void*) {
    return 0;
}


FXint
GNEToolDialog::openAsModalDialog(FXuint placement) {
    // create Dialog
    create();
    // show in the given position
    show(placement);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    return getApp()->runModalFor(this);
}

/****************************************************************************/
