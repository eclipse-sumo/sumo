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
/// @file    GNECursorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2022
///
// Dialog for edit element under cursor
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>

#include <netedit/GNEUndoList.h>

#include "GNECursorDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECursorDialog) GNECursorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,   GNECursorDialog::onCmdSelectElement),
};

// Object implementation
FXIMPLEMENT(GNECursorDialog, GUIGLObjectPopupMenu, GNECursorDialogMap, ARRAYNUMBER(GNECursorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECursorDialog::GNECursorDialog(GUIMainWindow* app, GUISUMOAbstractView* parent) :
    GUIGLObjectPopupMenu(app, parent) {

}


GNECursorDialog::~GNECursorDialog() {}


long 
GNECursorDialog::onCmdSelectElement(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
