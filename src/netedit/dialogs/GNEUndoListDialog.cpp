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
GNEUndoListDialog::show() {
    // recalc list
    recalcList();
    // show
    FXTopWindow::show(PLACEMENT_SCREEN);
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
    int index =0;
    // search button
    for (const auto &row : myRows) {
        if (row->getRadioButton() == obj) {
            index = row->getIndex();
        }
    }
    // now apply undo-redos
    if (index < 0) {
        for (int i = 0; i < (index * -1); i++) {
            myGNEApp->getUndoList()->undo();
        }
    } else {
        for (int i = 0; i < index; i++) {
            myGNEApp->getUndoList()->redo();
        }
    }
    // update list again
    updateList();
    return 1;
}


void
GNEUndoListDialog::updateList() {
    // declare vector of temporal rows
    std::vector<TemporalRow> temporalRows;
    // declare redo iterator over U/ndoList
    GNEUndoList::RedoIterator itRedo(myGNEApp->getUndoList());
    // declare index
    int index = 1;
    // fill temporal rows rows with elements to redo (in inverse)
    while (!itRedo.end()) {
        TemporalRow temporalRow;
        temporalRow.index = index;
        temporalRow.icon = itRedo.getIcon();
        temporalRow.text = itRedo.getDescription();
        temporalRows.push_back(temporalRow);
        // update counters
        itRedo++;
        index++;
    }
    // reverse temporal rows (redo)
    std::reverse(temporalRows.begin(), temporalRows.end());
    // declare undo iterator over UndoList
    GNEUndoList::UndoIterator itUndo(myGNEApp->getUndoList());
    // reset index
    index = 0;
    // fill rows with elements to undo
    while (!itUndo.end()) {
        TemporalRow temporalRow;
        temporalRow.index = index;
        temporalRow.icon = itUndo.getIcon();
        temporalRow.text = itUndo.getDescription();
        temporalRows.push_back(temporalRow);
        // update counters
        itUndo++;
        index--;
    }
    // fill rows
    for (int i = 0; i < (int)temporalRows.size(); i++) {
        myRows.at(i)->update(temporalRows.at(i).index, temporalRows.at(i).icon, temporalRows.at(i).text);
        if (temporalRows.at(i).index < 0) {
            myRows.at(i)->setBlueBackground();
        } else if (temporalRows.at(i).index > 0) {
            myRows.at(i)->setRedBackground();
        } else {
            myRows.at(i)->checkRow();
        }
    }
}


void
GNEUndoListDialog::recalcList() {
    // first clear rows
    for (auto &row : myRows) {
        delete row;
    }
    myRows.clear();
    // declare redo iterator over undoList and fill rows
    GNEUndoList::RedoIterator itRedo(myGNEApp->getUndoList());
    while (!itRedo.end()) {
        myRows.push_back(new Row(this, myRowFrame));
        itRedo++;
    }
    // declare undo iterator over undoList and fill rows
    GNEUndoList::UndoIterator itUndo(myGNEApp->getUndoList());
    while (!itUndo.end()) {
        myRows.push_back(new Row(this, myRowFrame));
        itUndo++;
    }
    // recalc frame and update list
    myRowFrame->recalc();
    updateList();
}


GNEUndoListDialog::Row::Row(GNEUndoListDialog* undoListDialog, FXVerticalFrame* mainFrame) {
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarHorizontalFrame);
    // build radio button
    myRadioButton = new FXRadioButton(horizontalFrame, "", undoListDialog, MID_CHOOSEN_OPERATION, GUIDesignRadioButtonSquared);
    // build icon label
    myIcon = new FXLabel(horizontalFrame, "", nullptr, GUIDesignLabelIconThick);
    // build text label
    myTextField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, undoListDialog, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
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


void 
GNEUndoListDialog::Row::update(int index, FXIcon* rowIcon, const std::string& text) {
    myIndex = index;
    myIcon->setIcon(rowIcon);
    myTextField->setText(text.c_str());
}


int
GNEUndoListDialog::Row::getIndex() const {
    return myIndex;
}


const FXRadioButton*
GNEUndoListDialog::Row::getRadioButton() const {
    return myRadioButton;
}


void
GNEUndoListDialog::Row::setRedBackground() {
    myRadioButton->setCheck(FALSE);
    myRadioButton->setBackColor(FXRGBA(255, 213, 213, 255));
}


void
GNEUndoListDialog::Row::setBlueBackground() {
    myRadioButton->setCheck(FALSE);
    myRadioButton->setBackColor(FXRGBA(210, 233, 255, 255));
}


void
GNEUndoListDialog::Row::checkRow() {
    myRadioButton->setCheck(TRUE);
    myRadioButton->setBackColor(FXRGBA(240, 255, 205, 255));
}

