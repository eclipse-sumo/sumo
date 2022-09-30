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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEUndoListDialog::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEUndoListDialog::onCmdSelectRow),
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
    auto mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    myRowFrame = new FXVerticalFrame(mainFrame, GUIDesignAuxiliarFrame);
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
    // close dialog
    hide();
    return 1;
}


long
GNEUndoListDialog::onCmdSelectRow(FXObject* obj, FXSelector, void*) {
    int index = -1;
    // search button
    for (int i = 0; i < (int)myRows.size(); i++) {
        if (myRows.at(i)->getRadioButton() == obj) {
            index = i;
            myRows.at(i)->enableRow();
        } else {
            if (index == -1) {
                myRows.at(i)->setBlueBackground();
            } else {
                myRows.at(i)->setRedBackground();
            }
        }
    }
    return 1;
}


void
GNEUndoListDialog::updateList() {
    // declare row for save current undo element
    Row* currentUndoElement = nullptr;
    // first clear rows
    for (auto &row : myRows) {
        delete row;
    }
    myRows.clear();
    // declare redo iterator over UndoList
    GNEUndoList::RedoIterator itRedo(myGNEApp->getUndoList());
    // fill rows with elements to redo
    while (!itRedo.end()) {
        // create red row and insert
        auto row = new Row(this, myRowFrame, itRedo.getIcon(), itRedo.getDescription());
        row->setBlueBackground();
        myRows.push_back(row);
        itRedo++;
    }
    // declare undo iterator over UndoList
    GNEUndoList::UndoIterator itUndo(myGNEApp->getUndoList());
    // fill rows with elements to undo
    while (!itUndo.end()) {
        // create row and insert it
        auto row = new Row(this, myRowFrame, itUndo.getIcon(), itUndo.getDescription());
        row->setRedBackground();
        myRows.push_back(row);
        // mark element
        if (currentUndoElement == nullptr) {
            currentUndoElement = myRows.back();
        }
        itUndo++;
    }
    if (currentUndoElement) {
        currentUndoElement->enableRow();
    }
    myRowFrame->recalc();
}


GNEUndoListDialog::Row::Row(GNEUndoListDialog* undoListDialog, FXVerticalFrame* mainFrame, FXIcon* icon, const std::string& text) {
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    // build radio button
    myRadioButton = new FXRadioButton(horizontalFrame, "", undoListDialog, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonSquared);
    // build icon label
    myIcon = new FXLabel(horizontalFrame, "", icon, GUIDesignLabelIconThick);
    // build text label
    myTextField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, undoListDialog, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextField->setText(text.c_str());
    myTextField->setEditable(false);
    // create elements
    horizontalFrame->create();
    myIcon->create();
    myTextField->create();
}


GNEUndoListDialog::Row::~Row() {
    delete myRadioButton;
    delete myIcon;
    delete myTextField;
}


const FXRadioButton*
GNEUndoListDialog::Row::getRadioButton() const {
    return myRadioButton;
}


void
GNEUndoListDialog::Row::setRedBackground() {
    myRadioButton->setCheck(FALSE);
    myRadioButton->setBackColor(FXRGB(255, 0, 0));
}


void
GNEUndoListDialog::Row::setBlueBackground() {
    myRadioButton->setCheck(FALSE);
    myRadioButton->setBackColor(FXRGB(0, 0, 255));
}


void
GNEUndoListDialog::Row::enableRow() {
    myRadioButton->setCheck(TRUE);
    myRadioButton->setBackColor(FXRGB(0, 255, 0));
}

