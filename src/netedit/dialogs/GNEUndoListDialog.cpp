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
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEUndoListDialog::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   0,                      GNEUndoListDialog::onCmdUpdate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEUndoListDialog::onCmdClose),
};

// Object implementation
FXIMPLEMENT(GNEUndoListDialog, FXTopWindow, GNEUndoListDialogMap, ARRAYNUMBER(GNEUndoListDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEUndoListDialog::GNEUndoListDialog(GNEApplicationWindow* GNEApp) :
    FXTopWindow(GNEApp->getApp(), "Undo/Redo history", GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), GUIDesignDialogBoxExplicit(500, 400)),
    myGNEApp(GNEApp) {
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create treelist dynamic
    myTreeListDynamic = new MFXTreeListDynamic(mainFrame, this, MID_GNE_UNDOLIST_UPDATE, GUIDesignTreeListDinamicExpandHeight);
    // create buttons centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonsFrame, "OK\tclose dialog", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEUndoListDialog::~GNEUndoListDialog() {}


void
GNEUndoListDialog::open() {
    // update table
    updateList();
    // reset last undo element
    myLastUndoElement = -1;

    // show
    show(PLACEMENT_SCREEN);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    myGNEApp->getApp()->runModalFor(this);

}


void
GNEUndoListDialog::hide() {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
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
    // reset selected elements
    myTreeListDynamic->resetSelectedItem();
    // close dialog
    hide();
    return 1;
}


long
GNEUndoListDialog::onCmdUpdate(FXObject*, FXSelector, void*) {
    // first check if shown
    if (shown() && (myLastUndoElement != myTreeListDynamic->getSelectedIndex())) {
        // set colors
        for (int i = 0; i < myTreeListDynamic->getSelectedIndex(); i++) {
            myTreeListDynamic->getItem(i)->setTextColor(FXRGB(255, 0, 0));
        }
        for (int i = myTreeListDynamic->getSelectedIndex(); i < myTreeListDynamic->getNumItems(); i++) {
            myTreeListDynamic->getItem(i)->setTextColor(FXRGB(0, 0, 0));
        }
        myTreeListDynamic->update();
        // undo/redo
        for (int i = myLastUndoElement; i < myTreeListDynamic->getSelectedIndex(); i++) {
            myGNEApp->getUndoList()->undo();
        }
        for (int i = myLastUndoElement; i >= myTreeListDynamic->getSelectedIndex(); i--) {
            myGNEApp->getUndoList()->redo();
        }
        myLastUndoElement = myTreeListDynamic->getSelectedIndex();
    }
    return 0;
}


void
GNEUndoListDialog::updateList() {
    // first clear myTreeListDynamic
    myTreeListDynamic->clearItems();
    // declare undo iterator over UndoList
    GNEUndoList::UndoIterator itUndo(myGNEApp->getUndoList());
    FXTreeItem* firstItem = nullptr;
    // fill myTreeListDynamic
    while (!itUndo.end()) {
        if (firstItem == nullptr) {
            firstItem = myTreeListDynamic->appendItem(nullptr, itUndo.getDescription().c_str(), itUndo.getIcon());
        } else {
            myTreeListDynamic->appendItem(nullptr, itUndo.getDescription().c_str(), itUndo.getIcon());
        }
        itUndo++;
    }
    // declare redo iterator over UndoList
    GNEUndoList::RedoIterator itRedo(myGNEApp->getUndoList());
    // fill myTreeListDynamic
    while (!itRedo.end()) {
        if (firstItem == nullptr) {
            firstItem = myTreeListDynamic->prependItem(nullptr, itRedo.getDescription().c_str(), itRedo.getIcon(), FXRGB(255, 0, 0));
        } else {
            myTreeListDynamic->prependItem(nullptr, itRedo.getDescription().c_str(), itRedo.getIcon(), FXRGB(255, 0, 0));
        }
        itRedo++;
    }
    if (firstItem) {
        firstItem->setSelected(true);
    }
}

/****************************************************************************/
