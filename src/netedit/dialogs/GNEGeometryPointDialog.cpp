/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEGeometryPointDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2021
///
// A dialog for set Geometry Points
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEGeometryPointDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGeometryPointDialog) GNEGeometryPointDialogMap[] = {
    FXMAPFUNC(SEL_KEYPRESS,     0,                      GNEGeometryPointDialog::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                      GNEGeometryPointDialog::onKeyRelease),
    FXMAPFUNC(SEL_CLOSE,        0,                      GNEGeometryPointDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_ACCEPT,  GNEGeometryPointDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_CANCEL,  GNEGeometryPointDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_RESET,   GNEGeometryPointDialog::onCmdReset),
};

// Object abstract implementation
FXIMPLEMENT_ABSTRACT(GNEGeometryPointDialog, FXTopWindow, GNEGeometryPointDialogMap, ARRAYNUMBER(GNEGeometryPointDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGeometryPointDialog::GNEGeometryPointDialog(GNEViewNet* viewNet, Position* pos) :
    FXTopWindow(viewNet, "Edit Geometry Point", GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), GUIDesignDialogBoxExplicit(100, 100)),
    myViewNet(viewNet),
    myPos(pos),
    myOriginalPos(*pos) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // Create frame for contents
    // myContentFrame = new FXVerticalFrame(mainFrame, GUIDesignContentsFrame);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose accepting changes",  GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose discarding changes", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\treset to previous values",  GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create
    create();
    // show in the given position
    show(PLACEMENT_SCREEN);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(this);
}


GNEGeometryPointDialog::~GNEGeometryPointDialog() {
    // return focus to GNEViewNet to avoid minimization
    getParent()->setFocus();
}


long 
GNEGeometryPointDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEGeometryPointDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEGeometryPointDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEGeometryPointDialog::onKeyPress(FXObject* sender, FXSelector sel, void* ptr) {
    return FXTopWindow::onKeyPress(sender, sel, ptr);
}


long
GNEGeometryPointDialog::onKeyRelease(FXObject* sender, FXSelector sel, void* ptr) {
    return FXTopWindow::onKeyRelease(sender, sel, ptr);
}

/****************************************************************************/
