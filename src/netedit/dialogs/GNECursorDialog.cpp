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
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/windows/GUIMainWindow.h>

#include <netedit/GNEUndoList.h>

#include "GNECursorDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECursorDialog) GNECursorDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SETFRONTELEMENT,    GNECursorDialog::onCmdSetFrontElement)
};

// Object implementation
FXIMPLEMENT(GNECursorDialog, GUIGLObjectPopupMenu, GNECursorDialogMap, ARRAYNUMBER(GNECursorDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNECursorDialog::GNECursorDialog(GNEViewNet* viewNet, const std::vector<GNEAttributeCarrier*>& ACs) :
    GUIGLObjectPopupMenu(viewNet->getViewParent()->getGNEAppWindows(), viewNet),
    myViewNet(viewNet) {
    // create header
    new MFXMenuHeader(this, viewNet->getViewParent()->getGNEAppWindows()->getBoldFont(), "Mark front element", GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), nullptr, 0);
    new FXMenuSeparator(this);
    // create a menu command for every AC
    for (const auto &AC : ACs) {
        myMoveDialogElementContainer[GUIDesigns::buildFXMenuCommand(this, AC->getID(), AC->getIcon(), this, MID_GNE_SETFRONTELEMENT)] = AC;
    }

}


GNECursorDialog::~GNECursorDialog() {}


long
GNECursorDialog::onCmdSetFrontElement(FXObject* obj, FXSelector, void*) {
    // set front attribute AC
    myViewNet->setFrontAttributeCarrier(myMoveDialogElementContainer.at(obj));
    // destroy popup
    myViewNet->destroyPopup();
    return 1;
}

/****************************************************************************/
