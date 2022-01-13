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
/// @file    GNEUndoListDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// Dialog for show undo-list
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

#include "GNEUndoListDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEUndoListDialog) GNEUndoListDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,        0,                                      GNEUndoListDialog::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_BUTTON_ACCEPT,                  GNEUndoListDialog::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,      MID_GNE_REROUTEDIALOG_ADD_INTERVAL,     GNEUndoListDialog::onCmdSelectElement),
};

// Object implementation
FXIMPLEMENT(GNEUndoListDialog, FXTopWindow, GNEUndoListDialogMap, ARRAYNUMBER(GNEUndoListDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEUndoListDialog::GNEUndoListDialog(GNEApplicationWindow* GNEApp) :
    FXTopWindow(GNEApp->getApp(), "Undo/Redo history", GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), GUIDesignDialogBoxExplicit(300, 400)),
    myGNEApp(GNEApp) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create treelist dinamic
    myTreeListDinamic = new FXTreeListDinamic(mainFrame, this, MID_GNE_UNDOLIST_UPDATE, GUIDesignTreeListDinamicExpandHeight);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonsFrame, "OK\tclose dialog",  GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEUndoListDialog::~GNEUndoListDialog() {}


void
GNEUndoListDialog::open() {
    // update table
    updateList();
    // show
    show(PLACEMENT_SCREEN);
}


void
GNEUndoListDialog::hide() {
    FXTopWindow::hide();
}


bool
GNEUndoListDialog::shown() const {
    return FXWindow::shown();
}


void
GNEUndoListDialog::setFocus() {
    FXWindow::setFocus();
}


long
GNEUndoListDialog::onCmdClose(FXObject*, FXSelector, void*) {
    // just close dialog
    hide();
    return 1;
}


long
GNEUndoListDialog::onCmdSelectElement(FXObject*, FXSelector, void*) {
    // currently unused
    return 1;
}


void
GNEUndoListDialog::updateList() {
    // first clear myTreeListDinamic
    myTreeListDinamic->clearItems();
    // declare iterator over UndoList
    GNEUndoList::Iterator it(myGNEApp->getUndoList());
    // fill myTreeListDinamic
    while (!it.end()) {
        myTreeListDinamic->insertItem(nullptr, nullptr, it.getDescription().c_str(), it.getIcon(), it.getIcon());
        it++;
    }
}

/****************************************************************************/
